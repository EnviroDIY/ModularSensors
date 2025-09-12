/**
 * @file ClockSupport.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the epochStart class, which is a helper to convert between
 * various time types used by different processors and the loggerClock class.
 */

// Header Guards
#ifndef SRC_CLOCKSUPPORT_H_
#define SRC_CLOCKSUPPORT_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_CLOCKSUPPORT_DEBUG
#define MS_DEBUGGING_STD "ClockSupport"
#endif
#ifdef MS_CLOCKSUPPORT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "ClockSupport"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include <Arduino.h>
#include <time.h>

// Where possible, use the board's built in clock
#if defined(ENVIRODIY_STONEFLY_M4) && not defined(MS_USE_RV8803)
/**
 * @brief Select RV-8803 as the RTC for the EnviroDIY Stonefly.
 */
#define MS_USE_RV8803
#undef MS_USE_DS3231
#undef MS_USE_RTC_ZERO
#elif defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) && not defined(MS_USE_DS3231)
/**
 * @brief Select DS3231 as the RTC for the EnviroDIY Mayfly.
 */
#define MS_USE_DS3231
#undef MS_USE_RV8803
#undef MS_USE_RTC_ZERO
#elif (defined(ARDUINO_ARCH_SAMD) && !defined(__SAMD51__)) && \
    !defined(MS_USE_DS3231) && !defined(MS_USE_RV8803) &&     \
    !defined(MS_USE_RTC_ZERO)
/**
 * @brief Select the SAMD21's internal clock (via RTC Zero) if no other RTC is
 * specified.
 */
#define MS_USE_RTC_ZERO
#undef MS_USE_RV8803
#undef MS_USE_DS3231
#endif

#if !defined(MS_USE_RV8803) && !defined(MS_USE_DS3231) && \
    !defined(MS_USE_RTC_ZERO)
#error Define a clock to use for the RTC for Modular Sensors!
#endif

/**
 * @def CLOCK_INTERRUPT_MODE
 * @brief The mode for the interrupt on the wake/RTC pin; must be one of HIGH,
 * LOW, FALLING, RISING, or CHANGE
 *
 * Using HIGH or LOW could trigger multiple interrupts when the clock interrupt
 * fires. It's best to catch the first edge of the clock interrupt. So for an
 * RTC with an active low interrupt, use "FALLING."
 *
 * @def MS_CLOCK_NAME
 * @brief A text description of the clock
 */
#if defined(MS_USE_RV8803)
#define MS_CLOCK_NAME "RV-8803"
#include <SparkFun_RV8803.h>
// Interrupt is active low on the RV8803
#define CLOCK_INTERRUPT_MODE FALLING
#elif defined(MS_USE_DS3231)
#define MS_CLOCK_NAME "DS3231"
#include <Sodaq_DS3231.h>
// Interrupt is active low on the DS3231
#define CLOCK_INTERRUPT_MODE FALLING
#elif defined(MS_USE_RTC_ZERO)
#define MS_CLOCK_NAME "SAMD 32-bit RTC"
#include <RTCZero.h>
#endif

#ifndef EPOCH_NIST_TO_UNIX
/**
 * @brief The difference in seconds between the epoch value returned by the NIST
 * Network Time Protocol and the Unix epoch
 *
 * The NIST Network Time Protocol (RFC-1305 and later versions) and Time
 * Protocol (RFC-868) both return a uint32_t which is the number of seconds from
 * January 1, 1900.  This is 2208988800 seconds behind the UNIX epoch.
 *
 * The NIST epoch will roll over and cease to work for processors with a
 * uint32_t time_t on February 7, 2036 6:28:15 AM.
 */
#define EPOCH_NIST_TO_UNIX 2208988800
#endif
#ifndef EPOCH_UNIX_TO_Y2K
/**
 * @brief The epoch starting Jan 1, 2000, as some RTC's and Arduinos do
 * (946684800s ahead of UNIX epoch)
 */
#define EPOCH_UNIX_TO_Y2K 946684800
#endif
#ifndef EPOCH_UNIX_TO_GPS
/**
 * @brief The GPS epoch starting Jan 5, 1980 (was 315964800s ahead of UNIX epoch
 * at founding, has drifted farther apart due to leap seconds)
 */
