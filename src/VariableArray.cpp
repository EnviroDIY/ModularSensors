/**
 * @file VariableArray.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the VariableArray class.
 */

#include "VariableArray.h"


// Constructors
VariableArray::VariableArray() {}
VariableArray::VariableArray(uint8_t variableCount, Variable* variableList[])
    : arrayOfVars(variableList),
      _variableCount(variableCount) {
    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount         = getSensorCount();
}
VariableArray::VariableArray(uint8_t variableCount, Variable* variableList[],
                             const char* uuids[])
    : arrayOfVars(variableList),
      _variableCount(variableCount) {
    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount         = getSensorCount();
    matchUUIDs(uuids);
}

// Destructor
VariableArray::~VariableArray() {}

void VariableArray::begin(uint8_t variableCount, Variable* variableList[],
                          const char* uuids[]) {
    _variableCount = variableCount;
    arrayOfVars    = variableList;

    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount         = getSensorCount();
    matchUUIDs(uuids);
    checkVariableUUIDs();
}
void VariableArray::begin(uint8_t variableCount, Variable* variableList[]) {
    _variableCount = variableCount;
    arrayOfVars    = variableList;

    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount         = getSensorCount();
    checkVariableUUIDs();
}
void VariableArray::begin() {
    _maxSamplestoAverage = countMaxToAverage();
    _sensorCount         = getSensorCount();
    checkVariableUUIDs();
}

// This counts and returns the number of calculated variables
uint8_t VariableArray::getCalculatedVariableCount(void) {
    uint8_t numCalc = 0;
    // Check for unique sensors
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (arrayOfVars[i]->isCalculated) numCalc++;
    }
    MS_DBG(F("There are"), numCalc, F("calculated variables in the group."));
    return numCalc;
}


// This counts and returns the number of sensors
uint8_t VariableArray::getSensorCount(void) {
    uint8_t numSensors = 0;
    // Check for unique sensors
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)) numSensors++;
    }
    return numSensors;
}

// This matches UUID's from an array of pointers to the variable array
void VariableArray::matchUUIDs(const char* uuids[]) {
    for (uint8_t i = 0; i < _variableCount; i++) {
        arrayOfVars[i]->setVarUUID(uuids[i]);
    }
}

// Public functions for interfacing with a list of sensors
// This sets up all of the sensors in the list
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a
// sensor.
bool VariableArray::setupSensors(void) {
    bool success = true;

    // #ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    // bool deepDebugTiming = true;
    // #else
    // bool deepDebugTiming = false;
    // #endif

    MS_DBG(F("Beginning setup for sensors and variables..."));

    // Now run all the set-up functions
    MS_DBG(F("Running sensor setup functions."));

    // Check for any sensors that have been set up outside of this (ie, the
    // modem)
    uint8_t nSensorsSetup = 0;
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)  // Skip non-unique sensors
            && bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0) ==
                1  // already set up
        ) {
            MS_DBG(F("   "), arrayOfVars[i]->getParentSensorNameAndLocation(),
                   F("was already set up!"));

            nSensorsSetup++;
        }
    }

    // We're going to keep looping through all of the sensors and check if each
    // one has been on long enough to be warmed up.  Once it has, we'll set it
    // up and increment the counter marking that's been done.
    // We keep looping until they've all been done.
    while (nSensorsSetup < _sensorCount) {
        for (uint8_t i = 0; i < _variableCount; i++) {
            if (isLastVarFromSensor(i)  // Skip non-unique sensors
                && bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0) ==
                    0  // only set up if it has not yet been set up
            ) {
                MS_DBG(F("    Set up of"),
                       arrayOfVars[i]->getParentSensorNameAndLocation(),
                       F("..."));

                bool sensorSuccess =
                    arrayOfVars[i]->parentSensor->setup();  // set it up
                success &= sensorSuccess;
                nSensorsSetup++;

                if (!sensorSuccess) {
                    MS_DBG(F("        ... setup failed!"));
                } else {
                    MS_DBG(F("        ... setup succeeded."));
                }
            }
        }
    }

    if (success) { MS_DBG(F("... Success!")); }

    return success;
}


