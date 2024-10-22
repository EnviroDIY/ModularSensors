/**
 * @file ClockSupport.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the epochStart class, which is a helper to convert between
 * various time types used by different processors.
 */

// Header Guards
#ifndef SRC_CLOCKSUPPORT_H_
#define SRC_CLOCKSUPPORT_H_

#ifdef MS_CLOCKSUPPORT_DEBUG
#define MS_DEBUGGING_STD "ClockSupport"
#endif

#ifdef MS_CLOCKSUPPORT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "ClockSupport"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include <Arduino.h>
#include <time.h>


#ifndef EARLIEST_SANE_UNIX_TIMESTAMP
/**
 * @brief The earliest unix timestamp that can be considered sane.
 *
 * January 1, 2023
 */
#define EARLIEST_SANE_UNIX_TIMESTAMP 1672531200
#endif

#ifndef LATEST_SANE_UNIX_TIMESTAMP
/**
 * @brief The latest unix timestamp that can be considered sane.
 *
 * January 1, 2030
 */
#define LATEST_SANE_UNIX_TIMESTAMP 1893456000
#endif

#if defined(ENVIRODIY_STONEFLY_M4) && not defined(MS_USE_DS3231) && \
    not defined(MS_USE_RV8803)
/**
 * @brief Select RV-8803 as the RTC
 */
#define MS_USE_RV8803
#elif defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) && not defined(MS_USE_RV8803) && \
    not defined(MS_USE_DS3231)
/**
 * @brief Select DS3231 as the RTC
 */
#define MS_USE_DS3231
#elif (defined(ARDUINO_ARCH_SAMD) && !defined(__SAMD51__) ||    \
       defined(ARDUINO_SAMD_ZERO)) &&                           \
    not defined(MS_USE_DS3231) && not defined(MS_USE_RV8803) && \
    not defined(MS_USE_RTC_ZERO)
/**
 * @brief Select the SAMD21's internal clock (via RTC Zero)
 */
#define MS_USE_RTC_ZERO
#endif

#if !defined(MS_USE_RV8803) && !defined(MS_USE_DS3231) && \
    !defined(MS_USE_RTC_ZERO)
#error Define a clock to use for the RTC for Modular Sensors!
#endif

// Bring in the library to communicate with an external high-precision real time
// clock.
#if defined(MS_USE_RV8803)
#include <SparkFun_RV8803.h>
#elif defined(MS_USE_DS3231)
#include <Sodaq_DS3231.h>
#elif defined(MS_USE_RTC_ZERO)
#include <RTCZero.h>
#endif

// https://stackoverflow.com/questions/21295935/can-a-c-enum-class-have-methods


class epochStart {
 public:
    /**
     * @brief Set the epoch start value.
     *
     * @warning 8-bit AVR processors use a uint32_t for time_t. Given the start
     * of the first epoch here is 1900, this will roll over and cease to work
     * for processors with a uint32_t time_t on February 7, 2036 6:28:15 AM
     */
    enum unixOffset : time_t {
        unix_epoch = 2208988800,  ///< Use a Unix epoch, starting Jan 1, 1970.
                                  ///< This is the default for this library
        y2k_epoch = unix_epoch +
            946684800,  ///< Use an epoch starting Jan 1, 2000, as some RTC's
                        ///< and Arduinos do (946684800s ahead of UNIX epoch)
        gps_epoch = unix_epoch +
            315878400,  ///< Use the GPS epoch starting Jan 5, 1980 (315878400s
                        ///< ahead of UNIX epoch)
        nist_epoch = 0  ///< Use the epoch starting Jan 1, 1900 as returned by
                        ///< the NIST Network Time Protocol (RFC-1305 and later
                        ///< versions) and Time Protocol (RFC-868) (2208988800
                        ///< behind the UNIX epoch)
    };

    // https://stackoverflow.com/questions/21295935/can-a-c-enum-class-have-methods
    epochStart() = default;
    constexpr epochStart(unixOffset epoch) : _unixOffset(epoch) {}


    // Allow switch and comparisons.
    constexpr operator unixOffset() const {
        return _unixOffset;
    }

    // Prevent usage: if(epochStart)
    explicit operator bool() const = delete;

#ifdef MS_CLOCKSUPPORT_DEBUG
    // helper functions to convert between epoch starts
    /**
     * @brief Gets a string name for the epoch
     *
     * @param epoch The epoch to get the name of
     * @return The name for the epoch
     */
    String printEpochName();
    /**
     * @brief Gets a string for the start date of the epoch
     *
     * @param epoch The epoch to get the starting date of
     * @return The starting date, in ISO8601
     */
    String printEpochStart();
#endif

 private:
    unixOffset _unixOffset;
};


// ===================================================================== //
/**
 * @anchor logger_time
 * @name Clock and Timezones
 * Public functions to access the clock in proper format and time zone
 */
/**@{*/
// ===================================================================== //

