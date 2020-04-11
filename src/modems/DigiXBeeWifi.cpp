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

#include "LoggerModemMacros.h"
//#define USE_NTP 1
#if defined USE_NTP
#include "NTPClientTinyGsm.h"
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);
NTPClient timeClient();
#endif //USE_NTP



// Constructor/Destructor
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *ssid, const char *pwd)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin),
    #ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger, modemResetPin),
    #else
    gsmModem(*modemStream, modemResetPin),
    #endif
    gsmClient(gsmModem)
{
    _ssid = ssid;
    _pwd = pwd;
}

// Destructor
DigiXBeeWifi::~DigiXBeeWifi() {}

MS_MODEM_WAKE(DigiXBeeWifi);

MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);

MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeWifi);

bool DigiXBeeWifi::extraModemSetup(void)
{
    bool success = true;
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    if (!success) {MS_DBG(F("Failed init")); } 
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    if (gsmModem.commandMode())
    {
        gsmModem.getSeries();
        _modemName = gsmModem.getModemName();
        PRINTOUT(F("XbeeWiFi Initializing Internet comms with modem '"),_modemName,F("'"));
        // Leave all unused pins disconnected. Use the PR command to pull all of the inputs on the device high
        // using 40 k internal pull-up resistors. You do not need a specific treatment for unused outputs.
        //   Mask Bit Description
        // 1 0001  0 TH11 DIO4
        // 1 0002  1 TH17 DIO3
        // 1 0004  2 TH18 DIO2
        // 1 0008  3 TH19 DIO1
        // 1 0010  4 TH20 DIO0
        // 1 0020  5 TH16 DIO6/RTS
        // 0 0040  6 TH09 DIO8/DTR/Sleep Request
        // 0 0080  7 TH03 DIN
        // 1 0100  8 TH15 DIO5/Associate
        // 0 0200  9 TH13 DIO9/- OnSLEEP
        // 1 0400 10 TH04 DIO12
        // 1 0800 11 TH06 DIO10/PWM RSSI
        // 1 1000 12 TH07 DIO11/PWM1
        // 1 2000 13 TH12 DIO7/-CTR
        // 0 4000 14 TH02 DIO13/DOUT 
        //   3D3F
        gsmModem.sendAT(GF("PR"),"3D3F");
        success &= gsmModem.waitResponse() == 1;
        if (!success) {MS_DBG(F("Fail PR "),success);}
        #if !defined MODEMPHY_NEVER_SLEEPS
            #define XBEE_SLEEP_SETTING 1
            #define XBEE_SLEEP_ASSOCIATE 200
        #else
            #define XBEE_SLEEP_SETTING 0
            #define XBEE_SLEEP_ASSOCIATE 40
        #endif //MODEMPHY_NEVER_SLEEPS
        // To use sleep pins they physically need to be enabled.
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(GF("D8"),XBEE_SLEEP_SETTING);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"),XBEE_SLEEP_SETTING);
        success &= gsmModem.waitResponse() == 1;
        if (!success) {MS_DBG(F("Fail D9 "),success);}/**/
        // /#endif //MODEMPHY_USE_SLEEP_PINS_SETTING
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        /*gsmModem.sendAT(GF("D7"),1);
        success &= gsmModem.waitResponse() == 1;
        if (!success) {MS_DBG(F("Fail D7 "),success);}*/
        // Turn on the associate LED (if you're using a board with one)
        // NOTE:  Only pin 15/DIO5 can be used for this function
        //gsmModem.sendAT(GF("D5"),1);
        //success &= gsmModem.waitResponse() == 1;
        // Turn on the RSSI indicator LED (if you're using a board with one)
        // NOTE:  Only pin 6/DIO10/PWM0 can be used for this function
        //gsmModem.sendAT(GF("P0"),1);
        //success &= gsmModem.waitResponse() == 1;
        //Set to TCP mode
        gsmModem.sendAT(GF("IP"),1);
        success &= gsmModem.waitResponse() == 1;
        if (!success) {MS_DBG(F("Fail IP "),success);}
     
        // Put the XBee in pin sleep mode in conjuction with D8=1
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"),XBEE_SLEEP_SETTING);
        success &= gsmModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        // 40 - Aay associated with AP during sleep - draws more current (+10mA?)
        //100 -Cyclic sleep ST specifies time before reutnring to sleep
        //200 - SRGD magic number
        gsmModem.sendAT(GF("SO"),XBEE_SLEEP_ASSOCIATE);
        success &= gsmModem.waitResponse() == 1;
       
        MS_DBG(F("Setting Wifi Network Options..."));
        // Put the network connection parameters into flash
        success &= gsmModem.networkConnect(_ssid, _pwd);
        // Set the socket timeout to 10s (this is default)
        if (!success) {MS_DBG(F("Fail Connect "),success);success=true;}
        gsmModem.sendAT(GF("TM"),64);
        success &= gsmModem.waitResponse() == 1;
        if (success) {MS_DBG(F("Setup Wifi Network "),_ssid);} 
        else  {MS_DBG(F("Failed Setting WiFi"),_ssid);}
        // Write changes to flash and apply them
        gsmModem.writeChanges();

        //Scan for AI  last node join request
        uint16_t loops=0;
        int16_t ui_db;
        uint8_t status;
        String ui_op;
        bool apRegistered=false;
        PRINTOUT(F("Loop=Sec] rx db : Status #Polled Status every 1sec/30sec"));
        uint8_t reg_count =0;
        for ( unsigned long start = millis(); millis() - start < 300000; loops++) {
            ui_db = 0;// gsmModem.getSignalQuality();
            gsmModem.sendAT(GF("AI"));
            status=gsmModem.readResponseInt(10000L);
            ui_op = String(loops)+"="+String((float)millis()/1000)+"] "+String(ui_db)+":0x"+String(status,HEX);
            if (0==status) {
                ui_op += " Cnt="+String(reg_count);
                PRINTOUT(ui_op);
                #define XBEE_SUCCESS_CNTS 3
                if (++reg_count > XBEE_SUCCESS_CNTS) {
                    apRegistered=true;
                    break;
                }
            }
            PRINTOUT(ui_op); 
            delay(1000);
        }         
        if (!apRegistered) {
           //Fut: Could Scan for access points here AS commnd to indicate what is available
            PRINTOUT(F("XbeeWiFi not AP Registered - aborting attempt, hope it works next time"));
            delay(100);
            //NVIC_SystemReset();
            success = false;
        } else {       
            MS_DBG(F("Get IP number"));
            String xbeeRsp;
            uint8_t index=0;
            bool AllocatedIpSuccess = false;
            //Checkfor IP allocation
            #define MDM_IP_STR_MIN_LEN 7
            #define MDM_LP_IPMAX 16        
            for (int mdm_lp=1;mdm_lp<=MDM_LP_IPMAX;mdm_lp++) {
                delay(mdm_lp*500);
                gsmModem.sendAT(F("MY"));  // Request IP #
                index = gsmModem.waitResponse(1000,xbeeRsp);
                MS_DBG(F("mdmIP["),mdm_lp,"/",MDM_LP_IPMAX,F("] '"),xbeeRsp,"'=",xbeeRsp.length());
                if (0!=xbeeRsp.compareTo("0.0.0.0") && (xbeeRsp.length()>MDM_IP_STR_MIN_LEN)) {
                    AllocatedIpSuccess = true;
                    break;
                }
                xbeeRsp="";
            }
            if (!AllocatedIpSuccess) {
                PRINTOUT(F("XbeeWiFi not received IP# -hope it works next time"));
                //delay(1000);
                //NVIC_SystemReset();
                success = false;
            } else 
            {
                //success &= AllocatedIpSuccess;
                PRINTOUT(F("XbeeWiFi IP# ["),xbeeRsp,F("]"));
                xbeeRsp="";
                //Display DNS allocation
                bool DnsIpSuccess = false;
                #define MDM_LP_DNSMAX 11   
                for (int mdm_lp=1;mdm_lp<=MDM_LP_DNSMAX;mdm_lp++) 
                {
                    delay(mdm_lp*500);
                    gsmModem.sendAT(F("NS"));  // Request DNS #
                    index &= gsmModem.waitResponse(1000,xbeeRsp);
                    MS_DBG(F("mdmDNS["),mdm_lp,"/",MDM_LP_DNSMAX,F("] '"),xbeeRsp,"'");
                    if (0!=xbeeRsp.compareTo("0.0.0.0")&& (xbeeRsp.length()>MDM_IP_STR_MIN_LEN)) 
                    {
                        DnsIpSuccess = true;
                        break;
                    }
                    xbeeRsp="";
                }

                if ( false == DnsIpSuccess)
                {
                    success = false;
                    PRINTOUT(F("XbeeWifi init test FAILED - hope it works next time"));
                } else {
                    PRINTOUT(F("XbeeWifi init test PASSED"));
                }     
            }
            #if 0 //defined MS_DIGIXBEEWIFI_DEBUG 
            //as of 0.23.15 the modem as sensor has problems
                int16_t rssi, percent;
                getModemSignalQuality(rssi, percent);
                MS_DBG(F("mdmSQ["),toAscii(rssi),F(","),percent,F("%]"));
            #endif //MS_DIGIXBEEWIFI_DEBUG
        }
        gsmModem.exitCommand();
    }
    
    if (false== success) 
    {
        PRINTOUT(F("Xbee '"),_modemName,F("' failed."));
    }

    return success;
}


