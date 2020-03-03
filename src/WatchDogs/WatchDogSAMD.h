/*
 *WatchDogSAMD.h

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the watchdog timer
 *Code for this is taken from the Adafruit SleepyDog library:
 *  https://github.com/adafruit/Adafruit_SleepyDog/
 *and this library:  https://github.com/javos65/WDTZero
*/

// Header Guards
#ifndef WatchDogSAMD_h
#define WatchDogSAMD_h

// Debugging Statement
// #define MS_WATCHDOGSAMD_DEBUG

#ifdef MS_WATCHDOGSAMD_DEBUG
#define MS_DEBUGGING_STD "WatchDogSAMD"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD


void WDT_Handler(void);  // ISR HANDLER FOR WDT EW INTERRUPT

class extendedWatchDogSAMD {
 public:
    // Constructor
    extendedWatchDogSAMD();
    ~extendedWatchDogSAMD();

    // One-time initialization of watchdog timer.
    void setupWatchDog(uint32_t resetTime_s);
    void enableWatchDog();
    void disableWatchDog();

    void resetWatchDog();

    static volatile uint32_t _barksUntilReset;

 private:
    void inline waitForWDTBitSync();
    uint32_t _resetTime_s;
};

#endif
