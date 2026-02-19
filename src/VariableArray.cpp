/**
 * @file VariableArray.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
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
    _sensorCount = getSensorCount();
}
VariableArray::VariableArray(uint8_t variableCount, Variable* variableList[],
                             const char* uuids[])
    : arrayOfVars(variableList),
      _variableCount(variableCount) {
    _sensorCount = getSensorCount();
    matchUUIDs(uuids);
}

// Destructor
VariableArray::~VariableArray() {}

void VariableArray::begin(uint8_t variableCount, Variable* variableList[],
                          const char* uuids[]) {
    _variableCount = variableCount;
    arrayOfVars    = variableList;

    _sensorCount = getSensorCount();
    matchUUIDs(uuids);
    checkVariableUUIDs();
}
void VariableArray::begin(uint8_t variableCount, Variable* variableList[]) {
    _variableCount = variableCount;
    arrayOfVars    = variableList;

    _sensorCount = getSensorCount();
    checkVariableUUIDs();
}
void VariableArray::begin() {
    _sensorCount = getSensorCount();
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

// This matches UUIDs from an array of pointers to the variable array
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
            && getSensorStatusBit(i, Sensor::SETUP_SUCCESSFUL) ==
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
                && getSensorStatusBit(i, Sensor::SETUP_SUCCESSFUL) ==
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
            && getSensorStatusBit(i, Sensor::WAKE_ATTEMPTED) ==
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
                && getSensorStatusBit(i, Sensor::WAKE_ATTEMPTED) ==
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
bool VariableArray::updateAllSensors(void) {
    return completeUpdate(false, false, false, false);
}

bool VariableArray::completeUpdate(bool powerUp, bool wake, bool sleep,
                                   bool powerDown) {
    bool    success           = true;
    uint8_t nSensorsCompleted = 0;

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    bool deepDebugTiming = true;
#else
    bool deepDebugTiming = false;
#endif

    MS_DBG(F("Creating an array of pointers to the sensors.."));
    Sensor* sensorList[_sensorCount];
    uint8_t addedSensors = 0;
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (isLastVarFromSensor(i)) {
            if (addedSensors >= _sensorCount) {
                MS_DBG(F("ERROR: More unique sensors found than expected!"));
                return false;
            }
            sensorList[addedSensors++] = arrayOfVars[i]->parentSensor;
        }
    }
    if (addedSensors != _sensorCount) {
        MS_DBG(F("ERROR: Expected"), _sensorCount, F("sensors but found"),
               addedSensors);
        return false;
    }

#if defined(MS_VARIABLEARRAY_DEBUG) || defined(MS_VARIABLEARRAY_DEBUG_DEEP)
    for (uint8_t i = 0; i < _sensorCount; i++) {
        MS_DBG(F("   Sensor"), i, F("is"),
               sensorList[i]->getSensorNameAndLocation());
    }
#endif

    if (powerUp) {
        // CLear power status bits before powering on.  The powerUp function
        // will check the actual power state of the sensor and set the bits
        // accordingly, so it's safe to clear them here before powering up.
        MS_DEEP_DBG(
            F("----->> Clearing all power status bits before taking new "
              "measurements. ..."));
        for (uint8_t i = 0; i < _sensorCount; i++) {
            sensorList[i]->clearPowerStatus();
        }
        MS_DEEP_DBG(F("   ... Complete. <<-----"));

        // power up all of the sensors together
        MS_DBG(F("----->> Powering up all sensors together. ..."));
        sensorsPowerUp();
        MS_DBG(F("   ... Complete. <<-----"));
    } else {
        // If this function isn't powering the sensors, check whether or not the
        // sensors are actually powered on before trying to wake them or
        // assuming they are awake.  If the sensors are not powered, the
        // checkPowerOn function will reset the power *and wake* bits so the
        // wake check or wake function will work correctly.
        MS_DBG(F("----->> Checking the power state of all sensor. ..."));
        for (uint8_t i = 0; i < _sensorCount; i++) {
            sensorList[i]->checkPowerOn();
        }
        MS_DBG(F("   ... Complete. <<-----"));
    }

    // NOTE: Don't clear the wake bits/timing!  If the power up function found
    // the sensor wasn't powered, it will have cleared the wake bits. If we
    // clear the wake bits here before checking them, then we won't be able to
    // tell if the sensor was already awake before this function was called.  If
    // this function is called with wake=false (ie, expecting the sensors to
    // have been awoken elsewhere), then we need to be able to check if the wake
    // was successful before attempting readings, so we need to keep the wake
    // bits intact.

    // Clear all measurement related status bits and timing values before
    // starting measurements. NOTE: These bits are set and checked **after**
    // starting a measurement to confirm that the measurement was actually
    // started, so it's safe to clear them before starting a measurement.
    MS_DEEP_DBG(F("----->> Clearing all measurement status bits before taking "
                  "new measurements. ..."));
    for (uint8_t i = 0; i < _sensorCount; i++) {
        sensorList[i]->clearMeasurementStatus();
    }
    MS_DEEP_DBG(F("   ... Complete. <<-----"));

    // Clear the initial variable values arrays and reset the measurement
    // attempt and retry counts.
    MS_DBG(F("----->> Clearing all results arrays before taking new "
             "measurements. ..."));
    for (uint8_t i = 0; i < _sensorCount; i++) { sensorList[i]->clearValues(); }
    MS_DBG(F("   ... Complete. <<-----"));

    while (nSensorsCompleted < _sensorCount) {
        for (uint8_t i = 0; i < _sensorCount; i++) {
            uint8_t nReq = sensorList[i]->getNumberMeasurementsToAverage();
#if defined(MS_VARIABLEARRAY_DEBUG) || defined(MS_VARIABLEARRAY_DEBUG_DEEP)
            String sName    = sensorList[i]->getSensorNameAndLocation();
            String cycCount = String(i) + '.' +
                String(sensorList[i]->getNumberCompleteMeasurementsAttempts() +
                       1) +
                '.' + String(sensorList[i]->getNumberRetryAttemptsMade());
#endif
            // Skip sensors that have already completed all their measurements
            if (sensorList[i]->getNumberCompleteMeasurementsAttempts() >=
                nReq) {
                continue;
            }

            // If attempts were made to wake the sensor, but they failed...
            // NOTE: We check if the wake was successful even if the wake
            // parameter is false because we need to know the sensor wake failed
            // before attempting readings even if the user called wake somewhere
            // else.
            if (sensorList[i]->getStatusBit(Sensor::WAKE_ATTEMPTED) == 1 &&
                sensorList[i]->getStatusBit(Sensor::WAKE_SUCCESSFUL) == 0) {
                MS_DBG(i, F("--->>"), sName,
                       F("failed to wake up! No measurements will be taken! "
                         "<<---"),
                       i);
                // Set the number of measurements already complete equal to
                // whatever total number requested to ensure the sensor is
                // skipped in further loops. NOTE: These are protected members
                // of the sensor class; we can only access them because the
                // variableArray class is a friend of the sensor class.
                sensorList[i]->_measurementAttemptsCompleted =
                    sensorList[i]->_measurementsToAverage;
            }

            if (wake
                // ^^ if we're supposed to wake the sensors
                && sensorList[i]->getStatusBit(Sensor::WAKE_ATTEMPTED) == 0
                // ^^ And no attempts yet made to wake the sensor up
                && sensorList[i]->isWarmedUp(deepDebugTiming)
                // ^^ and if it is already warmed up
            ) {
                MS_DBG(i, F("--->> Waking"), sName, F("..."));

                // Make a single attempt to wake the sensor after it is
                // warmed up
                bool sensorSuccess_wake = sensorList[i]->wake();
                success &= sensorSuccess_wake;

                if (sensorSuccess_wake) {
                    MS_DBG(F("   ... wake up succeeded. <<---"), i);
                } else {
                    MS_DBG(F("   ... wake up failed! <<---"), i);
                }
            }

            // If the sensor was successfully awoken/activated, but no
            // measurement was either started or finished ...
            if (sensorList[i]->getStatusBit(Sensor::WAKE_SUCCESSFUL) == 1 &&
                sensorList[i]->getStatusBit(Sensor::MEASUREMENT_ATTEMPTED) ==
                    0 &&
                sensorList[i]->getStatusBit(Sensor::MEASUREMENT_SUCCESSFUL) ==
                    0) {
                // .. check if it's stable
                if (sensorList[i]->isStable(deepDebugTiming)) {
                    MS_DBG(cycCount, F("--->> Starting reading on"), sName,
                           F("..."));

                    bool sensorSuccess_start =
                        sensorList[i]->startSingleMeasurement();
                    success &= sensorSuccess_start;

                    if (sensorSuccess_start) {
                        MS_DBG(F("   ... start reading succeeded. <<---"),
                               cycCount);
                    } else {
                        MS_DBG(F("   ... start reading failed! <<---"),
                               cycCount);
                    }
                }
            }

            // if measurements have been started, whether or not
            // successfully...
            // We aren't checking if the measurement start was successful;
            // isMeasurementComplete(deepDebugTiming) will do that.
            if (sensorList[i]->getStatusBit(Sensor::MEASUREMENT_ATTEMPTED) ==
                1) {
                // If a measurement is finished, get the result and tick up
                // the number of finished measurements.
                if (sensorList[i]->isMeasurementComplete(deepDebugTiming)) {
                    // Get the value
                    MS_DBG(cycCount,
                           F("--->> Collected result of reading from"), sName,
                           F("..."));

                    bool sensorSuccess_result =
                        sensorList[i]->addSingleMeasurementResult();
                    success &= sensorSuccess_result;

                    if (sensorSuccess_result) {
                        MS_DBG(F("   ... got measurement result. <<---"),
                               cycCount);
                    } else {
                        MS_DBG(F("   ... failed to get measurement result! "
                                 "<<---"),
                               cycCount);
                    }
                }
            }

            // If all the measurements are now complete
            if (sensorList[i]->getNumberCompleteMeasurementsAttempts() >=
                nReq) {
                if (sleep) {
                    MS_DBG(i, F("--->> Finished all measurements from"), sName,
                           F(", putting it to sleep. ..."));

                    // Put the completed sensor to sleep
                    bool sensorSuccess_sleep = sensorList[i]->sleep();
                    success &= sensorSuccess_sleep;

                    if (sensorSuccess_sleep) {
                        MS_DBG(F("   ... succeeded in putting sensor to sleep. "
                                 "Total wake time was"),
                               millis() - sensorList[i]->_millisSensorActivated,
                               F("ms <<---"), i);
                    } else {
                        MS_DBG(F("   ... sleep failed! <<---"), i);
                    }
                }
                if (powerDown) {
                    // NOTE: We are NOT checking if the sleep command succeeded!
                    // Cut the power, if ready, to this sensors and all that
                    // share the pin
                    bool canPowerDown = true;  // assume we can power down
                                               // unless we find a conflict
                    for (uint8_t k = 0; k < _sensorCount; k++) {
                        if ((
                                // Check if sensor i's primary pin matches
                                // either of sensor k's pins
                                (sensorList[i]->getPowerPin() >= 0 &&
                                 (sensorList[i]->getPowerPin() ==
                                      sensorList[k]->getPowerPin() ||
                                  sensorList[i]->getPowerPin() ==
                                      sensorList[k]->getSecondaryPowerPin()))
                                // Check if sensor i's secondary pin matches
                                // either of sensor k's pins
                                ||
                                (sensorList[i]->getSecondaryPowerPin() >= 0 &&
                                 (sensorList[i]->getSecondaryPowerPin() ==
                                      sensorList[k]->getPowerPin() ||
                                  sensorList[i]->getSecondaryPowerPin() ==
                                      sensorList[k]->getSecondaryPowerPin())))
                            // Check if sensor k still needs measurements
                            &&
                            (sensorList[k]
                                 ->getNumberCompleteMeasurementsAttempts() <
                             sensorList[k]->getNumberMeasurementsToAverage())) {
                            // If sensors i and k share a primary power pin or a
                            // secondary power pin and sensor k still needs
                            // measurements, sensor i can't be powered down
                            canPowerDown = false;
                            break;  // stop looping after finding a conflict
                        }
                    }
                    if (canPowerDown) { sensorList[i]->powerDown(); }
#if defined(MS_VARIABLEARRAY_DEBUG)
                    if (canPowerDown) {
                        MS_DBG(
                            i, F("--->> All measurements from"), sName,
                            F("are complete and no other sensors on the same "
                              "power pin need to take measurements.  "
                              "Powered down all sensors on pin"),
                            sensorList[i]->getPowerPin(), F("or pin"),
                            sensorList[i]->getSecondaryPowerPin(), F("..."));
                    } else {
                        MS_DBG(i, F("--->> All measurements from"), sName,
                               F("are complete but other sensors on the same "
                                 "power pin still need to take measurements.  "
                                 "Leaving power on pin"),
                               sensorList[i]->getPowerPin(), F("ON. <<---"));
                        // Find and report which sensor still needs measurements
                        for (uint8_t k = 0; k < _sensorCount; k++) {
                            if (((sensorList[i]->getPowerPin() >= 0 &&
                                  (sensorList[i]->getPowerPin() ==
                                       sensorList[k]->getPowerPin() ||
                                   sensorList[i]->getPowerPin() ==
                                       sensorList[k]
                                           ->getSecondaryPowerPin())) ||
                                 (sensorList[i]->getSecondaryPowerPin() >= 0 &&
                                  (sensorList[i]->getSecondaryPowerPin() ==
                                       sensorList[k]->getPowerPin() ||
                                   sensorList[i]->getSecondaryPowerPin() ==
                                       sensorList[k]
                                           ->getSecondaryPowerPin()))) &&
                                (sensorList[k]
                                     ->getNumberCompleteMeasurementsAttempts() <
                                 sensorList[k]
                                     ->getNumberMeasurementsToAverage())) {
                                MS_DBG(
                                    sName, F("shares a power pin with"),
                                    sensorList[k]->getSensorNameAndLocation(),
                                    F("which still needs to take"),
                                    sensorList[k]
                                            ->getNumberMeasurementsToAverage() -
                                        sensorList[k]
                                            ->getNumberCompleteMeasurementsAttempts(),
                                    F("measurements."));
                                MS_DBG(sName, '(', i, ')', F("pins are"),
                                       sensorList[i]->getPowerPin(), F("and"),
                                       sensorList[i]->getSecondaryPowerPin());
                                MS_DBG(
                                    sensorList[k]->getSensorNameAndLocation(),
                                    '(', k, ')', F("pins are"),
                                    sensorList[k]->getPowerPin(), F("and"),
                                    sensorList[k]->getSecondaryPowerPin());
                                break;
                            }
                        }
                    }
#endif
                }
                nSensorsCompleted++;  // mark the whole sensor as done
                MS_DBG(F("*****---"), nSensorsCompleted,
                       F("sensors now complete ---*****"));
            } else {
                MS_DEEP_DBG(
                    i, F("--->>"), sName, F("still needs to take"),
                    nReq -
                        sensorList[i]->getNumberCompleteMeasurementsAttempts(),
                    F("measurements."));
            }
        }
        MS_DEEP_DBG(F("xxxxx---"), _sensorCount - nSensorsCompleted,
                    F("sensors remaining ---xxxxx"));
    }

    // Average measurements and notify variables of the updates
    MS_DBG(F("----->> Averaging results and notifying all variables. ..."));
    for (uint8_t i = 0; i < _sensorCount; i++) {
        MS_DBG(F("--- Averaging results from"),
               sensorList[i]->getSensorNameAndLocation(), F("---"));
        sensorList[i]->averageMeasurements();
        MS_DBG(F("--- Notifying variables from"),
               sensorList[i]->getSensorNameAndLocation(), F("---"));
        sensorList[i]->notifyVariables();
    }
    MS_DBG(F("... Complete. <<-----"));

    MS_DBG(F("Updating calculated variables. ..."));
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (arrayOfVars[i]->isCalculated) { arrayOfVars[i]->getValue(true); }
    }

    return success;
}


// This function prints out the results for any connected sensors to a stream
//  Calculated Variable results will be included
void VariableArray::printSensorData(Stream* stream) {
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (arrayOfVars[i]->isCalculated) {
            stream->print(arrayOfVars[i]->getVarName());
            stream->print(F(" ("));
            stream->print(arrayOfVars[i]->getVarCode());
            stream->print(F(") "));
            stream->print(F(" is calculated to be "));
            stream->print(arrayOfVars[i]->getValueString());
            stream->print(F(" "));
            stream->print(arrayOfVars[i]->getVarUnit());
            stream->println();
        } else {
            stream->print(arrayOfVars[i]->getParentSensorNameAndLocation());
            // stream->print(F(" with status 0b"));
            // stream->print(getSensorStatusBit(i, 7));
            // stream->print(getSensorStatusBit(i, 6));
            // stream->print(getSensorStatusBit(i, 5));
            // stream->print(getSensorStatusBit(i, 4));
            // stream->print(getSensorStatusBit(i, 3));
            // stream->print(getSensorStatusBit(i, 2));
            // stream->print(getSensorStatusBit(i, 1));
            // stream->print(getSensorStatusBit(i, 0));
            stream->print(F(" reports "));
            stream->print(arrayOfVars[i]->getVarName());
            stream->print(F(" ("));
            stream->print(arrayOfVars[i]->getVarCode());
            stream->print(F(") "));
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
        Sensor* parSens = arrayOfVars[arrayIndex]->parentSensor;
        bool    unique  = true;
        for (int j = arrayIndex + 1; j < _variableCount; j++) {
            if (parSens == arrayOfVars[j]->parentSensor) {
                unique = false;
                break;
            }
        }
        return unique;
    }
}


bool VariableArray::getSensorStatusBit(int                        arrayIndex,
                                       Sensor::sensor_status_bits bitToGet) {
    if (arrayIndex < 0 || arrayIndex >= _variableCount) { return false; }
    return arrayOfVars[arrayIndex]->parentSensor->getStatusBit(bitToGet);
}


// Check that all variable have valid UUIDs, if they are assigned
bool VariableArray::checkVariableUUIDs(void) {
    bool success = true;
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (!arrayOfVars[i]->checkUUIDFormat()) {
            PRINTOUT(arrayOfVars[i]->getVarCode(), F("has an invalid UUID!"));
            success = false;
        }
        // if (strcmp(arrayOfVars[i]->getVarUUID(),
        //            "12345678-abcd-1234-ef00-1234567890ab") == 0) {
        //     PRINTOUT(arrayOfVars[i]->getVarCode(),
        //              F("has an placeholder UUID!"));
        //     success = false;
        // }
        if (arrayOfVars[i]->getVarUUID() != nullptr &&
            strlen(arrayOfVars[i]->getVarUUID()) > 0) {
            for (uint8_t j = i + 1; j < _variableCount; j++) {
                if (strcmp(arrayOfVars[i]->getVarUUID(),
                           arrayOfVars[j]->getVarUUID()) == 0 &&
                    strlen(arrayOfVars[j]->getVarUUID()) > 0) {
                    PRINTOUT(arrayOfVars[i]->getVarCode(),
                             F("has a non-unique UUID!"));
                    success = false;
                    // don't keep looping
                    j = _variableCount;
                }
            }
        }
    }
    if (success)
        PRINTOUT(F("All variable UUIDs appear to be correctly formed.\n"));
    // Print out all UUIDs to check
    for (uint8_t i = 0; i < _variableCount; i++) {
        if (arrayOfVars[i]->getVarUUID() != nullptr &&
            strlen(arrayOfVars[i]->getVarUUID()) > 0) {
            PRINTOUT(arrayOfVars[i]->getVarUUID(), F("->"),
                     arrayOfVars[i]->getVarCode());
        }
    }
    PRINTOUT(' ');
    return success;
}
