/**
 * @file LoggerBase.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the LoggerBase class which handles basic logging functions.
 *
 * Basic logger functions include communicating with a real-time clock, saving
 * to an SD card, and publishing data to remotes via a loggerModem connection.
 */

// Header Guards
#ifndef SRC_LOGGERBASE_H_
#define SRC_LOGGERBASE_H_

// Include config before anything else
#include "ModSensorConfig.h"

// Debugging Statement
// #define MS_LOGGERBASE_DEBUG
// #define MS_LOGGERBASE_DEBUG_DEEP

#ifdef MS_LOGGERBASE_DEBUG
#define MS_DEBUGGING_STD "LoggerBase"
#endif

#ifdef MS_LOGGERBASE_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerBase"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableArray.h"
#include "LoggerModem.h"
#include "ClockSupport.h"
#include <Wire.h>
// // For time
// #include <time.h>

// Bring in the libraries to handle the processor sleep/standby modes
// The SAMD library can also the built-in clock on those modules
/**
 * @def extendedWatchDog
 *
 * A define to simplify inclusing of either a AVR or SAMD based watchdog
 */
#if defined(ARDUINO_ARCH_SAMD)
#include "WatchDogs/WatchDogSAMD.h"
#define extendedWatchDog extendedWatchDogSAMD
#elif defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
#include <avr/power.h>
#include <avr/sleep.h>
#include "WatchDogs/WatchDogAVR.h"
#define extendedWatchDog extendedWatchDogAVR
#endif

#include <SdFat.h>  // To communicate with the SD card


class dataPublisher;  // Forward declaration


/**
 * @brief The "Logger" Class handles low power sleep for the main processor,
 * interfacing with the real-time clock and modem, writing to the SD card, and
 * passing data on to the data publishers.
 *
 * A logger is a device that can control all functions of the modem sensors and
 * that are attached to it and save the values of all variables measured by
 * those sensors to an attached SD card.  It must also work with a real-time
 * clock to give timestamps to values.  It may also work with a loggerModem for
 * internet connection and send out data over the internet through one or more
 * data publishers.
 *
 * In this library, all loggers are Arduino-style small processor circuit
 * boards.
 *
 * @ingroup base_classes
 */
class Logger {
    /**
     * @brief The dataPublisher class contains endpoints for logged data and the
     * proper formats for sending it.
     */
    friend class dataPublisher;

 public:
    /**
     * @brief Construct a new Logger object.
     *
     * @param loggerID A name for the logger - unless otherwise specified, files
     * saved to the SD card will be named with the logger id and the date the
     * file was started.
     * @param loggingIntervalMinutes The frequency in minutes at which data
     * should be logged
     * @param SDCardSSPin The pin of the chip select/slave select for the SPI
     * connection to the SD card
     * @param mcuWakePin The pin used to wake the logger from deep sleep -
     * expected to be attached to an alarm pin of the real-time clock.  Use a
     * value of -1 to prevent the board from sleeping.
     * @param inputArray A pointer to a variableArray object instance providing
     * data to be logged.  This is NOT an array of variables, but an object of
     * the variable array class.
     */
    Logger(const char* loggerID, uint16_t loggingIntervalMinutes,
           int8_t SDCardSSPin, int8_t mcuWakePin, VariableArray* inputArray);
    /**
     * @brief Construct a new Logger object.
     *
     * @param loggerID A name for the logger - unless otherwise specified, files
     * saved to the SD card will be named with the logger id and the date the
     * file was started.
     * @param loggingIntervalMinutes The frequency in minutes at which data
     * should be logged
     * @param inputArray A variableArray object instance providing data to be
     * logged.  This is NOT an array of variables, but an object of the variable
     * array class.
     */
    Logger(const char* loggerID, uint16_t loggingIntervalMinutes,
           VariableArray* inputArray);
    /**
     * @brief Construct a new Logger object.
     *
     * @note The logger ID and logging interval must be specified before the
     * logger object can be used.
     */
    Logger();
    /**
     * @brief Destroy the Logger object - takes no action.
     */
    virtual ~Logger();

    // ===================================================================== //
    /**
     * @anchor logger_basic_parameters
     * @name Basic Logging Parameters
     * Public functions to get and set basic logging paramters
     */
    /**@{*/
    // ===================================================================== //
    /**
     * @brief Set the Logger ID.
     *
     * Unless otherwise specified, files saved to the SD card will be named with
     * the logger id and the date the file was started.
     *
     * @param loggerID A pointer to the logger ID
     */
    void setLoggerID(const char* loggerID);
    /**
     * @brief Get the Logger ID.
     *
     * @return A pointer to the logger ID
     */
    const char* getLoggerID() {
        return _loggerID;
    }

    /**
     * @brief Set the logging interval in minutes.
     *
     * @param loggingIntervalMinutes The frequency with which to update sensor
     * values and write data to the SD card.
     */
    void setLoggingInterval(uint16_t loggingIntervalMinutes);
    /**
     * @brief Get the Logging Interval.
     *
     * @return The logging interval in minutes
     */
    uint16_t getLoggingInterval() {
        return _loggingIntervalMinutes;
    }

    /**
     * @brief Set the number of 1-minute intervals at the start before logging
     * on the regular logging interval.
     *
     * @param initialShortIntervals The number of 1-minute intervals at
     * the start before logging on the regular logging interval
     */
    void setinitialShortIntervals(uint16_t initialShortIntervals);
    /**
     * @brief Get the number of 1-minute intervals at the start before logging
     * on the regular logging interval
     *
     * @return The number of 1-minute intervals at the start before logging
     * on the regular logging interval
     */
    uint16_t getinitialShortIntervals() {
        return _initialShortIntervals;
    }

