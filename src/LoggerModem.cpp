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
loggerModem::loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                         bool (*wakeFxn)(), bool (*sleepFxn)(),
                         TinyGsmModem *inModem, Client *inClient, const char *APN)
    : Sensor("Tiny GSM Modem", MODEM_NUM_VARIABLES,
             MODEM_WARM_UP_TIME_MS, MODEM_STABILIZATION_TIME_MS, MODEM_MEASUREMENT_TIME_MS,
             powerPin, -1, 1),
      _statusPin(statusPin),  _statusLevel(statusLevel), _indicatorTime_ms(0), _disconnetTime_ms(0),
      _apn(APN), _lastNISTrequest(0)
{
    _tinyModem = inModem;
    _tinyClient = inClient;
    _wakeFxn = wakeFxn;
    _sleepFxn = sleepFxn;
    _modemName = "unspecified modem";
}

loggerModem::loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                         bool (*wakeFxn)(), bool (*sleepFxn)(),
                         TinyGsmModem *inModem, Client *inClient, const char *ssid, const char *pwd)
    : Sensor("Tiny GSM Modem", MODEM_NUM_VARIABLES,
             MODEM_WARM_UP_TIME_MS, MODEM_STABILIZATION_TIME_MS, MODEM_MEASUREMENT_TIME_MS,
             powerPin, -1, 1),
      _statusPin(statusPin),  _statusLevel(statusLevel), _indicatorTime_ms(0), _disconnetTime_ms(0),
      _ssid(ssid), _pwd(pwd), _lastNISTrequest(0)
{
    _tinyModem = inModem;
    _tinyClient = inClient;
    _wakeFxn = wakeFxn;
    _sleepFxn = sleepFxn;
    _modemName = "unspecified modem";
}

String loggerModem::getSensorName(void) { return _modemName; }
String loggerModem::getSensorLocation(void) { return F("modemSerial"); }


