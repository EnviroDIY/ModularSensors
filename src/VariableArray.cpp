/*
 *VariableArray.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the variable array class.
*/

#include "VariableArray.h"

    // Initialization - cannot do this in constructor arduino has issues creating
    // instances of classes with non-empty constructors
    void VariableArray::init(int variableCount, Variable *variableList[])
    {
        MS_DBG(F("Initializing variable array with "), variableCount, F(" variables...\n"));
        _variableCount = variableCount;
        _variableList = variableList;

        // Create a masking array with the unique sensors
        _uniqueSensorMask[_variableCount] = {true,};
        createUniqueSensorMask();

        MS_DBG(F("   ... Success!\n"));
    }

    // Functions to return information about the list

    // This counts and returns the number of sensors
    int VariableArray::getSensorCount(void)
    {
        int numSensors = 1;
        // Check for unique sensors
        for (int i = 0; i < _variableCount; i++)
        {
            if (_uniqueSensorMask[i]) numSensors++;
        }
        return numSensors;
    }

    // Public functions for interfacing with a list of sensors
    // This sets up all of the sensors in the list
    bool VariableArray::setupSensors(void)
    {
        bool success = true;

        MS_DBG(F("Beginning setup for sensors and variables...\n"));

        // First power all of the sensors
        MS_DBG(F("Powering up sensors for setup.\n"));
        sensorsPowerUp();

        // Now run all the set-up functions
        MS_DBG(F("Running setup functions.\n"));
        for (int i = 0; i < _variableCount; i++)
        {
            // Make 5 attempts to contact the sensor before giving up
            bool sensorSuccess = false;
            int setupTries = 0;
            if (_uniqueSensorMask[i]) // Skip non-unique sensors
            {
                while(setupTries < 5 and !sensorSuccess)
                {
                    delay(10);
                    sensorSuccess = _variableList[i]->parentSensor->setup();
                    setupTries++;
                }
                if (!sensorSuccess) MS_DBG(F("   ... Set up of "), _variableList[i]->getVarCode(), F(" failed!\n"));
                else MS_DBG(F("   ... Set up of "), _variableList[i]->getVarCode(), F(" succeeded.\n"));
                success &= sensorSuccess;
            }
        }

        // Power down all sensor;
        sensorsPowerDown();

        // Now attach all of the variables to their parents
        MS_DBG(F("Attaching variables to their parent sensors.\n"));
        for (int i = 0; i < _variableCount; i++){
            success &= _variableList[i]->setup();
        }

        if (success)
            MS_DBG(F("   ... Success!\n"));
        return success;
    }

    // This wakes sensors (ie, gives power)
    void VariableArray::sensorsPowerUp(void)
    {
        MS_DBG(F("Powering up sensors...\n"));
        for (int i = 0; i < _variableCount; i++)
        {
            if (_uniqueSensorMask[i]) // Skip non-unique sensors
            {
                MS_DBG(F("   ... Powering up "), _variableList[i]->getVarCode(), F("\n"));
                _variableList[i]->parentSensor->powerUp();
            }
        }
    }

    // This wakes sensors (ie, gives power)
    bool VariableArray::sensorsWake(void)
    {
        MS_DBG(F("Waking sensors...\n"));
        for (int i = 0; i < _variableCount; i++)
        {
            if (_uniqueSensorMask[i]) // Skip non-unique sensors
            {
                MS_DBG(F("   ... Waking "), _variableList[i]->getVarCode(), F("\n"));
                success &= _variableList[i]->parentSensor->wake();
            }
        }
        return success;
    }

    // This puts sensors to sleep (ie, cuts power)
    bool VariableArray::sensorsSleep(void)
    {
        MS_DBG(F("Putting sensors to sleep...\n"));
        bool success = true;
        for (int i = 0; i < _variableCount; i++)
        {
            if (_uniqueSensorMask[i]) // Skip non-unique sensors
            {
                MS_DBG(F("   ... putting "), _variableList[i]->getVarCode(), F(" to sleep.\n"));
                success &= _variableList[i]->parentSensor->sleep();
            }
        }
        return success;
    }

    // This puts sensors to sleep (ie, cuts power)
    void VariableArray::sensorsPowerDown(void)
    {
        MS_DBG(F("Powering down sensors...\n"));
        for (int i = 0; i < _variableCount; i++)
        {
            if (_uniqueSensorMask[i]) // Skip non-unique sensors
            {
                MS_DBG(F("   ... powering down "), _variableList[i]->getVarCode(), F("\n"));
                _variableList[i]->parentSensor->powerDown();
            }
        }
    }

    // This function updates the values for any connected sensors.
    bool VariableArray::updateAllSensors(void)
    {
        bool success = true;
        bool update_success = true;
        for (uint8_t i = 0; i < _variableCount; i++)
        {
            if (_uniqueSensorMask[i]) // Skip non-unique sensors
            {
                // Prints for debugging
                MS_DBG(F("--- Going to update "));
                MS_DBG(_variableList[i]->parentSensor->getSensorName());
                MS_DBG(F(" ---\n"));

                update_success = _variableList[i]->parentSensor->update();

                // Prints for debugging
                MS_DBG(F("--- Updated "));
                MS_DBG(_variableList[i]->parentSensor->getSensorName());
                MS_DBG(F(" ---\n"));
            }
        }
        success &= update_success;
        return success;
    }

    // This function prints out the results for any connected sensors to a stream
    void VariableArray::printSensorData(Stream *stream)
    {
        for (int i = 0; i < _variableCount; i++)
        {
            stream->print(_variableList[i]->parentSensor->getSensorName());
            stream->print(F(" at "));
            stream->print(_variableList[i]->parentSensor->getSensorLocation());
            // stream->print(F(" has status "));
            // stream->print(Sensor::printStatus(_variableList[i]->parentSensor->getStatus()));
            // stream->print(F(" and reports "));
            stream->print(F(" reports "));
            stream->print(_variableList[i]->getVarName());
            stream->print(F(" is "));
            stream->print(_variableList[i]->getValueString());
            stream->print(F(" "));
            stream->print(_variableList[i]->getVarUnit());
            stream->println();
        }
    }

    // This generates a comma separated list of sensor values WITHOUT TIME STAMP
    String VariableArray::generateSensorDataCSV(void)
    {
        String csvString = F("");

        for (uint8_t i = 0; i < _variableCount; i++)
        {
            csvString += _variableList[i]->getValueString();
            if (i + 1 != _variableCount)
            {
                csvString += F(",");
            }
        }

        return csvString;
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
        //     MS_DBG(_variableList[arrayIndex]->getVarName());
        //     MS_DBG(F(" from "), sensName, F(" at "), sensLoc);
        //     MS_DBG(F(" will be used for sensor references.\n"));
        // }
        // else{
        //     MS_DBG(_variableList[arrayIndex]->getVarName());
        //     MS_DBG(F(" from "), sensName, F(" at "), sensLoc);
        //     MS_DBG(F(" will be ignored.\n"));
        // }
        return unique;
    }

    void VariableArray::createUniqueSensorMask(void)
    {
        // Check for unique sensors
        for (int i = 0; i < _variableCount; i++)
        {
            _uniqueSensorMask[i] = isLastVarFromSensor(i);
        }
    }