    /**
     * @brief Set the universally unique identifier (UUID or GUID) of the
     * sampling feature.
     *
     * @param samplingFeatureUUID A pointer to the sampling feature UUID
     */
    void setSamplingFeatureUUID(const char* samplingFeatureUUID);
    /**
     * @brief Get the Sampling Feature UUID.
     *
     * @return The sampling feature UUID
     */
    const char* getSamplingFeatureUUID() {
        return _samplingFeatureUUID;
    }

    /**
     * @brief Set a digital pin number (on the mcu) to use to control power to
     * the SD card and activate it as an output pin.
     *
     * Because this sets the pin mode, this function should only be called
     * during the `setup()` or `loop()` portion of an Arduino program.
     *
     * @warning This functionality is not tested!
     *
     * @param SDCardPowerPin A digital pin number on the mcu controlling power
     * to the SD card.
     */
    void setSDCardPwr(int8_t SDCardPowerPin);
    /**
     * @brief Send power to the SD card by setting the SDCardPowerPin `HIGH`.
     *
     * Optionally waits for the card to "settle."  Has no effect if a pin has
     * not been set to control power to the SD card.
     *
     * @param waitToSettle True to add a short (6ms) delay between powering on
     * the card and beginning initialization.  Defaults to true.
     */
    void turnOnSDcard(bool waitToSettle = true);
    /**
     * @brief Cut power to the SD card by setting the SDCardPowerPin `LOW`.
     *
     * Optionally waits for the card to do "housekeeping" before cutting the
     * power.  Has o effect if a pin has not been set to control power to the SD
     * card.
     *
     * @param waitForHousekeeping True to add a 1 second delay between to allow
     * any on-chip writing to complete before cutting power.  Defaults to true.
     */
    void turnOffSDcard(bool waitForHousekeeping = true);

    /**
     * @brief Set a digital pin number for the slave select (chip select) of the
     * SD card and activate it as an output pin.
     *
     * This over-writes the value (if any) given in the constructor.  The pin
     * mode of this pin will be set as `OUTPUT`.
     *
     * Because this sets the pin mode, this function should only be called
     * during the `setup()` or `loop()` portion of an Arduino program.
     *
     * @param SDCardSSPin The pin on the mcu connected to the slave select of
     * the SD card.
     */
    void setSDCardSS(int8_t SDCardSSPin);

    /**
     * @brief Set both pin numbers related to the SD card and activate them as
     * output pins.
     *
     * These over-write the values (if any) given in the constructor.  The pin
     * mode of these pins will be set as `OUTPUT`.
     *
     * Because this sets the pin mode, this function should only be called
     * during the `setup()` or `loop()` portion of an Arduino program.
     *
     * @param SDCardSSPin The pin on the mcu connected to the slave select of
     * the SD card.
     * @param SDCardPowerPin A digital pin number on the mcu controlling power
     * to the SD card.
     */
    void setSDCardPins(int8_t SDCardSSPin, int8_t SDCardPowerPin);

    //
    /**
     * @brief Set digital pin number for the wake up pin used as an RTC
     * interrupt and activate it in the given pin mode.
     *
     * This over-writes the value (if any) given in the constructor.  Use a
     * value of -1 to prevent the board from attempting to sleep.  If using a
     * SAMD board with the internal RTC, the value of the pin is irrelevant as
     * long as it is positive.
     *
     * Because this sets the pin mode, this function should only be called
     * during the `setup()` or `loop()` portion of an Arduino program.
     *
     * @note This sets the pin mode but does NOT enable the interrupt! The
     * interrupt is enabled with Logger::enableLoggerInterrupts(), called
     * internally during Logger::begin().
     *
     * @param mcuWakePin The pin on the mcu to be used to wake the mcu from deep
     * sleep.
     * @param wakePinMode The pin mode to be used for wake up on the clock alert
     * pin.  Must be either `INPUT` OR `INPUT_PULLUP`.  Optional with a default
     * value of `INPUT_PULLUP`.  The DS3231 has an active low interrupt, so the
     * pull-up resistors should be enabled.
     */
    void setRTCWakePin(int8_t mcuWakePin, uint8_t wakePinMode = INPUT_PULLUP);

    /**
     * @brief Set the digital pin number to put out an alert that a measurement
     * is being logged and activate it as an output pin.
     *
     * The pin mode of this pin will be set as `OUTPUT`.  This is intended to be
     * a pin with a LED on it so you can see the light come on when a
     * measurement is being taken.
     *
     * Because this sets the pin mode, this function should only be called
     * during the `setup()` or `loop()` portion of an Arduino program.
     *
     * @param ledPin The pin on the mcu to be held `HIGH` while sensor data is
     * being collected and logged.
     */
    void setAlertPin(int8_t ledPin);
    /**
     * @brief Set the alert pin high.
     */
    void alertOn();
    /**
     * @brief Set the alert pin low.
     */
    void alertOff();

