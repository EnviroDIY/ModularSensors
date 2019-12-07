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
                           const char *ssid, const char *pwd,
                           uint8_t measurementsToAverage)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin,
             measurementsToAverage),
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
DigiXBeeWifi::~DigiXBeeWifi(){}


MS_MODEM_DID_AT_RESPOND(DigiXBeeWifi);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);
MS_MODEM_GET_MODEM_BATTERY_AVAILABLE(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(DigiXBeeWifi);
MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);


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
        gsmModem.sendAT(GF("PR"),"3D3F");
        success &= gsmModem.waitResponse() == 1;
        if (!success) {MS_DBG(F("Fail PR "),success);}
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        //#define XBEE_USE_SLEEP_PINS
        #if defined XBEE_USE_SLEEP_PINS
        gsmModem.sendAT(GF("D8"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"),1);
        success &= gsmModem.waitResponse() == 1;
        if (!success) {MS_DBG(F("Fail D9 "),success);}/**/
        #endif //XBEE_USE_SLEEP_PINS
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
        #if defined XBEE_USE_SLEEP_PINS
        // Put the XBee in pin sleep mode in conjuction with D8=1
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"),1);
        success &= gsmModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        // 40 - Aay associated with AP during sleep - draws more current (+10mA?)
        //100 -Cyclic sleep ST specifies time before reutnring to sleep
        //200 - SRGD magic number
        gsmModem.sendAT(GF("SO"),200);
        success &= gsmModem.waitResponse() == 1;
        #endif //XBEE_USE_SLEEP_PINS
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
        gsmModem.writeChanges();/**/

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
        //Fut: Could Scan for access points here AS commnd
        //if (false ==apRegistered) {reset;}
        MS_DBG(F("Get IP number"));
        String xbeeRsp;
        uint8_t index=0;
        bool AllocatedIpSuccess = false;
        //Checkfor IP allocation
        //#define MDM_LP_MAX 30
        #define MDM_LP_MAX 16        
        for (int mdm_lp=1;mdm_lp<MDM_LP_MAX;mdm_lp++) {
            delay(mdm_lp*500);
            gsmModem.sendAT(F("MY"));  // Request IP #
            index = gsmModem.waitResponse(1000,xbeeRsp);
            MS_DBG(F("mdmIP["),mdm_lp,"/",MDM_LP_MAX,F("] '"),xbeeRsp,"'=",xbeeRsp.length());
            if (0!=xbeeRsp.compareTo("0.0.0.0") && (xbeeRsp.length()>7)) {
                AllocatedIpSuccess = true;
                break;
            }
            xbeeRsp="";
        }
        if (!AllocatedIpSuccess) {
            while (1) {
                PRINTOUT(F("XbeeWiFi not received IP# -reset"));
                delay(1000);
                NVIC_SystemReset();
            }
        }
        //success &= AllocatedIpSuccess;
        PRINTOUT(F("XbeeWiFi IP# ["),xbeeRsp,F("]"));

        //#ifdef MS_DIGIXBEEWIFI_DEBUG 
            //Display DNS allocation
            bool DnsIpSuccess = false;
            for (int mdm_lp=1;mdm_lp<11;mdm_lp++) 
            {
                delay(mdm_lp*500);
                gsmModem.sendAT(F("NS"));  // Request DNS #
                index &= gsmModem.waitResponse(1000,xbeeRsp);
                MS_DBG(F("mdmDNS["),toAscii(index),F("]"),xbeeRsp);
                if (0!=xbeeRsp.compareTo("0.0.0.0")) 
                {
                    DnsIpSuccess = true;
                    break;

                }
                xbeeRsp="";
            }
        if ( true != (AllocatedIpSuccess || DnsIpSuccess))
        {
                success = false;
        }        
        #if 0 //defined MS_DIGIXBEEWIFI_DEBUG 
            int16_t rssi, percent;
            getModemSignalQuality(rssi, percent);
            MS_DBG(F("mdmSQ["),toAscii(rssi),F(","),percent,F("%]"));
        #endif //MS_DIGIXBEEWIFI_DEBUG

        gsmModem.exitCommand();
    }
    
    if (false== success) 
    {
        PRINTOUT(F("Xbee '"),_modemName,F("' failed."));
    }

    return success;
}