// This powers up the sensors
// There's no checking or waiting here, just turning on pins
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a
// sensor.
void VariableArray::sensorsPowerUp(void) {
    MS_DBG(F("Powering up sensors..."));
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)) {  // Skip non-unique sensors
            MS_DBG(F("    Powering up"),
                   arrayOfVars[i]->getParentSensorNameAndLocation());

            arrayOfVars[i]->parentSensor->powerUp();
        }
    }
}


// This wakes/activates the sensors
// Before a sensor is "awoken" we have to make sure it's had time to warm up
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a
// sensor.
bool VariableArray::sensorsWake(void) {
    MS_DBG(F("Waking sensors..."));
    bool    success       = true;
    uint8_t nSensorsAwake = 0;

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    bool deepDebugTiming = true;
#else
    bool deepDebugTiming = false;
#endif

    // Check for any sensors that are awake outside of being sent a "wake"
    // command
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)  // Skip non-unique sensors
            && bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) ==
                1  // already attempted to wake
        ) {
            MS_DBG(F("    Wake up of"),
                   arrayOfVars[i]->getParentSensorNameAndLocation(),
                   F("has already been attempted."));
            nSensorsAwake++;
        }
    }

    // We're going to keep looping through all of the sensors and check if each
    // one has been on long enough to be warmed up.  Once it has, we'll wake it
    // up and increment the counter marking that's been done.
    // We keep looping until they've all been done.
    while (nSensorsAwake < _sensorCount) {
        for (uint8_t i = 0; i < _variableCount; i++) {
            if (isLastVarFromSensor(i)  // Skip non-unique sensors
                && bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) ==
                    0  // If no attempts yet made to wake the sensor up
                && arrayOfVars[i]->parentSensor->isWarmedUp(
                       deepDebugTiming)  // and if it is already warmed up
            ) {
                MS_DBG(F("    Wake up of"),
                       arrayOfVars[i]->getParentSensorNameAndLocation(),
                       F("..."));

                // Make a single attempt to wake the sensor after it is
                // warmed up
                bool sensorSuccess = arrayOfVars[i]->parentSensor->wake();
                success &= sensorSuccess;
                // We increment up the number of sensors awake/active,
                // even if the wake up command failed!
                nSensorsAwake++;

                if (sensorSuccess) {
                    MS_DBG(F("        ... wake up succeeded."));
                } else {
                    MS_DBG(F("        ... wake up failed!"));
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
// a calculated variable will never be marked as the last variable from a
// sensor.
bool VariableArray::sensorsSleep(void) {
    MS_DBG(F("Putting sensors to sleep..."));
    bool success = true;
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)) {  // Skip non-unique sensors
            MS_DBG(F("    "), arrayOfVars[i]->getParentSensorNameAndLocation(),
                   F("..."));

            bool sensorSuccess = arrayOfVars[i]->parentSensor->sleep();
            success &= sensorSuccess;

            if (sensorSuccess) {
                MS_DBG(F("        ... successfully put to sleep."));
            } else {
                MS_DBG(F("        ... failed to sleep!"));
            }
        }
    }
    return success;
}


// This cuts power to the sensors
// We're not waiting for anything to be ready, we're just cutting power.
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a
// sensor.
void VariableArray::sensorsPowerDown(void) {
    MS_DBG(F("Powering down sensors..."));
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)) {  // Skip non-unique sensors
            MS_DBG(F("    Powering down"),
                   arrayOfVars[i]->getParentSensorNameAndLocation());

            arrayOfVars[i]->parentSensor->powerDown();
        }
    }
}


