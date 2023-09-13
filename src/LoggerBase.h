/**
 * @file LoggerBase.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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

// Debugging Statement
// #define MS_LOGGERBASE_DEBUG

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
#include "ms_common.h"
#include <Arduino.h>  // The base Arduino library

// Bring in the libraries to handle the processor sleep/standby modes
// The SAMD library can also the built-in clock on those modules
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAMD_ZERO)
#include <RTCZero.h>
#include "WatchDogs/WatchDogSAMD.h"
#elif defined(ARDUINO_ARCH_AVR) || defined(__AVR__)
#include <avr/power.h>
#include <avr/sleep.h>
#include "WatchDogs/WatchDogAVR.h"
#endif

// Bring in the library to communicate with an external high-precision real time
// clock This also implements a needed date/time class
#include <Sodaq_DS3231.h>
// FUT using namespace sodaq_DS3231_nm;
// This is a fudge for handling time between DS3231 and SAMD
#define EPOCH_TIME_DTCLASS EPOCH_TIME_OFF

/**
 * @brief January 1, 2000 00:00:00 in "epoch" time
 *
 * Need this b/c the date/time class in Sodaq_DS3231 treats a 32-bit long
 * timestamp as time from 2000-jan-01 00:00:00 instead of the standard (unix)
 * epoch beginning 1970-jan-01 00:00:00.
 */
#define EPOCH_TIME_OFF 946684800

#include <SdFat.h>  // To communicate with the SD card

typedef enum {
    LB_PWR_USEABLE_REQ = 0,
    LB_PWR_SENSOR_USE_REQ,
    LB_PWR_MODEM_USE_REQ,
    LB_PWR_END
} lb_pwr_req_t;
typedef bool (*bat_handler_atl)(lb_pwr_req_t reqBatState);


/**
 * @brief The largest number of data publishers possible
 */
#define MAX_NUMBER_SENDERS 4


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
     * @param inputArray A variableArray object instance providing data to be
     * logged.  This is NOT an array of variables, but an object of the variable
     * array class.
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
     * @return **const char\*** A pointer to the logger ID
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
     * @return **uint16_t** The logging interval in minutes
     */
    uint16_t getLoggingInterval() {
        return _loggingIntervalMinutes;
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
     * @return **const char\*** The sampling feature UUID
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
     * @note  This sets the pin mode but does NOT enable the interrupt!
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
     * @brief The sampling feature UUID
     */
    const char* _samplingFeatureUUID = nullptr;
    // ^^ Start with no feature UUID
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
     * @param inputArray A variable array object instance.  This is NOT an array
     * of variables, but an object of the variable array class.
     */
    void setVariableArray(VariableArray* inputArray);

    /**
     * @brief Get the number of variables in the internal variable array object.
     *
     * @return **uint8_t** The number of variables in the internal variable
     * array object
     */
    uint8_t getArrayVarCount();

    /**
     * @brief Get the name of the parent sensor of the variable at the given
     * position in the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return **String** The name of the parent sensor of that variable, if
     * applicable.
     */
    String getParentSensorNameAtI(uint8_t position_i);
    /**
     * @brief Get the name and pin location of the parent sensor of the variable
     * at the given position in the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return **String** The concatenated name and pin location of the parent
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
     * @return **String** The variable name
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
     * @return **String** The variable unit
     */
    String getVarUnitAtI(uint8_t position_i);
    /**
     * @brief Get the customized code of the variable at the given position in
     * the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return **String** The variable code
     */
    String getVarCodeAtI(uint8_t position_i);
    /**
     * @brief Get the UUID of the variable at the given position in the internal
     * variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return **String** The variable UUID
     */
    String getVarUUIDAtI(uint8_t position_i);
    /**
     * @brief Get the most recent value of the variable at the given position in
     * the internal variable array object.
     *
     * @param position_i The position of the variable in the array.
     * @return **String** The value of the variable as a string with the correct
     * number of significant figures.
     */
    String getValueStringAtI(uint8_t position_i);

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
     * @brief Use the attahed loggerModem to synchronize the real-time clock
     * with NIST time servers.
     *
     * @return **bool** True if clock synchronization was successful
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
     * @return **int8_t** The timezone data is be saved to the SD card in.  This
     * is not be the same as the timezone of the real time clock.
     */
    static int8_t getLoggerTimeZone(void);
    /**
     * @brief Retained for backwards compatibility; use setLoggerTimeZone(int8_t
     * timeZone) in new code.
     *
     * @m_deprecated_since{0,22,4}
     *
     * @param timeZone The timezone data shold be saved to the SD card in.  This
     * need not be the same as the timezone of the real time clock.
     */
    static void setTimeZone(int8_t timeZone);
    /**
     * @brief Retained for backwards compatibility; use getLoggerTimeZone() in
     * new code.
     *
     * @m_deprecated_since{0,22,4}
     *
     * @return **int8_t** The timezone data is be saved to the SD card in.  This
     * is not be the same as the timezone of the real time clock.
     */
    static int8_t getTimeZone(void);

    /**
     * @brief Set the static timezone that the RTC is programmed in.
     *
     * @note I VERY, VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
     *
     * @param timeZone The timezone of the real-time clock (RTC)
     */
    static void setRTCTimeZone(int8_t timeZone);
    /**
     * @brief Get the timezone of the real-time clock (RTC).
     *
     * @return **int8_t** The timezone of the real-time clock (RTC)
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
     * @return **int8_t** The offset between the built-in clock and the time
     * zone where the data is being recorded.
     */
    static int8_t getTZOffset(void);

// This gets the current epoch time (unix time, ie, the number of seconds
// from January 1, 1970 00:00:00 UTC) and corrects it for the specified time
// zone
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAMD_ZERO)
    /**
     * @brief The RTC object.
     *
     * @note Only one RTC may be used.  Either the built-in RTC of a SAMD board
     * *OR* a DS3231
     */
    static RTCZero zero_sleep_rtc;
