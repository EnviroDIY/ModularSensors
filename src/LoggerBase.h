/**
 * @file LoggerBase.h
 * @copyright 2020 Stroud Water Research Center
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

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableArray.h"
#include "LoggerModem.h"
#include "ms_common.h"
#include <Arduino.h> // The base Arduino library

// Bring in the libraries to handle the processor sleep/standby modes
// The SAMD library can also the built-in clock on those modules
#if defined(ARDUINO_ARCH_SAMD)
#include <RTCZero.h>
#include "WatchDogs/WatchDogSAMD.h"
#elif defined(ARDUINO_ARCH_AVR) || defined(__AVR__)
#include <avr/power.h>
#include <avr/sleep.h>
#include "WatchDogs/WatchDogAVR.h"
#endif

// Bring in the library to communicate with an external high-precision real time
// clock This also implements a needed date/time class
#if defined(ARDUINO_ARCH_SAMD)
#include <RTClib.h> //was <Sodaq_DS3231.h>
#elif defined(ARDUINO_ARCH_AVR) || defined(__AVR__)
#include <Sodaq_DS3231.h>
#endif
#define HOURS_TO_SECS 3600
// SECONDS_FROM_1970_TO_2000 946684800

/**
 * @brief January 1, 2000 00:00:00 in "epoch" time
 *
 * Need this b/c the date/time class in Sodaq_DS3231 treats a 32-bit long
 * timestamp as time from 2000-jan-01 00:00:00 instead of the standard (unix)
 * epoch beginning 1970-jan-01 00:00:00.
 */
#define EPOCH_TIME_OFF 946684800
// This is 2000-jan-01 00:00:00 in "epoch" time
// Need this b/c the date/time class in Sodaq_DS3231 treats a 32-bit long
// timestamp as time from 2000-jan-01 00:00:00 instead of the standard (unix)
// epoch beginning 1970-jan-01 00:00:00.
#define EPOCH_TIME_20200101_SECS 1577836800
#define EPOCH_TIME_20250101_SECS 1735689600

#include <SdFat.h>  // To communicate with the SD card
#if defined BOARD_SDQ_QSPI_FLASH
#include "Adafruit_SPIFlash.h" //This can be on the Adafruit Express options
#endif                         // BOARD_SDQ_QSPI_FLASH
#if defined USE_TINYUSB
#include "Adafruit_TinyUSB.h"
#endif // USE_TINYUSB
typedef int (*ini_handler_atl485)(const char *section, const char *name,
                                  const char *value);
// The largest number of variables from a single sensor

/**
 * @brief The largest number of variables from a single sensor
 */
#define MAX_NUMBER_SENDERS 4


