/**
 * @file LoggerBase.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Logger class.
 */

#include "LoggerBase.h"
#include "dataPublisherBase.h"

/**
 * @brief To prevent compiler/linker crashes with enable interrupt library, we
 * must define LIBCALL_ENABLEINTERRUPT before importing EnableInterrupt within a
 * library.
 */
#define LIBCALL_ENABLEINTERRUPT
// To handle external and pin change interrupts
#include <EnableInterrupt.h>
// For all i2c communication, including with the real time clock
#include <Wire.h>


// Initialize the static timezone
int8_t Logger::_loggerTimeZone = 0;
// Initialize the static time adjustment
int8_t Logger::_loggerRTCOffset = 0;
// Initialize the static timestamps
uint32_t Logger::markedLocalEpochTime = 0;
uint32_t Logger::markedUTCEpochTime   = 0;
// Initialize the testing/logging flags
volatile bool Logger::isLoggingNow = false;
volatile bool Logger::isTestingNow = false;
volatile bool Logger::startTesting = false;

// Initialize the RTC for the SAMD boards
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAMD_ZERO)
RTCZero Logger::zero_sleep_rtc;
#endif

#if defined USE_RTCLIB
// or change to 
// USE_RTC_EXTPHY rtcExtPhy;
USE_RTCLIB  rtcExtPhy;
// For RTClib.h:DateTime(uint32_t) use secs since 1970
#define DateTimeClass(varNam, epochTime) DateTime varNam(epochTime);
#else
// For Sodaq_DS3231.h:DateTime(long) uses secs since 2000
#define DateTimeClass(varNam, epochTime) \
    DateTime varNam((long)((uint64_t)(epochTime - EPOCH_TIME_DTCLASS)));
#endif  //  USE_RTCLIB
#define TEMP_BUFFER_SZ 37

// Constructors
Logger::Logger(const char* loggerID, uint16_t loggingIntervalMinutes,
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

    // Set the initial pin values
    // NOTE: Only setting values here, not the pin mode.
    // The pin mode can only be set at run time, not here at compile time.

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        dataPublishers[i] = nullptr;
    }
}
Logger::Logger(const char* loggerID, uint16_t loggingIntervalMinutes,
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

// Sets/Gets the logging interval
void Logger::setLoggingInterval(uint16_t loggingIntervalMinutes) {
    _loggingIntervalMinutes = loggingIntervalMinutes;
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
        digitalWrite(_SDCardPowerPin, HIGH);
        // TODO(SRGDamia1):  figure out how long to wait
        if (waitToSettle) { delay(6); }
    }
}
void Logger::turnOffSDcard(bool waitForHousekeeping) {
    if (_SDCardPowerPin >= 0) {
        // TODO(SRGDamia1): set All SPI pins to INPUT?
        // TODO(SRGDamia1): set ALL SPI pins HIGH (~30k pull-up)
        pinMode(_SDCardPowerPin, OUTPUT);
        digitalWrite(_SDCardPowerPin, LOW);
        // TODO(SRGDamia1):  wait in lower power mode
        if (waitForHousekeeping) {
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
    _mcuWakePin = mcuWakePin;
    if (_mcuWakePin >= 0) {
        pinMode(_mcuWakePin, wakePinMode);
        MS_DBG(F("Pin"), _mcuWakePin, F("set as RTC wake up pin"));
    } else {
        MS_DBG(F("Logger mcu will not sleep between readings!"));
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
void Logger::setTestingModePin(int8_t buttonPin, uint8_t buttonPinMode) {
    _buttonPin = buttonPin;

    // Set up the interrupt to be able to enter sensor testing mode
    // NOTE:  Entering testing mode before the sensors have been set-up may
    // give unexpected results.
    if (_buttonPin >= 0) {
        pinMode(_buttonPin, buttonPinMode);
        enableInterrupt(_buttonPin, Logger::testingISR, CHANGE);
        MS_DBG(F("Button on pin"), _buttonPin,
               F("can be used to enter sensor testing mode."));
    }
}


// Sets up the five pins of interest for the logger
void Logger::setLoggerPins(int8_t mcuWakePin, int8_t SDCardSSPin,
                           int8_t SDCardPowerPin, int8_t buttonPin,
                           int8_t ledPin, uint8_t wakePinMode,
                           uint8_t buttonPinMode) {
    setRTCWakePin(mcuWakePin, wakePinMode);
    setSDCardSS(SDCardSSPin);
    setSDCardPwr(SDCardPowerPin);
    setTestingModePin(buttonPin, buttonPinMode);
    setAlertPin(ledPin);
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
// This returns the current value of the variable as a string with the
// correct number of significant figures
String Logger::getValueStringAtI(uint8_t position_i) {
    return _internalArray->arrayOfVars[position_i]->getValueString();
}


// ===================================================================== //
// Public functions for internet and dataPublishers
// ===================================================================== //

// Set up communications
// Adds a loggerModem objct to the logger
// loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
void Logger::attachModem(loggerModem& modem) {
    _logModem = &modem;
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
                setRTClock(_logModem->getNISTTime());
                success = true;
                _logModem->updateModemMetadata();
            } else {
                PRINTOUT(F("Could not connect to internet for clock sync."));
            }
        } else {
            PRINTOUT(F("Could not wake modem for clock sync."));
        }
        watchDogTimer.resetWatchDog();
        // Power down the modem - but only if there will be more than 15 seconds
        // before the NEXT logging interval - it can take the modem that long to
        // shut down

        uint32_t setupFinishTime = getNowLocalEpoch();
        if (setupFinishTime % (_loggingIntervalMinutes * 60) > 15) {
            MS_DBG(F("At"), formatDateTime_ISO8601(setupFinishTime), F("with"),
                   setupFinishTime % (_loggingIntervalMinutes * 60),
                   F("seconds until next logging interval, putting modem to "
                     "sleep"));
            _logModem->disconnectInternet();
            _logModem->modemSleepPowerDown();
        } else {
            MS_DBG(F("At"), formatDateTime_ISO8601(setupFinishTime),
                   F("there are only"),
                   setupFinishTime % (_loggingIntervalMinutes * 60),
                   F("seconds until next logging interval; leaving modem on "
                     "and connected to the internet."));
        }
    }
    watchDogTimer.resetWatchDog();
    return success;
}


void Logger::registerDataPublisher(dataPublisher* publisher) {
    // find the next empty spot in the publisher array
    uint8_t i = 0;
    for (; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] == publisher) {
            MS_DBG(F("dataPublisher already registered."));
            return;
        }
        if (dataPublishers[i] == nullptr) break;
    }

    // register the publisher there
    dataPublishers[i] = publisher;
}


void Logger::publishDataToRemotes(void) {
    // Assumes that there is an internet connection
    MS_DBG(F("Sending out remote data."));

    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
            _dataPubInstance = i;
            PRINTOUT(F("\nSending data to ["), i, F("]"),
                     dataPublishers[i]->getEndpoint());
            dataPublishers[i]->publishData();
            watchDogTimer.resetWatchDog();
        }
    }
}
void Logger::sendDataToRemotes(void) {
    publishDataToRemotes();
}


// ===================================================================== //
// Public functions to access the clock in proper format and time zone
// ===================================================================== //

// Sets the static timezone that the data will be logged in - this must be set
void Logger::setLoggerTimeZone(int8_t timeZone) {
    _loggerTimeZone = timeZone;
// Some helpful prints for debugging
#ifdef STANDARD_SERIAL_OUTPUT
    const char* prtout1 = "Logger timezone is set to UTC";
    if (_loggerTimeZone == 0) {
        PRINTOUT(prtout1);
    } else if (_loggerTimeZone > 0) {
        PRINTOUT(prtout1, '+', _loggerTimeZone);
    } else {
        PRINTOUT(prtout1, _loggerTimeZone);
    }
#endif
}
int8_t Logger::getLoggerTimeZone(void) {
    return Logger::_loggerTimeZone;
}
// Duplicates for backwards compatibility
void Logger::setTimeZone(int8_t timeZone) {
    setLoggerTimeZone(timeZone);
}
int8_t Logger::getTimeZone(void) {
    return getLoggerTimeZone();
}

// Sets the static timezone that the RTC is programmed in
// I VERY VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setRTCTimeZone(int8_t timeZone) {
    _loggerRTCOffset = _loggerTimeZone - timeZone;
// Some helpful prints for debugging
#ifdef STANDARD_SERIAL_OUTPUT
    const char* prtout1 = "RTC timezone is set to UTC";
    if ((_loggerTimeZone - _loggerRTCOffset) == 0) {
        PRINTOUT(prtout1);
    } else if ((_loggerTimeZone - _loggerRTCOffset) > 0) {
        PRINTOUT(prtout1, '+', (_loggerTimeZone - _loggerRTCOffset));
    } else {
        PRINTOUT(prtout1, (_loggerTimeZone - _loggerRTCOffset));
    }
#endif
}
int8_t Logger::getRTCTimeZone(void) {
    return Logger::_loggerTimeZone - Logger::_loggerRTCOffset;
}


