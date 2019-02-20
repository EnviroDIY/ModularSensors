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
                         bool (*modemWakeFxn)(), bool (*modemSleepFxn)(),
                         TinyGsmModem *inModem, Client *inClient, const char *APN)
    : Sensor("Tiny GSM Modem", MODEM_NUM_VARIABLES,
             MODEM_WARM_UP_TIME_MS, MODEM_ATRESPONSE_TIME_MS, MODEM_MAX_SEARCH_TIME,
             powerPin, statusPin, 1),
      _statusLevel(statusLevel), _statusTime_ms(MODEM_STATUS_TIME_MS),
      _disconnetTime_ms(MODEM_DISCONNECT_TIME_MS),
      _apn(APN), _ssid(NULL), _pwd(NULL), _lastNISTrequest(0)
{
    _tinyModem = inModem;
    _tinyClient = inClient;
    _modemWakeFxn = modemWakeFxn;
    _modemSleepFxn = modemSleepFxn;
    _modemName = "unspecified modem";
    _lastConnectionCheck = 0;
    _lastATCheck = 0;
    _modemSleepRqPin = -1;
}
loggerModem::loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                         bool (*modemWakeFxn)(), bool (*modemSleepFxn)(),
                         TinyGsmModem *inModem, Client *inClient, const char *ssid, const char *pwd)
    : Sensor("Tiny GSM Modem", MODEM_NUM_VARIABLES,
             MODEM_WARM_UP_TIME_MS, MODEM_ATRESPONSE_TIME_MS, MODEM_MAX_SEARCH_TIME,
             powerPin, statusPin, 1),
      _statusLevel(statusLevel), _statusTime_ms(MODEM_STATUS_TIME_MS),
      _disconnetTime_ms(MODEM_DISCONNECT_TIME_MS),
      _apn(NULL), _ssid(ssid), _pwd(pwd), _lastNISTrequest(0)
{
    _tinyModem = inModem;
    _tinyClient = inClient;
    _modemWakeFxn = modemWakeFxn;
    _modemSleepFxn = modemSleepFxn;
    _modemName = "unspecified modem";
    _lastConnectionCheck = 0;
    _lastATCheck = 0;
    _modemSleepRqPin = -1;
}


// Destructor
loggerModem::~loggerModem(){}


String loggerModem::getSensorName(void) { return _modemName; }


// NOTE: Unlike other setup functions, this will NOT turn the power to the modem
// on and off, it will simply check if the power has already been turned on and
// return quickly if not.  If the power had been on, it is left ON after set-up.
bool loggerModem::setup(void)
{
    bool success = Sensor::setup();  // this will set pin modes and the setup status bit

    // Initialize the modem
    MS_MOD_DBG(F("Setting up the modem ..."));

    // Bail if there's no power
    if (!checkPowerOn())
    {
        MS_MOD_DBG(F("Modem doesn't have power and cannot be set-up!"));
        return false;
    }

    // Check if the modem was awake, wake it if not
    // NOTE:  We ar NOT powering up the modem!  Set up will NOT be successful
    // unless the modem is already powered external to this function.
    bool wasAwake = ( (_dataPin >= 0 && digitalRead(_dataPin) == _statusLevel)
                     || bitRead(_sensorStatus, 4) );
    if (!wasAwake)
    {
        waitForWarmUp();
        MS_MOD_DBG(F("Running given modem wake function ..."));
        success &= _modemWakeFxn();
        // NOTE:  not setting wake bits here because we'll go back to sleep
        // before the end of this function if we weren't awake
    }
    else MS_MOD_DBG(F("Modem was already awake."));

    if (success)
    {
        // The begin() generally starts with a 5 second testAT(), that should
        // be enough time to allow any modem to be ready to respond
        MS_MOD_DBG(F("Running modem's begin function ..."));
        success &= _tinyModem->begin();
        _modemName = _tinyModem->getModemName();
        if (success) MS_MOD_DBG(F("... Complete!  It's a"), getSensorName());
        else MS_MOD_DBG(F("... Failed!  It's a"), getSensorName());
    }
    else MS_MOD_DBG(F("... "), getSensorName(), F("did not wake up and cannot be set up!"));

    // Set the timing for modems based on their names
    // NOTE:  These are based on documentation for the raw chip!
    setModemTiming();

    MS_MOD_DBG(_modemName, F("warms up in"), _warmUpTime_ms, F("ms, indicates status in"),
           _statusTime_ms, F("ms, is responsive to AT commands in less than"),
           _stabilizationTime_ms, F("ms, and takes up to"), _disconnetTime_ms,
           F("ms to close connections and shut down."));

    // XBee saves all configurations to flash, so we can set them here
    if (_modemName.indexOf(F("XBee")) >= 0)
    {
       MS_MOD_DBG(F("Putting connection values into flash memory for the Digi XBee"));
       if (_tinyModem->hasWifi()) _tinyModem->networkConnect(_ssid, _pwd);
       else _tinyModem->gprsConnect(_apn, "", "");
    }

    // Print out some warnings if things seem to be improperly formatted
    if (_tinyModem->hasWifi() && _ssid == NULL)
        MS_MOD_DBG(F("WARNING:  Wifi modem with no SSID given!"));
     if (_tinyModem->hasGPRS() && _apn == NULL)
         MS_MOD_DBG(F("WARNING:  Cellular modem with no APN given!"));

    // Set the status bit marking that the modem has been set up (bit 0)
    // Only set the bit if setup was successful!
    if (success) _sensorStatus |= 0b00000001;
    // Otherwise, set the status error bit (bit 7)
    else _sensorStatus |= 0b10000000;

    // Put the modem to sleep after finishing setup
    // Only go to sleep if it had been asleep and is now awake
    bool isAwake = ( (_dataPin >= 0 && digitalRead(_dataPin) == _statusLevel)
                    || bitRead(_sensorStatus, 4) );
    if (!wasAwake && isAwake)
    {
        // Run the sleep function
        MS_MOD_DBG(F("Running given modem sleep function ..."));
        success &= _modemSleepFxn();
    }
    else MS_MOD_DBG(F("Leaving modem on after setup ..."));
    // Do NOT power down at the end, because this fxn cannot have powered the
    // modem up.

    return success;
}


