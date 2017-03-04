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


// There is no ""Main" class for the Mayfly, these are really 3 independent classes


// Defines the "Temperature Sensor"
class MayflyOnboardTemp : public virtual SensorBase
{
public:
    MayflyOnboardTemp(char const *version);
    String getSensorLocation(void) override;

    bool update(void) override;
    float getValue(void) override;
private:
    const char *_version;
    String sensorLocation;
    static float sensorValue_temp;
    static unsigned long sensorLastUpdated;
};


// Defines the "Battery Sensor"
class MayflyOnboardBatt : public virtual SensorBase
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
    static float sensorValue_battery;
    static unsigned long sensorLastUpdated;
};


// Defines the "Free Ram" This is not a sensor at all but a board diagnostidc
class MayflyFreeRam : public virtual SensorBase
{
public:
    MayflyFreeRam(void);

    bool update(void) override;
    String getSensorLocation(void) override;

    float getValue(void) override;
private:
    const char *_version;
    String sensorLocation;
    static float sensorValue_freeRam;
    static unsigned long sensorLastUpdated;
};

#endif
