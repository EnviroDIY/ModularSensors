/**
 * @file LoggerBase.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Logger class.
 */

#include "LoggerBase.h"
#include "dataPublisherBase.h"

#if (defined(__AVR__) || defined(ARDUINO_ARCH_AVR)) && \
    not defined(SDI12_INTERNAL_PCINT)
// Unless we're forcing use of internal interrupts, use EnableInterrupt for AVR
// boards
/**
 * @brief To prevent compiler/linker crashes with enable interrupt library, we
 * must define LIBCALL_ENABLEINTERRUPT before importing EnableInterrupt within a
 * library.
 */
#define LIBCALL_ENABLEINTERRUPT
// To handle external and pin change interrupts
#include "ModSensorInterrupts.h"
// For all i2c communication, including with the real time clock
// #include <Wire.h>

#elif not defined(__AVR__) && not defined(ARDUINO_ARCH_AVR)
// For compatibility with non AVR boards, we need these macros
#define enableInterrupt(pin, userFunc, mode) \
    attachInterrupt(digitalPinToInterrupt(pin), userFunc, mode)
#define disableInterrupt(pin) detachInterrupt(digitalPinToInterrupt(pin))
#endif


// Initialize the static timezone
int8_t Logger::_loggerUTCOffset = 0;
// Initialize the static epoch
epochStart Logger ::_loggerEpoch = MS_LOGGER_EPOCH;
// Initialize the static time adjustment
int8_t Logger::_loggerRTCOffset = 0;
// Initialize the static timestamps
uint32_t Logger::markedLocalUnixTime = 0;
uint32_t Logger::markedUTCUnixTime   = 0;
// Initialize the testing/logging flags
volatile bool Logger::isLoggingNow = false;
volatile bool Logger::isTestingNow = false;
volatile bool Logger::startTesting = false;
// Initialize flags for sleep settings for SAMD boards
#if defined(ARDUINO_ARCH_SAMD)
bool Logger::_tristatePins = true;
#if defined(__SAMD51__)
bool Logger::_peripheralShutdown = true;
#endif
#endif


// Constructors
Logger::Logger(const char* loggerID, const char* samplingFeatureUUID,
               int16_t loggingIntervalMinutes, int8_t SDCardSSPin,
               int8_t mcuWakePin, VariableArray* inputArray)
    : _SDCardSSPin(SDCardSSPin),
      _mcuWakePin(mcuWakePin) {
    // Set parameters from constructor
    setLoggerID(loggerID);
    setSamplingFeatureUUID(samplingFeatureUUID);
    setLoggingInterval(loggingIntervalMinutes);
    setVariableArray(inputArray);

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        dataPublishers[i] = nullptr;
    }

    // Set a datetime callback for automatic timestamping of files by SdFat
    SdFile::dateTimeCallback(fileDateTimeCallback);
}
Logger::Logger(const char* loggerID, int16_t loggingIntervalMinutes,
               int8_t SDCardSSPin, int8_t mcuWakePin, VariableArray* inputArray)
    : _SDCardSSPin(SDCardSSPin),
      _mcuWakePin(mcuWakePin) {
    // Set parameters from constructor
    setLoggerID(loggerID);
    setLoggingInterval(loggingIntervalMinutes);
    setVariableArray(inputArray);

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        dataPublishers[i] = nullptr;
    }

    // Set a datetime callback for automatic timestamping of files by SdFat
    SdFile::dateTimeCallback(fileDateTimeCallback);
}
Logger::Logger(const char* loggerID, const char* samplingFeatureUUID,
               int16_t loggingIntervalMinutes, VariableArray* inputArray) {
    // Set parameters from constructor
    setLoggerID(loggerID);
    setSamplingFeatureUUID(samplingFeatureUUID);
    setLoggingInterval(loggingIntervalMinutes);
    setVariableArray(inputArray);

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        dataPublishers[i] = nullptr;
    }

    // Set a datetime callback for automatic timestamping of files by SdFat
    SdFile::dateTimeCallback(fileDateTimeCallback);
}
Logger::Logger(const char* loggerID, const char* samplingFeatureUUID,
               int16_t loggingIntervalMinutes) {
    // Set parameters from constructor
    setLoggerID(loggerID);
    setSamplingFeatureUUID(samplingFeatureUUID);
    setLoggingInterval(loggingIntervalMinutes);

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        dataPublishers[i] = nullptr;
    }

    // Set a datetime callback for automatic timestamping of files by SdFat
    SdFile::dateTimeCallback(fileDateTimeCallback);
}
Logger::Logger(const char* loggerID, int16_t loggingIntervalMinutes,
               VariableArray* inputArray) {
    // Set parameters from constructor
    setLoggerID(loggerID);
    setLoggingInterval(loggingIntervalMinutes);
    setVariableArray(inputArray);

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        dataPublishers[i] = nullptr;
    }

    // Set a datetime callback for automatic timestamping of files by SdFat
    SdFile::dateTimeCallback(fileDateTimeCallback);
}
Logger::Logger() {
    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        dataPublishers[i] = nullptr;
    }

    // Set a datetime callback for automatic timestamping of files by SdFat
    SdFile::dateTimeCallback(fileDateTimeCallback);
}
// Destructor
Logger::~Logger() {}


// ===================================================================== //
// Public functions to get and set basic logging paramters
// ===================================================================== //

// Sets the logger ID
void Logger::setLoggerID(const char* loggerID) {
    _loggerID = loggerID;
}

// Sets the logging interval
void Logger::setLoggingInterval(int16_t loggingIntervalMinutes) {
    _loggingIntervalMinutes = loggingIntervalMinutes;
}


// Sets the number of initial short intervals
void Logger::setinitialShortIntervals(int16_t initialShortIntervals) {
    _remainingShortIntervals = initialShortIntervals;
}


// Adds the sampling feature UUID
void Logger::setSamplingFeatureUUID(const char* samplingFeatureUUID) {
    _samplingFeatureUUID = samplingFeatureUUID;
}

// Sets up a pin controlling the power to the SD card
void Logger::setSDCardPwr(int8_t SDCardPowerPin) {
    _SDCardPowerPin = SDCardPowerPin;
    if (_SDCardPowerPin >= 0) {
        pinMode(_SDCardPowerPin, OUTPUT);
        digitalWrite(_SDCardPowerPin, LOW);
        MS_DBG(F("Pin"), _SDCardPowerPin, F("set as SD Card Power Pin"));
    }
}
// NOTE:  Structure of power switching on SD card taken from:
// https://thecavepearlproject.org/2017/05/21/switching-off-sd-cards-for-low-power-data-logging/
void Logger::turnOnSDcard(bool waitToSettle) {
    if (_SDCardPowerPin >= 0) {
        MS_DBG(F("Pulling SS pin"), _SDCardSSPin,
               F("HIGH and then powering the SD card with pin"),
               _SDCardPowerPin);
        // Pull the SS pin high before power up.
        // Cave Pearl notes that some cars will fail on power-up unless SS is
        // pulled up
        pinMode(_SDCardSSPin, OUTPUT);
        digitalWrite(_SDCardSSPin, HIGH);
        delay(6);
        pinMode(_SDCardPowerPin, OUTPUT);
        digitalWrite(_SDCardPowerPin, HIGH);
        delay(6);
        // TODO(SRGDamia1):  figure out how long to wait
        if (waitToSettle) {
            MS_DEEP_DBG(F("Waiting 100ms for SD card to boot"));
            delay(100);
        }
    }
}
void Logger::turnOffSDcard(bool waitForHousekeeping) {
    if (_SDCardPowerPin >= 0) {
        MS_DBG(F("Cutting power to the SD card with pin"), _SDCardPowerPin);
        // TODO(SRGDamia1): set All SPI pins to INPUT?
        // TODO(SRGDamia1): set ALL SPI pins HIGH (~30k pull-up)
        pinMode(_SDCardPowerPin, OUTPUT);
        digitalWrite(_SDCardPowerPin, LOW);
        // TODO(SRGDamia1):  wait in lower power mode
        if (waitForHousekeeping) {
            MS_DEEP_DBG(
                F("Waiting 1s for SD card to finish any on-going writes"));
            // Specs say up to 1s for internal housekeeping after each write
            delay(1000);
        }
    }
}


// Sets up a pin for the slave select (chip select) of the SD card
void Logger::setSDCardSS(int8_t SDCardSSPin) {
    _SDCardSSPin = SDCardSSPin;
    if (_SDCardSSPin >= 0) {
        pinMode(_SDCardSSPin, OUTPUT);
        digitalWrite(_SDCardSSPin, HIGH);
        MS_DBG(F("Pin"), _SDCardSSPin, F("set as SD Card Slave/Chip Select"));
    }
}