#endif

    /**
     * @brief Get the current epoch time from the RTC (unix time, ie, the
     * number of seconds from January 1, 1970 00:00:00) and correct it to the
     * logging time zone.
     *
     * @return **uint32_t**  The number of seconds from January 1, 1970 in the
     * logging time zone.
     *
     * @m_deprecated_since{0,33,0}
     */
    static uint32_t getNowEpoch(void);

    /**
     * @brief Get the current epoch time from the RTC (unix time, ie, the
     * number of seconds from January 1, 1970 00:00:00) and correct it to the
     * logging time zone.
     *
     * @return **uint32_t**  The number of seconds from January 1, 1970 in the
     * logging time zone.
     */
    static uint32_t getNowLocalEpoch(void);

    /**
     * @brief Get the current Universal Coordinated Time (UTC) epoch time from
     * the RTC (unix time, ie, the number of seconds from January 1, 1970
     * 00:00:00 UTC)
     *
     * @return **uint32_t**  The number of seconds from 1970-01-01T00:00:00Z0000
     */
    static uint32_t getNowUTCEpoch(void);
    /**
     * @brief Set the real time clock to the given number of seconds from
     * January 1, 1970.
     *
     * The validity of the timestamp is not checked in any way!  In practice,
     * setRTClock(ts) should be used to avoid setting the clock to an obviously
     * invalid value.  The input value should be *in the timezone of the RTC.*
     *
     * @param ts The number of seconds since 1970.
     */
    static void setNowUTCEpoch(uint32_t ts);

    /**
     * @brief Convert the number of seconds from January 1, 1970 to a DateTime
     * object instance.
     *
     * @param epochTime The number of seconds since 1970.
     * @return **DateTime** The equivalent DateTime
     */
    static DateTime dtFromEpoch(uint32_t epochTime);

    /**
     * @brief Convert a date-time object into a ISO8601 formatted string.
     *
     * This assumes the supplied date/time is in the LOGGER's timezone and adds
     * the LOGGER's offset as the time zone offset in the string.
     *
     * @param dt A DateTime object to convert
     * @return **String** An ISO8601 formatted String.
     */
    static String formatDateTime_ISO8601(DateTime& dt);

    /**
     * @brief Convert an epoch time (unix time) into a ISO8601 formatted string.
     *
     * This assumes the supplied date/time is in the LOGGER's timezone and adds
     * the LOGGER's offset as the time zone offset in the string.
     *
     * @param epochTime The number of seconds since 1970.
     * @return **String** An ISO8601 formatted String.
     */
    static String formatDateTime_ISO8601(uint32_t epochTime);

    /**
     * @brief Veify that the input value is sane and if so sets the real time
     * clock to the given time.
     *
     * @param UTCEpochSeconds The number of seconds since 1970 in UTC.
     * @return **bool** True if the input timestamp passes sanity checks **and**
     * the clock has been successfully set.
     */
    bool setRTClock(uint32_t UTCEpochSeconds);

    /**
     * @brief Check that the current time on the RTC is within a "sane" range.
     *
     * To be sane the clock  must be between 2020 and 2030.
     *
     * @return **bool** True if the current time on the RTC passes sanity range
     * checking
     */
    static bool isRTCSane(void);
    /**
     * @brief Check that a given epoch time (seconds since 1970) is within a
     * "sane" range.
     *
     * To be sane the clock  must be between 2020 and 2025.
     *
     * @param epochTime The epoch time to be checked.
     * @return **bool** True if the given time passes sanity range checking.
     */
    static bool isRTCSane(uint32_t epochTime);

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
     * @return **unit8_t** Bitmask settings for actions to take on the inteval
     */
    uint8_t checkInterval(void);
    /// @brief  Take no action on interval
    static const uint8_t CIA_NOACTION = 0x0;
    /// @brief Take a new set of sensor readings on the interval (0x01 = 0b0001)
    static const uint8_t CIA_NEW_READING = 0x01;
    /// @brief Post new readings to publishers on the interval (0x02 = 0b0010)
    static const uint8_t CIA_POST_READINGS = 0x02;
    /// @brief Store readings to a queue file on the interval (0x04 = 0b0100)
    static const uint8_t CIA_RLB_READINGS = 0x04;  // store readings, no pub"
    /// @brief Prevent the logger from sleeping on this interval (0x08 = 0b1000)
    static const uint8_t CIA_NO_SLEEP = 0x08;  //

    /**
     * @brief Check if the MARKED time is an even interval of the logging rate -
     * That is the value saved in the static variable markedLocalEpochTime.
     *
     * This should be used in conjunction with markTime() to ensure that all
     * data outputs from a single data update session (SD, EnviroDIY, serial
     * printing, etc) have the same timestamp even though the update routine may
     * take several (or many) seconds.
     *
     * @todo: Rewrite this function to align with changes to the check interval
     * function.
     *
     * @return **bool** True if the marked time is an even interval of the
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
    static int8_t _loggerTimeZone;
    /**
     * @brief The static difference between the timezone of the RTC and the
     * timezone data is being logged in.
     *
     * @note All logger objects, if multiple are used, will be will have the
     * same offset.
     */
    static int8_t _loggerRTCOffset;
    /**@}*/

    // ===================================================================== //
    /**
     * @anchor logger_sleep
     * @name Clock and Timezones
     * Public Functions for sleeping the logger
     */
    /**@{*/
    // ===================================================================== //

 public:
    /**
     * @brief Set up the Interrupt Service Request for waking
     *
     * In this case, we're doing nothing, we just want the processor to wake.
     * This must be a static function (which means it can only call other static
     * funcions.)
     */
    static void wakeISR(void);

    /**
     * @brief Put the mcu to sleep to conserve battery life and handle
     * post-interrupt wake actions
     *
     * @note This DOES NOT sleep or wake the sensors!!
     */
    void systemSleep(void);

