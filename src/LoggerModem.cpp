/*
 *LoggerModem.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file wraps the tinyGSM library:  https://github.com/vshymanskyy/TinyGSM
 *and adds in the power functions to turn the modem on and off.
*/

#include "LoggerModem.h"


// Constructors
loggerModem::loggerModem(TinyGsmModem *inModem, Client *inClient, ModemOnOff *onOff,
            const char *APN)
    : Sensor("Tiny GSM Modem", MODEM_NUM_VARIABLES, MODEM_WARM_UP_TIME_MS, 0, 0, -1, -1, 1),
      _APN(APN), _lastNISTrequest(0)
{
    _tinyModem = inModem;
    _tinyClient = inClient;
    _modemOnOff = onOff;
}

loggerModem::loggerModem(TinyGsmModem *inModem, Client *inClient, ModemOnOff *onOff,
            const char *ssid, const char *pwd)
    : Sensor("Tiny GSM Modem", MODEM_NUM_VARIABLES, MODEM_WARM_UP_TIME_MS, 0, 0, -1, -1, 1),
      _ssid(ssid), _pwd(pwd), _lastNISTrequest(0)
{
    _tinyModem = inModem;
    _tinyClient = inClient;
    _modemOnOff = onOff;
}

String loggerModem::getSensorName(void) { return _tinyModem->getModemName(); }
String loggerModem::getSensorLocation(void) { return F("modemSerial"); }

// The modem must be setup separately!
bool loggerModem::setup(void)
{
    bool retVal = false;

    // Initialize the modem
    MS_MOD_DBG(F("Starting up the "), _tinyModem->getModemName(), F("...\n"));

    _modemOnOff->begin();

    // Turn the modem on .. whether it was on or not
    // Need to turn on no matter what because some modems don't have an
    // effective way of telling us whether they're on or not
    _modemOnOff->on();
    // Double check if the modem is on; turn it on if not
    if(!_modemOnOff->isOn()) _modemOnOff->on();
    // Check again if the modem is on.  Only "begin" if it responded.
    if(_modemOnOff->isOn())
    {
        retVal = _tinyModem->begin();
        _modemOnOff->off();
        MS_MOD_DBG(F("   ... Complete!\n"));
    }
    else MS_MOD_DBG(F("   ... Modem failed to turn on!\n"));

    // Set the status bit marking that the modem has been set up (bit 1)
    _sensorStatus |= 0b00000010;

    return retVal;
}

// Do NOT turn the modem on and off with the regular power up and down or
// wake and sleep functions.
// This is because when it is run in an array with other sensors, we will
// generally want the modem to remain on after all the other sensors have
// gone to sleep and powered down so the modem can send out data
void loggerModem::powerUp(void)
{
    MS_MOD_DBG(F("Skipping modem in sensor power up!\n"));
}
void loggerModem::powerDown(void)
{
    MS_MOD_DBG(F("Skipping modem in sensor power down!\n"));
}