// Sets both pins related to the SD card
void Logger::setSDCardPins(int8_t SDCardSSPin, int8_t SDCardPowerPin) {
    setSDCardPwr(SDCardPowerPin);
    setSDCardSS(SDCardSSPin);
}


// Sets up the wake up pin for an RTC interrupt
// NOTE:  This sets the pin mode but does NOT enable the interrupt!
void Logger::setRTCWakePin(int8_t mcuWakePin, uint8_t wakePinMode) {
    _mcuWakePin  = mcuWakePin;
    _wakePinMode = wakePinMode;
    if (_mcuWakePin >= 0) {
        pinMode(_mcuWakePin, _wakePinMode);
        MS_DBG(F("Pin"), _mcuWakePin, F("set as RTC wake up pin with pin mode"),
               _wakePinMode);
    } else {
#if !defined(MS_USE_RTC_ZERO)
        MS_DBG(F("External clock but no wake pin; logger mcu will not sleep "
                 "between readings!"));
#endif
    }
}


// Sets up a pin for an LED or other way of alerting that data is being logged
void Logger::setAlertPin(int8_t ledPin) {
    _ledPin = ledPin;
    if (_ledPin >= 0) {
        pinMode(_ledPin, OUTPUT);
        MS_DBG(F("Pin"), _ledPin, F("set as LED alert pin"));
    }
}
void Logger::alertOn() {
    if (_ledPin >= 0) { digitalWrite(_ledPin, HIGH); }
}
void Logger::alertOff() {
    if (_ledPin >= 0) { digitalWrite(_ledPin, LOW); }
}


// Sets up a pin for an interrupt to enter testing mode
// NOTE:  This sets the pin mode but does NOT enable the interrupt!
void Logger::setTestingModePin(int8_t buttonPin, uint8_t buttonPinMode) {
    _buttonPin     = buttonPin;
    _buttonPinMode = buttonPinMode;
    if (_buttonPin >= 0) {
        pinMode(_buttonPin, _buttonPinMode);
        MS_DBG(F("Pin"), _buttonPin, F("set as button pin with pin mode"),
               _buttonPinMode);
    }
}


// Sets up the five pins of interest for the logger
void Logger::setLoggerPins(int8_t mcuWakePin, int8_t SDCardSSPin,
                           int8_t SDCardPowerPin, int8_t buttonPin,
                           int8_t ledPin, uint8_t wakePinMode,
                           uint8_t buttonPinMode) {
    MS_DEEP_DBG("Setting all logger pins");
    setSDCardSS(SDCardSSPin);
    setSDCardPwr(SDCardPowerPin);
    setTestingModePin(buttonPin, buttonPinMode);
    setRTCWakePin(mcuWakePin, wakePinMode);
    setAlertPin(ledPin);
}

#if !defined(MS_USE_RTC_ZERO)
void Logger::enableRTCPinISR() {
    // Set up the interrupts on the wake pin
    // WARNING: This MUST be done AFTER beginning the RTC.
    if (_mcuWakePin >= 0) {
        disableInterrupt(_mcuWakePin);
        MS_DEEP_DBG(F("Disabled any previous interrupts attached to"),
                    _mcuWakePin);
        enableInterrupt(_mcuWakePin, loggerClock::rtcISR, CLOCK_INTERRUPT_MODE);
        MS_DEEP_DBG(F("Interrupt loggerClock::rtcISR attached to"), _mcuWakePin,
                    F("with interrupt mode"), CLOCK_INTERRUPT_MODE);
    }
}
#endif

void Logger::enableTestingISR() {
    // Set up the interrupt to be able to enter sensor testing mode
    // NOTE:  Entering testing mode before the sensors have been set-up may
    // give unexpected results.
    // NOTE: The testing ISR itself only wakes the board and sets a flag.
    if (_buttonPin >= 0) {
        disableInterrupt(_buttonPin);
        MS_DEEP_DBG(F("Disabled any previous interrupts attached to"),
                    _buttonPin);
        enableInterrupt(_buttonPin, Logger::testingISR, CHANGE);
        MS_DEEP_DBG(F("Interrupt Logger::testingISR attached to"), _buttonPin,
                    F("with interrupt mode"), CHANGE);
    }
    // reset the testing value, just in case
    Logger::startTesting = false;
}


// ===================================================================== //
// Public functions to get information about the attached variable array
// ===================================================================== //

// Assigns the variable array object
void Logger::setVariableArray(VariableArray* inputArray) {
    _internalArray = inputArray;
}


// Returns the number of variables in the internal array
uint8_t Logger::getArrayVarCount() {
    return _internalArray->getVariableCount();
}


// This gets the name of the parent sensor, if applicable
String Logger::getParentSensorNameAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getParentSensorName();
}
// This gets the name and location of the parent sensor, if applicable
String Logger::getParentSensorNameAndLocationAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]
        ->getParentSensorNameAndLocation();
}
// This gets the variable's name using http://vocabulary.odm2.org/variablename/
String Logger::getVarNameAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getVarName();
}
// This gets the variable's unit using http://vocabulary.odm2.org/units/
String Logger::getVarUnitAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getVarUnit();
}
// This returns a customized code for the variable, if one is given, and a
// default if not
String Logger::getVarCodeAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getVarCode();
}
// This returns the variable UUID, if one has been assigned
String Logger::getVarUUIDAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getVarUUID();
}
// This returns the variable resolution
uint8_t Logger::getVarResolutionAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getResolution();
}
// This returns the current value of the variable as a float
float Logger::getValueAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getValue();
}
// This returns the current value of the variable as a string with the
// correct number of significant figures
String Logger::getValueStringAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getValueString();
}
// This returns a particular value of the variable as a string with the
// correct number of significant figures
String Logger::formatValueStringAtI(uint8_t position_i, float value) {
    return _internalArray->arrayOfVars[position_i]->formatValueString(value);
}


// ===================================================================== //
// Public functions for internet and dataPublishers
// ===================================================================== //

// Set up communications
// Adds a loggerModem object to the logger
// loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
void Logger::attachModem(loggerModem& modem) {
    _logModem = &modem;
    _logModem->setModemTimeZone(_loggerUTCOffset);
    // update the modem pointer for all of the publishers
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
            dataPublishers[i]->setModemPointer(modem);
        }
    }
}


// Takes advantage of the modem to synchronize the clock
bool Logger::syncRTC() {
    bool success = false;
    if (_logModem != nullptr) {
        // Synchronize the RTC with NIST
        PRINTOUT(F("Attempting to connect to the internet and synchronize RTC "
                   "with NIST"));
        PRINTOUT(F("This may take up to two minutes!"));
        if (_logModem->modemWake()) {
            if (_logModem->connectInternet(120000L)) {
                loggerClock::setRTClock(_logModem->getNISTTime(), 0,
                                        epochStart::unix_epoch);
                MS_DBG(F("Current logger time after sync is"),
                       formatDateTime_ISO8601(getNowLocalEpoch()));
                success = true;
                _logModem->updateModemMetadata();
            } else {
                PRINTOUT(F("Could not connect to internet for clock sync."));
            }
        } else {
            PRINTOUT(F("Could not wake modem for clock sync."));
        }
        extendedWatchDog::resetWatchDog();

        // Power down the modem now that we are done with it
        MS_DBG(F("Powering down modem after clock sync."));
        _logModem->disconnectInternet();
        _logModem->modemSleepPowerDown();
    }
    extendedWatchDog::resetWatchDog();
    return success;
}


loggerModem* Logger::registerDataPublisher(dataPublisher* publisher) {
    // find the next empty spot in the publisher array
    uint8_t i = 0;
    for (; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] == publisher) {
            MS_DBG(F("dataPublisher already registered."));
            return nullptr;
        }
        if (dataPublishers[i] == nullptr) break;
    }
    // register the publisher to the empty spot
    dataPublishers[i] = publisher;
    // return a pointer to the modem for the publisher to use
    return _logModem;
}

bool Logger::checkRemotesConnectionNeeded(void) {
    MS_DBG(F("Asking publishers if they need a connection."));

    bool needed = false;
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
            needed = needed || dataPublishers[i]->connectionNeeded();
        }
    }

    return needed;
}