    /**
     * @brief Set the digital pin number for an interrupt pin used to enter
     * testing mode, activate that pin as the given input type, **and** attach
     * the testing interrupt to it.
     *
     * Intended to be used for a pin attached to a button or other manual
     * interrupt source.
     *
     * Because this sets the pin mode, this function should only be called
     * during the `setup()` or `loop()` portion of an Arduino program.
     *
     * Once in testing mode, the logger will attempt to connect the the internet
     * and take 25 measurements spaced at 5 second intervals writing the results
     * to the main output destination (ie, Serial).  Testing mode cannot be
     * entered while the logger is taking a scheduled measureemnt.  No data is
     * written to the SD card in testing mode.
     *
     * @note This sets the pin mode but does NOT enable the interrupt! The
     * interrupt is enabled with Logger::enableLoggerInterrupts(), called
     * internally during Logger::begin().
     *
     * @param buttonPin The pin on the mcu to listen to for a value-change
     * interrupt.
     * @param buttonPinMode The pin mode to be used for the button pin.  Must be
     * either `INPUT` OR `INPUT_PULLUP`.  Optional with a default value of
     * `INPUT`.  Using `INPUT_PULLUP` will enable processor input resistors,
     * while using `INPUT` will explicitly disable them.  If your pin is
     * externally pulled down or the button is a normally open (NO) switch with
     * common (COM) connected to Vcc, like the EnviroDIY Mayfly), you should use
     * the `INPUT` pin mode.  Coversely, if your button connect to ground when
     * activated, you should enable the processor pull-up resistors using
     * `INPUT_PULLUP`.
     */
    void setTestingModePin(int8_t buttonPin, uint8_t buttonPinMode = INPUT);

    /**
     * @brief Set the digital pin numbers and activate pin modes for the five
     * pins of interest for the logger
     *
     * Because this sets the pin mode, this function should only be called
     * during the `setup()` or `loop()` portion of an Arduino program.
     *
     * @param mcuWakePin The pin on the mcu to listen to for a value-change
     * interrupt to wake from deep sleep.  The mode of this pin will be set to
     * `wakePinMode`.
     * @param SDCardSSPin The pin on the mcu connected to the slave select of
     * the SD card.  The pin mode of this pin will be set as `OUTPUT`.
     * @param SDCardPowerPin A digital pin number on the mcu controlling power
     * to the SD card.  The pin mode of this pin will be set as `OUTPUT`.
     * @param buttonPin The pin on the mcu to listen to for a value-change
     * interrupt to enter testing mode.  The mode of this pin will be set to
     * `buttonPinMode`.
     * @param ledPin The pin on the mcu to be held `HIGH` while sensor data is
     * being collected and logged.  The pin mode of this pin will be set as
     * `OUTPUT`.
     * @param wakePinMode The pin mode to be used for wake up on the
     * `mcuWakePin` (clock alert) pin.  Must be either `INPUT` OR
     * `INPUT_PULLUP`. Optional with a default value of `INPUT_PULLUP`.  The
     * DS3231 has an active low interrupt, so the pull-up resistors should be
     * enabled.
     * @param buttonPinMode The pin mode to be used for the button pin.  Must be
     * either `INPUT` OR `INPUT_PULLUP`.  Optional with a default value of
     * `INPUT`.  Using `INPUT_PULLUP` will enable processor input resistors,
     * while using `INPUT` will explicitly disable them.  If your pin is
     * externally pulled down or the button is a normally open (NO) switch with
     * common (COM) connected to Vcc, like the EnviroDIY Mayfly), you should use
     * the `INPUT` pin mode.  Coversely, if your button is active when connected
     * to ground, you should enable the processor pull-up resistors using
     * `INPUT_PULLUP`.
     */
    void setLoggerPins(int8_t mcuWakePin, int8_t SDCardSSPin,
                       int8_t SDCardPowerPin, int8_t buttonPin, int8_t ledPin,
                       uint8_t wakePinMode   = INPUT_PULLUP,
                       uint8_t buttonPinMode = INPUT);

 protected:
    // Initialization variables
    /**
     * @brief The logger id
     */
    const char* _loggerID = "MyLogger";
    /**
     * @brief The logging interval in minutes
     */
    uint16_t _loggingIntervalMinutes = 5;
    /**
     * @brief The initial number of samples to log at an interval of 1 minute
     * for fast field verification
     */
    uint8_t _initialShortIntervals = 5;
    /**
     * @brief Digital pin number on the mcu controlling the SD card slave
     * select.
     */
    int8_t _SDCardSSPin = -1;
    /**
     * @brief Digital pin number on the mcu controlling SD card power
     */
    int8_t _SDCardPowerPin = -1;
    /**
     * @brief Digital pin number on the mcu receiving interrupts to wake from
     * deep-sleep.
     */
    int8_t _mcuWakePin = -1;
    /**
     * @brief The pin mode used for wake up on the clock alert pin.
     *
     * Must be either `INPUT` OR `INPUT_PULLUP` with an AVR board.  On a SAM/D
     * board `INPUT_PULLDOWN` is also an option.  Optional with a default value
     * of `INPUT_PULLUP`.  The DS3231 has an active low interrupt, so the
     * pull-up resistors should be enabled.
     */
    uint8_t _wakePinMode = INPUT_PULLUP;
    /**
     * @brief Digital pin number on the mcu used to output an alert that the
     * logger is measuring.
     *
     * Expected to be connected to a LED.
     */
    int8_t _ledPin = -1;
    /**
     * @brief Digital pin number on the mcu receiving interrupts to enter
     * testing mode.
     *
     * Expected to be connected to a user button.
     */
    int8_t _buttonPin = -1;
    /**
     * @brief The pin mode used for interrupts on the testing (button) pin.
     *
     * Must be either `INPUT` OR `INPUT_PULLUP` with an AVR board.  On a SAM/D
     * board `INPUT_PULLDOWN` is also an option.  Optional with a default value
     * of `INPUT_PULLUP`.
     */
    uint8_t _buttonPinMode = INPUT_PULLUP;

