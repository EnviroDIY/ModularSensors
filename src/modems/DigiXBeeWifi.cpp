/*
 *DigiXBeeWifi.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi S6B Wifi XBee's
*/

// Included Dependencies
#include "DigiXBeeWifi.h"

// Constructors
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *ssid, const char *pwd,
                           uint8_t measurementsToAverage)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin,
             measurementsToAverage),
    _tinyModem(*modemStream, modemResetPin)
{
    _ssid = ssid;
    _pwd = pwd;
    TinyGsmClient *tinyClient = new TinyGsmClient(_tinyModem);
    _tinyClient = tinyClient;
}


bool DigiXBeeWifi::didATRespond(void)
{
    return _tinyModem.testAT(10);
}


bool DigiXBeeWifi::extraModemSetup(void)
{
    bool success = true;
    success &= _tinyModem.init();  // initialize
    _modemName = _tinyModem.getModemName();
    if (_tinyModem.commandMode())
    {
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        _tinyModem.sendAT(F("D8"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        _tinyModem.sendAT(F("D9"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        _tinyModem.sendAT(F("D7"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Put the XBee in pin sleep mode
        _tinyModem.sendAT(F("SM"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        _tinyModem.sendAT(F("SO"),200);
        success &= _tinyModem.waitResponse() == 1;
        // Put the network connection parameters into flash
        success &= _tinyModem.networkConnect(_ssid, _pwd);
        // Write changes to flash and apply them
        _tinyModem.writeChanges();
        // Exit command mode
        _tinyModem.exitCommand();
    }
    else success = false;
    return success;
}


// This checks to see if enough time has passed for measurement completion
// In the case of the modem, we consider a measurement to be "complete" when
// the modem has registered on the network *and* returns good signal strength.
// In theory, both of these things happen at the same time - as soon as the
// module detects a network with sufficient signal strength, it connects and
// will respond corretly to requests for its connection status and the signal
// strength.  In reality sometimes the modem might respond with successful
// network connection before it responds with a valid signal strength or it
// might be able to return a real measurement of cellular signal strength but
// not be able to register to the network.  We'd prefer to wait until it both
// responses are good so we're getting an actual signal strength and it's as
// close as possible to what the antenna is will see when the data publishers
// push data.
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
    bool isConnected = _tinyModem.isNetworkConnected();
    int signalResponse = _tinyModem.getSignalQuality();
    if (isConnected && signalResponse != 0 && signalResponse != 99)
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


bool DigiXBeeWifi::addSingleMeasurementResult(void)
{
    bool success = true;

    // Initialize float variable
    int16_t signalQual = -9999;
    int16_t percent = -9999;
    int16_t rssi = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        // The WiFi XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // Connecting to the Google DNS servers for now
        MS_DBG(F("Opening connection to check connection strength..."));
        bool usedGoogle = false;
        if (!_tinyModem.gotIPforSavedHost())
        {
            usedGoogle = true;
            IPAddress ip(8, 8, 8, 8);  // This is the IP address of time-c-g.nist.gov
            success &= _tinyClient->connect(ip, 80);
        }
        _tinyClient->print('!');  // Need to send something before connection is made
        delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
        if (usedGoogle)
        {
            _tinyClient->stop();
        }

        // Get signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no service/signal.
        // The TinyGSM getSignalQuality function returns the same "no signal"
        // value (99 CSQ or 0 RSSI) in all 3 cases.
        MS_DBG(F("Getting signal quality:"));
        signalQual = _tinyModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);

        // Convert signal quality to RSSI
        rssi = signalQual;
        percent = getPctFromRSSI(signalQual);

        MS_DBG(F("RSSI:"), rssi);
        MS_DBG(F("Percent signal strength:"), percent);
    }
    else MS_DBG(getSensorName(), F("is not connected to the network; unable to get signal quality!"));

    verifyAndAddMeasurementResult(RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(PERCENT_SIGNAL_VAR_NUM, percent);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}


bool DigiXBeeWifi::connectInternet(uint32_t maxConnectionTime)
{
    bool retVal = true;

    if (bitRead(_sensorStatus, 1) == 0 || bitRead(_sensorStatus, 2) == 0)  // NOT yet powered
    {
        modemPowerUp();
    }
    if (bitRead(_sensorStatus, 3) == 0)  // No attempts yet to wake the modem
    {
        waitForWarmUp();
        retVal &= wake();
    }
    if (bitRead(_sensorStatus, 0) == 0)  // Not yet setup
    {
        retVal &= setup();  // Set-up if necessary
    }
    if (!retVal)
    {
        MS_DBG(F("Modem did't wake up! Cannot connect to the internet!"));
        return retVal;
    }

    // Check that the modem is responding to AT commands.  If not, give up.
    #if defined MS_DIGIXBEEWIFI_DEBUG
        uint32_t start = millis();
    #endif
    MS_DBG(F("\nWaiting for"), getSensorName(), F("to respond to AT commands..."));
    if (!_tinyModem.testAT(_stabilizationTime_ms + 500))
    {
        MS_DBG(F("No response to AT commands! Cannot connect to the internet!"));
        return false;
    }
    else MS_DBG(F("... AT OK after"), millis() - start, F("milliseconds!"));

    MS_DBG(F("\nAttempting to connect to WiFi network..."));
    if (!(_tinyModem.isNetworkConnected()))
    {
        MS_DBG(F("Sending credentials..."));
        while (!_tinyModem.networkConnect(_ssid, _pwd)) {};
        MS_DBG(F("Waiting up to"), maxConnectionTime/1000,
                   F("seconds for connection"));
        if (!_tinyModem.waitForNetwork(maxConnectionTime))
        {
            MS_DBG(F("... WiFi connection failed"));
            return false;
        }
    }
    MS_DBG(F("... WiFi connected after"), millis() - start,
               F("milliseconds!"));
    return true;
}


void DigiXBeeWifi::disconnectInternet(void)
{
    // XBee doesn't like to disconnect at all, so we're doing nothing
}


// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
uint32_t DigiXBeeWifi::getNISTTime(void)
{
    // bail if not connected to the internet
    if (!_tinyModem.isNetworkConnected())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    // Must ensure that we do not ping the daylight more than once every 4 seconds
    // NIST clearly specifies here that this is a requirement for all software
    // that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
    while (millis() < _lastNISTrequest + 4000) {}

    // Make TCP connection
    MS_DBG(F("Connecting to NIST daytime Server"));
    bool connectionMade = false;
    IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
    connectionMade = _tinyClient->connect(ip, 37);  // XBee's address lookup falters on time.nist.gov
    _tinyClient->print('!');  // Need to send something before connection is made
    delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

    // Wait up to 5 seconds for a response
    if (connectionMade)
    {
        long start = millis();
        while (_tinyClient && _tinyClient->available() < 4 && millis() - start < 5000L){}

        if (_tinyClient->available() >= 4)
        {
            MS_DBG(F("\nNIST responded after"), millis() - start, F("ms"));
            // Response is returned as 32-bit number as soon as connection is made
            // Connection is then immediately closed, so there is no need to close it
            uint32_t secFrom1900 = 0;
            byte response[4] = {0};
            for (uint8_t i = 0; i < 4; i++)
            {
                response[i] = _tinyClient->read();
                MS_DBG(F("\nResponse Byte"), i, ':', (char)response[i],
                           '=', response[i], '=', String(response[i], BIN));
                secFrom1900 += 0x000000FF & response[i];
                // MS_DBG(F("\nseconds from 1900 after byte:"),String(secFrom1900, BIN));
                if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
            }
            MS_DBG(F("\nSeconds from 1900 returned by NIST (UTC):"),
                       secFrom1900, '=', String(secFrom1900, BIN));

            // Close the TCP connection, just in case
            _tinyClient->stop();

            // Return the timestamp
            uint32_t unixTimeStamp = secFrom1900 - 2208988800;
            MS_DBG(F("\nUnix Timestamp returned by NIST (UTC):"), unixTimeStamp);
            // If before Jan 1, 2017 or after Jan 1, 2030, most likely an error
            if (unixTimeStamp < 1483228800) return 0;
            else if (unixTimeStamp > 1893456000) return 0;
            else return unixTimeStamp;
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
