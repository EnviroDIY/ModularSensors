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

#if defined(ENVIRODIY_STONEFLY_M4) && not defined(MS_USE_RV8803)
/**
 * @brief Select RV-8803 as the RTC
 */
#define MS_USE_RV8803
#undef MS_USE_DS3231
#undef MS_USE_RTC_ZERO
#elif defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) && not defined(MS_USE_DS3231)
/**
 * @brief Select DS3231 as the RTC
 */
#define MS_USE_DS3231
#undef MS_USE_RV8803
#undef MS_USE_RTC_ZERO
#elif (defined(ARDUINO_ARCH_SAMD) && !defined(__SAMD51__)) && \
    !defined(MS_USE_DS3231) && !defined(MS_USE_RV8803) &&     \
    !defined(MS_USE_RTC_ZERO)
/**
 * @brief Select the SAMD21's internal clock (via RTC Zero)
 */
#define MS_USE_RTC_ZERO
#undef MS_USE_RV8803
#undef MS_USE_DS3231
#endif

#if !defined(MS_USE_RV8803) && !defined(MS_USE_DS3231) && \
    !defined(MS_USE_RTC_ZERO)
#error Define a clock to use for the RTC for Modular Sensors!
#endif

// Bring in the library to communicate with an external high-precision real time
// clock.

/**
 * @def CLOCK_INTERRUPT_MODE
 * @brief The mode for the interrupt on the wake/RTC pin; must be one of HIGH,
 * LOW, FALLING, RISING, or CHANGE
 *
 * Using HIGH or LOW could trigger multiple interrupts when the clock interrut
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

// https://stackoverflow.com/questions/21295935/can-a-c-enum-class-have-methods


/**
 * @brief A class for dealing with different definitions of the start of the
 * epoch.
 */
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
    /**
     * @brief Constructor
     *
     * @param epoch The unix offfset from the unixOffset enum.
     */
    constexpr epochStart(unixOffset epoch) : _unixOffset(epoch) {}


    // Allow switch and comparisons.
    /**
     * @brief Operator for comparison
     *
     * @return The seconds from January 1, 1900
     */
    constexpr operator unixOffset() const {
        return _unixOffset;
    }

    /**
     * @brief Deleted bool conversion operator
     *
     * Deleting this prevents anyone from calling `if(epochStart)`
     *
     * @see
     * https://stackoverflow.com/questions/4600295/what-is-the-meaning-of-operator-bool-const
     */
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
     * @return The starting date, in ISO8601
     */
    String printEpochStart();
#endif

 private:
    /**
     * @brief Internal reference to the number of seconds from Jan 1, 1900.
     */
    unixOffset _unixOffset;
};


/**
 * @brief A class for the clock attached to the logger.
 *
 * @note This is effectively a static class, with all static functions and a
 * deleted constructor.
 */
class loggerClock {
 public:
    // Since there can only be one logger clock and all of it's methods are
    // static, disallow the creation of this class.
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
     * @param utcOffset The offset from UTC to return the epoch time in.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     *
     * @return The number of seconds from the start of the given epoch.
     */
    static uint32_t getNowAsEpoch(int8_t utcOffset, epochStart epoch);

    /**
     * @brief Convert an epoch time into a ISO8601 formatted string.
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
     * @brief Convert an epoch time into a character string based on the input
     * strftime format string and put it into the given buffer.
     *
     * @note This function DOES NOTE SUPPORT TIMEZONES. Do not use the %z or %Z
     * inputs!
     *
     * @see https://en.cppreference.com/w/cpp/chrono/c/strftime for possible
     * formatting strings.
     *
     * @param buffer A buffer to put the finished string into. Make sure that
     * the buffer is big enough to hold all of the characters!
     * @param fmt The strftime format string.
     * @param epochTime The number of seconds since the start of the given
     * epoch in the given offset from UTC.
     * @param epoch The epoch of the input epoch time.
     */
    static void formatDateTime(char* buffer, const char* fmt,
                               uint32_t epochTime, epochStart epoch);

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
     * @param utcOffset The offset of the epoch time from UTC in hours.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     * @return True if the given time passes sanity range checking.
     */
    static bool isEpochTimeSane(uint32_t ts, int8_t utcOffset,
                                epochStart epoch);

    /**
     * @brief Set an alarm to fire a clock inetrrupt at a specific epoch time
     *
     * @param ts The timestamp for the next interrupt
     * @param utcOffset The offset of the epoch time from UTC in hours.
     * @param epoch The type of epoch to use (ie, the standard for the start of
     * the epoch).
     */
    static void setNextRTCInterrupt(uint32_t ts, int8_t utcOffset,
                                    epochStart epoch);
    /**
     * @brief Enable 1 minute interrupts on the RTC
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

    /**
     * @brief Convert a timestamp with the given offset and epoch to the RTC
     * internal epoch and UTC offset.
     *
     * @param ts The input epoch time.
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
};

#endif
