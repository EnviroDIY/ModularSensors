/*
 *WatchDog.cpp

 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the watchdog timer
*/

// Included Dependencies
#include "WatchDog.h"

// Constructor
extendedWatchDog::extendedWatchDog(){}
extendedWatchDog::~extendedWatchDog(){}
volatile uint32_t extendedWatchDog::_barksUntilReset = 0;
