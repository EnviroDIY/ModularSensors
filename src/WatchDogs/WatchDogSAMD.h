/**
 * @file WatchDogSAMD.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the extendedWatchDogSAMD class.
 *
 * Code for this is taken from the Adafruit SleepyDog library:
 * https://github.com/adafruit/Adafruit_SleepyDog/ and this library:
 * https://github.com/javos65/WDTZero
 */

// Header Guards
#ifndef SRC_WATCHDOGS_WATCHDOGSAMD_H_
#define SRC_WATCHDOGS_WATCHDOGSAMD_H_

// Debugging Statement
// #define MS_WATCHDOGSAMD_DEBUG

#ifdef MS_WATCHDOGSAMD_DEBUG
#define MS_DEBUGGING_STD "WatchDogSAMD"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD

/**
 * @brief ISR handler for watchdog timer early warning (WDT EW ) interrupt
 */
void WDT_Handler(void);

/**
 * @brief The extendedWatchDogSAMD class uses the early warning interrupt to of
 * the built in SAMD watchdog to extend the allowable time between resets of the
 * watchdog's clock up to multiple minute timescales.
 *
 * Code for this is taken from the Adafruit SleepyDog library:
 * https://github.com/adafruit/Adafruit_SleepyDog/ and this library:
 * https://github.com/javos65/WDTZero
 *
 * @ingroup base_classes
 */
class extendedWatchDogSAMD {
 public:
    /**
     * @brief Construct a new extended watch dog object for SAMD processors.
     */
    extendedWatchDogSAMD();
    /**
     * @brief Destroy the extended watch dog object for SAMD processors.
     */
    ~extendedWatchDogSAMD();

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
    void inline waitForWDTBitSync();
    uint32_t _resetTime_s;
};

#endif  // SRC_WATCHDOGS_WATCHDOGSAMD_H_