// This function updates the values for any connected sensors.
// Please note that this does NOT run the update functions, it instead uses
// the startSingleMeasurement and addSingleMeasurementResult functions to
// take advantage of the ability of sensors to be measuring concurrently.
// NOTE:  Calculated variables will always be skipped in this process because
// a calculated variable will never be marked as the last variable from a
// sensor.
bool VariableArray::updateAllSensors(void) {
    bool    success           = true;
    uint8_t nSensorsCompleted = 0;

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    bool deepDebugTiming = true;
#else
    bool deepDebugTiming = false;
#endif

    // Create an array with the unique-ness value (so we can skip the function
    // calls later)
    MS_DBG(F("Creating a mask array with the uniqueness for each sensor.."));
    bool lastSensorVariable[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        lastSensorVariable[i] = isLastVarFromSensor(i);
    }

    // Create an array for the number of measurements already completed and set
    // all to zero
    MS_DBG(F("Creating an array for the number of completed measurements.."));
    uint8_t nMeasurementsCompleted[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        nMeasurementsCompleted[i] = 0;
    }

    // Create an array for the number of measurements to average (another short
    // cut)
    MS_DBG(F("Creating an array with the number of measurements to average.."));
    uint8_t nMeasurementsToAverage[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]) {
            nMeasurementsToAverage[i] =
                arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage();
        } else {
            nMeasurementsToAverage[i] = 0;
        }
    }

    // Clear the initial variable arrays
    MS_DBG(F("----->> Clearing all results arrays before taking new "
             "measurements. ..."));
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]) {
            arrayOfVars[i]->parentSensor->clearValues();
        }
    }
    MS_DBG(F("    ... Complete. <<-----"));

    // Check for any sensors that didn't wake up and mark them as "complete" so
    // they will be skipped in further looping.
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]  // Skip non-unique sensors
            && (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) ==
                    0  // No attempt made to wake the sensor up
                || bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4) ==
                    0  // OR Wake up failed
                )) {
            MS_DBG(i, F("--->>"),
                   arrayOfVars[i]->getParentSensorNameAndLocation(),
                   F("isn't awake/active!  No measurements will be taken! "
                     "<<---"),
                   i);

            // Set the number of measurements already equal to whatever
            // total number requested to ensure the sensor is skipped in
            // further loops.
            nMeasurementsCompleted[i] = nMeasurementsToAverage[i];
            // Bump up the finished count.
            nSensorsCompleted++;
        }
    }

    while (nSensorsCompleted < _sensorCount) {
        for (uint8_t i = 0; i < _variableCount; i++) {
            /***
            // THIS IS PURELY FOR DEEP DEBUGGING OF THE TIMING!
            // Leave this whole section commented out unless you want excessive
            // printouts (ie, thousands of lines) of the timing information!!
            if (lastSensorVariable[i] and
                nMeasurementsToAverage[i] > nMeasurementsCompleted[i])
            {
                MS_DEEP_DBG(i), '-',
            arrayOfVars[i]->getParentSensorNameAndLocation(), F("- millis:"),
            millis(), F("- status: 0b"),
                           bitRead(arrayOfVars[i]->parentSensor->getStatus(),
            7), bitRead(arrayOfVars[i]->parentSensor->getStatus(), 6),
                           bitRead(arrayOfVars[i]->parentSensor->getStatus(),
            5), bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4),
                           bitRead(arrayOfVars[i]->parentSensor->getStatus(),
            3), bitRead(arrayOfVars[i]->parentSensor->getStatus(), 2),
                           bitRead(arrayOfVars[i]->parentSensor->getStatus(),
            1), bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0), F("-
            measurement #"), (nMeasurementsCompleted[i] + 1);
            }
            // END CHUNK FOR DEBUGGING!
            ***/

            // Only do checks on sensors that still have measurements to finish
            if (lastSensorVariable[i] &&
                nMeasurementsToAverage[i] > nMeasurementsCompleted[i]) {
                // first, make sure the sensor is stable
                if (arrayOfVars[i]->parentSensor->isStable(deepDebugTiming)) {
                    // now, if the sensor is not currently measuring...
                    if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5) ==
                        0) {  // NO attempt yet to start a measurement
                        // Start a reading
                        MS_DBG(i, '.', nMeasurementsCompleted[i] + 1,
                               F("--->> Starting reading"),
                               nMeasurementsCompleted[i] + 1, F("on"),
                               arrayOfVars[i]->getParentSensorNameAndLocation(),
                               '-');

                        bool sensorSuccess_start =
                            arrayOfVars[i]
                                ->parentSensor->startSingleMeasurement();
                        success &= sensorSuccess_start;

                        if (sensorSuccess_start) {
                            MS_DBG(F("   ... reading started! <<---"), i, '.',
                                   nMeasurementsCompleted[i] + 1);
                        } else {
                            MS_DBG(F("   ... failed to start reading! <<---"),
                                   i, '.', nMeasurementsCompleted[i] + 1);
                        }
                    }

                    // otherwise, it is currently measuring so...
                    // if a measurement is finished, get the result and tick up
                    // the number of finished measurements
                    // NOTE:  isMeasurementComplete(deepDebugTiming) will
                    // immediately return true if the attempt to start a
                    // measurement failed (bit 6 not set).  In that case, the
                    // addSingleMeasurementResult() will be "adding" -9999
                    // values.
                    if (arrayOfVars[i]->parentSensor->isMeasurementComplete(
                            deepDebugTiming)) {
                        // Get the value
                        MS_DBG(i, '.', nMeasurementsCompleted[i] + 1,
                               F("--->> Collected result of reading"),
                               nMeasurementsCompleted[i] + 1, F("from"),
                               arrayOfVars[i]->getParentSensorNameAndLocation(),
                               F("..."));

                        bool sensorSuccess_result =
                            arrayOfVars[i]
                                ->parentSensor->addSingleMeasurementResult();
                        success &= sensorSuccess_result;
                        nMeasurementsCompleted[i] +=
                            1;  // increment the number of measurements that
                                // sensor has completed

                        if (sensorSuccess_result) {
                            MS_DBG(F("   ... got measurement result. <<---"), i,
                                   '.', nMeasurementsCompleted[i]);
                        } else {
                            MS_DBG(F("   ... failed to get measurement result! "
                                     "<<---"),
                                   i, '.', nMeasurementsCompleted[i]);
                        }
                    }
                }

                // if all the measurements are done, mark the whole sensor as
                // done
                if (nMeasurementsCompleted[i] == nMeasurementsToAverage[i]) {
                    MS_DBG(F("--- Finished all measurements from"),
                           arrayOfVars[i]->getParentSensorNameAndLocation(),
                           F("---"));

                    nSensorsCompleted++;
                    MS_DBG(F("*****---"), nSensorsCompleted,
                           F("sensors now complete ---*****"));
                }
            }
        }
    }

    // Average measurements and notify varibles of the updates
    MS_DBG(F("----->> Averaging results and notifying all variables. ..."));
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]) {
            MS_DEEP_DBG(F("--- Averaging results from"),
                        arrayOfVars[i]->getParentSensorNameAndLocation(),
                        F("---"));
            arrayOfVars[i]->parentSensor->averageMeasurements();
            MS_DEEP_DBG(F("--- Notifying variables from"),
                        arrayOfVars[i]->getParentSensorNameAndLocation(),
                        F("---"));
            arrayOfVars[i]->parentSensor->notifyVariables();
        }
    }
    MS_DBG(F("... Complete. <<-----"));

    return success;
}


