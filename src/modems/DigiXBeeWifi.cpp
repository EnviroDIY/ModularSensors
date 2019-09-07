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
MS_MODEM_GET_MODEM_BATTERY_AVAILABLE(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(DigiXBeeWifi);
MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);


bool DigiXBeeWifi::extraModemSetup(void)
{
    bool success = true;
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    MS_DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode())
    {
        MS_DBG(F("Setting I/O Pins..."));
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
        gsmModem.sendAT(GF("SO"),200);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Wifi Network Options..."));
        // Put the network connection parameters into flash
        success &= gsmModem.networkConnect(_ssid, _pwd);
        // Set the socket timeout to 10s (this is default)
        gsmModem.sendAT(GF("TM"),64);
        success &= gsmModem.waitResponse() == 1;
        // Write changes to flash and apply them
        gsmModem.writeChanges();
        // Exit command mode
        gsmModem.exitCommand();
    }
    else
    {
        success = false;
    }

    if (success)
    {
        MS_DBG(F("... Setup successful!"));
    }
    else
    {
        MS_DBG(F("... failed!"));
    }
    return success;
}


// This checks to see if enough time has passed for measurement completion
// In the case of the modem, we consider a measurement to be "complete" when
// the modem has registered on the network.
bool DigiXBeeWifi::isMeasurementComplete(bool debug)
{
    #if defined MS_DIGIXBEEWIFI_DEBUG
    debug = true;
    #endif
    
    // If a measurement failed to start, the sensor will never return a result,
    // so the measurement time is essentially already passed
    // For a cellular modem nothing happens to "start" a measurement so bit 6
    // will be set by startSingleMeasurement() as long as bit 4 was set by wake().
    // For a WiFi modem, startSingleMeasurement actually sets the WiFi connection
    // parameters.
    if (!bitRead(_sensorStatus, 6))
    {
        if (debug) {MS_DBG(getSensorName(),
            F("is not measuring and will not return a value!"));}
        return true;
    }

    // just defining this to not call multiple times below
    uint32_t now = millis();

    // We don't want to ping any of the modems too fast so they don't get
    // overwhelmed.  Make sure we've waited a little
    if (now - _lastConnectionCheck < 250) return false;

    // Check how long we've been waiting for the network connection and/or a
    // good measurement of signal quality.
    uint32_t elapsed_in_wait;

    // Cellular modems and wifi modems with the connection paramters always
    // saved to flash (like XBees) begin searching for and attempt to register
    // to the network as soon as they are awake - the GPRS paramters that need
    // to be set to actually *use* the network don't have to be set until we
    // make the attempt to use it.
    elapsed_in_wait = now - _millisSensorActivated;

    // If we're connected AND receiving valid signal strength, measurement is complete
    // In theory these happen at the same time, but in reality one or the other
    // may happen first.
    bool isConnected = gsmModem.isNetworkConnected();
    if (isConnected)
    {
        if (debug) MS_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"),
               getSensorName(), F("is now registered on the network and reporting valid signal strength!"));
        _lastConnectionCheck = now;
        return true;
    }

    // If we've exceeded the allowed time to wait for the network, give up
    if (elapsed_in_wait > XBEE_SIGNALQUALITY_TIME_MS)
    {
        if (debug) MS_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"),
               getSensorName(), F("has maxed out wait for network registration!  Ending wait."));
         // Leave status bits and times set - can still get a valid value!
        return true;
    }

    // If the modem isn't registered yet or doesn't report valid signal, we still need to wait
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
    else
    {
        MS_DBG(F("Unable to open TCP to NIST!"));
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
    // Connecting to the Google DNS servers for now
    MS_DBG(F("Opening connection to check connection strength..."));
    bool usedGoogle = false;
    if (!gsmModem.gotIPforSavedHost())
    {
        usedGoogle = true;
        IPAddress ip(8, 8, 8, 8);  // This is one of Google's IP's
        gsmClient.stop();
        success &= gsmClient.connect(ip, 80);
    }
    gsmClient.print('!');  // Need to send something before connection is made
    delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

    // MS_DBG(F("Opening connection to NIST to check connection strength..."));
    // /* This is the IP address of time-c-g.nist.gov */
    // /* XBee's address lookup falters on time.nist.gov */
    // IPAddress ip(129, 6, 15, 30);
    // gsmClient.connect(ip, 37);
    // /* Wait again so NIST doesn't refuse us! */
    // delay(4000L);
    // /* Need to send something before connection is made */
    // gsmClient.println('!');
    // delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality:"));
    signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    if (usedGoogle)
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

        // // The WiFi XBee needs to make an actual TCP connection and get some sort
        // // of response on that connection before it knows the signal quality.
        // // Connecting to the Google DNS servers for now
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

        // Must ensure that we do not ping the daylight more than once every 4 seconds
        // NIST clearly specifies here that this is a requirement for all software
        // that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() < _lastNISTrequest + 4000) {}
        // The WiFi XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        MS_DBG(F("Opening connection to NIST to check connection strength..."));
        /* This is the IP address of time-c-g.nist.gov */
        /* XBee's address lookup falters on time.nist.gov */
        IPAddress ip(129, 6, 15, 30);
        gsmClient.connect(ip, 37);
        /* Wait again so NIST doesn't refuse us! */
        delay(4000L);
        /* Need to send something before connection is made */
        gsmClient.println('!');
        delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

        // Get signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no service/signal.
        // The TinyGSM getSignalQuality function returns the same "no signal"
        // value (99 CSQ or 0 RSSI) in all 3 cases.
        MS_DBG(F("Getting signal quality:"));
        signalQual = gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);

        // if (usedGoogle)
        // {
        //     gsmClient.stop();
        // }

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