void Logger::publishDataToRemotes(bool forceFlush) {
    MS_DBG(F("Sending out remote data."));

    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
            PRINTOUT(F("\nSending data to ["), i, F("]"),
                     dataPublishers[i]->getEndpoint());
            dataPublishers[i]->publishData(forceFlush);
            extendedWatchDog::resetWatchDog();
        }
    }
}
void Logger::sendDataToRemotes(void) {
    publishDataToRemotes();
}
void Logger::publishMetadataToRemotes() {
    MS_DBG(F("Sending out remote metadata."));

    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
            PRINTOUT(F("\nSending metadata to ["), i, F("]"),
                     dataPublishers[i]->getEndpoint());
            dataPublishers[i]->publishMetadata();
            extendedWatchDog::resetWatchDog();
        }
    }
}


// ===================================================================== //
// Public functions to access the clock in proper format and time zone
// ===================================================================== //

// Sets the static timezone that the data will be logged in - this must be set
void Logger::setLoggerTimeZone(int8_t timeZone) {
    _loggerUTCOffset = timeZone;
// Some helpful prints for debugging
#if !defined(MS_SILENT)
    const char* prtout1 = "Logger timezone is set to UTC";
    if (_loggerUTCOffset == 0) {
        PRINTOUT(prtout1);
    } else if (_loggerUTCOffset > 0) {
        PRINTOUT(prtout1, '+', _loggerUTCOffset);
    } else {
        PRINTOUT(prtout1, _loggerUTCOffset);
    }
#endif
}
int8_t Logger::getLoggerTimeZone(void) {
    return Logger::_loggerUTCOffset;
}
// Sets the static timezone that the RTC is programmed in
// I VERY VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setRTCTimeZone(int8_t timeZone) {
    loggerClock::setRTCOffset(timeZone);
}
int8_t Logger::getRTCTimeZone(void) {
    return loggerClock::getRTCOffset();
}


// This set the offset between the built-in clock and the time zone where
// the data is being recorded.  If your RTC is set in UTC and your logging
// timezone is EST, this should be -5.  If your RTC is set in EST and your
// timezone is EST this does not need to be called.
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setTZOffset(int8_t offset) {
    _loggerRTCOffset = offset;
    loggerClock::setRTCOffset(Logger::_loggerUTCOffset +
                              Logger::_loggerRTCOffset);
    // Some helpful prints for debugging
    if (_loggerRTCOffset == 0) {
        PRINTOUT(F("RTC and Logger are set in the same timezone."));
    } else if (_loggerRTCOffset < 0) {
        PRINTOUT(F("RTC is set"), -1 * _loggerRTCOffset,
                 F("hours ahead of logging timezone"));
    } else {
        PRINTOUT(F("RTC is set"), _loggerRTCOffset,
                 F("hours behind the logging timezone"));
    }
}
int8_t Logger::getTZOffset(void) {
    return Logger::_loggerRTCOffset;
}
uint32_t Logger::getNowLocalEpoch() {
    return loggerClock::getNowAsEpoch(Logger::_loggerUTCOffset,
                                      Logger::_loggerEpoch);
}
uint32_t Logger::getNowUTCEpoch() {
    return loggerClock::getNowAsEpoch(0, Logger::_loggerEpoch);
}

// This converts an epoch time (unix time) into a ISO8601 formatted string.
// It assumes the supplied date/time is in the LOGGER's timezone and adds
// the LOGGER's offset as the time zone offset in the string. code modified
// from parts of the SparkFun RV-8803 library
String Logger::formatDateTime_ISO8601(uint32_t epochSeconds) {
    return loggerClock::formatDateTime_ISO8601(
        epochSeconds, Logger::_loggerUTCOffset, Logger::_loggerEpoch);
}
void Logger::formatDateTime(char* buffer, const char* fmt,
                            uint32_t epochSeconds) {
    loggerClock::formatDateTime(buffer, fmt, epochSeconds,
                                Logger::_loggerEpoch);
}
// This checks that the logger time is within a "sane" range
bool Logger::isRTCSane(void) {
    return loggerClock::isRTCSane();
}


// This sets static variables for the date/time - this is needed so that all
// data outputs (SD, EnviroDIY, serial printing, etc) print the same time
// for updating the sensors - even though the routines to update the sensors
// and to output the data may take several seconds.
// It is not currently possible to output the instantaneous time an individual
// sensor was updated, just a single marked time.  By custom, this should be
// called before updating the sensors, not after.
void Logger::markTime(void) {
    Logger::markedUTCUnixTime   = getNowUTCEpoch();
    Logger::markedLocalUnixTime = markedUTCUnixTime +
        ((uint32_t)Logger::_loggerUTCOffset) * 3600;
}