#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAMD_ZERO)
    /**
     * @brief A watch-dog implementation to use to reboot the system in case of
     * lock-ups
     */
    extendedWatchDogSAMD watchDogTimer;
#else
    /**
     * @brief A watch-dog implementation to use to reboot the system in case of
     * lock-ups
     */
    extendedWatchDogAVR watchDogTimer;
#endif
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
     * @return **String** The name of the file data is currently being saved to.
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
     * @return **bool** True if the file was successfully created.
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
     * @return **bool** True if the file was successfully created.
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
     * @return **bool** True if the file was successfully accessed or created
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
     * @return **bool** True if the file was successfully accessed or created
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
     * @return **bool** True if the file was successfully accessed or created
     * _and_ data appended to it.
     */
    bool logToSD(void);

 protected:
    // The SD card and file
    /**
     * @brief An internal reference to SdFat for SD card control
     */
    SdFat sd1_card_fatfs;
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
     * @brief Check if the SD card is available and ready to write to.
     *
     * We run this check before every communication with the SD card to prevent
     * hanging.
     *
     * @return **bool** True if the SD card is ready
     */
    bool initializeSDCard(void);

    /**
     * @brief Generate a file name from the logger id and the current date.
     *
     * @note This cannot be called until *after* the RTC is started
     */
    void generateAutoFileName(void);

    /**
     * @brief Set a timestamp on a file.
     *
     * @param fileToStamp The filename to change the timestamp of
     * @param stampFlag The "flag" of the timestamp to change - should be
     * T_CREATE, T_WRITE, or T_ACCESS
     */
    void setFileTimestamp(File fileToStamp, uint8_t stampFlag,
                          bool localTime = false);

    /**
     * @brief Open or creates a file, converting a string file name to a
     * character file name.
     *
     * @param filename The name of the file to open
     * @param createFile True to create the file if it did not already exist
     * @param writeDefaultHeader True to add a header to the file if it is
     * created
     * @return **bool** True if a file was successfully opened or created.
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
     */
    virtual void testingMode();
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
     */
    virtual void logData(void);

    /**
     * @brief This is a one-and-done to log data and publish the results to any
     * associated publishers.
     */
    void logDataAndPublish(void);

    /**
     * @brief The static "marked" epoch time for the local timezone.
     */
    static uint32_t markedLocalEpochTime;

    /**
     * @brief The static "marked" epoch time for UTC.
     */
    static uint32_t markedUTCEpochTime;

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
     * @brief Internal flag set to true with then logger should begin the
     * "testing mode" routine when it finishes other operations.
     */
    static volatile bool startTesting;
    /**@}*/

 protected:
    /**
     * @brief Active instance of the attached data publishers
     */
    uint8_t _dataPubInstance;

 public:
    /**
     * @brief A pointer to a battery handler object with the required voltages
     * for different actions and a function for determining the current voltage.
     */
    bat_handler_atl _bat_handler_atl = nullptr;

    /**
     * @brief Sets the battery handler function.
     */
    void setBatHandler(bool (*bat_handler_atl)(lb_pwr_req_t reqBatState));