void DigiXBeeWifi::disconnectInternet(void)
{
    // Wifi XBee doesn't like to disconnect AT ALL, so we're doing nothing
    // If you do disconnect, you must power cycle before you can reconnect
    // to the same access point.
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

    gsmClient.stop();

    /* Try up to 12 times to get a timestamp from NIST */
    #if !defined NIST_SERVER_RETRYS
    #define NIST_SERVER_RETRYS 4
    #endif //NIST_SERVER_RETRYS
    String nistIpStr;
    uint8_t index=0;
    for (uint8_t i = 0; i < NIST_SERVER_RETRYS; i++)
    {

        /* Must ensure that we do not ping the daylight more than once every 4 seconds */
        /* NIST clearly specifies here that this is a requirement for all software */
        /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
        while (millis() < _lastNISTrequest + 4000)
        {
        }

        /* Make TCP connection */
        MS_DBG(F("\nConnecting to NIST daytime Server @"),millis());
        bool connectionMade = false;

        /* This is the IP address of time-e-wwv.nist.gov  */
        /* If it fails options here https://tf.nist.gov/tf-cgi/servers.cgi */
        //gsmclient.ip("time.nist.gov");
        #define IP_STR_LEN 18
        const char ipAddr[NIST_SERVER_RETRYS][IP_STR_LEN] = {{"132,163, 97, 1"},{"132, 163, 97, 2"},{"132, 163, 97, 3"},{"132, 163, 97, 4"}} ;
        IPAddress ip1(132,163,97,1); //Initialize
        gsmModem.sendAT(F("LAtime.nist.gov"));
        index = gsmModem.waitResponse(1000,nistIpStr);
        nistIpStr.trim();
        if ((nistIpStr == "") || (nistIpStr == GF("ERROR")) || (nistIpStr.length() >6) ) {
            ip1.fromString(ipAddr[i]);
            MS_DBG(F("Bad lookup"),nistIpStr,"'=",nistIpStr.length(), F(" Using "),ipAddr[i]);
        } else {
            ip1.fromString(nistIpStr);
            MS_DBG(F("Good lookup mdmIP["),i,"/",MDM_LP_IPMAX,F("] '"),nistIpStr,"'=",nistIpStr.length());
        }


        //const char *address ="132, 163, 97, 6";
        //connectionMade = gsmClient.connect(ipAddr[i], 37);
        //IPAddress ip[NIST_SERVER_RETRYS] ={(132, 163, 97, 6),(132, 163, 97, 6),(132, 163, 97, 6),(132, 163, 97, 6) };

        connectionMade = gsmClient.connect(ip1, 37);

        // delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

        /* Wait up to 5 seconds for a response */
        if (connectionMade)
        {
            uint32_t start = millis();
            /* Slight delay Wait again so NIST doesn't refuse us! */
            delay((i+1)*100L);
            /* Need to send something before connection is made */
            gsmClient.println('!');
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


bool DigiXBeeWifi::getModemSignalQuality(int16_t &rssi, int16_t &percent)
{
    bool success = true;

    // Initialize float variable
    int16_t signalQual = -9999;
    percent = -9999;
    rssi = -9999;

    // The WiFi XBee needs to make an actual TCP connection and get some sort
    // of response on that connection before it knows the signal quality.
    // Connecting to the Google DNS servers - this doesn't really work
    // MS_DBG(F("Opening connection to check connection strength..."));
    // bool usedGoogle = false;
    // if (!gsmModem.gotIPforSavedHost())
    // {
    //     usedGoogle = true;
    //     IPAddress ip(8, 8, 8, 8);  // This is one of Google's IP's
    //     gsmClient.stop();
    //     success &= gsmClient.connect(ip, 80);
    // }
    // gsmClient.print('!');  // Need to send something before connection is made
    // delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

    //MS_DBG(F("Opening connection to NIST to check connection strength..."));
    // This is the IP address of time-c-g.nist.gov
    // XBee's address lookup falters on time.nist.gov
    //IPAddress ip(132, 163, 97, 6);
    //gsmClient.connect(ip, 37);
    // Wait again so NIST doesn't refuse us!
    //delay(4000L);
    // Need to send something before connection is made
    //gsmClient.println('!');
    //delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality2:"));
    signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    if (gsmClient.connected())
    {
        gsmClient.stop();
    }

    // Convert signal quality to RSSI
    rssi = signalQual;
    percent = getPctFromRSSI(signalQual);

    MS_DBG(F("RSSI:"), rssi);
    MS_DBG(F("Percent signal strength:"), percent);

    return success;
}


bool DigiXBeeWifi::updateModemMetadata(void)
{
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI = -9999;
    loggerModem::_priorSignalPercent = -9999;
    loggerModem::_priorBatteryState = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorModemTemp = -9999;

    // Initialize variable
    int16_t signalQual = -9999;
    uint16_t volt = 9999;

    // Enter command mode only once
    MS_DBG(F("Entering Command Mode:"));
    success &= gsmModem.commandMode();

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

    MS_DBG(F("Getting input voltage:"));
    volt = gsmModem.getBattVoltage();
    MS_DBG(F("CURRENT Modem input battery voltage:"), volt);
    if (volt != 9999)
        loggerModem::_priorBatteryVoltage = (float)volt;
    else
        loggerModem::_priorBatteryVoltage = (float)-9999;

    MS_DBG(F("Getting chip temperature:"));
    loggerModem::_priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem temperature:"), loggerModem::_priorModemTemp);

    // Exit command modem
    MS_DBG(F("Leaving Command Mode:"));
    gsmModem.exitCommand();

    return success;
}
#if 0 // !defined USE_NTP
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
            byte response[100] = {0}; //Needs to be larger enough for complete response
            gsmClient.read(response, 4);
            MS_DBG(F("<<< something fm gsmClient.read"));
            return parseNISTBytes(response);
        }
        else
        {
            MS_DBG(F("NIST Time server did not respond!"));
            return 0;
        }
    }
    else
    {
        MS_DBG(F("Unable to open TCP to NIST!"));
    }
    return 0;
}
#elif 0//1 == USE_NTP
// Get the time from Http TIME protocol 
uint32_t DigiXBeeWifi::getNISTTime(void)
{
    uint32_t _currentEpoc=0;
    /* bail if not connected to the internet */
    if (!isInternetAvailable())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    /* Must ensure that we do not ping the daylight more than once every 4 seconds */
    /* NIST clearly specifies here that this is a requirement for all software */
    /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
    //while (millis() < _lastNISTrequest + 4000) {}

    /* Make TCP connection */
    MS_DBG(F("\nUsing HTP for time"));
   // bool connectionMade = false;

    /* This is the IP address of time-c-g.nist.gov */
    /* XBee's address lookup falters on time.nist.gov */
    //IPAddress ip(129, 6, 15, 30);
    //connectionMade = gsmClient.connect(ip, 37, 15);
    /* Wait again so NIST doesn't refuse us! */
    //delay(4000L);
    /* Need to send something before connection is made */
    String ui_vers = gsmModem.sendATGetString(GF("VR"));
    gsmModem.sendAT(GF("IP"), 0);  // Put in UDP mode
    //gsmClient.println("ATP0");
    //gsmClient.println("DL192.241.211.46");

    /* Wait up to 5 seconds for a response */
    //if (connectionMade)
    {
        uint32_t start = millis();
        /*Look for
        [00] HTTP/1.1 400 Bad Request
        [27] Server: nginx/1.10.3 (Ubuntu)
        [58] Date: Wed, 28 Aug 2019 22:50:25 GMT
        [95] Content-Type: text/html
        */ 
        while (gsmClient && gsmClient.available() < 95 && millis() - start < 5000L){}


        if (gsmClient.available() >= 4)
        {
            MS_DBG(F("Web responded after"), millis() - start, F("ms"));
            byte response[101] = {0}; //Needs to be larger enough for complete response
            gsmClient.read(response, 100);
            
            MS_DBG(F("<<< something fm gsmClient.read"));
            MS_DBG(F("rsp"),response[58]);
            //parseNISTBytes(response);
        }
        else
        {
            MS_DBG(F("HTTP server did not respond!"));
        }
    }
    return _currentEpoc;
}
#endif //

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
        MS_DEEP_DBG(F("\nsetWiFiPwd cp "),  _ssid," sz: ",newPwd_sz);
    }     
 }

String DigiXBeeWifi::getWiFiId(void) {return _ssid;}
String DigiXBeeWifi::getWiFiPwd(void){return _pwd; }