class loggerClock {
 public:
    // Since there can only be one logger clock and all of it's methods are
    // static, disallow creating of this class.
    loggerClock() = delete;
    /**
     * @brief Set the static offset in hours from UTC that the RTC is programmed
     * in.
     *
     * @note I VERY, VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC(ie, offset =
     * 0)
     *
     * @param offsetHours The offset of the real-time clock (RTC) from UTC in
     * hours
     */
    static void setRTCOffset(int8_t offsetHours);
    /**
     * @brief Get the of the real-time clock (RTC) from UTC in hours.
     *
     * @return The offset of the real-time clock (RTC) from UTC in hours
     */
    static int8_t getRTCOffset(void);

#if defined(MS_USE_RV8803)
    /**
     * @brief The RTC object.
     *
     * @note Only one RTC may be used.  Either the built-in RTC of a SAMD board,
     * a DS3231, or a RV-8803.
     */
    static RV8803 rtc;
#elif defined(MS_USE_RTC_ZERO)
    /**
     * @brief The RTC object.
     *
     * @note Only one RTC may be used.  Either the built-in RTC of a SAMD board,
     * a DS3231, or a RV-8803.
     */
    static RTCZero zero_sleep_rtc;
#endif

    /**
     * @brief Get the current Universal Coordinated Time (UTC) epoch time from
     * the RTC.
     *
     * @param returnUTCOffset The offset from UTC to return the epoch time in.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     *
     * @return The number of seconds from the start of the given epoch.
     */
    static uint32_t getNowAsEpoch(int8_t returnUTCOffset, epochStart epoch);

    /**
     * @brief Convert an epoch time into a ISO8601 formatted string.
     *
     * This assumes the supplied date/time is in the LOGGER's timezone and adds
     * the LOGGER's offset as the time zone offset in the string.
     *
     * @param epochTime The number of seconds since the start of the given
     * epoch in the given offset from UTC.
     * @param epochTimeUTCOffset The offset of the input epoch time from UTC in
     * hours.
     * @param epoch The epoch of the input epoch time.
     * @return An ISO8601 formatted String.
     */
    static String formatDateTime_ISO8601(uint32_t   epochTime,
                                         int8_t     epochTimeUTCOffset,
                                         epochStart epoch);

    /**
     * @brief Veify that the input value is sane and if so sets the real time
     * clock to the given time.
     *
     * @param ts The number of seconds since the start of the given epoch.
     * @param utcOffset The offset of the epoch time from UTC.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     *
     * @return True if the input timestamp passes sanity checks **and**
     * the clock has been successfully set.
     *
     * @note There is no timezone correction in this function
     */
    static bool setRTClock(uint32_t ts, int8_t utcOffset, epochStart epoch);

    /**
     * @brief Check that the current time on the RTC is within a "sane" range.
     *
     * To be sane the clock must be between #EARLIEST_SANE_UNIX_TIMESTAMP and
     * #LATEST_SANE_UNIX_TIMESTAMP.
     *
     * @return True if the current time on the RTC passes sanity range
     * checking
     */
    static bool isRTCSane(void);
    /**
     * @brief Check that a given epoch time (seconds since 1970) is within a
     * "sane" range.
     *
     * To be sane, the clock must be between #EARLIEST_SANE_UNIX_TIMESTAMP and
     * #LATEST_SANE_UNIX_TIMESTAMP.
     *
     * @param ts The epoch time to be checked.
     * @param utcOffset The offset of the epoch time from UTC in hours. Optional
     * with a default value of 0.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch). Optional with a default value of epochStart::unix_epoch.
     * @return True if the given time passes sanity range checking.
     */
    static bool isEpochTimeSane(uint32_t ts, int8_t utcOffset,
                                epochStart epoch);

    /**
     * @brief Enable 1 minute interrupts on the RTC
     */
    static void enableRTCInterrupts();
    /**
     * @brief Disable interrupts on the RTC
     */
    static void disableRTCInterrupts();

    /**
     * @brief Start up the real-time clock.
     *
     * This is used for operations that cannot happen in the constructor - they
     * must happen at run time, not at compile time.
     */
    static void begin();

    /**
     * @brief Figure out where the epoch starts for the processor.
     *
     * The real time clock libraries mostly document this, but the cores for the
     * various Arduino processors don't. The time.h file is not much more than a
     * stub.
     *
     * @return The start of the epoch
     */
    static epochStart getProcessorEpochStart();

    /**
     * @brief The start of the epoch for the processor's internal time.h
     * library.
     */
    static epochStart _core_epoch;
 protected:
    /**
     * @brief The static offset data of the real time clock from UTC in hours
     */
    static int8_t _rtcUTCOffset;
    /**
     * @brief The start of the epoch for the RTC (or the RTC's library).
     */
    static epochStart _rtcEpoch;
    /**
     * @brief Get a raw epoch time from the RTC
     */
    static uint32_t getRawRTCNow();
    /**
     * @brief Sets the RTC to exactly the epoch time provided
     */
    static void setRawRTCNow(uint32_t ts);
};

#endif
