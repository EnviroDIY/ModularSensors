/*
 *VariableArray.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the variable array class.
*/

#include "VariableArray.h"
#include "SensorBase.h"
#include "VariableBase.h"

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
int VariableArray::getVariableCount(void){return _variableCount;}

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
    // if (unique){
    //     Serial.print(_variableList[arrayIndex]->getVarName());
    //     Serial.print(F(" from "));
    //     Serial.print(sensName);
    //     Serial.print(F(" at "));
    //     Serial.print(sensLoc);
    //     Serial.println(F(" will be used for sensor references."));
    // }
    // else{
    //     Serial.print(_variableList[arrayIndex]->getVarName());
    //     Serial.print(F(" from "));
    //     Serial.print(sensName);
    //     Serial.print(F(" at "));
    //     Serial.print(sensLoc);
    //     Serial.println(F(" will be ignored."));
    // }
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
            // Serial.print(F("--- Going to update "));  // For debugging
            // Serial.print(_variableList[i]->parentSensor->getSensorName());  // For debugging
            // Serial.println(F(" ---"));  // For Debugging

            update_success = _variableList[i]->parentSensor->update();

            // Prints for debugging
            // Serial.print(F("--- Updated "));  // For debugging
            // Serial.print(_variableList[i]->parentSensor->getSensorName());  // For debugging
            // Serial.println(F(" ---"));  // For Debugging
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
        stream->print(Sensor::printStatus(_variableList[i]->parentSensor->getStatus()));
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
