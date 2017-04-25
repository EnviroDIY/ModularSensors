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
Sensor::Sensor(int powerPin, int dataPin, String sensorName, int numReturnedVars)
{
    _powerPin = powerPin;
    _dataPin = dataPin;
    _sensorName = sensorName;
    _numReturnedVars = numReturnedVars;

    // Clear arrays
    variables[MAX_NUMBER_VARS] = {0};
    sensorValues[MAX_NUMBER_VARS] = {0};
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
void Sensor::powerUp(void)
{
    // Serial.println(F("Powering on Sensor"));  // For debugging
    digitalWrite(_powerPin, HIGH);
    delay(500);
}

// This is a helper function to turn off sensor power
void Sensor::powerDown(void)
{
    // Serial.println(F("Turning off Power"));  // For debugging
    digitalWrite(_powerPin, LOW);
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns ready
SENSOR_STATUS Sensor::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);

    Serial.print(F("Set up "));  // for debugging
    Serial.print(getSensorName());  // for debugging
    Serial.print(F(" attached at "));  // for debugging
    Serial.print(getSensorLocation());  // for debugging
    Serial.print(F(" which can return up to "));  // for debugging
    Serial.print(_numReturnedVars);  // for debugging
    Serial.println(F(" variable[s]."));  // for debugging

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
    Serial.print(F("... Registration for "));  // for debugging
    Serial.print(var->getVarName());  // for debugging
    Serial.println(F(" accepted."));  // for debugging
}

void Sensor::notifyVariables(void)
{
    // Serial.println(F("Notifiying registered variables."));
    // Make note of the last time updated
    sensorLastUpdated = millis();

    // Notify variables of update
    for (int i = 0; i < _numReturnedVars; i++){
        // Serial.print(F("Sending value update to "));  // for debugging
        // Serial.print(variables[i]->getVarName());  // for debugging
        // Serial.print(F("...   "));  // for debugging
        variables[i]->onSensorUpdate(this);
    }
}


// This function checks if a sensor needs to be updated or not
bool Sensor::checkForUpdate(unsigned long sensorLastUpdated)
{
    // Serial.print(F("It has been "));  // For debugging
    // Serial.print((millis() - sensorLastUpdated)/1000);  // For debugging
    // Serial.println(F(" seconds since the sensor value was checked"));  // For debugging
    if ((millis() > 60000 and millis() > sensorLastUpdated + 60000) or sensorLastUpdated == 0)
    {
        // Serial.println(F("Value out of date, updating"));  // For debugging
        return(update());
    }
    else return(true);
}


// This function just empties the value array
void Sensor::clearValues(void)
{
    // Serial.println(F("Clearing sensor value array."));
    for (int i = 0; i < _numReturnedVars; i++)
    { sensorValues[i] =  0; }
}