#define EPOCH_UNIX_TO_GPS 315964800
#endif
#ifndef NUMBER_LEAP_SECONDS
/**
 * @brief The number of announced leap seconds as of February 24, 2025
 */
#define NUMBER_LEAP_SECONDS 18
#endif
#ifndef LEAP_SECONDS
/**
 * @brief The GPS epoch equivalent for each of the announced leap seconds as of
 * February 24, 2025
 */
#define LEAP_SECONDS                                                     \
    {46828800,  78364801,  109900802, 173059203,  252028804,  315187205, \
     346723206, 393984007, 425520008, 457056009,  504489610,  551750411, \
     599184012, 820108813, 914803214, 1025136015, 1119744016, 1167264017}
#endif


/**
 * @brief Set the epoch start value.
 *
 * @warning 8-bit AVR processors use a uint32_t for time_t. Given the start
 * of the first epoch here is 1900, this will roll over and cease to work
 * for processors with a uint32_t time_t on February 7, 2036 6:28:15 AM
 *
 * @note: for AVR boards time_t is a typedef for uint32_t, defined in time.h For
 * SAMD time_t is a typedef for __int_least64_t _timeval.h implicit cast to
 * time_t
 */
enum class epochStart : time_t {
    unix_epoch =
        EPOCH_NIST_TO_UNIX,  ///< Use a Unix epoch, starting Jan 1, 1970.
                             ///< This is the default for this library
    y2k_epoch = EPOCH_NIST_TO_UNIX +
        EPOCH_UNIX_TO_Y2K,  ///< Use an epoch starting Jan 1, 2000, as some
                            ///< RTC's and Arduinos do (946684800s ahead of
                            ///< UNIX epoch)
    gps_epoch = EPOCH_NIST_TO_UNIX +
        EPOCH_UNIX_TO_GPS,  ///< Use the GPS epoch starting Jan 5, 1980
                            ///< (was 315964800s ahead of UNIX epoch at
                            ///< founding, has drifted farther apart due to
                            ///< leap seconds)
    nist_epoch = 0  ///< Use the epoch starting Jan 1, 1900 as returned by
                    ///< the NIST Network Time Protocol (RFC-1305 and later
                    ///< versions) and Time Protocol (RFC-868) (2208988800
                    ///< behind the UNIX epoch)
};


/**
 * @brief A class for dealing with different definitions of the start of the
 * epoch.
 *
 * The functions for converting between GSP and Unix epoch - taking leap
 * seconds into account - were converted from the php functions available at
 * https://www.andrews.edu/~tzs/timeconv/timealgorithm.html. From that site:
 *
 * > [W]hile there was an offset of 315964800 seconds between Unix and GPS
 * > time when GPS time began, that offset changes each time there is a leap
 * > second. GPS time labels each second uniquely including leap seconds while
 * > Unix time does not, preferring to count a constant number of seconds a
 * > day including those containing leap seconds.
 */
class epochTime {
 public:
    /**
     * @brief Constructor, requires thse unix offset value as input.
     *
     * @param timestamp A timestamp - in seconds since the start of the
     * given epoch.
     * @param epoch The start if the epoch for the timestamp.
     */
    epochTime(time_t timestamp, epochStart epoch = epochStart::unix_epoch);
    /**
     * @brief Deleted bool conversion operator
     *
     * The bool conversion operator allows an object of this class to be
     * converted into a boolean. Deleting this prevents anyone from calling
     * `if(epochStart)`
     *
     * @see
     * https://stackoverflow.com/questions/4600295/what-is-the-meaning-of-operator-bool-const
     */
    explicit operator bool() const = delete;

    /**
     * @brief Equality comparison operator
     *
     * @param a Another epochStart object
     * @return True if the objects have the same epoch start
     */
    bool operator==(epochTime a) const {
        return _unixTimestamp == a._unixTimestamp;
    }
    /**
     * @brief In-equality comparison operator
     *
     * @param a Another epochStart object
     * @return True if the objects have the a different epoch start
     */
    bool operator!=(epochTime a) const {
        return _unixTimestamp != a._unixTimestamp;
    }

