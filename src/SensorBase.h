/*
 *SensorBase.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the sensor base class.
*/

#ifndef SensorBase_h
#define SensorBase_h


#include <Arduino.h>

enum SENSOR_STATUS
{
    SENSOR_ERROR,
    SENSOR_READY,
    SENSOR_WAITING,
    SENSOR_UNKNOWN
};


// Defines the "SensorBase" Class
class SensorBase
{
public:
    SensorBase(void);
    virtual SENSOR_STATUS setup(void);
    virtual SENSOR_STATUS getStatus(void);
    virtual bool sleep(void);
    virtual bool wake(void);

    virtual bool update(void) = 0;
    virtual String getSensorName(void) = 0;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String varName;
    String unit;
    float sensorValue;
};

#endif
