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
                         int8_t modemResetPin, int8_t modemSleepRqPin, bool alwaysRunWake,
                         uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                         uint32_t max_warmUpTime_ms, uint32_t max_atresponse_time_ms,
                         uint32_t max_signalQuality_time_ms,
                         uint8_t measurementsToAverage)
    : Sensor("Tiny GSM Modem", MODEM_NUM_VARIABLES,
             max_warmUpTime_ms, max_atresponse_time_ms, max_signalQuality_time_ms,
             powerPin, statusPin, measurementsToAverage)
{
    _modemName = "unspecified modem";

    _modemResetPin = modemResetPin;
    _modemSleepRqPin = modemSleepRqPin;
    _modemLEDPin = -1;
    _alwaysRunWake = alwaysRunWake;

    _statusLevel = statusLevel;
    _statusTime_ms = max_status_time_ms,
    _disconnetTime_ms = max_disconnetTime_ms;

    _lastNISTrequest = 0;
    _lastConnectionCheck = 0;
    _lastATCheck = 0;

    previousCommunicationFailed = false;
}


// Destructor
loggerModem::~loggerModem(){}


void loggerModem::setModemLED(int8_t modemLEDPin)
{
    _modemLEDPin = modemLEDPin;
    if (_modemLEDPin >= 0)
    {
        pinMode(_modemLEDPin, OUTPUT);
        digitalWrite(_modemLEDPin, LOW);
    }
};
void loggerModem::modemLEDOn(void)
{
    if (_modemLEDPin >= 0)
    {
        digitalWrite(_modemLEDPin, HIGH);
    }
}
void loggerModem::modemLEDOff(void)
{
    if (_modemLEDPin >= 0)
    {
        digitalWrite(_modemLEDPin, LOW);
    }
}


void loggerModem::modemHardReset(void)
{
    if (_modemResetPin >= 0)
    {
        MS_DBG(F("Doing a hard reset!"));
        digitalWrite(_modemResetPin, LOW);
        delay(200);
        digitalWrite(_modemResetPin, HIGH);
        // Re-set _millisSensorActivated  - the hard reset is a new activation
        _millisSensorActivated = millis();
        // Unset the flag for prior communication failure
        previousCommunicationFailed = false;
    }
}


String loggerModem::getSensorName(void) { return _modemName; }


// NOTE: Unlike other setup functions, this will NOT turn the power to the modem
// on and off, it will simply check if the power has already been turned on and
// return quickly if not.  If the power had been on, it is left ON after set-up.
bool loggerModem::setup(void)
{
    bool success = Sensor::setup();  // this will set pin modes and the setup status bit

    // Set-up pin modes
    if (_modemSleepRqPin >= 0)
    {
        pinMode(_modemSleepRqPin, OUTPUT);
        // NOTE:  Not setting level of sleep request pin
    }
    if (_modemResetPin >= 0)
    {
        pinMode(_modemResetPin, OUTPUT);
        digitalWrite(_modemResetPin, HIGH);
    }
    if (_modemLEDPin >= 0)
    {
        pinMode(_modemLEDPin, OUTPUT);
        digitalWrite(_modemLEDPin, LOW);
    }

    // Initialize the modem
    MS_DBG(F("Setting up the modem ..."));

    // Bail if there's no power
    if (!checkPowerOn())
    {
        MS_DBG(F("Modem doesn't have power and cannot be set-up!"));
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
        MS_DBG(F("Running modem wake function ..."));
        success &= wake();
        // NOTE:  not setting wake bits here because we'll go back to sleep
        // before the end of this function if we weren't awake
    }
    else MS_DBG(F("Modem was already awake."));

    if (success)
    {
        // The begin() generally starts with a 5 second testAT(), that should
        // be enough time to allow any modem to be ready to respond
        MS_DBG(F("Running modem's begin function ..."));
        success &= extraModemSetup();
        if (success) MS_DBG(F("... Complete!  It's a"), getSensorName());
        else MS_DBG(F("... Failed!  It's a"), getSensorName());
    }
    else MS_DBG(F("... "), getSensorName(), F("did not wake up and cannot be set up!"));

    // Set the timing for modems based on their names
    // NOTE:  These are based on documentation for the raw chip!
    // setModemTiming();


    MS_DBG(_modemName, F("warms up in"), _warmUpTime_ms, F("ms, indicates status in"),
           _statusTime_ms, F("ms, is responsive to AT commands in less than"),
           _stabilizationTime_ms, F("ms, and takes up to"), _disconnetTime_ms,
           F("ms to close connections and shut down."));

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
        MS_DBG(F("Running given modem sleep function ..."));
        success &= modemSleepFxn();
    }
    else MS_DBG(F("Leaving modem on after setup ..."));
    // Do NOT power down at the end, because this fxn cannot have powered the
    // modem up.

    return success;
}


