/*
 *WatchDog.h

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the watchdog timer
*/

// Header Guards
#ifndef WatchDog_h
#define WatchDog_h

// Debugging Statement
// #define MS_WATCHDOG_DEBUG

#ifdef MS_WATCHDOG_DEBUG
#define MS_DEBUGGING_STD "WatchDog"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD

volatile uint32_t _barksUntilReset;

class extendedWatchDog
{

public:
    // Constructor
    extendedWatchDog(uint32_t resetTime_s);

    // Set/Get the time before reset
    void setResetTime(uint32_t resetTime_s);
    uint32_t getResetTime();

    // One-time initialization of watchdog timer.
    virtual void setupWatchDog() = 0;
    virtual void enableWatchDog() = 0;
    virtual void disableWatchDog() = 0;

    virtual void resetWatchDog() = 0;

protected:
    uint32_t _resetTime_s;
};

#endif