// This checks to see if the CURRENT time is an even interval of the logging
// rate
bool Logger::checkInterval(void) {
    bool     retval;
    uint32_t checkTime = getNowLocalEpoch();
    int16_t  interval  = _loggingIntervalMinutes;
    if (_remainingShortIntervals > 0) {
        // log the first few samples at an interval of 1 minute so that
        // operation can be quickly verified in the field
        interval = 1;
    }

    MS_DBG(F("Current Unix Timestamp:"), checkTime, F("->"),
           formatDateTime_ISO8601(checkTime));
    MS_DBG(F("Logging interval in seconds:"), (interval * 60));
    MS_DBG(F("Mod of Logging Interval:"), checkTime % (interval * 60));

#if MS_LOGGERBASE_BUTTON_BENCH_TEST == 0
    // If the person has set the button pin **NOT** to be used for "bench
    // testing" (ie, immediate rapid logging) then we instead read this button
    // testing flag to mean "log now." To make that happen, we mark the time
    // here and return true if the flag is set.
    bool testing = Logger::startTesting;
    if ((checkTime % (interval * 60) == 0) || testing) {
        if (testing) {
            MS_DBG(F("Interval irrelevant: This was a button press!"));
        }
#else
    // If the testing mode the button calls **is** set to "bench testing" then
    // we only return true here if it actually is an even interval.
    if (checkTime % (interval * 60) == 0) {
#endif
        // Update the time variables with the current time
        markTime();
        MS_DBG(F("Time marked at (unix):"), Logger::markedLocalUnixTime);
        MS_DBG(F("Time to log!"));
#if MS_LOGGERBASE_BUTTON_BENCH_TEST == 0
        if ((_remainingShortIntervals > 0) && (!testing)) {
#else
        if ((_remainingShortIntervals > 0)) {
#endif
            MS_DBG(F("Within initial 1-minute intervals; "),
                   _remainingShortIntervals, F("left."));
            // once we've marked the time, we need to decrement the remaining
            // short intervals by one. (IFF not in "log now" testing mode.)
            _remainingShortIntervals -= 1;
        }
        retval = true;
    } else {
        MS_DBG(F("Not time yet."));
        retval = false;
    }
    return retval;
}


// This checks to see if the MARKED time is an even interval of the logging rate
bool Logger::checkMarkedInterval(void) {
    int16_t interval = _loggingIntervalMinutes;
    // If we're within the range of our initial short intervals, we're logging,
    // then set the interval to 1.
    if (_remainingShortIntervals > 0) { interval = 1; }

    bool retval;
    MS_DBG(F("Marked Time:"), Logger::markedLocalUnixTime,
           F("Logging interval in seconds:"), (interval * 60),
           F("Mod of Logging Interval:"),
           Logger::markedLocalUnixTime % (interval * 60));

    if (Logger::markedLocalUnixTime != 0 &&
        (Logger::markedLocalUnixTime % (interval * 60) == 0)) {
        MS_DBG(F("Time to log!"));
        // De-increment the number of short intervals after marking
        if (_remainingShortIntervals > 0) {
            MS_DBG(F("Within initial 1-minute intervals. There are "),
                   _remainingShortIntervals, F("left."));
            _remainingShortIntervals -= 1;
        }
        retval = true;
    } else {
        MS_DBG(F("Not time yet."));
        retval = false;
    }
    return retval;
}


// ============================================================================
//  Public Functions for sleeping the logger
// ============================================================================

// Set up the Interrupt Service Request for waking
// In this case, we're doing nothing, we just want the processor to wake
// This must be a static function (which means it can only call other static
// functions.)
void Logger::wakeISR(void) {
    MS_DEEP_DBG(F("\nInterrupt on wake pin!"));
}


// Puts the system to sleep to conserve battery life.
// This DOES NOT sleep or wake the sensors!!
void Logger::systemSleep(void) {
#if !defined(MS_USE_RTC_ZERO)
    // Don't go to sleep unless there's a wake pin!
    if (_mcuWakePin < 0) {
        MS_DBG(F("Use a non-negative wake pin to request sleep!"));
        return;
    }
#endif

    // Send a message that we're getting ready
    MS_DBG(F("Preparing clock interrupts to wake processor"));
    loggerClock::enablePeriodicRTCInterrupts();

    // Enable the RTC ISR
    // NOTE: It seems to work better if we enable this interrupt **AFTER** we've
    // told the clock to fire interrupts. Otherwise the interrupt sometimes
    // fires instantly after the clock interrupts.
#if !defined(MS_USE_RTC_ZERO)
    enableRTCPinISR();
#endif

    // Send one last message before shutting down serial ports
    MS_DBG(F("Putting processor to sleep.  ZZzzz..."));

    // Stop any I2C connections
    // WARNING: After stopping I2C, we can no longer communicate with the RTC!
    // Any calls to get the current time, change the alarm settings, reset the
    // alarm flags, or any other event that involves communication with the RTC
    // will fail!
    MS_DEEP_DBG(F("Ending I2C"));
    // For an AVR board, this function disables the two-wire pin functionality
    // and turns off the internal pull-up resistors.
    // For a SAMD board, this only disables the I2C sercom and does nothing with
    // the pins. The Wire.end() function does **NOT** force the pins low.
    Wire.end();

// Now force the I2C pins to LOW
// I2C devices have a nasty habit of stealing power from the SCL and SDA pins.
// This will only work for the "main" I2C/TWI interface
// WARNING: Any calls to the I2C/Wire library when pins are forced low will
// cause an endless board hang.
#ifdef SDA
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, LOW);
#endif
#ifdef SCL
    pinMode(SCL, OUTPUT);
    digitalWrite(SCL, LOW);
#endif

    // Disable the watch-dog timer
    MS_DEEP_DBG(F("Disabling the watchdog"));
    extendedWatchDog::disableWatchDog();

#if defined(ARDUINO_ARCH_SAMD)

#if !defined(USE_TINYUSB) && defined(USBCON)
    // Detach the USB, iff not using TinyUSB
    MS_DEEP_DBG(F("Detaching USBDevice"));
    USBDevice.detach();   // USB->DEVICE.CTRLB.bit.DETACH = 1;
    USBDevice.end();      // USB->DEVICE.CTRLA.bit.ENABLE = 0; wait for sync;
    USBDevice.standby();  // USB->DEVICE.CTRLA.bit.RUNSTDBY = 0;

#endif

#if defined(__SAMD51__)

    // Clear the FPU interrupt because it can prevent us from sleeping.
    // From Circuit Python:
    // https://github.com/maholli/circuitpython/blob/210ce1d1dc9b1c6c615ff2d3201dde89cb75c555/ports/atmel-samd/supervisor/port.c#L654
    if (__get_FPSCR() & ~(0x9f)) {
        __set_FPSCR(__get_FPSCR() & ~(0x9f));
        (void)__get_FPSCR();
    }

    // Set the sleep config
    // @see #sleep_config_samd51
    MS_DEEP_DBG(F("Setting sleep config"));
    PM->SLEEPCFG.bit.SLEEPMODE = PM_SLEEPCFG_SLEEPMODE_STANDBY_Val;
    // From datasheet 18.6.3.3: A small latency happens between the store
    // instruction and actual writing of the SLEEPCFG register due to bridges.
    // Software must ensure that the SLEEPCFG register reads the desired value
    // before executing a WFI instruction.
    while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_STANDBY_Val);

    // Configure standby mode
    // PM->STDBYCFG.reg = PM_STDBYCFG_RAMCFG(0x0) | PM_STDBYCFG_FASTWKUP(0x0);
    // PM_STDBYCFG_RAMCFG(0x0) = In standby mode, all the system RAM is retained
    // PM_STDBYCFG_FASTWKUP(0x0) = Fast wakeup is disabled.

    // Configure hibernate mode
    // PM->HIBCFG.reg = PM_HIBCFG_RAMCFG(0x0) | PM_HIBCFG_BRAMCFG(0x0);
    // 0x0 = In hibernate mode, all the system RAM is retained
    // 0x0 = In hibernate mode, all the backup RAM is retained.

    //  From datasheet 18.6.3.3: After power-up, the MAINVREG low power mode
    //  takes some time to stabilize. Once stabilized, the INTFLAG.SLEEPRDY
    //  bit is set. Before entering Standby, Hibernate or Backup mode,
    //  software must ensure that the INTFLAG.SLEEPRDY bit is set.
    //  SRGD Note: I believe this only applies at power-on, but it's probably
    //  not a bad idea to check that the flag has been set.
    while (!PM->INTFLAG.bit.SLEEPRDY);
#else
    //^^ SAMD21

    // Don't fully power down flash when in sleep
    // Datasheet Eratta 1.14.2 says this is required.
    NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;

    // Disable systick interrupt:  See
    // https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
    // Due to a hardware bug on the SAMD21, the SysTick interrupts become active
    // before the flash has powered up from sleep, causing a hard fault. To
    // prevent this the SysTick interrupts are disabled before entering sleep
    // mode.
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    // Set the sleep config
    // SCB = System Control Space Base Address
    // SCR = System Control Register
    // SCB_SCR_SLEEPDEEP_Msk = (1UL << 2U), the position of the deep sleep bit
    // in the system control register
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#endif

    // force all pins to minimum power draw levels (tri-state)
    // Set direction (DIR) to 0 (input)
    // Set input enable (PINCFG.INEN) to 0 (disable input buffer)
    // Set pullup enable (PINCFG.PULLEN) to 0 (disable pull up/down)
    if (_tristatePins) {
        for (uint32_t ulPin = 0; ulPin < PINS_COUNT; ulPin++) {
            // Handle the case the pin isn't usable as PIO
            if (g_APinDescription[ulPin].ulPinType != PIO_NOT_A_PIN) {
                if (ulPin != static_cast<uint32_t>(_mcuWakePin) &&
                    ulPin != static_cast<uint32_t>(_buttonPin)) {
                    EPortType port    = g_APinDescription[ulPin].ulPort;
                    uint32_t  pin     = g_APinDescription[ulPin].ulPin;
                    uint32_t  pinMask = (1ul << pin);
                    PORT->Group[port].DIRCLR.reg             = pinMask;
                    PORT->Group[port].PINCFG[pin].bit.INEN   = 0;
                    PORT->Group[port].PINCFG[pin].bit.PULLEN = 0;
                } else {
                    MS_DEEP_DBG(F("Pin"), ulPin, "pin settings retained");
                }
            }
        }
    }

#if defined(__SAMD51__)
    if (_peripheralShutdown) {
        // Disable all unused peripheral clocks and peripheral clocks/timers to
        // reduce power draw during sleep.
        // @see #sleep_peripherals_samd51n for details
        // @see #samd51_clock_other_libraries for a list of which peripherals
        // each of these numbers pertain to
        uint8_t unused_peripherals[] = {
            4,  5,  6,  9,  11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 25,
            26, 27, 28, 29, 30, 31, 32, 33, 38, 39, 42, 43, 44, 45, 46, 47};
        MS_DEEP_DBG(F(
            "Configuring GCLK7 to be disconnected from an oscillator source"));
        GCLK->GENCTRL[7].reg = 0;  // The reset value is 0x00000106 for
                                   // Generator n=0, else 0x00000000
        for (uint8_t upn = 0;
             upn < sizeof(unused_peripherals) / sizeof(unused_peripherals[0]);
             upn++) {
            MS_DEEP_DBG(
                F("Setting peripheral clock"), unused_peripherals[upn],
                F("to be disabled and attached to GCLK7 with no oscillator "
                  "source"));
            GCLK->PCHCTRL[unused_peripherals[upn]].reg =
                GCLK_PCHCTRL_GEN_GCLK7_Val & ~(1 << GCLK_PCHCTRL_CHEN_Pos);
        }
    }
#endif

// Wait until the serial ports have finished transmitting
// This is crucial for the SAMD boards that will continuously wake if they have
// data remaining in the buffer.
#if defined(SERIAL_PORT_USBVIRTUAL)
    SERIAL_PORT_USBVIRTUAL.flush();
#endif
#if defined(MS_OUTPUT)
    MS_OUTPUT.flush();
#endif
#if defined(MS_2ND_OUTPUT)
    MS_2ND_OUTPUT.flush();
#endif

    __DSB();  // Data sync barrier - to ensure outgoing memory accesses
              // complete

    // For tips on failing to sleep, see:
    // https://www.eevblog.com/forum/microcontrollers/crashing-through-__wfi/
    __WFI();

#elif defined(ARDUINO_ARCH_AVR)

// Disable USB if it exists
#ifdef USBCON
    USBCON |= _BV(FRZCLK);  // freeze USB clock
    PLLCSR &= ~_BV(PLLE);   // turn off USB PLL
    USBCON &= ~_BV(USBE);   // disable USB
#endif

    // Set the sleep mode
    // In the avr/sleep.h file, the call names of these 5 sleep modes are:
    // SLEEP_MODE_IDLE         -the least power savings
    // SLEEP_MODE_ADC
    // SLEEP_MODE_PWR_SAVE
    // SLEEP_MODE_STANDBY
    // SLEEP_MODE_PWR_DOWN     -the most power savings
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Temporarily disables interrupts, so no mistakes are made when writing
    // to the processor registers
    noInterrupts();

    // Disable the processor ADC (This must be disabled before the board will
    // power down.)
    // ADCSRA = ADC Control and Status Register A
    // ADEN = ADC Enable
    ADCSRA &= ~_BV(ADEN);

// turn off the brown-out detector, if possible
// BODS = brown-out detector sleep
// BODSE = brown-out detector sleep enable
#if defined(BODS) && defined(BODSE)
    sleep_bod_disable();
#endif

    // disable all power-reduction modules (ie, the processor module clocks)
    // NOTE:  This only shuts down the various clocks on the processor via
    // the power reduction register!  It does NOT actually disable the
    // modules themselves or set the pins to any particular state!  This
    // means that the I2C/Serial/Timer/etc pins will still be active and
    // powered unless they are turned off prior to calling this function.
    power_all_disable();

    // Set the sleep enable bit.
    sleep_enable();

    // Re-enables interrupts so we can wake up again
    interrupts();
// Wait until the serial ports have finished transmitting
// This isn't very important on AVR boards
#if defined(SERIAL_PORT_USBVIRTUAL)
    SERIAL_PORT_USBVIRTUAL.flush();
#endif
#if defined(MS_OUTPUT)
    MS_OUTPUT.flush();
#endif
#if defined(MS_2ND_OUTPUT)
    MS_2ND_OUTPUT.flush();
#endif

    // Actually put the processor into sleep mode.
    // This must happen after the SE bit is set.
    sleep_cpu();

#endif
    // ---------------------------------------------------------------------


    // ---------------------------------------------------------------------
    // -- The portion below this happens on wake up, after any wake ISR's --

#if defined(ARDUINO_ARCH_SAMD)
#if !defined(__SAMD51__)
    // Enable systick interrupt
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
#endif
    // Reattach the USB
#if !defined(USE_TINYUSB) && defined(USBCON)
    MS_DEEP_DBG(F("Reattaching USBDevice"));
    USBDevice.init();
    // ^^ Restarts the bus, including re-attaching the NVIC interrupts
    USBDevice.attach();
    // ^^ USB->DEVICE.CTRLB.bit.DETACH = 0; enables USB interrupts
#endif

    // Re-set the various pin modes - the pins were all set to tri-state to save
    // power above
    // NOTE: Do this whether or not _tristatePins is set because it takes almost
    // no time, won't hurt to reset the pins to the state they're already in,
    // and ensures the pins work after wake.
    setLoggerPins(_mcuWakePin, _SDCardSSPin, _SDCardPowerPin, _buttonPin,
                  _ledPin, _wakePinMode, _buttonPinMode);

#endif

#if defined(ARDUINO_ARCH_AVR)

    // Temporarily disables interrupts, so no mistakes are made when writing
    // to the processor registers
    noInterrupts();

    // Re-enable all power modules (ie, the processor module clocks)
    // NOTE:  This only re-enables the various clocks on the processor!
    // The modules may need to be re-initialized after the clocks re-start.
    power_all_enable();

    // Clear the SE (sleep enable) bit.
    sleep_disable();

    // Re-enable the processor ADC
    ADCSRA |= _BV(ADEN);

    // Re-enables interrupts
    interrupts();

#endif

    // Wake-up message
    MS_DBG(F("\n\n\n... zzzZZ Processor is now awake!"));

    // Re-enable the watch-dog timer
    MS_DEEP_DBG(F("Re-enabling the watchdog"));
    extendedWatchDog::enableWatchDog();

    // Re-start the I2C interface
    MS_DEEP_DBG(F("Restarting I2C"));
    // The Wire.begin() function will set the propper pin modes for SCL and SDA
    // (to INPUT_PULLUP on AVR and SERCOM for SAMD); we don't need to change the
    // pin mode or turn on any resistors..
    Wire.begin();
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);

