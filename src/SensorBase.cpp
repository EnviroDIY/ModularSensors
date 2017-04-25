/*
 *Sensor.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the sensor base class.
*/

#include <Arduino.h>
#include "SensorBase.h"


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
    Serial.println(F("Notifiying registered variables."));
    // Make note of the last time updated
    sensorLastUpdated = millis();

    // Notify variables of update
    for (int i = 0; i < _numReturnedVars; i++){
        Serial.print(F("Sending value update to "));  // for debugging
        Serial.println(variables[i]->getVarName());  // for debugging
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
    for (int i = 0; i < _numReturnedVars; i++)
    { sensorValues[i] =  0; }
}



// ============================================================================
//  The class and functions for interfacing with a specific variable.
// ============================================================================

// The constructor
Variable::Variable(Sensor *parentSense, int varNum, String varName, String varUnit,
                   unsigned int decimalResolution, String dreamHost)
{
    parentSensor = parentSense;
    _varNum = varNum;
    _varName = varName;
    _varUnit = varUnit;
    _decimalResolution = decimalResolution;
    _dreamHost = dreamHost;
}

void Variable::attachSensor(int varNum, Sensor *parentSense) {
    Serial.print(F("Attempting to register to "));  // for debugging
    Serial.print(parentSense->getSensorName());  // for debugging
    Serial.print(F(" attached at "));  // for debugging
    Serial.print(parentSense->getSensorLocation());  // for debugging
    Serial.print(F("...   "));  // for debugging
    parentSense->registerVariable(varNum, this);
}

bool Variable::setup(void)
{
    attachSensor(_varNum, parentSensor);
    return true;
}

void Variable::onSensorUpdate(Sensor *parentSense)
{
    sensorValue = parentSense->sensorValues[_varNum];
    Serial.print(F("Received value update of "));  // for debugging
    Serial.println(sensorValue);  // for debugging
}

// This returns the variable's name using http://vocabulary.odm2.org/variablename/
String Variable::getVarName(void){return _varName;}

// This returns the variable's unit using http://vocabulary.odm2.org/units/
String Variable::getVarUnit(void){return _varUnit;}

// This returns the dreamhost PHP tag - for old SWRC dreamhost system
String Variable::getDreamHost(void){return _dreamHost;}

// This returns the current value of the variable as a float
float Variable::getValue(void)
{
    parentSensor->checkForUpdate(parentSensor->sensorLastUpdated);
    return sensorValue;
}

// This returns the current value of the variable as a string
// with the correct number of significant figures
String Variable::getValueString(void)
{
    // Need this because otherwise get extra spaces in strings from int
    if (_decimalResolution == 0)
    {
        int val = int(getValue());
        return String(val);
    }
    else
    {return String(getValue(), _decimalResolution);}
}


// ============================================================================
//  The class and functions for interfacing with an array of variables.
// ============================================================================

// Constructor
void VariableArray::init(int variableCount, Variable *variableList[])
{
    Serial.println(F("Initializing varible array"));  // for debugging
    _variableCount = variableCount;
    _variableList = variableList;
};

// This just returns the number of variables
int VariableArray::getVariableCount(void)
{return _variableCount;}

// This counts and returns the number of sensors
int VariableArray::getSensorCount(void)
{
    int numSensors = 1;
    // Check for unique sensors
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) numSensors++;
    }
    return numSensors;
}

