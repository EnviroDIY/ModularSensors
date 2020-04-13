/*
 *DigiXBeeCellularTransparent.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi Cellular XBee's
*/

// Included Dependencies
#include "DigiXBeeCellularTransparent.h"
#include "LoggerModemMacros.h"

// Constructor/Destructor
DigiXBeeCellularTransparent::DigiXBeeCellularTransparent(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *apn)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin),
    #ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger, modemResetPin),
    #else
    gsmModem(*modemStream, modemResetPin),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}

// Destructor
DigiXBeeCellularTransparent::~DigiXBeeCellularTransparent() {}

MS_MODEM_WAKE(DigiXBeeCellularTransparent);

MS_MODEM_CONNECT_INTERNET(DigiXBeeCellularTransparent);
MS_MODEM_DISCONNECT_INTERNET(DigiXBeeCellularTransparent);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeCellularTransparent);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(DigiXBeeCellularTransparent);
MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeCellularTransparent);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeCellularTransparent);

// We turn off airplane mode in the wake.
bool DigiXBeeCellularTransparent::modemWakeFxn(void)
{
    if (_modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        MS_DBG(F("Setting pin"), _modemSleepRqPin, _wakeLevel ? F("HIGH") : F("LOW"),
               F("to wake"), _modemName);
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        MS_DBG(F("Turning off airplane mode..."));
        if (gsmModem.commandMode())
        {
            gsmModem.sendAT(GF("AM"),0);
            gsmModem.waitResponse();
            // Write changes to flash and apply them
            gsmModem.writeChanges();
            // Exit command mode
            gsmModem.exitCommand();
        }
        return true;
    }
    else
    {
        return true;
    }
}


// We turn on airplane mode in before sleep
bool DigiXBeeCellularTransparent::modemSleepFxn(void)
{
    if (_modemSleepRqPin >= 0)
    {
        MS_DBG(F("Turning on airplane mode..."));
        if (gsmModem.commandMode())
        {
            gsmModem.sendAT(GF("AM"),0);
            gsmModem.waitResponse();
            // Write changes to flash and apply them
            gsmModem.writeChanges();
            // Exit command mode
            gsmModem.exitCommand();
        }
        MS_DBG(F("Setting pin"), _modemSleepRqPin, !_wakeLevel ? F("HIGH") : F("LOW"),
               F("to put"), _modemName, F("to sleep"));
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        return true;
    }
    else
    {
        return true;
    }
}