// This function is an even more complete version of the updateAllSensors
// function - it handles power up/down and wake/sleep.
bool VariableArray::completeUpdate(void) {
    bool    success           = true;
    uint8_t nSensorsCompleted = 0;

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    bool deepDebugTiming = true;
#else
    bool deepDebugTiming = false;
#endif

    // Create an array with the unique-ness value (so we can skip the function
    // calls later)
    MS_DBG(F("Creating a mask array with the uniqueness for each sensor.."));
    bool lastSensorVariable[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        lastSensorVariable[i] = isLastVarFromSensor(i);
    }

    // Create an array for the number of measurements already completed and set
    // all to zero
    MS_DBG(F("Creating an array for the number of completed measurements.."));
    uint8_t nMeasurementsCompleted[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        nMeasurementsCompleted[i] = 0;
    }

    // Create an array for the number of measurements to average (another short
    // cut)
    MS_DBG(F("Creating an array with the number of measurements to average.."));
    uint8_t nMeasurementsToAverage[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]) {
            nMeasurementsToAverage[i] =
                arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage();
        } else {
            nMeasurementsToAverage[i] = 0;
        }
    }

    // Create an array of the power pins
    MS_DBG(F("Creating an array of the power pins.."));
    int8_t powerPins[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]) {
            powerPins[i] = arrayOfVars[i]->parentSensor->getPowerPin();
        } else {
            powerPins[i] = 0;
        }
    }

    // Create an array of the last variable on each power pin
    MS_DBG(F("Creating arrays of the power pin locations.."));
    bool lastPinVariable[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) { lastPinVariable[i] = true; }
    // Create an array containing the index of the power pin in the powerPins
    // array
    int8_t powerPinIndex[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) { powerPinIndex[i] = 0; }
    // Create an array to tell us how many measurements must be taken
    // before all the sensors attached to a power pin are done
    uint8_t nMeasurementsOnPin[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        nMeasurementsOnPin[i] = nMeasurementsToAverage[i];
    }
    // Now correctly populate the previous three arrays
    for (uint8_t i = 0; i < _variableCount; i++) {
        for (uint8_t j = i + 1; j < _variableCount; j++) {
            if (!lastSensorVariable[i]) {
                lastPinVariable[i] = false;
            } else if (powerPins[i] == powerPins[j]) {
                lastPinVariable[i] = false;
            }
            i++;
        }
    }
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastPinVariable[i]) {
            powerPinIndex[i]      = i;
            nMeasurementsOnPin[i] = nMeasurementsToAverage[i];
            for (uint8_t j = 0; j < _variableCount; j++) {
                if (powerPins[j] == powerPins[i] && i != j) {
                    powerPinIndex[j] = i;
                    nMeasurementsOnPin[i] += nMeasurementsToAverage[j];
                    nMeasurementsOnPin[j] = 0;
                }
            }
        }
    }

