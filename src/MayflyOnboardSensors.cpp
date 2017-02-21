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
MayflyOnboardSensors::MayflyOnboardSensors(int batteryPin) : SensorBase()
{
    _batteryPin = batteryPin;
}

// The sensor name
String MayflyOnboardSensors::getSensorName(void)
{
    sensorName = F("EnviroDIYMayfly");
    return sensorName;
}

// The location of the sensor on the Mayfly
String MayflyOnboardSensors::getSensorLocation(void)
{
    sensorLocation = String(_batteryPin);
    return sensorLocation;
}

// The static variables that need to be updated
float MayflyOnboardSensors::sensorValue_temp = 0;
float MayflyOnboardSensors::sensorValue_battery = 0;

// How to update the onboard sensors
bool MayflyOnboardSensors::update(void)
{
    // Get the temperature from the Mayfly's real time clock
    rtc.convertTemperature();  //convert current temperature into registers
    float tempVal = rtc.getTemperature();
    MayflyOnboardSensors::sensorValue_temp = tempVal;

    // Get the battery voltage
    float rawBattery = analogRead(_batteryPin);
    MayflyOnboardSensors::sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;
    
    // Return true when finished
    return true;
}



// The constructor - needs to reference the super-class constructor
MayflyOnboardTemp::MayflyOnboardTemp(int batteryPin) : MayflyOnboardSensors(batteryPin)
{
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
    return sensorValue_temp;
}

String MayflyOnboardTemp::getDreamHost(void)
{
    String column = F("BoardTemp");
    return column;
}




MayflyOnboardBatt::MayflyOnboardBatt(int batteryPin) : MayflyOnboardSensors(batteryPin)
{
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
    return sensorValue_battery;
}

String MayflyOnboardBatt::getDreamHost(void)
{
    String column = F("Battery");
    return column;
}
