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

// Include the EpochTime library
#include <EpochTime.h>

// Validate that exactly one clock has been selected (should be set by
// KnownProcessors.h)
#if (defined(MS_USE_RV8803) + defined(MS_USE_DS3231) + \
     defined(MS_USE_RTC_ZERO)) > 1
#error Multiple clocks defined! Only one of MS_USE_RV8803, MS_USE_DS3231, or MS_USE_RTC_ZERO can be selected at a time.
#elif (defined(MS_USE_RV8803) + defined(MS_USE_DS3231) + \
       defined(MS_USE_RTC_ZERO)) == 0 &&                 \
    (defined(ARDUINO_ARCH_SAMD) && !defined(__SAMD51__))
#pragma message "No clock defined! Using processor as RTC."
#elif (defined(MS_USE_RV8803) + defined(MS_USE_DS3231) + \
       defined(MS_USE_RTC_ZERO)) == 0
#error No clock defined! Define exactly one of MS_USE_RV8803, MS_USE_DS3231, or MS_USE_RTC_ZERO for the RTC. Check that KnownProcessors.h is properly setting defaults for your board, or select a clock in ModSensorConfig.h for other processors.
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
// #pragma message "Using RV-8803 RTC as the clock."
#define MS_CLOCK_NAME "RV-8803"
#include <SparkFun_RV8803.h>
// Interrupt is active low on the RV8803
#define CLOCK_INTERRUPT_MODE FALLING
#elif defined(MS_USE_DS3231)
// #pragma message "Using DS3231 RTC as the clock."
#define MS_CLOCK_NAME "DS3231"
#include <Sodaq_DS3231.h>
// Interrupt is active low on the DS3231
#define CLOCK_INTERRUPT_MODE FALLING
#elif defined(MS_USE_RTC_ZERO)
// #pragma message "Using SAMD 32-bit RTC as the clock."
#define MS_CLOCK_NAME "SAMD 32-bit RTC"
#include <RTCZero.h>
#endif


/**
 * @brief A class for the clock attached to the logger.
 *
 * @note This is effectively a static class, with all static functions and a
 * deleted constructor.
 *
 * @todo Support half/quarter hour time zones
 *
 * Dealing with time is **hard**! This library only supports the bare minimum of
 * what I think is necessary to get the logger's clock working and to convert
 * between different epoch types. It does not support time zones (other than a
 * static offset from UTC), daylight savings time, or any of the other
 * complications of time.
 *
 * If you thought handling time was simple, read this:
 * https://gist.github.com/timvisee/fcda9bbdff88d45cc9061606b4b923ca
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
     * @note I VERY, VERY STRONGLY RECOMMEND SETTING THE RTC IN UTC
     * (i.e., offset = 0)
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
    static int8_t getRTCOffset();

    /**
     * @brief Get the current Universal Coordinated Time (UTC) epoch time from
     * the RTC.
     *
     * @param utcOffset The offset from UTC to return the epoch time in.
     * @param epoch The type of epoch to use (i.e., the standard for the start
     * of the epoch).
     *
     * @return The number of seconds from the start of the given epoch.
     */
    static time_t getNowAsEpoch(int8_t utcOffset, epochStart epoch);

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
     * @brief Verify that the input value is sane and if so sets the real time
     * clock to the given time.
     *
     * @param ts The number of seconds since the start of the given epoch.
     * @param utcOffset The offset of the epoch time from UTC.
     * @param epoch The type of epoch to use (i.e., the standard for the start
     * of the epoch).
     *
     * @return True if the input timestamp passes sanity checks **and**
     * the clock is now at or within tolerance (±5 seconds) of the target time.
     * This includes both cases where the clock was successfully set and where
     * the clock was already within tolerance and did not need adjustment.
     *
     * @note There is no timezone correction in this function
     * @note Changed behavior: Previously returned true only when clock was
     * actually written. Now returns true when clock is at/within tolerance,
     * regardless of whether it was written.
     */
    static bool setRTClock(time_t ts, int8_t utcOffset, epochStart epoch);
    /**
     * @brief Verify that the input value is sane and if so sets the real time
     * clock to the given time.
     *
     * @param in_time An epochTime object
     * @param utcOffset The offset of the epoch time from UTC.
     *
     * @return True if the input timestamp passes sanity checks **and**
     * the clock is now at or within tolerance (±5 seconds) of the target time.
     * This includes both cases where the clock was successfully set and where
     * the clock was already within tolerance and did not need adjustment.
     *
     * @note There is no timezone correction in this function
     * @note Changed behavior: Previously returned true only when clock was
     * actually written. Now returns true when clock is at/within tolerance,
     * regardless of whether it was written.
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
    static bool isRTCSane();

    /**
     * @brief Set an alarm to fire a clock interrupt at a specific epoch time
     *
     * @param ts The timestamp for the next interrupt - in seconds from the
     * start of the input epoch.
     * @param utcOffset The offset of the epoch time from UTC in hours.
     * @param epoch The type of epoch to use (i.e., the standard for the start
     * of the epoch).
     */
    static void setNextRTCInterrupt(time_t ts, int8_t utcOffset,
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
    static void rtcISR();

    /**
     * @brief Start up the real-time clock.
     *
     * This is used for operations that cannot happen in the constructor - they
     * must happen at run time, not at compile time.
     */
    static void begin();

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
    static inline time_t tsToRawRTC(time_t ts, int8_t utcOffset,
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
    static inline time_t tsFromRawRTC(time_t ts, int8_t utcOffset,
                                      epochStart epoch);

    /**
     * @brief Get a raw epoch time from the RTC
     *
     * @return The raw epoch time from the RTC
     */
    static time_t getRawRTCNow();
    /**
     * @brief Sets the RTC to exactly the epoch time provided
     *
     * @param ts A timestamp already in the epoch and timezone used internally
     * by the RTC
     */
    static void setRawRTCNow(time_t ts);
    /**
     * @brief Begins the underlying RTC
     */
    static void rtcBegin();
};

#endif

// cSpell:ignore nist hile
