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
    arrayOfVars = variableList;

    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount = getSensorCount();
}

// This counts and returns the number of calculated variables
int VariableArray::getCalculatedVariableCount(void)
{
    int numCalc = 0;
    // Check for unique sensors
    for (int i = 0; i < _variableCount; i++)
    {
        if (arrayOfVars[i]->isCalculated) numCalc++;
    }
    MS_DBG(F("There are "), numCalc,
           F(" calculated variables in the group.\n"));
    return numCalc;
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
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a sensor.
bool VariableArray::setupSensors(void)
{
    bool success = true;

    MS_DBG(F("Beginning setup for sensors and variables...\n"));

    // First power all of the sensors
    // NOTE:  Because we are running this *before* running each sensor's setup
    // function, this may actually fail to power a sensors if the pin mode for
    // that sensor's power pin is not correct.  The pin modes are set *during*
    // most sensor's set-up function.  For this reason, each sensor that requires
    // power for setup should have a powerUp() and waitForWarmup() written into
    // its setup function.
    // But, for logger boards that have been programmed more than once, the pin
    // mode from the previous power up of the MCU is usually retained so chances
    // are high that this will actually power up the sensors. If this does
    // successfully power up the sensors, it saves us all of the waitForWarmup()
    // time and prevents the power from flickering on and off during setup in
    // cases where many sensors are powered on the same circuit.
    // MS_DBG(F("Powering up sensors for setup.\n"));
    // sensorsPowerUp();

    // Now run all the set-up functions
    MS_DBG(F("Running setup functions.\n"));

    // Check for any sensors that have been set up outside of this (ie, the modem)
    uint8_t nSensorsSetup = 0;
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0) == 1)  // already set up
            {
                MS_DBG(F("   ... "), arrayOfVars[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(arrayOfVars[i]->getParentSensorLocation());
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
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0) == 0)  // not yet set up
                {
                    if (arrayOfVars[i]->parentSensor->isWarmedUp())  // is warmed up
                    {
                        MS_DBG(F("   ... Set up of "));
                        MS_DBG(arrayOfVars[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                        MS_DBG(F(" ...\n"));

                        sensorSuccess = arrayOfVars[i]->parentSensor->setup();  // set it up
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
    // sensorsPowerDown();

    // Now attach all of the variables to their parents
    // MS_DBG(F("Attaching variables to their parent sensors.\n"));
    // for (int i = 0; i < _variableCount; i++){
    //     success &= arrayOfVars[i]->setup();
    // }

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
            MS_DBG(arrayOfVars[i]->getParentSensorName());
            MS_DBG(F(" at "));
            MS_DBG(arrayOfVars[i]->getParentSensorLocation());
            MS_DBG('\n');

            arrayOfVars[i]->parentSensor->powerUp();
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
            if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) == 1)  // already awake
            {
                MS_DBG(arrayOfVars[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(arrayOfVars[i]->getParentSensorLocation());
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
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) == 0)  // No attempts yet made to wake the sensor up
                {
                    if (arrayOfVars[i]->parentSensor->isWarmedUp())  // already warmed up
                    {
                        MS_DBG(F("   ... Wake up of "));
                        MS_DBG(arrayOfVars[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                        MS_DBG(F(" ...\n"));

                        // Make a single attempt to wake the sensor after it is warmed up
                        bool sensorSuccess = arrayOfVars[i]->parentSensor->wake();
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
            MS_DBG(arrayOfVars[i]->getParentSensorName());
            MS_DBG(F(" at "));
            MS_DBG(arrayOfVars[i]->getParentSensorLocation());

            bool sensorSuccess = arrayOfVars[i]->parentSensor->sleep();
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
            MS_DBG(arrayOfVars[i]->getParentSensorName());
            MS_DBG(F(" at "));
            MS_DBG(arrayOfVars[i]->getParentSensorLocation());
            MS_DBG('\n');

            arrayOfVars[i]->parentSensor->powerDown();
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
    MS_DBG(F("----->> Clearing all results arrays before taking new measurements. ...\n"));
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
        {
            arrayOfVars[i]->parentSensor->clearValues();
        }
    }
    MS_DBG(F("   ... Complete. <<-----\n"));

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
            if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) == 0 ||  // No attempt made to wake the sensor up
                bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4) == 0 )  // OR Wake up failed
            {
                MS_DBG(i, F(" --->> "));
                MS_DBG(arrayOfVars[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                MS_DBG(F(" isn't awake/active!  No measurements will be taken! <<--- "), i, '\n');

                // Set the number of measurements already equal to whatever total
                // number requested to ensure the sensor is skipped in further loops.
                nMeasurementsCompleted[i] = arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage();
                // Bump up the finished count.
                nSensorsCompleted++;
            }
        }
    }

    while (nSensorsCompleted < _sensorCount)
    {
        for (uint8_t i = 0; i < _variableCount; i++)
        {
            /***
            // THIS IS PURELY FOR DEEP DEBUGGING OF THE TIMING!
            // Leave this whole section commented out unless you want excessive
            // printouts (ie, thousands of lines) of the timing information!!
            if (isLastVarFromSensor(i) and
                arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage() > nMeasurementsCompleted[i])
            {
                MS_DBG(i);
                MS_DBG(F(" - "));
                MS_DBG(arrayOfVars[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                MS_DBG(F(" - millis: "));
                MS_DBG(millis());
                MS_DBG(F(" - status: 0b"));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 7));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 6));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 2));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 1));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0));
                MS_DBG(F(" - measurement #"));
                MS_DBG(nMeasurementsCompleted[i] + 1);
                MS_DBG('\n');
            }
            // END CHUNK FOR DEBUGGING!
            ***/

            // Only do checks on sensors that still have measurements to finish
            if (isLastVarFromSensor(i) and
                arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage() > nMeasurementsCompleted[i])
            {
                // first, make sure the sensor is stable
                if ( arrayOfVars[i]->parentSensor->isStable())
                {

                    // now, if the sensor is not currently measuring...
                    if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5) == 0)  // NO attempt yet to start a measurement
                    {
                            // Start a reading
                            MS_DBG(i, '.', nMeasurementsCompleted[i]+1,
                                   F(" --->> Starting reading "));
                            MS_DBG(nMeasurementsCompleted[i]+1);
                            MS_DBG(F(" on "));
                            MS_DBG(arrayOfVars[i]->getParentSensorName());
                            MS_DBG(F(" at "));
                            MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                            MS_DBG(F(" -\n"));

                            bool sensorSuccess_start = arrayOfVars[i]->parentSensor->startSingleMeasurement();
                            success &= sensorSuccess_start;

                            if (sensorSuccess_start) MS_DBG(F("   ... Success. <<--- "), i, '.', nMeasurementsCompleted[i]+1, '\n');
                            else MS_DBG(F("   ... Failed! <<--- "), i, '.', nMeasurementsCompleted[i]+1, '\n');
                    }

                    // otherwise, it is currently measuring so...
                    // if a measurement is finished, get the result and tick up the number of finished measurements
                    // NOTE:  isMeasurementComplete() will immediately return true if the attempt to start a
                    // measurement failed (bit 6 not set).  In that case, the addSingleMeasurementResult()
                    // will be "adding" -9999 values.
                    if(arrayOfVars[i]->parentSensor->isMeasurementComplete())
                    {
                        // Get the value
                        MS_DBG(i, '.', nMeasurementsCompleted[i]+1,
                               F(" --->> Collected result of reading "));
                        MS_DBG(F(" from "));
                        MS_DBG(arrayOfVars[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                        MS_DBG(F(" --\n"));

                        bool sensorSuccess_result = arrayOfVars[i]->parentSensor->addSingleMeasurementResult();
                        success &= sensorSuccess_result;
                        nMeasurementsCompleted[i] += 1;  // increment the number of measurements that sensor has completed

                        if (sensorSuccess_result) MS_DBG(F("   ... Success. <<--- "), i, '.', nMeasurementsCompleted[i], '\n');
                        else MS_DBG(F("   ... Failed! <<--- "), i, '.', nMeasurementsCompleted[i], '\n');
                    }

                }

                // if all the measurements are done, mark the whole sensor as done
                if (nMeasurementsCompleted[i] == arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage())
                {
                    MS_DBG(F("--- Finished all measurements from "));
                    MS_DBG(arrayOfVars[i]->getParentSensorName());
                    MS_DBG(F(" at "));
                    MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                    MS_DBG(F(" ---\n"));

                    nSensorsCompleted++;
                    MS_DBG(F("*****--- "), nSensorsCompleted, F(" sensors now complete ---*****\n"));
                }
            }
        }
    }

    // Average measurements and notify varibles of the updates
    MS_DBG(F("----->> Averaging results and notifying all variables. ...\n"));
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i))
        {
            // MS_DBG(F("--- Averaging results from "));
            // MS_DBG(arrayOfVars[i]->getParentSensorName());
            arrayOfVars[i]->parentSensor->averageMeasurements();
            // MS_DBG(F(" ---\n"));
            // MS_DBG(F("--- Notifying variables from "));
            // MS_DBG(arrayOfVars[i]->getParentSensorName());
            arrayOfVars[i]->parentSensor->notifyVariables();
            // MS_DBG(F(" ---\n"));
        }
    }
    MS_DBG(F("   ... Complete. <<-----\n"));

    return success;
}