bool VariableArray::isLastVarFromSensor(int arrayIndex)
{
    // Check for unique sensors
    String sensName = _variableList[arrayIndex]->parentSensor->getSensorName();
    String sensLoc = _variableList[arrayIndex]->parentSensor->getSensorLocation();
    bool unique = true;
    for (int j = arrayIndex + 1; j < _variableCount; j++)
    {
        if (sensName == _variableList[j]->parentSensor->getSensorName() &&
            sensLoc == _variableList[j]->parentSensor->getSensorLocation())
        {
            unique = false;
            break;
        }
    }
    // Prints for debugging
    if (unique){
        Serial.print(_variableList[arrayIndex]->getVarName());
        Serial.print(F(" from "));
        Serial.print(sensName);
        Serial.print(F(" at "));
        Serial.print(sensLoc);
        Serial.println(F(" will be used for sensor references."));
    }
    else{
        Serial.print(_variableList[arrayIndex]->getVarName());
        Serial.print(F(" from "));
        Serial.print(sensName);
        Serial.print(F(" at "));
        Serial.print(sensLoc);
        Serial.println(F(" will be ignored."));
    }
    return unique;
}

// This sets up the sensors, generally setting pin modes and the like
bool VariableArray::setupSensors(void)
{
    bool success = true;
    bool sensorSuccess = false;
    int setupTries = 0;

    // First setup the sensors
    for (int i = 0; i < _variableCount; i++)
    {
        // Make 5 attempts to contact the sensor before giving up
        while(setupTries < 5)
        {
            // Setting up the sensors for all variables whether they are repeats
            // or not.  This means setting up some sensors multiple times, but
            // this should be OK because setup is only run in the setup, not
            // repeatedly. It is not possible to check for repeated sensors in
            // the variable list until after the sensors have all been
            // registered and then all of the variables attached.
            sensorSuccess = _variableList[i]->parentSensor->setup();

            if(sensorSuccess) break;
            else setupTries++;
        }
        success &= sensorSuccess;
    }

    // Now attach all of the variables to their parents
    for (int i = 0; i < _variableCount; i++){
        success &= _variableList[i]->setup();
    }
    return success;
}

bool VariableArray::sensorsSleep(void)
{
    // Serial.println(F("Putting sensors to sleep."));  // For debugging
    bool success = true;
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
            success &= _variableList[i]->parentSensor->sleep();
    }
    return success;
}

bool VariableArray::sensorsWake(void)
{
    // Serial.println(F("Waking sensors."));  // For debugging
    bool success = true;
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
            success &= _variableList[i]->parentSensor->wake();
    }
    return success;
}

// This function updates the values for any connected sensors.
bool VariableArray::updateAllSensors(void)
{
    bool success = true;
    bool update_success = true;
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
        {
            // Prints for debugging
            Serial.print(F("--- Going to update "));  // For debugging
            Serial.print(_variableList[i]->parentSensor->getSensorName());  // For debugging
            Serial.println(F(" ---"));  // For Debugging
            delay(250);
            update_success = _variableList[i]->parentSensor->update();
            delay(250);
            // Prints for debugging
            Serial.print(F("--- Updated "));  // For debugging
            Serial.print(_variableList[i]->parentSensor->getSensorName());  // For debugging
            Serial.println(F(" ---"));  // For Debugging
        }
    }
    success &= update_success;
    return success;
}

// This function prints out the results for any connected sensors to a stream
void VariableArray::printSensorData(Stream *stream /* = &Serial*/)
{
    for (int i = 0; i < _variableCount; i++)
    {
        stream->print(_variableList[i]->parentSensor->getSensorName());
        stream->print(F(" attached at "));
        stream->print(_variableList[i]->parentSensor->getSensorLocation());
        stream->print(F(" has status "));
        stream->print(_variableList[i]->parentSensor->getStatus());
        stream->print(F(" and reports "));
        stream->print(_variableList[i]->getVarName());
        stream->print(F(" is "));
        stream->print(_variableList[i]->getValueString());
        stream->print(F(" "));
        stream->print(_variableList[i]->getVarUnit());
        stream->println();
    }
}

// This function generates a comma separated list of sensor values
String VariableArray::generateSensorDataCSV(void)
{
    String csvString = F("");

    for (uint8_t i = 0; i < _variableCount; i++)
    {
        csvString += _variableList[i]->getValueString();
        if (i + 1 != _variableCount)
        {
            csvString += F(", ");
        }
    }

    return csvString;
}