// NOTE!! Power is left ON after set-up
bool loggerModem::setup(void)
{
    bool retVal = false;

    // Initialize the modem
    MS_MOD_DBG(F("Starting up the modem...\n"));

    if (_powerPin >= 0) pinMode(_powerPin, OUTPUT);
    if (_statusPin >= 0) pinMode(_statusPin, INPUT_PULLUP);

    // Turn the modem on, and if successful, begin
    if(_wakeFxn())
    {
        retVal = _tinyModem->begin();
        _modemName = _tinyModem->getModemName();
        MS_MOD_DBG(F("   ... Complete!\n"));
    }
    else MS_MOD_DBG(F("   ... Modem failed to turn on!\n"));

    // Set some warm-up times for specific models
    // NOTE:  These times are for raw cellular chips they do no necessarily
    // apply to assembled break-out boards or modules
    if (_modemName.indexOf("SIMCom SIM800") > 0)
    {
        _warmUpTime_ms = 450; // Time after power on before "PWRKEY" can be used
        _indicatorTime_ms = 2100;  // Time after end pulse until status pin becomes active (>3sec from start of pulse)
        _stabilizationTime_ms = 2100;  // Time after end pulse until serial port becomes active (>3sec from start of pulse)
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 600;  // 1sec > t > 3.3sec
        _disconnetTime_ms = 3100;  // power down (gracefully) takes >3sec
    }
    if (_modemName.indexOf("SIMCom SIM900") > 0)
    {
        _warmUpTime_ms = 1000; // Time after power on before "PWRKEY" can be used (guess - diagram isn't clear)
        _indicatorTime_ms = 2200;  // Time after end pulse until status pin becomes active (>2.2sec)
        _stabilizationTime_ms = 2200;  // Time after end pulse until serial port becomes active (>2.2sec)
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 600;  // 0.5sec > pull down > 1sec
        _disconnetTime_ms = 1800;  // power down (gracefully) takes >1.
    }
    if (_modemName.indexOf("SARA-R4") > 0  ||
        _modemName.indexOf("XBee3™ Cellular LTE-M") > 0  ||
        _modemName.indexOf("Digi XBee3™ Cellular NB-IoT") > 0)
    {
        _warmUpTime_ms = 250; // Time after power on before PWR_ON can be used ??? Unclear in diagram!
        _indicatorTime_ms = 0;  // V_INT becomes active mid-way through on-pulse
        _stabilizationTime_ms = 4600;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 200;  // 0.15-3.2s
        // _off_pull_down_ms = 1600;  // >1.5s
        _disconnetTime_ms = 4600;  // power down time is until Vint pin is reading low
    }
    if (_modemName.indexOf("SARA-U2") > 0  ||
        _modemName.indexOf("XBee® Cellular 3G") > 0)
    {
        _warmUpTime_ms = 0; // Module turns on when power is applied - level of PWR_ON then irrelevant
        _indicatorTime_ms = 250;  // Time after end pulse until V_INT becomes active ??? Unclear in diagram!
        _stabilizationTime_ms = 6000;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 1;  // 50-80µs
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 6000;  // power down time is until Vint pin is reading low
    }
    if (_modemName.indexOf("SARA-G3") > 0)
    {
        _warmUpTime_ms = 0; // Module turns on when power is applied - level of PWR_ON then irrelevant
        _indicatorTime_ms = 250;  // Time after end pulse until V_INT becomes active ??? Unclear in diagram!
        _stabilizationTime_ms = 5000;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 5;  // >5ms
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 6000;  // power down time is until Vint pin is reading low
    }
    if (_modemName.indexOf("LISA-U2") > 0)
    {
        _warmUpTime_ms = 0; // Module turns on when power is applied - level of PWR_ON then irrelevant
        _indicatorTime_ms = 35;  // Time after end pulse until V_INT becomes active <35ms
        _stabilizationTime_ms = 3000;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 1;  // 50-80µs
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 2600;  // power down (gracefully) takes ~2.5sec
    }
    if (_modemName.indexOf("Digi XBee® Cellular LTE Cat 1") > 0  ||
        _modemName.indexOf("Digi XBee3™ Cellular LTE CAT 1") > 0  ||
        _modemName.indexOf("Telit LE866") > 0)
    {
        _warmUpTime_ms = 0; // Module turns on when power is applied
        _indicatorTime_ms = 100;  // N/A? - No status pin on Telit chip
        _stabilizationTime_ms = 25000L;  // Wait with 25s time-out for first AT response
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 10000L;  // Wait with 10s time-out for sleep
    }
    if (_modemName.indexOf("ESP8266") > 0)
    {
        _warmUpTime_ms = 0; // Module turns on when power is applied
        _indicatorTime_ms = 0;  // N/A? - No status pin
        _stabilizationTime_ms = 200;
        // _on_pull_down_ms = 10;  // immediate
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 200;
        // power down ???
    }
    if (_modemName.indexOf("Neoway M590") > 0)
    {
        _warmUpTime_ms = 0; // ON/OFF pin should be held low as soon as power is applied
        _indicatorTime_ms = 300;  // Time after end pulse until status pin becomes active (?? not specified)
        _stabilizationTime_ms = 300;  // Time until UART is active
        // _on_pull_down_ms = 510;  // >300ms (>500ms recommended)
        // _off_pull_down_ms = 510;  // >300ms
        _disconnetTime_ms = 5000;  // power down (gracefully) takes ~5sec
    }
    if (_modemName.indexOf("Quectel BG96") > 0)
    {
        _warmUpTime_ms = 30; // Time after VBAT is stable before PWRKEY can be used
        _indicatorTime_ms = 4800;  // Time after end pulse until status pin becomes active
        _stabilizationTime_ms = 4900;  // USB active at >4.2 sec, status at >4.8 sec, URAT at >4.9
        // _on_pull_down_ms = 110;  // >100ms
        // _off_pull_down_ms = 700;  // ≥ 650ms
        _disconnetTime_ms = 2000;  // > 2 sec
    }
    if (_modemName.indexOf("Quectel M95") > 0)
    {
        _warmUpTime_ms = 30; // Time after VBAT is stable before PWRKEY can be used
        _indicatorTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 0;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 2000;  // until either status key goes on, or > 1.0 sec (~2s)
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 2000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf("Quectel MC60") > 0)
    {
        _warmUpTime_ms = 100; // Time after VBAT is stable before PWRKEY can be used
        _indicatorTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 0;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 2000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf("AI-Thinker A") > 0)
    {
        _warmUpTime_ms = 0; //??
        _indicatorTime_ms = 0;  // ??
        _stabilizationTime_ms = 0;  // ??
        // _on_pull_down_ms = 2000;  // >2s
        // _off_pull_down_ms = 0;  // ??
        _disconnetTime_ms = 0;  // ??
    }

    // Set the status bit marking that the modem has been set up (bit 1)
    _sensorStatus |= 0b00000010;

    return retVal;
}


