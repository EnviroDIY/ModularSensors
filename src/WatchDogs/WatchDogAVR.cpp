/*
 *WatchDogAVR.cpp

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the watchdog timer
*/

#include "WatchDogAVR.h"

// Be careful to use a platform-specific conditional include to only make the
// code visible for the appropriate platform.  Arduino will try to compile and
// link all .cpp files regardless of platform.
#if defined(ARDUINO_ARCH_AVR) || defined(__AVR__)

#include <avr/interrupt.h>
#include <avr/wdt.h>

extendedWatchDogAVR::extendedWatchDogAVR(uint32_t resetTime_s)
  : extendedWatchDog(resetTime_s)
{
    _barksUntilReset = _resetTime_s;  // interrupt is in place of the reset
}



// One-time initialization of watchdog timer.
void extendedWatchDogAVR::setupWatchDog(){}


void extendedWatchDogAVR::enableWatchDog()
{
    // We're always using a 1-second timeout

    cli();                              // disable interrupts

    MCUSR = 0;                          // reset status register flags

                                     // Put timer in interrupt-only mode:
    WDTCSR |= 0b00011000;               // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                     // using bitwise OR assignment (leaves other bits unchanged).
    WDTCSR =  0b01000000 | 0b000110;    // set WDIE (interrupt enable...7th from left, on left side of bar)
                                     // clr WDE (reset enable...4th from left)
                                     // and set delay interval (right side of bar) to 1 second,
                                     // using bitwise OR operator.

    sei();                              // re-enable interrupts
    //wdt_reset();                      // this is not needed...timer starts without it

    // delay interval patterns:
    //  16 ms:     0b000000
    //  500 ms:    0b000101
    //  1 second:  0b000110
    //  2 seconds: 0b000111
    //  4 seconds: 0b100000
    //  8 seconds: 0b100001

    _barksUntilReset = _resetTime_s;
}


void extendedWatchDogAVR::disableWatchDog()
{
    // Disable the watchdog
    wdt_disable();
}


void extendedWatchDogAVR::resetWatchDog()
{
    _barksUntilReset = _resetTime_s;
    // Reset the watchdog.
    wdt_reset();
}


ISR(WDT_vect)  // ISR for watchdog early warning
{
    _barksUntilReset--;  // Increament down the counter, makes multi cycle WDT possible
    if (_barksUntilReset<=0)
    {

      MCUSR = 0;                          // reset flags

                                          // Put timer in reset-only mode:
      WDTCSR |= 0b00011000;               // Enter config mode.
      WDTCSR =  0b00001000 | 0b000000;    // clr WDIE (interrupt enable...7th from left)
                                          // set WDE (reset enable...4th from left), and set delay interval
                                          // reset system in 16 ms...
                                          // unless wdt_disable() in loop() is reached first

      //wdt_reset(); // not needed
    }
    else
    {
        wdt_reset();  // start timer again (still in interrupt-only mode)
    }
}

#endif