// This function is an even more complete version of the updateAllSensors
// function - it handles power up/down and wake/sleep.
bool VariableArray::completeUpdate(void)
{
    bool success = true;
    uint8_t nSensorsCompleted = 0;

    // Create an array with the unique-ness value (so we can skip the function calls later)
    bool lastSensorVariable[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        lastSensorVariable[i] = isLastVarFromSensor(i);

    // Create an array for the number of measurements already completed and set all to zero
    uint8_t nMeasurementsCompleted[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        nMeasurementsCompleted[i] = 0;

    // Create an array for the number of measurements to average (another short cut)
    uint8_t nMeasurementsToAverage[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        nMeasurementsToAverage[i] = arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage();

    // Create an array of the power pins
    int8_t powerPins[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        powerPins[i] = arrayOfVars[i]->parentSensor->getPowerPin();

    // Create an array of the last variable on each power pin
    bool lastPinVariable[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        lastPinVariable[i] = 0;
    // Create an array containing the index of the power pin in the powerPins array
    int8_t powerPinIndex[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        powerPinIndex[i] = 0;
    // Create an array to tell us how many measurements must be taken
    // before all the sensors attached to a power pin are done
    uint8_t nMeasurementsOnPin[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        nMeasurementsOnPin[i] = nMeasurementsToAverage[i];

    // Now correctly populate the previous three arrays
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (!lastSensorVariable[i])
        {
            lastPinVariable[i] = 0;
            // MS_DBG(i, F(" isn't the last variable on pin, not last variable in sensor\n"));
            nMeasurementsOnPin[i] = 0;
            i++;
        }
        for (int j = i + 1; j < _variableCount; j++)
        {
            if (powerPins[j] == powerPins[i])
            {
                lastPinVariable[i] = 0;
                // MS_DBG(i, F(" isn't the last variable on pin, matches "), j, '\n');
                if (!lastSensorVariable[j]) nMeasurementsOnPin[j] = 0;
                nMeasurementsOnPin[j] += nMeasurementsOnPin[i];
                nMeasurementsOnPin[i] = 0;
                i++;
            }
        }
        lastPinVariable[i] = 1;
        // MS_DBG(i, F(" is the last variable on pin.\n"));
        powerPinIndex[i] = i;
    }
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if(!lastPinVariable[i])
        {
            for (int j = i + 1; j < _variableCount; j++)
            {
                if (powerPins[j] == powerPins[i])
                {
                    powerPinIndex[i] = powerPinIndex[j];
                }
            }
        }
    }

    /***
    // This is just for debugging
    // uint8_t arrayPositions[_variableCount];
    // for (uint8_t i = 0; i < _variableCount; i++)
    //     arrayPositions[i] = i;
    // MS_DBG(F("----------------------------------\n"));
    // MS_DBG(F("arrayPositions:\t\t\t"));
    // prettyPrintArray(arrayPositions);
    // MS_DBG(F("lastSensorVariable:\t\t"));
    // prettyPrintArray(lastSensorVariable);
    // MS_DBG(F("nMeasurementsToAverage:\t\t"));
    // prettyPrintArray(nMeasurementsToAverage);
    // MS_DBG(F("powerPins:\t\t\t"));
    // prettyPrintArray(powerPins);
    // MS_DBG(F("lastPinVariable:\t\t"));
    // prettyPrintArray(lastPinVariable);
    // MS_DBG(F("nMeasurementsOnPin:\t\t"));
    // prettyPrintArray(nMeasurementsOnPin);
    // MS_DBG(F("powerPinIndex:\t\t\t"));
    // prettyPrintArray(powerPinIndex);
    ***/

    // Another array for the number of measurements already completed per power pin
    uint8_t nCompletedOnPin[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++)
        nCompletedOnPin[i] = 0;


    // Clear the initial variable arrays
    MS_DBG(F("----->> Clearing all results arrays before taking new measurements. ...\n"));
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (lastSensorVariable[i])
        {
            arrayOfVars[i]->parentSensor->clearValues();
        }
    }
    MS_DBG(F("   ... Complete. <<-----\n"));

    // power up all of the sensors together
    MS_DBG(F("----->> Powering up all sensors together. ...\n"));
    sensorsPowerUp();
    MS_DBG(F("   ... Complete. <<-----\n"));

    while (nSensorsCompleted < _sensorCount)
    {
        for (uint8_t i = 0; i < _variableCount; i++)
        {
            /***
            // THIS IS PURELY FOR DEEP DEBUGGING OF THE TIMING!
            // Leave this whole section commented out unless you want excessive
            // printouts (ie, thousands of lines) of the timing information!!
            if (isLastVarFromSensor(i) and
                nMeasurementsToAverage[i] > nMeasurementsCompleted[i])
            {
                MS_DBG(i);
                MS_DBG(F(" - "));
                MS_DBG(arrayOfVars[i]->getParentSensorName());
                MS_DBG(F(" at "));
                MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                MS_DBG(F(" - millis: "));
                MS_DBG(millis());
                MS_DBG(F(" - status: 0b"));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 7));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 6));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 2));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 1));
                MS_DBG(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0));
                MS_DBG(F(" - measurement #"));
                MS_DBG(nMeasurementsCompleted[i] + 1);
                MS_DBG('\n');
            }
            // END CHUNK FOR DEBUGGING!
            ***/

            // Only do checks on sensors that still have measurements to finish
            if (lastSensorVariable[i] && nMeasurementsToAverage[i] > nMeasurementsCompleted[i])
            {
                // If no attempts have been made to wake the sensor, wake it up when it's ready
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) == 0)
                {
                    if (arrayOfVars[i]->parentSensor->isWarmedUp())  // already warmed up
                    {
                        MS_DBG(i, F(" --->> Waking "));
                        MS_DBG(arrayOfVars[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                        MS_DBG(F(" ...\n"));

                        // Make a single attempt to wake the sensor after it is warmed up
                        bool sensorSuccess_wake = arrayOfVars[i]->parentSensor->wake();
                        success &= sensorSuccess_wake;

                        if (sensorSuccess_wake) MS_DBG(F("   ... Success. <<--- "), i, '\n');
                        else MS_DBG(F("   ... Failed! <<--- "), i, '\n');
                    }
                }

                // If attempts were made to wake the sensor, but they failed
                // then we're just bumping up the number of measurements to completion
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) == 1 &&
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4) == 0)
                {
                    MS_DBG(i, F(" --->> "));
                    MS_DBG(arrayOfVars[i]->getParentSensorName());
                    MS_DBG(F(" at "));
                    MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                    MS_DBG(F(" did not wake up! No measurements will be taken! <<--- "), i, '\n');
                    // Set the number of measurements already equal to whatever total
                    // number requested to ensure the sensor is skipped in further loops.
                    nMeasurementsCompleted[i] = nMeasurementsToAverage[i];
                    // increment the number of measurements that the power pin has completed
                    nCompletedOnPin[powerPinIndex[i]] += nMeasurementsToAverage[i];
                }

                // If the sensor was successfully awoken/activated...
                // .. make sure the sensor is stable
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4) == 1 &&
                    arrayOfVars[i]->parentSensor->isStable())
                {

                    // If no attempt has yet been made to start a measurement, start one
                    if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5) == 0)
                    {
                            // Start a reading
                            MS_DBG(i, '.', nMeasurementsCompleted[i]+1,
                                   F(" --->> Starting reading "));
                            MS_DBG(nMeasurementsCompleted[i]+1);
                            MS_DBG(F(" on "));
                            MS_DBG(arrayOfVars[i]->getParentSensorName());
                            MS_DBG(F(" at "));
                            MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                            MS_DBG(F(" ...\n"));

                            bool sensorSuccess_start = arrayOfVars[i]->parentSensor->startSingleMeasurement();
                            success &= sensorSuccess_start;

                            if (sensorSuccess_start) MS_DBG(F("   ... Success. <<--- "), i, '.', nMeasurementsCompleted[i]+1, '\n');
                            else MS_DBG(F("   ... Failed! <<--- "), i, '.', nMeasurementsCompleted[i]+1, '\n');
                    }

                    // If a measurement is finished, get the result and tick up
                    // the number of finished measurements.  We aren't bothering
                    // to check if the measurement start was successful,
                    // isMeasurementComplete() will do that and we stil want the
                    // addSingleMeasurementResult() function to fill in the -9999
                    // results for a failed measurement.
                    if(arrayOfVars[i]->parentSensor->isMeasurementComplete())
                    {
                        // Get the value
                        MS_DBG(i, '.', nMeasurementsCompleted[i]+1,
                               F(" --->> Collected result of reading "));
                        MS_DBG(nMeasurementsCompleted[i]+1);
                        MS_DBG(F(" from "));
                        MS_DBG(arrayOfVars[i]->getParentSensorName());
                        MS_DBG(F(" at "));
                        MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                        MS_DBG(F(" ...\n"));

                        bool sensorSuccess_result = arrayOfVars[i]->parentSensor->addSingleMeasurementResult();
                        success &= sensorSuccess_result;
                        nMeasurementsCompleted[i] += 1;  // increment the number of measurements that sensor has completed
                        nCompletedOnPin[powerPinIndex[i]] += 1;  // increment the number of measurements that the power pin has completed

                        if (sensorSuccess_result) MS_DBG(F("   ... Success. <<--- "), i, '.', nMeasurementsCompleted[i], '\n');
                        else MS_DBG(F("   ... Failed! <<--- "), i, '.', nMeasurementsCompleted[i], '\n');
                    }

                }

                // If all the measurements are done
                if (nMeasurementsCompleted[i] == nMeasurementsToAverage[i])
                {
                    MS_DBG(i, F(" --->> Finished all measurements from "));
                    MS_DBG(arrayOfVars[i]->getParentSensorName());
                    MS_DBG(F(" at "));
                    MS_DBG(arrayOfVars[i]->getParentSensorLocation());
                    MS_DBG(F(", putting it to sleep. ...\n"));

                    // Put the completed sensor to sleep
                    bool sensorSuccess_sleep = arrayOfVars[i]->parentSensor->sleep();
                    success &= sensorSuccess_sleep;

                    if (sensorSuccess_sleep) MS_DBG(F("   ... Success. <<--- "), i, '\n');
                    else MS_DBG(F("   ... Failed! <<--- "), i, '\n');

                    // Now cut the power, if ready, to this sensors and all that share the pin
                    if (nCompletedOnPin[powerPinIndex[i]] == nMeasurementsOnPin[powerPinIndex[i]])
                    {
                        for (uint8_t k = 0; k < _variableCount; k++)
                        {
                            if (powerPinIndex[k] == powerPinIndex[i] && lastSensorVariable[k] )
                            {
                                MS_DBG(k, F(" --->> Powering down "));
                                MS_DBG(arrayOfVars[k]->getParentSensorName());
                                MS_DBG(F(" at "));
                                MS_DBG(arrayOfVars[k]->getParentSensorLocation());

                                arrayOfVars[k]->parentSensor->powerDown();
                                MS_DBG(F("   ... Complete. <<--- "), k, '\n');
                            }
                        }
                    }

                    nSensorsCompleted++;  // mark the whole sensor as done
                    MS_DBG(F("*****--- "), nSensorsCompleted, F(" sensors now complete ---*****\n"));
                }
            }
        }
    }

    // Average measurements and notify varibles of the updates
    MS_DBG(F("----->> Averaging results and notifying all variables. ...\n"));
    for (uint8_t i = 0; i < _variableCount; i++)
    {
        if (lastSensorVariable[i])
        {
            // MS_DBG(F("--- Averaging results from "));
            // MS_DBG(arrayOfVars[i]->getParentSensorName());
            arrayOfVars[i]->parentSensor->averageMeasurements();
            // MS_DBG(F(" ---\n"));
            // MS_DBG(F("--- Notifying variables from "));
            // MS_DBG(arrayOfVars[i]->getParentSensorName());
            arrayOfVars[i]->parentSensor->notifyVariables();
            // MS_DBG(F(" ---\n"));
        }
    }
    MS_DBG(F("   ... Complete. <<-----\n"));

    return success;
}