// The function to wake up a sensor
// There are so many ways to wake a modem that we're requiring an input function
bool loggerModem::wake(void)
{
    // Try the given wake function up to 5 times
    int ntries = 0;
    bool success = false;
    while (!success && ntries < 5)
    {
        MS_MOD_DBG(F("Waking "), getSensorName(), F("("), ntries+1, F("): "));
        success = _wakeFxn();
        ntries++;
    }
    if(success)
    {
        // Mark the time that the sensor was activated
        _millisSensorActivated = millis();
        // Set the status bit for sensor activation (bit 3)
        _sensorStatus |= 0b00001000;
        MS_DBG(F("Sensor activated and measuring.\n"));
    }
    else
    {
        // Make sure the activation time is not set
        _millisSensorActivated = 0;
        // Make sure the status bit for sensor activation (bit 3) is not set
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Sensor NOT activated!\n"));
    }
    return success;
}


// Do NOT turn the modem on and off with the regular power up and down!
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
    bool success = true;

    // Initialize float variable
    int signalQual = 0;
    int percent = -9999;
    int rssi = -9999;

    if (_millisMeasurementRequested > 0)
    {
        // The XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // Connecting to the NIST daytime server, which immediately returns a
        // 4 byte response and then closes the connection
        if (_modemName.indexOf("XBee") > 0)
        {
            // Must ensure that we do not ping the daylight more than once every 4 seconds
            // NIST clearly specifies here that this is a requirement for all software
            /// that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
            while (millis() < _lastNISTrequest + 4000) {}
            MS_MOD_DBG(F("Connecting to NIST daytime server to check connection strength...\n"));
            IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
            success &= openTCP(ip, 37);
            _tinyClient->print(F("Hi!"));  // Need to send something before connection is made
            delay(100); // Need this delay!  Can get away with 50, but 100 is safer.
            while (_tinyClient->available()) _tinyClient->read();  // Delete anything returned
            _lastNISTrequest = millis();
        }

        // Get signal quality
        MS_MOD_DBG(F("Getting signal quality:\n"));
        signalQual = _tinyModem->getSignalQuality();

        // Convert signal quality to RSSI, if necessary
        if (_modemName.indexOf("XBee") > 0 ||
            _modemName.indexOf("ESP8266") > 0)
        {
            rssi = signalQual;
            percent = getPctFromRSSI(signalQual);
        }
        else
        {
            rssi = getRSSIFromCSQ(signalQual);
            percent = getPctFromCSQ(signalQual);
        }

        MS_MOD_DBG(F("RSSI: "), rssi, F("\n"));
        MS_MOD_DBG(F("Percent signal strength: "), percent, F("\n"));
    }
    else MS_DBG(F("MModem is not connected to network; unable to get signal quality!\n"));

    verifyAndAddMeasurementResult(RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(PERCENT_SIGNAL_VAR_NUM, percent);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    return success;
}


