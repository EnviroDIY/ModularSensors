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

extendedWatchDogAVR::extendedWatchDogAVR(){}
extendedWatchDogAVR::~extendedWatchDogAVR()
{
    disableWatchDog();
}



// One-time initialization of watchdog timer.
void extendedWatchDogAVR::setupWatchDog(uint32_t resetTime_s)
{
    _resetTime_s = resetTime_s;
    extendedWatchDog::_barksUntilReset = _resetTime_s/8;
    MS_DBG(F("Watch-dog timeout is set for"),
           _resetTime_s,
           F("with the interrupt firing"),
           extendedWatchDog::_barksUntilReset,
           F("times before the reset."));
}


void extendedWatchDogAVR::enableWatchDog()
{
    MS_DBG(F("Enabling watch dog..."),

    cli();                              // disable interrupts

    MCUSR = 0;                          // reset status register flags

                                     // Put timer in interrupt-only mode:
    WDTCSR |= 0b00011000;               // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                     // using bitwise OR assignment (leaves other bits unchanged).
    WDTCSR =  0b01000000 | 0b100001;    // set WDIE (interrupt enable...7th from left, on left side of bar)
                                     // clr WDE (reset enable...4th from left)
                                     // and set delay interval (right side of bar) to 8 seconds,
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

    extendedWatchDog::_barksUntilReset = _resetTime_s/8;
    MS_DBG(F("The watch dog enabled in interrupt-only mode."));"
    MS_DBG(F("The interrupt will fire"),
           extendedWatchDog::_barksUntilReset,
           F("times before the system resets."));
}


void extendedWatchDogAVR::disableWatchDog()
{
    // Disable the watchdog
    wdt_disable();
}


void extendedWatchDogAVR::resetWatchDog()
{
    extendedWatchDog::_barksUntilReset = _resetTime_s/8;
    // Reset the watchdog.
    wdt_reset();
}


ISR(WDT_vect)  // ISR for watchdog early warning
{
    extendedWatchDog::_barksUntilReset--;  // Increament down the counter, makes multi cycle WDT possible
    MS_DBG(F("Watchdog interrupt!"), extendedWatchDog::_barksUntilReset);
    if (extendedWatchDog::_barksUntilReset<=0)
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