bool loggerModem::addSingleMeasurementResult(void)
{
    int signalQual = 0;
    int percent = 0;
    int rssi = -9999;

    // Check that the modem is responding to AT commands.  If not, give up.
    MS_MOD_DBG(F("\nWaiting up to 5 seconds for modem to respond to AT commands...\n"));
    if (_tinyModem->testAT(5000))
    {
        // The XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // Connecting to the NIST daytime server, which immediately returns a
        // 4 byte response and then closes the connection
        if (_tinyModem->getModemName().indexOf("XBee") > 0)
        {
            // Connect to the network
            // Only waiting for up to 5 seconds here for the internet!
            if (!(_tinyModem->isNetworkConnected()))
            {
                MS_MOD_DBG(F("No prior internet connection, attempting to make a connection."));
                connectInternet(5000L);
            }
            // Must ensure that we do not ping the daylight more than once every 4 seconds
            // NIST clearly specifies here that this is a requirement for all software
            /// that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
            while (millis() < _lastNISTrequest + 4000) {}
            MS_MOD_DBG("Connecting to NIST daytime server to check connection strength...\n");
            IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
            openTCP(ip, 37);
            _tinyClient->print(F("Hi!"));  // Need to send something before connection is made
            delay(100); // Need this delay!  Can get away with 50, but 100 is safer.
            while (_tinyClient->available()) _tinyClient->read();  // Delete anything returned
            _lastNISTrequest = millis();
        }

        // Get signal quality
        // Non XBee's do not need to be registered or "connnected" to the
        // network to get quality
        // if (_tinyModem->isNetworkConnected())
        // {

            MS_MOD_DBG("Getting signal quality:\n");
            signalQual = _tinyModem->getSignalQuality();

            // Convert signal quality to RSSI, if necessary
            if (_tinyModem->getModemName().indexOf("XBee") > 0 ||
                _tinyModem->getModemName().indexOf("ESP8266") > 0)
            {
                rssi = signalQual;
                percent = getPctFromRSSI(signalQual);
            }
            else
            {
                rssi = getRSSIFromCSQ(signalQual);
                percent = getPctFromCSQ(signalQual);
            }
        // }
        // else MS_MOD_DBG("Insufficient signal to connect to the internet!\n");
    }
    else MS_MOD_DBG(F("\nModem does not respond to AT commands!\n"));

    MS_MOD_DBG(F("RSSI: "), rssi, F("\n"));
    MS_MOD_DBG(F("Percent signal strength: "), percent, F("\n"));

    verifyAndAddMeasurementResult(RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(PERCENT_SIGNAL_VAR_NUM, percent);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    return true;
}


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//
bool loggerModem::connectInternet(uint32_t waitTime_ms)
{
    bool retVal = false;

    // Check if the modem is on; turn it on if not
    if(!_modemOnOff->isOn()) _modemOnOff->on();
    // Check again if the modem is on.  If it still isn't on, give up
    if(!_modemOnOff->isOn())
    {
        MS_MOD_DBG(F("\nModem failed to turn on!\n"));
        return false;
    }

    // Check that the modem is responding to AT commands.  If not, give up.
    MS_MOD_DBG(F("\nWaiting up to 5 seconds for modem to respond to AT commands...\n"));
    if (!_tinyModem->testAT(5000))
    {
        MS_MOD_DBG(F("\nModem does not respond to AT commands!\n"));
        return false;
    }

    // WiFi modules immediately re-connect to the last access point so we
    // can save just a tiny bit of time (and thus power) by not resending
    // the credentials every time.  (True for both ESP8266 and Wifi XBee)
    if (_ssid)
    {
        MS_MOD_DBG(F("\nAttempting to connect to WiFi network...\n"));
        if (!(_tinyModem->isNetworkConnected()))
        {
            MS_MOD_DBG("   Sending credentials...\n");
            while (!_tinyModem->networkConnect(_ssid, _pwd)) {};
            MS_MOD_DBG(F("   Waiting up to "), waitTime_ms/1000,
                       F(" seconds for connection\n"));
            uint32_t start = millis();
            if (_tinyModem->waitForNetwork(waitTime_ms))
            {
                retVal = true;
                MS_MOD_DBG("   ... WiFi connected after ", millis() - start,
                           " milliseconds!\n");
            }
            else MS_MOD_DBG("   ... WiFi connection failed\n");
        }
        else
        {
            MS_MOD_DBG("   ... Connected with saved WiFi settings!\n");
            retVal = true;
        }

    }
    else
    {
        MS_MOD_DBG(F("\nWaiting up to "), waitTime_ms/1000,
                   F(" seconds for cellular network registration...\n"));
        uint32_t start = millis();
        if (_tinyModem->waitForNetwork(waitTime_ms))
        {
            MS_MOD_DBG("   ... Registered after ", millis() - start,
                       " milliseconds.  Connecting to GPRS...\n");
            _tinyModem->gprsConnect(_APN, "", "");
            MS_MOD_DBG("   ...Connected!\n");
            retVal = true;
        }
        else MS_MOD_DBG("   ...Connection failed.\n");
    }
    _millisSensorActivated = millis();
    return retVal;
}

void loggerModem::disconnectInternet(void)
{
    if (_tinyModem->hasGPRS())
    {
        _tinyModem->gprsDisconnect();
        MS_MOD_DBG(F("Disconnected from cellular network.\n"));
    }
    else{}
        // _tinyModem->networkDisconnect();  // Eh.. why bother?
        // MS_MOD_DBG(F("Disconnected from WiFi network.\n"));
    _millisSensorActivated = 0;
}

int loggerModem::openTCP(const char *host, uint16_t port)
{
    MS_MOD_DBG("Connecting to ", host, "...");
    int ret_val = _tinyClient->connect(host, port);
    if (ret_val) MS_MOD_DBG("   ...Success!\n");
    else MS_MOD_DBG("   ...Connection failed.\n");
    return ret_val;
}

int loggerModem::openTCP(IPAddress ip, uint16_t port)
{
    MS_MOD_DBG("Connecting to ", ip, "...");
    int ret_val = _tinyClient->connect(ip, port);
    if (ret_val) MS_MOD_DBG("   ...Success!\n");
    else MS_MOD_DBG("   ...Connection failed.\n");
    return ret_val;
}

void loggerModem::closeTCP(void)
{
    _tinyClient->stop();
    MS_MOD_DBG(F("Closed TCP/IP.\n"));
}

bool loggerModem::modemPowerUp(void)
{
    // Turn the modem on .. whether it was on or not
    // Need to turn on no matter what because some modems don't have an
    // effective way of telling us whether they're on or not
    MS_MOD_DBG(F("Turning modem on.\n"));
    _modemOnOff->on();
    // Double check if the modem is on; turn it on if not
    if(!_modemOnOff->isOn()) _modemOnOff->on();
    // Mark the time that the sensor was powered
    _millisPowerOn = millis();
    // Set the status bit for sensor power (bit 0)
    _sensorStatus |= 0b00000001;
    return _modemOnOff->isOn();
}

bool loggerModem::modemPowerDown(void)
{
    MS_MOD_DBG(F("Turning modem off.\n"));
     // Wait for any sending to complete
    _tinyClient->flush();
    // Turn the modem off .. whether it was on or not
    // Need to turn off no matter what because some modems don't have an
    // effective way of telling us whether they're on or not
    _modemOnOff->off();
    // Unset the status bits for sensor power (bit 0), warm-up (bit 2),
    // activation (bit 3), stability (bit 4), measurement request (bit 5), and
    // measurement completion (bit 6)
    _sensorStatus &= 0b10000010;
    return true;
}

// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
uint32_t loggerModem::getNISTTime(void)
{
    bool connectionMade = false;
    // bail if not connected to the internet
    if (!_tinyModem->isNetworkConnected())
    {
        MS_MOD_DBG(F("No internet connection, cannot connect to NIST.\n"));
        return 0;
    }

    // Must ensure that we do not ping the daylight more than once every 4 seconds
    // NIST clearly specifies here that this is a requirement for all software
    // that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
    while (millis() < _lastNISTrequest + 4000) {}

    // Make TCP connection
    MS_MOD_DBG(F("Connecting to NIST daytime Server\n"));
    if (_tinyModem->getModemName().indexOf("XBee") > 0)
    {
        IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
        connectionMade = openTCP(ip, 37);  // XBee's address lookup falters on time.nist.gov
        _tinyClient->print(F("Hi!"));  // Need to send something before connection is made
        delay(100); // Need this delay!  Can get away with 50, but 100 is safer.
    }
    else connectionMade = openTCP("time.nist.gov", 37);

    // Wait up to 15 seconds for a response
    if (connectionMade)
    {
        long start = millis();
        while (_tinyClient->available() < 4 && millis() - start < 15000L){}

        if (_tinyClient->available() >= 4)
        {
            MS_MOD_DBG("\nNIST responded after ", millis() - start, " ms");
            // Response is returned as 32-bit number as soon as connection is made
            // Connection is then immediately closed, so there is no need to close it
            uint32_t secFrom1900 = 0;
            byte response[4] = {0};
            for (uint8_t i = 0; i < 4; i++)
            {
                response[i] = _tinyClient->read();
                MS_MOD_DBG("\nResponse Byte [", i, "]:",response[i], "(",
                           String(response[i], BIN),")");
                secFrom1900 += 0x000000FF & response[i];
                MS_MOD_DBG("\nseconds from 1900 after byte: ",String(secFrom1900, BIN));
                if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
            }
            MS_MOD_DBG("\nfinal seconds from 1900:  ",secFrom1900);

            // Close the TCP connection, just in case
            closeTCP();

            // Return the timestamp
            uint32_t unixTimeStamp = secFrom1900 - 2208988800;
            MS_MOD_DBG(F("\nUnix Timestamp returned by NIST (UTC): "), unixTimeStamp, '\n');
            // If before Jan 1, 2017 or after Jan 1, 2030, most likely an error
            if (unixTimeStamp < 1483228800) return 0;
            else if (unixTimeStamp > 1893456000) return 0;
            else return unixTimeStamp;
        }
        else
        {
            MS_MOD_DBG(F("NIST Time server did not respond!\n"));
            // Close the TCP connection, just in case
            closeTCP();
            return 0;
        }
    }
    else MS_MOD_DBG(F("Unable to open TCP to NIST!\n"));

    // Close the TCP connection, just in case
    closeTCP();
    return 0;
}

// Helper to get approximate RSSI from CSQ (assuming no noise)
int loggerModem::getRSSIFromCSQ(int csq)
{
    int CSQs[33]  = {  0,   1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 99};
    int RSSIs[33] = {113, 111, 109, 107, 105, 103, 101, 99, 97, 95, 93, 91, 89, 87, 85, 83, 81, 79, 77, 75, 73, 71, 69, 67, 65, 63, 61, 59, 57, 55, 53, 51, 0};
    for (int i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return RSSIs[i];
    }
    return 0;
}

// Helper to get signal percent from CSQ
int loggerModem::getPctFromCSQ(int csq)
{
    int CSQs[33] = {0, 1, 2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 99};
    int PCTs[33] = {0, 3, 6, 10, 13, 16, 19, 23, 26, 29, 32, 36, 39, 42, 45, 48, 52, 55, 58, 61, 65, 68, 71, 74, 78, 81, 84, 87, 90, 94, 97, 100, 0};
    for (int i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return PCTs[i];
    }
    return 0;
}

// Helper to get signal percent from CSQ
int loggerModem::getPctFromRSSI(int rssi)
{
    int pct = 1.6163*rssi + 182.61;
    if (rssi == 0) pct = 0;
    if (rssi == (255-93)) pct = 0;  // This is a no-data-yet value from XBee
    return pct;
}
