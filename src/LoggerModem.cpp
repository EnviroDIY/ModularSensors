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

// Initialize the static members
int16_t loggerModem::_priorRSSI = -9999;
int16_t loggerModem::_priorSignalPercent = -9999;
float loggerModem::_priorModemTemp = -9999;
float loggerModem::_priorBatteryState = -9999;
float loggerModem::_priorBatteryPercent = -9999;
float loggerModem::_priorBatteryVoltage = -9999;
// float loggerModem::_priorActivationDuration = -9999;
// float loggerModem::_priorPoweredDuration = -9999;

// Constructor
loggerModem::loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                         int8_t modemResetPin, bool resetLevel, uint32_t resetPulse_ms,
                         int8_t modemSleepRqPin, bool wakeLevel, uint32_t wakePulse_ms,
                         uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                         uint32_t wakeDelayTime_ms, uint32_t max_atresponse_time_ms)
    : _powerPin(powerPin), _statusPin(statusPin), _statusLevel(statusLevel),
      _modemResetPin(modemResetPin), _resetLevel(resetLevel), _resetPulse_ms(resetPulse_ms),
      _modemSleepRqPin(modemSleepRqPin), _wakeLevel(wakeLevel), _wakePulse_ms(wakePulse_ms),
      _statusTime_ms(max_status_time_ms), _disconnetTime_ms(max_disconnetTime_ms),
      _wakeDelayTime_ms(wakeDelayTime_ms), _max_atresponse_time_ms(max_atresponse_time_ms)
{
    _modemName = "unspecified modem";
    _modemLEDPin = -1;

    _millisPowerOn = 0;
    _lastNISTrequest = 0;

    _hasBeenSetup = false;
    _pinModesSet = false;

    _priorSignalPercent = 0;
    _priorRSSI = 0;
    _priorModemTemp = 0;
    _priorBatteryState = 0;
    _priorBatteryPercent = 0;
    _priorBatteryVoltage = 0;
    // _priorActivationDuration = 0;
    // _priorPoweredDuration = 0;
}


// Destructor
loggerModem::~loggerModem() {}


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

String loggerModem::getModemName(void) { return _modemName; }