// This is just for debugging
#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    uint8_t arrayPositions[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) { arrayPositions[i] = i; }
    String nameLocation[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) {
        // nameLocation[i] = arrayOfVars[i]->getParentSensorNameAndLocation();
        nameLocation[i] = arrayOfVars[i]->getParentSensorName();
    }
    MS_DEEP_DBG(F("----------------------------------"));
    MS_DEEP_DBG(F("arrayPositions:\t\t\t"));
    prettyPrintArray(arrayPositions);
    MS_DEEP_DBG(F("sensor:\t\t\t"));
    prettyPrintArray(nameLocation);
    MS_DEEP_DBG(F("lastSensorVariable:\t\t"));
    prettyPrintArray(lastSensorVariable);
    MS_DEEP_DBG(F("nMeasurementsToAverage:\t\t"));
    prettyPrintArray(nMeasurementsToAverage);
    MS_DEEP_DBG(F("nMeasurementsCompleted:\t\t"));
    prettyPrintArray(nMeasurementsCompleted);
    MS_DEEP_DBG(F("powerPins:\t\t\t"));
    prettyPrintArray(powerPins);
    MS_DEEP_DBG(F("lastPinVariable:\t\t"));
    prettyPrintArray(lastPinVariable);
    MS_DEEP_DBG(F("powerPinIndex:\t\t\t"));
    prettyPrintArray(powerPinIndex);
    MS_DEEP_DBG(F("nMeasurementsOnPin:\t\t"));
    prettyPrintArray(nMeasurementsOnPin);
    MS_DEEP_DBG(F("powerPinIndex:\t\t\t"));
    prettyPrintArray(powerPinIndex);
