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

    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount = getSensorCount();

    MS_DBG(F("   ... Success!\n"));
}


// This counts and returns the number of sensors
int VariableArray::getSensorCount(void)
{
    int numSensors = 0;
    // Check for unique sensors
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) numSensors++;
    }
    MS_DBG(F("There are "), numSensors, F(" unique sensors in the group.\n"));
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

    // Check for any sensors that have been set up outside of this (ie, the modem)
    uint8_t nSensorsSetup = 0;
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            if (bitRead(_variableList[i]->parentSensor->getStatus(), 1) == 1)  // already set up
            {
                MS_DBG(F("   ... "),_variableList[i]->getVarCode(), F(" was already set up!\n"));
                nSensorsSetup++;
            }
        }
    }

    // We're going to keep looping through all of the sensors and check if each
    // one has been on long enouggh to be warmed up.  Once it has, we'll set it
    // up and increment the counter marking that's been done.
    // We keep looping until they've all been done.
    while (nSensorsSetup < _sensorCount)
    {
        for (int i = 0; i < _variableCount; i++)
        {
            bool sensorSuccess = false;
            if (isLastVarFromSensor(i)) // Skip non-unique sensors
            {
                if (_variableList[i]->parentSensor->isWarmedUp() &&  // is warmed up
                    bitRead(_variableList[i]->parentSensor->getStatus(), 1) == 0)  // and not yet set up
                {
                    sensorSuccess = _variableList[i]->parentSensor->setup();  // set it up
                    if (!sensorSuccess) MS_DBG(F("   ... Set up of "), _variableList[i]->getVarCode(), F(" failed!\n"));
                    else MS_DBG(F("   ... Set up of "), _variableList[i]->getVarCode(), F(" succeeded.\n"));
                    success &= sensorSuccess;
                    nSensorsSetup++;
                }
            }
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
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
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
    bool success = true;
    uint8_t nSensorsAwake = 0;
    // We're going to keep looping through all of the sensors and check if each
    // one has been on long enouggh to be warmed up.  Once it has, we'll wake it
    // up and increment the counter marking that's been done.
    // We keep looping until they've all been done.
    while (nSensorsAwake < _sensorCount)
    {
        for (int i = 0; i < _variableCount; i++)
        {
            if (isLastVarFromSensor(i)) // Skip non-unique sensors
            {
                if (_variableList[i]->parentSensor->isWarmedUp())
                {
                    MS_DBG(F("   ... Waking "), _variableList[i]->getVarCode(), F("\n"));
                    success &= _variableList[i]->parentSensor->wake();
                    nSensorsAwake++;
                }
            }
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
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
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
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            MS_DBG(F("   ... powering down "), _variableList[i]->getVarCode(), F("\n"));
            _variableList[i]->parentSensor->powerDown();
        }
    }
}


// This function updates the values for any connected sensors.
// Please note that this does NOT run the update functions, it instead uses
// the startSingleMeasurement and addSingleMeasurementResult functions to
// take advantage of the ability of sensors to be measuring concurrently.
bool VariableArray::updateAllSensors(void)
{
    bool success = true;

    // Clear the initial variable arrays
    MS_DBG(F("--- Clearing all results before measurements ---\n"));
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
        {
            _variableList[i]->parentSensor->clearValues();
        }
    }

    uint8_t nSensorsCompleted = 0;
    uint8_t nMeasurementsCompleted[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        nMeasurementsCompleted[i] = 0;

    while (nSensorsCompleted < _sensorCount)
    {
        for (uint8_t i = 0; i < _variableCount; i++)
        {
            // if (isLastVarFromSensor(i))
            // {
            //     Serial.print(i);
            //     Serial.print(" - ");
            //     Serial.print(_variableList[i]->parentSensor->getSensorName());
            //     Serial.print(" - status: 0b");
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 7));
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 6));
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 5));
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 4));
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 3));
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 2));
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 1));
            //     Serial.print(bitRead(_variableList[i]->parentSensor->getStatus(), 0));
            //     Serial.print(" - finished: ");
            //     Serial.print(nMeasurementsCompleted[i]);
            //     Serial.print(" of ");
            //     Serial.println(_variableList[i]->parentSensor->getNumberMeasurementsToAverage());
            // }

            if (isLastVarFromSensor(i) and _variableList[i]->parentSensor->isStable() and
                _variableList[i]->parentSensor->getNumberMeasurementsToAverage() > nMeasurementsCompleted[i])
            {
                // is powered, setup, warmed-up, awake, and stable but not yet taking measurement
                if(_variableList[i]->parentSensor->getStatus() == 0b00011111 ||
                   _variableList[i]->parentSensor->getStatus() == 0b10011111 ||  // accept if error code is flagged
                      _variableList[i]->parentSensor->getStatus() == 0b10011101)  // accept if set-up failed
                {
                    // Start a reading
                    MS_DBG(F("--- Starting reading on "));
                    MS_DBG(_variableList[i]->parentSensor->getSensorName());
                    MS_DBG(F(" ---\n"));
                    success &= _variableList[i]->parentSensor->startSingleMeasurement();
                }
                // is powered, setup, warmed-up, awake, stable, and has started and completed a measurement
                if(_variableList[i]->parentSensor->isMeasurementComplete() and
                   (_variableList[i]->parentSensor->getStatus() == 0b01111111 ||
                   _variableList[i]->parentSensor->getStatus() == 0b11111111 ||  // accept if error code is flagged
                   _variableList[i]->parentSensor->getStatus() == 0b11111101))  // accept if set-up failed
                {
                    // Get the value
                    MS_DBG(F("--- Collecting result of reading from "));
                    MS_DBG(_variableList[i]->parentSensor->getSensorName());
                    MS_DBG(F(" ---\n"));
                    success &= _variableList[i]->parentSensor->addSingleMeasurementResult();
                    nMeasurementsCompleted[i] += 1;  // increment the number of measurements that sensor has completed
                }
                // sensor has taken all the measurements it needs
                if (nMeasurementsCompleted[i] == _variableList[i]->parentSensor->getNumberMeasurementsToAverage())
                {

                    MS_DBG(F("--- Finished all readings from "));
                    MS_DBG(_variableList[i]->parentSensor->getSensorName());
                    MS_DBG(F(" ---\n"));
                    nSensorsCompleted++;
                }
            }
        }
    }

    // Average readings and notify varibles of the updates
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
        {
            MS_DBG(F("--- Averaging results from "));
            MS_DBG(_variableList[i]->parentSensor->getSensorName());
            _variableList[i]->parentSensor->averageMeasurements();
            MS_DBG(F(" ---\n"));
            MS_DBG(F("--- Notifying variables from "));
            MS_DBG(_variableList[i]->parentSensor->getSensorName());
            _variableList[i]->parentSensor->notifyVariables();
            MS_DBG(F(" ---\n"));
        }
    }
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
    return unique;
}


uint8_t VariableArray::countMaxToAverage(void)
{
    int numReps = 0;
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            numReps = max(numReps, _variableList[i]->parentSensor->getNumberMeasurementsToAverage());
        }
    }
    MS_DBG(F("The largest number of measurements to average will be "), numReps, F(".\n"));
    return numReps;
}
