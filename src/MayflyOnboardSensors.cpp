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

#include <Sodaq_DS3231.h>
#include "MayflyOnboardSensors.h"

// Need to know the Mayfly version because the battery resistor depends on it
EnviroDIYMayfly::EnviroDIYMayfly(const char *version) : Sensor(-1, -1, F("EnviroDIYMayfly"), MAYFLY_NUM_MEASUREMENTS)
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

String EnviroDIYMayfly::getSensorLocation(void) {return F("EnviroDIYMayfly");}
bool EnviroDIYMayfly::sleep(void) {return true;}
bool EnviroDIYMayfly::wake(void) {return true;}

bool EnviroDIYMayfly::update(void)
{
    // Clear values before starting loop
    clearValues();

    // Get the temperature from the Mayfly's real time clock
    DBGM(F("Getting DS3231 Temperature\n"));
    rtc.convertTemperature();  //convert current temperature into registers
    float tempVal = rtc.getTemperature();
    sensorValues[MAYFLY_TEMP_VAR_NUM] = tempVal;

    // Get the battery voltage from the Mayfly's real time clock
    DBGM(F("Getting battery voltage\n"));
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
    DBGM(F("Getting Free RAM\n"));
    extern int __heap_start, *__brkval;
    int v;
    float sensorValue_freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    sensorValues[MAYFLY_RAM_VAR_NUM] = sensorValue_freeRam;

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
