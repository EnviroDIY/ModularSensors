/*
 *DigiXBeeWifi.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi S6B Wifi XBee's
*/

// Included DependenciesV
#include "DigiXBeeWifi.h"
#include "modems/LoggerModemMacros.h"


// Constructor/Destructor
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *ssid, const char *pwd,
                           uint8_t measurementsToAverage)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin,
             measurementsToAverage),
    #ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _ssid = ssid;
    _pwd = pwd;
}


// Destructor
DigiXBeeWifi::~DigiXBeeWifi(){}


MS_MODEM_DID_AT_RESPOND(DigiXBeeWifi);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(DigiXBeeWifi);
MS_MODEM_GET_MODEM_BATTERY_NA(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(DigiXBeeWifi);
MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);


bool DigiXBeeWifi::extraModemSetup(void)
{
    bool success = true;
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    if (!success)  MS_DBG(F("Failed init")); 
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    if (gsmModem.commandMode())
    {
        MS_DBG( _modemName,F(" in Cmd Mode. Setting I/O Pins..."));
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
        gsmModem.sendAT(GF("SO"),100);
        success &= gsmModem.waitResponse() == 1;
        if (!success)  MS_DBG(F("Failed Setup"));

        // Put the network connection parameters into flash
        success &= gsmModem.networkConnect(_ssid, _pwd);
        if (success) {MS_DBG(F("Setup Wifi Network "),_ssid);} 
        else  {MS_DBG(F("Failed Setting WiFi"),_ssid);}
        // Write changes to flash and apply them
        gsmModem.writeChanges();

        #ifdef MS_DIGIXBEEWIFI_DEBUG 
            MS_DBG(F("Get IP number"));
            String xbeeRsp;
            uint8_t index;
            bool AllocatedIpSuccess = false;
            for (int mdm_lp=1;mdm_lp<11;mdm_lp++) {
                delay(mdm_lp*500);
                gsmModem.sendAT(F("MY"));  // Request IP #
                index &= gsmModem.waitResponse(1000,xbeeRsp);
                MS_DBG(F("mdmIP["),toAscii(index),F("]"),xbeeRsp);
                if (0!=xbeeRsp.compareTo("0.0.0.0")) {
                    AllocatedIpSuccess = true;
                    break;
                }
                xbeeRsp="";
            }
            if (AllocatedIpSuccess) {
                int16_t rssi, percent;
                getModemSignalQuality(rssi, percent);
            }
        #endif //MS_DIGIXBEEWIFI_DEBUG
        // Exit command mode
        gsmModem.exitCommand();
    }
    else 
    {
        success = false;
        MS_DBG( _modemName,F(" failed to set Cmd Mode."));
    }
    return success;
}


// Get the time from NIST via TIME protocol (rfc868)
uint32_t DigiXBeeWifi::getNISTTime(void)
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

    /* Make TCP connection */
    MS_DBG(F("\nConnecting to NIST daytime Server"));
    bool connectionMade = false;

    /* This is the IP address of time-c-g.nist.gov */
    /* XBee's address lookup falters on time.nist.gov */
    IPAddress ip(129, 6, 15, 30);
    connectionMade = gsmClient.connect(ip, 37, 15);
    /* Wait again so NIST doesn't refuse us! */
    delay(4000L);
    /* Need to send something before connection is made */
    gsmClient.println('!');

    /* Wait up to 5 seconds for a response */
    if (connectionMade)
    {
        uint32_t start = millis();
        while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L){}


        if (gsmClient.available() >= 4)
        {
            MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
            byte response[4] = {0};
            gsmClient.read(response, 4);
            return parseNISTBytes(response);
        }
        else
        {
            MS_DBG(F("NIST Time server did not respond!"));
            return 0;
        }
    }
    else MS_DBG(F("Unable to open TCP to NIST!"));
    return 0;
}


bool DigiXBeeWifi::getModemSignalQuality(int16_t &rssi, int16_t &percent)
{
    // Initialize float variable
    bool success = true;
    int16_t signalQual = -9999;
    percent = -9999;
    rssi = -9999;

    // The WiFi XBee needs to make an actual TCP connection and get some sort
    // of response on that connection before it knows the signal quality.
    // Connecting to the Google DNS servers for now
    MS_DBG(F("Opening connection to check connection strength..."));
    bool usedGoogle = false;
    if (!gsmModem.gotIPforSavedHost())
    {
        usedGoogle = true;
        IPAddress ip(8, 8, 8, 8);  // This is one of Google's IP's
        success &= gsmClient.connect(ip, 80, 5);  // 5 second timeout
    }
    gsmClient.print('!');  // Need to send something before connection is made
    delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
    if (usedGoogle)
    {
        gsmClient.stop(15000L);
    }

    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality:"));
    signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    // Convert signal quality to RSSI
    rssi = signalQual;
    percent = getPctFromRSSI(signalQual);

    MS_DBG(F("RSSI:"), rssi);
    MS_DBG(F("Percent signal strength:"), percent);

    return success;
}