bool DigiXBeeCellularTransparent::extraModemSetup(void)
{
    bool success = true;
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    if (gsmModem.commandMode())
    {
        gsmModem.getSeries();
        _modemName = gsmModem.getModemName();
        MS_DBG(F("'"),_modemName,F("' in command mode. Setting I/O Pins..."));
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(GF("D8"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        gsmModem.sendAT(GF("D7"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on the associate LED (if you're using a board with one)
        // NOTE:  Only pin 15/DIO5 can be used for this function
        gsmModem.sendAT(GF("D5"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on the RSSI indicator LED (if you're using a board with one)
        // NOTE:  Only pin 6/DIO10/PWM0 can be used for this function
        gsmModem.sendAT(GF("P0"),1);
        success &= gsmModem.waitResponse() == 1;
        // Put the XBee in pin sleep mode
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"),1);
        success &= gsmModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        gsmModem.sendAT(GF("SO"),0);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Other Options..."));
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        gsmModem.sendAT(GF("DO"),0);
        success &= gsmModem.waitResponse() == 1;
        // Ask data to be "packetized" and sent out with every new line (0x0A
        gsmModem.sendAT(GF("TD0A"));
        success &= gsmModem.waitResponse() == 1;
        // Make sure pins 7&8 are not set for USB direct on XBee3 units
        gsmModem.sendAT(GF("P1"),0);
        success &= gsmModem.waitResponse() == 1;
        // Set the socket timeout to 10s
        gsmModem.sendAT(GF("TM"),64);
        success &= gsmModem.waitResponse() == 1;
        // MS_DBG(F("Setting Cellular Carrier Options..."));
        // // Carrier Profile - 1 = No profile/SIM ICCID selected
        // gsmModem.sendAT(GF("CP"),0);
        // gsmModem.waitResponse();  // Don't check for success - only works on LTE
        // // Cellular network technology - LTE-M/NB IoT
        // gsmModem.sendAT(GF("N#"),0);
        // gsmModem.waitResponse();  // Don't check for success - only works on LTE
        // Put the network connection parameters into flash
        success &= gsmModem.gprsConnect(_apn);
        MS_DBG(F("Ensuring XBee is in transparent mode..."));
        // Make sure we're really in transparent mode
        gsmModem.sendAT(GF("AP0"));
        success &= gsmModem.waitResponse() == 1;
        // Write changes to flash and apply them
        MS_DBG(F("Applying changes..."));
        gsmModem.writeChanges();

        String ui_vers = gsmModem.getIMEI();
        PRINTOUT(F("IMa "), ui_vers);
        //ui_vers = gsmModem.getIMEI();
        //PRINTOUT(F("IMb "), ui_vers);
        //ui_vers = gsmModem.getRegistrationStatus();
        //gsmModem.sendAT(GF("+CREG"));
        //ui_vers=gsmModem.readResponseInt(10000L);
        //PRINTOUT(F("Registration '"), ui_vers,"'");
        #if defined MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
        ui_vers = gsmModem.sendATGetString(GF("VR"));
        //ui_vers += " "+gsmModem.sendATGetString(F("VL"));
        MS_DBG(F("Version "), ui_vers);
        #endif
        uint16_t loops=0;
        int16_t ui_db;
        uint8_t status;
        String ui_op;
        bool cellRegistered=false;
        PRINTOUT(F("Loop=Sec] rx db : Status ' Operator ' #Polled Cell Status every 1sec"));
        uint8_t reg_count =1;
        for ( unsigned long start = millis(); millis() - start < 300000; ++loops) {
            ui_db = 0;// gsmModem.getSignalQuality();
            gsmModem.sendAT(GF("AI"));
            status=gsmModem.readResponseInt(10000L);
            ui_op = String(loops)+"="+String((float)millis()/1000)+"] "+String(ui_db)+":0x"+String(status,HEX)+" '"+ gsmModem.getOperator()+"'";
            if ((0==status) ||(0x23 ==status)) {
                ui_op += " Cnt="+String(reg_count);
                PRINTOUT(ui_op);
                if (++reg_count > 3) {
                    cellRegistered=true;
                    break;
                }
            } else {
                reg_count=1;
                //String ui_scan = gsmModem.sendATGetString(GF("AS")); //Scan
                //ui_op += " Cell Scan "+ui_scan;
                PRINTOUT(ui_op);
                if (100 == loops) {
                    /*Not clear why this may force a registration
                    Early experience with Hologram SIMs was they aren't registering,
                    However throwing this in, might do something or maybe just coincedence that it started working after this
                    */ 
                    gsmModem.sendAT(GF("+CREG"));
                    //String ui_creg=gsmModem.readResponseInt(10000L);
                    //PRINTOUT(F("UseRandom +CREG '"), ui_creg,"'");
                    PRINTOUT(F("UseRandom +CREG '"));
                }
            }
            delay(1000);
        }
        if (cellRegistered) {
            String ui_scan = gsmModem.sendATGetString(GF("AS")); //Scan
            PRINTOUT(F("Cell scan '"),ui_scan,"' success",success);
            success = true; //Not sure why need to force this
        } else {success = false;}
 
        //uint32_t time_epoch_sec=getNISTTime();//getTimeCellTower();
        //Logger::setRTClock(time_epoch_sec); ////#include "LoggerBase.h"
        //PRINTOUT(F("Startup Time & thrownaway "),time_epoch_sec,"sec" ); /**/
 
        // Exit command mode
        gsmModem.exitCommand();
    }
    else
    {
        success = false;
    }

    if (success)
    {
        MS_DBG(F("... setup successful!"));
    }
    else
    {
        MS_DBG(F("... setup failed!"));
    }
    return success;
}


// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
/*uint32_t DigiXBeeCellularTransparent::getNISTTime(void)
{
    // bail if not connected to the internet
    gsmModem.commandMode();
    if (!gsmModem.isNetworkConnected())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        gsmModem.exitCommand();
        return 0;
    }

    // We can get the NIST timestamp directly from the XBee
    gsmModem.sendAT(GF("DT0"));
    String res = gsmModem.readResponseString();
    gsmModem.exitCommand();
    MS_DBG(F("Raw hex response from XBee:"), res);
    char buf[9] = {0,};
    res.toCharArray(buf, 9);
    uint32_t secFrom2000 = strtol(buf, 0, 16);
    MS_DBG(F("Seconds from Jan 1, 2000 from XBee (UTC):"), secFrom2000);

    // Convert from seconds since Jan 1, 2000 to 1970
    uint32_t unixTimeStamp = secFrom2000 + 946684800 ;
    MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);

    // If before Jan 1, 2019 or after Jan 1, 2030, most likely an error
    if (unixTimeStamp < 1546300800)
    {
        return 0;
    }
    else if (unixTimeStamp > 1893456000)
    {
        return 0;
    }
    else
    {
        return unixTimeStamp;
    }
}*/
uint32_t DigiXBeeCellularTransparent::getNISTTimeOrig(void)
{
    /* bail if not connected to the internet */
    if (!isInternetAvailable())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    /* Try up to 12 times to get a timestamp from NIST */
    for (uint8_t i = 0; i < 12; i++)
    {

        /* Must ensure that we do not ping the daylight more than once every 4 seconds */
        /* NIST clearly specifies here that this is a requirement for all software */
        /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
        while (millis() < _lastNISTrequest + 4000)
        {
        }

        /* Make TCP connection */
        MS_DBG(F("\nConnecting to NIST daytime Server"));
        bool connectionMade = false;

        /* This is the IP address of time-e-wwv.nist.gov  */
        /* XBee's address lookup falters on time.nist.gov */
        IPAddress ip(132, 163, 97, 1);
        connectionMade = gsmClient.connect(ip, 37, 15);
        /* Wait again so NIST doesn't refuse us! */
        delay(4000L);
        /* Try sending something to ensure connection */
        gsmClient.println('!');

        /* Wait up to 5 seconds for a response */
        if (connectionMade)
        {
            uint32_t start = millis();
            while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L)
            {
            }

            if (gsmClient.available() >= 4)
            {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte response[4] = {0};
                gsmClient.read(response, 4);
                gsmClient.stop();
                return parseNISTBytes(response);
            }
            else
            {
                MS_DBG(F("NIST Time server did not respond!"));
                gsmClient.stop();
            }
        }
        else
        {
            MS_DBG(F("Unable to open TCP to NIST!"));
        }
    }
    return 0;
}


bool DigiXBeeCellularTransparent::updateModemMetadata(void)
{
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI = SENSOR_DEFAULT;
    loggerModem::_priorSignalPercent =SENSOR_DEFAULT;
    //loggerModem::_priorBatteryState = SENSOR_DEFAULT;
    //loggerModem::_priorBatteryPercent = SENSOR_DEFAULT;
    //loggerModem::_priorBatteryPercent = SENSOR_DEFAULT;
    loggerModem::_priorModemTemp = SENSOR_DEFAULT;

    // Initialize variable
    int16_t signalQual = SENSOR_DEFAULT;

    //if not enabled don't collect data
    if (!loggerModem::_pollModemMetaData) return false;

    // Enter command mode only once
    MS_DBG(F("Entering Command Mode:"));
    gsmModem.commandMode();

    // Try for up to 15 seconds to get a valid signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    uint32_t startMillis = millis();
    do
    {
        MS_DBG(F("Getting signal quality:"));
        signalQual = gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);
        if (signalQual != 0 && signalQual != -9999)
            break;
        delay(250);
    } while ((signalQual == 0 || signalQual == -9999) &&
             millis() - startMillis < 15000L && success);

    // Convert signal quality to RSSI
    loggerModem::_priorRSSI = signalQual;
    MS_DBG(F("CURRENT RSSI:"), signalQual);
    loggerModem::_priorSignalPercent = getPctFromRSSI(signalQual);
    MS_DBG(F("CURRENT Percent signal strength:"), getPctFromRSSI(signalQual));

    MS_DBG(F("Getting chip temperature:"));
    loggerModem::_priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem temperature:"), loggerModem::_priorModemTemp);

    // Exit command modem
    MS_DBG(F("Leaving Command Mode:"));
    gsmModem.exitCommand();

    return success;
}

//******** Nh Experimental ***************************

uint32_t DigiXBeeCellularTransparent::getNISTTime(void)
{
    uint32_t time_epochTz0;
    time_epochTz0 = getNISTTimeOrig();
    if (0 == time_epochTz0) {
        time_epochTz0= (getTimeCellTower()+(8*3600));
    }
    return time_epochTz0;
}

uint32_t DigiXBeeCellularTransparent::getTimeCellTower(void)
{
    uint32_t timeTzEpoch_sec = 0 ; //base 1970 Jan 1
    // bail if not connected to the internet
    if (!gsmModem.commandMode())
    //if (!gsmModem.isNetworkConnected())
    {
        MS_DBG(F("getTimeCellTower: No Cell LTE connection. "));
        gsmModem.exitCommand();
        return timeTzEpoch_sec;
    }/**/
    // We can get a timestamp directly from the XB3 - ATT gives local time
    String res; 
    uint8_t rxCellTime=0;  //0 no Time, 1 ASCII ISO8601 time, 2 32bit Time since 2000-01-01 00:00:00
    uint16_t rxCell_cnt=10;
    #if 0 
    // Check for ISO8601 time - ATT returned ' 2019-09-25T17:06:32 '
    do {
        MS_DBG(F("CellTowerTAReq"),rxCell_cnt);
        gsmModem.sendAT(GF("DT1"));
        res = gsmModem.readResponseString(10000);
        if (res == "" || res == " OK ") 
        {
            res = gsmModem.readResponseString(10000);
            if (!(res == "" || res == " OK ")) {
                rxCellTime=1;
                 MS_DBG(F("CellTARsp2 '"),res,"'");
            } 
        } else {
            rxCellTime=1;
            MS_DBG(F("CellTARsp1 '"),res,"'");
        }
    } while ((0==rxCellTime) && --rxCell_cnt);
    #endif //0

    if (0==rxCellTime) 
    {
        //Check for U32int time ATT returns res=' 24FD8365 '
        rxCell_cnt=10;
        do {
            MS_DBG(F("CellTowerTElReq"),rxCell_cnt);
            gsmModem.sendAT(GF("DT0"));
            res = gsmModem.readResponseString(10000);
            if (res == "" || res == " OK ") 
            {
                res = gsmModem.readResponseString(10000);
                if (!(res == "" || res == " OK ")) {
                    rxCellTime=2;
                    MS_DBG(F("CellTERsp2 '"),res,"'");
                } 
            } else {
                rxCellTime=2;
                MS_DBG(F("CellTERsp1 '"),res,"'");
            }
        } while ((2!=rxCellTime) && --rxCell_cnt);
    }

    char buf[10] = {0,};
    uint32_t secFrom2000=0;
    switch(rxCellTime) {
        case 1: 
            MS_DBG(F("CellTower Time:"), res);
        break;
        
        case 2:
            res.toCharArray(buf, 9);
            secFrom2000 = strtol(buf, 0, 16);
            MS_DBG(F("Seconds from Jan 1, 2000 from CellTower (PST):"), secFrom2000);
            timeTzEpoch_sec = secFrom2000 + 946684800 ;
            MS_DBG(F("Epoch Timestamp returned UTC:"),timeTzEpoch_sec);
            break;
        default: break;
    }
    // Check sanity of time
    if (0 != secFrom2000) 
    {
        // If before Jan 1, 2019 or after Jan 1, 2030, most likely an error
        if (timeTzEpoch_sec < 1546300800) {timeTzEpoch_sec= 0;}
        else if (timeTzEpoch_sec > 1893456000) {timeTzEpoch_sec=0;}
    }
    return timeTzEpoch_sec;
}

#if 0
uint32_t DigiXBeeCellularTransparent::getTimeNIST(void) //nh getNISTTime(void)
{
    /* bail if not connected to the internet */
    if (!isInternetAvailable())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    /* Must ensure that we do not ping the daylight more than once every 4 seconds */
    /* NIST clearly specifies here that this is a requirement for all software */
    /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
    while (millis() < _lastNISTrequest + 4000) {}
    //_lastNISTrequest =millis();

    /* Make TCP connection */
    MS_DBG(F("\nConnecting to NIST daytime Server"));
    bool connectionMade = false;

    /* This is the IP address of time-c-g.nist.gov */
    /* XBee's address lookup falters on time.nist.gov */
    //FUT: There are about 30 servers, so could try lookup and caching 
const char *timeNistHost = "time.nist.gov";
const int timeNistPort = 37; 
    IPAddress IP_MA1(129,  6, 15, 30);
    IPAddress IP_CO1(132,163, 96,  3);
    #define NIST_IP IP_CO1
    #define NIST_CONNECTION_TIMER 15

    gsmModem.sendAT(GF("TD0A")); //expect    "TD0A"
    gsmModem.waitResponse(); 
    connectionMade = gsmClient.connect(timeNistHost, timeNistPort,NIST_CONNECTION_TIMER);
    if (!connectionMade) {
        
        connectionMade = gsmClient.connect(NIST_IP, timeNistPort, NIST_CONNECTION_TIMER);
        MS_DBG(F("NIST.TIME.GOV lookup failed, tried "),NIST_IP,F(" and connect="),connectionMade );
    }
    
    /* Wait up to 5 seconds for a response */
    #define NIST_RSP_TIMER 10
    if (connectionMade)
    {
        //poll for IP connection
        delay(4000L);
        /* Need to send something before connection is made */
        //gsmClient.println("TryToRquestTime");
        //gsmClient.println("Try2ndTime");
        gsmClient.println('!');
        uint32_t start = millis();
        while (gsmClient && (gsmClient.available() <= NIST_RSP_TIMER) && ((millis() - start) < NIST_RSP_TIMER*1000L) ){}

        /* Must ensure that we do not ping the daylight more than once every 4 seconds */
        /* NIST clearly specifies here that this is a requirement for all software */
        /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
        while (millis() < _lastNISTrequest + 4000)
        {
        }


        if (gsmClient.available() >= NIST_RSP_TIMER)
        {
            MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
            byte response[4] = {0};
            gsmClient.read(response, 4);
            return parseNISTBytes(response);
        }
        else
        {
            MS_DBG(F("NIST Time server did not respond in "),NIST_RSP_TIMER,F("secs"));
            return 0;
        }
    }
    else
    {
        MS_DBG(F("Unable to open TCP to NIST!"));
    }
    return 0;
}
#endif //0
#if 0
// Get the time from NIST via NTP protocol. Can't be used behind a firewall.
uint32_t DigiXBeeCellularTransparent::getTimeNTP(void)
{
    bool success = true;
    uint32_t _currentEpoc=0;
    /* bail if not connected to WIFI SSID */
    if (!isInternetAvailable())
    {
        MS_DBG(F("No WiFi connection, cannot connect to NTP."));
        return 0;
    }

    /* Must ensure that we do not ping the daylight more than once every 4 seconds */
    /* NIST clearly specifies here that this is a requirement for all software */
    /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
    //while (millis() < _lastNISTrequest + 4000) {}

    /* Make TCP connection */
    delay(2000); //Allow buffer to clear - or should it be flush
    MS_DBG(F("\nConnecting to NTP/UDP Server"));
    bool connectionMade = false;

    gsmModem.sendAT(GF("IP"), 0);  // Put in UDP mode
    success &= (1 == gsmModem.waitResponse());
    //https://tf.nist.gov/tf-cgi/servers.cgi
    //use 192.241.211.46  
    String host("192.241.211.46"); //from ping pool.ntp.org

    //host.reserve(16);

    gsmModem.sendAT(GF("DL"), host);  // Set the "Destination Address Low"
    success &= (1 == gsmModem.waitResponse());
    //#define NTP_DEFAULT_LOCAL_PORT 1337
    #define NTP_REQUESTS_PORT 123
    gsmModem.sendAT(GF("DE"), String(NTP_REQUESTS_PORT, HEX));  // Set the destination port
    success &= (1 == gsmModem.waitResponse());

    //delay(4000L);
    /* Need to send something before connection is made */
    //gsmClient.println('!');

  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
    {
        byte  _packetBuffer[NTP_PACKET_SIZE+10];
        memset(_packetBuffer, 0, NTP_PACKET_SIZE+10);
        _packetBuffer[0] = 0b11100011;   // LI, Version, Mode
        _packetBuffer[1] = 0;     // Stratum, or type of clock
        _packetBuffer[2] = 6;     // Polling Interval
        _packetBuffer[3] = 0xEC;  // Peer Clock Precision
        // 8 bytes of zero for Root Delay & Root Dispersion
        _packetBuffer[12]  = 49;
        _packetBuffer[13]  = 0x4E;
        _packetBuffer[14]  = 49;
        _packetBuffer[15]  = 52;
        gsmClient.write(_packetBuffer,NTP_PACKET_SIZE);
    }
    /* Wait up to 5 seconds for a response */
    if (connectionMade)
    {
        uint32_t start = millis();
        while (gsmClient && (gsmClient.available() < NTP_PACKET_SIZE) && ((millis() - start) < 5000L)){}
        int Nist_pkt_sz = gsmClient.available();
        MS_DBG(F("NIST responded after"), millis() - start, F("ms with "),Nist_pkt_sz);
        if (Nist_pkt_sz >= 44)
        {
            byte  _packetRx[NTP_PACKET_SIZE+10];
            memset(_packetRx, 0, NTP_PACKET_SIZE+10);
  
            //byte response[100] = {0}; //Needs to be larger enough for complete response
            gsmClient.read(_packetRx, NTP_PACKET_SIZE);
            uint32_t highWord = word( _packetRx[40],  _packetRx[41]);
            uint32_t lowWord = word( _packetRx[42],  _packetRx[43]);
            // combine the four bytes (two words) into a long integer
            // this is NTP time (seconds since Jan 1 1900):
            uint32_t secsSince1900 = highWord << 16 | lowWord;
            /* If before Jan 1, 2019 or after Jan 1, 2030, most likely an error */
            // Should be after compile date
            if ((_currentEpoc > 1546300800) &&  (_currentEpoc< 1893456000)) {
                _currentEpoc = secsSince1900 - SEVENZYYEARS;
                MS_DBG(F("NIST time "), _currentEpoc);
            } else {
                MS_DBG(F("NIST invalid time "), secsSince1900);
            }
        }
        else
        {
            MS_DBG(F("NIST Time server did not respond!"));
        }
    }
    else {MS_DBG(F("Unable to open UDP to NIST!"));}
    gsmModem.sendAT(GF("IP"), 1);  // Leave in TCP mode
    return _currentEpoc;
}
#endif //0

//Az extensions
void DigiXBeeCellularTransparent::setApn(const char *newAPN,bool copyId)
{ 
   uint8_t newAPN_sz = strlen(newAPN);
    _apn = newAPN; 
     //TODO: njh test setAPN CopyID functons

    if (copyId) {
        /* Do size checks, allocate memory for the LoggerID, copy it there
        *  then set assignment.
        */
       // For cell phone note clear what max size is.
        #define  CELLAPN_MAX_sz 99
        if (newAPN_sz > CELLAPN_MAX_sz) {
            char *apn2 = (char *)newAPN;
            PRINTOUT(F("\n\r   LoggerModem:setAPN too long: Trimmed to "),newAPN_sz);
            apn2[newAPN_sz] = 0; //Trim max size
            newAPN_sz=CELLAPN_MAX_sz; 
        }
        if (NULL == _apn_buf) {
            _apn_buf = new char[newAPN_sz+2]; //Allow for trailing 0
        } else {
            PRINTOUT(F("\nLoggerModem::setAPN error - expected NULL ptr"));
        }
        if (NULL == _apn_buf) {
            //Major problem
            PRINTOUT(F("\nLoggerModem::setAPN error -no buffer "),  _apn_buf);
        } else {
            strcpy (_apn_buf,newAPN);
            _apn =  _apn_buf;
        }
        MS_DBG(F("\nsetAPN cp "),  _apn," sz: ",newAPN_sz);
    } 
}

String DigiXBeeCellularTransparent::getApn(void){ return _apn; }