// This checks to see if enough time has passed for stability
// In the case of the modem, we consider it to be "stable" when it responds to
// AT commands
bool loggerModem::isStable(bool debug)
{
    uint32_t elapsed_since_wake_up = millis() - _millisSensorActivated;

    // If the modem never turned on, then it will never respond and thus it's
    // essentially already "stable."
    if (!bitRead(_sensorStatus, 3))
    {
        if (debug) MS_MOD_DBG(getSensorName(),
                F(" is not on and AT commands will not be attempted!\n"));
        // Set the status bit for stability completion (bit 4)
        _sensorStatus |= 0b00010000;
        return true;
    }
    // If the modem has a status pin and it's off, give up
    else if ( (_statusPin >= 0 && elapsed_since_wake_up > _indicatorTime_ms &&
              digitalRead(_statusPin) != _statusLevel))
    {
        if (debug) MS_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and status pin on "),
              getSensorName(), F(" indicates it is off.  AT commands will not be attempted!\n"));
        // Set the status bit for warm-up completion (bit 2)
        _sensorStatus |= 0b00000100;
        return true;
    }
    // If the modem is now responding to AT commands, it's "stable"
    else if (_tinyModem->testAT(100))
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" is now responding to AT commands!\n"));

        // For the wifi modems, the SSID and password need to be set before they
        // can join a network.  We'll do this as soon as it responds to commands.
        // For cellular modems, network registration happens automatically.  The
        // GPRS bearer (APN) is then set after registration.
        if (_ssid && _tinyModem->hasWifi())
        {
            _tinyModem->networkConnect(_ssid, _pwd);
        }

        // Set the status bit for stability completion (bit 4)
        _sensorStatus |= 0b00010000;
        return true;
    }
    // If we've exceeded the time-out, give up
    else if (elapsed_since_wake_up > MODEM_MAX_REPLY_TIME)
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" has maxed out wait for AT command reply!  Ending wait.\n"));

        // Set the status bit for stability completion (bit 4)
        _sensorStatus |= 0b00010000;
        return true;
    }
    // If the modem isn't responding to AT commands yet, we still need to wait
    else
    {
        // Make sure the status bits for stability (bit 4), measurement
        // start (bit 5) and measurement completion (bit 6) are not set
        _sensorStatus &= 0b10001111;
        return false;
    }
}


