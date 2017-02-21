/*
 *CampbellOSB3.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices 5TM Soild Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#ifndef CampbellOSB3_h
#define CampbellOSB3_h

#include <Arduino.h>
#include "SensorBase.h"

// The main class for the Campbell OSB3
class CampbellOSB3 : public virtual SensorBase
{
public:
    CampbellOSB3(int powerPin, int dataPin,
                 float A, float B, float C);
    SENSOR_STATUS setup(void) override;

    bool update(void) override;
    String getSensorName(void) override;
    String getSensorLocation(void) override;
    bool sleep(void) override;
    bool wake(void) override;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    virtual String getDreamHost(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String sensorLocation;
    String varName;
    String unit;
    int _powerPin;
    int16_t _dataPin;
    float _A;
    float _B;
    float _C;
    static float sensorValue;
};


// Defines the "Low Turbidity Sensor"
class CampbellOSB3_TurbLow : public virtual CampbellOSB3
{
public:
    CampbellOSB3_TurbLow(int powerPin, int dataPin,
                         float A, float B, float C);

    String getDreamHost(void) override;
};


// Defines the "High Turbidity Sensor"
class CampbellOSB3_TurbHigh : public virtual CampbellOSB3
{
public:
    CampbellOSB3_TurbHigh(int powerPin, int dataPin,
                          float A, float B, float C);

    String getDreamHost(void) override;
};

#endif
