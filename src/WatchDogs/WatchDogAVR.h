/*
 *WatchDogAVR.h

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *Copyright 2020 Stroud Water Research Center
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the watchdog timer
 *Code for this is taken from this forum post:
 *https://forum.arduino.cc/index.php?topic=248263.0
*/

// Header Guards
#ifndef SRC_WATCHDOGS_WATCHDOGAVR_H_
#define SRC_WATCHDOGS_WATCHDOGAVR_H_

// Debugging Statement
// #define MS_WATCHDOGAVR_DEBUG

#ifdef MS_WATCHDOGAVR_DEBUG
#define MS_DEBUGGING_STD "WatchDogAVR"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD

class extendedWatchDogAVR {
 public:
    // Constructor
    extendedWatchDogAVR();
    ~extendedWatchDogAVR();

    // One-time initialization of watchdog timer.
    void setupWatchDog(uint32_t resetTime_s);
    void enableWatchDog();
    void disableWatchDog();

    void resetWatchDog();

    static volatile uint32_t _barksUntilReset;

 private:
    uint32_t _resetTime_s;
};

#endif  // SRC_WATCHDOGS_WATCHDOGAVR_H_