    /**
     * @brief Static function to convert between any two timestamps
     * @param in_timestamp The timestamp to convert - in seconds since the start
     * of the input epoch.
     * @param in_epoch The starting epoch
     * @param out_epoch The ending epoch
     * @return The equivalent timestamp relative to the requested epoch
     */
    static time_t convert_epoch(time_t in_timestamp, epochStart in_epoch,
                                epochStart out_epoch);

    /**
     * @brief Get the timestamp of an epochTime object in a different epoch
     *
     * @param in_time The input epochTime object
     * @param out_epoch The ending epoch
     * @return The equivalent timestamp relative to the requested epoch
     */
    static time_t convert_epoch(epochTime in_time, epochStart out_epoch);

#ifndef DOXYGEN
    // Delete functions to avoid implicit conversions
    // https://stackoverflow.com/questions/12877546/how-do-i-avoid-implicit-conversions-on-non-constructing-functions
    template <class T>
    time_t convert_epoch(T, epochStart in_epoch,
                         epochStart out_epoch) = delete;  // C++11
    template <class T>
    time_t convert_epoch(T, epochStart out_epoch) = delete;  // C++11
#endif

    /**
     * @brief Convert Unix Time to GPS Time
     *
     * @param unixTime A unix epoch timestamp
     * @return The equivalent GPS epoch timestamp
     */
    static time_t unix2gps(time_t unixTime);
    /**
     * @brief Convert GPS Time to Unix Time
     *
     * @param gpsTime A GPS epoch timestamp
     * @return The equivalent Unix epoch timestamp
     */
    static time_t gps2unix(time_t gpsTime);

#if defined(MS_CLOCKSUPPORT_DEBUG) || defined(MS_CLOCKSUPPORT_DEBUG_DEEP)
    /**
     * @brief Gets a string name for the epoch
     *
     * @param epoch The epoch to get the name of
     * @return The name for the epoch
     */
    static String printEpochName(epochStart epoch);
    /**
     * @brief Gets a string for the start date of the epoch
     *
     * @return The starting date, in ISO8601
     */
    static String printEpochStart(epochStart epoch);
#endif

 private:
    /**
     * @brief Internal reference to the timestamp IN UNIX EPOCH
     */
    time_t _unixTimestamp;

 private:
    /**
     * @brief Array of leap seconds as of February 24, 2025
     */
    static const uint32_t leapSeconds[NUMBER_LEAP_SECONDS];

    /**
     * @brief Test to see if a GPS second is a leap second
     *
     * @param gpsTime A timestamp in the GPS epoch
     * @return True if the time is a leap second
     */
    static bool isLeap(uint32_t gpsTime);

    /**
     * @brief Count number of leap seconds that have passed between the start of
     * the GPS epoch and the given time
     *
     * @param gpsTime A timestamp in the GPS epoch
     * @param unix2gps True if the input time is in Unix epoch, false if it is
     * GPS epoch
     * @return The number of leap seconds that have passed between the start of
     * the GPS epoch and the given time
     */
    static int8_t countLeaps(uint32_t gpsTime, bool unix2gps);
};


/**
 * @brief A class for the clock attached to the logger.
 *
 * @note This is effectively a static class, with all static functions and a
 * deleted constructor.
 *
 * @todo Support half/quarter hour time zones
 */
class loggerClock {
 public:
    // Since there can only be one logger clock and all of it's methods are
    // static, disallow the creation of this class.
    loggerClock() = delete;

#if defined(MS_USE_RV8803)
    /**
     * @brief The RTC object.
     *
     * @note Only one RTC may be used.  Either the built-in RTC of a SAMD board,
     * a DS3231, or a RV-8803.
     */
    static RV8803 rtc;
#elif defined(MS_USE_DS3231)
// pass
// The Sodaq DS3231 library creates and externs the clock instance, so it's not
// needed here
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

    /**
     * @brief Get the current Universal Coordinated Time (UTC) epoch time from
     * the RTC.
     *
     * @param utcOffset The offset from UTC to return the epoch time in.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     *
     * @return The number of seconds from the start of the given epoch.
     */
    static uint32_t getNowAsEpoch(int8_t utcOffset, epochStart epoch);

