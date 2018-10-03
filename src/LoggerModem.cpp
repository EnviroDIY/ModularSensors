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
             powerPin, statusPin, 1),
      _statusLevel(statusLevel), _indicatorTime_ms(0), _disconnetTime_ms(0),
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
             powerPin, statusPin, 1),
      _statusLevel(statusLevel), _indicatorTime_ms(0), _disconnetTime_ms(0),
      _ssid(ssid), _pwd(pwd), _lastNISTrequest(0)
{
    _tinyModem = inModem;
    _tinyClient = inClient;
    _wakeFxn = wakeFxn;
    _sleepFxn = sleepFxn;
    _modemName = "unspecified modem";
}

String loggerModem::getSensorName(void) { return _modemName; }


// NOTE!! Power is left ON after set-up
bool loggerModem::setup(void)
{
    bool success = true;

    // Initialize the modem
    MS_MOD_DBG(F("Setting up the modem ...\n"));

    if (_powerPin >= 0) pinMode(_powerPin, OUTPUT);
    if (_dataPin >= 0) pinMode(_dataPin, INPUT);

    // Check if the an attempt was made to power the sensor (bit 1) and it succeeded (bit 2)
    // Currently there is no double check that the powerOn() was successful
    if (!bitRead(_sensorStatus, 1) || !bitRead(_sensorStatus, 2) || _millisPowerOn == 0)
    {
        MS_MOD_DBG(getSensorName(), F(" doesn't have power and cannot be set up!\n"));
        success = false;
    }

    // Turn the modem on, and if successful, begin
    else if(wake())
    {
        success &= _tinyModem->begin();  // This generally begins with a 5 second testAT()
        _modemName = _tinyModem->getModemName();
        if (success) MS_MOD_DBG(F("   ... Complete!  It's a "), getSensorName(), F(".\n"));
        else MS_MOD_DBG(F("   ... Failed!  It's a "), getSensorName(), F(".\n"));
    }
    else MS_MOD_DBG(F("   ... "), getSensorName(), F(" did not wake up and cannot be set up!\n"));

    // Set some warm-up times for specific models
    // NOTE:  These times are for raw cellular chips they do no necessarily
    // apply to assembled break-out boards or modules
    if (_modemName.indexOf("SIMCom SIM800") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a SIMCom SIM800\n"));
        _warmUpTime_ms = 450; // Time after power on before "PWRKEY" can be used
        _indicatorTime_ms = 2100;  // Time after end pulse until status pin becomes active (>3sec from start of pulse)
        _stabilizationTime_ms = 2100;  // Time after end pulse until serial port becomes active (>3sec from start of pulse)
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 1100;  // 1sec > t > 3.3sec
        _disconnetTime_ms = 3100;  // power down (gracefully) takes >3sec
    }
    if (_modemName.indexOf("SIMCom SIM900") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a SIMCom SIM900\n"));
        _warmUpTime_ms = 1000; // Time after power on before "PWRKEY" can be used (guess - diagram isn't clear)
        _indicatorTime_ms = 2200;  // Time after end pulse until status pin becomes active (>2.2sec)
        _stabilizationTime_ms = 2200;  // Time after end pulse until serial port becomes active (>2.2sec)
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 600;  // 0.5sec > pull down > 1sec
        _disconnetTime_ms = 1800;  // power down (gracefully) takes >1.
    }
    if (_modemName.indexOf("SARA-R4") >= 0  ||
        _modemName.indexOf("XBee3™ Cellular LTE-M") >= 0  ||
        _modemName.indexOf("Digi XBee3™ Cellular NB-IoT") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a u-blox SARA-R4\n"));
        _warmUpTime_ms = 250; // Time after power on before PWR_ON can be used ??? Unclear in diagram!
        _indicatorTime_ms = 0;  // V_INT becomes active mid-way through on-pulse
        _stabilizationTime_ms = 4600;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 200;  // 0.15-3.2s
        // _off_pull_down_ms = 1600;  // >1.5s
        _disconnetTime_ms = 4600;  // power down time is until Vint pin is reading low
    }
    if (_modemName.indexOf("SARA-U2") >= 0  ||
        _modemName.indexOf("XBee® Cellular 3G") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a u-blox SARA-U2\n"));
        _warmUpTime_ms = 0; // Module turns on when power is applied - level of PWR_ON then irrelevant
        _indicatorTime_ms = 35;  // Time after end pulse until V_INT becomes active
                                 // Unclear in diagram! Taking value from Lisa U2
        _stabilizationTime_ms = 6000;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 1;  // 50-80µs
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 6000;  // power down time is until Vint pin is reading low
    }
    if (_modemName.indexOf("SARA-G3") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a u-blox SARA-G3\n"));
        _warmUpTime_ms = 0; // Module turns on when power is applied - level of PWR_ON then irrelevant
        _indicatorTime_ms = 35;  // Time after end pulse until V_INT becomes active
                                 // Unclear in diagram! Taking value from Lisa U2
        _stabilizationTime_ms = 5000;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 5;  // >5ms
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 6000;  // power down time is until Vint pin is reading low
    }
    if (_modemName.indexOf("LISA-U2") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a u-blox LISA-U2\n"));
        _warmUpTime_ms = 0; // Module turns on when power is applied - level of PWR_ON then irrelevant
        _indicatorTime_ms = 35;  // Time after end pulse until V_INT becomes active <35ms
        _stabilizationTime_ms = 3000;  // Time until system and digital pins are operational
        // _on_pull_down_ms = 1;  // 50-80µs
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 2600;  // power down (gracefully) takes ~2.5sec
    }
    if (_modemName.indexOf("Digi XBee® Cellular LTE Cat 1") >= 0  ||
        _modemName.indexOf("Digi XBee3™ Cellular LTE CAT 1") >= 0  ||
        _modemName.indexOf("Telit LE866") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a Telit LE866\n"));
        _warmUpTime_ms = 0; // Module turns on when power is applied
        _indicatorTime_ms = 0;  // N/A? - No status pin on Telit chip
        _stabilizationTime_ms = 25000L;  // Wait with 25s time-out for first AT response
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 10000L;  // Wait with 10s time-out for sleep
    }
    if (_modemName.indexOf("ESP8266") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a ESP8266\n"));
        _warmUpTime_ms = 0; // Module turns on when power is applied
        _indicatorTime_ms = 350;  // N/A? - No status pin - use boot time if using a GPIO pin
        _stabilizationTime_ms = 350;  // Boot up time 200-300ms
        // _on_pull_down_ms = 10;  // immediate
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 200;
        // power down ???
    }
    if (_modemName.indexOf("Neoway M590") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a Neoway M590\n"));
        _warmUpTime_ms = 0; // ON/OFF pin should be held low as soon as power is applied
        _indicatorTime_ms = 300;  // Time after end pulse until status pin becomes active (?? not specified)
        _stabilizationTime_ms = 300;  // Time until UART is active
        // _on_pull_down_ms = 510;  // >300ms (>500ms recommended)
        // _off_pull_down_ms = 510;  // >300ms
        _disconnetTime_ms = 5000;  // power down (gracefully) takes ~5sec
    }
    if (_modemName.indexOf("Quectel BG96") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a Quectel BG96\n"));
        _warmUpTime_ms = 30; // Time after VBAT is stable before PWRKEY can be used
        _indicatorTime_ms = 4800;  // Time after end pulse until status pin becomes active
        _stabilizationTime_ms = 4900;  // USB active at >4.2 sec, status at >4.8 sec, URAT at >4.9
        // _on_pull_down_ms = 110;  // >100ms
        // _off_pull_down_ms = 700;  // ≥ 650ms
        _disconnetTime_ms = 2000;  // > 2 sec
    }
    if (_modemName.indexOf("Quectel M95") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a Quectel M95\n"));
        _warmUpTime_ms = 30; // Time after VBAT is stable before PWRKEY can be used
        _indicatorTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 0;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 2000;  // until either status key goes on, or > 1.0 sec (~2s)
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 2000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf("Quectel MC60") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for a Quectel MC60\n"));
        _warmUpTime_ms = 100; // Time after VBAT is stable before PWRKEY can be used
        _indicatorTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 0;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 2000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf("AI-Thinker A") >= 0)
    {
        MS_MOD_DBG(F("Using expected warm-up and disconnect timing for an AI-Thinker A6\n"));
        _warmUpTime_ms = 0; //??
        _indicatorTime_ms = 0;  // ??
        _stabilizationTime_ms = 0;  // ??
        // _on_pull_down_ms = 2000;  // >2s
        // _off_pull_down_ms = 0;  // ??
        _disconnetTime_ms = 0;  // ??
    }
    if (_modemName.indexOf("XBee") >= 0)
    {
        MS_MOD_DBG(F("Using status timing for a Digi XBee\n"));
        _indicatorTime_ms = 50;  // ??? WAG!
    }

    // Set the status bit marking that the modem has been set up (bit 0)
    // Only set the bit if setup was successful!
    if (success) _sensorStatus |= 0b00000001;
    // Otherwise, set the status error bit (bit 7)
    else _sensorStatus |= 0b10000000;

    return success;
}