bool DigiXBeeWifi::addSingleMeasurementResult(void)
{
    bool success = true;

    /* Initialize float variable */
    int16_t signalQual = -9999;
    int16_t percent = -9999;
    int16_t rssi = -9999;
    float temp = -9999;

    /* Check a measurement was *successfully* started (status bit 6 set) */
    /* Only go on to get a result if it was */
    if (bitRead(_sensorStatus, 6))
    {
        // The WiFi XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // Connecting to the Google DNS servers for now
        MS_DBG(F("Opening connection to check connection strength..."));
        bool usedGoogle = false;
        if (!gsmModem.gotIPforSavedHost())
        {
            usedGoogle = true;
            IPAddress ip(8, 8, 8, 8);  // This is one of Google's IP's
            success &= gsmClient.connect(ip, 80, 5);  // 5 second timeout
        }
        gsmClient.print('!');  // Need to send something before connection is made
        delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
        if (usedGoogle)
        {
            gsmClient.stop(15000L);
        }

        // Enter command mode only once
        MS_DBG(F("Entering Command Mode:"));
        gsmModem.commandMode();

        // Get signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no service/signal.
        // The TinyGSM getSignalQuality function returns the same "no signal"
        // value (99 CSQ or 0 RSSI) in all 3 cases.
        MS_DBG(F("Getting signal quality:"));
        signalQual = gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);

        // Convert signal quality to RSSI
        rssi = signalQual;
        percent = getPctFromRSSI(signalQual);

        MS_DBG(F("RSSI:"), rssi);
        MS_DBG(F("Percent signal strength:"), percent);

        MS_DBG(F("Getting chip temperature:"));
        temp = getModemTemperature();
        MS_DBG(F("Modem temperature:"), temp);

        // Exit command modem
        MS_DBG(F("Leaving Command Mode:"));
        gsmModem.exitCommand();
    }
    else MS_DBG(getSensorName(), F("is not connected to the network; unable to get signal quality!"));

    verifyAndAddMeasurementResult(MODEM_RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(MODEM_PERCENT_SIGNAL_VAR_NUM, percent);
    verifyAndAddMeasurementResult(MODEM_BATTERY_STATE_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_BATTERY_PERCENT_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_BATTERY_VOLT_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_TEMPERATURE_VAR_NUM, temp);

    /* Unset the time stamp for the beginning of this measurement */
    _millisMeasurementRequested = 0;
    /* Unset the status bits for a measurement request (bits 5 & 6) */
    _sensorStatus &= 0b10011111;

    return success;
}

//Az extensions
void DigiXBeeWifi::setWiFiId(const char *newSsid,bool copyId) 
{
    uint8_t newSsid_sz = strlen(newSsid);
    _ssid = newSsid;
    if (copyId) {
        /* Do size checks, allocate memory for the LoggerID, copy it there
        *  then set assignment.
        */
        #define  WIFI_SSID_MAX_sz 32
        if (newSsid_sz > WIFI_SSID_MAX_sz) {
            char *WiFiId2 = (char *)newSsid;
            PRINTOUT(F("\n\r   LoggerModem:setWiFiId too long: Trimmed to "),newSsid_sz);
            WiFiId2[newSsid_sz] = 0; //Trim max size
            newSsid_sz=WIFI_SSID_MAX_sz; 
        }
        if (NULL == _ssid_buf) {
            _ssid_buf = new char[newSsid_sz+2]; //Allow for trailing 0
        } else {
            PRINTOUT(F("\nLoggerModem::setWiFiId error - expected NULL ptr"));
        }
        if (NULL == _ssid_buf) {
            //Major problem
            PRINTOUT(F("\nLoggerModem::setWiFiId error -no buffer "),  _ssid_buf);
        } else {
            strcpy (_ssid_buf,newSsid);
            _ssid =  _ssid_buf;
            //_ssid2 =  _ssid_buf;
        }
        MS_DBG(F("\nsetWiFiId cp "),  _ssid," sz: ",newSsid_sz);
    } 
}

void DigiXBeeWifi::setWiFiPwd(const char *newPwd,bool copyId)
/* nh Tested with and without newPwd setting
*/
{
    uint8_t newPwd_sz = strlen(newPwd);
    _pwd = newPwd;

    if (copyId) {
        /* Do size checks, allocate memory for the LoggerID, copy it there
        *  then set assignment.
        */
        #define  WIFI_PWD_MAX_sz 63 //Len 63 printable chars + 0
        if (newPwd_sz > WIFI_PWD_MAX_sz) {
            char *pwd2 = (char *)newPwd;
            PRINTOUT(F("\n\r   LoggerModem:setWiFiPwd too long: Trimmed to "),newPwd_sz);
            pwd2[newPwd_sz] = 0; //Trim max size
            newPwd_sz=WIFI_PWD_MAX_sz; 
        }
        if (NULL == _pwd_buf) {
            _pwd_buf = new char[newPwd_sz+2]; //Allow for trailing 0
        } else {
            PRINTOUT(F("\nLoggerModem::setWiFiPwd error - expected NULL ptr"));
        }
        if (NULL == _pwd_buf) {
            //Major problem
            PRINTOUT(F("\nLoggerModem::setWiFiPwd error -no buffer "),  _pwd_buf);
        } else {
            strcpy (_pwd_buf,newPwd);
            _pwd =  _pwd_buf;
        }
        MS_DBG(F("\nsetWiFiPwd cp "),  _ssid," sz: ",newPwd_sz);
    }     
 }

String DigiXBeeWifi::getWiFiId(void) {return _ssid;}
String DigiXBeeWifi::getWiFiPwd(void){return _pwd; }