    /**
     * @brief Get the current epoch time from the RTC and return it as
     * individual parts.
     *
     * @param seconds [out] Reference to a variable where the seconds will be
     * stored
     * @param minutes [out] Reference to a variable where the minutes will be
     * stored
     * @param hours [out] Reference to a variable where the hours will be stored
     * @param day [out] Reference to a variable where the day will be stored
     * @param month [out] Reference to a variable where the month will be stored
     * @param year [out] Reference to a variable where the year will be stored
     * @param tz_offset [in] The time zone offset from UTC in hours
     *
     * @note The tz_offset is an input parameter - you need to set the timezone
     * that you want the other parts to be set in!
     */
    static void getNowAsParts(int8_t& seconds, int8_t& minutes, int8_t& hours,
                              int8_t& day, int8_t& month, int16_t& year,
                              uint8_t tz_offset);

    /**
     * @brief Convert an epoch time (seconds since a fixed epoch start) into a
     * ISO8601 formatted string.
     *
     * Code modified from parts of the SparkFun RV-8803 library.
     *
     * @param epochSeconds The number of seconds since the start of the given
     * epoch.
     * @param epochSecondsUTCOffset The offset of the input epoch time from
     * UTC in hours.
     * @param epoch The epoch of the input epoch time.
     * @return An ISO8601 formatted String.
     */
    static String formatDateTime_ISO8601(uint32_t   epochSeconds,
                                         int8_t     epochSecondsUTCOffset,
                                         epochStart epoch);
    /**
     * @brief Convert an epochTime object into a ISO8601 formatted string.
     *
     * @param in_time An epochTime object
     * @param epochSecondsUTCOffset The offset of the input epoch time from
     * UTC in hours.
     * @return An ISO8601 formatted String.
     */
    static String formatDateTime_ISO8601(epochTime in_time,
                                         int8_t    epochSecondsUTCOffset);

    /**
     * @brief Convert an epoch time into a character string based on the input
     * strftime format string and put it into the given buffer.
     *
     * @note This function DOES NOT SUPPORT TIMEZONES. Do not use the %z or %Z
     * inputs!
     *
     * @see https://en.cppreference.com/w/cpp/chrono/c/strftime for possible
     * formatting strings.
     *
     * @param buffer A buffer to put the finished string into. Make sure that
     * the buffer is big enough to hold all of the characters!
     * @param fmt The strftime format string.
     * @param epochSeconds The number of seconds since the start of the given
     * epoch in the given offset from UTC.
     * @param epoch The epoch of the input epoch time.
     */
    static void formatDateTime(char* buffer, const char* fmt,
                               uint32_t epochSeconds, epochStart epoch);
    /**
     * @brief Convert an epoch time into a character string based on the input
     * strftime format string and put it into the given buffer.
     *
     * @note This function DOES NOT SUPPORT TIMEZONES. Do not use the %z or %Z
     * inputs!
     *
     * @see https://en.cppreference.com/w/cpp/chrono/c/strftime for possible
     * formatting strings.
     *
     * @param buffer A buffer to put the finished string into. Make sure that
     * the buffer is big enough to hold all of the characters!
     * @param fmt The strftime format string.
     * @param in_time An epochTime object
     */
    static void formatDateTime(char* buffer, const char* fmt,
                               epochTime in_time);

    /**
     * @brief Verify that the input value is sane and if so sets the real time
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
     * @brief Verify that the input value is sane and if so sets the real time
     * clock to the given time.
     *
     * @param in_time An epochTime object
     * @param utcOffset The offset of the epoch time from UTC.
     *
     * @return True if the input timestamp passes sanity checks **and**
     * the clock has been successfully set.
     *
     * @note There is no timezone correction in this function
     */
    static bool setRTClock(epochTime in_time, int8_t utcOffset);

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
     * @param ts The timestamp to check (in seconds since the start of the given
     * epoch).
     * @param utcOffset The offset of the epoch time from UTC in hours.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     * @return True if the given time passes sanity range checking.
     */
    static bool isEpochTimeSane(uint32_t ts, int8_t utcOffset,
                                epochStart epoch);
    /**
     * @brief Check that a given epoch time (an epochTime object) is within a
     * "sane" range.
     *
     * To be sane, the clock must be between #EARLIEST_SANE_UNIX_TIMESTAMP and
     * #LATEST_SANE_UNIX_TIMESTAMP.
     *
     * @param in_time An epochTime object
     * @param utcOffset The offset of the epoch time from UTC in hours.
     * @return True if the given time passes sanity range checking.
     */
    static bool isEpochTimeSane(epochTime in_time, int8_t utcOffset);