// This checks to see if enough time has passed for measurement completion
// In the case of the modem, we consider a measurement to be "complete" when
// the modem has registered on the network
bool loggerModem::isMeasurementComplete(bool debug)
{
    uint32_t elapsed_since_wake_up = millis() - _millisSensorActivated;

    // If the modem never responded to AT commands, we'll never get a
    // signal strength, so the measurement is essentially "done."
    if (!bitRead(_sensorStatus, 5))
    {
        if (debug) MS_DBG(getSensorName(),
               F(" is not responding to AT commands and will not give signal strength!\n"));
        // Set the status bit for measurement completion (bit 6)
        _sensorStatus |= 0b01000000;
        return true;
    }
    // If the modem is registered on the network, it's "stable"
    else if (_tinyModem->isNetworkConnected())
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" is now registered on the network!\n"));

        // For the cellular modems, the APN must be set after registration
        // on the network.  We'll do this as soon as registration is complete.
        if (_ssid && _tinyModem->hasWifi())
        {
            _tinyModem->gprsConnect(_apn, "", "");
        }

        // Set the status bit for measurement completion (bit 6)
        _sensorStatus |= 0b01000000;
        return true;
    }
    // If we've exceeded the time-out, give up
    else if (elapsed_since_wake_up > MODEM_MAX_SEARCH_TIME)
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" has not yet registered on the network!  Ending wait.\n"));

        // Set the status bit for stability completion (bit 4)
        _sensorStatus |= 0b00010000;
        return true;
    }
    // If the modem isn't registered yet, we still need to wait
    else
    {
        // Make sure the status bit for measurement completion (bit 6) is not set
        _sensorStatus &= 0b10111111;
        return false;
    }
}


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//
bool loggerModem::connectInternet(uint32_t waitTime_ms)
{
    bool retVal = false;

    if (bitRead(getStatus(), 0) == 0)  // NOT yet powered
        modemPowerUp();

    if (bitRead(getStatus(), 0) == 3)  // NOT yet awake/actively measuring
    {
        waitForWarmUp();
        wake();  // This sets the modem to on
    }

    // Check that the modem is responding to AT commands.  If not, give up.
    uint32_t start = millis();
    MS_MOD_DBG(F("\nWaiting up to 5 seconds for modem to respond to AT commands...\n"));
    if (!_tinyModem->testAT(5000))
    {
        MS_MOD_DBG(F("\nModem does not respond to AT commands!\n"));
        return false;
    }
    else MS_MOD_DBG(F("   ... AT OK after "), millis() - start, F(" milliseconds!\n"));

    if (_ssid)
    {
        MS_MOD_DBG(F("\nAttempting to connect to WiFi network...\n"));
        if (!(_tinyModem->isNetworkConnected()))
        {
            MS_MOD_DBG(F("   Sending credentials...\n"));
            while (!_tinyModem->networkConnect(_ssid, _pwd)) {};
            MS_MOD_DBG(F("   Waiting up to "), waitTime_ms/1000,
                       F(" seconds for connection\n"));
            if (_tinyModem->waitForNetwork(waitTime_ms))
            {
                retVal = true;
                MS_MOD_DBG(F("   ... WiFi connected after "), millis() - start,
                           F(" milliseconds!\n"));
            }
            else MS_MOD_DBG(F("   ... WiFi connection failed\n"));
        }
        else
        {
            MS_MOD_DBG(F("   ... Connected with saved WiFi settings!\n"));
            retVal = true;
        }

    }
    else
    {
        MS_MOD_DBG(F("\nWaiting up to "), waitTime_ms/1000,
                   F(" seconds for cellular network registration...\n"));
        if (_tinyModem->waitForNetwork(waitTime_ms))
        {
            MS_MOD_DBG(F("   ... Registered after "), millis() - start,
                       F(" milliseconds.  Connecting to GPRS...\n"));
            _tinyModem->gprsConnect(_apn, "", "");
            MS_MOD_DBG(F("   ... Connected after "), millis() - start,
                       F(" milliseconds.\n"));
            retVal = true;
        }
        else MS_MOD_DBG(F("   ...Connection failed.\n"));
    }
    return retVal;
}


void loggerModem::disconnectInternet(void)
{
    if (_tinyModem->hasGPRS())
    {
        _tinyModem->gprsDisconnect();
        MS_MOD_DBG(F("Disconnected from cellular network.\n"));
    }
    else
    {
        _tinyModem->networkDisconnect();
        MS_MOD_DBG(F("Disconnected from WiFi network.\n"));
    }
    _millisSensorActivated = 0;
}


int loggerModem::openTCP(const char *host, uint16_t port)
{
    MS_MOD_DBG(F("Connecting to "), host, F("..."));
    int ret_val = _tinyClient->connect(host, port);
    if (ret_val) MS_MOD_DBG(F("   ...Success!\n"));
    else MS_MOD_DBG(F("   ...Connection failed.\n"));
    return ret_val;
}


int loggerModem::openTCP(IPAddress ip, uint16_t port)
{
    MS_MOD_DBG(F("Connecting to "), ip, F("..."));
    int ret_val = _tinyClient->connect(ip, port);
    if (ret_val) MS_MOD_DBG(F("   ...Success!\n"));
    else MS_MOD_DBG(F("   ...Connection failed.\n"));
    return ret_val;
}


void loggerModem::closeTCP(void)
{
    _tinyClient->stop();
    MS_MOD_DBG(F("Closed TCP/IP.\n"));
}


