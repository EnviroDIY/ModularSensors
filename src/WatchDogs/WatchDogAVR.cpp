/**
 * @file WatchDogAVR.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the extendedWatchDogAVR class.
 */

#include "WatchDogAVR.h"

// Be careful to use a platform-specific conditional include to only make the
// code visible for the appropriate platform.  Arduino will try to compile and
// link all .cpp files regardless of platform.
#if defined(ARDUINO_ARCH_AVR) || defined(__AVR__)

#include <avr/interrupt.h>
#include <avr/wdt.h>

volatile uint32_t extendedWatchDogAVR::_barksUntilReset = 0;

extendedWatchDogAVR::extendedWatchDogAVR() {}
extendedWatchDogAVR::~extendedWatchDogAVR() {
    disableWatchDog();
}


// One-time initialization of watchdog timer.
void extendedWatchDogAVR::setupWatchDog(uint32_t resetTime_s) {
    _resetTime_s                          = resetTime_s;
    extendedWatchDogAVR::_barksUntilReset = _resetTime_s / 8;
    MS_DBG(F("Watch-dog timeout is set for"), _resetTime_s,
           F("sec with the interrupt firing"),
           extendedWatchDogAVR::_barksUntilReset, F("times before the reset."));
}


void extendedWatchDogAVR::enableWatchDog() {
    MS_DBG(F("Enabling watch dog..."));

    cli();  // disable interrupts

    MCUSR = 0;  // reset status register flags

    // Put timer in interrupt-only mode:
    // WDTCSR - Watchdog Timer Control Register
    WDTCSR |= 0b00011000;  // Set Bit 4 – WDCE: Watchdog Change Enable
                           // Set Bit 3 – WDE: Watchdog System Reset Enable
    // bitwise OR assignment (leaves other bits unchanged)
    // Need to set the change and reset enables before changing the prescaler

    WDTCSR = 0b01100001;  // Set Bit 6 – WDIE: Watchdog Interrupt Enable
                          // Unset Bit 4 – WDCE: Watchdog Change Enable
                          // Unset Bit 3 – WDE: Watchdog System Reset Enable
                          // Set Bit 5 - WDP[3] and Bit 0 – WDP[0]:
    // Watchdog Timer Prescalers 3 and 0 - 1024K cycles = 8.0s
    // bitwise OR assignment (leaves other bits unchanged)

    sei();  // re-enable interrupts
    // wdt_reset();  // this is not needed...timer starts without it

    // delay interval patterns:
    //  16 ms:     0bxx0xx000
    //  500 ms:    0bxx0xx101
    //  1 second:  0bxx0xx110
    //  2 seconds: 0bxx0xx111
    //  4 seconds: 0bxx1xx000
    //  8 seconds: 0bxx1xx001

    extendedWatchDogAVR::_barksUntilReset = _resetTime_s / 8;
    MS_DBG(F("The watch dog is enabled in interrupt-only mode."));
    MS_DBG(F("The interrupt will fire"), extendedWatchDogAVR::_barksUntilReset,
           F("times before the system resets."));
}


void extendedWatchDogAVR::disableWatchDog() {
    // Disable the watchdog
    wdt_disable();
}


void extendedWatchDogAVR::resetWatchDog() {
    extendedWatchDogAVR::_barksUntilReset = _resetTime_s / 8;
    // Reset the watchdog.
    wdt_reset();
}


/**
 * @brief ISR for watchdog early warning
 */
ISR(WDT_vect) {
    extendedWatchDogAVR::_barksUntilReset--;  // Increament down the counter,
                                              // makes multi cycle WDT possible
    // MS_DBG(F("\nWatchdog interrupt!"),
    // extendedWatchDogAVR::_barksUntilReset);
    if (extendedWatchDogAVR::_barksUntilReset <= 0) {
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
        wdt_reset();  // start timer again (still in interrupt-only mode)
    }
}

#endif
