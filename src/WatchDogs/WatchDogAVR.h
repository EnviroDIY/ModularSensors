/**
 * @file WatchDogAVR.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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

#ifdef MS_WATCHDOGAVR_DEBUG
#define MS_DEBUGGING_STD "WatchDogAVR"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD

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
    /**
     * @brief Construct a new extended watch dog object for AVR processors.
     */
    extendedWatchDogAVR();
    /**
     * @brief Destroy the extended watch dog object for AVR processors.
     */
    ~extendedWatchDogAVR();

    /**
     * @brief One-time initialization of watchdog timer.
     *
     * @param resetTime_s The length of time in seconds between resets of the
     * watchdog before the entire board is reset.
     */
    void setupWatchDog(uint32_t resetTime_s);
    /**
     * @brief Enable the watchdog.
     */
    void enableWatchDog();
    /**
     * @brief Disable the watchdog.
     */
    void disableWatchDog();

    /**
     * @brief Reset the watchdog's clock to prevent the board from resetting.
     */
    void resetWatchDog();


    /**
     * @brief The number of times the pre-reset interrupt is allowed to fire
     * before the watchdog reset is allowed.
     */
    static volatile uint32_t _barksUntilReset;

 private:
    uint32_t _resetTime_s;
};

#endif  // SRC_WATCHDOGS_WATCHDOGAVR_H_