    /**
     * @brief The sampling feature UUID
     */
    const char* _samplingFeatureUUID = nullptr;
    // ^^ Start with no feature UUID

#if !defined(MS_USE_RTC_ZERO)
    /**
     * @brief Attaches the RTC ISR to the wake pin.
     *
     * @warning The RTC ISR **CANNOT** be attached to the wake pin before the
     * RTC is begun! Initializing/resetting the RTC can trigger an interrupt on
     * the wake pin. That triggered interrupt will attempt to change the RTC's
     * interrupt flag before the RTC is fully initialized, causing a crash or
     * confused behavior.  This function is protected to protect users from
     * calling it in the wrong order.
     *
     * @note This function is excluded when using the built in RTC on the SAMD21
     * (ie `#define MS_USE_RTC_ZERO`) because there is no pin involved
     */
    void enableRTCPinISR();
#endif

    /**
     * @brief Attaches the testing ISR to the button pin.
     *
     * @warning The testing ISR shouldn't be attached before the sensors have
     * been begun.  Doing so may cause strange results.  This function is
     * protected to protect users from calling it in the wrong order.
     *
     */
    void enableTestingISR();
    /**@}*/

    // ===================================================================== //
    /**
     * @anchor logger_internal_array
     * @name Attached Variable Array Functions
     * Public functions to get information about the attached variable array
     */
    /**@{*/
    // ===================================================================== //

 public:
    /**
     * @brief Set the variable array object.
     *
     * @param inputArray A pointer to a variable array object instance.  This is
     * NOT an array of variables, but an object of the variable array class.
     */
    void setVariableArray(VariableArray* inputArray);

    /**
     * @brief Get the number of variables in the internal variable array object.
     *
     * @return The number of variables in the internal variable
     * array object
     */
    uint8_t getArrayVarCount();

    /**
     * @brief Get the name of the parent sensor of the variable at the given
     * position in the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return The name of the parent sensor of that variable, if
     * applicable.
     */
    String getParentSensorNameAtI(uint8_t position_i);
    /**
     * @brief Get the name and pin location of the parent sensor of the variable
     * at the given position in the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return The concatenated name and pin location of the parent
     * sensor of that variable, if applicable.
     */
    String getParentSensorNameAndLocationAtI(uint8_t position_i);
    /**
     * @brief Get the name of the variable at the given position in the
     * internal variable array object.
     *
     * Variable names must follow the controlled vocabulary documented here:
     * http://vocabulary.odm2.org/variablename/
     *
     * @param position_i The position of the variable in the array.
     * @return The variable name
     */
    String getVarNameAtI(uint8_t position_i);
    /**
     * @brief Get the unit of the variable at the given position in the
     * internal variable array object.
     *
     * Variable units must follow the  controlled vocabulary documented here:
     * http://vocabulary.odm2.org/units/
     *
     * @param position_i The position of the variable in the array.
     * @return The variable unit
     */
    String getVarUnitAtI(uint8_t position_i);
    /**
     * @brief Get the customized code of the variable at the given position in
     * the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return The variable code
     */
    String getVarCodeAtI(uint8_t position_i);
    /**
     * @brief Get the UUID of the variable at the given position in the internal
     * variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return The variable UUID
     */
    String getVarUUIDAtI(uint8_t position_i);
    /**
     * @brief Get the most recent value of the variable at the given position in
     * the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return The value of the variable as a float.
     */
    float getValueAtI(uint8_t position_i);
    /**
     * @brief Get the most recent value of the variable at the given position in
     * the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return The value of the variable as a string with the correct
     * number of significant figures.
     */
    String getValueStringAtI(uint8_t position_i);
    /**
     * @brief Get the string representing a particular value of the variable at
     * the given position in the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @param value The value to format.
     * @return The given value as a string with the correct number of
     *  significant figures.
     */
    String formatValueStringAtI(uint8_t position_i, float value);

 protected:
    /**
     * @brief A pointer to the internal variable array instance
     */
    VariableArray* _internalArray;
    /**@}*/

    // ===================================================================== //
    /**
     * @anchor logger_internet
     * @name Internet and Publisher Functions
     * Public functions for internet and dataPublishers
     */
    /**@{*/
    // ===================================================================== //

 public:
    /**
     * @brief Attach a loggerModem to the logger to provide internet access.
     *
     * See [Modem and Internet Functions](@ref the_modems)
     * for more information on how the modem must be set up before it is
     * attached to the logger.  You must include an ampersand to tie in the
     * already created modem!  If you do not attach a modem, no action will be
     * taken to publish data.
     *
     * @param modem An instance of the loggerModem class
     */
    void attachModem(loggerModem& modem);
    /**
     * @brief Use the attached loggerModem to synchronize the real-time clock
     * with NIST time servers.
     *
     * @return True if clock synchronization was successful
     */
    bool syncRTC();

    /**
     * @brief Register a data publisher object to receive data from the logger.
     *
     * @param publisher A dataPublisher object
     */
    void registerDataPublisher(dataPublisher* publisher);
    /**
     * @brief Publish data to all registered data publishers.
     */
    void publishDataToRemotes(void);
    /**
     * @brief Retained for backwards compatibility, use publishDataToRemotes()
     * in new code.
     *
     * @m_deprecated_since{0,22,5}
     */
    void sendDataToRemotes(void);

 protected:
    /**
     * @brief The internal modem instance
     *
     * @note The internal #_logModem must be a POINTER not a reference because
     * it is possible for no modem to be attached (and thus the pointer could be
     * null).  It is not possible to have a null reference.
     */
    loggerModem* _logModem = nullptr;
    // ^^ Start with no modem attached

