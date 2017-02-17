// MayFlyOnboardSensors.cpp

#include "Arduino.h"
#include "Sensor.h"
#include "MayFlyOnboardSensors.h"
#include <Sodaq_DS3231.h>



// The constructor - what happens when initializing the onboard sensors
// only need to tell it the battery pin
MayFlyOnboardSensors::MayFlyOnboardSensors(void)
{
    _batteryPin = A6;
}

// The destructor - leave empty
MayFlyOnboardSensors::~MayFlyOnboardSensors(void) {}

// How to update the onboard sensors
bool MayFlyOnboardSensors::update(void)
{
    // Get the temperature from the Mayfly's real time clock
    rtc.convertTemperature();  //convert current temperature into registers
    float tempVal = rtc.getTemperature();
    sensorValue_temp = tempVal;

    // Get the battery voltage
    float rawBattery = analogRead(_batteryPin);
    sensorValue_battery = (3.3 / 1023.) * 1.47 * rawBattery;

    // Return true when finished
    Serial.print("updated sensor");
    return true;
}

// The sensor name
String MayFlyOnboardSensors::getSensorName(void)
{
    sensorName = F("EnviroDIY Mayfly");
    return sensorName;
}



MayFlyOnboardTemp::MayFlyOnboardTemp(void) {}
MayFlyOnboardTemp::~MayFlyOnboardTemp(void) {}

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
    return 0.1;
}


MayFlyOnboardBatt::MayFlyOnboardBatt(void) {}
MayFlyOnboardBatt::~MayFlyOnboardBatt(void) {}
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
