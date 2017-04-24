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

#include <Arduino.h>
#include <Sodaq_DS3231.h>
#include "SensorBase.h"

#define MAYFLY_NUM_MEASUREMENTS 3

#define DS3231_TEMP_RESOLUTION 2
#define MAYFLY_TEMP_VAR_NUM 0

#define MAYFLY_BATTERY_RESOLUTION 3
#define MAYFLY_BATTERY_VAR_NUM 1

#define MAYFLY_RAM_RESOLUTION 0
#define MAYFLY_RAM_VAR_NUM 2


// The "Main" class for the Mayfly
// Only need a sleep and wake since these DON'T use the default of powering up and down
class EnviroDIYMayfly : public virtual Sensor
{
public:
    // Need to know the Mayfly version because the battery resistor depends on it
    EnviroDIYMayfly(char *version) : Sensor(-1, -1, F("EnviroDIYMayfly"), MAYFLY_NUM_MEASUREMENTS)
    {
        _version = version;

        if (strcmp(_version, "v0.3") == 0 or strcmp(_version, "v0.4") == 0)
        {
          // Set the pin to read the battery voltage
          _batteryPin = A6;
        }
        if (strcmp(_version, "v0.5") == 0)
        {
          // Set the pin to read the battery voltage
          _batteryPin = A6;
        }
    }
    // Overload constructor
    EnviroDIYMayfly(char version) : Sensor(-1, -1, F("EnviroDIYMayfly"))
    {
        _version = &version;

        if (strcmp(_version, "v0.3") == 0 or strcmp(_version, "v0.4") == 0)
        {
          // Set the pin to read the battery voltage
          _batteryPin = A6;
        }
        if (strcmp(_version, "v0.5") == 0)
        {
          // Set the pin to read the battery voltage
          _batteryPin = A6;
        }
    }

    String getSensorLocation(void) override {return F("EnviroDIYMayfly");}
    bool sleep(void) override {return true;}
    bool wake(void) override {return true;}

    bool update(void) override
    {
        // Clear values before starting loop
        clearValues();

        // Get the temperature from the Mayfly's real time clock
        rtc.convertTemperature();  //convert current temperature into registers
        float tempVal = rtc.getTemperature();
        sensorValues[MAYFLY_TEMP_VAR_NUM] = tempVal;

        float rawBattery;
        float sensorValue_battery;
        if (strcmp(_version, "v0.3") == 0 or strcmp(_version, "v0.4") == 0)
        {
            // Get the battery voltage
            rawBattery = analogRead(_batteryPin);
            sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
        }
        if (strcmp(_version, "v0.5") == 0)
        {
            // Get the battery voltage
            rawBattery = analogRead(_batteryPin);
            sensorValue_battery = (3.3 / 1023.) * 4.7 * rawBattery;
        }
        sensorValues[MAYFLY_BATTERY_VAR_NUM] = sensorValue_battery;

        // Used only for debugging - can be removed
        extern int __heap_start, *__brkval;
        int v;
        float sensorValue_freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
        sensorValues[MAYFLY_RAM_VAR_NUM] = sensorValue_freeRam;

        // Update the registered variables with the new values
        notifyVariables();

        // Return true when finished
        return true;
    }

private:
    const char *_version;
    int _batteryPin;
};


class MayflyOnboardTemp : public virtual Variable
{
public:
    MayflyOnboardTemp(Sensor *parentSense)
      : Variable(parentSense, MAYFLY_TEMP_VAR_NUM,
                 F("temperatureDatalogger"), F("degreeCelsius"),
                 DS3231_TEMP_RESOLUTION, F("BoardTemp"))
    {}
};


class MayflyOnboardBatt : public virtual Variable
{
public:
    MayflyOnboardBatt(Sensor *parentSense)
      : Variable(parentSense, MAYFLY_BATTERY_VAR_NUM,
                 F("batteryVoltage"), F("Volt"),
                 MAYFLY_BATTERY_RESOLUTION, F("Battery"))
    {}
};


// Defines the "Free Ram" This is not a sensor at all but a board diagnostic
class MayflyFreeRam : public virtual Variable
{
public:
    MayflyFreeRam(Sensor *parentSense)
      : Variable(parentSense, MAYFLY_RAM_VAR_NUM,
                 F("Free SRAM"), F("Bit"),
                 MAYFLY_RAM_RESOLUTION, F("FreeRam"))
    {}
};

#endif