#if !defined SERIALIZE_POST_MAX_READINGS
/**
 * @brief The default maximum number of readings to upload after a successful
 * connection to a data publishing endpoint.
 */
#define SERIALIZE_POST_MAX_READINGS 20
#endif  // SERIALIZE_POST_MAX_READINGS
    /**
     * @brief The maximum number of readings to upload after a
     * successful connection to a data publishing endpoint.
     */
    uint16_t _sendAtOneTimeMaxX_num = SERIALIZE_POST_MAX_READINGS;
    /**
     * @brief Sets the interval for how frequently to post collected data to the
     * internet and the maximum number of queued readings to send at each post
     * attempt.
     */
    void setSendEveryX(
        uint8_t  sendEveryX_num,
        uint16_t sendAtOneTimeMaxX_num = SERIALIZE_POST_MAX_READINGS) {
        _sendEveryX_num = sendEveryX_num;
        // Check range, if too small set to max
        if (sendAtOneTimeMaxX_num > 3) {
            _sendAtOneTimeMaxX_num = sendAtOneTimeMaxX_num;
        } else {
            _sendAtOneTimeMaxX_num = -1;  // Max
        }
    }
    /**
     * @brief Gets the number of readings to collect before publishing data.
     *
     * @return **uint8_t** The current publishing interval
     */
    uint8_t getSendEveryX(void) {
        return _sendEveryX_num;
    }
