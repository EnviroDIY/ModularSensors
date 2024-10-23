/**
 * @file WatchDogSAMD.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
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
// #define MS_WATCHDOGSAMD_DEBUG_DEEP

#ifdef MS_WATCHDOGSAMD_DEBUG
#define MS_DEBUGGING_STD "WatchDogSAMD"
#endif

#ifdef MS_WATCHDOGSAMD_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "WatchDogSAMD"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD


/// The clock generator number to use for the watchdog timer and the external
/// interrupt controller
#define GENERIC_CLOCK_GENERATOR_MS (5u)

/**
 * @brief ISR handler for watchdog timer early warning (WDT EW) interrupt
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
     * @brief One-time initialization of external interrupt controller.
     */
    void setupEIC();

    /**
     * @brief Configure the 32768 Hz Oscillator
     */
    void config32kOSC();
    /**
     * @brief Configure the generic clock generator - sourced from the 32k
     * oscillator
     */
    void configureClockGenerator();
    /**
     * @brief Configure the peripheral clock for the WDT sourced from the
     * generic clock generator
     */
    void configureWDTClock();
    /**
     * @brief Configure the peripheral clock for the external interrupt
     * congtroller (EIC) - sourced from the generic clock generator
     */
    void configureEICClock();


    /**
     * @brief The number of times the pre-reset interrupt is allowed to fire
     * before the watchdog reset is allowed.
     */
    static volatile uint32_t _barksUntilReset;

 private:
    /**
     * @brief Wait for the WDT config bit sync to finish.+
     */
    void inline waitForWDTBitSync();
    /**
     * @brief Wait for the GCLK config bit sync to finish.+
     */
    void inline waitForGCLKBitSync();
    /**
     * @brief Internal reference to the number of seconds of silence before
     * the module is reset.
     */
    uint32_t _resetTime_s;
};

#endif  // SRC_WATCHDOGS_WATCHDOGSAMD_H_