#endif

    // Another array for the number of measurements already completed per power
    // pin
    uint8_t nCompletedOnPin[_variableCount];
    for (uint8_t i = 0; i < _variableCount; i++) { nCompletedOnPin[i] = 0; }

    // Clear the initial variable arrays
    MS_DBG(F("----->> Clearing all results arrays before taking new "
             "measurements. ..."));
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]) {
            arrayOfVars[i]->parentSensor->clearValues();
        }
    }
    MS_DBG(F("   ... Complete. <<-----"));

    // power up all of the sensors together
    MS_DBG(F("----->> Powering up all sensors together. ..."));
    sensorsPowerUp();
    MS_DBG(F("   ... Complete. <<-----"));

    while (nSensorsCompleted < _sensorCount) {
        for (uint8_t i = 0; i < _variableCount; i++) {
            /***
            // THIS IS PURELY FOR DEEP DEBUGGING OF THE TIMING!
            // Leave this whole section commented out unless you want excessive
            // printouts (ie, thousands of lines) of the timing information!!
            if (lastSensorVariable[i] and
                nMeasurementsToAverage[i] > nMeasurementsCompleted[i]) {
                MS_DEEP_DBG(
                    i, '-', arrayOfVars[i]->getParentSensorNameAndLocation(),
                    F("- millis:"), millis(), F("- status: 0b"),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 7),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 6),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 2),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 1),
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0),
                    F("- measurement #"), (nMeasurementsCompleted[i] + 1));
            }
            MS_DEEP_DBG(F("----------------------------------"));
            MS_DEEP_DBG(F("nMeasurementsToAverage:\t\t"));
            prettyPrintArray(nMeasurementsToAverage);
            MS_DEEP_DBG(F("nMeasurementsCompleted:\t\t"));
            prettyPrintArray(nMeasurementsCompleted);
            MS_DEEP_DBG(F("nMeasurementsOnPin:\t\t"));
            prettyPrintArray(nMeasurementsOnPin);
            MS_DEEP_DBG(F("nCompletedOnPin:\t\t\t"));
            prettyPrintArray(nCompletedOnPin);
            // END CHUNK FOR DEBUGGING!
            ***/

            // Only do checks on sensors that still have measurements to finish
            if (lastSensorVariable[i] &&
                nMeasurementsToAverage[i] > nMeasurementsCompleted[i]) {
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) ==
                        0  // If no attempts yet made to wake the sensor up
                    && arrayOfVars[i]->parentSensor->isWarmedUp(
                           deepDebugTiming)  // and if it is already warmed up
                ) {
                    MS_DBG(i, F("--->> Waking"),
                           arrayOfVars[i]->getParentSensorNameAndLocation(),
                           F("..."));

                    // Make a single attempt to wake the sensor after it is
                    // warmed up
                    bool sensorSuccess_wake =
                        arrayOfVars[i]->parentSensor->wake();
                    success &= sensorSuccess_wake;

                    if (sensorSuccess_wake) {
                        MS_DBG(F("   ... wake up success. <<---"), i);
                    } else {
                        MS_DBG(F("   ... wake up failed! <<---"), i);
                    }
                }

                // If attempts were made to wake the sensor, but they failed
                // then we're just bumping up the number of measurements to
                // completion
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3) ==
                        1 &&
                    bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4) ==
                        0) {
                    MS_DBG(i, F("--->>"),
                           arrayOfVars[i]->getParentSensorNameAndLocation(),
                           F("did not wake up! No measurements will be taken! "
                             "<<---"),
                           i);
                    // Set the number of measurements already equal to whatever
                    // total number requested to ensure the sensor is skipped in
                    // further loops.
                    nMeasurementsCompleted[i] = nMeasurementsToAverage[i];
                    // increment the number of measurements that the power pin
                    // has completed
                    nCompletedOnPin[powerPinIndex[i]] +=
                        nMeasurementsToAverage[i];
                }

                // If the sensor was successfully awoken/activated...
                // .. make sure the sensor is stable
                if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4) ==
                        1 &&
                    arrayOfVars[i]->parentSensor->isStable(deepDebugTiming)) {
                    // If no attempt has yet been made to start a measurement,
                    // start one
                    if (bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5) ==
                        0) {
                        // Start a reading
                        MS_DBG(i, '.', nMeasurementsCompleted[i] + 1,
                               F("--->> Starting reading"),
                               nMeasurementsCompleted[i] + 1, F("on"),
                               arrayOfVars[i]->getParentSensorNameAndLocation(),
                               F("..."));

                        bool sensorSuccess_start =
                            arrayOfVars[i]
                                ->parentSensor->startSingleMeasurement();
                        success &= sensorSuccess_start;

                        if (sensorSuccess_start) {
                            MS_DBG(F("   ... start reading succeeded. <<---"),
                                   i, '.', nMeasurementsCompleted[i] + 1);
                        } else {
                            MS_DBG(F("   ... start reading failed! <<---"), i,
                                   '.', nMeasurementsCompleted[i] + 1);
                        }
                    }

                    // If a measurement is finished, get the result and tick up
                    // the number of finished measurements.  We aren't bothering
                    // to check if the measurement start was successful,
                    // isMeasurementComplete(deepDebugTiming) will do that and
                    // we stil want the addSingleMeasurementResult() function to
                    // fill in the -9999 results for a failed measurement.
                    if (arrayOfVars[i]->parentSensor->isMeasurementComplete(
                            deepDebugTiming)) {
                        // Get the value
                        MS_DBG(i, '.', nMeasurementsCompleted[i] + 1,
                               F("--->> Collected result of reading"),
                               nMeasurementsCompleted[i] + 1, F("from"),
                               arrayOfVars[i]->getParentSensorNameAndLocation(),
                               F("..."));

                        bool sensorSuccess_result =
                            arrayOfVars[i]
                                ->parentSensor->addSingleMeasurementResult();
                        success &= sensorSuccess_result;
                        nMeasurementsCompleted[i] +=
                            1;  // increment the number of measurements that
                                // sensor has completed
                        nCompletedOnPin[powerPinIndex[i]] +=
                            1;  // increment the number of measurements that the
                                // power pin has completed

                        if (sensorSuccess_result) {
                            MS_DBG(F("   ... got measurement result. <<---"), i,
                                   '.', nMeasurementsCompleted[i]);
                        } else {
                            MS_DBG(F("   ... failed to get measurement result! "
                                     "<<---"),
                                   i, '.', nMeasurementsCompleted[i]);
                        }
                    }
                }

                // If all the measurements are done
                if (nMeasurementsCompleted[i] == nMeasurementsToAverage[i]) {
                    MS_DBG(i, F("--->> Finished all measurements from"),
                           arrayOfVars[i]->getParentSensorNameAndLocation(),
                           F(", putting it to sleep. ..."));

                    // Put the completed sensor to sleep
                    bool sensorSuccess_sleep =
                        arrayOfVars[i]->parentSensor->sleep();
                    success &= sensorSuccess_sleep;

                    if (sensorSuccess_sleep) {
                        MS_DBG(F("   ... succeeded in putting sensor to sleep. "
                                 "<<---"),
                               i);
                    } else {
                        MS_DBG(F("   ... sleep failed! <<---"), i);
                    }

                    // Now cut the power, if ready, to this sensors and all that
                    // share the pin
                    if (nCompletedOnPin[powerPinIndex[i]] ==
                        nMeasurementsOnPin[powerPinIndex[i]]) {
                        for (uint8_t k = 0; k < _variableCount; k++) {
                            if (powerPinIndex[k] == powerPinIndex[i] &&
                                lastSensorVariable[k]) {
                                arrayOfVars[k]->parentSensor->powerDown();
                                MS_DBG(k, F("--->>"),
                                       arrayOfVars[k]
                                           ->getParentSensorNameAndLocation(),
                                       F("powered down. <<---"), k);
                            }
                        }
                    }

                    nSensorsCompleted++;  // mark the whole sensor as done
                    MS_DBG(F("*****---"), nSensorsCompleted,
                           F("sensors now complete ---*****"));
                }
            }
        }
    }

    // Average measurements and notify varibles of the updates
    MS_DBG(F("----->> Averaging results and notifying all variables. ..."));
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (lastSensorVariable[i]) {
            MS_DBG(F("--- Averaging results from"),
                   arrayOfVars[i]->getParentSensorNameAndLocation(), F("---"));
            arrayOfVars[i]->parentSensor->averageMeasurements();
            MS_DBG(F("--- Notifying variables from"),
                   arrayOfVars[i]->getParentSensorNameAndLocation(), F("---"));
            arrayOfVars[i]->parentSensor->notifyVariables();
        }
    }
    MS_DBG(F("... Complete. <<-----"));

    return success;
}