#if !defined SERIALIZE_SENDEVERYX_NUM
/**
 * @brief The default number of readings to collect before publishing data.
 */
#define SERIALIZE_SENDEVERYX_NUM 2
#endif  // SERIALIZE_SENDEVERYX_NUM
    /**
     * @brief The number of readings to collect and queue before publishing
     * data.  Range typically 0-20.  Defaults to #SERIALIZE_SENDEVERYX_NUM
     */
    uint8_t _sendEveryX_num = SERIALIZE_SENDEVERYX_NUM;
    /**
     * @brief A counter for the number of readings that have been taken since
     * data was last published.
     */
    int8_t _sendEveryX_cnt = 0;

    /**
     * @brief Sets the offset between when samples are collected and published.
     *
     * @param sendOffset_minutes The number of minutes to offset the data
     * publishing from the data collection.
     */
    void setSendOffset(uint8_t sendOffset_minutes) {
        // TODO: Might also have check for SampleTime * _sendEveryX_num <
        // sendOffset_minutes
        if (_sendOffset_MAX >= sendOffset_minutes) {
            _sendOffset_minutes = sendOffset_minutes;
        } else {
            _sendOffset_minutes = _sendOffset_MAX;
        }
    }
    /**
     * @brief Get the offset (in minutes) between when samples are collected and
     * published.
     *
     * @return *uint8_t* The offset (in minutes) between when samples are
     * collected and published
     */
    uint8_t getSendOffset(void) {
        return _sendOffset_minutes;
    }
#if !defined SERIALIZE_SENDOFFSET_MINUTES
/**
 * @brief The default offset (in minutes) between when samples are collected and
 * published.
 */
#define SERIALIZE_SENDOFFSET_MINUTES 0
#endif  // SERIALIZE_SENDOFFSET_MINUTES
    /**
     * @brief The offset (in minutes) between when samples are collected and
     * published.
     */
    uint8_t _sendOffset_minutes = SERIALIZE_SENDOFFSET_MINUTES;
    /**
     * @brief The maximum allowed offset (in minutes) between when samples are
     * collected and published.
     */
    const uint8_t _sendOffset_MAX = 14;  // Max allowed
    /**
     * @brief Whether a send offset is active or not; that is, whether or not
     * the current reading should be held for offset publishing or published
     * when it is taken.
     */
    bool _sendOffset_act = false;
    /**
     * @brief A counter for delayed data publishing.
     */
    uint8_t _sendOffset_cnt = 0;

    uint16_t setSendQueueSz_num(uint16_t sqz_num) {
        MS_DBG(F("setSendQueueSz_num"), sqz_num);
        return _sendQueueSz_num = sqz_num;
    }

    uint16_t getSendQueueSz_num() {
        MS_DBG(F("getSendQueueSz_num"), _sendQueueSz_num);
        return _sendQueueSz_num;
    }
#if !defined LB_SENDQUESZ_NUM_DEF
#define LB_SENDQUESZ_NUM_DEF 2800L  // MMWGI_SEND_QUE_SZ_NUM_DEF
#endif                              // LB_SENDQUESZ_NUM_DEF
    uint16_t _sendQueueSz_num =
        LB_SENDQUESZ_NUM_DEF;  // See MMMWGI_SEND_QUE_SZ_NUM_DEF

    uint16_t setPostMax_num(uint16_t mp_num) {
        MS_DBG(F("setMaxPost_num"), mp_num);
        return _postMax_num = mp_num;
    }

    uint16_t getPostMax_num() {
        MS_DBG(F("getPostMax_num"), _postMax_num);
        return _postMax_num;
    }
