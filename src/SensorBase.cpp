/*
 *SensorBase.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the sensor base class.
*/

#include "SensorBase.h"
#include "VariableBase.h"

// ============================================================================
//  The class and functions for interfacing with a sensor
// ============================================================================

// The constructor
Sensor::Sensor(int powerPin, int dataPin, String sensorName, int numReturnedVars, int WarmUpTime_ms)
{
    _powerPin = powerPin;
    _dataPin = dataPin;
    _sensorName = sensorName;
    _numReturnedVars = numReturnedVars;
    _WarmUpTime_ms = WarmUpTime_ms;
    _millisPowerOn = 0;

    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_VARS; i++)
    {
        variables[i] = NULL;
        sensorValues[i] = 0;
    }
}

// This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
String Sensor::getSensorLocation(void)
{
    String senseLoc = F("Pin");
    senseLoc +=String(_dataPin);
    return senseLoc;
}

// This returns the name of the sensor.
String Sensor::getSensorName(void){return _sensorName;}


// This is a helper function to check if the power needs to be turned on
bool Sensor::checkPowerOn(void)
{
    DBGS(F("Checking power status.\n"));
    if (_powerPin > 0)
    {
        int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
        if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
        {
            // DBGS(F("Power was off.\n"));
            if (_millisPowerOn != 0) _millisPowerOn = 0;
            return false;
        }
        else
        {
            // DBGS(F("Power was on.\n"));
            if (_millisPowerOn == 0) _millisPowerOn = millis();
            return true;
        }
    }
    else
    {
        DBGS(F("Power was on.\n"));
        if (_millisPowerOn == 0) _millisPowerOn = millis();
        return true;
    }
}

// This is a helper function to turn on sensor power
void Sensor::powerUp(void)
{
    if (_powerPin > 0)
    {
        // DBGS(F("Powering on Sensor with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, HIGH);
        _millisPowerOn = millis();
    }
}

// This is a helper function to turn off sensor power
void Sensor::powerDown(void)
{
    if (_powerPin > 0)
    {
        // DBGS(F("Turning off Power\n"));
        digitalWrite(_powerPin, LOW);
        _millisPowerOn = 0;
    }
}

// This is a helper function to wait that enough time has passed for the sensor
// to warm up before taking readings
void Sensor::waitForWarmUp(void)
{
    if (_WarmUpTime_ms != 0)
    {
        if (millis() > _millisPowerOn + _WarmUpTime_ms)  // already ready
        {
            DBGS(F("Sensor already warmed up!\n"));
        }
        else if (millis() > _millisPowerOn)  // just in case millis() has rolled over
        {
            DBGS(F("Waiting "), (millis() + _WarmUpTime_ms - _millisPowerOn), F("ms for sensor warm-up\n"));
            while((millis() - _millisPowerOn) < _WarmUpTime_ms){}
        }
        else  // if we get really unlucky and are measuring as millis() rolls over
        {
            DBGS(F("Waiting 2000ms for sensor warm-up\n"));
            while(millis() < 2000){}
        }
    }
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS Sensor::setup(void)
{
    if (_powerPin > 0) pinMode(_powerPin, OUTPUT);
    if (_dataPin > 0) pinMode(_dataPin, INPUT_PULLUP);

    DBGS(F("Set up "));
    DBGS(getSensorName());
    DBGS(F(" attached at "));
    DBGS(getSensorLocation());
    DBGS(F(" which can return up to "));
    DBGS(_numReturnedVars);
    DBGS(F(" variable[s].\n"));

    return SENSOR_READY;
}

// The function to return the status of a sensor
// By default, simply returns ready
SENSOR_STATUS Sensor::getStatus(void){return SENSOR_READY;}

String Sensor::printStatus(SENSOR_STATUS stat)
{
    String status;
    switch(stat)
    {
        case SENSOR_ERROR: status = F("Errored"); break;
        case SENSOR_READY: status = F("Ready"); break;
        case SENSOR_WAITING: status = F("Waiting"); break;
        case SENSOR_UNKNOWN: status = F("Unknown"); break;
    }
    return status;
}

// The function to put a sensor to sleep
// By default, powers down and returns true
bool Sensor::sleep(void)
{
    powerDown();
    return true;
}

// The function to wake up a sensor
// By default, powers up and returns true
bool Sensor::wake(void)
{
    if(!checkPowerOn()){powerUp();}
    return true;
}

void Sensor::registerVariable(int varNum, Variable* var)
{
    variables[varNum] = var;
    DBGS(F("... Registration for "));
    DBGS(var->getVarName());
    DBGS(F(" accepted.\n"));
}

void Sensor::notifyVariables(void)
{
    DBGS(F("Notifying registered variables.\n"));
    // Make note of the last time updated
    sensorLastUpdated = millis();

    // Notify variables of update
    for (int i = 0; i < _numReturnedVars; i++)
    {
        if (variables[i] != NULL)  // Bad things happen if try to update nullptr
        {
            DBGS(F("Sending value update to variable "));
            DBGS(i);
            DBGS(F(" which is "));
            DBGS(variables[i]->getVarName());
            DBGS(F("...   "));
            variables[i]->onSensorUpdate(this);
        }
        // else DBGS(F("Null pointer\n"));
    }
}


// This function checks if a sensor needs to be updated or not
bool Sensor::checkForUpdate(unsigned long sensorLastUpdated)
{
    // DBGS(F("It has been "), (millis() - sensorLastUpdated)/1000);
    // DBGS(F(" seconds since the sensor value was checked\n"));
    if ((millis() > 60000 and millis() > sensorLastUpdated + 60000) or sensorLastUpdated == 0)
    {
        // DBGS(F("Value out of date, updating\n"));
        return(update());
    }
    else return(true);
}


// This function just empties the value array
void Sensor::clearValues(void)
{
    DBGS(F("Clearing sensor value array.\n"));
    for (int i = 0; i < _numReturnedVars; i++)
    { sensorValues[i] =  0; }
}