// This function prints out the results for any connected sensors to a stream
// Calculated variable results will be included
void VariableArray::printSensorData(Stream *stream)
{
    for (int i = 0; i < _variableCount; i++)
    {
        if (arrayOfVars[i]->isCalculated)
        {
            stream->print(arrayOfVars[i]->getVarName());
            stream->print(F(" is calculated to be "));
            stream->print(arrayOfVars[i]->getValueString());
            stream->print(F(" "));
            stream->print(arrayOfVars[i]->getVarUnit());
            stream->println();
        }
        else
        {
            stream->print(arrayOfVars[i]->getParentSensorName());
            stream->print(F(" at "));
            stream->print(arrayOfVars[i]->getParentSensorLocation());
            // stream->print(F(" with status 0b"));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 7));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 6));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 2));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 1));
            // stream->print(bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0));
            stream->print(F(" reports "));
            stream->print(arrayOfVars[i]->getVarName());
            stream->print(F(" is "));
            stream->print(arrayOfVars[i]->getValueString());
            stream->print(F(" "));
            stream->print(arrayOfVars[i]->getVarUnit());
            stream->println();
        }
    }
}

/***
// These generate some helpful comma-separated lists of variable information
// This is a PRE-PROCESSOR MACRO to speed up generating header rows
// Again, THIS IS NOT A FUNCTION, it is a pre-processor macro
#define makeVarListCSV(function) \
    { \
        String csvString = ""; \
        for (uint8_t i = 0; i < _variableCount; i++) \
        { \
            csvString += arrayOfVars[i]->function; \
            if (i + 1 != _variableCount) \
            { \
                csvString += F(","); \
            } \
        } \
        return csvString; \
    }
// This generates a comma separated list of sensor values WITHOUT TIME STAMP
String VariableArray::generateSensorDataCSV(void){makeVarListCSV(getValueString())};
// This generates a comma separated list of parent sensor names
String VariableArray::listParentSensorNames(void){makeVarListCSV(getParentSensorName())};
// This generates a comma separated list of variable names
String VariableArray::listVariableNames(void){makeVarListCSV(getVarName())};
// This generates a comma separated list of variable units
String VariableArray::listVariableUnits(void){makeVarListCSV(getVarUnit())};
// This generates a comma separated list of variable codes
String VariableArray::listVariableCodes(void){makeVarListCSV(getVarCode())};
// This generates a comma separated list of variable UUID's
String VariableArray::listVariableUUIDs(void){makeVarListCSV(getVarUUID())};


// These generate some helpful comma-separated lists of variable information
// This is a PRE-PROCESSOR MACRO to speed up generating header rows
// Again, THIS IS NOT A FUNCTION, it is a pre-processor macro
#define streamVarListCSV(function) \
    { \
        for (uint8_t i = 0; i < _variableCount; i++) \
        { \
            stream->print(arrayOfVars[i]->function); \
            if (i + 1 != _variableCount) \
            { \
                stream->print(','); \
            } \
        } \
    }
void VariableArray::streamSensorDataCSV(Stream *stream){streamVarListCSV(getValueString())};
void VariableArray::streamParentSensorNames(Stream *stream){streamVarListCSV(getParentSensorName())};
void VariableArray::streamVariableNames(Stream *stream){streamVarListCSV(getVarName())};
void VariableArray::streamVariableUnits(Stream *stream){streamVarListCSV(getVarUnit())};
void VariableArray::streamVariableCodes(Stream *stream){streamVarListCSV(getVarCode())};
void VariableArray::streamVariableUUIDs(Stream *stream){streamVarListCSV(getVarUUID())};
***/