    /**
     * @brief An array of all of the attached data publishers
     */
    dataPublisher* dataPublishers[MAX_NUMBER_SENDERS];
    /**@}*/

    // ===================================================================== //
    /**
     * @anchor logger_time
     * @name Clock and Timezones
     * Public functions to access the clock in proper format and time zone
     */
    /**@{*/
    // ===================================================================== //

 public:
    /**
     * @brief Set the static timezone that the data will be logged in.
     *
     * @note This must be set.
     *
     * @param timeZone The timezone data shold be saved to the SD card in.  This
     * need not be the same as the timezone of the real time clock.
     */
    static void setLoggerTimeZone(int8_t timeZone);
    /**
     * @brief Get the Logger Time Zone.
     *
     * @return The timezone data is be saved to the SD card in.  This
     * is not be the same as the timezone of the real time clock.
     */
    static int8_t getLoggerTimeZone(void);

    /**
     * @brief A passthrough to loggerClock::setRTCOffset(int8_t offsetHours);
     * set the static timezone that the RTC is programmed in.
     *
     * @m_deprecated_since{0,37,0}
     *
     * Use loggerClock::setRTCOffset(_offsetHours) in new code!
     *
     * @note I VERY, VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
     *
     * @param timeZone The offset of the real-time clock (RTC) from UTC in hours
     */
    static void setRTCTimeZone(int8_t timeZone);
    /**
     * @brief A passthrough to loggerClock::getRTCOffset(); get the timezone of
     * the real-time clock (RTC).
     *
     * @m_deprecated_since{0,37,0}
     *
     * Use loggerClock::getRTCOffset() in new code!
     *
     * @return The offset of the real-time clock (RTC) from UTC in hours
     */
    static int8_t getRTCTimeZone(void);
    /**
     * @brief Set the offset between the built-in clock and the time zone
     * where the data is being recorded.
     *
     * If your RTC is set in UTC and your logging timezone is EST, this should
     * be -5.  If your RTC is set in EST and your timezone is EST this does not
     * need to be called.
     *
     * @param offset The difference between the timezone of the RTC and the
     * saved data
     */
    static void setTZOffset(int8_t offset);
    /**
     * @brief Get the offset between the built-in clock and the time zone
     * where the data is being recorded.
     *
     * @return The offset between the built-in clock and the time
     * zone where the data is being recorded.
     */
    static int8_t getTZOffset(void);

    /**
     * @brief Get the current epoch time from the RTC and correct it to the
     * logging time zone.
     *
     * @return The number of seconds from the start of the given epoch in
     * the logging time zone.
     */
    static uint32_t getNowLocalEpoch();

    /**
     * @brief Get the current Universal Coordinated Time (UTC) epoch time from
     * the RTC.
     *
     * @return The number of seconds from the start of the given epoch.
     */
    static uint32_t getNowUTCEpoch();

    /**
     * @brief Convert an epoch time into a ISO8601 formatted string.
     *
     * This assumes the supplied date/time is in the LOGGER's timezone and the
     * LOGGER's epoch start. It adds the LOGGER's offset as the time zone offset
     * in the string.
     *
     * @param epochTime The number of seconds since the start of the logger's
     * epoch (#MS_LOGGER_EPOCH).
     * @return An ISO8601 formatted String.
     */
    static String formatDateTime_ISO8601(uint32_t epochTime);

    /**
     * @brief Convert an epoch time into a character string based on the input
     * strftime format string and put it into the given buffer.
     *
     * This assumes the supplied date/time is in the LOGGER's timezone and adds
     * the LOGGER's offset as the time zone offset in the string.
     *
     * @see https://en.cppreference.com/w/cpp/chrono/c/strftime for possible
     * formatting strings.
     *
     * @note This function DOES NOTE SUPPORT TIMEZONES. Do not use the %z or %Z
     * inputs!
     *
     * @param buffer A buffer to put the finished string into. Make sure that
     * the buffer is big enough to hold all of the characters!
     * @param fmt The strftime format string.
     * @param epochTime The number of seconds since the start of the given
     * epoch in the given offset from UTC.
     */
    static void formatDateTime(char* buffer, const char* fmt,
                               uint32_t epochTime);

    /**
     * @brief Pass-through to loggerClock::setRTClock(uint32_t
     * UTCEpochSeconds,0, epochStart::unix_epoch) Veify that the input value is
     * sane and if so set the real time clock to the given time.
     *
     * @m_deprecated_since{0,37,0}
     *
     * Call loggerClock::setRTClock(uint32_t ts, int8_t utcOffset , epochStart
     * epoch) directly in new programs.
     *
     * @param UTCEpochSeconds The number of seconds since 1970 in UTC.
     * @return True if the input timestamp passes sanity checks **and**
     * the clock has been successfully set.
     */
    bool setRTClock(uint32_t UTCEpochSeconds);

    /**
     * @brief Passthrough to loggerClock::isRTCSane(); check that the current
     * time on the RTC is within a "sane" range.
     *
     * @m_deprecated_since{0,37,0}
     *
     * Use loggerClock::isRTCSane() directly in new code!
     *
     * To be sane the clock must be between #EARLIEST_SANE_UNIX_TIMESTAMP and
     * #LATEST_SANE_UNIX_TIMESTAMP.
     *
     * @return True if the current time on the RTC passes sanity range
     * checking
     */
    static bool isRTCSane(void);

