/*
 *SensorBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the sensor base class.
*/

#include <Arduino.h>
#include "SensorBase.h"

// The constructor
SensorBase::SensorBase(int dataPin, String sensorName, String varName, String varUnit, String dreamHost)
{
    _dataPin = dataPin;
    _sensorName = sensorName;
    _varName = varName;
    _varUnit = varUnit;
    _dreamHost = dreamHost;
}

// This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
String SensorBase::getSensorLocation(void){return String(_dataPin);};
// This returns the name of the sensor.
String SensorBase::getSensorName(void){return _sensorName;};
// This returns the variable's name using http://vocabulary.odm2.org/variablename/
String SensorBase::getVarName(void){return _varName;};
// This returns the variable's unit using http://vocabulary.odm2.org/units/
String SensorBase::getVarUnit(void){return _varUnit;};
// This returns the dreamhost PHP tag - for old SWRC dreamhost system
String SensorBase::getDreamHost(void){return _dreamHost;};


// The function to set up connection to a sensor.
// By default, returns ready
SENSOR_STATUS SensorBase::setup(void){return SENSOR_READY;}

// The function to return the status of a sensor
// By default, returns ready
SENSOR_STATUS SensorBase::getStatus(void){return SENSOR_READY;}

// The function to put a sensor to sleep
// By default, returns true
bool SensorBase::sleep(void){return true;}

// The function to wake up a sensor
// By default, returns true
bool SensorBase::wake(void){return true;}


// This function checks if a sensor needs to be updated or not
bool SensorBase::checkForUpdate(unsigned long sensorLastUpdated)
{
    if ((millis() > 60000 and millis() > sensorLastUpdated + 60000) or sensorLastUpdated == 0)
    {
        Serial.print(F("It has been "));
        Serial.print((millis() - sensorLastUpdated)/1000);
       // Serial.println(F(" seconds since the sensor value was checked"));  // For debugging
       // Serial.println(F("Value out of date, updating"));  // For debugging
        return(update());
    }
    else return(true);
}
