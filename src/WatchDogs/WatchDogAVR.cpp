/**
 * @file WatchDogAVR.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the extendedWatchDogAVR class.
 */

#include "WatchDogAVR.h"

// Be careful to use a platform-specific conditional include to only make the
// code visible for the appropriate platform.  Arduino will try to compile and
// link all .cpp files regardless of platform.
#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)

#include <avr/interrupt.h>
#include <avr/wdt.h>

volatile uint32_t extendedWatchDogAVR::_barksUntilReset = 0;
uint32_t          extendedWatchDogAVR::_resetTime_s     = 900;


// One-time initialization of watchdog timer.
void extendedWatchDogAVR::setupWatchDog(uint32_t resetTime_s) {
    _resetTime_s                          = resetTime_s;
    extendedWatchDogAVR::_barksUntilReset = _resetTime_s /
        MAXIMUM_WATCHDOG_PERIOD;
    MS_DBG(F("Watch-dog configured to reset the board after"), _resetTime_s,
           F("sec with a pre-reset interrupt firing every"),
           MAXIMUM_WATCHDOG_PERIOD, F("sec for a total of"),
           extendedWatchDogAVR::_barksUntilReset,
           F("warnings before the reset."));
    // NOTE: The actual processor setup of the watchdog happens in
    // enableWatchDog();
}


void extendedWatchDogAVR::enableWatchDog() {
    MS_DBG(F("Enabling watch dog..."));

    // The next section is timing critical so interrupts are disabled.
    cli();
    // First clear any previous watchdog reset.
    MCUSR &= ~(1 << WDRF);

    // Put timer in interrupt-only mode:
    // WDTCSR - Watchdog Timer Control Register

    // Set WDCE and WDE to enable changes.
    // If changes aren't enabled, we cannot change the prescaler
    WDTCSR |= 0b00011000;  // Set Bit 4 – WDCE: Watchdog Change Enable
                           // Set Bit 3 – WDE: Watchdog System Reset Enable
    // bitwise OR assignment (leaves other bits unchanged)

    // Now can set the full register including the prescaler
    WDTCSR = 0b01100001;
    // Bit 7: WDIF (Watchdog Interrupt Flag) - 0 (Read only)
    // Bit 6: WDIE (Watchdog Interrupt Enable) - 1 (Enabled)
    // Bit 5: WDP3 (Watchdog Timer Prescaler) - see delay interval patterns
    // Bit 4: WDCE (Watchdog Change Enable) - 0 (disable further changes)
    // Bit 3: WDE  (Watchdog System Reset Enable) - 0 (Clear?)
    // Bits 2:0 Watchdog timer prescaler [WDP2:0] - see delay interval patterns

    // maxium delay interval:
    // 0bxx1xx001 = 1048576 clock cycles = ~8 seconds @ 8MHz

    sei();  // re-enable interrupts
    // wdt_reset();  // this is not needed...timer starts without it

    extendedWatchDogAVR::_barksUntilReset = _resetTime_s /
        MAXIMUM_WATCHDOG_PERIOD;
    MS_DBG(F("The watch dog is enabled in interrupt-only mode."));
    MS_DBG(F("The interrupt will fire"), extendedWatchDogAVR::_barksUntilReset,
           F("times before the system resets."));
}


void extendedWatchDogAVR::disableWatchDog() {
    // Disable the watchdog
    wdt_disable();
}


void extendedWatchDogAVR::resetWatchDog() {
    MS_DEEP_DBG(F("Feeding the watch-dog!"));
    extendedWatchDogAVR::_barksUntilReset = _resetTime_s /
        MAXIMUM_WATCHDOG_PERIOD;
}


void extendedWatchDogAVR::clearWDTInterrupt() {
    MS_DEEP_DBG(F("Restarting the processor watchdog timer"));
    wdt_reset();  // start timer again (still in interrupt-only mode)
}


/**
 * @brief ISR for watchdog early warning
 */
ISR(WDT_vect) {
    MS_DEEP_DBG(F("\nWatchdog interrupt!"));
    extendedWatchDogAVR::_barksUntilReset--;  // Increment down the counter,
                                              // makes multi cycle WDT possible
    if (extendedWatchDogAVR::_barksUntilReset <= 0) {
        MS_DEEP_DBG(F("The dog has barked enough; resetting the board."));
        MCUSR = 0;  // reset flags

        // Put timer in reset-only mode:
        WDTCSR |= 0b00011000;  // Enter config mode.
        WDTCSR = 0b00001000 |
            0b000000;  // clr WDIE (interrupt enable...7th from left)
                       // set WDE (reset enable...4th from left), and set delay
                       // interval reset system in 16 ms... unless wdt_disable()
                       // in loop() is reached first

        // wdt_reset();  // not needed
    } else {
        MS_DEEP_DBG(F("There will be"), extendedWatchDogAVR::_barksUntilReset,
                    F("more barks until total time is"),
                    extendedWatchDogAVR::_resetTime_s, F("and board resets"));
        clearWDTInterrupt();  // start timer again
    }
}

#endif