// The function to wake up the modem
// There are so many ways to wake a modem that we're requiring an input function
bool loggerModem::wake(void)
{
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;
    // NOTE:  _millisSensorActivated can be set here or by modemHardReset
    // NOTE:  This is the ONLY place status bit 4 is set!
    bool success = true;

    // Check the status pin and wake bits before running wake function
    // Don't want to accidently pulse an already on modem to off
    // NOTE:  It's possible that the status pin is on, but the modem is actually
    // mid-shutdown.  In that case, we'll mistakenly skip re-waking it.
    if (_dataPin >= 0 && digitalRead(_dataPin) == _statusLevel && !_alwaysRunWake)
    {
        MS_DBG(getSensorName(), F("was already on!  (status pin"), _dataPin,
               F("level = "), _statusLevel, F(") Will not run wake function."));
    }
    else
    {
        // Run the input wake function
        MS_DBG(F("Running wake function for"), getSensorName());
        success &= modemWakeFxn();
    }

    // Re-check the status pin
    // Only works if the status pin comes on immediately - most don't
    if (_dataPin >= 0 && _statusTime_ms == 0 && digitalRead(_dataPin) != _statusLevel)
    {
        MS_DBG(F("Status pin"), _dataPin, F("on"), getSensorName(), F("is"),
                   digitalRead(_dataPin), F("indicating it is off!"));
        success = false;
        modemHardReset();
        success = digitalRead(_dataPin) == _statusLevel;
    }

    if (success)
    {
        modemLEDOn();
        MS_DBG(getSensorName(), F("should be awake."));
    }
    else
    {
        MS_DBG(getSensorName(), F("failed to wake!"));
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
    MS_DBG(F("Skipping"), getSensorName(), F("in sensor power up!"));
}
void loggerModem::powerDown(void)
{
    MS_DBG(F("Skipping"), getSensorName(), F("in sensor power down!"));
}


// This checks to see if enough time has passed for stability
// In the case of the modem, we consider it to be "stable" when it responds to
// AT commands
bool loggerModem::isStable(bool debug)
{
    #if defined MS_LOGGERMODEM_DEBUG_DEEP
    debug = true;
    #endif

    // If the modem never "woke", then it will never respond and thus it's
    // essentially already "stable."
    if (!bitRead(_sensorStatus, 4))
    {
        if (debug) MS_DBG(getSensorName(),
                F("did not wake; AT commands will not be attempted!"));
        return true;
    }

    // just defining this to not call multiple times below
    uint32_t now = millis();

    uint32_t elapsed_since_wake_up = now - _millisSensorActivated;

    // If the modem has a status pin and it's still off after the specified time
    // plus a 500ms buffer, then, if it's the first time this happened give up
    // and move on.  If it's happened twice in a row, try a hard reset and
    // keep waiting.
    if ( (_dataPin >= 0 && elapsed_since_wake_up > (_statusTime_ms + 500) &&
              digitalRead(_dataPin) != _statusLevel))
    {
        // if we maxed out the wait for the status pin after the previous wake
        // and we've maxed it again after the most recent wake, do a hard reset
        // and continue waiting
        if (previousCommunicationFailed)
        {
            modemHardReset();
            return false;
        }
        else
        {
            if (debug) MS_DBG(F("It's been"), (elapsed_since_wake_up), F("ms, and status pin"),
              _dataPin, F("on"), getSensorName(), F("is"), digitalRead(_dataPin),
              F("indicating it is off.  Will not continue to attempt communication!"));
            // Unset status bit 4 (wake up success) and _millisSensorActivated
            // We unset these bits here because it's possible that a modem "passed"
            // the wake command, but never really woke.  For sensors that take time
            // before their status pin becomes active, (_statusTime_ms > 0) we
            // don't know that they failed to wake until we check here
            _millisSensorActivated = 0;
            _sensorStatus &= 0b11101111;
            previousCommunicationFailed = true;
            return true;
        }
    }

    // We don't want to ping any of the modems too fast so they don't get
    // overwhelmed.  Make sure we've waited a little
    if (now - _lastATCheck < 250) return false;

    // If the modem is now responding to AT commands, it's "stable"
    if (didATRespond())
    {
        if (debug) MS_DBG(F("It's been"), (elapsed_since_wake_up), F("ms, and"),
               getSensorName(), F("is now responding to AT commands!"));
        _lastATCheck = now;
        previousCommunicationFailed = false;
        return true;
    }

    // If we've exceeded the documented time until UART should respond (plus
    // 750ms buffer), then, if it's the first time this happened give up
    // and move on.  If it's happened twice in a row, try a hard reset and
    // keep waiting.
    if (elapsed_since_wake_up > (_stabilizationTime_ms + 750))
    {
        if (previousCommunicationFailed)
        {
            // if we maxed out the wait last time and we've maxed it again,
            // do a hard reset and continue waiting
            modemHardReset();
            return false;
        }
        else
        {
            if (debug) MS_DBG(F("It's been"), (elapsed_since_wake_up), F("ms, and"),
                   getSensorName(), F("has maxed out wait for AT command reply!  Ending wait."));
             // Unset status bit 4 (wake up success) and _millisSensorActivated
             // It's safe to unset these here because we've already tested and failed
             // to respond to AT commands.
             _millisSensorActivated = 0;
             _sensorStatus &= 0b11101111;
             previousCommunicationFailed = true;
            return true;
        }
    }

    // If the modem isn't responding to AT commands yet, but its status pin shows
    // it's on and we haven't maxed out the response time, we still need to wait
    _lastATCheck = now;
    return false;
}


bool loggerModem::isMeasurementComplete(bool debug)
{
    return verifyMeasurementComplete(debug);
}


bool loggerModem::addSingleMeasurementResult(void)
{
    bool success = true;

    /* Initialize float variable */
    int16_t percent = -9999;
    int16_t rssi = -9999;
    uint8_t state = 99;
    int8_t bpercent = -99;
    uint16_t volt = 9999;
    float temp = -9999;
    float fstate = -9999;
    float fbpercent = -9999;
    float fvolt = -9999;

    /* Check a measurement was *successfully* started (status bit 6 set) */
    /* Only go on to get a result if it was */
    if (bitRead(_sensorStatus, 6))
    {
        /* Get signal quality */
        /* NOTE:  We can't actually distinguish between a bad modem response, no */
        /* modem response, and a real response from the modem of no service/signal. */
        /* The TinyGSM getSignalQuality function returns the same "no signal" */
        /* value (99 CSQ or 0 RSSI) in all 3 cases. */
        MS_DBG(F("Asking modem to give signal quality:"));
        success &= getModemSignalQuality(rssi, percent);
        MS_DBG(F("RSSI:"), rssi);
        MS_DBG(F("Percent signal strength:"), percent);

        MS_DBG(F("Getting battery info, if possible:"));
        success &= getModemBatteryStats(state, bpercent, volt);
        // convert responses to floats
        if (success)
        {
            if (state != 99) fstate = (float)state;
            if (bpercent != -99) fbpercent = (float)bpercent;
            if (volt != 9999) fvolt = (float)volt;
            MS_DBG(F("Modem battery charge state:"), fstate);
            MS_DBG(F("Modem battery percentage:"), fbpercent);
            MS_DBG(F("Modem battery voltage:"), fvolt);
        }
        else MS_DBG(F("Battery information not returned!"));

        MS_DBG(F("Getting chip temperature, if possible:"));
        temp = getModemTemperature();
        MS_DBG(F("Modem temperature:"), temp);
    }
    else MS_DBG(getSensorName(), F("is not expected to return measurement results!"));

    verifyAndAddMeasurementResult(MODEM_RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(MODEM_PERCENT_SIGNAL_VAR_NUM, percent);
    verifyAndAddMeasurementResult(MODEM_BATTERY_STATE_VAR_NUM, fstate);
    verifyAndAddMeasurementResult(MODEM_BATTERY_PERCENT_VAR_NUM, fbpercent);
    verifyAndAddMeasurementResult(MODEM_BATTERY_VOLT_VAR_NUM, fvolt);
    verifyAndAddMeasurementResult(MODEM_TEMPERATURE_VAR_NUM, temp);

    /* Unset the time stamp for the beginning of this measurement */
    _millisMeasurementRequested = 0;
    /* Unset the status bits for a measurement request (bits 5 & 6) */
    _sensorStatus &= 0b10011111;

    return success;
}


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//


void loggerModem::modemPowerUp(void)
{
    if (_powerPin >= 0)
    {
        MS_DBG(F("Powering"), getSensorName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    else
    {
        MS_DBG(F("Power to"), getSensorName(), F("is not controlled by this library."));
        // Mark the power-on time, just in case it had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    _sensorStatus |= 0b00000110;
}


bool loggerModem::modemSleepPowerDown(void)
{
    bool success = true;
    uint32_t start = millis();
    MS_DBG(F("Turning"), getSensorName(), F("off."));

    // If there's a status pin available, check before running the sleep function
    // NOTE:  It's possible that the modem could still be in the process of turning
    // on and thus status pin isn't valid yet.  In that case, we wouldn't yet
    // know it's coming on and so we'd mistakenly assume it's already off and
    // not turn it back off.
    if (_dataPin >= 0 && digitalRead(_dataPin) != _statusLevel && !_alwaysRunWake)
    {
        MS_DBG(F("Status pin"), _dataPin, F("on"), getSensorName(), F("is"),
               digitalRead(_dataPin), F("indicating it is already off!  Will not run sleep function."));
    }
    // If there's no status pin, check against the status bits
    else if (_dataPin < 0 && !bitRead(_sensorStatus, 4) && !_alwaysRunWake)
    {
        MS_DBG(getSensorName(), F("was never sucessfully turned on.  Will not run sleep function."));
    }
    else
    {
        // Run the sleep function
        MS_DBG(F("Running given sleep function for"), getSensorName());
        success &= modemSleepFxn();
        modemLEDOff();
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
            MS_DBG(F("Waiting up to"), _disconnetTime_ms, F("milliseconds for graceful shutdown as indicated by pin"),
                   _dataPin, F("going"), !_statusLevel, F("..."));
            while (millis() - start < _disconnetTime_ms && digitalRead(_dataPin) == _statusLevel){}
            if (digitalRead(_dataPin) == _statusLevel)
                MS_DBG(F("... "), getSensorName(), F("did not successfully shut down!"));
            else MS_DBG(F("... shutdown complete after"), millis() - start, F("ms."));
        }
        else if (_disconnetTime_ms > 0)
        {
            MS_DBG(F("Waiting"), _disconnetTime_ms, F("ms for graceful shutdown."));
            while (millis() - start < _disconnetTime_ms){}
        }

        MS_DBG(F("Turning off power to"), getSensorName(), F("with pin"), _powerPin);
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
        MS_DBG(F("Power to"), getSensorName(), F("is not controlled by this library."));
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
void loggerModem::setModemTiming(void)
{
    if (_modemName.indexOf(F("SARA-G3")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a u-blox SARA-G3"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active
                              // Unspecified in documentation! Taking value from Lisa U2
        _stabilizationTime_ms = 5000;  // Time until system and digital pins are operational (5 sec typical)
        // _on_pull_down_ms = 6;  // >5ms
        // _off_pull_down_ms = 1100;  // >1s
        _disconnetTime_ms = 15000;  // Power down time "can largely vary depending
        // on the application / network settings and the concurrent module
        // activities."  Vint/status pin should be monitored and power not withdrawn
        // until that pin reads low.  Giving 15sec here in case it is not monitored.
    }
    if (_modemName.indexOf(F("LISA-U2")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a u-blox LISA-U2"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active <35ms
        _stabilizationTime_ms = 3000;  // Time until system and digital pins are operational (3 sec typical)
        // _on_pull_down_ms = 1;  // 50-80µs
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 400;  // power down (gracefully) takes ~400ms
    }
    if (_modemName.indexOf(F("Digi XBee® Cellular LTE Cat 1")) >= 0  ||
        _modemName.indexOf(F("Digi XBee3™ Cellular LTE CAT 1")) >= 0  ||
        _modemName.indexOf(F("Telit LE866")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Telit LE866"));
        _warmUpTime_ms = 0;  // Module turns on when power is applied
        _statusTime_ms = 50;  // Documentation does not specify how long between
        // power on and high reading on VAUX / PWRMON pin
        _stabilizationTime_ms = 25000;  // Documentation says to wait up to 25 (!!) seconds.
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 10000L;  // Wait with 10s time-out for sleep
    }
    if (_modemName.indexOf(F("Neoway M590")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Neoway M590"));
        _warmUpTime_ms = 300;  // ON/OFF pin can be held low when power is applied
        // If the ON/OFF pin is not held low at time power is applied, wait at
        // least 300ms before dropping it low to turn the module on
        _statusTime_ms = 300;  // Time after end pulse until VCCIO becomes active
        _stabilizationTime_ms = 300;  // Time until UART is active (300ms)
        // _on_pull_down_ms = 510;  // >300ms (>500ms recommended)
        // _off_pull_down_ms = 510;  // >300ms
        _disconnetTime_ms = 6000;  // power down (gracefully) takes ~5sec
    }
    if (_modemName.indexOf(F("Quectel BC95")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel BC95"));
        _warmUpTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _statusTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _stabilizationTime_ms = 5000;  // ?? Time to UART availability not documented
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        // use AT+CPSMS command for LTE-M power saving - no other power save method
        _disconnetTime_ms = 0;  // N/A - If the reset pin is used as a status pin, it will not ever turn off
    }
    if (_modemName.indexOf(F("Quectel M95")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel M95"));
        _warmUpTime_ms = 30;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 2000;  // until either status key goes on, or > 1.0 sec (~2s)
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf(F("Quectel MC60")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel MC60"));
        _warmUpTime_ms = 100;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _stabilizationTime_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
}
***/