    /**
     * @brief Set static variables for the date/time
     *
     * This is needed so that all data outputs (SD, EnviroDIY, serial printing,
     * etc) print the same time for updating the sensors - even though the
     * routines to update the sensors and to output the data may take several
     * seconds.  It is not currently possible to output the instantaneous time
     * an individual sensor was updated, just a single marked time.  By custom,
     * this should be called before updating the sensors, not after.
     */
    static void markTime(void);

    /**
     * @brief Check if the CURRENT time is an even interval of the logging rate
     *
     * @return True if the current time on the RTC is an even interval
     * of the logging rate.
     */
    bool checkInterval(void);

    /**
     * @brief Check if the MARKED time is an even interval of the logging rate -
     * That is the value saved in the static variable markedLocalUnixTime.
     *
     * This should be used in conjunction with markTime() to ensure that all
     * data outputs from a single data update session (SD, EnviroDIY, serial
     * printing, etc) have the same timestamp even though the update routine may
     * take several (or many) seconds.
     *
     * @return True if the marked time is an even interval of the
     * logging rate.
     */
    bool checkMarkedInterval(void);

 protected:
    /**
     * @brief The static timezone data is being logged in.
     *
     * @note All logger objects, if multiple are used, will be in the same
     * timezone.
     */
    static int8_t _loggerUTCOffset;
    /**
     * @brief The start of the epoch for the logger.
     *
     * @note This is fixed as #MS_LOGGER_EPOCH
     */
    static epochStart _loggerEpoch;
    /**
     * @brief The static difference between the timezone of the RTC and the
     * timezone data is being logged in.
     *
     * @note All logger objects, if multiple are used, will be will have the
     * same offset.
     */
    static int8_t _loggerRTCOffset;

    // ===================================================================== //
    /**
     * @anchor logger_sleep
     * @name Sleep and Power Saving
     * Public Functions for sleeping the logger
     *
     * # AVR Sleep modes
     *
     * In the avr/sleep.h file, the call names of these 5 sleep modes are:
     * SLEEP_MODE_IDLE         - the least power savings
     * SLEEP_MODE_ADC
     * SLEEP_MODE_PWR_SAVE
     * SLEEP_MODE_STANDBY
     * SLEEP_MODE_PWR_DOWN     - the most power savings
     *
     * # SAMD21 Sleep Modes
     *
     * > The SAM D21/DA1 have two software-selectable sleep modes, Idle and
     * > Stand-by.
     * > In Idle mode, the CPU is stopped while all other functions can be kept
     * > running.
     * > In Stand-by mode, all clocks and functions are stopped, expect those
     * > selected to continue running.
     * > The device supports SleepWalking.
     * > This feature allows the peripheral to wake up from sleep based on
     * > predefined conditions, and thus allows the CPU to wake up only when
     * > needed, e.g., when a threshold is crossed or a result is ready.
     * > The Event System supports synchronous and asynchronous events, allowing
     * > peripherals to receive, react to and send events even in Stand-by mode.
     *
     * # SAMD51 Sleep Modes
     *
     * > The device can be set in a sleep mode. In sleep mode, the CPU is
     * > stopped and the peripherals are either active or idle, according to the
     * > sleep mode depth:
     * >
     * >  - Idle sleep mode:
     * >    - The CPU is stopped.
     * >    - Synchronous clocks are stopped except when requested.
     * >    - The logic is retained.
     * >  - Standby sleep mode:
     * >    - The CPU is stopped as well as the peripherals.
     * >    - The logic is retained, and power domain gating can be used to
     * > fully or partially turn off the PDSYSRAM power domain.
     * >  - Hibernate sleep mode:
     * >    - PDCORESW power domain is turned OFF.
     * >    - The backup power domain is kept powered to allow few features to
     * > run (RTC, 32KHz clock sources, and wake-up from external pins).
     * >    - The PDSYSRAM power domain can be retained according to software
     * > configuration.
     * >  - Backup sleep mode:
     * >    - Only the backup domain is kept powered to allow few features to
     * > run (RTC, 32KHz clock sources, and wake-up from external pins).
     * >    - The PDBKUPRAM power domain can be retained according to software
     * > configuration.
     * >  - Off sleep mode:
     * >    - The entire device is powered off.
     *
     * ## Bit Settings
     * | Value |    Name   |                 Definition                 |
     * |:-----:|:---------:|:------------------------------------------:|
     * | 0x0   | Reserved  | -                                          |
     * | 0x1   | Reserved  | -                                          |
     * | 0x2   | IDLE      | CPU, AHBx, and APBx clocks are OFF         |
     * | 0x3   | Reserved  | Reserved                                   |
     * | 0x4   | STANDBY   | All Clocks are OFF                         |
     * | 0x5   | HIBERNATE | Backup domain is ON as well as some PDRAMs |
     * | 0x6   | BACKUP    | Only Backup domain is powered ON           |
     * | 0x7   | OFF       | All power domains are powered OFF          |
     *
     * @note For the SAMD51, hibernate, backup, and off modes cause a full
     * system reset on wake. Because we don't want to fully reset the device
     * (and go back to the setup) on wake, the lowest power mode we can use
     * is standby.
     */
    /**@{*/
    // ===================================================================== //

 public:
    /**
     * @brief Set up the Interrupt Service Request for waking
     *
     * In this case, we're doing nothing, we just want the processor to wake.
     * This must be a static function (which means it can only call other static
     * functions.)
     *
     * @m_deprecated_since{0,37,0}
     *
     * Use loggerClock::RTCISR() in new code!
     */
    static void wakeISR(void);

