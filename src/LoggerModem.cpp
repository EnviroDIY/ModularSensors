/**
 * @file LoggerModem.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the loggerModem class.
 */

#include "LoggerModem.h"

// Initialize the static members
int16_t loggerModem::_priorRSSI           = -9999;
int16_t loggerModem::_priorSignalPercent  = -9999;
float   loggerModem::_priorModemTemp      = -9999;
float   loggerModem::_priorBatteryState   = -9999;
float   loggerModem::_priorBatteryPercent = -9999;
float   loggerModem::_priorBatteryVoltage = -9999;

// Constructor
loggerModem::loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                         int8_t modemResetPin, bool resetLevel,
                         uint32_t resetPulse_ms, int8_t modemSleepRqPin,
                         bool wakeLevel, uint32_t wakePulse_ms,
                         uint32_t max_status_time_ms,
                         uint32_t max_disconnetTime_ms,
                         uint32_t wakeDelayTime_ms,
                         uint32_t max_atresponse_time_ms)
    : _powerPin(powerPin),
      _statusPin(statusPin),
      _statusLevel(statusLevel),
      _modemResetPin(modemResetPin),
      _resetLevel(resetLevel),
      _resetPulse_ms(resetPulse_ms),
      _modemSleepRqPin(modemSleepRqPin),
      _wakeLevel(wakeLevel),
      _wakePulse_ms(wakePulse_ms),
      _statusTime_ms(max_status_time_ms),
      _disconnetTime_ms(max_disconnetTime_ms),
      _wakeDelayTime_ms(wakeDelayTime_ms),
      _max_atresponse_time_ms(max_atresponse_time_ms),
      _modemName("unspecified modem") {}

// Destructor
loggerModem::~loggerModem() {}


void loggerModem::setModemLED(int8_t modemLEDPin) {
    _modemLEDPin = modemLEDPin;
    if (_modemLEDPin >= 0) {
        pinMode(_modemLEDPin, OUTPUT);
        digitalWrite(_modemLEDPin, LOW);
    }
}
void loggerModem::modemLEDOn(void) {
    if (_modemLEDPin >= 0) { digitalWrite(_modemLEDPin, HIGH); }
}
void loggerModem::modemLEDOff(void) {
    if (_modemLEDPin >= 0) { digitalWrite(_modemLEDPin, LOW); }
}

String loggerModem::getModemName(void) {
    return _modemName;
}