bool DigiXBeeWifi::startSingleMeasurement(void)
{
    // Sensor::startSingleMeasurement() checks that if it's awake/active and sets
    // the timestamp and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    bool success = true;
    MS_DBG(F("Starting measurement on"), getSensorName());
    // Set the status bits for measurement requested (bit 5)
    // Setting this bit even if we failed to start a measurement to show that an attempt was made.
    _sensorStatus |= 0b00100000;

    // The SSID and password need to be set before the ESP8266m can join a
    //network and get signal strength
    bool alreadyConnect = gsmModem.isNetworkConnected();
    if (!alreadyConnect) success &= gsmModem.networkConnect(_ssid, _pwd);

    if (success)
    {
        MS_DBG(F("Connected (not clear why this is needed- no IP attempted)..."));
        // The WiFi XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // MS_DBG(F("Opening connection to check connection strength..."));
        // Connecting to the Google DNS servers - this just isn't as reliable
        // if (!gsmModem.gotIPforSavedHost())
        // {
        //     MS_DBG(F("Using a Google IP to test connection..."));
        //     gsmClient.stop();
        //     IPAddress ip(8, 8, 8, 8);  // This is one of Google's IP's
        //     success &= gsmClient.connect(ip, 80);
        // }
        // else
        // {
        //     MS_DBG(F("Using last connected IP to test connection:"));
        // }
        // gsmClient.print('!');  // Need to send something before connection is made

       // MS_DBG(F("Opening connection to NIST to check connection strength..."));
        // This is the IP address of time-e-wwv.nist.gov
        // XBee's address lookup falters on time.nist.gov
        //IPAddress ip(132, 163, 97, 6);
        //gsmClient.connect(ip, 37);

        // Unfortunately, using a ping doesn't work
        // gsmModem.commandMode();
        // gsmModem.sendAT(GF("PG8.8.8.8"));
        // gsmModem.waitResponse(10000L, GF("ms"));
        // gsmModem.exitCommand();

        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
    }
    // Otherwise, make sure that the measurement start time and success bit (bit 6) are unset
    else
    {
        MS_DBG(getSensorNameAndLocation(), F("did not successfully start a measurement."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
    }

    return success;
}


// This checks to see if enough time has passed for measurement completion
// In the case of the modem, we consider a measurement to be "complete" when
// the modem has registered on the network.
bool DigiXBeeWifi::verifyMeasurementComplete(bool debug)
{
    /* If a measurement failed to start, the sensor will never return a result, */
    /* so the measurement time is essentially already passed */
    /* For a cellular modem nothing happens to "start" a measurement so bit 6 */
    /* will be set by startSingleMeasurement() as long as bit 4 was set by wake(). */
    /* For a WiFi modem, startSingleMeasurement actually sets the WiFi connection */
    /* parameters. */
    if (!bitRead(_sensorStatus, 6))
    {
        if (debug)
        {
            MS_DBG(getSensorName(),
                   F("is not measuring and will not return a value!"));
        }
        return true;
    }

    /* just defining this to not call multiple times below */
    uint32_t now = millis();

    /* We don't want to ping any of the modems too fast so they don't get */
    /* overwhelmed.  Make sure we've waited a little */
    if (now - _lastConnectionCheck < 250)
        return false;

    /* Check how long we've been waiting for the network connection and/or a */
    /* good measurement of signal quality. */
    uint32_t elapsed_in_wait;
    MS_MODEM_IMEC_WAIT_LINE

    /* If we're connected AND receiving valid signal strength, measurement is complete */
    /* In theory these happen at the same time, but in reality one or the other */
    /* may happen first. */
    bool isConnected = gsmModem.isNetworkConnected();
    int signalResponse = gsmModem.getSignalQuality();
    /* The Wifi XBee is unique in that it cannot get signal quality until it */
    /* not only is connected to an access point and has opened a socket, but */
    /* has actually received data on that socket.  There's no command to force */
    /* open the socket, so we do it by sending out one character. */
    if (isConnected && signalResponse == 0 && millis() > _lastNISTrequest + 4000)
    {
        if (debug)
        {
            MS_DBG(F("Attempting to force open a connection to try and get valid signal strength!"));
        }
        gsmClient.print('!');  // Need to send something before connection is made
        delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
        return false;
    }
    if (isConnected && signalResponse != 0 && signalResponse != 99)
    {
        if (debug)
        {
            MS_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"), getSensorName(),
            F("is now registered on the network and reporting valid signal strength!"));
        }
        _lastConnectionCheck = now;
        return true;
    }

    /* If we've exceeded the allowed time to wait for the network, give up */
    if (elapsed_in_wait > _measurementTime_ms)
    {
        if (debug)
        {
            MS_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"), getSensorName(),
             F("has maxed out wait for network registration!  Ending wait."));
        }
        /* Leave status bits and times set - can still get a valid value! */
        return true;
    }

    /* If the modem isn't registered yet or doesn't report valid signal, we still need to wait */
    _lastConnectionCheck = now;
    return false;
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
        IPAddress ip(132, 163, 97, 6);
        connectionMade = gsmClient.connect(ip, 37);
        /* Wait again so NIST doesn't refuse us! */
        delay(4000L);
        /* Need to send something before connection is made */
        gsmClient.println('!');
        // delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

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


bool DigiXBeeWifi::getModemSignalQuality(int16_t &rssi, int16_t &percent)
{
    // Initialize float variable
    bool success = true;
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
    MS_DBG(F("Getting signal quality:"));
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


bool DigiXBeeWifi::addSingleMeasurementResult(void)
{
    bool success = true;

    /* Initialize float variable */
    int16_t signalQual = -9999;
    int16_t percent = -9999;
    int16_t rssi = -9999;
    float temp = -9999;
    float volt = -9999;

    /* Check a measurement was *successfully* started (status bit 6 set) */
    /* Only go on to get a result if it was */
    if (bitRead(_sensorStatus, 6))
    {

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

        // Since we had to open a connection in start single measurement, we
        // want to stop it here
        if (gsmClient.connected())
        {
            gsmClient.stop();
        }

        // Convert signal quality to RSSI
        rssi = signalQual;
        percent = getPctFromRSSI(signalQual);

        MS_DBG(F("RSSI:"), rssi);
        MS_DBG(F("Percent signal strength:"), percent);

        MS_DBG(F("Getting input voltage temperature:"));
        volt = gsmModem.getBattVoltage();
        MS_DBG(F("Modem input battery voltage:"), volt);

        MS_DBG(F("Getting chip temperature:"));
        temp = getModemTemperature();
        MS_DBG(F("Modem temperature:"), temp);

        // Exit command modem
        MS_DBG(F("Leaving Command Mode:"));
        gsmModem.exitCommand();
    }
    else
    {
        MS_DBG(getSensorName(), F("is not connected to the network; unable to get signal quality!"));
    }

    MS_DBG(F("PRIOR modem active time:"), String(_priorActivationDuration, 3));
    MS_DBG(F("PRIOR modem powered time:"), String(_priorPoweredDuration, 3));

    verifyAndAddMeasurementResult(MODEM_RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(MODEM_PERCENT_SIGNAL_VAR_NUM, percent);
    verifyAndAddMeasurementResult(MODEM_BATTERY_STATE_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_BATTERY_PERCENT_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_BATTERY_VOLT_VAR_NUM, volt);
    verifyAndAddMeasurementResult(MODEM_TEMPERATURE_VAR_NUM, temp);
    verifyAndAddMeasurementResult(MODEM_ACTIVATION_VAR_NUM, _priorActivationDuration);
    verifyAndAddMeasurementResult(MODEM_POWERED_VAR_NUM, _priorPoweredDuration);

    /* Unset the time stamp for the beginning of this measurement */
    _millisMeasurementRequested = 0;
    /* Unset the status bits for a measurement request (bits 5 & 6) */
    _sensorStatus &= 0b10011111;

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
        MS_DEEP_DBG(F("\nsetWiFiPwd cp "),  _ssid," sz: ",newPwd_sz);
    }     
 }

String DigiXBeeWifi::getWiFiId(void) {return _ssid;}
String DigiXBeeWifi::getWiFiPwd(void){return _pwd; }
