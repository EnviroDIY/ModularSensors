/*
 *WatchDogAVR.h

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the watchdog timer
 *Code for this is taken from this forum post:
 *https://forum.arduino.cc/index.php?topic=248263.0
*/

// Header Guards
#ifndef WatchDogAVR_h
#define WatchDogAVR_h

// Debugging Statement
// #define MS_WATCHDOGAVR_DEBUG

#ifdef MS_WATCHDOGAVR_DEBUG
#define MS_DEBUGGING_STD "WatchDogAVR"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "WatchDog.h"

class extendedWatchDogAVR : extendedWatchDog
{

public:
    // Constructor
    extendedWatchDogAVR();
    virtual ~extendedWatchDogAVR();

    // One-time initialization of watchdog timer.
    void setupWatchDog(uint32_t resetTime_s);
    void enableWatchDog();
    void disableWatchDog();

    void resetWatchDog();
};

#endif