    /**
     * @brief Put the mcu to sleep to conserve battery life and handle
     * post-interrupt wake actions
     *
     * @note This DOES NOT sleep or wake the sensors!!
     *
     * @warning During sleep, the I2C/Wire interface is disabled and the SCL and
     * SDA pins are forced low to save power. Any attempt to communicate with an
     * I2C device during sleep (ie, thorough an interrupt) will cause the board
     * to hang indefinitely.
     */
    void systemSleep(void);
    /**@}*/

    // ===================================================================== //
    /**
     * @anchor logger_sdcard
     * @name SD Cards and Saving Data
     * Public functions for logging data to an SD card
     */
    /**@{*/
    // ===================================================================== //

 public:
    /**
     * @brief Set the file name, if you want to decide on it in advance.
     *
     * If the file name is set using this function, the same file name will be
     * used for every single file created by the logger.
     *
     * @param fileName The file name
     */
    void setFileName(const char* fileName);
    /**
     * @brief Set the file name, if you want to decide on it in advance.
     *
     * If the file name is set using this function, the same file name will be
     * used for every single file created by the logger.
     *
     * @param fileName  The file name
     */
    void setFileName(String& fileName);

    /**
     * @brief Get the current filename.
     *
     * This may be a single filename set using the setFileName(name) function or
     * an auto-generated filename which is a concatenation of the logger id and
     * the date when the file was started.
     *
     * @return The name of the file data is currently being saved to.
     */
    String getFileName(void) {
        return _fileName;
    }

    /**
     * @brief Print a header out to a stream.
     *
     * This removes need to pass around very long string objects which can crash
     * the logger
     *
     * @param stream An Arduino stream instance - expected to be an SdFat file -
     * but could also be the "main" Serial port for debugging.
     */
    virtual void printFileHeader(Stream* stream);

    /**
     * @brief Print a comma separated list of volues of sensor data -
     * including the time in the logging timezone -  out over an Arduino stream
     *
     * @param stream An Arduino stream instance - expected to be an SdFat file -
     * but could also be the "main" Serial port for debugging.
     */
    void printSensorDataCSV(Stream* stream);

    /**
     * @brief Check if the SD card is available and ready to write to.
     *
     * We run this check before every communication with the SD card to prevent
     * hanging.
     *
     * @return True if the SD card is ready
     */
    bool initializeSDCard(void);

    /**
     * @brief Create a file on the SD card and set the created, modified, and
     * accessed timestamps in that file.
     *
     * The filename will be the value specified in the function.  If desired, a
     * header will also be written to the file based on the variable information
     * from the variable array.  This can be used to force a logger to create a
     * file with a secondary file name.
     *
     * @param filename The name of the file to create
     * @param writeDefaultHeader True to write a header to the file, default is
     * false
     * @return True if the file was successfully created.
     */
    bool createLogFile(String& filename, bool writeDefaultHeader = false);
    /**
     * @brief Create a file on the SD card and set the created, modified, and
     * accessed timestamps in that file.
     *
     * The filename will be the one set by setFileName(String) or generated
     * using the logger id and the date.  If desired, a header will also be
     * written to the file based on the variable information from the variable
     * array.
     *
     * @param writeDefaultHeader True to write a header to the file, default is
     * false
     * @return True if the file was successfully created.
     */
    bool createLogFile(bool writeDefaultHeader = false);

    /**
     * @brief Open a file with the given name on the SD card and append the
     * given line to the bottom of it.
     *
     * If a file with the specified name does not already exist, attempt to
     * create the file and add a header to it.  Set the modified and accessed
     * timestamps of the file to the current time.
     *
     * @param filename The name of the file to write to
     * @param rec The line to be written to the file
     * @return True if the file was successfully accessed or created
     * _and_ data appended to it.
     */
    bool logToSD(String& filename, String& rec);
    /**
     * @brief Open a file named with the current internal filename value and
     * append the given line to the bottom of it.
     *
     * If a file with the with the intenal filename does not already exist,
     * attempt to create a file with that name and add a header to it.  Set the
     * modified and accessed timestamps of the file to the current time.
     *
     * @param rec The line to be written to the file
     * @return True if the file was successfully accessed or created
     * _and_ data appended to it.
     */
    bool logToSD(String& rec);
    /**
     * @brief Open a file named with the current internal filename value and
     * append a line to the bottom of it with the most recent values of all
     * variables in the variable array as a comma separated list.
     *
     * If a file with the with the intenal filename does not already exist,
     * attempt to create the file and add a header to it.  Set the modified and
     * accessed timestamps of the file to the current time.
     *
     * @return True if the file was successfully accessed or created
     * _and_ data appended to it.
     */
    bool logToSD(void);

 protected:
    // The SD card and file
    /**
     * @brief An internal reference to SdFat for SD card control
     */
    SdFat sd;
    /**
     * @brief An internal reference to an SdFat file instance
     */
    File logFile;
    /**
     * @brief An internal reference to the current filename
     */
    String _fileName = "";
    // ^^ Initialize with no file name

    /**
     * @brief Generate a file name from the logger id and the current date.
     *
     * @note This cannot be called until *after* the RTC is started
     */
    void generateAutoFileName(void);

    /**
     * @brief This function is used to automatically mark files as
     * created/accessed/modified when operations are done by the SdFat library.
     *
     * This function will be called automatically by SdFat, but is not intended
     * to be used at any other time.
     *
     * @param date Pointer to a uint16_t to store the date
     * @param time Pointer to a uint16_t to store the time
     */
    static void fileDateTimeCallback(uint16_t* date, uint16_t* time);