// The function to wake up a sensor
// There are so many ways to wake a modem that we're requiring an input function
bool loggerModem::wake(void)
{
    // this will check for power and set timestamp and status bit
    bool success = Sensor::wake();

    // if the sensor::wake() failed, there's no power, so bail
    if (!success) return success;

    // Try the given wake function up to 5 times
    uint32_t start = millis();
    bool fxnSuccess = false;
    int ntries = 0;
    MS_MOD_DBG(F("Waking "), getSensorName());
    while (!fxnSuccess && ntries < 5)
    {
        MS_MOD_DBG(F(" ("), ntries+1, F("): "));
        start = millis();
        fxnSuccess = _wakeFxn();
        ntries++;
    }
    success &= fxnSuccess;

    // Before we quit, check the status pin
    // Only works if the status pin comes on immediately
    if ((_dataPin >= 0 && start - millis() > _indicatorTime_ms &&
         digitalRead(_dataPin) != _statusLevel))
    {
        MS_MOD_DBG(F("It's been "), (start - millis()), F("ms, and status pin on "),
              getSensorName(), F(" is "), digitalRead(_dataPin), F(" indicating it is off!\n"));
        success = false;
    }

    if (success)
    {
        // Update the time that the sensor was activated
        _millisSensorActivated = millis();
        MS_MOD_DBG(getSensorName(), F(" should be awake.\n"));
    }
    else
    {
        MS_MOD_DBG(getSensorName(), F(" is not awake!\n"));
        // Make sure the activation time is zero and the wake success bit (bit 4) is unset
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
    }
    return success;
}