#if !defined LB_POSTMAX_NUM_DEF
#define LB_POSTMAX_NUM_DEF 96L  // MMWGI_POST_MAX_RECS_MUM_DEF
#endif                          // LB_POSTMAX_NUM_DEF
    uint16_t _postMax_num =
        LB_POSTMAX_NUM_DEF;  // See MMWGI_POST_MAX_RECS_MUM_DEF

    // Time woken up
    uint32_t wakeUpTime_secs;


 public:
    /**
     * @brief Forces the logger to restart by delaying until the watchdog kicks
     * in.
     */
    void forceSysReset(uint8_t source, uint16_t simpleCheck);
    /**
     * @brief This is a one-and-done to log data, queue it to the read-delay
     * file, and publish the data from the read-delayed and queued files to any
     * associated publishers when the countdown is reached.
     *
     * If previously registered, it will determine if battery power is available
     * It uses an algorithim to reliably deliver the readings.
     */
    void logDataAndPubReliably(uint8_t cia_val_override = 0);


/**
 * @brief The maximum number of read-delayed records to attempt to post before
 * forwarding the data directly to the failed queue.
 */
#define RDELAY_FAILED_POSTS_THRESHOLD 7

    /**
     * @brief Process queued readings to send to remote if internet available.
     *
     * @param internetPresent  true if an internet connection is present.
     *   For false store the readings for later transmission
     *   This reads from stored files
     *   1) RDELAY.txt
     *   2) QUE0.txt
     *   For data in RDELAY.txt, an attempt is made to transmit each line,
     *   if not sucessful then it is stored at the end of QUE0.txt
     *   Once RDELAY.txt is complete,
     *   any readings in QUE0.txt are attempted,
     *
     * The format of the readings in the file are dependent on sensor
     * configuration, so they may not be compatible if the builds sensor
     * configuration changes.
     *
     *   The forwarding of RDELAY.txt to QUE0.txt is only up to specific
     * thresholds POST_MAX_NUM RDELAY_FAILED_POSTS_THRESHOLD
     */
    void publishDataQueuedToRemotes(bool internetPresent);

 private:
    /**
     * @brief Check HTTP status response
     *
     * @param rspCode True if server is deemed to have received message
     * or nothing further can be done with message.
     */
    bool publishRspCodeAccepted(int16_t rspCode);

    // ===================================================================== //
    /* Serializing/Deserialing
      A common set of functions that operate on files
      serzRdelFn_str
      serzQueuedFn
    */
    // ===================================================================== //


 public:
    /**
     * @brief Sets the pointer to start of the next field to be deserialized
     * from the character string created from the last line to have been
     * read from an SD-card file.
     *
     * @return **bool** True if a new pointer could be set.
     */
    bool deszqNextCh(void);
    /**
     * @brief The epoch time deserialized from the character string created from
     * the last line to have been read from an SD-card file.
     */
    uint32_t deszq_epochTime = 0;  // Marked Epoch Time
    /**
     * @brief A pointer to the start of the next field to be deserialized.
     *
     * This pointer is set by deszqNextCh().
     */
    char* deszq_nextChar;
    /**
     * @brief The size of the next field to be deserialized
     */
    uint16_t deszq_nextCharSz;

    /**
     * @brief Calculated total length of all of the time variant data fields as
     * ASCII+ delimiter, except for last data field
     */
    uint16_t deszq_timeVariant_sz;

 private:
#define sd1_Err(s) sd1_card_fatfs.errorPrint(F(s))
    /**
     * @brief The bit wise status of reading deserialized from the character
     * string created from the last line to have been read from an SD-card file.
     */
    uint16_t deszq_status = 0;
    /**
     * @brief The total number of lines published from both the read-delay and
     * queue file.
     */
    uint16_t deszLinesRead = 0;
    /**
     * @brief The maximum length of one line of queued data
     */
#define QUEFILE_MAX_LINE 255
    /**
     * @brief A character buffer holding the current line being deserialized
     * (read) from the queue file.
     */
    char     deszq_line[QUEFILE_MAX_LINE] = "";
    uint16_t desz_pending_records         = 0;

    // Qu SdFat/sd1_card_fatfs connects to Physical pins or File/logFile or
    // keep to LFN - capitals  https://en.wikipedia.org/wiki/8.3_filename