#if !defined(MS_USE_RTC_ZERO)
    // Detach RTC interrupt the from the wake pin
    MS_DEEP_DBG(F("Disabled interrupts on pin"), _mcuWakePin);
    disableInterrupt(_mcuWakePin);
#endif

    MS_DEEP_DBG(F("Disabling RTC interrupts"));
    loggerClock::disableRTCInterrupts();
    // The logger will now start the next function after the systemSleep
    // function in either the loop or setup
}


// ===================================================================== //
// Public functions for logging data to an SD card
// ===================================================================== //

// This sets a file name, if you want to decide on it in advance
void Logger::setFileName(String& fileName) {
    _fileName = fileName;
}
// Same as above, with a character array (overload function)
void Logger::setFileName(const char* fileName) {
    auto StrName = String(fileName);
    setFileName(StrName);
}

String Logger::generateFileName(bool include_time, const char* extension,
                                const char* filePrefix) {
    if (Logger::markedLocalUnixTime == 0) { markTime(); }
    const char* use_prefix = filePrefix != nullptr ? filePrefix : getLoggerID();
    uint8_t     len_underscore      = strlen(use_prefix) > 0 ? 1 : 0;
    uint8_t     len_time            = include_time ? 15 : 8;
    char        time_buff[len_time] = {'\0'};
    if (include_time) {
        formatDateTime(time_buff, "%Y%m%d_%H%M%S", Logger::markedLocalUnixTime);
    } else {
        formatDateTime(time_buff, "%Y%m%d", Logger::markedLocalUnixTime);
    }
    String filename = String(use_prefix);
    if (len_underscore) { filename += String("_"); }
    filename += String(time_buff);
    filename += String(extension);
    MS_DBG(F("Generated filename: "), filename);
    return filename;
}


// This generates a file name from the logger id and the current date
// This will be used if the setFileName function is not called before the
// begin() function is called.
void Logger::generateAutoFileName(void) {
    // Generate the file name from logger ID and date
    auto fileName = generateFileName(false, ".csv");
    setFileName(fileName);
}


/**
 * @brief This is a PRE-PROCESSOR MACRO to speed up generating header rows
 *
 * THIS IS NOT A FUNCTION, it is a pre-processor macro
 */
#define STREAM_CSV_ROW(firstCol, function)                       \
    stream->print("\"");                                         \
    stream->print(firstCol);                                     \
    stream->print("\",");                                        \
    for (uint8_t i = 0; i < getArrayVarCount(); i++) {           \
        stream->print("\"");                                     \
        stream->print(function);                                 \
        stream->print("\"");                                     \
        if (i + 1 != getArrayVarCount()) { stream->print(","); } \
    }                                                            \
    stream->println();

// This sends a file header out over an Arduino stream
void Logger::printFileHeader(Stream* stream) {
    // Very first line of the header is the logger ID
    stream->print(F("Data Logger: "));
    stream->println(_loggerID);

    // Next we're going to print the current file name
    stream->print(F("Data Logger File: "));
    stream->println(_fileName);

    // Adding the sampling feature UUID (only applies to EnviroDIY logger)
    if (strlen(_samplingFeatureUUID) > 1) {
        stream->print(F("Sampling Feature UUID: "));
        stream->print(_samplingFeatureUUID);
        stream->println(',');
    }

    // Next line will be the parent sensor names
    STREAM_CSV_ROW(F("Sensor Name:"), getParentSensorNameAtI(i))
    // Next comes the ODM2 variable name
    STREAM_CSV_ROW(F("Variable Name:"), getVarNameAtI(i))
    // Next comes the ODM2 unit name
    STREAM_CSV_ROW(F("Result Unit:"), getVarUnitAtI(i))
    // Next comes the variable UUIDs
    // We'll only add UUID's if we see a UUID for the first variable
    if (getVarUUIDAtI(0).length() > 1) {
        STREAM_CSV_ROW(F("Result UUID:"), getVarUUIDAtI(i))
    }

    // We'll finish up the the custom variable codes
    String dtRowHeader = F("Date and Time in UTC");
    if (_loggerUTCOffset > 0) {
        dtRowHeader += '+' + _loggerUTCOffset;
    } else if (_loggerUTCOffset < 0) {
        dtRowHeader += _loggerUTCOffset;
    }
    STREAM_CSV_ROW(dtRowHeader, getVarCodeAtI(i))
}


