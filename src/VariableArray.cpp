/*
 *VariableArray.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the variable array class.
*/

#include "VariableArray.h"


// Constructor
VariableArray::VariableArray(int variableCount, Variable *variableList[])
{
    _variableCount = variableCount;
    _variableList = variableList;

    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount = getSensorCount();

    MS_DBG(F("Initializing variable array with "), variableCount, F(" variables...\n"));
    MS_DBG(F("   ... Success!\n"));
}


// This counts and returns the number of sensors
int VariableArray::getSensorCount(void)
{
    int numSensors = 0;
    int numCalc = 0;
    // Check for unique sensors
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) numSensors++;
        if (_variableList[i]->isCalculated) numCalc++;
    }
    MS_DBG(F("There are "), numSensors, F(" unique sensors and "), numCalc,
           F(" calculated variablesin the group.\n"));
    return numSensors;
}


// Public functions for interfacing with a list of sensors
// This sets up all of the sensors in the list
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a sensor.
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
                MS_DBG(F("   ... "), _variableList[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(_variableList[i]->getParentSensorLocation());
                MS_DBG(F(" was already set up!\n"));

                nSensorsSetup++;
            }
        }
    }

    // We're going to keep looping through all of the sensors and check if each
    // one has been on long enough to be warmed up.  Once it has, we'll set it
    // up and increment the counter marking that's been done.
    // We keep looping until they've all been done.
    while (nSensorsSetup < _sensorCount)
    {
        for (int i = 0; i < _variableCount; i++)
        {
            bool sensorSuccess = false;
            if (isLastVarFromSensor(i)) // Skip non-unique sensors
            {
                if (bitRead(_variableList[i]->parentSensor->getStatus(), 1) == 0)  // not yet set up
                {
                    if (_variableList[i]->parentSensor->isWarmedUp())  // is warmed up
                    {
                        MS_DBG(F("   ... Set up of "));
                        MS_DBG(_variableList[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(_variableList[i]->getParentSensorLocation());
                        MS_DBG(F(" ...\n"));

                        sensorSuccess = _variableList[i]->parentSensor->setup();  // set it up
                        success &= sensorSuccess;
                        nSensorsSetup++;

                        if (!sensorSuccess) MS_DBG(F("   ... failed! ...\n"));
                        else MS_DBG(F("   ... succeeded. ...\n"));
                    }
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


// This powers up the sensors
// There's no checking or waiting here, just turning on pins
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a sensor.
void VariableArray::sensorsPowerUp(void)
{
    MS_DBG(F("Powering up sensors...\n"));
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            MS_DBG(F("   ... Powering up "));
            MS_DBG(_variableList[i]->getParentSensorName());
            MS_DBG(F(" at "));
            MS_DBG(_variableList[i]->getParentSensorLocation());
            MS_DBG('\n');

            _variableList[i]->parentSensor->powerUp();
        }
    }
}


// This wakes/activates the sensors
// Before a sensor is "awoken" we have to make sure it's had time to warm up
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a sensor.
bool VariableArray::sensorsWake(void)
{
    MS_DBG(F("Waking sensors...\n"));
    bool success = true;
    uint8_t nSensorsAwake = 0;

    // Check for any sensors that are awake outside of being sent a "wake" command
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            if (bitRead(_variableList[i]->parentSensor->getStatus(), 3) == 1)  // already awake
            {
                MS_DBG(_variableList[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(_variableList[i]->getParentSensorLocation());
                MS_DBG(F(" was already awake.\n"));
                nSensorsAwake++;
            }
        }
    }

    // We're going to keep looping through all of the sensors and check if each
    // one has been on long enough to be warmed up.  Once it has, we'll wake it
    // up and increment the counter marking that's been done.
    // We keep looping until they've all been done.
    while (nSensorsAwake < _sensorCount)
    {
        for (int i = 0; i < _variableCount; i++)
        {
            if (isLastVarFromSensor(i)) // Skip non-unique sensors
            {
                if (bitRead(_variableList[i]->parentSensor->getStatus(), 3) == 0)  // NOT yet awake
                {
                    if (_variableList[i]->parentSensor->isWarmedUp())  // already warmed up
                    {
                        MS_DBG(F("   ... Wake up of "));
                        MS_DBG(_variableList[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(_variableList[i]->getParentSensorLocation());
                        MS_DBG(F(" ...\n"));

                        // Make a single attempt to wake the sensor after it is warmed up
                        bool sensorSuccess = _variableList[i]->parentSensor->wake();
                        success &= sensorSuccess;
                        // We increment up the number of sensors awake/active, even
                        // if the wake up command failed!
                        nSensorsAwake++;

                        if (sensorSuccess) MS_DBG(F("   ... succeeded. ...\n"));
                        else MS_DBG(F("   ... failed! ...\n"));
                    }
                }
            }
        }
    }
    return success;
}


// This puts sensors to sleep
// We're not waiting for anything to be ready, we're just sending the command
// to put it to sleep no matter what its current state is.
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a sensor.
bool VariableArray::sensorsSleep(void)
{
    MS_DBG(F("Putting sensors to sleep...\n"));
    bool success = true;
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            MS_DBG(F("   ... "));
            MS_DBG(_variableList[i]->getParentSensorName());
            MS_DBG(F(" at "));
            MS_DBG(_variableList[i]->getParentSensorLocation());

            bool sensorSuccess = _variableList[i]->parentSensor->sleep();
            success &= sensorSuccess;

            if (sensorSuccess) MS_DBG(F(" successfully put to sleep.\n"));
            else MS_DBG(F(" failed to sleep!\n"));
        }
    }
    return success;
}


// This cuts power to the sensors
// We're not waiting for anything to be ready, we're just cutting power.
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a sensor.
void VariableArray::sensorsPowerDown(void)
{
    MS_DBG(F("Powering down sensors...\n"));
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            MS_DBG(F("   ... powering down "));
            MS_DBG(_variableList[i]->getParentSensorName());
            MS_DBG(F(" at "));
            MS_DBG(_variableList[i]->getParentSensorLocation());
            MS_DBG('\n');

            _variableList[i]->parentSensor->powerDown();
        }
    }
}


// This function updates the values for any connected sensors.
// Please note that this does NOT run the update functions, it instead uses
// the startSingleMeasurement and addSingleMeasurementResult functions to
// take advantage of the ability of sensors to be measuring concurrently.
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a sensor.
bool VariableArray::updateAllSensors(void)
{
    bool success = true;

    // Clear the initial variable arrays
    MS_DBG(F("--- Clearing all results arrays before taking new measurements ---\n"));
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

    // Check for any sensors that didn't wake up and mark them as "complete" so
    // they will be skipped in further looping.
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            if (bitRead(_variableList[i]->parentSensor->getStatus(), 3) == 0)  // NOT awake/activated
            {
                MS_DBG(_variableList[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(_variableList[i]->getParentSensorLocation());
                MS_DBG(F(" isn't awake/active!  No readings will be taken!\n"));

                // Set the number of readings already equal to whatever total
                // number requested to ensure the sensor is skipped in further loops.
                nMeasurementsCompleted[i] = _variableList[i]->parentSensor->getNumberMeasurementsToAverage();
                // Bump up the finished count.
                nSensorsCompleted++;
            }
        }
    }

    while (nSensorsCompleted < _sensorCount)
    {
        for (uint8_t i = 0; i < _variableCount; i++)
        {
            // THIS IS PURELY FOR DEEP DEBUGGING OF THE TIMING!
            // Leave this whole section commented out unless you want excessive
            // printouts (ie, thousands of lines) of the timing information!!
            // if (isLastVarFromSensor(i) and
            //     _variableList[i]->parentSensor->getNumberMeasurementsToAverage() > nMeasurementsCompleted[i])
            // {
            //     _variableList[i]->parentSensor->updateStatusBits(true);
            //     MS_DBG(i);
            //     MS_DBG(F(" - "));
            //     MS_DBG(_variableList[i]->getParentSensorName());
            //     MS_DBG(F(" at "));
            //     MS_DBG(_variableList[i]->getParentSensorLocation());
            //     MS_DBG(F(" - millis: "));
            //     MS_DBG(millis());
            //     MS_DBG(F(" - status: 0b"));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 7));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 6));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 5));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 4));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 3));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 2));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 1));
            //     MS_DBG(bitRead(_variableList[i]->parentSensor->getStatus(), 0));
            //     MS_DBG(F(" - measurement #"));
            //     MS_DBG(nMeasurementsCompleted[i] + 1);
            //     MS_DBG('\n');
            // }
            // END CHUNK FOR DEBUGGING!

            // Only do checks on sensors that still have readings to finish
            if (isLastVarFromSensor(i) and
                _variableList[i]->parentSensor->getNumberMeasurementsToAverage() > nMeasurementsCompleted[i])
            {
                // first, make sure the sensor is stable
                if ( _variableList[i]->parentSensor->isStable())
                {

                // now, if the sensor is not currently measuring...
                    if (bitRead(_variableList[i]->parentSensor->getStatus(), 5) == 0)  // NOT currently measuring
                    {
                            // Start a reading
                            MS_DBG(F("- Starting reading "));
                            MS_DBG(nMeasurementsCompleted[i]+1);
                            MS_DBG(F(" on "));
                            MS_DBG(_variableList[i]->getParentSensorName());
                            MS_DBG(F(" at "));
                            MS_DBG(_variableList[i]->getParentSensorLocation());
                            MS_DBG(F(" -\n"));

                            bool sensorSuccess_start = _variableList[i]->parentSensor->startSingleMeasurement();
                            success &= sensorSuccess_start;

                            if (sensorSuccess_start) MS_DBG(F("- Success -\n"));
                            else MS_DBG(F("- Failed! -\n"));
                    }

                    // otherwise, it is currently measuring so...
                    // if a measurement is finished, get the result and tick up the number of finished readings
                    if(_variableList[i]->parentSensor->isMeasurementComplete())
                    {
                        // Get the value
                        MS_DBG(F("-- Collected result of reading "));
                        MS_DBG(nMeasurementsCompleted[i]+1);
                        MS_DBG(F(" from "));
                        MS_DBG(_variableList[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(_variableList[i]->getParentSensorLocation());
                        MS_DBG(F(" --\n"));

                        bool sensorSuccess_result = _variableList[i]->parentSensor->addSingleMeasurementResult();
                        success &= sensorSuccess_result;
                        nMeasurementsCompleted[i] += 1;  // increment the number of measurements that sensor has completed

                        if (sensorSuccess_result) MS_DBG(F("-- Success --\n"));
                        else MS_DBG(F("-- Failed! --\n"));
                    }

                }

                // if all the readings are done, mark the whole sensor as done
                if (nMeasurementsCompleted[i] == _variableList[i]->parentSensor->getNumberMeasurementsToAverage())
                {
                    MS_DBG(F("--- Finished all readings from "));
                    MS_DBG(_variableList[i]->getParentSensorName());
                    MS_DBG(F(" at "));
                    MS_DBG(_variableList[i]->getParentSensorLocation());
                    MS_DBG(F(" ---\n"));

                    nSensorsCompleted++;
                }
            }
        }
    }

    // Average readings and notify varibles of the updates
    MS_DBG(F("--- Averaging results and notifying variables ---\n"));
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
        {
            // MS_DBG(F("--- Averaging results from "));
            // MS_DBG(_variableList[i]->getParentSensorName());
            _variableList[i]->parentSensor->averageMeasurements();
            // MS_DBG(F(" ---\n"));
            // MS_DBG(F("--- Notifying variables from "));
            // MS_DBG(_variableList[i]->getParentSensorName());
            _variableList[i]->parentSensor->notifyVariables();
            // MS_DBG(F(" ---\n"));
        }
    }
    return success;
}


// This function prints out the results for any connected sensors to a stream
// Calculated variable results will be included
void VariableArray::printSensorData(Stream *stream)
{
    for (int i = 0; i < _variableCount; i++)
    {
        if (_variableList[i]->isCalculated)
        {
            stream->print(_variableList[i]->getVarName());
            stream->print(F(" is calculated to be "));
            stream->print(_variableList[i]->getValueString());
            stream->print(F(" "));
            stream->print(_variableList[i]->getVarUnit());
            stream->println();
        }
        else
        {
            stream->print(_variableList[i]->getParentSensorName());
            stream->print(F(" at "));
            stream->print(_variableList[i]->getParentSensorLocation());
            // stream->print(F(" with status 0b"));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 7));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 6));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 5));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 4));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 3));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 2));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 1));
            // stream->print(bitRead(_variableList[i]->parentSensor->getStatus(), 0));
            stream->print(F(" reports "));
            stream->print(_variableList[i]->getVarName());
            stream->print(F(" is "));
            stream->print(_variableList[i]->getValueString());
            stream->print(F(" "));
            stream->print(_variableList[i]->getVarUnit());
            stream->println();
        }
    }
}


// This generates a comma separated list of sensor values WITHOUT TIME STAMP
// Calculated variable results will be included
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


// Check for unique sensors
bool VariableArray::isLastVarFromSensor(int arrayIndex)
{
    // MS_DBG(F("Checking if "), _variableList[arrayIndex]->getVarName(), F(" ("),
    //        arrayIndex, F(") is the last variable from a sensor..."));

    // Calculated variables are never the last variable from a sensor, simply
    // because the don't come from a sensor at all.
    if (_variableList[arrayIndex]->isCalculated)
    {
        // MS_DBG(F("   ... Nope, it's calculated!\n"));
        return false;
    }

    else
    {
        String sensName = _variableList[arrayIndex]->getParentSensorName();
        String sensLoc = _variableList[arrayIndex]->getParentSensorLocation();
        bool unique = true;
        for (int j = arrayIndex + 1; j < _variableCount; j++)
        {
            if (sensName == _variableList[j]->getParentSensorName() &&
                sensLoc == _variableList[j]->getParentSensorLocation())
            {
                unique = false;
                // MS_DBG(F("   ... Nope, there are others after it!\n"));
                break;
            }
        }
        // if (unique) MS_DBG(F("   ... Yes, it is!\n"));
        return unique;
    }
}


// Count the maximum number of readings needed from a single sensor for the
// requested averaging
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
