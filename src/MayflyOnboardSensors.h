/*
 *MayflyOnboardSensors.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
*/

#ifndef MayflyOnboardSensors_h
#define MayflyOnboardSensors_h

#include <Arduino.h>
#include "SensorBase.h"


// The "Main" class for the Mayfly
// Only need a sleep and wake since these DON'T use the default of powering up and down
class MayflyOnboardSensors : public virtual Sensor
{
public:
    bool sleep(void) override;
    bool wake(void) override;
};


// Defines the "Temperature Sensor"
class MayflyOnboardTemp : public virtual MayflyOnboardSensors
{
public:
    MayflyOnboardTemp(char const *version);
    String getSensorLocation(void) override;

    bool update(void) override;
    float getValue(void) override;
private:
    const char *_version;
    String sensorLocation;
    float sensorValue_temp;
    unsigned long sensorLastUpdated;
};


// Defines the "Battery Sensor"
class MayflyOnboardBatt : public virtual MayflyOnboardSensors
{
public:
    MayflyOnboardBatt(char const *version);

    bool update(void) override;
    String getSensorLocation(void) override;

    float getValue(void) override;
private:
    const char *_version;
    int _batteryPin;
    String sensorLocation;
    float sensorValue_battery;
    unsigned long sensorLastUpdated;
};


// Defines the "Free Ram" This is not a sensor at all but a board diagnostidc
class MayflyFreeRam : public virtual MayflyOnboardSensors
{
public:
    MayflyFreeRam(void);

    bool update(void) override;
    String getSensorLocation(void) override;

    float getValue(void) override;
private:
    const char *_version;
    String sensorLocation;
    float sensorValue_freeRam;
    unsigned long sensorLastUpdated;
};

#endif