// This prints a comma separated list of volues of sensor data - including the
// time -  out over an Arduino stream
void Logger::printSensorDataCSV(Stream* stream) {
    String csvString = "";
    String iso8601   = formatDateTime_ISO8601(Logger::markedLocalUnixTime);
    iso8601.replace("T", " ");
    csvString += iso8601.substring(0, 19);
    csvString += ',';
    stream->print(csvString);
    for (uint8_t i = 0; i < getArrayVarCount(); i++) {
        stream->print(getValueStringAtI(i));
        if (i + 1 != getArrayVarCount()) { stream->print(','); }
    }
    stream->println();
}

// Protected helper function - This checks if the SD card is available and ready
bool Logger::initializeSDCard(void) {
    // If we don't know the slave select of the sd card, we can't use it
    if (_SDCardSSPin < 0) {
        PRINTOUT(F("Slave/Chip select pin for SD card has not been set."));
        PRINTOUT(F("Data will not be saved!"));
        return false;
    }
    // Initialise the SD card
    // If you have a dedicated SPI for the SD card, you can overrride the
    // default shared SPI using this:

    // SdSpiConfig(SdCsPin_t cs, uint8_t opt, uint32_t maxSpeed, SpiPort_t*
    // port);
    // sd.begin(SdSpiConfig(_SDCardSSPin, DEDICATED_SPI, SPI_FULL_SPEED));
    // else the default writes to
    // sd.begin(SdSpiConfig(_SDCardSSPin, SHARED_SPI, SPI_FULL_SPEED));
    // With DEDICATED_SPI, multi-block SD I/O may be used for better
    // performance. The SPI bus may not be shared with other devices in this
    // mode.
    // NOTE: SPI_FULL_SPEED is 50MHz

    // In order to prevent the SD card library from calling SPI.begin ever
    // again, we need to make sure we set up the SD card object with a
    // SdSpiConfig object with option "USER_SPI_BEGIN."
    // so we extend the options to be
    // sd.begin(SdSpiConfig(_SDCardSSPin, DEDICATED_SPI | USER_SPI_BEGIN,
    // SPI_FULL_SPEED));

#if (defined(ARDUINO_ARCH_SAMD)) && !defined(__SAMD51__)
    // Dispite the 48MHz clock speed, the max SPI speed of a SAMD21 is 12 MHz
    // see https://github.com/arduino/ArduinoCore-samd/pull/292
    // The Adafruit SAMD core does NOT automatically manage the SPI speed, so
    // this needs to be set.
    SdSpiConfig customSdConfig(static_cast<SdCsPin_t>(_SDCardSSPin),
                               (uint8_t)(DEDICATED_SPI), SD_SCK_MHZ(12),
                               &SDCARD_SPI);
#else
    // The SAMD51 is fast enough to handle SPI_FULL_SPEED=SD_SCK_MHZ(50).
    // The SPI library of the Adafruit/Arduino AVR core will automatically
    // adjust the full speed of the SPI clock down to whatever the board can
    // handle.
    SdSpiConfig customSdConfig(static_cast<SdCsPin_t>(_SDCardSSPin),
                               (uint8_t)(DEDICATED_SPI), SPI_FULL_SPEED,
                               &SDCARD_SPI);
#endif

    if (!sd.begin(customSdConfig)) {
        PRINTOUT(F("Error: SD card failed to initialize or is missing."));
        PRINTOUT(F("Data will not be saved!"));
        return false;
    } else {
        // skip everything else if there's no SD card, otherwise it might hang
        MS_DBG(F("Successfully connected to SD Card with card/slave select on "
                 "pin"),
               _SDCardSSPin);
        return true;
    }
}

// This function is used to automatically mark files as
// created/accessed/modified when operations are done by the SdFat library. User
// provided date time callback function. See SdFile::dateTimeCallback() for
// usage.
void Logger::fileDateTimeCallback(uint16_t* date, uint16_t* time) {
    // Create a temporary variable for the epoch time
    // NOTE: time_t is a typedef for uint32_t, defined in time.h
    time_t t = getNowLocalEpoch();
    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp = gmtime(&t);
    MS_DEEP_DBG(F("Time components: "), tmp->tm_year, F(" - "), tmp->tm_mon + 1,
                F(" - "), tmp->tm_mday, F("    "), tmp->tm_hour, F(" : "),
                tmp->tm_min, F(" : "), tmp->tm_sec);

    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday);

    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}