// The function to wake up the modem
// There are so many ways to wake a modem that we're requiring an input function
bool loggerModem::wake(void)
{
    bool success = true;

    MS_MOD_DBG(F("Waking"), getSensorName());
    // Set the status bit for sensor activation attempt (bit 3)
    // Setting this bit even if the activation failed, to show the attempt was made
    _sensorStatus |= 0b00001000;

    // Check if the sensor was successfully powered
    if (!bitRead(_sensorStatus, 2))
    {
        MS_MOD_DBG(getSensorName(), F("doesn't have power and will never wake up!"));
        // Make sure that the wake time and wake success bit (bit 4) are unset
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
        return false;
    }

    // Check the status pin and wake bits before running wake function
    // Don't want to accidently pulse an already on modem to off
    if ( bitRead(_sensorStatus, 4))
    {
        MS_MOD_DBG(getSensorName(), F("has already been woken up!  Will not run wake function."));
        success = true;
    }
    // NOTE:  It's possible that the status pin is on, but the modem is actually
    // mid-shutdown.  In that case, we'll mistakenly skip re-waking it.
    else if (_dataPin >= 0 && digitalRead(_dataPin) == _statusLevel)
    {
        MS_MOD_DBG(getSensorName(), F("was already on!  Will not run wake function."));
        success = true;
    }
    else
    {
        waitForWarmUp();
        // Run the input wake function
        MS_MOD_DBG(F("Running wake function for"), getSensorName());
        success &= _modemWakeFxn();
    }

    // Re-check the status pin
    // Only works if the status pin comes on immediately
    if (_dataPin > 0 && _statusTime_ms == 0 && digitalRead(_dataPin) != _statusLevel)
    {
        MS_MOD_DBG(F("Status pin on"), getSensorName(), F("is"),
                   digitalRead(_dataPin), F("indicating it is off!"));
        // Make sure that the wake time and wake success bit (bit 4) are unset
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
        return false;
    }

    if (success)
    {
        // Mark the time that the modem woke up
        // NOTE:  This is the ONLY place _millisSensorActivated is set!
        if (_millisSensorActivated == 0) _millisSensorActivated = millis();
        // Set the status bit for sensor wake/activation success (bit 4)
        // NOTE:  This is the ONLY place bit 4 is set!
        _sensorStatus |= 0b00010000;
        MS_MOD_DBG(getSensorName(), F("should be awake."));
    }

    // check if the modem was successfully set up, run set up if not
    if (!bitRead(_sensorStatus, 0))
    {
        MS_MOD_DBG(getSensorName(), F("was never properly set up, attempting setup now!"));
        setup();
    }

    return success;
}


// Do NOT turn the modem on and off with the regular power up and down!
// This is because when it is run in an array with other sensors, we will
// generally want the modem to remain on after all the other sensors have
// gone to sleep and powered down so the modem can send out data
void loggerModem::powerUp(void)
{
    MS_MOD_DBG(F("Skipping"), getSensorName(), F("in sensor power up!"));
}
void loggerModem::powerDown(void)
{
    MS_MOD_DBG(F("Skipping"), getSensorName(), F("in sensor power down!"));
}