#if defined MS_LOGGERBASE_POSTS
    File        postsLogHndl;            // Record all POSTS when enabled
    const char* postsLogFn_str = "DBG";  // Not more than 8.3 total

#endif  // MS_LOGGERBASE_POSTS

    // que Readings DELAYed (RDEL) ~ serialize/deserialize
    File        serzRdelFile;
    const char* serzRdelFn_str = "RDELAY.TXT";

    // QUEueD for reliable delivery
    // first POST didn't suceed to serialize/deserialize
    // Potentially multiple versions of files based on dataPublisher[]
    File serzQueuedFile;
#define FN_BUFFER_SZ 13
    char        serzQueuedFn[FN_BUFFER_SZ] = "";
    const char* serzQueuedFn_str           = "QUE";  // begin of name, keep 8.3


    // perform a serialize to RdelFile
    bool serzRdel_Line(void);
    // Uses serzRdelFn_str, File serzRdelFile
    bool deszRdelStart();

    /**
     * @brief Finds the location of fixed delimeter within a string data line;
     * behave as strchrnul() if goes past end of string (that is, returns a
     * pointer to the null at the end of the string)
     *
     * @param in_line The deserialized string (data line) to search for the
     * delimeter.
     * @param caller_id For debug printing only; a character representing the
     * function calling the find.
     *
     * @return **char\*** A pointer to the location of the delimeter in the
     * string.
     */
    char* deszFind(const char* in_line, char caller_id);
#define deszRdelLine() deszLine(&serzRdelFile)
    bool deszRdelClose(bool deleteFile = false);

    /**
     * @brief Creates and opens a queue file for future sending of data.
     *
     * The filename will be a concatenation of the Logger::serzQueuedFn_str and
     * the uniqueId.  The filename is stored in Logger::serzQueuedFn.  Uses the
     * file object Logger::serzQueuedFile.
     *
     * @param uniqueId A unique letter to append to the end of the queue file
     * name (Logger::serzQueuedFn).
     *
     * @return **bool** True if the file was successfully opened or created
     */
    bool serzQueuedStart(char uniqueId);  // Use 1st, & sets filename
    /**
     * @brief Opens the file named Logger::serzQueuedFn and reads the first line
     * from it.
     *
     * Logger::serzQueuedFn is set in Logger::serzQueuedStart(char uniqueId).
     *
     * @return **bool** True if the file was successfully opened
     */
    bool deszQueuedStart(void);
#define deszQueuedLine() deszLine(&serzQueuedFile)
    uint16_t serzQueuedFlushFile();
    bool     serzQueuedCloseFile(bool action);
    /*
    bool deszQueuedCleanup(bool debug = false);
    */
    // This does the work
    bool deszLine(File* filep);

    // Utility resources
    // void setFileTimeStampMet(File fileToStamp, uint8_t stampFlag);
    bool deszDbg(void);
    bool postLogOpen();
    bool postLogOpen(const char* postsLogNm_str);
    void postLogLine(uint32_t tmr_ms, int16_t rspParam);
    void postLogLine(const char* logMsg, bool addCR = true);
// Macro to print to TTY and log on uSD
#define PRINT_LOGLINE_P(msg_parm)      \
    char tttbuf[sizeof(msg_parm) + 1]; \
    strcpy_P(tttbuf, msg_parm);        \
    PRINTOUT(tttbuf);                  \
    postLogLine(tttbuf);
    void postLogClose();
    bool listFile(File* filep, char* fn_str, char* uid);

 public:
    // A simple data-time formatting, compatible with reading into excel
    // spreadsheet - see also formatDateTime_ISO8601()
    String formatDateTime_str(DateTime& dt);
    String formatDateTime_str(uint32_t epochTime);
    bool   serzBegin(void);
    /**@}*/
};

#endif  // SRC_LOGGERBASE_H_