// Protected helper function - This sets a timestamp on a file
void Logger::setFileTimestamp(File& fileToStamp, uint8_t stampFlag) {
    // Create a temporary variable for the epoch time
    // NOTE: time_t is a typedef for uint32_t, defined in time.h
    time_t t = getNowLocalEpoch();
    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp = gmtime(&t);
    MS_DEEP_DBG(F("Time components: "), tmp->tm_year, F(" - "), tmp->tm_mon + 1,
                F(" - "), tmp->tm_mday, F("    "), tmp->tm_hour, F(" : "),
                tmp->tm_min, F(" : "), tmp->tm_sec);
    fileToStamp.timestamp(stampFlag, tmp->tm_year + 1900, tmp->tm_mon + 1,
                          tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}


// Protected helper function - This opens or creates a file, converting a string
// file name to a character file name
bool Logger::openFile(String& filename, bool createFile,
                      bool writeDefaultHeader) {
    // Initialise the SD card
    // skip everything else if there's no SD card, otherwise it might hang
    if (!initializeSDCard()) return false;

    // Convert the string filename to a character file name for SdFat
    unsigned int fileNameLength = filename.length() + 1;
    char         charFileName[fileNameLength];
    filename.toCharArray(charFileName, fileNameLength);

    // First attempt to open an already existing file (in write mode), so we
    // don't try to re-create something that's already there.
    // This should also prevent the header from being written over and over
    // in the file.
    if (logFile.open(charFileName, O_WRITE | O_AT_END)) {
        MS_DBG(F("Opened existing file:"), filename);
        return true;
    } else if (createFile) {
        // Create and then open the file in write mode
        if (logFile.open(charFileName, O_CREAT | O_WRITE | O_AT_END)) {
            MS_DBG(F("Created new file:"), filename);
            // Write out a header, if requested
            if (writeDefaultHeader) {
                // Add header information
                printFileHeader(&logFile);
// Print out the header for debugging
#if defined(MS_DEBUGGING_STD)
                MS_DBG(F("\n \\/---- File Header ----\\/"));
                printFileHeader(&MS_OUTPUT);
#if defined(MS_2ND_OUTPUT)
                printFileHeader(&MS_2ND_OUTPUT);
#endif
                PRINTOUT('\n');
#endif
            }
            return true;
        } else {
            // Return false if we couldn't create the file
            MS_DBG(F("Unable to create new file:"), filename);
            return false;
        }
    } else {
        // Return false if we couldn't access the file (and were not told to
        // create it)
        MS_DBG(F("Unable to to write to file:"), filename);
        return false;
    }
}


// These functions create a file on the SD card with the given filename and
// set the proper timestamps to the file.
// The filename may either be the one set by
// setFileName(String)/setFileName(void) or can be specified in the function. If
// specified, it will also write a header to the file based on the sensors in
// the group. This can be used to force a logger to create a file with a
// secondary file name.
bool Logger::createLogFile(String& filename, bool writeDefaultHeader) {
    // Attempt to create and open a file
    if (openFile(filename, true, writeDefaultHeader)) {
        // Close the file to save it (only do this if we'd opened it)
        logFile.close();
        PRINTOUT(F("Data will be saved as"), _fileName);
        return true;
    } else {
        PRINTOUT(F("Unable to create a file to save data to!"));
        return false;
    }
}
bool Logger::createLogFile(bool writeDefaultHeader) {
    if (_fileName == "") generateAutoFileName();
    return createLogFile(_fileName, writeDefaultHeader);
}


// These functions write a file on the SD card with the given filename and
// set the proper timestamps to the file.
// The filename may either be the one set by
// setFileName(String)/setFileName(void) or can be specified in the function. If
// the file does not already exist, the file will be created. This can be used
// to force a logger to write to a file with a secondary file name.
bool Logger::logToSD(String& filename, String& rec) {
    // First attempt to open the file without creating a new one
    if (!openFile(filename, false, false)) {
        PRINTOUT(F("Could not write to existing file on SD card, attempting to "
                   "create a file!"));
        // Next try to create the file, bail if we couldn't create it
        // This will not attempt to generate a new file name or add a header!
        if (!openFile(filename, true, false)) {
            PRINTOUT(F("Unable to write to SD card!"));
            return false;
        }
    }

    // If we could successfully open or create the file, write the data to it
    logFile.println(rec);
    // Echo the line to the serial port
    PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/"));
    PRINTOUT(rec);

    // Close the file to save it
    logFile.close();
    return true;
}
bool Logger::logToSD(String& rec) {
    // Get a new file name if the name is blank
    if (_fileName == "") generateAutoFileName();
    return logToSD(_fileName, rec);
}
// NOTE:  This is structured differently than the version with a string input
// record.  This is to avoid the creation/passing of very long strings.
bool Logger::logToSD(void) {
    // Get a new file name if the name is blank
    if (_fileName == "") generateAutoFileName();

    // First attempt to open the file without creating a new one
    if (!openFile(_fileName, false, false)) {
        // Next try to create a new file, bail if we couldn't create it
        // Generate a filename with the current date, if the file name isn't set
        if (_fileName == "") generateAutoFileName();
        // Do add a default header to the new file!
        if (!openFile(_fileName, true, true)) {
            PRINTOUT(F("Unable to write to SD card!"));
            return false;
        }
    }

    // Write the data
    printSensorDataCSV(&logFile);
// Echo the line to the serial port
#if !defined(MS_SILENT)
    PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/"));
    printSensorDataCSV(&MS_OUTPUT);
#if defined(MS_2ND_OUTPUT)
    printSensorDataCSV(&MS_2ND_OUTPUT);
#endif
    PRINTOUT('\n');
#endif

    // Close the file to save it
    logFile.close();
    return true;
}


// ===================================================================== //
// Public functions for a "sensor testing" mode
// ===================================================================== //
// A static function if you'd prefer to enter testing based on an interrupt
void Logger::testingISR() {
    MS_DEEP_DBG(F("\nTesting interrupt!"));
#if defined(MS_LOGGERBASE_DEBUG_DEEP)
    PRINTOUT(" ");
#endif
    if (!Logger::isTestingNow && !Logger::isLoggingNow) {
        Logger::startTesting = true;
        MS_DEEP_DBG(F("Testing flag has been set."));
    }
}


// This defines what to do in the bench testing mode
void Logger::benchTestingMode(bool sleepBeforeReturning) {
    // If bench testing is not enabled, return now. Allows the function body to
    // be compiled out if this functionality is disabled.
    if (!(MS_LOGGERBASE_BUTTON_BENCH_TEST)) return;
    // Flag to notify that we're in testing mode
    Logger::isTestingNow = true;
    // Unset the startTesting flag
    Logger::startTesting = false;

    PRINTOUT(F("------------------------------------------"));
    PRINTOUT(F("Entering bench testing mode"));
    delay(100);  // This seems to prevent crashes, no clue why ....

    // Get the modem ready

    bool gotInternetConnection = false;
    if (_logModem != nullptr) {
        MS_DBG(F("Waking up"), _logModem->getModemName(), F("..."));
        if (_logModem->modemWake()) {
            // Connect to the network
            extendedWatchDog::resetWatchDog();
            MS_DBG(F("Connecting to the Internet..."));
            if (_logModem->connectInternet()) {
                gotInternetConnection = true;
                // Publish data to remotes
                extendedWatchDog::resetWatchDog();
            }
        }
    }

    // Power up all of the sensors
    _internalArray->sensorsPowerUp();

    // Wake up all of the sensors
    _internalArray->sensorsWake();

    // Update the sensors and print out data 25 times
    for (uint8_t i = 0; i < 25; i++) {
        PRINTOUT(F("------------------------------------------"));

        // Update the modem metadata
        if (gotInternetConnection) { _logModem->updateModemMetadata(); }

        extendedWatchDog::resetWatchDog();
        // Update the values from all attached sensors
        // NOTE:  NOT using complete update because we want the sensors to be
        // left on between iterations in testing mode.
        _internalArray->updateAllSensors();
        // Print out the current logger time
        PRINTOUT(F("Current logger time is"),
                 formatDateTime_ISO8601(getNowLocalEpoch()));
        PRINTOUT(F("-----------------------"));
// Print out the sensor data
#if !defined(MS_SILENT)
        _internalArray->printSensorData(&MS_OUTPUT);
#if defined(MS_2ND_OUTPUT)
        _internalArray->printSensorData(&MS_2ND_OUTPUT);
#endif
#endif
        PRINTOUT(F("-----------------------"));
        extendedWatchDog::resetWatchDog();

        delay(5000);
        extendedWatchDog::resetWatchDog();
    }

    // Put sensors to sleep
    _internalArray->sensorsSleep();
    _internalArray->sensorsPowerDown();

    // Turn the modem off
    if (_logModem != nullptr) {
        if (gotInternetConnection) { _logModem->disconnectInternet(); }
        _logModem->modemSleepPowerDown();
    }

    PRINTOUT(F("Exiting testing mode"));
    PRINTOUT(F("------------------------------------------"));
    extendedWatchDog::resetWatchDog();

    // Unset testing mode flag
    Logger::isTestingNow = false;

    if (sleepBeforeReturning) {
        // Sleep
        systemSleep();
    }
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //

// This does all of the setup that can't happen in the constructors
// That is, things that require the actual processor/MCU to do something
// rather than the compiler to do something.
void Logger::begin(const char* loggerID, int16_t loggingIntervalMinutes,
                   VariableArray* inputArray) {
    setLoggerID(loggerID);
    setLoggingInterval(loggingIntervalMinutes);
    begin(inputArray);
}
void Logger::begin(VariableArray* inputArray) {
    setVariableArray(inputArray);
    begin();
}
void Logger::begin() {
    MS_DBG(F("Logger ID is:"), _loggerID);
    MS_DBG(F("Logger is set to record at"), _loggingIntervalMinutes,
           F("minute intervals."));

    // Set all of the pin modes
    // NOTE:  This must be done here at run time not at compile time
    setLoggerPins(_mcuWakePin, _SDCardSSPin, _SDCardPowerPin, _buttonPin,
                  _ledPin, _wakePinMode, _buttonPinMode);

    uint16_t realWatchDogTime = max(_loggingIntervalMinutes * 2, 5);
    MS_DBG(F("Setting up a watch-dog timer to restart the board after"),
           realWatchDogTime,
           F("minutes without being fed (2x logging interval)"));
    extendedWatchDog::setupWatchDog((uint32_t)(realWatchDogTime * 60));
    // Enable the watchdog
    MS_DEEP_DBG(F("Enabling the watchdog"));
    extendedWatchDog::enableWatchDog();

#if defined(ARDUINO_ARCH_SAMD)
    // Set the resolution for the processor ADC, only applies to SAMD
    // boards.
    MS_DBG(F("Setting analog read resolution for onboard ADC to"),
           MS_PROCESSOR_ADC_RESOLUTION, F("bit"));
    analogReadResolution(MS_PROCESSOR_ADC_RESOLUTION);
#endif
    // Set the analog reference mode for processor ADC voltage measurements.
    // If possible, to get the best results, an external reference should be
    // used.
    MS_DBG(F("Setting analog read reference mode for onboard ADC to mode"),
           MS_PROCESSOR_ADC_REFERENCE_MODE);
    analogReference(MS_PROCESSOR_ADC_REFERENCE_MODE);

#if defined(ARDUINO_ARCH_SAMD)
    MS_DBG(F("Disabling the USB on standby to lower sleep current"));
    USB->DEVICE.CTRLA.bit.ENABLE = 0;         // Disable the USB peripheral
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE);  // Wait for synchronization
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 0;       // Deactivate run on standby
    USB->DEVICE.CTRLA.bit.ENABLE   = 1;       // Enable the USB peripheral
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE);  // Wait for synchronization
#if !defined(__SAMD51__)
    // Keep the voltage regulator running in standby
    // doing this just in case the various periperals try to suck too much power
    // while the board is asleep.
    SYSCTRL->VREG.bit.RUNSTDBY = 1;
#endif
#endif

#if defined(ARDUINO_ARCH_SAMD)
    if (_mcuWakePin >= 0 || _buttonPin >= 0) {
        // NOTE: This has to be done AFTER setupWatchDog, because setupWatchDog
        // calls config32kOSC() and configureClockGenerator(), both of which are
        // needed before we can call configureEICClock(). This also should
        // happen after *both* the wake pin and testing mode pin have been set
        // to ensure that all clock changes WInterrupts.c have already been
        // applied. If `attachInterrupt` within WInterrupts.c is called again,
        // the `extendedWatchDog::configureEICClock()` function must be rerun on
        // the SAMD21. If `__initialize()` within WInterrupts.c is called again
        // the `extendedWatchDog::configureEICClock()` function must be run
        // again for both the SAMD51 and the SAMD21.
        MS_DEEP_DBG(F("Configuring the EIC clock"));
        extendedWatchDog::configureEICClock();
    }
#endif

// Set the pins for I2C
#ifdef SDA
    MS_DBG(F("Setting SDA pin"), SDA, F("to INPUT_PULLUP"));
    pinMode(SDA, INPUT_PULLUP);  // set as input with the pull-up on
#endif
#ifdef SCL
    MS_DBG(F("Setting SCL pin"), SCL, F("to INPUT_PULLUP"));
    pinMode(SCL, INPUT_PULLUP);
#endif
    MS_DBG(F("Beginning wire (I2C)"));
    Wire.begin();
    extendedWatchDog::resetWatchDog();

    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);

    MS_DEEP_DBG(F("Beginning the logger clock"));
    loggerClock::begin();
    PRINTOUT(F("Current localized logger time is:"),
             formatDateTime_ISO8601(getNowLocalEpoch()));
    // Reset the watchdog
    extendedWatchDog::resetWatchDog();

    // Begin the internal array
    _internalArray->begin();
    PRINTOUT(F("This logger has a variable array with"), getArrayVarCount(),
             F("variables, of which"),
             getArrayVarCount() - _internalArray->getCalculatedVariableCount(),
             F("come from"), _internalArray->getSensorCount(), F("sensors and"),
             _internalArray->getCalculatedVariableCount(),
             F("are calculated."));

    if (_samplingFeatureUUID != nullptr) {
        PRINTOUT(F("Sampling feature UUID is:"), _samplingFeatureUUID);
    }


    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
            PRINTOUT(F("Data will be published to ["), i, F("]"),
                     dataPublishers[i]->getEndpoint());
        }
    }
    // Enable the testing ISR
    enableTestingISR();

    PRINTOUT(F("Logger portion of setup finished.\n"));
}