bool loggerModem::startSingleMeasurement(void)
{
    bool success = true;
    MS_MOD_DBG(F("Starting measurement on"), getSensorName());
    // Set the status bits for measurement requested (bit 5)
    // Setting this bit even if we failed to start a measurement to show that an attempt was made.
    _sensorStatus |= 0b00100000;

    // Check if the modem was successfully awoken (bit 4 set)
    // Only mark the measurement request time if it is
    if (bitRead(_sensorStatus, 4))
    {
        // For the wifi modems, the SSID and password need to be set before they
        // can join a network.
        // For **MOST** cellular modems, network registration (should) happen automatically.
        // The GPRS bearer (APN) is then set after registration when making the GPRS
        // (data) link.
        // For XBee models, the SSID, password, and APN are always saved in
        // the board's memory, even if power is disconnected, so those values
        // are set in the setup function.
        if (_ssid && _tinyModem->hasWifi() && !_tinyModem->isNetworkConnected() &&
            _modemName.indexOf(F("XBee")) < 0 )
        {
            success &= _tinyModem->networkConnect(_ssid, _pwd);
        }

        // Mark the time that a measurement was requested
        _millisMeasurementRequested = millis();
        // Set the status bit for measurement start success (bit 6)
        _sensorStatus |= 0b01000000;
    }
    // Otherwise, make sure that the measurement start time and success bit (bit 6) are unset
    else
    {
        MS_MOD_DBG(getSensorNameAndLocation(),
               F("isn't awake/active!  A measurement cannot be started."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
        success = false;
    }
    return success;
}


bool loggerModem::addSingleMeasurementResult(void)
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
        // Connecting to the NIST daytime server, which immediately returns a
        // 4 byte response and then closes the connection
        if (_modemName.indexOf(F("XBee")) >= 0  && _tinyModem->hasWifi())
        {
            MS_MOD_DBG(F("Connecting to NIST daytime server to check connection strength..."));
            IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
            success &= _tinyClient->connect(ip, 37);
            _tinyClient->print('!');  // Need to send something before connection is made
            delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
            char junkBuff[5];
            _tinyClient->readBytes(junkBuff, 4);  // Dump the returned bytes
            // This should ensure we don't wait for more than 4 character time outs
            _lastNISTrequest = millis();
            _tinyClient->stop();  // NIST will close on it's side, but need to stop
            // because otherwise the Bee won't realize the socket has closed
        }

        // Get signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no service/signal.
        // The TinyGSM getSignalQuality function returns the same "no signal"
        // value (99 CSQ or 0 RSSI) in all 3 cases.
        MS_MOD_DBG(F("Getting signal quality:"));
        signalQual = _tinyModem->getSignalQuality();
        MS_MOD_DBG(F("Raw signal quality:"), signalQual);

        // Convert signal quality to RSSI, if necessary
        if ((_modemName.indexOf(F("XBee")) >= 0 || _modemName.indexOf(F("ESP8266")) >= 0))
        {
            rssi = signalQual;
            percent = getPctFromRSSI(signalQual);
        }
        else
        {
            rssi = getRSSIFromCSQ(signalQual);
            percent = getPctFromCSQ(signalQual);
        }

        MS_MOD_DBG(F("RSSI:"), rssi);
        MS_MOD_DBG(F("Percent signal strength:"), percent);
    }
    else MS_MOD_DBG(getSensorName(), F("is not connected to the network; unable to get signal quality!"));

    verifyAndAddMeasurementResult(RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(PERCENT_SIGNAL_VAR_NUM, percent);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}


// This checks to see if enough time has passed for stability
// In the case of the modem, we consider it to be "stable" when it responds to
// AT commands
bool loggerModem::isStable(bool debug)
{
    // If the modem never "woke", then it will never respond and thus it's
    // essentially already "stable."
    if (!bitRead(_sensorStatus, 4))
    {
        if (debug) MS_MOD_DBG(getSensorName(),
                F("did not wake; AT commands will not be attempted!"));
        return true;
    }

    // just defining this to not call multiple times below
    uint32_t now = millis();

    uint32_t elapsed_since_wake_up = now - _millisSensorActivated;
    // If the modem has a status pin and it's off, give up
    if ( (_dataPin >= 0 && elapsed_since_wake_up > _statusTime_ms &&
              digitalRead(_dataPin) != _statusLevel))
    {
        if (debug) MS_MOD_DBG(F("It's been"), (elapsed_since_wake_up), F("ms, and status pin on"),
              getSensorName(), F("is"), digitalRead(_dataPin),
              F("indicating it is off.  AT commands will not be attempted!"));
        // Unset status bit 4 (wake up success) and _millisSensorActivated
        // We unset these bits here because it's possible that a modem "passed"
        // the wake command, but never really woke.  For sensors that take time
        // before their status pin becomes active, (_statusTime_ms > 0) we
        // don't know immediately that they failed to wake
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
        return true;
    }

    // We don't want to ping any of the modems too fast so they don't get
    // overwhelmed.  Make sure we've waited a little
    if (now - _lastATCheck < 250) return false;

    // If the modem is now responding to AT commands, it's "stable"
    if (_tinyModem->testAT(10))
    {
        if (debug) MS_MOD_DBG(F("It's been"), (elapsed_since_wake_up), F("ms, and"),
               getSensorName(), F("is now responding to AT commands!"));
        _lastATCheck = now;
        return true;
    }

    // If we've exceeded the documented time until UART should respond (plus 500ms buffer), give up
    if (elapsed_since_wake_up > (_stabilizationTime_ms + 500))
    {
        if (debug) MS_MOD_DBG(F("It's been"), (elapsed_since_wake_up), F("ms, and"),
               getSensorName(), F("has maxed out wait for AT command reply!  Ending wait."));
         // Unset status bit 4 (wake up success) and _millisSensorActivated
         // It's safe to unset these here because we've already tested and failed
         // to respond to AT commands.
         _millisSensorActivated = 0;
         _sensorStatus &= 0b11101111;
        return true;
    }

    // If the modem isn't responding to AT commands yet, but its status pin shows
    // it's on and we haven't maxed out the response time, we still need to wait
    _lastATCheck = now;
    return false;
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
bool loggerModem::isMeasurementComplete(bool debug)
{
    // If a measurement failed to start, the sensor will never return a result,
    // so the measurement time is essentially already passed
    // For a cellular modem nothing happens to "start" a measurement so bit 6
    // will be set by startSingleMeasurement() as long as bit 4 was set by wake().
    // For a WiFi modem, startSingleMeasurement actually sets the WiFi connection
    // parameters.
    if (!bitRead(_sensorStatus, 6))
    {
        if (debug) {MS_MOD_DBG(getSensorName(),
            F("is not measuring and will not return a value!"));}
        return true;
    }

    // just defining this to not call multiple times below
    uint32_t now = millis();

    // For Wifi XBee's, we need to open a socket before we get signal strength
    // For ease, we will be opening that socket to NIST in addSingleMeasurementResult.
    // Because NIST very clearly specifies that we may not ping more frequently
    // than once every 4 seconds, we need to wait if it hasn't been that long.
    // See:  https://tf.nist.gov/tf-cgi/servers.cgi
    if (_modemName.indexOf(F("XBee")) >= 0  && _tinyModem->hasWifi()
        && now - _lastNISTrequest < 4000)
    {
        return false;
    }

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
    if (_tinyModem->hasGPRS() || _modemName.indexOf(F("XBee")) >= 0)
        elapsed_in_wait = now - _millisSensorActivated;

    // For Wifi modems without settings in flash, the connection parameters
    // need to set before it can register to the network - that is done in the
    // startSingleMeasurement() function and becomes the measurement request time.
    else elapsed_in_wait = now - _millisMeasurementRequested;

    // If we're connected AND receiving valid signal strength, measurement is complete
    bool isConnected = _tinyModem->isNetworkConnected();
    int signalResponse = _tinyModem->getSignalQuality();
    if (isConnected && signalResponse != 0 && signalResponse != 99)
    {
        if (debug) MS_MOD_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"),
               getSensorName(), F("is now registered on the network and reporting valid signal strength!"));
        _lastConnectionCheck = now;
        return true;
    }

    // If we've exceeded the allowed time to wait for the network, give up
    if (elapsed_in_wait > MODEM_MAX_SEARCH_TIME)
    {
        if (debug) MS_MOD_DBG(F("It's been"), (elapsed_in_wait), F("ms, and"),
               getSensorName(), F("has maxed out wait for network registration!  Ending wait."));
         // Leave status bits and times set - can still get a valid value!
        return true;
    }

    // If the modem isn't registered yet or doesn't report valid signal, we still need to wait
    _lastConnectionCheck = now;
    return false;
}


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//
bool loggerModem::connectInternet(uint32_t waitTime_ms)
{
    bool retVal = true;

    if (bitRead(_sensorStatus, 1) == 0 || bitRead(_sensorStatus, 2) == 0)  // NOT yet powered
        modemPowerUp();

    if (bitRead(_sensorStatus, 3) == 0)  // No attempts yet to wake the modem
    {
        waitForWarmUp();
        retVal &= wake();  // This sets the modem to on, will also set-up if necessary
    }
    if (!retVal)
    {
        MS_MOD_DBG(F("Modem did't wake up! Cannot connect to the internet!"));
        return retVal;
    }

    // Check that the modem is responding to AT commands.  If not, give up.
    #ifdef MODEM_DEBUGGING_SERIAL_OUTPUT
        uint32_t start = millis();
    #endif
    MS_MOD_DBG(F("\nWaiting for"), getSensorName(), F("to respond to AT commands..."));
    if (!_tinyModem->testAT(_stabilizationTime_ms + 500))
    {
        MS_MOD_DBG(F("No response to AT commands! Cannot connect to the internet!"));
        return false;
    }
    else MS_MOD_DBG(F("... AT OK after"), millis() - start, F("milliseconds!"));

    if (_tinyModem->hasWifi())
    {
        MS_MOD_DBG(F("\nAttempting to connect to WiFi network..."));
        if (!(_tinyModem->isNetworkConnected()))
        {
            MS_MOD_DBG(F("Sending credentials..."));
            while (!_tinyModem->networkConnect(_ssid, _pwd)) {};
            MS_MOD_DBG(F("Waiting up to"), waitTime_ms/1000,
                       F("seconds for connection"));
            if (!_tinyModem->waitForNetwork(waitTime_ms))
            {
                MS_MOD_DBG(F("... WiFi connection failed"));
                return false;
            }
        }
        MS_MOD_DBG(F("... WiFi connected after"), millis() - start,
                   F("milliseconds!"));
        return true;
    }
    else  // must be GPRS
    {
        MS_MOD_DBG(F("\nWaiting up to"), waitTime_ms/1000,
                   F("seconds for cellular network registration..."));
        if (_tinyModem->waitForNetwork(waitTime_ms))
        {
            MS_MOD_DBG(F("... Registered after"), millis() - start,
                       F("milliseconds.  Connecting to GPRS..."));
            _tinyModem->gprsConnect(_apn, "", "");
            MS_MOD_DBG(F("... Connected after"), millis() - start,
                       F("milliseconds."));
            retVal = true;
        }
        else
        {
            MS_MOD_DBG(F("...GPRS connection failed."));
            return false;
        }
    }
    return retVal;
}


void loggerModem::disconnectInternet(void)
{
    #ifdef MODEM_DEBUGGING_SERIAL_OUTPUT
        uint32_t start = millis();
    #endif
    if (_tinyModem->hasGPRS() && _modemName.indexOf(F("XBee")) < 0)  // XBee doesn't like to disconnect
    {
        _tinyModem->gprsDisconnect();
        MS_MOD_DBG(F("Disconnected from cellular network after"), millis() - start,
                   F("milliseconds."));
    }
    else if (_modemName.indexOf(F("XBee")) < 0)  // XBee doesn't like to disconnect
    // If you tell the XBee to disconnect, it will not reconnect to the same
    // access point until it has been restarted or powered on and off.
    // Since we may not have control of the power off, we just won't disconnect.
    {
        _tinyModem->networkDisconnect();
        MS_MOD_DBG(F("Disconnected from WiFi network after"), millis() - start,
                   F("milliseconds."));
    }
}


/***
int16_t loggerModem::openTCP(const char *host, uint16_t port)
{
    MS_MOD_DBG(F("Connecting to"), host, F("..."));
    int16_t ret_val = _tinyClient->connect(host, port);
    if (ret_val) MS_MOD_DBG(F("...Success!"));
    else MS_MOD_DBG(F("...Connection failed."));
    return ret_val;
}


int16_t loggerModem::openTCP(IPAddress ip, uint16_t port)
{
    MS_MOD_DBG(F("Connecting to"), ip, F("..."));
    int16_t ret_val = _tinyClient->connect(ip, port);
    if (ret_val) MS_MOD_DBG(F("...Success!"));
    else MS_MOD_DBG(F("...Connection failed."));
    return ret_val;
}


void loggerModem::closeTCP(void)
{
    if(_tinyClient)
        _tinyClient->stop();
    MS_MOD_DBG(F("Closed TCP/IP."));
}
***/


void loggerModem::modemPowerUp(void)
{
    if (_powerPin >= 0)
    {
        MS_MOD_DBG(F("Powering"), getSensorName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    else
    {
        MS_MOD_DBG(F("Power to"), getSensorName(), F("is not controlled by this library."));
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    _sensorStatus |= 0b00000110;
}


bool loggerModem::modemSleepPowerDown(void)
{
    bool success = true;
    uint32_t start = millis();
    MS_MOD_DBG(F("Turning"), getSensorName(), F("off."));

    // If there's a status pin available, check before running the sleep function
    // NOTE:  It's possible that the modem could still be in the process of turning
    // on and thus status pin isn't valid yet.  In that case, we wouldn't yet
    // know it's coming on and so we'd mistakenly assume it's already off and
    // not turn it back off.
    if (_dataPin >= 0 && digitalRead(_dataPin) != _statusLevel)
        MS_MOD_DBG(getSensorName(), F("appears to have already been off.  Will not run sleep function."));
    // Also check against the status bits
    else if (!bitRead(_sensorStatus, 4))
        MS_MOD_DBG(getSensorName(), F("was never sucessfully turned on.  Will not run sleep function."));
    else
    {
        // Run the sleep function
        MS_MOD_DBG(F("Running given sleep function for"), getSensorName());
        success &= _modemSleepFxn();
    }

    // Unset the activation time
    _millisSensorActivated = 0;
    // Unset the measurement request time
    _millisMeasurementRequested = 0;
    // Unset the status bits for sensor activation (bits 3 & 4) and measurement
    // request (bits 5 & 6)
    _sensorStatus &= 0b10000111;

    // Now power down
    if (_powerPin >= 0)
    {
        // If there's a status pin available, wait until modem shows it's ready to be powered off
        // This allows the modem to shut down gracefully.
        if (_dataPin >= 0)
        {
            MS_MOD_DBG(F("Waiting up to"), _disconnetTime_ms, F("milliseconds for graceful shutdown..."));
            while (millis() - start < _disconnetTime_ms && digitalRead(_dataPin) == _statusLevel){}
            if (digitalRead(_dataPin) == _statusLevel)
                MS_MOD_DBG(F("... "), getSensorName(), F("did not successfully shut down!"));
            else MS_MOD_DBG(F("... shutdown complete after"), millis() - start, F("ms."));
        }
        else if (_disconnetTime_ms > 0)
        {
            MS_MOD_DBG(F("Waiting"), _disconnetTime_ms, F("ms for graceful shutdown."));
            while (millis() - start < _disconnetTime_ms){}
        }

        MS_MOD_DBG(F("Turning off power to"), getSensorName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000001;
    }
    else
    {
        // If we're not going to power the modem down, there's no reason to hold
        // up the  main processor while waiting for the modem to shut down.
        // It can just do its thing unwatched while the main processor sleeps.
        MS_MOD_DBG(F("Power to"), getSensorName(), F("is not controlled by this library."));
        // Unset the power-on time and bits even if we didn't do anything.
        // This prevents the wake from happening on modems with no power pin
        // unless modemPowerUp() is called.
        _millisPowerOn = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000001;
    }

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
        MS_MOD_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    // Must ensure that we do not ping the daylight more than once every 4 seconds
    // NIST clearly specifies here that this is a requirement for all software
    // that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
    while (millis() < _lastNISTrequest + 4000) {}

    // Make TCP connection
    MS_MOD_DBG(F("Connecting to NIST daytime Server"));
    bool connectionMade = false;
    if (_modemName.indexOf(F("XBee")) >= 0)
    {
        IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
        connectionMade = _tinyClient->connect(ip, 37);  // XBee's address lookup falters on time.nist.gov
        _tinyClient->print('!');  // Need to send something before connection is made
        delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
    }
    else connectionMade = _tinyClient->connect("time.nist.gov", 37);

    // Wait up to 5 seconds for a response
    if (connectionMade)
    {
        long start = millis();
        while (_tinyClient && _tinyClient->available() < 4 && millis() - start < 5000L){}

        if (_tinyClient->available() >= 4)
        {
            MS_MOD_DBG(F("\nNIST responded after"), millis() - start, F("ms"));
            // Response is returned as 32-bit number as soon as connection is made
            // Connection is then immediately closed, so there is no need to close it
            uint32_t secFrom1900 = 0;
            byte response[4] = {0};
            for (uint8_t i = 0; i < 4; i++)
            {
                response[i] = _tinyClient->read();
                MS_MOD_DBG(F("\nResponse Byte"), i, ':', (char)response[i],
                           '=', response[i], '=', String(response[i], BIN));
                secFrom1900 += 0x000000FF & response[i];
                // MS_MOD_DBG(F("\nseconds from 1900 after byte:"),String(secFrom1900, BIN));
                if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
            }
            MS_MOD_DBG(F("\nSeconds from 1900 returned by NIST (UTC):"),
                       secFrom1900, '=', String(secFrom1900, BIN));

            // Close the TCP connection, just in case
            _tinyClient->stop();

            // Return the timestamp
            uint32_t unixTimeStamp = secFrom1900 - 2208988800;
            MS_MOD_DBG(F("\nUnix Timestamp returned by NIST (UTC):"), unixTimeStamp);
            // If before Jan 1, 2017 or after Jan 1, 2030, most likely an error
            if (unixTimeStamp < 1483228800) return 0;
            else if (unixTimeStamp > 1893456000) return 0;
            else return unixTimeStamp;
        }
        else
        {
            MS_MOD_DBG(F("NIST Time server did not respond!"));
            return 0;
        }
    }
    else MS_MOD_DBG(F("Unable to open TCP to NIST!"));
    return 0;
}

// Helper to get approximate RSSI from CSQ (assuming no noise)
int16_t loggerModem::getRSSIFromCSQ(int16_t csq)
{
    int16_t CSQs[33]  = {   0,    1,    2,    3,    4,    5,    6,   7,   8,   9,
                           10,   11,   12,   13,   14,   15,   16,  17,  18,  19,
                           20,   21,   22,   23,   24,   25,   26,  27,  28,  29,
                           30,   31,   99};
    int16_t RSSIs[33] = {-113, -111, -109, -107, -105, -103, -101, -99, -97, -95,
                          -93,  -91,  -89,  -87,  -85,  -83,  -81, -79, -77, -75,
                          -73,  -71,  -69,  -67,  -65,  -63,  -61, -59, -57, -55,
                          -53,  -51,    0};
    for (uint8_t i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return RSSIs[i];
    }
    return 0;
}

// Helper to get signal percent from CSQ
int16_t loggerModem::getPctFromCSQ(int16_t csq)
{
    int16_t CSQs[33]  = {   0,    1,    2,    3,    4,    5,    6,   7,   8,   9,
                           10,   11,   12,   13,   14,   15,   16,  17,  18,  19,
                           20,   21,   22,   23,   24,   25,   26,  27,  28,  29,
                           30,   31,   99};
    int16_t PCTs[33] = {    0,    3,    6,   10,   13,   16,   19,  23,  26,  29,
                           32,   36,   39,   42,   45,   48,   52,  55,  58,  61,
                           65,   68,   71,   74,   78,   81,   84,  87,  90,  94,
                           97,  100,    0};
    for (uint8_t i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return PCTs[i];
    }
    return 0;
}

// Helper to get signal percent from RSSI
int16_t loggerModem::getPctFromRSSI(int16_t rssi)
{
    int16_t pct = 1.6163*rssi + 182.61;
    if (rssi == 0) pct = 0;
    if (rssi == (255-93)) pct = 0;  // This is a no-data-yet value from XBee
    return pct;
}


// Helper to set the timing for specific cellular chipsets based on their documentation
/***
NOTE:  These times are for raw cellular chips they do no necessarily
apply to assembled break-out boards or modules
**warmUpTime** = Length of time after power is applied to module before the
enable pin can be called to turn on the module or other wake fxn can be used.
If the module boots up as soon as power is applied, this value is 0.
**indicatorTime** = Length of time from the completion of wake up  request
until the modem status pin begins to show an "on" status.
**stabilizationTime** =  Length of time from the completion of wake up
function until UART port becomes available for AT commands.  This becomes
the MAXIMUM amount of time we will wait for a response.  Where I could
find a time listed for boot up in the documentation, I use that time.
Where I could not find it listed, I use 5 seconds.
**disconnetTime** - Approximate length of time for unit to gracefully
close sockets and disconnect from the network.  Most manufactures strongly
recommend allowing a graceful shut-down rather than a sudden power-off.
***/
void loggerModem::setModemTiming(void)
{
    if (_modemName.indexOf(F("SIMCom SIM800")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a SIMCom SIM800"));
        _warmUpTime_ms = 450;  // Time after power on before "PWRKEY" can be used - >0.4sec
        _statusTime_ms = 2000;  // Time after end pulse until status pin becomes active (>3sec from start of 1s pulse)
        _stabilizationTime_ms = 2000;  // Time after end pulse until serial port becomes active (>3sec from start of 1s pulse)
        _on_pull_down_ms = 1100;  // >1s
        _off_pull_down_ms = 1100;  // 1sec > t > 33sec
        _disconnetTime_ms = 1500;  // power down (gracefully) takes >3sec
        // (Giving 15sec for shutdown in case it is not monitored.)
    }
    if (_modemName.indexOf(F("SIMCom SIM900")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a SIMCom SIM900"));
        _warmUpTime_ms = 1000;  // Time after power on before "PWRKEY" can be used (guess - diagram isn't clear)
        _statusTime_ms = 2200;  // Time after end pulse until status pin becomes active (>2.2sec)
        _stabilizationTime_ms = 2200;  // Time after end pulse until serial port becomes active (>2.2sec)
        _on_pull_down_ms = 1100;  // >1s
        _off_pull_down_ms = 600;  // 0.5sec > pull down > 1sec
        _disconnetTime_ms = 15000;  // power down (gracefully) takes >1.7 sec
        // (Giving 15sec for shutdown in case it is not monitored.)
    }
    if (_modemName.indexOf(F("SIMCom SIM7000")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a SIMCom SIM7000"));
        _warmUpTime_ms = 0;  // Time after power on before "PWRKEY" can be used
        _statusTime_ms = 4500;  // Time after end pulse until status pin becomes active (>4.5sec)
        _stabilizationTime_ms = 4500;  // Time after end pulse until serial port becomes active (>4.5sec)
        _on_pull_down_ms = 1100;  // >1s
        _off_pull_down_ms = 1300;  // >1.2sec
        _disconnetTime_ms = 7000;  // power down (gracefully) takes 1.8-6.9 sec
    }
    if (_modemName.indexOf(F("SARA-R4")) >= 0  ||
        _modemName.indexOf(F("SARA-N4")) >= 0  ||
        _modemName.indexOf(F("XBee3™ Cellular LTE-M")) >= 0  ||
        _modemName.indexOf(F("Digi XBee3™ Cellular NB-IoT")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a u-blox SARA-R4"));
        _warmUpTime_ms = 250;  // Time after power on before PWR_ON can be used ??? Unclear in documentation!
        _statusTime_ms = 0;  // V_INT becomes active mid-way through on-pulse
        _stabilizationTime_ms = 4500;  // Time until system and digital pins are operational (~4.5s)
        _on_pull_down_ms = 200;  // 0.15-3.2s
        _off_pull_down_ms = 1600;  // >1.5s
        _disconnetTime_ms = 15000;  // Power down time "can largely vary depending
        // on the application / network settings and the concurrent module
        // activities."  Vint/status pin should be monitored and power not withdrawn
        // until that pin reads low.  Giving 15sec here in case it is not monitored.
    }
    if (_modemName.indexOf(F("SARA-U2")) >= 0  ||
        _modemName.indexOf(F("XBee® Cellular 3G")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a u-blox SARA-U2"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active
                              // Unspecified in documentation! Taking value from Lisa U2
        _stabilizationTime_ms = 6000;  // Time until system and digital pins are operational
        // (6 sec typical for SARA U201, others 5 sec typical)
        // Time for an AT response may be much longer when using a 3G XBee in bypass mode!
        _on_pull_down_ms = 1;  // 50-80µs
        _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 15000;  // Power down time "can largely vary depending
        // on the application / network settings and the concurrent module
        // activities."  Vint/status pin should be monitored and power not withdrawn
        // until that pin reads low.  Giving 15sec here in case it is not monitored.
    }
    if (_modemName.indexOf(F("SARA-G3")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a u-blox SARA-G3"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active
                              // Unspecified in documentation! Taking value from Lisa U2
        _stabilizationTime_ms = 5000;  // Time until system and digital pins are operational (5 sec typical)
        _on_pull_down_ms = 6;  // >5ms
        _off_pull_down_ms = 1100;  // >1s
        _disconnetTime_ms = 15000;  // Power down time "can largely vary depending
        // on the application / network settings and the concurrent module
        // activities."  Vint/status pin should be monitored and power not withdrawn
        // until that pin reads low.  Giving 15sec here in case it is not monitored.
    }
    if (_modemName.indexOf(F("LISA-U2")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a u-blox LISA-U2"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active <35ms
        _stabilizationTime_ms = 3000;  // Time until system and digital pins are operational (3 sec typical)
        _on_pull_down_ms = 1;  // 50-80µs
        _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 400;  // power down (gracefully) takes ~400ms
    }
    if (_modemName.indexOf(F("Digi XBee® Cellular LTE Cat 1")) >= 0  ||
        _modemName.indexOf(F("Digi XBee3™ Cellular LTE CAT 1")) >= 0  ||
        _modemName.indexOf(F("Telit LE866")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a Telit LE866"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied
        _statusTime_ms = 50;  // Documentation does not specify how long between
        // power on and high reading on VAUX / PWRMON pin
        _stabilizationTime_ms = 25000;  // Documentation says to wait up to 25 (!!) seconds.
        _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 10000L;  // Wait with 10s time-out for sleep
    }
    if (_modemName.indexOf(F("ESP8266")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a ESP8266"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied
        _statusTime_ms = 350;  // N/A? - No status pin - use boot time if using a GPIO pin
        _stabilizationTime_ms = 350;  // Boot up time 200-300ms
        _on_pull_down_ms = 10;  // immediate
        _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 500;  // power down ???
    }
    if (_modemName.indexOf(F("Neoway M590")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a Neoway M590"));
        _warmUpTime_ms = 300;  // ON/OFF pin can be held low when power is applied
        // If the ON/OFF pin is not held low at time power is applied, wait at
        // least 300ms before dropping it low to turn the module on
        _statusTime_ms = 300;  // Time after end pulse until VCCIO becomes active
        _stabilizationTime_ms = 300;  // Time until UART is active (300ms)
        _on_pull_down_ms = 510;  // >300ms (>500ms recommended)
        _off_pull_down_ms = 510;  // >300ms
        _disconnetTime_ms = 6000;  // power down (gracefully) takes ~5sec
    }
    if (_modemName.indexOf(F("Quectel BG96")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a Quectel BG96"));
        _warmUpTime_ms = 30;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 4800;  // Time after end pulse until status pin becomes active
        _stabilizationTime_ms = 4200;  // USB active at >4.2 sec, status at >4.8 sec, URAT at >4.9
        _on_pull_down_ms = 110;  // >100ms
        _off_pull_down_ms = 700;  // ≥ 650ms
        _disconnetTime_ms = 15000;  // > 2 sec (Giving 15sec here in case it is not monitored.)
    }
    if (_modemName.indexOf(F("Quectel BC95")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a Quectel BC95"));
        _warmUpTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _statusTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _stabilizationTime_ms = 5000;  // ?? Time to UART availability not documented
        _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        // use AT+CPSMS command for LTE-M power saving - no other power save method
        _disconnetTime_ms = 0;  // N/A - If the reset pin is used as a status pin, it will not ever turn off
    }
    if (_modemName.indexOf(F("Quectel M95")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a Quectel M95"));
        _warmUpTime_ms = 30;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        _on_pull_down_ms = 2000;  // until either status key goes on, or > 1.0 sec (~2s)
        _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf(F("Quectel MC60")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a Quectel MC60"));
        _warmUpTime_ms = 100;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        _on_pull_down_ms = 1100;  // >1s
        _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf(F("Sequans VZM20Q")) >= 0)
    {
        MS_MOD_DBG(F("Resetting warm-up and disconnect timing for a Quectel MC60"));
        _warmUpTime_ms = 0;  // Module automatically boots when power is applied
        _statusTime_ms = 50;  // ?? Undocumented
        _stabilizationTime_ms = 5000;  // ?? Time to UART availability not documented
        _on_pull_down_ms = 50;  // ?? Undocumented
        _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        // use AT+CPSMS command for LTE-M power saving
        // use AT+SQNSSHDN command for device shut down
        _disconnetTime_ms = 15000;  // ?? Undocumented (Giving 15sec here in case it is not monitored.)
    }
    if (_modemName.indexOf(F("XBee")) >= 0)
    {
        MS_MOD_DBG(F("Putting connection values into flash memory for the Digi XBee"));
        _statusTime_ms = 50;  // ??? WAG!
        // XBee saves all configurations to flash, so we can set them here
        if (_tinyModem->hasWifi()) _tinyModem->networkConnect(_ssid, _pwd);
        else _tinyModem->gprsConnect(_apn, "", "");
    }
}
