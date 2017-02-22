/*
 *MayflyOnboardSensors.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
*/

#ifndef MayflyOnboardSensors_h
#define MayflyOnboardSensors_h

#include <Arduino.h>
#include "SensorBase.h"


// The main class for the Mayfly
class MayflyOnboardSensors : public virtual SensorBase
{
public:
    MayflyOnboardSensors(char const *version);

    virtual bool update(void)  = 0;
    String getSensorName(void) override;
    virtual String getSensorLocation(void) =0 ;

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
    int _batteryPin;
    const char *_version;
    static float sensorValue_temp;
    static float sensorValue_battery;
    static float sensorValue_freeRam;
};


// Defines the "Temperature Sensor"
class MayflyOnboardTemp : public virtual MayflyOnboardSensors
{
public:
    MayflyOnboardTemp(char const *version);

    bool update(void) override;
    String getSensorLocation(void) override;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    static unsigned long sensorLastUpdated;
};


// Defines the "Battery Sensor"
class MayflyOnboardBatt : public virtual MayflyOnboardSensors
{
public:
    MayflyOnboardBatt(char const *version);

    bool update(void) override;
    String getSensorLocation(void) override;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    static unsigned long sensorLastUpdated;
};


// Defines the "Free Ram" This is not a sensor at all but a board diagnostidc
class MayflyFreeRam : public virtual MayflyOnboardSensors
{
public:
    MayflyFreeRam(char const *version);

    bool update(void) override;
    String getSensorLocation(void) override;

    String getVarName(void) override;
    String getVarUnit(void) override;
    float getValue(void) override;
    String getDreamHost(void) override;
private:
    static unsigned long sensorLastUpdated;
};

#endif