// This set the offset between the built-in clock and the time zone where
// the data is being recorded.  If your RTC is set in UTC and your logging
// timezone is EST, this should be -5.  If your RTC is set in EST and your
// timezone is EST this does not need to be called.
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setTZOffset(int8_t offset) {
    _loggerRTCOffset = offset;
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

// This gets the current epoch time (unix time, ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) and corrects it to the specified time zone

uint32_t Logger::getNowEpoch(void) {
    // Depreciated in 0.33.0, left in for compatiblity
    return getNowLocalEpoch();
}
uint32_t Logger::getNowLocalEpoch(void) {
    uint32_t currentEpochTime = getNowUTCEpoch();
    // Do NOT apply an offset if the timestamp is obviously bad
    if (isRTCSane(currentEpochTime))
        currentEpochTime += ((uint32_t)_loggerRTCOffset) * 3600;
    return currentEpochTime;
}

#if defined(MS_SAMD_DS3231) || not defined(ARDUINO_ARCH_SAMD)

uint32_t Logger::getNowUTCEpoch(void) {
    return rtc.now().getEpoch();
}
void Logger::setNowUTCEpoch(uint32_t ts) {
    rtc.setEpoch(ts);
}

#elif defined ARDUINO_ARCH_SAMD

uint32_t Logger::getNowUTCEpoch(void) {
    return zero_sleep_rtc.getEpoch();
}
void Logger::setNowUTCEpoch(uint32_t ts) {
    zero_sleep_rtc.setEpoch(ts);
}

#endif

// This converts the current UNIX timestamp (ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) into a DateTime object
// The DateTime object constructor requires the number of seconds from
// January 1, 2000 (NOT 1970) as input, so we need to subtract.
DateTime Logger::dtFromEpoch(uint32_t epochTime) {
    DateTime dt(epochTime - EPOCH_TIME_OFF);
    return dt;
}

// This converts a date-time object into a ISO8601 formatted string
// It assumes the supplied date/time is in the LOGGER's timezone and adds
// the LOGGER's offset as the time zone offset in the string.
String Logger::formatDateTime_ISO8601(DateTime& dt) {
    // Set up an inital string
    String dateTimeStr;
    // Convert the DateTime object to a String
    dt.addToString(dateTimeStr);
    dateTimeStr.replace(" ", "T");
    auto tzString = String(_loggerTimeZone);
    if (-24 <= _loggerTimeZone && _loggerTimeZone <= -10) {
        tzString += F(":00");
    } else if (-10 < _loggerTimeZone && _loggerTimeZone < 0) {
        tzString = tzString.substring(0, 1) + '0' + tzString.substring(1, 2) +
            F(":00");
    } else if (_loggerTimeZone == 0) {
        tzString = 'Z';
    } else if (0 < _loggerTimeZone && _loggerTimeZone < 10) {
        tzString = "+0" + tzString + F(":00");
    } else if (10 <= _loggerTimeZone && _loggerTimeZone <= 24) {
        tzString = "+" + tzString + F(":00");
    }
    dateTimeStr += tzString;
    return dateTimeStr;
}

// This converts an epoch time (unix time) into a ISO8601 formatted string.
// It assumes the supplied date/time is in the LOGGER's timezone and adds the
// LOGGER's offset as the time zone offset in the string.
String Logger::formatDateTime_ISO8601(uint32_t epochTime) {
    // Create a DateTime object from the epochTime
    DateTime dt = dtFromEpoch(epochTime);
    return formatDateTime_ISO8601(dt);
}


// This sets the real time clock to the given time
bool Logger::setRTClock(uint32_t UTCEpochSeconds) {
    // If the timestamp is zero, just exit
    if (UTCEpochSeconds == 0) {
        PRINTOUT(F("Bad timestamp, not setting clock."));
        return false;
    }

    // The "setTime" is the number of seconds since Jan 1, 1970 in UTC
    // We're interested in the setTime in the logger's and RTC's timezone
    // The RTC's timezone is equal to the logger's timezone minus the offset
    // between the logger and the RTC.
    uint32_t set_rtcTZ = UTCEpochSeconds;
    // NOTE:  We're only looking at local time here in order to print it out for
    // the user
    uint32_t set_logTZ = UTCEpochSeconds +
        ((uint32_t)getLoggerTimeZone()) * 3600;
    MS_DBG(F("    Time for Logger supplied by NIST:"), set_logTZ, F("->"),
           formatDateTime_ISO8601(set_logTZ));

    // Check the current RTC time
    uint32_t cur_logTZ = getNowLocalEpoch();
    MS_DBG(F("    Current Time on RTC:"), cur_logTZ, F("->"),
           formatDateTime_ISO8601(cur_logTZ));
    MS_DBG(F("    Offset between NIST and RTC:"), abs(set_logTZ - cur_logTZ));

    // NOTE:  Because we take the time to do some UTC/Local conversions and
    // print stuff out, the clock might end up being set up to a few
    // milliseconds behind the input time.  Given the clock is only accurate to
    // seconds (not milliseconds or less), I don't think this is a problem.

    // If the RTC and NIST disagree by more than 5 seconds, set the clock
    if (abs(set_logTZ - cur_logTZ) > 5) {
        setNowUTCEpoch(set_rtcTZ);
        PRINTOUT(F("Clock set!"));
        return true;
    } else {
        PRINTOUT(F("Clock already within 5 seconds of time."));
        return false;
    }
}

// This checks that the logger time is within a "sane" range
bool Logger::isRTCSane(void) {
    uint32_t curRTC = getNowLocalEpoch();
    return isRTCSane(curRTC);
}
bool Logger::isRTCSane(uint32_t epochTime) {
    // Before January 1, 2020 or After January 1, 2030
    if (epochTime < 1577836800 || epochTime > 1893474000) {
        return false;
    } else {
        return true;
    }
}


// This sets static variables for the date/time - this is needed so that all
// data outputs (SD, EnviroDIY, serial printing, etc) print the same time
// for updating the sensors - even though the routines to update the sensors
// and to output the data may take several seconds.
// It is not currently possible to output the instantaneous time an individual
// sensor was updated, just a single marked time.  By custom, this should be
// called before updating the sensors, not after.
void Logger::markTime(void) {
    Logger::markedUTCEpochTime   = getNowUTCEpoch();
    Logger::markedLocalEpochTime = markedUTCEpochTime +
        ((uint32_t)_loggerRTCOffset) * 3600;
}


// This checks to see if the CURRENT time is an even interval of the logging
// rate
uint8_t Logger::checkInterval(void) {
    uint8_t retval = CIA_NOACTION;
    uint32_t checkTime = getNowLocalEpoch();
    int modulus_time_sec =checkTime % (_loggingIntervalMinutes * 60);
    MS_DBG(F("Current Epoch local Timestamp:"), checkTime, F("->"),
           formatDateTime_ISO8601(checkTime));
    MS_DBG(F("Logging interval in seconds:"), (_loggingIntervalMinutes * 60));
    MS_DBG(F("Mod of Logging Interval:"),
           modulus_time_sec);

    if (_sendOffset_act) {
        // A Timer is counting down to perform delayed Post Readings
        if (0 >= --_sendOffset_cnt) {
            // Timer has expired
            _sendOffset_act = false;
            retval |= CIA_POST_READINGS;
            MS_DBG(F("sendOffset Post Readings"));
        } else {
            MS_DBG(F("sendOffset Timer "), _sendOffset_cnt);
        }
    }

    if (modulus_time_sec < 59 ) {
        // Update the time variables with the current time
        markTime();
        MS_DBG(F("Take Sensor readings. Epoch:"), Logger::markedLocalEpochTime);

        // Check what actions for this time period
        retval |= CIA_NEW_READING;
        if (1 < _sendEveryX_num) {
            _sendEveryX_cnt++;
            if (_sendEveryX_cnt >= _sendEveryX_num) {
                _sendEveryX_cnt = 0;
                // Check if delay ~ offset to Send Readings
                if (0 == _sendOffset_min) {
                    // No dealy ~ send readings now
                    retval |= CIA_POST_READINGS;
                    MS_DBG(F("sendEveryX Post Readings"));
                } else {
                    // delayed retval |= CIA_POST_READINGS;
                    _sendOffset_act = true;
                    _sendOffset_cnt = _sendOffset_min;
                    MS_DBG(F("sendEveryX Timer sendOffset started "),
                           _sendOffset_min);
                }
            } else {
                MS_DBG(F("sendEveryX "), _sendEveryX_cnt, F("counting to "),
                       _sendEveryX_num);
            }
        } else {
            retval |= CIA_POST_READINGS;
            MS_DBG(F("Post readings."));
        }
    } else {
        MS_DBG(F("Not time yet."));
    }
    if (!isRTCSane(checkTime)) {
        PRINTOUT(F("----- WARNING ----- !!!!!!!!!!!!!!!!!!!!"));
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(F("!!!!!!!!!! ----- WARNING ----- !!!!!!!!!!"));
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(F("!!!!!!!!!!!!!!!!!!!! ----- WARNING ----- "));
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(' ');
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(F("The current clock timestamp is not valid!"),
                 formatDateTime_ISO8601(getNowUTCEpoch()).substring(0, 10));
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(' ');
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(F("----- WARNING ----- !!!!!!!!!!!!!!!!!!!!"));
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(F("!!!!!!!!!! ----- WARNING ----- !!!!!!!!!!"));
        alertOn();
        delay(25);
        alertOff();
        delay(25);
        PRINTOUT(F("!!!!!!!!!!!!!!!!!!!! ----- WARNING ----- "));
        alertOn();
        delay(25);
        alertOff();
        delay(25);
    }
    return retval;
}


// This checks to see if the MARKED time is an even interval of the logging rate
bool Logger::checkMarkedInterval(void) {
    bool retval;
    MS_DBG(F("Marked Time:"), Logger::markedLocalEpochTime,
           F("Logging interval in seconds:"), (_loggingIntervalMinutes * 60),
           F("Mod of Logging Interval:"),
           Logger::markedLocalEpochTime % (_loggingIntervalMinutes * 60));

    if (Logger::markedLocalEpochTime != 0 &&
        (Logger::markedLocalEpochTime % (_loggingIntervalMinutes * 60) == 0)) {
        MS_DBG(F("Time to log!"));
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
// funcions.)
void Logger::wakeISR(void) {
    MS_DEEP_DBG(F("\nClock interrupt!"));
}


// Puts the system to sleep to conserve battery life.
// This DOES NOT sleep or wake the sensors!!
void Logger::systemSleep(void) {
    // Don't go to sleep unless there's a wake pin!
    if (_mcuWakePin < 0) {
        MS_DBG(F("Use a non-negative wake pin to request sleep!"));
        return;
    }

#if defined(MS_SAMD_DS3231) || not defined(ARDUINO_ARCH_SAMD)

    // Unfortunately, because of the way the alarm on the DS3231 is set up, it
    // cannot interrupt on any frequencies other than every second, minute,
    // hour, day, or date.  We could set it to alarm hourly every 5 minutes past
    // the hour, but not every 5 minutes.  This is why we set the alarm for
    // every minute and use the checkInterval function.  This is a hardware
    // limitation of the DS3231; it is not due to the libraries or software.
    MS_DBG(F("Setting alarm on DS3231 RTC for every minute."));
    rtc.enableInterrupts(EveryMinute);

    // Clear the last interrupt flag in the RTC status register
    // The next timed interrupt will not be sent until this is cleared
    rtc.clearINTStatus();

    // Set up a pin to hear clock interrupt and attach the wake ISR to it
    pinMode(_mcuWakePin, INPUT_PULLUP);
    enableInterrupt(_mcuWakePin, wakeISR, CHANGE);

#elif defined ARDUINO_ARCH_SAMD

    // Make sure interrupts are enabled for the clock
    NVIC_EnableIRQ(RTC_IRQn);       // enable RTC interrupt
    NVIC_SetPriority(RTC_IRQn, 0);  // highest priority

    // Alarms on the RTC built into the SAMD21 appear to be identical to those
    // in the DS3231.  See more notes below.
    // We're setting the alarm seconds to 59 and then seting it to go off
    // whenever the seconds match the 59.  I'm using 59 instead of 00
    // because there seems to be a bit of a wake-up delay
    MS_DBG(F("Setting alarm on SAMD built-in RTC for every minute."));
    zero_sleep_rtc.attachInterrupt(wakeISR);
    zero_sleep_rtc.setAlarmSeconds(59);
    zero_sleep_rtc.enableAlarm(zero_sleep_rtc.MATCH_SS);

#endif

    // Send one last message before shutting down serial ports
    MS_DBG(F("Putting processor to sleep.  ZZzzz..."));

// Wait until the serial ports have finished transmitting
// This does not clear their buffers, it just waits until they are finished
// TODO(SRGDamia1):  Make sure can find all serial ports
#if defined(STANDARD_SERIAL_OUTPUT)
    STANDARD_SERIAL_OUTPUT.flush();  // for debugging
#endif
#if defined DEBUGGING_SERIAL_OUTPUT
    DEBUGGING_SERIAL_OUTPUT.flush();  // for debugging
#endif

    // Stop any I2C connections
    // This function actually disables the two-wire pin functionality and
    // turns off the internal pull-up resistors.
    Wire.end();
// Now force the I2C pins to LOW
// I2C devices have a nasty habit of stealing power from the SCL and SDA pins...
// This will only work for the "main" I2C/TWI interface
#ifdef SDA
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, LOW);
#endif
#ifdef SCL
    pinMode(SCL, OUTPUT);
    digitalWrite(SCL, LOW);
#endif

#if defined ARDUINO_ARCH_SAMD

    // Disable the watch-dog timer
    watchDogTimer.disableWatchDog();

    // Sleep code from ArduinoLowPowerClass::sleep()
    bool restoreUSBDevice = false;
    // if (SERIAL_PORT_USBVIRTUAL)
    // {
    //     USBDevice.standby();
    // }
    // else
    // {
#ifndef USE_TINYUSB
    USBDevice.detach();
#endif
    restoreUSBDevice = true;
    // }
    // Disable systick interrupt:  See
    // https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    // Now go to sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __DSB();
    __WFI();

#elif defined ARDUINO_ARCH_AVR

    // Set the sleep mode
    // In the avr/sleep.h file, the call names of these 5 sleep modes are:
    // SLEEP_MODE_IDLE         -the least power savings
    // SLEEP_MODE_ADC
    // SLEEP_MODE_PWR_SAVE
    // SLEEP_MODE_STANDBY
    // SLEEP_MODE_PWR_DOWN     -the most power savings
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Disable the watch-dog timer
    watchDogTimer.disableWatchDog();

    // Temporarily disables interrupts, so no mistakes are made when writing
    // to the processor registers
    noInterrupts();

    // Disable the processor ADC (must be disabled before it will power down)
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

    // Actually put the processor into sleep mode.
    // This must happen after the SE bit is set.
    sleep_cpu();

#endif
    // ---------------------------------------------------------------------


    // ---------------------------------------------------------------------
    // -- The portion below this happens on wake up, after any wake ISR's --

#if defined ARDUINO_ARCH_SAMD
    // Reattach the USB after waking
    // Enable systick interrupt
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    if (restoreUSBDevice) {
#ifndef USE_TINYUSB
        USBDevice.attach();
#endif
        uint32_t startTimer = millis();
        while (!SERIAL_PORT_USBVIRTUAL && ((millis() - startTimer) < 1000L)) {
            // wait
        }
    }
#endif

#if defined ARDUINO_ARCH_AVR

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

    // Re-enable the watch-dog timer
    watchDogTimer.enableWatchDog();

// Re-start the I2C interface
#ifdef SDA
    pinMode(SDA, INPUT_PULLUP);  // set as input with the pull-up on
#endif
#ifdef SCL
    pinMode(SCL, INPUT_PULLUP);
#endif
    Wire.begin();
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);

#if defined(MS_SAMD_DS3231) || not defined(ARDUINO_ARCH_SAMD)
    // Stop the clock from sending out any interrupts while we're awake.
    // There's no reason to waste thought on the clock interrupt if it
    // happens while the processor is awake and doing other things.
    rtc.disableInterrupts();
    // Detach the from the pin
    disableInterrupt(_mcuWakePin);

#elif defined ARDUINO_ARCH_SAMD
    zero_sleep_rtc.disableAlarm();
#endif

    // Wake-up message
    MS_DBG(F("\n\n\n... zzzZZ Processor is now awake!"));

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


// This generates a file name from the logger id and the current date
// This will be used if the setFileName function is not called before
// the begin() function is called.
void Logger::generateAutoFileName(void) {
    // Generate the file name from logger ID and date
    auto fileName = String(_loggerID);
    fileName += "_";
    fileName += formatDateTime_ISO8601(getNowLocalEpoch()).substring(0, 10);
    fileName += ".csv";
    setFileName(fileName);
    _fileName = fileName;
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
    if (_loggerTimeZone > 0) {
        dtRowHeader += '+' + _loggerTimeZone;
    } else if (_loggerTimeZone < 0) {
        dtRowHeader += _loggerTimeZone;
    }
    STREAM_CSV_ROW(dtRowHeader, getVarCodeAtI(i))
}


// This prints a comma separated list of volues of sensor data - including the
// time -  out over an Arduino stream
void Logger::printSensorDataCSV(Stream* stream) {
    String csvString = "";
    dtFromEpoch(Logger::markedLocalEpochTime).addToString(csvString);
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
    if (!sd1_card_fatfs.begin(_SDCardSSPin, SPI_FULL_SPEED)) {
        PRINTOUT(F("Error: SD card failed to initialize or is missing."));
        PRINTOUT(F("Data will not be saved!"));
        return false;
    } else {
        // skip everything else if there's no SD card, otherwise it mighthang
        MS_DBG(F("Successfully connected to SD Card with card/slave select on "
                 "pin"),
               _SDCardSSPin);
        return true;
    }
}


// Protected helper function - This sets a timestamp on a file
void Logger::setFileTimestamp(File fileToStamp, uint8_t stampFlag, bool localTime) {
    if (false == localTime) {
        fileToStamp.timestamp(stampFlag, dtFromEpoch(getNowLocalEpoch()).year(),
                            dtFromEpoch(getNowLocalEpoch()).month(),
                            dtFromEpoch(getNowLocalEpoch()).date(),
                            dtFromEpoch(getNowLocalEpoch()).hour(),
                            dtFromEpoch(getNowLocalEpoch()).minute(),
                            dtFromEpoch(getNowLocalEpoch()).second());
    }else {

        DateTime markedDtTz(getNowLocalEpoch()- EPOCH_TIME_DTCLASS );

        MS_DEEP_DBG(F("setFTTz"),markedDtTz.year(),markedDtTz.month(), markedDtTz.date(),
            markedDtTz.hour(), markedDtTz.minute(), markedDtTz.second());
        bool crStat = fileToStamp.timestamp(
            stampFlag, markedDtTz.year(), markedDtTz.month(), markedDtTz.date(),
            markedDtTz.hour(), markedDtTz.minute(), markedDtTz.second());
        if (!crStat) {
            PRINTOUT(F("setFTTz err for "), markedDtTz.year(), markedDtTz.month(),
                    markedDtTz.date(), markedDtTz.hour(), markedDtTz.minute(),
                    markedDtTz.second());
        }
    }
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
        // Set access date time
        setFileTimestamp(logFile, T_ACCESS);
        return true;
    } else if (createFile) {
        // Create and then open the file in write mode
        if (logFile.open(charFileName, O_CREAT | O_WRITE | O_AT_END)) {
            MS_DBG(F("Created new file:"), filename);
            // Set creation date time
            setFileTimestamp(logFile, T_CREATE);
            // Write out a header, if requested
            if (writeDefaultHeader) {
                // Add header information
                printFileHeader(&logFile);
// Print out the header for debugging
#if defined(DEBUGGING_SERIAL_OUTPUT) && defined(MS_DEBUGGING_STD)
                MS_DBG(F("\n \\/---- File Header ----\\/"));
                printFileHeader(&DEBUGGING_SERIAL_OUTPUT);
                MS_DBG('\n');
#endif
                // Set write/modification date time
                setFileTimestamp(logFile, T_WRITE);
            }
            // Set access date time
            setFileTimestamp(logFile, T_ACCESS);
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

    // Set write/modification date time
    setFileTimestamp(logFile, T_WRITE);
    // Set access date time
    setFileTimestamp(logFile, T_ACCESS);
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
#if defined(STANDARD_SERIAL_OUTPUT)
    PRINTOUT(F("\n \\/---- Line Saved to SD Card ----\\/"));
    printSensorDataCSV(&STANDARD_SERIAL_OUTPUT);
    PRINTOUT('\n');
#endif

    // Set write/modification date time
    setFileTimestamp(logFile, T_WRITE);
    // Set access date time
    setFileTimestamp(logFile, T_ACCESS);
    // Close the file to save it
    logFile.close();
    return true;
}


// ===================================================================== //
// Public functions for a "sensor testing" mode
// ===================================================================== //
// A static function if you'd prefer to enter testing based on an interrupt
void Logger::testingISR() {
    MS_DEEP_DBG(F("Testing interrupt!"));
    if (!Logger::isTestingNow && !Logger::isLoggingNow) {
        Logger::startTesting = true;
        MS_DEEP_DBG(F("Testing flag has been set."));
    }
}


// This defines what to do in the testing mode
void Logger::testingMode() {
    // Flag to notify that we're in testing mode
    Logger::isTestingNow = true;
    // Unset the startTesting flag
    Logger::startTesting = false;

    PRINTOUT(F("------------------------------------------"));
    PRINTOUT(F("Entering sensor testing mode"));
    delay(100);  // This seems to prevent crashes, no clue why ....

    // Get the modem ready

    bool gotInternetConnection = false;
    if (_logModem != nullptr) {
        MS_DBG(F("Waking up"), _logModem->getModemName(), F("..."));
        if (_logModem->modemWake()) {
            // Connect to the network
            watchDogTimer.resetWatchDog();
            MS_DBG(F("Connecting to the Internet..."));
            if (_logModem->connectInternet()) {
                gotInternetConnection = true;
                // Publish data to remotes
                watchDogTimer.resetWatchDog();
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

        watchDogTimer.resetWatchDog();
        // Update the values from all attached sensors
        // NOTE:  NOT using complete update because we want the sensors to be
        // left on between iterations in testing mode.
        _internalArray->updateAllSensors();
        // Print out the current logger time
        PRINTOUT(F("Current logger time is"),
                 formatDateTime_ISO8601(getNowLocalEpoch()));
        PRINTOUT(F("-----------------------"));
// Print out the sensor data
#if defined(STANDARD_SERIAL_OUTPUT)
        _internalArray->printSensorData(&STANDARD_SERIAL_OUTPUT);
#endif
        PRINTOUT(F("-----------------------"));
        watchDogTimer.resetWatchDog();

        delay(5000);
        watchDogTimer.resetWatchDog();
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
    watchDogTimer.resetWatchDog();

    // Unset testing mode flag
    Logger::isTestingNow = false;

    // Sleep
    systemSleep();
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //

// This does all of the setup that can't happen in the constructors
// That is, things that require the actual processor/MCU to do something
// rather than the compiler to do something.
void Logger::begin(const char* loggerID, uint16_t loggingIntervalMinutes,
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

    MS_DBG(F(
        "Setting up a watch-dog timer to fire after 5 minutes of inactivity"));
    watchDogTimer.setupWatchDog((uint32_t)(5 * 60 * 3));
    // Enable the watchdog
    watchDogTimer.enableWatchDog();

#if defined ARDUINO_ARCH_SAMD
    MS_DBG(F("Beginning internal real time clock"));
    zero_sleep_rtc.begin();
#endif
    watchDogTimer.resetWatchDog();

    // Set the pins for I2C
    MS_DBG(F("Setting I2C Pins to INPUT_PULLUP"));
#ifdef SDA
    pinMode(SDA, INPUT_PULLUP);  // set as input with the pull-up on
#endif
#ifdef SCL
    pinMode(SCL, INPUT_PULLUP);
#endif
    MS_DBG(F("Beginning wire (I2C)"));
    Wire.begin();
    watchDogTimer.resetWatchDog();

    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);

    // Set all of the pin modes
    // NOTE:  This must be done here at run time not at compile time
    setLoggerPins(_mcuWakePin, _SDCardSSPin, _SDCardPowerPin, _buttonPin,
                  _ledPin);

#if defined(MS_SAMD_DS3231) || not defined(ARDUINO_ARCH_SAMD)
    MS_DBG(F("Beginning DS3231 real time clock"));
    rtc.begin();
#endif
    watchDogTimer.resetWatchDog();

    // Print out the current time
    PRINTOUT(F("Current RTC time is:"),
             formatDateTime_ISO8601(getNowUTCEpoch()));
    PRINTOUT(F("Current localized logger time is:"),
             formatDateTime_ISO8601(getNowLocalEpoch()));

    // Reset the watchdog
    watchDogTimer.resetWatchDog();

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

    PRINTOUT(F("Logger portion of setup finished.\n"));
}


// This is a one-and-done to log data
void Logger::logData(void) {
    // Reset the watchdog
    watchDogTimer.resetWatchDog();

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval()) {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        // Reset the watchdog
        watchDogTimer.resetWatchDog();

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
        watchDogTimer.resetWatchDog();
        _internalArray->completeUpdate();
        watchDogTimer.resetWatchDog();

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
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) testingMode();

    // Sleep
    systemSleep();
}
// This is a one-and-done to log data
void Logger::logDataAndPublish(void) {
    // Reset the watchdog
    watchDogTimer.resetWatchDog();

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval()) {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        // Reset the watchdog
        watchDogTimer.resetWatchDog();

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
        watchDogTimer.resetWatchDog();
        _internalArray->completeUpdate();
        watchDogTimer.resetWatchDog();

// Print out the sensor data
#if defined(STANDARD_SERIAL_OUTPUT)
        MS_DBG('\n');
        _internalArray->printSensorData(&STANDARD_SERIAL_OUTPUT);
        MS_DBG('\n');
#endif

        // Create a csv data record and save it to the log file
        logToSD();

        if (_logModem != nullptr) {
            MS_DBG(F("Waking up"), _logModem->getModemName(), F("..."));
            if (_logModem->modemWake()) {
                // Connect to the network
                watchDogTimer.resetWatchDog();
                MS_DBG(F("Connecting to the Internet..."));
                if (_logModem->connectInternet()) {
                    // Publish data to remotes
                    watchDogTimer.resetWatchDog();
                    publishDataToRemotes();
                    watchDogTimer.resetWatchDog();

                    if ((Logger::markedLocalEpochTime != 0 &&
                         Logger::markedLocalEpochTime % 86400 == 43200) ||
                        !isRTCSane(Logger::markedLocalEpochTime)) {
                        // Sync the clock at noon
                        MS_DBG(F("Running a daily clock sync..."));
                        setRTClock(_logModem->getNISTTime());
                        watchDogTimer.resetWatchDog();
                    }

                    // Update the modem metadata
                    MS_DBG(F("Updating modem metadata..."));
                    _logModem->updateModemMetadata();

                    // Disconnect from the network
                    MS_DBG(F("Disconnecting from the Internet..."));
                    _logModem->disconnectInternet();
                } else {
                    MS_DBG(F("Could not connect to the internet!"));
                    watchDogTimer.resetWatchDog();
                }
            }
            // Turn the modem off
            _logModem->modemSleepPowerDown();
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
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) testingMode();

    // Call the processor sleep
    systemSleep();
}

void Logger::forceSysReset(uint8_t source, uint16_t simpleMagicNumber) {
    
    if (4567 !=simpleMagicNumber) return;

    PRINTOUT(F("Forcing reset"), source);
    delay(20);
    watchDogTimer.setupWatchDog(1);
    watchDogTimer.enableWatchDog();
    delay(100000); //Expect watchdog to kick in within 8secs
} //forceReset

// ===================================================================== //
// Reliable Delivery functions
// see class headers
// ===================================================================== //

void Logger::logDataAndPubReliably(uint8_t cia_val_override) {

    if (cia_val_override & CIA_NO_SLEEP) {
        cia_val_override &= ~CIA_NO_SLEEP;
    } else {
        // Sleep at start of cycle, so data is available for caller at the end
        systemSleep();
    }

    // Reset the watchdog
    watchDogTimer.resetWatchDog();


    // Assuming we were woken up by the clock, check if the current time is
    // an even interval of the logging interval
    uint8_t cia_val = checkInterval();
    if (cia_val_override) {
        cia_val = cia_val_override;
        wakeUpTime_secs = getNowLocalEpoch();//Set reference time
        markTime();
        PRINTOUT(F("logDataAndPubReliably - overide with "),cia_val);
    }

    if (NULL != _bat_handler_atl) {
        _bat_handler_atl(LB_PWR_USEABLE_REQ);  // Set battery status
        if (!_bat_handler_atl(LB_PWR_SENSOR_USE_REQ)) {
            // Squash any activity
            //PRINTOUT(F("logDataAndPubReliably - all cancelled"));
            const static char ALL_CANCELLED_pm[] EDIY_PROGMEM = 
            "logDataAndPubReliably - all cancelled"; 
            // njh PRINT_LOGLINE_P(ALL_CANCELLED_pm);
            cia_val = 0;
        }
        if (!_bat_handler_atl(LB_PWR_MODEM_USE_REQ)) {
            if (CIA_POST_READINGS & cia_val) {
                // Change publish attempt to saving for next publish attempt
                cia_val &= ~CIA_POST_READINGS;
                cia_val |= CIA_RLB_READINGS;  //
                //PRINTOUT(F("logDataAndPubReliably - tx cancelled"));
                const static char TX_CANCELLED_pm[] EDIY_PROGMEM = 
                "logDataAndPubReliably - tx cancelled";
                // njh PRINT_LOGLINE_P(TX_CANCELLED_pm);
            }
        }
    }

#if 1
    if (cia_val) {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;
        // Reset the watchdog
        watchDogTimer.resetWatchDog();

        // Print a line to show new reading
        //PRINTOUT(F("---logDataAndPubReliably ("),cia_val,F(")----"));
        STANDARD_SERIAL_OUTPUT.print(F("---logDataAndPubReliably (0x"));
        STANDARD_SERIAL_OUTPUT.print(cia_val,HEX);
        STANDARD_SERIAL_OUTPUT.println(F(")----"));
        // Turn on the LED to show we're taking a reading
        alertOn();
        // Power up the SD Card
        // TODO(SRGDamia1):  Decide how much delay is needed between turning on
        // the card and writing to it.  Could we turn it on just before writing?
        turnOnSDcard(false);
        if (cia_val & CIA_NEW_READING) {
            // Do a complete update on the variable array.
            // This this includes powering all of the sensors, getting
            // updated values, and turing them back off. NOTE:  The wake
            // function for each sensor should force sensor setup to run if
            // the sensor was not previously set up.
            PRINTOUT(F("Read sensors..."));
            watchDogTimer.resetWatchDog();
            _internalArray->completeUpdate();
            watchDogTimer.resetWatchDog();

            // Create a csv data record and save it to the log file
            logToSD();

            serzRdel_Line();  // Start Queue
        }
        if (cia_val & CIA_POST_READINGS) {
            if (_logModem != NULL) {
                MS_DBG(F("Waking up"), _logModem->getModemName(), F("..."));
                if (_logModem->modemWake()) {
                    // Connect to the network
                    watchDogTimer.resetWatchDog();
                    PRINTOUT(F("Connecting to the Internet with"),_logModem->getModemName());
                    if (_logModem->connectInternet()) {
                        const static char CONNECT_INTERNET_pm[] EDIY_PROGMEM = 
                        "Connected Internet"; 
                        PRINT_LOGLINE_P(CONNECT_INTERNET_pm);
                        // be nice to add _logModem->getModemName()
                        //This doesn't work PRINT_LOGLINE_P2(CONNECT_INTERNET_pm,_logModem->getModemName().c_str());
                        // Publish data to remotes
                        watchDogTimer.resetWatchDog();
                        publishDataQueuedToRemotes(true);
                        watchDogTimer.resetWatchDog();

// Sync the clock at midnight or on the hour
#define NIST_SYNC_DAY 86400
#define NIST_SYNC_HR 3600
#if defined NIST_SYNC_HOURLY
#define NIST_SYNC_RATE NIST_SYNC_HR 
#else
#define NIST_SYNC_RATE NIST_SYNC_DAY
#endif //NIST_SYNC_HOURLY
                        uint32_t logIntvl_sec = _loggingIntervalMinutes * 60; 
                        uint32_t timeToday_sec = markedLocalEpochTime % NIST_SYNC_RATE;
                        bool doSyncTimeCheck = (timeToday_sec< logIntvl_sec);
                        /*MS_DBG*/PRINTOUT(F("SyncTimeCheck "),doSyncTimeCheck," modulo_sec",timeToday_sec," Time",Logger::markedLocalEpochTime);
                        if (doSyncTimeCheck) {
                            MS_DBG(F("Running an NIST clock sync..."));
                            if(setRTClock(_logModem->getNISTTime())) {
                                const static char CLOCK_NIST_OK_pm[] EDIY_PROGMEM ="Clock Nist Synced"; 
                                PRINT_LOGLINE_P(CLOCK_NIST_OK_pm);                                       
                            } else {
                                const static char CLOCK_NIST_FAIL_pm[] EDIY_PROGMEM ="Clock Nist Failed"; 
                                PRINT_LOGLINE_P(CLOCK_NIST_FAIL_pm);       
                            }
                        }
                        watchDogTimer.resetWatchDog();

                        // Update the modem metadata
                        MS_DBG(F("Updating modem metadata..."));
                        _logModem->updateModemMetadata();

                        // Disconnect from the network
                        MS_DBG(F("Disconnecting from the Internet..."));
                        _logModem->disconnectInternet();
                    } else {
                        //RINTOUT(F("Connect to the internet failed with"),_logModem->getModemName());
                        const static char CONNECT_FAILED_pm[] EDIY_PROGMEM = 
                        "Connected Internet Failed"; 
                        PRINT_LOGLINE_P(CONNECT_FAILED_pm);
                        watchDogTimer.resetWatchDog();
                    }
                } else {
                    PRINTOUT(F("Failed to wake "), _logModem->getModemName());
                }
                // Turn the modem off
                _logModem->modemSleepPowerDown();
            } else
                PRINTOUT(F("Internet failed, no _logModem "));
        } else if (cia_val & CIA_RLB_READINGS) {
            // Values not transmitted,  save readings for later transmission
            PRINTOUT(F("logDataAndPubReliably - store readings, no pub"));
            publishDataQueuedToRemotes(false);
        }


        // TODO(SRGDamia1):  Do some sort of verification that minimum 1 sec has
        // passed for internal SD card housekeeping before cutting power It
        // seems very unlikely based on my testing that less than one second
        // would be taken up in publishing data to remotes
        // Cut power from the SD card - without additional housekeeping wait
        turnOffSDcard(false);

        // Turn off the LED
        alertOff();
        // Print a line to show reading ended
        PRINTOUT(F("---logDataAndPubReliably  Complete----------"));

        // Unset flag
        Logger::isLoggingNow = false;
        Logger::startTesting = false; //Interrupt going off
        // njh dumpFreeRam(8256); //large Number
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) testingMode();

    // Call the processor sleep
    //systemSleep();
    #endif //0
} // logDataAndPubReliably

bool Logger::publishRspCodeAccepted(int16_t  rspCode) {
    if (HTTPSTATUS_CREATED_201 == rspCode) return true;
    //return (HTTPSTATUS_CREATED_201 == rspCode);
    #if defined MS_DISCARD_HTTP_500
    if (HTTPSTATUS_GT_500 == rspCode) {
        //As of 2022Sept15 this error is repetitive and prevents more messages being sent 
        // https://github.com/ODM2/ODM2DataSharingPortal/issues/628
        //https://github.com/neilh10/ModularSensors/issues/119
        // Unfortunately throw away this reading
        PRINTOUT(F("pubRspCode SERVER ERROR discard reading"));
        return true;
        }
    #endif //MS_DISCARD_HTTP_500
    return false;
} //publishRspCodeAccepted

void Logger::publishDataQueuedToRemotes(bool internetPresent) {
    // Assumes that there is an internet connection
    // bool    useQueue = false;
    int16_t  rspCode = 0;
    uint32_t tmrGateway_ms;
    bool     dslStatus = false;
    bool     retVal    = false;
    // MS_DBG(F("Pub Data Queued"));
    MS_DBG(F("pubDQTR from"), serzRdelFn_str, internetPresent);

    // Open debug file
#if defined MS_LOGGERBASE_POSTS
    retVal = postLogOpen(postsLogFn_str);
#endif  // MS_LOGGERBASE_POSTS

    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != NULL) {
            _dataPubInstance = i;
            PRINTOUT(F("\npubDQTR Sending data to ["), i, F("]"),
                     dataPublishers[i]->getEndpoint());
            // open the qued file for serialized readings
            // (char*)serzQueuedFn_str


            // dataPublishers[i]->publishData(_logModem->getClient());
            // Need to manage filenames[i]

            /* TODO njh check power availability
            ps_Lbatt_status_t Lbatt_status;
            Lbatt_status =
            mcuBoard.isBatteryStatusAbove(true,PS_PWR_USEABLE_REQ);
            if (no power) break out for loop;
            */

            if (dataPublishers[i]->getQueuedStatus()) {
                uint16_t delay_posted_pacing_ms = dataPublishers[i]->getTimerPostPacing_mS();
                uint16_t published_this_pass =0;
                serzQueuedStart((char)('0' + i));
                deszRdelStart();
                // MS_START_DEBUG_TIMER;
                tmrGateway_ms = millis();
                uint32_t tmrThisPublish_ms;
                bool attemptPostStatus=true;
                uint16_t attemptPostCnt=0;
                uint16_t attemptPostFailedCnt=0;
                while ((dslStatus = deszRdelLine())) {
                    attemptPostCnt++;
                    tmrThisPublish_ms = millis();
                    if (internetPresent && attemptPostStatus) {
                        rspCode = dataPublishers[i]->publishData();
                    } else {
                        if (internetPresent) {
                            rspCode = HTTPSTATUS_NC_901;
                        } else {
                            //then must be attemptPostStatus==false
                            rspCode = HTTPSTATUS_NC_904;
                        }
                    }

                    watchDogTimer.resetWatchDog();
                    // MS_DBG(F("Rsp"), rspCode, F(", in"),
                    // MS_PRINT_DEBUG_TIMER,    F("ms\n"));
                    postLogLine( (millis() -tmrThisPublish_ms), rspCode);

                    if (false == publishRspCodeAccepted(rspCode)) {
#define DESLZ_STATUS_UNACK '1'
#define DESLZ_STATUS_MAX '8'
#define DESLZ_STATUS_POS 0
#if 0
                        if (++deszq_line[0] > '8') {
                            deszq_line[DESLZ_STATUS_POS] = DESLZ_STATUS_UNACK;
                        }
#endif  // if x
#if defined(USE_PS_modularSensorsNetwork)
                        if ((desz_pending_records >= _sendQueueSz_num)&&(MMWGI_SEND_QUE_SZ_NUM_NOP != _sendQueueSz_num )) {
                                PRINTOUT(F("pubDQTR QueuedFull, skip reading. sendQueue "),  _sendQueueSz_num);
                                postLogLine(0,rspCode); //Log skipped readings
                        } else 
 #endif // USE_PS_modularSensorsNetwork
                        {
                            retVal = serzQueuedFile.print(deszq_line);
                            if (0 >= retVal) {
                                PRINTOUT(F("pubDQTR serzQueuedFil err"), retVal);
                            }
                            desz_pending_records++;  
                        }
                    } else {
                        /*A publish has been sucessfull.
                         * Slow Down sending based on publishers acceptance rate
                         * Each publish creates and tears down a TCP connection */
                        /*TODO njh create intergrate all POSTS to one tcp/ip connection */
                        published_this_pass++;
                        attemptPostFailedCnt=0;
                        MS_DBG(F("pubDQTR1 delay"),delay_posted_pacing_ms ,F("mS : posted"), published_this_pass);
                        delay(delay_posted_pacing_ms);
                    }

                    //Check for any limits that might have been exceeded
                    if ((attemptPostCnt >= _postMax_num) && (0 != _postMax_num)) {
                        //Exceeded number to attempt, force write to serzQueue
                        attemptPostStatus = false;
                    }

                    if (++attemptPostFailedCnt > RDELAY_FAILED_POSTS_THRESHOLD ) {
                        //Exceeded number of consecutive failures, force write to serzQueue
                        attemptPostStatus = false;
                    }
                }  // while reading line
                deszRdelClose(true);
                serzQueuedCloseFile(false);
                // retVal = serzQueuedFile.close();
                // if (!retVal)
                //    PRINTOUT(
                //        F("publishDataQueuedToRemote serzQueuedFile.close err"));

                PRINTOUT(F("Sent"), deszLinesRead, F("readings in"),
                         ((float)(millis() - tmrGateway_ms)) / 1000,
                         F("sec. Queued readings="), desz_pending_records);

                if (true == publishRspCodeAccepted(rspCode)) {
                    // Do retrys through publisher - if file exists
                    if (sd1_card_fatfs.exists(serzQueuedFn)) {
                        uint16_t tot_posted           = 0;
                        uint16_t cnt_for_pwr_analysis = 1;
                        MS_DBG(F("pubDQTR retry from"), serzQueuedFn);
                         deszQueuedStart();
                        while ((dslStatus = deszQueuedLine()) )  {

                            /*At least one publish has been sucessfull.
                             * Slow Down sending based on publishers acceptance rate
                             * Each publish creates and tears down a TCP connection */
                            MS_DBG(F("pubDQTR2 delay"),delay_posted_pacing_ms ,F("mS : total posted"), published_this_pass);
                            delay(delay_posted_pacing_ms);

                            // setup for publisher to call deszqNextCh()
                            rspCode = dataPublishers[i]->publishData();
                            watchDogTimer.resetWatchDog();
                            postLogLine(i, rspCode);
                            if (false == publishRspCodeAccepted(rspCode)) break;

                            tot_posted++;
                            published_this_pass++;

                            deszq_line[0] = 0;  // Show completed

                            // Check for enough battery power
                            if (cnt_for_pwr_analysis++ >=
                                _sendAtOneTimeMaxX_num) {
                                if (NULL != _bat_handler_atl) {
                                    // Measure  battery
                                    _bat_handler_atl(LB_PWR_USEABLE_REQ);
                                    if (!_bat_handler_atl(
                                            LB_PWR_MODEM_USE_REQ)) {
                                        // stop transmission
                                        cnt_for_pwr_analysis = 0;
                                        PRINTOUT(F("pubDQTR not enough power available"));
                                        break;
                                    }
                                }
                                cnt_for_pwr_analysis = 1;
                            }
                            if ((tot_posted  >= _postMax_num) && (0 != _postMax_num)) {
                                PRINTOUT(F("pubDQTR POST_MAX_NUM reached"), tot_posted);
                                break; /// unsent lines are copied through
                            }
                        } //while
// increment status of number attempts
#if 0
                        if (deszq_line[DESLZ_STATUS_POS]++ >=
                            DESLZ_STATUS_MAX) {
                            deszq_line[DESLZ_STATUS_POS] = DESLZ_STATUS_MAX;
                        }
#endif  // if z
        // deszQueuedCloseFile() is serzQueuedCloseFile(true)
                        if (tot_posted) {
                            // At least one POST was accepted, if 2 or more, the last may have failed
                            // and still be in deszq_line
                            serzQueuedCloseFile(true);
                        } else {
                            serzQueuedCloseFile(false);
                        }
                    } else { MS_DBG(F("pubDQTR no queued file"), serzQueuedFn);}
                } else {
                    MS_DBG(F("pubDQTR drop retrys. rspCode"), rspCode);
                }
            }
        }
    }
    postLogClose();
} // publishDataQueuedToRemotes

// ===================================================================== //
// Serialize/deserialize functions
// see class headers
// ===================================================================== //

#define DELIM_CHAR2 ','
#define SERZQUED_OFLAGS
bool Logger::serzQueuedStart(char uniqueId) {
    strcpy(serzQueuedFn, serzQueuedFn_str);
    strncat(serzQueuedFn, &uniqueId, 1);
    strcat(serzQueuedFn, ".TXT");

    if (!serzQueuedFile.open(serzQueuedFn, (O_WRITE | O_CREAT | O_AT_END))) {
        PRINTOUT(F("serzQueuedStart open err"));
        return false;
    } else {
        MS_DEEP_DBG(F("serzQueuedStart open"), serzQueuedFn);
    }
    return true;
}

bool Logger::serzQueuedCloseFile(bool flush) {
    /* This closes the file, removing the sent messages
     Assumes serzQueuedFile points incoming file if flush==true
    */
    bool    retBool=true;

    if (flush) {   
        // There may be 0, or more of unsent records left in serzQueued
        uint16_t num_lines = serzQueuedFlushFile();

        PRINTOUT(F("seQCF Queue for next pass unsent records"), num_lines);
        desz_pending_records = num_lines;

    } else { // !flush simple clean
        retBool = serzQueuedFile.close();
        if (!retBool) {
            sd1_Err("seQCF serzQueuedFile.close2 err");
            return false;
        }
    }
    return retBool;
}

#define TEMP_BASE_FN_STR "TMP01.TXT"
#define QUEOLD_BASE_FN_STR "QUEDEL01.TXT"
inline uint16_t Logger::serzQueuedFlushFile() {
    /*  The flush algorithim is, 
     copy unsent lines to a temporary_file up to _sendQueueSz_num, and then discard rest
     Assumes serzQueuedFile points incoming file
     when complete rename serzQueuedFile  to delete_file
     rename temporary_file to serzQueuedFile to complete flush
    */
    const char* tempFn = TEMP_BASE_FN_STR;
    const char* queDelFn = QUEOLD_BASE_FN_STR;
    File    tgtoutFile;
    int16_t retNum;
    int16_t  num_char ;
    uint16_t num_lines = 0;   
    uint16_t num_skipped=0;
    bool    retBool;

    // Check if exists and delete
    if (sd1_card_fatfs.exists(tempFn)) {
        if (!sd1_card_fatfs.remove(tempFn)) {
            PRINTOUT(F("seQFF remove1 err"), tempFn);
            sd1_Err("seQFF err6 remove");
        } else {
            MS_DEEP_DBG(F("seQFF remove "), tempFn);
        }
    }  
    retBool = tgtoutFile.open(tempFn, (O_WRITE | O_CREAT));
    if (!retBool) {
        PRINTOUT(F("seQFF open2 err"), tempFn);
        // sd1_Err("seQCF open4");
        //todo close all other files
        return 0;
    } else {
        MS_DEEP_DBG(F("seQFF opened "), tempFn);
    }

    num_char  = strlen(deszq_line);
    if (num_char) {  // Test could be min size, but this unknown
        MS_DBG(F("seQFF Last POST Failed "),  deszq_line);
        retNum = tgtoutFile.write(deszq_line, num_char);
        if (retNum != num_char) {
            PRINTOUT(F("seQFF tgtoutFile write1 err"), num_char);
            // sd1_Err("seQCF write2");
        }
    } 

    MS_DBG(F("seQFF cpy lines across"));
    while (0 < (num_char = serzQueuedFile.fgets(deszq_line,
                                                QUEFILE_MAX_LINE))) {

#if defined(USE_PS_modularSensorsNetwork)
        if ((num_lines>=_sendQueueSz_num)&&(MMWGI_SEND_QUE_SZ_NUM_NOP != _sendQueueSz_num )) {
            /*Limit sendQueueSz on Copy, implicitly this not on creation 
            This is the first pass at limiting the size of the que by dumping the newest.
            FIFO.
            Future may want to keep the latest readings 
            */
            postLogLine((MAX_NUMBER_SENDERS+1),HTTPSTATUS_NC_903);
            num_skipped++;
        } else
#endif // USE_PS_modularSensorsNetwork 
        {

            retNum = tgtoutFile.write(deszq_line, num_char);
            // Squelch last char LF
            deszq_line[sizeof(deszq_line) - 1] = 0;
            MS_DBG(deszq_line);
            if (retNum != num_char) {
                PRINTOUT(F("seQFF tgtoutFile write3 err"), num_char,
                            retNum);
                // sd1_Err("seQFF write4");
                break;
            }
            num_lines++;
        }
    }
    if (num_skipped){ 
        PRINTOUT(F("seQFF sendQueue Size "), _sendQueueSz_num, F(",queued"),num_lines, F(",latest readings discarded"),num_skipped);
    };
    //Cleanup flushed serzQueuedFile to del_file as debugging aid
    if (sd1_card_fatfs.exists(queDelFn)) {
        if (!sd1_card_fatfs.remove(queDelFn)) {
            PRINTOUT(F("seQFF remove2 err"), queDelFn);
            sd1_Err("seQFF err7 remove");
        }
        if (sd1_card_fatfs.exists(queDelFn)) {
            PRINTOUT(F("seQFF err failed remove"), queDelFn);
        }
    }     

    retBool = serzQueuedFile.rename(queDelFn);
    if (!retBool) {
        PRINTOUT(F("seQFF REBOOT rename1 err"), queDelFn);
        //Problem - unrecoverable, so reboot
        retBool = serzQueuedFile.close();
        if (!retBool) {
            PRINTOUT(F("seQFF close1 failed err"), serzQueuedFn);
        }
        forceSysReset(1,4567);
        //sd1_card_fatfs.remove(serzQueuedFn);
        // sd1_Err("seQFF rename2");
        //return num_lines;
    } else {
        MS_DBG(F("seQFF cleanup rename "), serzQueuedFn, F("to"), queDelFn);

        retBool = serzQueuedFile.close();
        if (!retBool) {
            sd1_Err("seQFF serzQueuedFile.close2 err");
            return  num_lines;
        } else {MS_DEEP_DBG(F("seQFF close serzQueuedFile")); }

        retBool = tgtoutFile.rename(serzQueuedFn);
        if (!retBool) {
            sd1_Err("seQFF tgtoutFile.rename err");
            return  num_lines;
        } else {MS_DEEP_DBG(F("seQFF rename "), tempFn, F("to"), serzQueuedFn); }

        retBool = tgtoutFile.close();
        if (!retBool) {
            sd1_Err("seQFF tgtoutFile.close1 err");
            return  num_lines;
        } else {MS_DEEP_DBG(F("seQFF closed tgtoutFile")); }
    }

    return  num_lines;
} //serzQueuedFlushFile

/*
For serialize, create ASCII CSV records of the form
status,<marked epoch time> n*[<,values>]
*/
#define RDEL_OFLAG (O_WRITE | O_CREAT | O_AT_END)
bool Logger::serzRdel_Line() {
    if (serzRdelFile.open(serzRdelFn_str, RDEL_OFLAG)) {
        uint16_t outputSz;
        // String csvString(Logger::markedLocalEpochTime);
        outputSz = serzRdelFile.print("0,");  // Start READINGS_STATUS
        outputSz += serzRdelFile.print(Logger::markedLocalEpochTime);
        for (uint8_t i = 0; i < getArrayVarCount(); i++) {
            // csvString += ',';
            outputSz += serzRdelFile.print(',' + getValueStringAtI(i));
        }
        outputSz += serzRdelFile.println();
        // setFileAccessTime(serzRdelFile);
        serzRdelFile.close();
        MS_DEEP_DBG(F("serzRdel_Line on"), serzRdelFn_str, F(" at "),
               markedLocalEpochTime, F(" size="), outputSz);
    } else {
        PRINTOUT(F("serzRdel_Line; No file"), serzRdelFn_str);
        return false;
    }
    return true;
}  // Logger::serzLine


/* Deserializer functions

For deserialize, read  ASCII CSV records of the form
<marked epoch time> n*[<,values>]

deszRdelStart()   ~ to open file
deszLine()  to populate
    deszq_epochTime &

*/

/* Find fixed delimeter
 * behave as strchrnul() if goes past end of string
 */
char* Logger::deszFind(const char* in_line, char caller_id) {
    char* retResult = strchr(in_line, DELIM_CHAR2);
    if (NULL != retResult) return retResult;
    MS_DEEP_DBG(F("deszFind NULL found on "), caller_id);
    // For NULL return pointer as per strchrnul
    // should only occur on last search
    return (char*)(in_line + strlen(in_line));

    /*    The strchrnul() function is like strchr() except that if \p c is not
        found in \p s, then it returns a pointer to the null byte at the end
        of \p s, rather than \c NULL. (Glibc, GNU extension.)

        \return The strchrnul() function returns a pointer to the matched
        character, or a pointer to the null byte at the end of \p s (i.e.,
        \c s+strlen(s)) if the character is not found.
    //char *strchrnul(const char *in, int delim_char) */
}


bool Logger::deszRdelStart() {
    deszLinesRead = deszLinesUnsent = 0;

    deszq_nextChar = deszq_line;
    // Open - RD & WR. WR needed to be able to delete when complete.
    if (!serzRdelFile.open(serzRdelFn_str, (O_RDWR | O_CREAT))) {
        PRINTOUT(F("deRS; No file "), serzRdelFn_str);
        return false;
    } else {
        MS_DEEP_DBG(F("deRS open RDWR"), serzRdelFn_str);
    }
    return true;
}

bool Logger::deszQueuedStart() {
    deszLinesRead = deszLinesUnsent = 0;

    deszq_nextChar = deszq_line;
    // Open - RD & WR. WR needed to be able to delete when complete.
    // Expect serzQueuedFn to be setup in serzQueuedStart
    if (!serzQueuedFile.open(serzQueuedFn, O_RDWR)) {
        // This could be that there aren;t any Queued readings
        MS_DEEP_DBG(F("deQS; No file "), serzQueuedFn);
        // sd1_card_fatfs.ls();
        return false;
    } else {
        MS_DEEP_DBG(F("deQS open READ"), serzQueuedFn);
    }

    return true;
}
bool Logger::deszLine(File* filep) {
    char* errCheck;
    /* Scan through one line. Expect format
      <ascii Digits>,   representing integer STATUS
      <ascii Digitis>, represnting inteeger marked Epoch Time
      .... <ascii Digits> representing reading values

    Not renetrant, assumption: there is only deserialize going on at a time.
    Uses
    char    deszq_line[],
    uint8_t deszq_status
    long    deszq_epochTime
    char   *deszq_nextChar
            deszq_nextCharSz
    */

    uint16_t num_char = filep->fgets(deszq_line, QUEFILE_MAX_LINE);
    char*    orig_nextChar;

    if (0 == num_char) return false;
    deszLinesRead++;
    // First is the Status of record
    deszq_status = strtol(deszq_line, &errCheck, 10);
    if (errCheck == deszq_line) {
        PRINTOUT(F("deszLine Status err'"), deszq_line, F("'"));
        return false;  // EIO;
    }
    // Find next DELIM and go past it
    deszq_nextChar = 1 + deszFind(deszq_line, '1');
    if (deszq_nextChar == deszq_line) {
        PRINTOUT(F("deszLine epoch start not found"), deszq_line, F("'"));
        deszq_nextCharSz = 0;
        return false;
    }
    // Second is the epochTime,
    deszq_epochTime = strtol(deszq_nextChar, &errCheck, 10);
    if (errCheck == deszq_line) {
        PRINTOUT(F("deszLine Epoch err'"), deszq_line, F("'"));
        return false;  // EIO;
    }
    // Find next DELIM and go past it
    orig_nextChar  = deszq_nextChar;
    deszq_nextChar = 1 + deszFind(deszq_nextChar, '2');
    if (orig_nextChar == deszq_nextChar) {
        PRINTOUT(F("deszLine readung start not found"), deszq_line, F("'"));
        deszq_nextCharSz = 0;
        return false;
    }
    // Find sz of this field
    char* nextCharEnd = deszFind(deszq_nextChar, '3');
    deszq_nextCharSz  = nextCharEnd - deszq_nextChar;

    deszq_timeVariant_sz = strlen(deszq_nextChar) - 1;
    MS_DBG(F("deszLine Reading sz"), deszq_timeVariant_sz, F(":"), deszq_nextChar,
           F(":"));
    return true;
}

bool Logger::deszqNextCh(void) {
    char* deszq_old = deszq_nextChar;
    // Find next DELIM and go past it
    deszq_nextChar = 1 + deszFind(deszq_nextChar, 'L');
    if ((deszq_old == deszq_nextChar)) {
        deszq_nextCharSz = 0;
        PRINTOUT(F("deszqNextCh 1error:"), deszq_nextChar, F("'"));
        return false;
    }
    /* Find sz of this field
        either
    <value>,[..]
    <value><CR><LF>EOL
    EOF
    */
    char* nextCharEnd = strchr(deszq_nextChar, DELIM_CHAR2);
    deszq_nextCharSz  = strlen(deszq_nextChar);
    if ((0 == deszq_nextCharSz)) {
        // Found end of line
        MS_DBG(F("dSRN unexpected EOL "));
        return false;
    } else if (NULL == nextCharEnd) {
        // Found <value>EOF ~ nextSr_sz is valid
        deszq_nextCharSz -= 1;  // take off turds <LF>
        MS_DEEP_DBG(F("dSRN info "), deszq_nextCharSz, " '", deszq_nextChar,
                    "'");
        // return true
    } else {
        // expect to have found <value>,[..]
        // if found ,, then invalid and finish
        deszq_nextCharSz = nextCharEnd - deszq_nextChar;
        if (0 == deszq_nextCharSz) {
            MS_DEEP_DBG(F("dSRN unexpected 0 bytes "));
            return false;
        }
    }
    return true;
}

bool Logger::deszRdelClose(bool deleteFile) {
    bool retVal = false;

    if (!(retVal = serzRdelFile.close())) {
        PRINTOUT(F("deSRC close err"), serzRdelFn_str);
        sd1_Err("serzBegin err close");
    } else {
        MS_DEEP_DBG(F("deSRC closed"), serzRdelFn_str);
    }
    if (deleteFile) {
        // if (!(retVal = serzRdelFile.remove())) {
        if (!(retVal = sd1_card_fatfs.remove(serzRdelFn_str))) {
            PRINTOUT(F("deSRC remove err"), serzRdelFn_str);
            sd1_Err("serzBegin err remove");
        }
        MS_DEEP_DBG(F("deSRC removed"), serzRdelFn_str);
    }

    return retVal;
}

/* Prototyping des example
 */

uint16_t serialCnt = 0;
// SdFat    sdfat_phy;
// SdFile   rootDir;
bool Logger::deszDbg(void) {
    // char* next_token;
#define TEMPBUF_SZ 37
    char tempBuffer[TEMPBUF_SZ] = "";
    if (++serialCnt >= SERIALIZE_sendEveryX_NUM) {
        String d_str(80);
        serialCnt = 0;
        deszRdelStart();
        while (deszRdelLine()) {
            d_str = formatDateTime_ISO8601(deszq_epochTime) + ';';
            // next_token = find_chars_or_comment(deszq_nextChar,
            // DELIM_CHAR2);
            tempBuffer[0] = 0;
            strncat(tempBuffer, deszq_nextChar, deszq_nextCharSz);
            strcat(tempBuffer, ";");
            // PRINTOUT("Sn=", tempBuffer);
            d_str.concat(tempBuffer);
            // getline
            while (deszqNextCh()) {
                tempBuffer[0] = 0;
                strncat(tempBuffer, deszq_nextChar, deszq_nextCharSz);
                strcat(tempBuffer, ";");
                d_str.concat(tempBuffer);
                // PRINTOUT("t='", tempBuffer, F("'"));
            }
            PRINTOUT("L=", d_str, "Stat=", deszq_status);
        }
        deszRdelClose(true);  // Delete serial file
    }
    return true;
}

bool Logger::postLogOpen(const char* postLogNam_str) {
    bool retVal = false;
#if defined MS_LOGGERBASE_POSTS
    // Generate the file name from logger ID and date
    // Create rotating log of 4 chars YYMM - formatDateTime is YYYY MM DD
     String nameTemp = formatDateTime_str(getNowLocalEpoch());

    // Drop middle _ and get YYMM
    String fileName = String(postLogNam_str + nameTemp.substring(2, 4) + nameTemp.substring(5, 7) + ".log");

    // Convert the string filename to a character file name for SdFat
    uint16_t fileNameLength = fileName.length()+2;
    MS_DBG(F("PLO postLog file"), fileName, F("res len"),fileNameLength);
    char    charFileName[fileNameLength];
    fileName.toCharArray(charFileName, fileNameLength);

    // Attempt to open an existing file
    retVal = postsLogHndl.open(charFileName, (O_WRITE | O_AT_END));
    if (!retVal) {
        retVal = postsLogHndl.open(charFileName,
                                   (O_CREAT | O_WRITE | O_AT_END));
        if (!retVal) {
            PRINTOUT(F("logPLO err opening"), charFileName);

        } else {
            setFileTimestamp(postsLogHndl, T_CREATE,true);
            MS_DBG(F("logPLO new file"), charFileName);
        }
    }
#endif  // MS_LOGGERBASE_POSTS
    return retVal;
}
bool Logger::postLogOpen() {
    bool     retVal    = false;
#if defined MS_LOGGERBASE_POSTS
    retVal =postLogOpen(postsLogFn_str);
#endif // 
    return retVal;
}
void        Logger::postLogClose() {
#if defined MS_LOGGERBASE_POSTS

    setFileTimestamp(postsLogHndl, (T_WRITE),true);  //| T_ACCESS
    postsLogHndl.close();


#endif  // MS_LOGGERBASE_POSTS
}

void Logger::postLogLine(uint32_t tmr_ms, int16_t rspParam) {
// If debug ...keep record
#if defined MS_LOGGERBASE_POSTS
#if 0
    if (0 == postsLogHndl.print(getNowEpochUTC())) {
        PRINTOUT(F("publishDataQueuedToRemote postsLog err"));
    }
#else

    char tempBuffer[TEMP_BUFFER_SZ];
    //Print internal time
    formatDateTime_str(getNowLocalEpoch())
        .toCharArray(tempBuffer, TEMP_BUFFER_SZ);
    postsLogHndl.print(tempBuffer);
#endif
    postsLogHndl.print(F(",POST,"));
    itoa(rspParam, tempBuffer, 10);
    postsLogHndl.print(tempBuffer);
        postsLogHndl.print(F(","));
        itoa(tmr_ms, tempBuffer, 10);
        postsLogHndl.print(tempBuffer);
        postsLogHndl.print(F(","));
    postsLogHndl.print(deszq_line);
#endif  //#if defined MS_LOGGERBASE_POSTS
}

void Logger::postLogLine(const char *logMsg,bool addCRNL) {
#if defined MS_LOGGERBASE_POSTS 
    bool wasOpen =true;   
    if (!postsLogHndl.isOpen()) {
        wasOpen=false;
        if (!postLogOpen()) {
            PRINTOUT(F("postLogLine can't open file"));      
            //TODO possible reboot         
            return;
        } 
    }
    char tempBuffer[TEMP_BUFFER_SZ];
    //Print internal time
    formatDateTime_str(getNowLocalEpoch())
        .toCharArray(tempBuffer, TEMP_BUFFER_SZ);    
    postsLogHndl.print(tempBuffer);
    postsLogHndl.print(F(",MSG,"));
    postsLogHndl.print(logMsg);
    if (addCRNL)postsLogHndl.println();

    if (!wasOpen) postLogClose();
#endif //MS_LOGGERBASE_POSTS
}
/*
Cleanup if necessary
*/

bool Logger::listFile(File* filep, char* fn_str, char* uid) {
    char    loc_line[QUEFILE_MAX_LINE];
    int16_t num_char;
    int16_t num_cnt = 0;

    if (!filep->open(fn_str, O_READ)) {
        PRINTOUT(F("listFile; No file "), fn_str);
        sd1_Err("listFile: no file2");
        return false;
    } else {
        MS_DBG(F("listFile"), fn_str, uid, F("<beg>"));
    }

    while (0 < (num_char = filep->fgets(loc_line, QUEFILE_MAX_LINE))) {
        PRINTOUT(++num_cnt, loc_line);
    }
    if (0 > num_char) {
        PRINTOUT(F("listFile err"), num_char);
        sd1_Err("listFile err2");
    }
    if (!filep->close()) {
        PRINTOUT(F("listFile; close err "), fn_str);
        sd1_Err("listFile close err2");
        return false;
    }
    MS_DBG(F("listFile"), uid, F("<end>"));
    return true;
}

/* This tests all the primitives used to access the SD card.
 */
bool Logger::serzBegin(void) {
    bool dslStat_bool;

    MS_DBG(F("serzBegin list1---"));
    if (!sd1_card_fatfs.ls()) {
        // MS_DBG(F("serzBegin ls err"));
        sd1_Err("serzBegin err ls");
    } else {
        MS_DBG(F("---1Complete"));
    }

    // Test  RDELAY.TXT
    serzRdelFile.open(serzRdelFn_str, RDEL_OFLAG);
    serzRdelFile.println(F("1,1595653100,1,4.730,-38"));
    serzRdelFile.println(F("1,1595653200,2,4.730,-38"));
    serzRdelFile.close();

    serzRdelFile.open(serzRdelFn_str, RDEL_OFLAG);
    serzRdelFile.println(F("1,1595653300,3,4.730,-38"));
    serzRdelFile.println(F("1,1595653400,4,4.730,-38"));
    serzRdelFile.close();

    PRINTOUT(F("serzBegin list2---"));
    if (!sd1_card_fatfs.ls()) {
        // MS_DBG(F("serzBegin ls err"));
        sd1_Err("serzBegin err ls");
    } else {
        PRINTOUT(F("---2Complete"));
    }
    listFile(&serzRdelFile, (char*)serzRdelFn_str, (char*)"1");


    deszRdelStart();
    int16_t cnt_num = 0;
    while (0 < (dslStat_bool = deszRdelLine())) {
        PRINTOUT(++cnt_num, F("] "), dslStat_bool, deszq_line);
    }

    deszRdelClose(true);  // Test for delete
    PRINTOUT(F("serzBegin list3---"));
    if (!sd1_card_fatfs.ls()) {
        // MS_DBG(F("serzBegin ls err"));
        sd1_Err("serzBegin err ls");
    } else {
        PRINTOUT(F("---3Complete"));
    }

// Test QUED algorithims ~ use QUE7.txt
#define QUE_TST '7'
#define TESTQ_FN_STR "QUE7.TXT"
    MS_DBG(F("TESTQ START"));
    if (sd1_card_fatfs.exists(TESTQ_FN_STR)) {
        // PRINTOUT(F("serzBegin removing "), TESTQ_FN_STR);
        if (!sd1_card_fatfs.remove(TESTQ_FN_STR)) {
            PRINTOUT(F("serzBegin err remove"), TESTQ_FN_STR);
            sd1_Err("serzBegin remove");
        }
    } else {
        MS_DBG(F("serzBegin no "), TESTQ_FN_STR);
    }
    // Test1 ** QUED new file name & update
    MS_DBG(F("TESTQ1"));
    serzQueuedStart((char)(QUE_TST));
    serzQueuedFile.println(F("1,1595654100,1,4.7,-38"));
    serzQueuedFile.println(F("1,1595654200,2,4.7,-38"));
    serzQueuedCloseFile(false);


    // Test2 ** QUED file update
    MS_DBG(F("TESTQ2"));
    if (!serzQueuedFile.open(serzQueuedFn, (O_WRITE | O_AT_END))) {
        // Could be that there are no retrys.
        PRINTOUT(F("serzQueuedFile.open err"), serzQueuedFn);
        sd1_Err("serzQueuedFile.open err2");
        return false;
    } else {
        PRINTOUT(F("Testq2 Opened"), serzQueuedFn);
    }
    serzQueuedFile.println(F("1,1595654300,3,4.7,-38"));
    serzQueuedFile.println(F("1,1595654400,4,4.7,-38"));
    if (!serzQueuedCloseFile(false)) return false;

    PRINTOUT(F("serzBegin list4---"));
    if (!sd1_card_fatfs.ls()) {
        // MS_DBG(F("serzBegin ls err"));
        sd1_Err("serzBegin err4 ls");
        return false;
    } else {
        PRINTOUT(F("---4Complete"));
    }
    listFile(&serzQueuedFile, serzQueuedFn, (char*)"2");

    // Test3 ** QUED file rollover
    MS_DBG(F("TESTQ3"));
    if (!deszQueuedStart()) return false;

    dslStat_bool = deszQueuedLine();
    MS_DBG(F("1: deszq_line"), dslStat_bool, deszq_line);
    if (!dslStat_bool) return false;
    dslStat_bool = deszQueuedLine();
    MS_DBG(F("2: deszq_line"), dslStat_bool, deszq_line);
    if (!dslStat_bool) return false;
    // only the 1: should be dropped
    dslStat_bool = serzQueuedCloseFile(true);
    PRINTOUT(F("serzBegin list5---"));
    if (!sd1_card_fatfs.ls()) {
        // MS_DBG(F("serzBegin ls err"));
        sd1_Err("serzBegin err5 ls");
        return false;
    } else {
        PRINTOUT(F("---5Complete"));
    }
    listFile(&serzQueuedFile, serzQueuedFn, (char*)"3");
    if (!dslStat_bool) return false;

    if (sd1_card_fatfs.exists(serzQueuedFn)) {
        PRINTOUT(F("serzBegin removing "), serzQueuedFn);
        if (!sd1_card_fatfs.remove(serzQueuedFn)) {
            PRINTOUT(F("serzBegin err remove"), serzQueuedFn);
            sd1_Err("serzBegin err6 remove");
        }
    } else {
        PRINTOUT(F("serzBegin no "), TEMP_BASE_FN_STR);
    }

    // Cleanup

    MS_DBG(F("TESTQ CLEANUP"));
    if (sd1_card_fatfs.exists(TEMP_BASE_FN_STR)) {
        PRINTOUT(F("serzBegin removing "), TEMP_BASE_FN_STR);
        if (!sd1_card_fatfs.remove(TEMP_BASE_FN_STR)) {
            PRINTOUT(F("serzBegin err remove"), TEMP_BASE_FN_STR);
            sd1_Err("serzBegin err6 remove");
        }
    } else {
        MS_DBG(F("serzBegin no "), TEMP_BASE_FN_STR);
    } /* */
    MS_DBG(F("TESTQ END END END \n\n"));
    return true;
}

// Convert a date-time object into a formatted string
String Logger::formatDateTime_str(DateTime& dt) {
    String dateTimeStr;
    dt.addToString(dateTimeStr);
    return dateTimeStr;
}

// Convert an epoch time into a formatted string
String Logger::formatDateTime_str(uint32_t epochTime) {
    DateTimeClass(dt1, epochTime);
    return formatDateTime_str(dt1);
}
// End LoggerBase.cpp
