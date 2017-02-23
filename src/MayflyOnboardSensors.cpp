/*
 *MayflyOnboardSensors.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
*/

#include <Arduino.h>
#include "MayflyOnboardSensors.h"
#include <Sodaq_DS3231.h>



// The constructor - needs to reference the super-class constructor
// only need to tell it the battery pin
MayflyOnboardSensors::MayflyOnboardSensors(char const *version) :SensorBase()
{
    _version = version;
}

// The sensor name
String MayflyOnboardSensors::getSensorName(void)
{
    sensorName = F("EnviroDIYMayfly");
    return sensorName;
}

// The static variables that need to be updated
float MayflyOnboardTemp::sensorValue_temp = 0;
float MayflyOnboardBatt::sensorValue_battery = 0;
float MayflyFreeRam::sensorValue_freeRam = 0;
unsigned long MayflyOnboardTemp::sensorLastUpdated = 0;
unsigned long MayflyOnboardBatt::sensorLastUpdated = 0;
unsigned long MayflyFreeRam::sensorLastUpdated = 0;



// The constructor - needs to reference the super-class constructor
MayflyOnboardTemp::MayflyOnboardTemp(char const *version) : MayflyOnboardSensors(version) {}

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

// The location of the sensor on the Mayfly
String MayflyOnboardTemp::getSensorLocation(void)
{
    sensorLocation = "DS3231";
    return sensorLocation;
}

String MayflyOnboardTemp::getVarName(void)
{
    varName = F("temperatureDatalogger");
    return varName;
}

String MayflyOnboardTemp::getVarUnit(void)
{
    String unit = F("degreeCelsius");
    return unit;
}

float MayflyOnboardTemp::getValue(void)
{
    if (millis() > 30000 and millis() > MayflyOnboardTemp::sensorLastUpdated + 30000)
        {MayflyOnboardTemp::update();}
    return MayflyOnboardTemp::sensorValue_temp;
}

String MayflyOnboardTemp::getDreamHost(void)
{
    String column = F("BoardTemp");
    return column;
}





// The constructor - needs to reference the super-class constructor
MayflyOnboardBatt::MayflyOnboardBatt(char const *version) : MayflyOnboardSensors(version){}

// How to update the onboard sensors
bool MayflyOnboardBatt::update(void)
{
    if (strcmp(_version, "v0.3") == 0)
    {
        // Set the pin to read the battery voltage
        int _batteryPin = A6;
        // Get the battery voltage
        float rawBattery = analogRead(_batteryPin);
        MayflyOnboardBatt::sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
        MayflyOnboardBatt::sensorLastUpdated = millis();
    }

    // Return true when finished
    return true;
}

// The location of the sensor on the Mayfly
String MayflyOnboardBatt::getSensorLocation(void)
{
    sensorLocation = String(_batteryPin);
    return sensorLocation;
}

String MayflyOnboardBatt::getVarName(void)
{
    varName = F("batteryVoltage");
    return varName;
}


String MayflyOnboardBatt::getVarUnit(void)
{
    unit = F("Volt");
    return unit;
}

float MayflyOnboardBatt::getValue(void)
{
    if (millis() > 30000 and millis() > MayflyOnboardBatt::sensorLastUpdated + 30000)
        {MayflyOnboardBatt::update();}
    return MayflyOnboardBatt::sensorValue_battery;
}

String MayflyOnboardBatt::getDreamHost(void)
{
    String column = F("Battery");
    return column;
}




// The constructor - needs to reference the super-class constructor
MayflyFreeRam::MayflyFreeRam(char const *version) : MayflyOnboardSensors(version) {}

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

// The location of the sensor on the Mayfly
String MayflyFreeRam::getSensorLocation(void)
{
    sensorLocation = "AtMega1284P";
    return sensorLocation;
}

String MayflyFreeRam::getVarName(void)
{
    varName = F("Free SRAM");
    return varName;
}


String MayflyFreeRam::getVarUnit(void)
{
    unit = F("Bit");
    return unit;
}

float MayflyFreeRam::getValue(void)
{
    if (millis() > 30000 and millis() > MayflyFreeRam::sensorLastUpdated + 30000)
        {MayflyFreeRam::update();}
    return MayflyFreeRam::sensorValue_freeRam;
}

String MayflyFreeRam::getDreamHost(void)
{
    String column = F("FreeRam");
    return column;
}