// This function prints out the results for any connected sensors to a stream
//  Calculated Variable results will be included
void VariableArray::printSensorData(Stream* stream) {
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (arrayOfVars[i]->isCalculated) {
            stream->print(arrayOfVars[i]->getVarName());
            stream->print(F(" is calculated to be "));
            stream->print(arrayOfVars[i]->getValueString());
            stream->print(F(" "));
            stream->print(arrayOfVars[i]->getVarUnit());
            stream->println();
        } else {
            stream->print(arrayOfVars[i]->getParentSensorNameAndLocation());
            // stream->print(F(" with status 0b"));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 7));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 6));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 5));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 4));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 3));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 2));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 1));
            // stream->print(
            //     bitRead(arrayOfVars[i]->parentSensor->getStatus(), 0));
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


// Check for unique sensors
bool VariableArray::isLastVarFromSensor(int arrayIndex) {
    // Calculated Variables are never the last variable from a sensor, simply
    // because the don't come from a sensor at all.
    if (arrayOfVars[arrayIndex]->isCalculated) {
        return false;
    } else {
        String sensNameLoc =
            arrayOfVars[arrayIndex]->getParentSensorNameAndLocation();
        bool unique = true;
        for (int j = arrayIndex + 1; j < _variableCount; j++) {
            if (sensNameLoc ==
                arrayOfVars[j]->getParentSensorNameAndLocation()) {
                unique = false;
                break;
            }
        }
        return unique;
    }
}


// Count the maximum number of measurements needed from a single sensor for the
// requested averaging
uint8_t VariableArray::countMaxToAverage(void) {
    uint8_t numReps = 0;
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)) {  // Skip non-unique sensors
            numReps = max(
                numReps,
                arrayOfVars[i]->parentSensor->getNumberMeasurementsToAverage());
        }
    }
    return numReps;
}


// Check that all variable have valid UUID's, if they are assigned
bool VariableArray::checkVariableUUIDs(void) {
    bool success = true;
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (!arrayOfVars[i]->checkUUIDFormat()) {
            PRINTOUT(arrayOfVars[i]->getVarCode(), F("has an invalid UUID!"));
            success = false;
        }
        for (uint8_t j = i + 1; j < _variableCount; j++) {
            if (arrayOfVars[i]->getVarUUID() == arrayOfVars[j]->getVarUUID()) {
                PRINTOUT(arrayOfVars[i]->getVarCode(),
                         F("has a non-unique UUID!"));
                success = false;
                // don't keep looping
                j = _variableCount;
            }
        }
    }
    if (success)
        PRINTOUT(F("All variable UUID's appear to be correctly formed.\n"));
    // Print out all UUID's to check
    for (uint8_t i = 0; i < _variableCount; i++) {
        PRINTOUT(arrayOfVars[i]->getVarUUID(), F("->"),
                 arrayOfVars[i]->getVarCode());
    }
    PRINTOUT(' ');
    return success;
}
