/*
 *SensorBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the sensor base class.
*/

#include <Arduino.h>
#include "SensorBase.h"

// The constructor
SensorBase::SensorBase(void)
{}


// The function to set up connection to a sensor.
// By default, returns ready
SENSOR_STATUS SensorBase::setup(void)
{
    return SENSOR_READY;
}

// The function to return the status of a sensor
// By default, returns ready
SENSOR_STATUS SensorBase::getStatus(void)
{
    return SENSOR_READY;
}

// The function to put a sensor to sleep
// By default, returns true
bool SensorBase::sleep(void)
{
    return true;
}

// The function to wake up a sensor
// By default, returns true
bool SensorBase::wake(void)
{
    return true;
}