    /**
     * @brief Set an alarm to fire a clock interrupt at a specific epoch time
     *
     * @param ts The timestamp for the next interrupt - in seconds from the
     * start of the input epoch.
     * @param utcOffset The offset of the epoch time from UTC in hours.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     */
    static void setNextRTCInterrupt(uint32_t ts, int8_t utcOffset,
                                    epochStart epoch);
    /**
     * @brief Set an alarm to fire a clock interrupt at a specific epoch time
     *
     * @param in_time An epochTime object
     * @param utcOffset The offset of the epoch time from UTC in hours.
     */
    static void setNextRTCInterrupt(epochTime in_time, int8_t utcOffset);

    /**
     * @brief Enable 1 minute interrupts on the RTC
     *
     * Unfortunately, most RTC's do not seem to follow anything like a cron
     * schedule. Recurring/Periodic alarms can generally be only on single
     * seconds/minutes/hours/days not on custom intervals.
     */
    static void enablePeriodicRTCInterrupts();
    /**
     * @brief Disable interrupts on the RTC
     */
    static void disableRTCInterrupts();
    /**
     * @brief Reset the clock interrupt status.
     *
     * Some RTC's will not fire a new interrupt until the previous interrupt
     * flag has been cleared.
     *
     * @note Wire/I2C must be active for this function!
     *
     */
    static void resetClockInterruptStatus();
    /**
     * @brief An ISR to run when the clock interrupt fires
     *
     * For some clocks, we need to reset the clock's interrupt flag so the next
     * interrupt will fire.
     */
    static void rtcISR(void);

    /**
     * @brief Start up the real-time clock.
     *
     * This is used for operations that cannot happen in the constructor - they
     * must happen at run time, not at compile time.
     */
    static void begin();

    /**
     * @brief Get the epoch start for the processor/Arduino core as an
     * epochStart object
     *
     * @return The epoch start for the processor/Arduino core
     */
    static epochStart getCoreEpochStart() {
        return loggerClock::_core_epoch;
    };
    /**
     * @brief Get the epoch start for the RTC as an epochStart object
     *
     * @return The epoch start for the RTC
     */
    static epochStart getRTCEpochStart() {
        return _rtcEpoch;
    };

 protected:

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

    /**
     * @brief The static offset data of the real time clock from UTC in hours
     */
    static int8_t _rtcUTCOffset;
    /**
     * @brief The start of the epoch for the RTC (or the RTC's library).
     */
    static epochStart _rtcEpoch;

    /**
     * @brief Convert a timestamp with the given offset and epoch to the RTC
     * internal epoch and UTC offset.
     *
     * @param ts The input epoch time - in seconds since the start of the input
     * epoch.
     * @param utcOffset The offset of the input epoch time from UTC in hours.
     * @param epoch The type of epoch of the input timestamp
     * @return A timestamp converted to the epoch and timezone used internally
     * by the RTC
     */
    static inline uint32_t tsToRawRTC(uint32_t ts, int8_t utcOffset,
                                      epochStart epoch);
    /**
     * @brief Convert a timestamp from the RTC's internal epoch and UTC offset
     * to the requested offset and epoch.
     *
     * @param ts The timestamp in the epoch and timezone used internally by the
     * RTC.
     * @param utcOffset The offset of the desired output epoch time from UTC in
     * hours.
     * @param epoch The type of epoch of the output timestamp
     * @return A timestamp converted from the epoch and timezone used internally
     * by the RTC to the requested epoch and offset
     */
    static inline uint32_t tsFromRawRTC(uint32_t ts, int8_t utcOffset,
                                        epochStart epoch);

    /**
     * @brief Get a raw epoch time from the RTC
     *
     * @return The raw epoch time from the RTC
     */
    static uint32_t getRawRTCNow();
    /**
     * @brief Sets the RTC to exactly the epoch time provided
     *
     * @param ts A timestamp already in the epoch and timezone used internally
     * by the RTC
     */
    static void setRawRTCNow(uint32_t ts);
    /**
     * @brief Begins the underlying RTC
     */
    static void rtcBegin();
};

#endif

// cSpell:ignore nist hile