// Check for unique sensors
bool VariableArray::isLastVarFromSensor(int arrayIndex)
{
    // MS_DBG(F("Checking if "), arrayOfVars[arrayIndex]->getVarName(), F(" ("),
    //        arrayIndex, F(") is the last variable from a sensor..."));

    // Calculated variables are never the last variable from a sensor, simply
    // because the don't come from a sensor at all.
    if (arrayOfVars[arrayIndex]->isCalculated)
    {
        // MS_DBG(F("   ... Nope, it's calculated!\n"));
        return false;
    }

    else
    {
        String sensName = arrayOfVars[arrayIndex]->getParentSensorName();
        String sensLoc = arrayOfVars[arrayIndex]->getParentSensorLocation();
        bool unique = true;
        for (int j = arrayIndex + 1; j < _variableCount; j++)
        {
            if (sensName == arrayOfVars[j]->getParentSensorName() &&
                sensLoc == arrayOfVars[j]->getParentSensorLocation())
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


// Count the maximum number of measurements needed from a single sensor for the
// requested averaging
uint8_t VariableArray::countMaxToAverage(void)
{
    int numReps = 0;
    for (int i = 0; i < _variableCount; i++)
    {
        if (isLastVarFromSensor(i)) // Skip non-unique sensors
        {
            numReps = max(numReps, arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage());
        }
    }
    MS_DBG(F("The largest number of measurements to average will be "), numReps, F(".\n"));
    return numReps;
}