// Do NOT turn the modem on and off with the regular power up and down!
// This is because when it is run in an array with other sensors, we will
// generally want the modem to remain on after all the other sensors have
// gone to sleep and powered down so the modem can send out data
void loggerModem::powerUp(void)
{
    MS_MOD_DBG(F("Skipping "), getSensorName(), F(" in sensor power up!\n"));
}
void loggerModem::powerDown(void)
{
    MS_MOD_DBG(F("Skipping "), getSensorName(), F(" in sensor power down!\n"));
}
bool loggerModem::sleep(void)
{
    MS_MOD_DBG(F("Skipping "), getSensorName(), F(" in sensor sleep!\n"));
    return true;
}

bool loggerModem::startSingleMeasurement(void)
{
    bool success = true;
    MS_MOD_DBG(F("Starting measurement on "), getSensorName(), F(" at "),
           getSensorLocation(), F(".\n"));
    // Set the status bits for measurement requested (bit 5)
    // Setting this bit even if we failed to start a measurement to show that an attempt was made.
    _sensorStatus |= 0b00100000;

    // Check if BOTH an activation/wake attempt was made (status bit 3 set)
    // AND that attempt was successful (bit 4 set, _millisSensorActivated > 0)
    // Only mark the measurement request time if it is
    if (bitRead(_sensorStatus, 3) && bitRead(_sensorStatus, 4) && _millisSensorActivated > 0)
    {

        // For the wifi modems, the SSID and password need to be set before they
        // can join a network.
        // For cellular modems, network registration (should) happen automatically.
        // The GPRS bearer (APN) is then set after registration when making the GPRS
        // (data) link.
        if (_ssid && _tinyModem->hasWifi() && !_tinyModem->isNetworkConnected())
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
        MS_MOD_DBG(getSensorName(), F(" at "), getSensorLocation(),
               F(" isn't awake/active!  A measurement cannot be started.\n"));
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
    int signalQual = -9999;
    int percent = -9999;
    int rssi = -9999;

    // Check if BOTH a measurement start attempt was made (status bit 5 set)
    // AND that attempt was successful (bit 6 set, _millisMeasurementRequested > 0)
    // Only go on to get a result if it is
    if (bitRead(_sensorStatus, 5) && bitRead(_sensorStatus, 6) && _millisMeasurementRequested > 0)
    {
        // The XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // Connecting to the NIST daytime server, which immediately returns a
        // 4 byte response and then closes the connection
        if (_modemName.indexOf("XBee") >= 0)
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
        MS_MOD_DBG(F("Raw signal quality: "), signalQual, F("\n"));

        // Convert signal quality to RSSI, if necessary
        if ((_modemName.indexOf("XBee") >= 0 || _modemName.indexOf("ESP8266") >= 0)
            && signalQual != -9999)
        {
            rssi = signalQual;
            percent = getPctFromRSSI(signalQual);
        }
        else if (signalQual != -9999)
        {
            rssi = getRSSIFromCSQ(signalQual);
            percent = getPctFromCSQ(signalQual);
        }

        MS_MOD_DBG(F("RSSI: "), rssi, F("\n"));
        MS_MOD_DBG(F("Percent signal strength: "), percent, F("\n"));
    }
    else MS_MOD_DBG(getSensorName(), F(" is not connected to the network; unable to get signal quality!\n"));

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
    if (!bitRead(_sensorStatus, 3) || !bitRead(_sensorStatus, 4) || _millisSensorActivated == 0)
    {
        if (debug) MS_MOD_DBG(getSensorName(),
                F(" is not on and AT commands will not be attempted!\n"));
        return true;
    }

    uint32_t elapsed_since_wake_up = millis() - _millisSensorActivated;
    // If the modem has a status pin and it's off, give up
    if ( (_dataPin >= 0 && elapsed_since_wake_up > _indicatorTime_ms &&
              digitalRead(_dataPin) != _statusLevel))
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and status pin on "),
              getSensorName(), F(" is "), digitalRead(_dataPin),
              F(" indicating it is off.  AT commands will not be attempted!\n"));
        // Unset status bit 4 (wake up success) and _millisSensorActivated
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
        return true;
    }
    // If the modem is now responding to AT commands, it's "stable"
    else if (_tinyModem->testAT(100))
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" is now responding to AT commands!\n"));
        return true;
    }
    // If we've exceeded the time-out, give up
    else if (elapsed_since_wake_up > MODEM_MAX_REPLY_TIME)
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" has maxed out wait for AT command reply!  Ending wait.\n"));
         // Unset status bit 4 (wake up success) and _millisSensorActivated
         _millisSensorActivated = 0;
         _sensorStatus &= 0b11101111;
        return true;
    }
    // If the modem isn't responding to AT commands yet, we still need to wait
    else return false;
}