void loggerModem::modemPowerUp(void) {
    if (_powerPin >= 0) {
        if (_modemSleepRqPin >= 0) {
            // For most modules, the sleep pin should be held high during power
            // up. After some warm-up time, that pin is usually pulsed low to
            // wake the module.
            MS_DBG(F("Setting sleep pin"), _modemSleepRqPin, F("to"),
                   !_wakeLevel ? F("HIGH") : F("LOW"), F("while powering on"),
                   getModemName());
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        MS_DBG(F("Powering"), getModemName(), F("with pin"), _powerPin);
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    } else {
        MS_DBG(F("Power to"), getModemName(),
               F("is not controlled by this library."));
        // Mark the power-on time, just in case it had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
}

void loggerModem::modemPowerDown(void) {
    if (_powerPin >= 0) {
        MS_DBG(F("Turning off power to"), getModemName(), F("with pin"),
               _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    } else {
        MS_DBG(F("Power to"), getModemName(),
               F("is not controlled by this library."));
    }
}

bool loggerModem::modemSetup(void) {
    // NOTE:  Set flag FIRST to stop infinite loop between modemSetup() and
    // modemWake()
    bool success  = true;
    _hasBeenSetup = true;

    MS_DBG(F("Setting up the modem ..."));

    // Power up
    bool wasPowered = true;
    if (_millisPowerOn == 0) {
        modemPowerUp();
        wasPowered = false;
    }

    // Check if the modem was awake, wake it if not
    bool wasAwake = isModemAwake();
    if (!wasAwake) {
        while (millis() - _millisPowerOn < _wakeDelayTime_ms) {
            // wait
        }
        MS_DBG(F("Waking up the modem for setup ..."));
        success &= modemWake();
    } else {
        MS_DBG(F("Modem was already awake and should be ready for setup."));
    }

    if (success) {
        MS_DBG(F("Running modem's extra setup function ..."));
        success &= extraModemSetup();
        if (success) {
            MS_DBG(F("... setup complete!  It's a"), getModemName());
        } else {
            MS_DBG(F("... setup failed!  It's a"), getModemName());
            _hasBeenSetup = false;
        }
    } else {
        MS_DBG(F("... "), getModemName(),
               F("did not wake up and cannot be set up!"));
    }

    MS_DBG(_modemName, F("warms up in"), _wakeDelayTime_ms,
           F("ms, indicates status in"), _statusTime_ms,
           F("ms, is responsive to AT commands in less than"),
           _max_atresponse_time_ms, F("ms, and takes up to"), _disconnetTime_ms,
           F("ms to close connections and shut down."));

    // Put the modem back to sleep if it was woken up just for setup
    // Only go to sleep if it had been asleep and is now awake
    if (!wasPowered) {  // Run the sleep and power down functions
        MS_DBG(F("Because the modem was not powered prior to setup, putting it"
                 "back to sleep and powering it down now."));
        success &= modemSleepPowerDown();
    } else if (!wasAwake) {  // Run only the sleep function
        MS_DBG(F("Because the modem was asleep prior to setup, putting it back "
                 "to sleep now."));
        success &= modemSleep();
    } else {
        MS_DBG(F("Leaving modem on after setup ..."));
    }

    return success;
}

// Nicely put the modem to sleep and power down
bool loggerModem::modemSleep(void) {
    bool success = true;
    MS_DBG(F("Putting"), getModemName(), F("to sleep."));

    // If there's a status pin available, check before running the sleep
    // function NOTE:  It's possible that the modem could still be in the
    // process of turning on and thus status pin isn't valid yet.  In that case,
    // we wouldn't yet know it's coming on and so we'd mistakenly assume it's
    // already off and not turn it back off. This only applies to modules with a
    // pulse wake (ie, non-zero wake time). For all modules that do pulse on,
    // where possible I've selected a pulse time that is sufficient to wake but
    // not quite long enough to put it to sleep and am using AT commands to
    // sleep.  This *should* keep everything lined up.
    if (!isModemAwake()) {
        MS_DBG(getModemName(),
               F("is already off!  Will not run sleep function."));
    } else {
        // Run the sleep function
        MS_DBG(F("Running given sleep function for"), getModemName());
        success &= modemSleepFxn();
        modemLEDOff();
    }
    return success;
}

// Nicely put the modem to sleep and power down
bool loggerModem::modemSleepPowerDown(void) {
    bool     success = true;
    uint32_t start   = millis();
    MS_DBG(F("Turning"), getModemName(), F("off."));

    modemSleep();

    // Now power down
    if (_powerPin >= 0) {
        // If there's a status pin available, wait until modem shows it's ready
        // to be powered off This allows the modem to shut down gracefully.
        if (_statusPin >= 0) {
            MS_DBG(F("Waiting up to"), _disconnetTime_ms,
                   F("milliseconds for graceful shutdown as indicated by pin"),
                   _statusPin, F("going"), !_statusLevel ? F("HIGH") : F("LOW"),
                   F("..."));
            while (millis() - start < _disconnetTime_ms &&
                   digitalRead(_statusPin) ==
                       static_cast<int>(_statusLevel)) {  // wait
            }
            if (digitalRead(_statusPin) == static_cast<int>(_statusLevel)) {
                MS_DBG(F("... "), getModemName(),
                       F("did not successfully shut down!"));
            } else {
                MS_DBG(F("... shutdown complete after"), millis() - start,
                       F("ms."));
            }
        } else if (_disconnetTime_ms > 0) {
            MS_DBG(F("Waiting"), _disconnetTime_ms,
                   F("ms for graceful shutdown."));
            while (millis() - start < _disconnetTime_ms) {
                // wait
            }
        }

        MS_DBG(F("Turning off power to"), getModemName(), F("with pin"),
               _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    } else {
        // If we're not going to power the modem down, there's no reason to hold
        // up the main processor while waiting for the modem to shut down.
        // It can just do its thing unwatched while the main processor sleeps.
        MS_DBG(F("Power to"), getModemName(),
               F("is not controlled by this library - not waiting for "
                 "shut-down to complete."));
    }

    return success;
}

// Perform a hard/panic reset for when the modem is completely unresponsive
bool loggerModem::modemHardReset(void) {
    if (_modemResetPin >= 0) {
        MS_DBG(F("Doing a hard reset on the modem by setting pin"),
               _modemResetPin, _resetLevel ? F("HIGH") : F("LOW"), F("for"),
               _resetPulse_ms, F("ms"));
        digitalWrite(_modemResetPin, _resetLevel);
        delay(_resetPulse_ms);
        digitalWrite(_modemResetPin, !_resetLevel);
        return true;
    } else {
        MS_DBG(F("No pin has been provided to reset the modem!"));
        return false;
    }
}
void loggerModem::setModemStatusLevel(bool level) {
    _statusLevel = level;
}
void loggerModem::setModemWakeLevel(bool level) {
    _wakeLevel = level;
}
void loggerModem::setModemResetLevel(bool level) {
    _resetLevel = level;
}


void loggerModem::setModemPinModes(void) {
    // Set-up pin modes
    if (!_pinModesSet) {
        // NOTE:  We're going to set the power pin mode every time in power up,
        // just to be safe
        if (_statusPin >= 0) {
            MS_DBG(F("Initializing pin"), _statusPin,
                   F("for modem status with on level expected to be"),
                   _statusLevel ? F("HIGH") : F("LOW"));
            pinMode(_statusPin, INPUT);
        }
        if (_modemSleepRqPin >= 0) {
            MS_DBG(F("Initializing pin"), _modemSleepRqPin,
                   F("for modem sleep with starting value"),
                   !_wakeLevel ? F("HIGH") : F("LOW"));
            pinMode(_modemSleepRqPin, OUTPUT);
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        if (_modemResetPin >= 0) {
            MS_DBG(F("Initializing pin"), _modemResetPin,
                   F("for modem reset with starting value"),
                   !_resetLevel ? F("HIGH") : F("LOW"));
            pinMode(_modemResetPin, OUTPUT);
            digitalWrite(_modemResetPin, !_resetLevel);
        }
        if (_modemLEDPin >= 0) {
            MS_DBG(F("Initializing pin"), _modemLEDPin,
                   F("for modem status LED with starting value 0"));
            pinMode(_modemLEDPin, OUTPUT);
            digitalWrite(_modemLEDPin, LOW);
        }
        _pinModesSet = true;
    }
}


bool loggerModem::updateModemMetadata(void) {
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI           = -9999;
    loggerModem::_priorSignalPercent  = -9999;
    loggerModem::_priorBatteryState   = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorModemTemp      = -9999;

    // Initialize variable
    int16_t  rssi     = -9999;
    int16_t  percent  = -9999;
    uint8_t  state    = 99;
    int8_t   bpercent = -99;
    uint16_t volt     = 9999;

    // Try for up to 15 seconds to get a valid signal quality
    uint32_t startMillis = millis();
    do {
        success &= getModemSignalQuality(rssi, percent);
        loggerModem::_priorRSSI          = rssi;
        loggerModem::_priorSignalPercent = percent;
        if (rssi != 0 && rssi != -9999) break;
        delay(250);
    } while ((rssi == 0 || rssi == -9999) && millis() - startMillis < 15000L &&
             success);
    MS_DBG(F("CURRENT RSSI:"), rssi);
    MS_DBG(F("CURRENT Percent signal strength:"), percent);

    success &= getModemBatteryStats(state, bpercent, volt);
    MS_DBG(F("CURRENT Modem Battery Charge State:"), state);
    MS_DBG(F("CURRENT Modem Battery Charge Percentage:"), bpercent);
    MS_DBG(F("CURRENT Modem Battery Voltage:"), volt);
    if (state != 99)
        loggerModem::_priorBatteryState = static_cast<float>(state);
    else
        loggerModem::_priorBatteryState = static_cast<float>(-9999);

    if (bpercent != -99)
        loggerModem::_priorBatteryPercent = static_cast<float>(bpercent);
    else
        loggerModem::_priorBatteryPercent = static_cast<float>(-9999);

    if (volt != 9999)
        loggerModem::_priorBatteryVoltage = static_cast<float>(volt);
    else
        loggerModem::_priorBatteryVoltage = static_cast<float>(-9999);

    loggerModem::_priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem Chip Temperature:"), loggerModem::_priorModemTemp);

    return success;
}

float loggerModem::getModemRSSI() {
    float retVal = loggerModem::_priorRSSI;
    MS_DEEP_DBG(F("PRIOR RSSI:"), retVal);
    return retVal;
}
float loggerModem::getModemSignalPercent() {
    float retVal = loggerModem::_priorSignalPercent;
    MS_DEEP_DBG(F("PRIOR Percent signal strength:"), retVal);
    return retVal;
}
float loggerModem::getModemBatteryChargeState() {
    float retVal = loggerModem::_priorBatteryState;
    MS_DEEP_DBG(F("PRIOR Modem Battery Charge State:"), retVal);
    return retVal;
}
float loggerModem::getModemBatteryChargePercent() {
    float retVal = loggerModem::_priorBatteryPercent;
    MS_DEEP_DBG(F("PRIOR Modem Battery Charge Percentage:"), retVal);
    return retVal;
}
float loggerModem::getModemBatteryVoltage() {
    float retVal = loggerModem::_priorBatteryVoltage;
    MS_DEEP_DBG(F("PRIOR Modem Battery Voltage:"), retVal);
    return retVal;
}
float loggerModem::getModemTemperature() {
    float retVal = loggerModem::_priorModemTemp;
    MS_DEEP_DBG(F("PRIOR Modem Chip Temperature:"), retVal);
    return retVal;
}

// Helper to get approximate RSSI from CSQ (assuming no noise)
int16_t loggerModem::getRSSIFromCSQ(int16_t csq) {
    int16_t CSQs[33]  = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                         11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                         22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 99};
    int16_t RSSIs[33] = {-113, -111, -109, -107, -105, -103, -101, -99, -97,
                         -95,  -93,  -91,  -89,  -87,  -85,  -83,  -81, -79,
                         -77,  -75,  -73,  -71,  -69,  -67,  -65,  -63, -61,
                         -59,  -57,  -55,  -53,  -51,  0};
    for (uint8_t i = 0; i < 33; i++) {
        if (CSQs[i] == csq) return RSSIs[i];
    }
    return 0;
}

// Helper to get signal percent from CSQ
int16_t loggerModem::getPctFromCSQ(int16_t csq) {
    int16_t CSQs[33] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                        11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                        22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 99};
    int16_t PCTs[33] = {0,  3,  6,  10, 13, 16, 19, 23, 26, 29,  32,
                        36, 39, 42, 45, 48, 52, 55, 58, 61, 65,  68,
                        71, 74, 78, 81, 84, 87, 90, 94, 97, 100, 0};
    for (uint8_t i = 0; i < 33; i++) {
        if (CSQs[i] == csq) return PCTs[i];
    }
    return 0;
}

// Helper to get signal percent from RSSI
int16_t loggerModem::getPctFromRSSI(int16_t rssi) {
    auto pct = static_cast<int16_t>(1.6163 * static_cast<float>(rssi) + 182.61);
    if (rssi == 0) pct = 0;
    if (rssi == (255 - 93)) pct = 0;  // This is a no-data-yet value from XBee
    return pct;
}


uint32_t loggerModem::parseNISTBytes(byte nistBytes[4]) {
    // Response is returned as 32-bit number as soon as connection is made
    // Connection is then immediately closed, so there is no need to close it
    uint32_t secFrom1900 = 0;
    for (uint8_t i = 0; i < 4; i++) {
        MS_DBG(F("Response Byte"), i, ':', static_cast<char>(nistBytes[i]), '=',
               nistBytes[i], '=', String(nistBytes[i], BIN));
        secFrom1900 += 0x000000FF & nistBytes[i];
        if (i + 1 < 4) { secFrom1900 = secFrom1900 << 8; }
    }
    MS_DBG(F("Seconds from Jan 1, 1900 returned by NIST (UTC):"), secFrom1900,
           '=', String(secFrom1900, BIN));

    // Return the timestamp
    uint32_t unixTimeStamp = secFrom1900 - 2208988800;
    MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);
    // If before Jan 1, 2019 or after Jan 1, 2030, most likely an error
    if (unixTimeStamp < 1546300800) {
        return 0;
    } else if (unixTimeStamp > 1893456000) {
        return 0;
    } else {
        return unixTimeStamp;
    }
}
