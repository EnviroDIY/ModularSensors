/*
 *SensorBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the sensor base class.
*/

#include <Arduino.h>
#include "SensorBase.h"

// The constructor
SensorBase::SensorBase(int dataPin, int powerPin, String sensorName, String varName, String varUnit, String dreamHost)
{
    _powerPin = powerPin;
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


// This is a helper function to check if the power needs to be turned on
bool SensorBase::checkPowerOn(void)
{
    int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
    if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
    {
        // Serial.println(F("Power was off."));  // For debugging
        return false;
    }
    else
    {
        // Serial.println(F("Power was on."));  // For debugging
        return true;
    }
}

// This is a helper function to turn on sensor power
void SensorBase::powerUp(void)
{
    // Serial.println(F("Powering on Sensor"));  // For debugging
    digitalWrite(_powerPin, HIGH);
    delay(500);
}

// This is a helper function to turn off sensor power
void SensorBase::powerDown(void)
{
    // Serial.println(F("Turning off Power"));  // For debugging
    digitalWrite(_powerPin, LOW);
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS SensorBase::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);
    return SENSOR_READY;
}

// The function to return the status of a sensor
// By default, simply returns ready
SENSOR_STATUS SensorBase::getStatus(void){return SENSOR_READY;}

// The function to put a sensor to sleep
// By default, powers down and returns true
bool SensorBase::sleep(void)
{
    powerDown();
    return true;
}

// The function to wake up a sensor
// By default, powers up and returns true
bool SensorBase::wake(void)
{
    if(!checkPowerOn()){powerUp();}
    return true;
}


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


// ============================================================================
//  The class and functions for interfacing with an array of sensors.
// ============================================================================


// Constructor
SensorArray::SensorArray(int sensorCount, SensorBase *SENSOR_LIST[])
{
    _sensorCount = sensorCount;
    _sensorList = SENSOR_LIST;
};


// This sets up the sensors, generally setting pin modes and the like
bool SensorArray::setupSensors(void)
{
    bool success = true;
    bool sensorSuccess = false;
    int setupTries = 0;
    for (int i = 0; i < _sensorCount; i++)
    {
        // Make 5 attempts before giving up
        while(setupTries < 5)
        {
            sensorSuccess = _sensorList[i]->setup();
            // Prints for debugging
            if(sensorSuccess)
            {
                Serial.print(F("--- Successfully set up "));
                Serial.print(_sensorList[i]->getSensorName());
                Serial.println(F(" ---"));
                break;
            }
            else
            {
                Serial.print(F("--- Setup for  "));
                Serial.print(_sensorList[i]->getSensorName());
                Serial.println(F(" failed! ---"));
                setupTries++;
            }
        }
        success &= sensorSuccess;

        // Check for and skip the setup of any identical sensors
        for (int j = i+1; j < _sensorCount; j++)
        {
            if (_sensorList[i]->getSensorName() == _sensorList[j]->getSensorName() &&
                _sensorList[i]->getSensorLocation() == _sensorList[j]->getSensorLocation())
            {i++;}
            else {break;}
        }
    }
    return success;
}

bool SensorArray::sensorsSleep(void)
{
    // Serial.println(F("Putting sensors to sleep."));  // For debugging
    bool success = true;
    for (int i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->sleep();
    }

    return success;
}

bool SensorArray::sensorsWake(void)
{
    // Serial.println(F("Waking sensors."));  // For debugging
    bool success = true;
    for (int i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->wake();
    }

    return success;
}

// This function updates the values for any connected sensors.
bool SensorArray::updateAllSensors(void)
{
    bool success = true;
    for (uint8_t i = 0; i < _sensorCount; i++)
    {
        success &= _sensorList[i]->update();
        // Prints for debugging
        // Serial.print(F("--- Updated "));  // For debugging
        // Serial.print(_sensorList[i]->getSensorName());  // For debugging
        // Serial.print(F(" for "));  // For debugging
        // Serial.print(_sensorList[i]->getVarName());  // For debugging

        // Check for and skip the updates of any identical sensors
        for (int j = i+1; j < _sensorCount; j++)
        {
            if (_sensorList[i]->getSensorName() == _sensorList[j]->getSensorName() &&
                _sensorList[i]->getSensorLocation() == _sensorList[j]->getSensorLocation())
            {
                // Prints for debugging
                // Serial.print(F(" and "));  // For debugging
                // Serial.print(_sensorList[i+1]->getVarName());  // For debugging
                i++;
            }
            else {break;}
        }
        // Serial.println(F(" ---"));  // For Debugging
        // delay(250);  // A short delay before next sensor - is this necessary??
    }

    return success;
}

// This function prints out the results for any connected sensors to a stream
void SensorArray::printSensorData(Stream *stream)
{
    for (int i = 0; i < _sensorCount; i++)
    {
        stream->print(_sensorList[i]->getSensorName());
        stream->print(F(" attached at "));
        stream->print(_sensorList[i]->getSensorLocation());
        stream->print(F(" has status "));
        stream->print(_sensorList[i]->getStatus());
        stream->print(F(" and reports "));
        stream->print(_sensorList[i]->getVarName());
        stream->print(F(" is "));
        stream->print(_sensorList[i]->getValue());
        stream->print(F(" "));
        stream->print(_sensorList[i]->getVarUnit());
        stream->println();
    }
}
