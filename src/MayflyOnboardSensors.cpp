/*
 *MayflyOnboardSensors.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
*/

#include <Arduino.h>
#include "MayflyOnboardSensors.h"
#include <Sodaq_DS3231.h>


// No power pin to switch, only returns true
bool MayflyOnboardSensors::sleep(void)
{
    return true;
}

// No power pin to switch, only returns true
bool MayflyOnboardSensors::wake(void)
{
    return true;
}


// The static variables that need to be updated
float MayflyOnboardTemp::sensorValue_temp = 0;
float MayflyOnboardBatt::sensorValue_battery = 0;
float MayflyFreeRam::sensorValue_freeRam = 0;
unsigned long MayflyOnboardTemp::sensorLastUpdated = 0;
unsigned long MayflyOnboardBatt::sensorLastUpdated = 0;
unsigned long MayflyFreeRam::sensorLastUpdated = 0;


MayflyOnboardTemp::MayflyOnboardTemp(char const *version)
  : SensorBase(-1, -1, F("EnviroDIYMayfly"), F("temperatureDatalogger"), F("degreeCelsius"), F("BoardTemp"))
{ _version = version; }
// The location of the sensor on the Mayfly
String MayflyOnboardTemp::getSensorLocation(void)
{
    sensorLocation = F("DS3231");
    return sensorLocation;
}

// How to update the onboard sensors
bool MayflyOnboardTemp::update(void)
{
    // Get the temperature from the Mayfly's real time clock
    rtc.convertTemperature();  //convert current temperature into registers
    float tempVal = rtc.getTemperature();
    MayflyOnboardTemp::sensorValue_temp = tempVal;
    MayflyOnboardTemp::sensorLastUpdated = millis();

    // Return true when finished
    return true;
}

float MayflyOnboardTemp::getValue(void)
{
    checkForUpdate(MayflyOnboardTemp::sensorLastUpdated);
    return MayflyOnboardTemp::sensorValue_temp;
}


// The constructor - needs to reference the super-class constructor
MayflyOnboardBatt::MayflyOnboardBatt(char const *version)
  : SensorBase(-1, -1, F("EnviroDIYMayfly"), F("batteryVoltage"), F("Volt"), F("Battery"))
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

// The location of the sensor on the Mayfly
String MayflyOnboardBatt::getSensorLocation(void)
{
    sensorLocation = String(_batteryPin);
    return sensorLocation;
}

// How to update the onboard sensors
bool MayflyOnboardBatt::update(void)
{
    if (strcmp(_version, "v0.3") == 0 or strcmp(_version, "v0.4") == 0)
    {
        // Get the battery voltage
        float rawBattery = analogRead(_batteryPin);
        MayflyOnboardBatt::sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
        MayflyOnboardBatt::sensorLastUpdated = millis();
    }
    if (strcmp(_version, "v0.5") == 0)
    {
        // Get the battery voltage
        float rawBattery = analogRead(_batteryPin);
        MayflyOnboardBatt::sensorValue_battery = (3.3 / 1023.) * 4.7 * rawBattery;
        MayflyOnboardBatt::sensorLastUpdated = millis();
    }

    // Return true when finished
    return true;
}

float MayflyOnboardBatt::getValue(void)
{
    checkForUpdate(MayflyOnboardBatt::sensorLastUpdated);
    return MayflyOnboardBatt::sensorValue_battery;
}




// The constructor - needs to reference the super-class constructor
MayflyFreeRam::MayflyFreeRam(void)
  : SensorBase(-1, -1, F("EnviroDIYMayfly"), F("Free SRAM"), F("Bit"), F("FreeRam"))
{}

// The location of the sensor on the Mayfly
String MayflyFreeRam::getSensorLocation(void)
{
    sensorLocation = "AtMega1284P";
    return sensorLocation;
}

// How to update the onboard sensors
bool MayflyFreeRam::update(void)
{
    // Used only for debugging - can be removed
      extern int __heap_start, *__brkval;
      int v;
      MayflyFreeRam::sensorValue_freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
      MayflyFreeRam::sensorLastUpdated = millis();

    // Return true when finished
    return true;
}

float MayflyFreeRam::getValue(void)
{
    checkForUpdate(MayflyFreeRam::sensorLastUpdated);
    return MayflyFreeRam::sensorValue_freeRam;
}
