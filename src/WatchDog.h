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


class extendedWatchDog
{

public:
    // Constructor
    extendedWatchDog();
    virtual ~extendedWatchDog();

    virtual void setupWatchDog(uint32_t resetTime_s) = 0;
    virtual void enableWatchDog() = 0;
    virtual void disableWatchDog() = 0;

    virtual void resetWatchDog() = 0;
    static volatile uint32_t _barksUntilReset;

protected:
    uint32_t _resetTime_s;
};

#endif