// This checks to see if enough time has passed for measurement completion
// In the case of the modem, we consider a measurement to be "complete" when
// the modem has registered on the network
bool loggerModem::isMeasurementComplete(bool debug)
{
    // If the modem never responded to AT commands,this will fail.
    if (_millisSensorActivated == 0)
    {
        if (debug) MS_MOD_DBG(getSensorName(),
               F(" is not responding to AT commands; will not ask for signal strength!\n"));
        return true;
    }

    // Unlike most sensors where we're interested in the millis since measurement
    // was started for a measurement completion time, for the modem the only
    // time of interest is the time since it was turned on.
    uint32_t elapsed_since_wake_up = millis() - _millisSensorActivated;
    // If the modem is registered on the network, it's "stable"
    if (_tinyModem->isNetworkConnected())
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" is now registered on the network!\n"));
        return true;
    }
    // If we've exceeded the time-out, give up
    if (elapsed_since_wake_up > MODEM_MAX_SEARCH_TIME)
    {
        if (debug) MS_MOD_DBG(F("It's been "), (elapsed_since_wake_up), F("ms, and "),
               getSensorName(), F(" has maxed out wait for network registration!  Ending wait.\n"));
         // Unset status bit 6 (start measurement success) and _millisMeasurementRequested
         _millisMeasurementRequested = 0;
         _sensorStatus &= 0b11101111;
        return true;
    }
    // If the modem isn't registered yet, we still need to wait
    return false;
}


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//
bool loggerModem::connectInternet(uint32_t waitTime_ms)
{
    bool retVal = true;

    if (bitRead(getStatus(), 1) == 0 || bitRead(getStatus(), 2) == 0)  // NOT yet powered
        modemPowerUp();

    // Check if the modem has previously be set-up and set it up if not
    // we're doing this because it's possible that the modem might be skipped
    // during the main setup/begin function due to low battery power.
    // Some of the parameters for the modem functionality are actually read
    // during the modem set up, so we need to make sure a set-up happens.
    if (bitRead(getStatus(), 0) == 0)  // NOT yet set up
    {
        MS_MOD_DBG(F("Modem has not yet been set up.  Running setup now.\n"));
        waitForWarmUp();
        retVal &= setup();
    }
    if (!retVal)
    {
        MS_MOD_DBG(F("Modem setup failed! Cannot connect to the internet!\n"));
        return retVal;
    }

    if (bitRead(getStatus(), 3) == 0)  // NOT yet awake/actively measuring
    {
        waitForWarmUp();
        retVal &= wake();  // This sets the modem to on
    }
    if (!retVal)
    {
        MS_MOD_DBG(F("Modem did't wake up! Cannot connect to the internet!\n"));
        return retVal;
    }

    // Check that the modem is responding to AT commands.  If not, give up.
    uint32_t start = millis();
    MS_MOD_DBG(F("\nWaiting up to 5 seconds for "), getSensorName(), F(" to respond to AT commands...\n"));
    if (!_tinyModem->testAT(5000))
    {
        MS_MOD_DBG(F("No response to AT commands! Cannot connect to the internet!\n"));
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
            if (!_tinyModem->waitForNetwork(waitTime_ms))
            {
                MS_MOD_DBG(F("   ... WiFi connection failed\n"));
                return false;
            }
        }
        MS_MOD_DBG(F("   ... WiFi connected after "), millis() - start,
                   F(" milliseconds!\n"));
        return true;
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
    uint32_t start = millis();
    if (_tinyModem->hasGPRS() && _modemName.indexOf("XBee") < 0)  // XBee doesn't like to disconnect)
    {
        _tinyModem->gprsDisconnect();
        MS_MOD_DBG(F("Disconnected from cellular network after "), millis() - start,
                   F(" milliseconds.\n"));
    }
    else if (_modemName.indexOf("XBee") < 0)  // XBee doesn't like to disconnect
    {
        _tinyModem->networkDisconnect();
        MS_MOD_DBG(F("Disconnected from WiFi network after "), millis() - start,
                   F(" milliseconds.\n"));
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
    if(_tinyClient)
        _tinyClient->stop();
    MS_MOD_DBG(F("Closed TCP/IP.\n"));
}


void loggerModem::modemPowerUp(void)
{
    if (_powerPin >= 0)
    {
        MS_MOD_DBG(F("Powering "), getSensorName(), F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    else
    {
        MS_MOD_DBG(F("Power to "), getSensorName(), F(" is not controlled by this library.\n"));
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    _sensorStatus |= 0b00000110;
}


bool loggerModem::modemSleepPowerDown(void)
{
    bool success = true;

    MS_MOD_DBG(F("Turning "), getSensorName(), F(" off.\n"));
     // Wait for any sending to complete
    _tinyClient->flush();
    uint32_t start = millis();

    // If there's a status pin available, check before running the sleep function
    if (_dataPin >= 0 && digitalRead(_dataPin) != _statusLevel)
        MS_MOD_DBG(getSensorName(), F("  appears to have already been off.\n"));
    else
    {
        // Run the sleep function
        MS_MOD_DBG(F("Running sleep function.\n"));
        success &= _sleepFxn();
    }

    // If there's a status pin available, wait until modem shows it's ready to be powered off
    // This allows the modem to shut down gracefully.
    if (_dataPin >= 0)
    {
        MS_MOD_DBG(F("Waiting up to 15 seconds for graceful shutdown...\n"));
        while (millis() - start < 15000L && digitalRead(_dataPin) == _statusLevel){}
        if (digitalRead(_dataPin) == _statusLevel)
            MS_MOD_DBG(F("  ... "), getSensorName(), F(" did not successfully shut down!\n"));
        else MS_MOD_DBG(F("  ... shutdown complete after "), millis() - start, F("ms.\n"));
    }
    else if (_disconnetTime_ms > 0)
    {
        MS_MOD_DBG(F("Waiting "), _disconnetTime_ms, F("ms for graceful shutdown.\n"));
        while (millis() - start < _disconnetTime_ms){}
    }

    // Unset the activation time
    _millisSensorActivated = 0;

    // Now power down
    if (_powerPin >= 0)
    {
        MS_MOD_DBG(F("Turning off power to "), getSensorName(), F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    else
    {
        MS_MOD_DBG(F("Power to "), getSensorName(), F(" is not controlled by this library.\n"));
    }
    // Unset the status bits for sensor power (bits 1 & 2),
    // activation (bits 3 & 4), and measurement request (bits 5 & 6)
    _sensorStatus &= 0b10000001;

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
    if (_modemName.indexOf("XBee") >= 0)
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
                MS_MOD_DBG(F("\nResponse Byte "), i, F(": "), (char)response[i],
                           F(" = "), response[i], F(" = "), String(response[i], BIN));
                secFrom1900 += 0x000000FF & response[i];
                // MS_MOD_DBG(F("\nseconds from 1900 after byte: "),String(secFrom1900, BIN));
                if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
            }
            MS_MOD_DBG(F("\nSeconds from 1900 returned by NIST (UTC):  "),
                       secFrom1900, String(secFrom1900, BIN));

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