void loggerModem::modemPowerUp(void)
{
    if (_powerPin >= 0)
    {
        MS_DBG(F("Powering "), getSensorName(), F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    else
    {
        MS_DBG(F("Power to "), getSensorName(), F(" is not controlled by this library.\n"));
    }
    // Set the status bit for sensor power (bit 0)
    _sensorStatus |= 0b00000001;
}


bool loggerModem::modemSleepPowerDown(void)
{
    bool success = true;

    MS_MOD_DBG(F("Turning modem off.\n"));
     // Wait for any sending to complete
    _tinyClient->flush();
    uint32_t start = millis();

    // If there's a status pin available, check before running the sleep function
    if (_statusPin >= 0 && _statusPin != _statusLevel)
        MS_MOD_DBG(F("Modem appears to have already been off.\n"));
    else
    {
        // Run the sleep function
        MS_MOD_DBG(F("Running sleep function.\n"));
        success &= _sleepFxn();
    }

    // If there's a status pin available, wait until modem shows it's ready to be powered off
    // This allows the modem to shut down gracefully.
    MS_MOD_DBG(F("Waiting for graceful shutdown...\n"));
    if (_statusPin >= 0)
    {
        while (millis() - start < 10000L && _statusPin == _statusLevel){}
        if (_statusPin == _statusLevel)
            MS_MOD_DBG(F("  ... Modem did not successfully shut down!\n"));
        else MS_MOD_DBG(F("  ... shutdown complete after "), millis() - start, F("ms.\n"));
    }
    else if (_disconnetTime_ms > 0)
    {
        while (millis() - start < _disconnetTime_ms){}
        MS_MOD_DBG(F("  ... assumed to be complete.\n"));
    }

    // Unset the activation time
    _millisSensorActivated = 0;
    // Unset the activated status bit (bit 3), stability (bit 4), measeurement
    // request (bit 5) and measurement completion (bit 6)
    _sensorStatus &= 0b10000111;

    // Now power down
    if (_powerPin >= 0)
    {
        MS_DBG(F("Turning off power to "), getSensorName(), F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    else
    {
        MS_DBG(F("Power to "), getSensorName(), F(" is not controlled by this library.\n"));
    }
    // Unset the status bits for sensor power (bit 0), warm-up (bit 2),
    // activation (bit 3), stability (bit 4), measurement request (bit 5), and
    // measurement completion (bit 6)
    _sensorStatus &= 0b10000010;

    return success;
}

// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
uint32_t loggerModem::getNISTTime(void)
{
    // bail if not connected to the internet
    if ( (_tinyModem->hasGPRS() && !_tinyModem->isGprsConnected()) ||
         (_tinyModem->hasWifi() && !_tinyModem->isNetworkConnected()) )
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
    bool connectionMade = false;
    if (_modemName.indexOf("XBee") > 0)
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
            MS_MOD_DBG(F("\nNIST responded after "), millis() - start, F(" ms"));
            // Response is returned as 32-bit number as soon as connection is made
            // Connection is then immediately closed, so there is no need to close it
            uint32_t secFrom1900 = 0;
            byte response[4] = {0};
            for (uint8_t i = 0; i < 4; i++)
            {
                response[i] = _tinyClient->read();
                MS_MOD_DBG(F("\nResponse Byte ["), i, F("]:"),response[i], '(',
                           String(response[i], BIN),')');
                secFrom1900 += 0x000000FF & response[i];
                MS_MOD_DBG(F("\nseconds from 1900 after byte: "),String(secFrom1900, BIN));
                if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
            }
            MS_MOD_DBG(F("\nfinal seconds from 1900:  "),secFrom1900);

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

// Helper to get signal percent from RSSI
int loggerModem::getPctFromRSSI(int rssi)
{
    int pct = 1.6163*rssi + 182.61;
    if (rssi == 0) pct = 0;
    if (rssi == (255-93)) pct = 0;  // This is a no-data-yet value from XBee
    return pct;
}