class dataPublisher; // Forward declaration


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
    // Public functions to get and set basic logging paramters
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
     * @param samplingFeatureUUID a pointer to the sampling feature UUID
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
     * @brief Set the pin (on the mcu) to use to control power to the SD card.
     *
     * @note This functionality is not tested!
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
     * @brief Set a pin for the slave select (chip select) of the SD card.
     *
     * This over-writes the value (if any) given in the constructor.
     *
     * @param SDCardSSPin The pin on the mcu connected to the slave select of
     * the SD card.
     */
    void setSDCardSS(int8_t SDCardSSPin);

    /**
     * @brief Set both pins related to the SD card.
     *
     * These over-write the values (if any) given in the constructor.
     *
     * @param SDCardSSPin The pin on the mcu connected to the slave select of
     * the SD card.
     * @param SDCardPowerPin A digital pin number on the mcu controlling power
     * to the SD card.
     */
    void setSDCardPins(int8_t SDCardSSPin, int8_t SDCardPowerPin);

    //
    /**
     * @brief Set up the wake up pin for an RTC interrupt.
     *
     * This over-writes the value (if any) given in the constructor.  Use a
     * value of -1 to prevent the board from attempting to sleep.  If using a
     * SAMD board with the internal RTC, the value of the pin is irrelevant as
     * long as it is positive.
     *
     * @param mcuWakePin The pin on the mcu to be used to wake the mcu from deep
     * sleep.
     */
    void setRTCWakePin(int8_t mcuWakePin);

    /**
     * @brief Set a pin to put out an alert that a measurement is being logged.
     *
     * This is intended to be a pin with a LED on it so you can see the light
     * come on when a measurement is being taken.
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
     * @brief Set up a pin for an interrupt to enter testing mode.
     *
     * Intended to be attached to a button or other manual interrupt source.
     *
     * Once in testing mode, the logger will attempt to connect the the internet
     * and take 25 measurements spaced at 5 second intervals writing the results
     * to the main output destination (ie, Serial).  Testing mode cannot be
     * entered while the logger is taking a scheduled measureemnt.  No data is
     * written to the SD card in testing mode.
     *
     * @param buttonPin The pin on the mcu to listen to for a value-change
     * interrupt.
     */
    void setTestingModePin(int8_t buttonPin);

    /**
     * @brief Set the five pins of interest for the logger
     *
     * @param mcuWakePin The pin on the mcu to listen to for a value-change
     * interrupt to wake from deep sleep.
     * @param SDCardSSPin The pin on the mcu connected to the slave select of
     * the SD card.
     * @param SDCardPowerPin A digital pin number on the mcu controlling power
     * to the SD card.
     * @param buttonPin The pin on the mcu to listen to for a value-change
     * interrupt to enter testing mode.
     * @param ledPin The pin on the mcu to be held `HIGH` while sensor data is
     * being collected and logged.
     */
    void setLoggerPins(int8_t mcuWakePin, int8_t SDCardSSPin,
                       int8_t SDCardPowerPin, int8_t buttonPin, int8_t ledPin);

 protected:
    // Initialization variables
    /**
     * @brief The logger id
     */
    const char* _loggerID;
    /**
     * @brief The logging interval in minutes
     */
    uint16_t _loggingIntervalMinutes;
    /**
     * @brief Digital pin number on the mcu controlling the SD card slave
     * select.
     */
    int8_t _SDCardSSPin;
    /**
     * @brief Digital pin number on the mcu controlling SD card power
     */
    int8_t _SDCardPowerPin;
    /**
     * @brief Digital pin number on the mcu receiving interrupts to wake from
     * deep-sleep.
     */
    int8_t _mcuWakePin;
    /**
     * @brief Digital pin number on the mcu used to output an alert that the
     * logger is measuring.
     *
     * Expected to be connected to a LED.
     */
    int8_t _ledPin;
    /**
     * @brief Digital pin number on the mcu receiving interrupts to enter
     * testing mode.
     *
     * Expected to be connected to a user button.
     */
    int8_t _buttonPin;

    /**
     * @brief The sampling feature UUID
     */
    const char* _samplingFeatureUUID;

    // ===================================================================== //
    // Public functions to get information about the attached variable array
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

  // ===================================================================== //
  // Public functions for internet and dataPublishers
  // ===================================================================== //

 public:
    /**
     * @brief Attach a loggerModem to the logger to provide internet access.
     *
     * See [Modem and Internet
     * Functions](https://github.com/EnviroDIY/ModularSensors/wiki/Modem-Functions)
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
     * @return **true** Indicates clock synchronization was successful
     * @return **false** Clock was not successfully synchronized
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
     * @brief Retained for backwards compatibility.
     *
     * @deprecated use publishDataToRemotes()
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
    loggerModem* _logModem;
    //

    /**
     * @brief An array of all of the attached data publishers
     */
    dataPublisher* dataPublishers[MAX_NUMBER_SENDERS];

    // ===================================================================== //
    // Public functions to access the clock in proper format and time zone
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
     * @brief Retained for backwards compatibility.
     *
     * @deprecated use setLoggerTimeZone(int8_t timeZone)
     *
     * @param timeZone The timezone data shold be saved to the SD card in.  This
     * need not be the same as the timezone of the real time clock.
     */
    static void setTimeZone(int8_t timeZone);
    /**
     * @brief Retained for backwards compatibility.
     *
     * @deprecated use getLoggerTimeZone()
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
#if defined(ARDUINO_ARCH_SAMD)
    /**
     * @brief The RTC object.
     *
     * @note Only one RTC may be used.  Either the built-in RTC of a SAMD board
     * *OR* a DS3231
     */
    static RTCZero zero_sleep_rtc;
