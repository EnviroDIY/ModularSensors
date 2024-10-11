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
int8_t Logger::_loggerTimeZone = 0;
// Initialize the static time adjustment
int8_t Logger::_loggerRTCOffset = 0;
// Initialize the static timestamps
uint32_t Logger::markedLocalUnixTime = 0;
uint32_t Logger::markedUTCUnixTime   = 0;
// Initialize the testing/logging flags
volatile bool Logger::isLoggingNow = false;
volatile bool Logger::isTestingNow = false;
volatile bool Logger::startTesting = false;


// Initialize the internal logger clock
loggerClock Logger::_loggerClock;


// Constructors
Logger::Logger(const char* loggerID, uint16_t loggingIntervalMinutes,
               int8_t SDCardSSPin, int8_t mcuWakePin, VariableArray* inputArray)
    : _SDCardSSPin(SDCardSSPin),
      _mcuWakePin(mcuWakePin) {
    // Set parameters from constructor
    setLoggerID(loggerID);
    setLoggingInterval(loggingIntervalMinutes);
    setVariableArray(inputArray);

    // _core_epoch = getProcessorEpochStart();

    // Set the testing/logging flags to false
    isLoggingNow = false;
    isTestingNow = false;
    startTesting = false;

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

    // _core_epoch = getProcessorEpochStart();

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

    // _core_epoch = getProcessorEpochStart();

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
    // reset the testing value, just in case
    Logger::startTesting = false;
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
// Adds a loggerModem object to the logger
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
                _loggerClock.setRTClock(_logModem->getNISTTime(),
                                        epochStart::unix_epoch);
                MS_DBG(F("Current logger time after sync is"),
                       formatDateTime_ISO8601(
                           getNowLocalEpoch(epochStart::unix_epoch),
                           epochStart::unix_epoch));
                success = true;
                _logModem->updateModemMetadata();
            } else {
                PRINTOUT(F("Could not connect to internet for clock sync."));
            }
        } else {
            PRINTOUT(F("Could not wake modem for clock sync."));
        }
        watchDogTimer.resetWatchDog();

        // Power down the modem now that we are done with it
        MS_DBG(F("Powering down modem after clock sync."));
        _logModem->disconnectInternet();
        _logModem->modemSleepPowerDown();
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
    MS_DBG(F("Sending out remote data."));

    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
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
#if 0
// Duplicates for backwards compatibility
void Logger::setTimeZone(int8_t timeZone) {
    setLoggerTimeZone(timeZone);
}
int8_t Logger::getTimeZone(void) {
    return getLoggerTimeZone();
}
#endif
// Sets the static timezone that the RTC is programmed in
// I VERY VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setRTCTimeZone(int8_t timeZone) {
    _loggerClock.setRTCTimeZone(timeZone);
}
int8_t Logger::getRTCTimeZone(void) {
    return _loggerClock.getRTCTimeZone();
}


