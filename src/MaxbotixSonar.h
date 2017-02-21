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

// The main class for the MaxBotix Sonar
class MaxBotixSonar : public virtual SensorBase
{
public:
    MaxBotixSonar(int powerPin, int dataPin);
    SENSOR_STATUS setup(void) override;
    bool sleep(void) override;
    bool wake(void) override;

    bool update(void) override;
    String getSensorName(void) override;
    String getSensorLocation(void) override;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
    virtual String getDreamHost(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String sensorLocation;
    String varName;
    String unit;
    int _powerPin;
    int _dataPin;
    static float sensorValue_depth;
};


// Defines the "Depth Sensor"
class MaxBotixSonar_Depth : public virtual MaxBotixSonar
{
public:
    MaxBotixSonar_Depth(int powerPin, int dataPin);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
};

#endif
