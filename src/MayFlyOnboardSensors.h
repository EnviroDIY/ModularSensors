/*
 *MayFlyOnboardSensors.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
*/

#ifndef MayFlyOnboardSensors_h
#define MayFlyOnboardSensors_h

#include <Arduino.h>
#include "SensorBase.h"

// The main class for the Mayfly
class MayFlyOnboardSensors : public virtual SensorBase
{
public:
    MayFlyOnboardSensors(void);
    // virtual ~MayFlyOnboardSensors(void);

    bool update(void) override;
    String getSensorName(void) override;

    virtual String getVarName(void) = 0;
    virtual String getVarUnit(void) = 0;
    virtual float getValue(void) = 0;
protected:
    SENSOR_STATUS sensorStatus;
    String sensorName;
    String varName;
    String unit;
    int _batteryPin;
    float sensorValue_temp;
    float sensorValue_battery;
};


// Defines the "Temperature Sensor"
class MayFlyOnboardTemp : public virtual MayFlyOnboardSensors
{
public:
    using MayFlyOnboardSensors::MayFlyOnboardSensors;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};


// Defines the "Battery Sensor"
class MayFlyOnboardBatt : public virtual MayFlyOnboardSensors
{
public:
    using MayFlyOnboardSensors::MayFlyOnboardSensors;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
};

#endif