// This set the offset between the built-in clock and the time zone where
// the data is being recorded.  If your RTC is set in UTC and your logging
// timezone is EST, this should be -5.  If your RTC is set in EST and your
// timezone is EST this does not need to be called.
// You can either set the RTC offset directly or set the offset between the
// RTC and the logger
void Logger::setTZOffset(int8_t offset) {
    _loggerRTCOffset = offset;
    _loggerClock.setRTCTimeZone(Logger::_loggerTimeZone +
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
uint32_t Logger::getNowLocalEpoch(epochStart epoch) {
    uint32_t currentEpochTime = _loggerClock.getNowUTCEpoch(epoch);
    // Do NOT apply an offset if the timestamp is obviously bad
    if (_loggerClock.isRTCSane(currentEpochTime, epoch))
        currentEpochTime += ((uint32_t)_loggerRTCOffset) * 3600;
    return currentEpochTime;
}
uint32_t Logger::getNowUTCEpoch(epochStart epoch) {
    return _loggerClock.getNowUTCEpoch(epoch);
}
void Logger::setNowUTCEpoch(uint32_t ts, epochStart epoch) {
    return _loggerClock.setNowUTCEpoch(ts, epoch);
}

// This converts an epoch time (unix time) into a ISO8601 formatted string.
// It assumes the supplied date/time is in the LOGGER's timezone and adds
// the LOGGER's offset as the time zone offset in the string. code modified
// from parts of the SparkFun RV-8803 library
String Logger::formatDateTime_ISO8601(uint32_t epochTime, epochStart epoch) {
    return _loggerClock.formatDateTime_ISO8601(epochTime, _loggerTimeZone,
                                               epoch);
}

// This checks that the logger time is within a "sane" range
bool Logger::isRTCSane(void) {
    return _loggerClock.isRTCSane();
}


// This sets static variables for the date/time - this is needed so that all
// data outputs (SD, EnviroDIY, serial printing, etc) print the same time
// for updating the sensors - even though the routines to update the sensors
// and to output the data may take several seconds.
// It is not currently possible to output the instantaneous time an individual
// sensor was updated, just a single marked time.  By custom, this should be
// called before updating the sensors, not after.
void Logger::markTime(void) {
    Logger::markedUTCUnixTime =
        _loggerClock.getNowUTCEpoch(epochStart::unix_epoch);
    Logger::markedLocalUnixTime = markedUTCUnixTime +
        ((uint32_t)_loggerRTCOffset) * 3600;
}


// This checks to see if the CURRENT time is an even interval of the logging
// rate
bool Logger::checkInterval(void) {
    bool     retval;
    uint32_t checkTime = getNowLocalEpoch(epochStart::unix_epoch);
    uint16_t interval  = _loggingIntervalMinutes;
    if (_initialShortIntervals > 0) {
        // log the first few samples at an interval of 1 minute so that
        // operation can be quickly verified in the field
        _initialShortIntervals -= 1;
        interval = 1;
    }

    MS_DBG(F("Current Unix Timestamp:"), checkTime, F("->"),
           formatDateTime_ISO8601(checkTime, epochStart::unix_epoch));
    MS_DBG(F("Logging interval in seconds:"), (interval * 60));
    MS_DBG(F("Mod of Logging Interval:"), checkTime % (interval * 60));

    if (checkTime % (interval * 60) == 0) {
        // Update the time variables with the current time
        markTime();
        MS_DBG(F("Time marked at (unix):"), Logger::markedLocalUnixTime);
        MS_DBG(F("Time to log!"));
        retval = true;
    } else {
        MS_DBG(F("Not time yet."));
        retval = false;
    }
    return retval;
}


// This checks to see if the MARKED time is an even interval of the logging rate
bool Logger::checkMarkedInterval(void) {
    bool retval;
    MS_DBG(F("Marked Time:"), Logger::markedLocalUnixTime,
           F("Logging interval in seconds:"), (_loggingIntervalMinutes * 60),
           F("Mod of Logging Interval:"),
           Logger::markedLocalUnixTime % (_loggingIntervalMinutes * 60));

    if (Logger::markedLocalUnixTime != 0 &&
        (Logger::markedLocalUnixTime % (_loggingIntervalMinutes * 60) == 0)) {
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
// functions.)
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

    // Send a message that we're getting ready
    MS_DBG(F("Preparing clock interrupts to wake processor"));

#if defined(MS_USE_RV8803)
    // Disable any previous interrupts
    rtc.disableAllInterrupts();
    // Clear all flags in case any interrupts have occurred.
    rtc.clearAllInterruptFlags();
    // Enable a periodic update for every minute
    rtc.setPeriodicTimeUpdateFrequency(TIME_UPDATE_1_MINUTE);
    // Enable the hardware interrupt
    rtc.enableHardwareInterrupt(UPDATE_INTERRUPT);

#elif defined(MS_USE_DS3231)

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
#endif

#if defined(MS_USE_RV8803) || defined(MS_USE_DS3231)
    // Set up a pin to hear clock interrupt and attach the wake ISR to it
    MS_DBG(F("Enabling interrupts on pin"), _mcuWakePin);
    // Set the pin mode, although this shouldn't really need to be re-set here
    pinMode(_mcuWakePin, _wakePinMode);
    // attach the interrupt
    enableInterrupt(_mcuWakePin, wakeISR, RISING);

#if defined(ARDUINO_ARCH_SAMD) && !defined(__SAMD51__)
    // Reconfigure the external interrupt controller (EIC) clock after attaching
    // the interrupt This is needed because the attachInterrupt function will
    // reconfigure the clock source for the EIC to GCLK0 every time a new
    // interrupt is attached
    // - and after being detached, reattaching the same interrupt is just like a
    // new one). We need to switch the EIC source back to our configured GCLK2.
    MS_DEEP_DBG(F("Re-attaching the EIC to GCLK2"));
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(2) |  // Select generic clock 2
        GCLK_CLKCTRL_CLKEN |       // Enable the generic clock clontrol
        GCLK_CLKCTRL_ID(GCM_EIC);  // Feed the GCLK to the EIC
    while (GCLK->STATUS.bit.SYNCBUSY) {
        // Wait for synchronization
    }

    // get the interrupt number associated with the pin
    EExt_Interrupts in = g_APinDescription[_mcuWakePin].ulExtInt;
    // Enable wakeup capability on pin in case being used during sleep
    EIC->WAKEUP.reg |= (1 << in);
#endif  // #if defined(ARDUINO_ARCH_SAMD) && ! defined(__SAMD51__)
#endif  //#if defined(MS_USE_RV8803) || defined(MS_USE_DS3231)

#if defined(MS_USE_RTC_ZERO)

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

#endif  // defined(MS_USE_RTC_ZERO)


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
    MS_DEEP_DBG(F("Ending I2C"));
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

    // Disable the watch-dog timer
    MS_DEEP_DBG(F("Disabling the watchdog"));
    watchDogTimer.disableWatchDog();

// Wait until the serial ports have finished transmitting
// This does not clear their buffers, it just waits until they are finished
// TODO(SRGDamia1):  Make sure can find all serial ports
#if defined(STANDARD_SERIAL_OUTPUT)
    STANDARD_SERIAL_OUTPUT.flush();  // for debugging
#endif
#if defined(DEBUGGING_SERIAL_OUTPUT)
    DEBUGGING_SERIAL_OUTPUT.flush();  // for debugging
#endif

#if defined(ARDUINO_ARCH_SAMD)

#if !defined(USE_TINYUSB) && defined(USBCON)
    // Detach the USB, iff not using TinyUSB
    MS_DEEP_DBG(F("Detaching USB"));
    Serial.flush();  // wait for any outgoing messages on Serial = USB
    USBDevice.detach();
    USBDevice.end();
    USBDevice.standby();
#endif

    // Copied from Adafruit SleepDog
    // Enable standby sleep mode (deepest sleep) and activate.
    // Insights from Atmel ASF library.

    // NOTE: The macros SAMD20_SERIES and SAMD21_SERIES capture all of the MCU
    // defines for all processors in that series.

#if defined(__SAMD51__)
    // Set the sleep config
    // PM_SLEEPCFG_SLEEPMODE_BACKUP = 0x4
    PM->SLEEPCFG.bit.SLEEPMODE = 0x4;
    while (PM->SLEEPCFG.bit.SLEEPMODE != 0x4)
        ;  // Wait for it to take
#else
    // Don't fully power down flash when in sleep
    // Adafruit SleepDog and ArduinoLowPower both do this.
    // TODO: Figure out if this is really necessary
    // NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;

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

    // Actually sleep
    __DSB();  // Data sync to ensure outgoing memory accesses complete
    __WFI();  // Wait for interrupt (places device in sleep mode)

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

#if defined(ARDUINO_ARCH_SAMD)
#if !defined(__SAMD51__)
    // Enable systick interrupt
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
#endif
    // Reattach the USB
#if !defined(USE_TINYUSB) && defined(USBCON)
    USBDevice.init();
    USBDevice.attach();
#endif
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
    watchDogTimer.enableWatchDog();

    // Re-start the I2C interface
    MS_DEEP_DBG(F("Restarting I2C"));
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

    // Stop the clock from sending out any interrupts while we're awake.
    // There's no reason to waste thought on the clock interrupt if it
    // happens while the processor is awake and doing other things.
#if defined(MS_USE_RV8803)
    MS_DEEP_DBG(F("Unsetting the alarm on the RV-8803"));
    rtc.disableHardwareInterrupt(UPDATE_INTERRUPT);
#elif defined(MS_USE_DS3231)
    MS_DEEP_DBG(F("Unsetting the alarm on the DS2321"));
    rtc.disableInterrupts();
#endif

#if defined(MS_USE_RV8803) || defined(MS_USE_DS3231)
    // Detach the from the pin
    disableInterrupt(_mcuWakePin);
#endif

#if defined(MS_USE_RTC_ZERO)
    MS_DEEP_DBG(F("Unsetting the alarm on the built in RTC"));
    zero_sleep_rtc.disableAlarm();
#endif

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
    fileName += formatDateTime_ISO8601(getNowLocalEpoch(epochStart::unix_epoch),
                                       epochStart::unix_epoch)
                    .substring(0, 10);
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
    String iso8601   = formatDateTime_ISO8601(Logger::markedLocalUnixTime,
                                              epochStart::unix_epoch);
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


    SdSpiConfig customSdConfig(static_cast<SdCsPin_t>(_SDCardSSPin),
                               (uint8_t)(DEDICATED_SPI), SPI_FULL_SPEED, &SPI);

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


// Protected helper function - This sets a timestamp on a file
void Logger::setFileTimestamp(File& fileToStamp, uint8_t stampFlag) {
    // Create a temporary variable for the epoch time
    // NOTE: time_t is a typedef for uint32_t, defined in time.h
    time_t t = getNowLocalEpoch(epochStart::unix_epoch);
    // create a temporary time struct
    // tm is a struct for time parts, defined in time.h
    struct tm* tmp = gmtime(&t);
    fileToStamp.timestamp(stampFlag, tmp->tm_year, tmp->tm_mon + 1,
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
void Logger::testingMode(bool sleepBeforeReturning) {
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
        PRINTOUT(
            F("Current logger time is"),
            formatDateTime_ISO8601(getNowLocalEpoch(epochStart::unix_epoch),
                                   epochStart::unix_epoch));
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

#if defined(ARDUINO_ARCH_SAMD)
    MS_DBG(F("Disabling the USB on standby to lower sleep current"));
    USB->DEVICE.CTRLA.bit.ENABLE = 0;  // Disable the USB peripheral
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE)
        ;                                // Wait for synchronization
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 0;  // Deactivate run on standby
    USB->DEVICE.CTRLA.bit.ENABLE   = 1;  // Enable the USB peripheral
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE)
        ;  // Wait for synchronization
#endif

    MS_DBG(F(
        "Setting up a watch-dog timer to fire after 15 minutes of inactivity"));
    watchDogTimer.setupWatchDog((uint32_t)(5 * 60 * 3));
    // Enable the watchdog
    watchDogTimer.enableWatchDog();

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

    MS_DEEP_DBG(F("Beginning the internal logger clock"));
    _loggerClock.begin();
    PRINTOUT(F("Current localized logger time is:"),
             formatDateTime_ISO8601(getNowLocalEpoch(epochStart::unix_epoch),
                                    epochStart::unix_epoch));
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


    for (uint8_t i = 0; i < MAX_NUMBER_SENDERS; i++) {
        if (dataPublishers[i] != nullptr) {
            PRINTOUT(F("Data will be published to ["), i, F("]"),
                     dataPublishers[i]->getEndpoint());
        }
    }

    PRINTOUT(F("Logger portion of setup finished.\n"));
}


// This is a one-and-done to log data
void Logger::logData(bool sleepBeforeReturning) {
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
    if (Logger::startTesting) testingMode(sleepBeforeReturning);

    if (sleepBeforeReturning) {
        // Sleep
        systemSleep();
    }
}
// This is a one-and-done to log data
void Logger::logDataAndPublish(bool sleepBeforeReturning) {
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
        STANDARD_SERIAL_OUTPUT.println();
        _internalArray->printSensorData(&STANDARD_SERIAL_OUTPUT);
        STANDARD_SERIAL_OUTPUT.println();
#endif

        // Create a csv data record and save it to the log file
        logToSD();

        if (_logModem != nullptr) {
            MS_DBG(F("Waking up"), _logModem->getModemName(), F("..."));
            if (_logModem->modemWake()) {
                // Connect to the network
                watchDogTimer.resetWatchDog();
                MS_DBG(F("Connecting to the Internet..."));
                if (_logModem->connectInternet(240000L)) {
                    // Publish data to remotes
                    watchDogTimer.resetWatchDog();
                    publishDataToRemotes();
                    watchDogTimer.resetWatchDog();

                    if ((Logger::markedLocalUnixTime != 0 &&
                         Logger::markedLocalUnixTime % 86400 == 43200) ||
                        !_loggerClock.isRTCSane(Logger::markedLocalUnixTime)) {
                        // Sync the clock at noon
                        MS_DBG(F("Running a daily clock sync..."));
                        _loggerClock.setRTClock(_logModem->getNISTTime(),
                                                epochStart::unix_epoch);
                        MS_DBG(F("Current logger time after sync is"),
                               formatDateTime_ISO8601(
                                   getNowLocalEpoch(epochStart::unix_epoch),
                                   epochStart::unix_epoch));
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
    if (Logger::startTesting) testingMode(sleepBeforeReturning);

    if (sleepBeforeReturning) {
        // Sleep
        systemSleep();
    }
}
