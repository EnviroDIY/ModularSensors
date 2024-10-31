/**
 * @file WatchDogAVR.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the extendedWatchDogAVR class
 *
 * Code for this is taken from this forum post:
 * https://forum.arduino.cc/index.php?topic=248263.0
 */

// Header Guards
#ifndef SRC_WATCHDOGS_WATCHDOGAVR_H_
#define SRC_WATCHDOGS_WATCHDOGAVR_H_

// Debugging Statement
// #define MS_WATCHDOGAVR_DEBUG
// #define MS_WATCHDOGAVR_DEBUG_DEEP

#ifdef MS_WATCHDOGAVR_DEBUG
#define MS_DEBUGGING_STD "WatchDogAVR"
#endif

#ifdef MS_WATCHDOGAVR_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "WatchDogAVR"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

/**
 * @brief The longest possible time between watchdog interrupts in seconds.
 *
 * For an AVR board, there is one possible watchdog period and the watchdog can
 * be configured to either fire an interrupt at that time or to reset at that
 * time.
 */
#define MAXIMUM_WATCHDOG_PERIOD static_cast<uint32_t>(F_CPU / 1048576)

/**
 * @brief The extendedWatchDogAVR class uses the pre-reset interrupt to of the
 * built in AVR watchdog to extend the allowable time between resets of the
 * watchdog's clock up to multiple minute timescales.
 *
 * The standard watchdog on an AVR processor has a maximum period of 8s without
 * a reset of the watchdog clock before the processor is restarted.
 *
 * Code for this is taken from this forum post:
 * https://forum.arduino.cc/index.php?topic=248263.0
 *
 * @ingroup base_classes
 */
class extendedWatchDogAVR {
 public:
    // Since there can only be one watchdog and all of it's methods are
    // static, disallow the creation of this class.
    extendedWatchDogAVR() = delete;

    /**
     * @brief One-time initialization of watchdog timer.
     *
     * @param resetTime_s The length of time in seconds between resets of the
     * watchdog before the entire board is reset.
     */
    static void setupWatchDog(uint32_t resetTime_s);
    /**
     * @brief Enable the watchdog.
     */
    static void enableWatchDog();
    /**
     * @brief Disable the watchdog.
     */
    static void disableWatchDog();

    /**
     * @brief Reset the number of barks left before the watchdog bites and the
     * board resets.
     *
     * @note This does NOT reset the processor's WDT; that happens in
     * clearWDTInterrupt() called by the ISR.
     */
    static void resetWatchDog();

    /**
     * @brief Reset the processor watchdog flag.
     */
    static void clearWDTInterrupt();

    /**
     * @brief The number of times the pre-reset interrupt is allowed to fire
     * before the watchdog reset is allowed.
     */
    static volatile uint32_t _barksUntilReset;

    /**
     * @brief Internal reference to the number of seconds of silence before the
     * module is reset.
     */
    static uint32_t _resetTime_s;
};

#endif  // SRC_WATCHDOGS_WATCHDOGAVR_H_
