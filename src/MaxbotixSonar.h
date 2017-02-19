/*
 *MaxbotixSonar.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Maxbotix Sonar Library
 *It is dependent on Software Serial.
*/

#ifndef MaxbotixSonar_h
#define MaxbotixSonar_h

#include <Arduino.h>
#include <SoftwareSerial_PCINT12.h>
#include "SensorBase.h"

// The main class for the Maxbotix Sonar
class MaxbotixSonar : public virtual SensorBase
{
public:
    MaxbotixSonar(int excitePin, int dataPin);
    SENSOR_STATUS setup(void) override;
    bool sleep(void) override;

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
    int _excitePin;
    int _dataPin;
    static float sensorValue_depth;
private:
    SoftwareSerialMod *_sonarSerial;
};


// Defines the "Depth Sensor"
class MaxbotixSonar_Depth : public virtual MaxbotixSonar
{
public:
    MaxbotixSonar_Depth(int excitePin, int dataPin);

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
};

#endif
