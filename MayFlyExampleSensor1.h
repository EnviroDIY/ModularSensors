// MayFlyExampleSensor1.h

#ifndef _MAYFLYEXAMPLESENSOR1_h
#define _MAYFLYEXAMPLESENSOR1_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Sensor.h"
#include "Sodaq_DS3231.h"
#include <Wire.h>

class MayFlyExampleSensor1 : public Sensor<float>
{
public:
    MayFlyExampleSensor1(void);
    bool update(void);
    String getValueAsString();
};

#endif
