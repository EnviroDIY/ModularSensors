// MayFlyExampleSensor2.h

#ifndef _MAYFLYEXAMPLESENSOR2_h
#define _MAYFLYEXAMPLESENSOR2_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Sensor.h"
#include "Sodaq_DS3231.h"
#include <Wire.h>

class MayFlyExampleSensor2 : public Sensor<float>
{
public:
    MayFlyExampleSensor2(void);
    bool update(void);
    String getValueAsString();
};

#endif
