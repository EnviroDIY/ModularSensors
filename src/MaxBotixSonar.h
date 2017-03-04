/*
 *MaxBotixSonar.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the MaxBotix Sonar Library
 *It is dependent on Software Serial.
*/

#ifndef MaxBotixSonar_h
#define MaxBotixSonar_h

#include <Arduino.h>
#include <SoftwareSerial_PCINT12.h>
#include "SensorBase.h"

// The main class for the MaxBotix Sonar.  No sub-classes are needed
class MaxBotixSonar_Range : public virtual SensorBase
{
public:
    MaxBotixSonar_Range(int powerPin, int dataPin, int triggerPin = -1);

    SENSOR_STATUS setup(void) override;

    bool update(void) override;

    float getValue(void) override;
protected:
    float sensorValue_depth;
    static unsigned long sensorLastUpdated;
private:
    int _triggerPin;
};

#endif