void loggerModem::modemPowerUp(void)
{
    if (_powerPin >= 0)
    {
        if (_modemSleepRqPin >= 0)
        {
            // For most modules, the sleep pin should be held high during power up.
            // After some warm-up time, that pin is usually pulsed low to wake
            // the module.
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        MS_DBG(F("Powering"), getModemName(), F("with pin"), _powerPin);
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    else
    {
        MS_DBG(F("Power to"), getModemName(), F("is not controlled by this library."));
        // Mark the power-on time, just in case it had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
}

void loggerModem::modemPowerDown(void)
{
    if (_powerPin >= 0)
    {
        // _priorPoweredDuration = ((float)(millis() - _millisPowerOn)) / 1000;
        // MS_DBG(F("Total modem power-on time (s):"), String(_priorPoweredDuration, 3));

        MS_DBG(F("Turning off power to"), getModemName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    else
    {
        MS_DBG(F("Power to"), getModemName(), F("is not controlled by this library."));
        // Unset the power-on time
        // _millisPowerOn = 0;
    }
}

bool loggerModem::modemSetup(void)
{
    // NOTE:  Set flag FIRST to stop infinite loop between modemSetup() and modemWake()
    bool success = true;
    _hasBeenSetup = true;

    MS_DBG(F("Setting up the modem ..."));

    // Power up
    bool wasPowered = true;
    if (_millisPowerOn == 0)
    {
        modemPowerUp();
        wasPowered = false;
    }

    // Check if the modem was awake, wake it if not
    bool wasAwake = true;
    if (_wakePulse_ms > 0 && _statusPin >= 0)
    {
        // If there's a pulse wake up (ie, non-zero wake time) and there's a status pin,
        // use that to determine if the modem was awake before setup began.
        wasAwake = digitalRead(_statusPin) == _statusLevel;
    }
    else if (_wakePulse_ms == 0)
    {
        // If the wake up is one where a pin is held (0 wake time) then we're going to check
        // the level of the held pin as the indication of whether attempts were made to wake
        // the modem before entering the setup function
        int8_t sleepRqBitNumber = log(digitalPinToBitMask(_modemSleepRqPin)) / log(2);
        int8_t currentRqPinState =
            bitRead(*portInputRegister(digitalPinToPort(_modemSleepRqPin)), sleepRqBitNumber);
        MS_DBG(F("Current state of sleep request pin"), _modemSleepRqPin, '=', currentRqPinState);
        wasAwake = (currentRqPinState == _wakeLevel);
    }
    if (!wasAwake)
    {
        while (millis() - _millisPowerOn < _wakeDelayTime_ms)
        {
        }
        MS_DBG(F("Waking up the modem for setup ..."));
        success &= modemWake();
    }
    else
    {
        MS_DBG(F("Modem was already awake and should be ready for setup."));
    }

    if (success)
    {
        MS_DBG(F("Running modem's extra setup function ..."));
        success &= extraModemSetup();
        if (success)
        {
            MS_DBG(F("... Complete!  It's a"), getModemName());
        }
        else
        {
            MS_DBG(F("... Failed!  It's a"), getModemName());
            _hasBeenSetup = false;
        }
    }
    else
    {
        MS_DBG(F("... "), getModemName(), F("did not wake up and cannot be set up!"));
    }

    MS_DBG(_modemName, F("warms up in"), _wakeDelayTime_ms, F("ms, indicates status in"),
           _statusTime_ms, F("ms, is responsive to AT commands in less than"),
           _max_atresponse_time_ms, F("ms, and takes up to"), _disconnetTime_ms,
           F("ms to close connections and shut down."));

    // Put the modem back to sleep if it was woken up just for setup
    // Only go to sleep if it had been asleep and is now awake
    if (!wasPowered)
    {  // Run the sleep and power down functions
        MS_DBG(F("Putting the modem to sleep and powering it down ..."));
        success &= modemSleepPowerDown();
    }
    else if (!wasAwake)
    {  // Run only the sleep function
        MS_DBG(F("Putting the modem to sleep ..."));
        success &= modemSleep();
    }
    else
    {
        MS_DBG(F("Leaving modem on after setup ..."));
    }

    return success;
}

// Nicely put the modem to sleep and power down
bool loggerModem::modemSleep(void)
{
    bool success = true;
    MS_DBG(F("Putting"), getModemName(), F("to sleep."));

    // If there's a status pin available, check before running the sleep function
    // NOTE:  It's possible that the modem could still be in the process of turning
    // on and thus status pin isn't valid yet.  In that case, we wouldn't yet
    // know it's coming on and so we'd mistakenly assume it's already off and
    // not turn it back off.
    // This only applies to modules with a pulse wake (ie, non-zero wake time).
    // For all modules that do pulse on, where possible I've selected a pulse time that is
    // sufficient to wake but not quite long enough to put it to sleep and am using AT
    // commands to sleep.  This *should* keep everything lined up.
    if (_statusPin >= 0 && digitalRead(_statusPin) != _statusLevel && _wakePulse_ms > 0)
    {
        MS_DBG(F("Status pin"), _statusPin, F("on"), getModemName(), F("is"),
               digitalRead(_statusPin),
               F("indicating it is already off!  Will not run sleep function."));
        // _priorActivationDuration = 0;
    }
    else
    {
        // Run the sleep function
        MS_DBG(F("Running given sleep function for"), getModemName());
        success &= modemSleepFxn();
        modemLEDOff();
    }
    return success;
}

// Nicely put the modem to sleep and power down
bool loggerModem::modemSleepPowerDown(void)
{
    bool success = true;
    uint32_t start = millis();
    MS_DBG(F("Turning"), getModemName(), F("off."));

    modemSleep();

    // Now power down
    if (_powerPin >= 0)
    {
        // If there's a status pin available, wait until modem shows it's ready to be powered off
        // This allows the modem to shut down gracefully.
        if (_statusPin >= 0)
        {
            MS_DBG(F("Waiting up to"), _disconnetTime_ms,
                   F("milliseconds for graceful shutdown as indicated by pin"),
                   _statusPin, F("going"), !_statusLevel, F("..."));
            while (millis() - start < _disconnetTime_ms && digitalRead(_statusPin) == _statusLevel) {}
            if (digitalRead(_statusPin) == _statusLevel)
            {
                MS_DBG(F("... "), getModemName(), F("did not successfully shut down!"));
            }
            else
            {
                MS_DBG(F("... shutdown complete after"), millis() - start, F("ms."));
            }
        }
        else if (_disconnetTime_ms > 0)
        {
            MS_DBG(F("Waiting"), _disconnetTime_ms, F("ms for graceful shutdown."));
            while (millis() - start < _disconnetTime_ms) {}
        }

        // _priorPoweredDuration = ((float)(millis() - _millisPowerOn)) / 1000;
        // MS_DBG(F("Total modem power-on time (s):"), String(_priorPoweredDuration, 3));

        MS_DBG(F("Turning off power to"), getModemName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    else
    {
        // _priorPoweredDuration = (float)-9999;

        // If we're not going to power the modem down, there's no reason to hold
        // up the main processor while waiting for the modem to shut down.
        // It can just do its thing unwatched while the main processor sleeps.
        MS_DBG(F("Power to"), getModemName(), F("is not controlled by this library."));
        // Unset the power-on time
        // _millisPowerOn = 0;
    }

    return success;
}

// Perform a hard/panic reset for when the modem is completely unresponsive
bool loggerModem::modemHardReset(void)
{
    if (_modemResetPin >= 0)
    {
        MS_DBG(F("Doing a hard reset on the modem!"));
        digitalWrite(_modemResetPin, _resetLevel);
        delay(_resetPulse_ms);
        digitalWrite(_modemResetPin, !_resetLevel);
        return true;
    }
    else
    {
        MS_DBG(F("No pin has been provided to reset the modem!"));
        return false;
    }
}
void loggerModem::setModemStatusLevel(bool level) { _statusLevel = level; }
void loggerModem::setModemWakeLevel(bool level) { _wakeLevel = level; }
void loggerModem::setModemResetLevel(bool level) { _resetLevel = level; }


void loggerModem::setModemPinModes(void)
{
    // Set-up pin modes
    if (!_pinModesSet)
    {
        // NOTE:  We're going to set the power pin mode every time in power up, just to be safe
        if (_statusPin >= 0)
        {
            MS_DBG(F("Initializing pin"), _statusPin, F("for modem status with on level expected to be"), _statusLevel);
            pinMode(_modemResetPin, INPUT);
        }
        if (_modemSleepRqPin >= 0)
        {
            MS_DBG(F("Initializing pin"), _modemSleepRqPin, F("for modem sleep with starting value"), !_wakeLevel);
            pinMode(_modemSleepRqPin, OUTPUT);
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        if (_modemResetPin >= 0)
        {
            MS_DBG(F("Initializing pin"), _modemResetPin, F("for modem reset with starting value"), !_resetLevel);
            pinMode(_modemResetPin, OUTPUT);
            digitalWrite(_modemResetPin, !_resetLevel);
        }
        if (_modemLEDPin >= 0)
        {
            MS_DBG(F("Initializing pin"), _modemLEDPin, F("for modem status LED with starting value 0"));
            pinMode(_modemLEDPin, OUTPUT);
            digitalWrite(_modemLEDPin, LOW);
        }
        _pinModesSet = true;
    }
}


bool loggerModem::updateModemMetadata(void)
{
    bool success = true;

    // Initialize variable
    int16_t rssi = -9999;
    int16_t percent = -9999;
    uint8_t state = 99;
    int8_t bpercent = -99;
    uint16_t volt = 9999;

    success &= getModemSignalQuality(rssi, percent);
    MS_DBG(F("CURRENT RSSI:"), rssi);
    MS_DBG(F("CURRENT Percent signal strength:"), percent);
    _priorRSSI = rssi;
    _priorSignalPercent = percent;

    success &= getModemBatteryStats(state, bpercent, volt);
    MS_DBG(F("CURRENT Modem Battery Charge State:"), state);
    MS_DBG(F("CURRENT Modem Battery Charge Percentage:"), bpercent);
    MS_DBG(F("CURRENT Modem Battery Voltage:"), bpercent);
    if (state != 99)
    {
        _priorBatteryState = (float)state;
    }
    else
    {
        _priorBatteryState = (float)-9999;
    }

    if (bpercent != -99)
    {
        _priorBatteryPercent = (float)bpercent;
    }
    else
    {
        _priorBatteryPercent = (float)-9999;
    }

    if (volt != 9999)
    {
        _priorBatteryVoltage = (float)volt;
    }
    else
    {
        _priorBatteryVoltage = (float)-9999;
    }

    _priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem Chip Temperature:"), _priorModemTemp);

    return success;
}

float loggerModem::getModemRSSI()
{
    float retVal = _priorRSSI;
    _priorRSSI = -9999;
    return retVal;
}
float loggerModem::getModemSignalPercent()
{
    float retVal = _priorSignalPercent;
    _priorSignalPercent = -9999;
    return retVal;
}
float loggerModem::getModemBatteryChargeState()
{
    float retVal = _priorBatteryState;
    _priorBatteryState = -9999;
    return retVal;
}
float loggerModem::getModemBatteryChargePercent()
{
    float retVal = _priorBatteryPercent;
    _priorBatteryPercent = -9999;
    return retVal;
}
float loggerModem::getModemBatteryVoltage()
{
    float retVal = _priorBatteryPercent;
    _priorBatteryPercent = -9999;
    return retVal;
}
float loggerModem::getModemTemperature()
{
    float retVal = _priorModemTemp;
    _priorModemTemp = -9999;
    return retVal;
}
// template <class Derived, typename modemType, typename modemClientType>
// float loggerModem::getModemActivationDuration()
// {
//     float retVal = _priorActivationDuration;
//     _priorActivationDuration = -9999;
//     return retVal;
// }
// template <class Derived, typename modemType, typename modemClientType>
// float loggerModem::getModemPoweredDuration()
// {
//     float retVal = _priorPoweredDuration;
//     _priorPoweredDuration = -9999;
//     return retVal;
// }

// Helper to get approximate RSSI from CSQ (assuming no noise)
int16_t loggerModem::getRSSIFromCSQ(int16_t csq)
{
    int16_t CSQs[33] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                        30, 31, 99};
    int16_t RSSIs[33] = {-113, -111, -109, -107, -105, -103, -101, -99, -97, -95,
                         -93, -91, -89, -87, -85, -83, -81, -79, -77, -75,
                         -73, -71, -69, -67, -65, -63, -61, -59, -57, -55,
                         -53, -51, 0};
    for (uint8_t i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return RSSIs[i];
    }
    return 0;
}

// Helper to get signal percent from CSQ
int16_t loggerModem::getPctFromCSQ(int16_t csq)
{
    int16_t CSQs[33] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                        30, 31, 99};
    int16_t PCTs[33] = {0, 3, 6, 10, 13, 16, 19, 23, 26, 29,
                        32, 36, 39, 42, 45, 48, 52, 55, 58, 61,
                        65, 68, 71, 74, 78, 81, 84, 87, 90, 94,
                        97, 100, 0};
    for (uint8_t i = 0; i < 33; i++)
    {
        if (CSQs[i] == csq) return PCTs[i];
    }
    return 0;
}

// Helper to get signal percent from RSSI
int16_t loggerModem::getPctFromRSSI(int16_t rssi)
{
    int16_t pct = 1.6163 * rssi + 182.61;
    if (rssi == 0) pct = 0;
    if (rssi == (255 - 93)) pct = 0;  // This is a no-data-yet value from XBee
    return pct;
}


uint32_t loggerModem::parseNISTBytes(byte nistBytes[4])
{
    // Response is returned as 32-bit number as soon as connection is made
    // Connection is then immediately closed, so there is no need to close it
    uint32_t secFrom1900 = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        MS_DBG(F("Response Byte"), i, ':', (char)nistBytes[i],
               '=', nistBytes[i], '=', String(nistBytes[i], BIN));
        secFrom1900 += 0x000000FF & nistBytes[i];
        // MS_DBG(F("\nseconds from 1900 after byte:"),String(secFrom1900, BIN));
        if (i + 1 < 4)
        {
            secFrom1900 = secFrom1900 << 8;
        }
    }
    MS_DBG(F("Seconds from Jan 1, 1900 returned by NIST (UTC):"),
           secFrom1900, '=', String(secFrom1900, BIN));

    // Return the timestamp
    uint32_t unixTimeStamp = secFrom1900 - 2208988800;
    MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);
    // If before Jan 1, 2019 or after Jan 1, 2030, most likely an error
    if (unixTimeStamp < 1546300800)
    {
        return 0;
    }
    else if (unixTimeStamp > 1893456000)
    {
        return 0;
    }
    else
    {
        return unixTimeStamp;
    }
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
        _wakeDelayTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active
                              // Unspecified in documentation! Taking value from Lisa U2
        _max_atresponse_time_ms = 5000;  // Time until system and digital pins are operational (5 sec typical)
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
        _wakeDelayTime_ms = 0;  // Module turns on when power is applied - level of PWR_ON then irrelevant
        _statusTime_ms = 35;  // Time after end pulse until V_INT becomes active <35ms
        _max_atresponse_time_ms = 3000;  // Time until system and digital pins are operational (3 sec typical)
        // _on_pull_down_ms = 1;  // 50-80µs
        // _off_pull_down_ms = 1000;  // >1s
        _disconnetTime_ms = 400;  // power down (gracefully) takes ~400ms
    }
    if (_modemName.indexOf(F("Digi XBee® Cellular LTE Cat 1")) >= 0  ||
        _modemName.indexOf(F("Digi XBee3™ Cellular LTE CAT 1")) >= 0  ||
        _modemName.indexOf(F("Telit LE866")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Telit LE866"));
        _wakeDelayTime_ms = 0;  // Module turns on when power is applied
        _statusTime_ms = 50;  // Documentation does not specify how long between
        // power on and high reading on VAUX / PWRMON pin
        _max_atresponse_time_ms = 25000;  // Documentation says to wait up to 25 (!!) seconds.
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        _disconnetTime_ms = 10000L;  // Wait with 10s time-out for sleep
    }
    if (_modemName.indexOf(F("Neoway M590")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Neoway M590"));
        _wakeDelayTime_ms = 300;  // ON/OFF pin can be held low when power is applied
        // If the ON/OFF pin is not held low at time power is applied, wait at
        // least 300ms before dropping it low to turn the module on
        _statusTime_ms = 300;  // Time after end pulse until VCCIO becomes active
        _max_atresponse_time_ms = 300;  // Time until UART is active (300ms)
        // _on_pull_down_ms = 510;  // >300ms (>500ms recommended)
        // _off_pull_down_ms = 510;  // >300ms
        _disconnetTime_ms = 6000;  // power down (gracefully) takes ~5sec
    }
    if (_modemName.indexOf(F("Quectel BC95")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel BC95"));
        _wakeDelayTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _statusTime_ms = 1;  // Time after VBAT is stable before RESET becomes valid - < 535 µs
        _max_atresponse_time_ms = 5000;  // ?? Time to UART availability not documented
        // _on_pull_down_ms = 0;  // N/A - standard chip cannot be powered on with pin
        // _off_pull_down_ms = 0;  // N/A - standard chip cannot be powered down with pin
        // use AT+CPSMS command for LTE-M power saving - no other power save method
        _disconnetTime_ms = 0;  // N/A - If the reset pin is used as a status pin, it will not ever turn off
    }
    if (_modemName.indexOf(F("Quectel M95")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel M95"));
        _wakeDelayTime_ms = 30;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _max_atresponse_time_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 2000;  // until either status key goes on, or > 1.0 sec (~2s)
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
    if (_modemName.indexOf(F("Quectel MC60")) >= 0)
    {
        MS_DBG(F("Resetting warm-up and disconnect timing for a Quectel MC60"));
        _wakeDelayTime_ms = 100;  // Time after VBAT is stable before PWRKEY can be used
        _statusTime_ms = 0;  // Time after end pulse until status pin becomes active (54ms after start of pulse)
        _max_atresponse_time_ms = 500;  // UART should respond as soon as PWRKEY pulse ends
        // _on_pull_down_ms = 1100;  // >1s
        // _off_pull_down_ms = 700;  // 0.6s<Pulldown<1s
        _disconnetTime_ms = 12000;  // disconnect in 2-12 seconds
    }
}
***/