// This is a one-and-done to log data
void Logger::logData(bool sleepBeforeReturning) {
    // Reset the watchdog
    extendedWatchDog::resetWatchDog();

    // If bench testing is enabled and button was pressed, do the bench test
    if ((MS_LOGGERBASE_BUTTON_BENCH_TEST) && Logger::startTesting) {
        benchTestingMode();
    } else if (checkInterval()) {
        // We've checked that the current time is an even interval of the
        // logging interval or we were requested to log by the button

        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        // Reset the watchdog
        extendedWatchDog::resetWatchDog();

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        alertOn();
        // Power up the SD Card
        // TODO(SRGDamia1):  Decide how much delay is needed between turning on
        // the card and writing to it.  Could we turn it on just before writing?
        turnOnSDcard(false);

        // Do a complete sensor update
        MS_DBG(F("    Running a complete sensor update..."));
        extendedWatchDog::resetWatchDog();
        _internalArray->completeUpdate();
        extendedWatchDog::resetWatchDog();

        // Create a csv data record and save it to the log file
        logToSD();
        // Cut power from the SD card, waiting for housekeeping
        turnOffSDcard(true);

        // Turn off the LED
        alertOff();
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
        // Acknowledge testing button if pressed
        Logger::startTesting = false;
    }

    if (sleepBeforeReturning) {
        // Sleep
        systemSleep();
    }
}

// This is a one-and-done to log data
void Logger::logDataAndPublish(bool sleepBeforeReturning) {
    // Reset the watchdog
    extendedWatchDog::resetWatchDog();

    // If bench testing is enabled and button was pressed, do the bench test
    if ((MS_LOGGERBASE_BUTTON_BENCH_TEST) && Logger::startTesting) {
        benchTestingMode();
    } else if (checkInterval()) {
        // We've checked that the current time is an even interval of the
        // logging interval or we were requested to log by the button

        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        // Reset the watchdog
        extendedWatchDog::resetWatchDog();

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        alertOn();
        // Power up the SD Card
        // TODO(SRGDamia1):  Decide how much delay is needed between turning on
        // the card and writing to it.  Could we turn it on just before writing?
        turnOnSDcard(false);

        // Do a complete update on the variable array.
        // This this includes powering all of the sensors, getting updated
        // values, and turing them back off.
        // NOTE:  The wake function for each sensor should force sensor setup to
        // run if the sensor was not previously set up.
        MS_DBG(F("Running a complete sensor update..."));
        extendedWatchDog::resetWatchDog();
        _internalArray->completeUpdate();
        extendedWatchDog::resetWatchDog();

// Print out the sensor data
#if !defined(MS_SILENT)
        PRINTOUT(" ");
        _internalArray->printSensorData(&MS_OUTPUT);
#if defined(MS_2ND_OUTPUT)
        _internalArray->printSensorData(&MS_2ND_OUTPUT);
#endif
        PRINTOUT(" ");
#endif

        // Create a csv data record and save it to the log file
        logToSD();

        // Flush the publisher buffers (if any) if we have been invoked by the
        // testing button, otherwise follow settings from
        // MS_ALWAYS_FLUSH_PUBLISHERS
        bool forceFlush = Logger::startTesting || MS_ALWAYS_FLUSH_PUBLISHERS;

        // Sync the clock at noon
        bool clockSyncNeeded = (Logger::markedLocalUnixTime != 0 &&
                                Logger::markedLocalUnixTime % 86400 == 43200) ||
            !isRTCSane();
        bool connectionNeeded = checkRemotesConnectionNeeded() ||
            clockSyncNeeded || forceFlush;

        if (_logModem != nullptr && connectionNeeded) {
            MS_DBG(F("Waking up"), _logModem->getModemName(), F("..."));
            if (_logModem->modemWake()) {
                // Connect to the network
                extendedWatchDog::resetWatchDog();
                MS_DBG(F("Connecting to the Internet..."));
                if (_logModem->connectInternet(240000L)) {
                    // Publish data to remotes
                    extendedWatchDog::resetWatchDog();
                    publishDataToRemotes(forceFlush);
                    extendedWatchDog::resetWatchDog();

                    if (clockSyncNeeded) {
                        MS_DBG(F("Running a daily clock sync..."));
                        loggerClock::setRTClock(_logModem->getNISTTime(), 0,
                                                epochStart::unix_epoch);
                        MS_DBG(F("Current logger time after sync is"),
                               formatDateTime_ISO8601(getNowLocalEpoch()));
                        extendedWatchDog::resetWatchDog();
                    }

                    // Update the modem metadata
                    MS_DBG(F("Updating modem metadata..."));
                    _logModem->updateModemMetadata();

                    // Disconnect from the network
                    MS_DBG(F("Disconnecting from the Internet..."));
                    _logModem->disconnectInternet();
                } else {
                    MS_DBG(F("Could not connect to the internet!"));
                    extendedWatchDog::resetWatchDog();
                }
            }
            // Turn the modem off
            _logModem->modemSleepPowerDown();
        } else if (_logModem != nullptr) {
            MS_DBG(F("Nobody needs it so publishing without connecting..."));
            // Call publish function without connection
            extendedWatchDog::resetWatchDog();
            publishDataToRemotes(false);  // can't flush without a connection
            extendedWatchDog::resetWatchDog();
        }


        // Cut power from the SD card - without additional housekeeping wait
        // TODO(SRGDamia1):  Do some sort of verification that minimum 1 sec has
        // passed for internal SD card housekeeping before cutting power -
        // although it seems very unlikely based on my testing that less than
        // one second would be taken up in publishing data to remotes.
        turnOffSDcard(false);

        // Turn off the LED
        alertOff();
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
        // Acknowledge testing button if pressed
        Logger::startTesting = false;
    }

    if (sleepBeforeReturning) {
        // Call the processor sleep
        systemSleep();
    }
}


void Logger::makeInitialConnections() {
    // Reset the watchdog
    extendedWatchDog::resetWatchDog();

    if (_logModem != nullptr) {
        // Synchronize the RTC with NIST
        PRINTOUT(F("Attempting to connect to the internet, synchronize RTC "
                   "with NIST, and publish metadata to remotes."));
        PRINTOUT(F("This may take up to two minutes!"));
        if (_logModem->modemWake()) {
            if (_logModem->connectInternet(120000L)) {
                loggerClock::setRTClock(_logModem->getNISTTime(), 0,
                                        epochStart::unix_epoch);
                MS_DBG(F("Current logger time after sync is"),
                       formatDateTime_ISO8601(getNowLocalEpoch()));
                _logModem->updateModemMetadata();

                MS_DBG(F("Publishing configuration metadata to remotes"));
                publishMetadataToRemotes();
            } else {
                MS_DBG(F("Could not make initial internet connection!"));
            }
        } else {
            MS_DBG(F("Could not wake modem for initial internet connection."));
        }
        extendedWatchDog::resetWatchDog();

        // Power down the modem now that we are done with it
        MS_DBG(F("Powering down modem after clock sync."));
        _logModem->disconnectInternet();
        _logModem->modemSleepPowerDown();
    }
    extendedWatchDog::resetWatchDog();
}