#endif

  static uint32_t getNowEpochT0(void);    // Get Epoch standard UST
  static uint32_t getNowEpochTz(void);    // Get Epoch with time zone offset
  static void setNowEpochT0(uint32_t ts); // Set Epoch with standard UST
  // static void setNowEpochTz(uint32_t ts); //Set Epoch with standard UST
    /**
     * @brief Get the current epoch time from the RTC (unix time, ie, the
     * number of seconds from January 1, 1970 00:00:00) and correct it to the
     * logging time zone.
     *
     * @return **uint32_t**  The number of seconds from January 1, 1970 in the
     * logging time zone.
     */
    static uint32_t getNowEpoch(void);
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
    static void setNowEpoch(uint32_t ts);

  static DateTime dtFromEpochT0(uint32_t epochTime);
  static DateTime dtFromEpochTz(uint32_t epochTime);

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
     * @return **true** The input timestamp passes sanity checks and the clock
     * has been set.
     * @return **false** Either the input timestamp fails sanity range checking
     * or the clock was not set successfully.
     */
    bool setRTClock(uint32_t UTCEpochSeconds);

    /**
     * @brief Check that the current time on the RTC is within a "sane" range.
     *
     * @return **true** Current time on the RTC passes sanity range checking
     * @return **false** Current time on the RTC is obvioiusly incorrect
     */
    static bool isRTCSane(void);
    /**
     * @brief Check that a given epoch time (seconds since 1970) is within a
     * "sane" range.
     *
     * @param epochTime The epoch time to be checked.
     * @return **true** The given time passes sanity range checking.
     * @return **false** The given time does not pass sanity range checking.
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
     * @return **true** The current time on the RTC is an even interval of the
     * logging rate.
     * @return **false** The current time on the RTC is NOT an even interval of
     * the logging rate.
     */
    bool checkInterval(void);

    /**
     * @brief Check if the MARKED time is an even interval of the logging rate -
     * That is the value saved in the static variable markedEpochTime.
     *
     * This should be used in conjunction with markTime() to ensure that all
     * data outputs from a single data update session (SD, EnviroDIY, serial
     * printing, etc) have the same timestamp even though the update routine may
     * take several (or many) seconds.
     *
     * @return **true** The marked time is an even interval of the
     * logging rate.
     * @return **false** The marked time is NOT an even interval of the
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

    // ============================================================================
    //  Public Functions for sleeping the logger
    // ============================================================================

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
    void systemSleep(uint8_t sleep_min = 0);

#if defined(ARDUINO_ARCH_SAMD)
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

    // ===================================================================== //
    // Public functions for logging data to an SD card
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
     * from the variable array. This can be used to force a logger to create a
     * file with a secondary file name.
     *
     * @param filename The name of the file to create
     * @param writeDefaultHeader True to write a header to the file, default is
     * false
     * @return **true** The file was successfully created.
     * @return **false** The file was NOT sucessfully created.
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
     * @return **true** The file was successfully created.
     * @return **false** The file was NOT sucessfully created.
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
     * @return **true** The file was successfully accessed or created and data
     * appended to it.
     * @return **false** The file could not be accessed or data could not be
     * written to it.
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
     * @return **true** The file was successfully accessed or created and data
     * appended to it.
     * @return **false** The file could not be accessed or data could not be
     * written to it.
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
     * @return **true** The file was successfully accessed or created and data
     * appended to it.
     * @return **false** The file could not be accessed or data could not be
     * written to it.
     */
    bool logToSD(void);

 protected:
    // The SD card and file
    /**
     * @brief An internal reference to SdFat for SD card control
     */
    SdFat sd1_card_fatfs;;
    /**
     * @brief An internal reference to an SdFat file instance
     */
    File logFile;
    /**
     * @brief An internal reference to the current filename
     */
    String _fileName;

    /**
     * @brief Check if the SD card is available and ready to write to.
     *
     * We run this check before every communication with the SD card to prevent
     * hanging.
     *
     * @return **true** The SD card is ready
     * @return **false** The SD card is not available to be written to
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
    void setFileTimestamp(File fileToStamp, uint8_t stampFlag);

    /**
     * @brief Open or creates a file, converting a string file name to a
     * character file name.
     *
     * @param filename The name of the file to open
     * @param createFile True to create the file if it did not already exist
     * @param writeDefaultHeader True to add a header to the file if it is
     * created
     * @return **true** If a file was successfully opened or created.
     * @return **false** If the file was NOT successfully opened or created.
     */
    bool openFile(String& filename, bool createFile, bool writeDefaultHeader);

  // ===================================================================== //
    // Public functions for a "sensor testing" mode
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

    // ===================================================================== //
    // Convience functions to call several of the above functions
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
     * array class. Supplying a variableArray object here will override any
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
     * array class. Supplying a variableArray object here will override any
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
     * @brief The static "marked" epoch time.
     */
    static uint32_t markedEpochTime;

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

#include "LoggerBaseHextClass.h"
};

#endif  // SRC_LOGGERBASE_H_
