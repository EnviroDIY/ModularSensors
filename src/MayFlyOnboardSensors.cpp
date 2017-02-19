/*
 *MayFlyOnboardSensors.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
*/

#include <Arduino.h>
#include "MayFlyOnboardSensors.h"
#include <Sodaq_DS3231.h>



// The constructor - what happens when initializing the onboard sensors
// only need to tell it the battery pin
MayFlyOnboardSensors::MayFlyOnboardSensors(void)
{
    _batteryPin = A6;
}

// The sensor name
String MayFlyOnboardSensors::getSensorName(void)
{
    sensorName = F("EnviroDIYMayfly");
    return sensorName;
}

// The location of the sensor on the Mayfly
String MayFlyOnboardSensors::getSensorLocation(void)
{
    sensorLocation = String(_batteryPin);
    return sensorLocation;
}

// The static variables that need to be updated
float MayFlyOnboardSensors::sensorValue_temp = 0;
float MayFlyOnboardSensors::sensorValue_battery = 0;

// How to update the onboard sensors
bool MayFlyOnboardSensors::update(void)
{
    // Get the temperature from the Mayfly's real time clock
    rtc.convertTemperature();  //convert current temperature into registers
    float tempVal = rtc.getTemperature();
    MayFlyOnboardSensors::sensorValue_temp = tempVal;

    // Get the battery voltage
    float rawBattery = analogRead(_batteryPin);
    MayFlyOnboardSensors::sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;

    // Prints for debugging
    Serial.print(F("------updated "));
    Serial.print(getSensorName());
    Serial.println(F(" sensor------"));

    // Return true when finished
    return true;
}




String MayFlyOnboardTemp::getVarName(void)
{
    varName = F("temperatureDatalogger");
    return varName;
}

String MayFlyOnboardTemp::getVarUnit(void)
{
    String unit = F("degreeCelsius");
    return unit;
}

float MayFlyOnboardTemp::getValue(void)
{
    return sensorValue_temp;
}

String MayFlyOnboardTemp::getDreamHost(void)
{
    String column = F("BoardTemp");
    return column;
}




String MayFlyOnboardBatt::getVarName(void)
{
    varName = F("batteryVoltage");
    return varName;
}


String MayFlyOnboardBatt::getVarUnit(void)
{
    unit = F("Volt");
    return unit;
}

float MayFlyOnboardBatt::getValue(void)
{
    return sensorValue_battery;
}

String MayFlyOnboardBatt::getDreamHost(void)
{
    String column = F("Battery");
    return column;
}
