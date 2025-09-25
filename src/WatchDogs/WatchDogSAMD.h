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

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_WATCHDOGSAMD_DEBUG
#define MS_DEBUGGING_STD "WatchDogSAMD"
#endif
#ifdef MS_WATCHDOGSAMD_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "WatchDogSAMD"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP


/**
 * @brief The longest possible time between watchdog interrupts in seconds.
 *
 * For a SAMD board, we can get the longest possible time between interrupts by
 * using the maximum closed window period in "windowed" mode and setting the
 * early warning interrupt that opens the window to occur at the mimimum
 * possible time before a reset fires. The maximum number of clock cycles for
 * the closed window period is 16384 cycles on both a SAMD21 and SAM(D/E)51.
 *
 * On a SAM(D/E)51 the only clock available for the watch dog is the 1.024kHz
 * CLK_WDT_OSC clock sourced from the ULP32KOSC.
 *
 * On a SAMD21 the WDT can be clocked from any clock source with the maximum
 * dividor depending on the selected clock generator. To save power, we force
 * the SAMD21 to use the ULP32KOSC for the WDT and EIC. For simplicity of code,
 * we use a 32x divisor on the ULP32KOSC to match the SAM(D/E)51 1.024kHz
 * CLK_WDT_OSC.
 *
 * 16384 clock cycles at 1.024kHz = 16s
 */
#define MAXIMUM_WATCHDOG_PERIOD 16UL

/**
 * @brief The clock generator number to use for the watchdog timer and the
 * external interrupt controller.
 *
 * This only applies to a SAMD21 device. Neither the WDT nor the EIC require a
 * dedicated clock generator on the SAM(D/E)51.
 */
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
    // Since there can only be one watchdog and all of it's methods are
    // static, disallow the creation of this class.
    extendedWatchDogSAMD() = delete;

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
     * @brief One-time initialization of external interrupt controller.
     */
    static void setupEIC();

    /**
     * @brief Configure the 32768 Hz Oscillator
     */
    static void config32kOSC();
    /**
     * @brief Configure the generic clock generator - sourced from the 32k
     * oscillator
     */
    static void configureClockGenerator();
    /**
     * @brief Configure the peripheral clock for the WDT sourced from the
     * generic clock generator
     */
    static void configureWDTClock();
    /**
     * @brief Configure the peripheral clock for the external interrupt
     * congtroller (EIC) - sourced from the generic clock generator
     */
    static void configureEICClock();

    /**
     * @brief Reset the processor watchdog flag.
     */
    static void clearWDTInterrupt();

    /**
     * @brief Wait for the WDT config bit sync to finish.+
     */
    static void inline waitForWDTBitSync();
    /**
     * @brief Wait for the GCLK config bit sync to finish.+
     */
    static void inline waitForGCLKBitSync();

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

#endif  // SRC_WATCHDOGS_WATCHDOGSAMD_H_