    /**
     * @brief Set a timestamp on a file.
     *
     * @param fileToStamp The filename to change the timestamp of
     * @param stampFlag The "flag" of the timestamp to change - should be
     * T_CREATE, T_WRITE, or T_ACCESS
     */
    void setFileTimestamp(File& fileToStamp, uint8_t stampFlag);

    /**
     * @brief Open or creates a file, converting a string file name to a
     * character file name.
     *
     * @param filename The name of the file to open
     * @param createFile True to create the file if it did not already exist
     * @param writeDefaultHeader True to add a header to the file if it is
     * created
     * @return True if a file was successfully opened or created.
     */
    bool openFile(String& filename, bool createFile, bool writeDefaultHeader);
    /**@}*/

    // ===================================================================== //
    /**
     * @anchor logger_testing
     * @name Sensor Testing Mode
     * Public functions for a "sensor testing" mode
     */
    /**@{*/
    // ===================================================================== //

 public:
    // This checks to see if you want to enter the sensor mode
    // This should be run as the very last step within the setup function
    // void checkForTestingMode(int8_t buttonPin);

    /**
     * @brief The interrupt sevice routine called when an iterrupt is detected
     * on the pin assigned for "testing" mode.
     */
    static void testingISR(void);

    /**
     * @brief Execute testing mode.
     *
     * In testing mode, the logger uses the loggerModem, if attached, to connect
     * to the internet.  It then powers up all sensors tied to variable in the
     * internal variable array.  The logger then updates readings from all
     * sensors 25 times with a 5 second wait in between.  All results are output
     * to the "main" output - ie Serial - and NOT to the SD card.  After 25
     * measurements, the sensors are put to sleep, the modem is disconnected
     * from the internet, and the logger goes back to sleep.
     *
     * @param sleepBeforeReturning True to put the logger to sleep before
     * returning from the function; optional with a default value of true.
     */
    virtual void testingMode(bool sleepBeforeReturning = true);
    /**@}*/

    // ===================================================================== //
    /**
     * @anchor logger_conv
     * @name Do-It-All Convience Functions
     * Convience functions to call several of the above functions
     */
    /**@{*/
    // ===================================================================== //

    /**
     * @brief Set all pin levels and does initial communication with the
     * real-time clock and SD card to prepare the logger for full functionality.
     *
     * This is used for operations that cannot happen in the logger constructor
     * - they must happen at run time, not at compile time.
     *
     * @param loggerID An ID for the logger - will be used to auto-generate file
     * names.  Supplying a logger ID here will override any value given in the
     * constructor.
     * @param loggingIntervalMinutes The interval in minutes at which to log
     * data.  Supplying an interval here will override any value given in the
     * constructor.
     * @param inputArray A variableArray object instance providing data to be
     * logged.  This is NOT an array of variables, but an object of the variable
     * array class.  Supplying a variableArray object here will override any
     * value given in the constructor.
     */
    virtual void begin(const char* loggerID, uint16_t loggingIntervalMinutes,
                       VariableArray* inputArray);
    /**
     * @brief Set all pin levels and does initial communication with the
     * real-time clock and SD card to prepare the logger for full functionality.
     *
     * This is used for operations that cannot happen in the logger constructor
     * - they must happen at run time, not at compile time.
     *
     * @param inputArray A variableArray object instance providing data to be
     * logged.  This is NOT an array of variables, but an object of the variable
     * array class.  Supplying a variableArray object here will override any
     * value given in the constructor.
     */
    virtual void begin(VariableArray* inputArray);
    /**
     * @brief Set all pin levels and does initial communication with the
     * real-time clock and SD card to prepare the logger for full functionality.
     *
     * This is used for operations that cannot happen in the logger constructor
     * - they must happen at run time, not at compile time.
     */
    virtual void begin();

    /**
     * @brief This is a one-and-done to log data
     *
     * @param sleepBeforeReturning True to put the logger to sleep before
     * returning from the function; optional with a default value of true.
     * @note If sleepBeforeReturning is set to false, the logger WILL NOT sleep
     * between readings.
     */
    virtual void logData(bool sleepBeforeReturning = true);

    /**
     * @brief This is a one-and-done to log data and publish the results to any
     * associated publishers.
     *
     * @param sleepBeforeReturning True to put the logger to sleep before
     * returning from the function; optional with a default value of true.
     * @note If sleepBeforeReturning is set to false, the logger WILL NOT sleep
     * between readings.
     */
    void logDataAndPublish(bool sleepBeforeReturning = true);

    /**
     * @brief The static "marked" epoch time for the local timezone.
     */
    static uint32_t markedLocalUnixTime;

    /**
     * @brief The static "marked" epoch time for UTC.
     */
    static uint32_t markedUTCUnixTime;

    // These are flag fariables noting the current state (logging/testing)
    // NOTE:  if the logger isn't currently logging or testing or in the middle
    // of set-up, it's probably sleeping
    // Setting these as volatile because the flags can be changed in ISR's
    /**
     * @brief Internal flag set to true when logger is currently updating
     * sensors or writing to the SD card
     */
    static volatile bool isLoggingNow;
    /**
     * @brief Internal flag set to true when the logger is going through the
     * "testing mode" routine.
     */
    static volatile bool isTestingNow;
    /**
     * @brief Internal flag set to true when the logger should begin the
     * "testing mode" routine when it finishes other operations.
     */
    static volatile bool startTesting;
    /**@}*/
};

#endif  // SRC_LOGGERBASE_H_
