/*
 *MayflyOnboardSensors.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
 *
 *For temperature from the DS3231:
 *  Resolution is 0.25°C
 *  Accuracy is ±3°C
 *
 *For battery voltage:
 *  Range of 0-5V with 10bit ADC - resolution of 0.005
*/

#ifndef MayflyOnboardSensors_h
#define MayflyOnboardSensors_h

#include "SensorBase.h"
#include "VariableBase.h"

// #define MODULES_DBG Serial
#include "ModSensorDebugger.h"

#define MAYFLY_NUM_MEASUREMENTS 3

#define DS3231_TEMP_RESOLUTION 2
#define MAYFLY_TEMP_VAR_NUM 0

#define MAYFLY_BATTERY_RESOLUTION 3
#define MAYFLY_BATTERY_VAR_NUM 1

#define MAYFLY_RAM_RESOLUTION 0
#define MAYFLY_RAM_VAR_NUM 2


// The "Main" class for the Mayfly
// Only need a sleep and wake since these DON'T use the default of powering up and down
class EnviroDIYMayfly : public Sensor
{
public:
    // Need to know the Mayfly version because the battery resistor depends on it
    EnviroDIYMayfly(const char *version);

    String getSensorLocation(void) override;
    bool sleep(void) override;
    bool wake(void) override;

    bool update(void) override;

private:
    const char *_version;
    int _batteryPin;
};


class EnviroDIYMayfly_Temp : public Variable
{
public:
    EnviroDIYMayfly_Temp(Sensor *parentSense)
      : Variable(parentSense, MAYFLY_TEMP_VAR_NUM,
                 F("temperatureDatalogger"), F("degreeCelsius"),
                 DS3231_TEMP_RESOLUTION, F("BoardTemp"))
    {}
};


class EnviroDIYMayfly_Batt : public Variable
{
public:
    EnviroDIYMayfly_Batt(Sensor *parentSense)
      : Variable(parentSense, MAYFLY_BATTERY_VAR_NUM,
                 F("batteryVoltage"), F("Volt"),
                 MAYFLY_BATTERY_RESOLUTION, F("Battery"))
    {}
};


// Defines the "Free Ram" This is not a sensor at all but a board diagnostic
class EnviroDIYMayfly_FreeRam : public Variable
{
public:
    EnviroDIYMayfly_FreeRam(Sensor *parentSense)
      : Variable(parentSense, MAYFLY_RAM_VAR_NUM,
                 F("Free SRAM"), F("Bit"),
                 MAYFLY_RAM_RESOLUTION, F("FreeRam"))
    {}
};

#endif
