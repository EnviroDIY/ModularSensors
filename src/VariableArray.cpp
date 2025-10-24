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
/**
 * @brief Construct a VariableArray from a list of variable pointers.
 *
 * Initializes the internal variable pointer array and variable count, then
 * calculates the number of distinct sensors represented by those variables.
 *
 * @param variableCount Number of entries in `variableList`.
 * @param variableList Array of pointers to Variable objects; must contain at least `variableCount` elements.
 */
VariableArray::VariableArray(uint8_t variableCount, Variable* variableList[])
    : arrayOfVars(variableList),
      _variableCount(variableCount) {
    _sensorCount = getSensorCount();
}
/**
 * @brief Construct a VariableArray with variables and assign UUIDs.
 *
 * Initializes the instance with the provided variable pointers and count,
 * computes the sensor count from those variables, and assigns each provided
 * UUID string to the corresponding variable.
 *
 * @param variableCount Number of entries in `variableList`.
 * @param variableList Array of pointers to Variable instances (length == variableCount).
 * @param uuids Array of C-strings containing UUIDs; each entry is applied to the corresponding variable in `variableList`.
 */
VariableArray::VariableArray(uint8_t variableCount, Variable* variableList[],
                             const char* uuids[])
    : arrayOfVars(variableList),
      _variableCount(variableCount) {
    _sensorCount = getSensorCount();
    matchUUIDs(uuids);
}

// Destructor
VariableArray::~VariableArray() {}

/**
 * @brief Initialize the VariableArray with variables and optional UUIDs.
 *
 * Sets the internal variable count and pointer array, recalculates the number of unique
 * sensors, associates provided UUIDs with each variable (if given), and validates all
 * variable UUIDs.
 *
 * @param variableCount Number of entries in `variableList`.
 * @param variableList Array of pointers to Variable objects; length must be `variableCount`.
 * @param uuids Optional array of C-strings containing UUIDs; if non-null, each element is
 *              assigned to the corresponding variable in `variableList`.
 */
void VariableArray::begin(uint8_t variableCount, Variable* variableList[],
                          const char* uuids[]) {
    _variableCount = variableCount;
    arrayOfVars    = variableList;

    _sensorCount = getSensorCount();
    matchUUIDs(uuids);
    checkVariableUUIDs();
}
/**
 * @brief Initialize the VariableArray with a list of variables and validate their UUIDs.
 *
 * Sets the internal variable count and array, recalculates the number of unique sensors,
 * and verifies the format and uniqueness of each variable's UUID.
 *
 * @param variableCount Number of entries in @p variableList.
 * @param variableList  Array of pointers to Variable instances to manage.
 */
void VariableArray::begin(uint8_t variableCount, Variable* variableList[]) {
    _variableCount = variableCount;
    arrayOfVars    = variableList;

    _sensorCount = getSensorCount();
    checkVariableUUIDs();
}
/**
 * @brief Recalculates the internal sensor count and validates all variable UUIDs.
 *
 * Recomputes the number of unique sensors based on the current variable list and runs UUID format
 * and uniqueness checks for each variable, updating internal state accordingly.
 */
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
/**
 * @brief Performs a measurement cycle for all sensors without altering their power or sleep states.
 *
 * Performs updates for every sensor, allowing concurrent measurements where supported, while leaving
 * sensor power and sleep/wake state unchanged.
 *
 * @return true if all sensor updates completed successfully, false otherwise.
 */
bool VariableArray::updateAllSensors(void) {
    return completeUpdate(false, false, false, false);
}

/**
 * @brief Performs a complete sensor measurement cycle, applies optional power/wake/sleep actions, averages measurements, and notifies variables.
 *
 * Executes a full update for every unique sensor managed by this VariableArray: clears previous results, optionally powers up sensors, optionally wakes sensors when warmed up, starts and collects single measurements until each sensor has completed its configured number of measurements, optionally puts sensors to sleep and powers them down when eligible, averages collected measurements, and updates/ notifies all associated variables including recalculation of calculated variables.
 *
 * @param powerUp If true, power up all sensors before starting measurements.
 * @param wake If true, attempt to wake sensors (once warmed up) prior to starting measurements.
 * @param sleep If true, put sensors to sleep after they finish all required measurements.
 * @param powerDown If true, power down sensors (and shared power pins) when no remaining sensors require power.
 * @return true if all invoked sensor operations (wake/start/result collection/sleep/power actions) succeeded for all sensors; `false` if any sensor operation failed.
 */
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
            sensorList[addedSensors++] = arrayOfVars[i]->parentSensor;
        }
    }
#if defined(MS_VARIABLEARRAY_DEBUG) || defined(MS_VARIABLEARRAY_DEBUG_DEEP)
    for (uint8_t i = 0; i < _sensorCount; i++) {
        MS_DBG(F("   Sensor"), i, F("is"),
               sensorList[i]->getSensorNameAndLocation());
    }
#endif

    // Clear the initial variable arrays
    MS_DBG(F("----->> Clearing all results arrays before taking new "
             "measurements. ..."));
    for (uint8_t i = 0; i < _sensorCount; i++) { sensorList[i]->clearValues(); }
    MS_DBG(F("   ... Complete. <<-----"));

    // power up all of the sensors together
    if (powerUp) {
        MS_DBG(F("----->> Powering up all sensors together. ..."));
        sensorsPowerUp();
        MS_DBG(F("   ... Complete. <<-----"));
    }

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
                    bool canPowerDown = false;
                    for (uint8_t k = 0; k < _sensorCount; k++) {
                        if (((sensorList[i]->getPowerPin() >= 0 &&
                              (sensorList[i]->getPowerPin() ==
                                   sensorList[k]->getPowerPin() ||
                               sensorList[i]->getPowerPin() ==
                                   sensorList[k]->getSecondaryPowerPin())) ||
                             (sensorList[i]->getSecondaryPowerPin() >= 0 &&
                              (sensorList[i]->getSecondaryPowerPin() ==
                                   sensorList[k]->getPowerPin() ||
                               sensorList[i]->getSecondaryPowerPin() ==
                                   sensorList[k]->getSecondaryPowerPin()))) &&
                            (sensorList[k]
                                 ->getNumberCompleteMeasurementsAttempts() <
                             sensorList[k]->getNumberMeasurementsToAverage())) {
                            canPowerDown = false;  // another sensor on this pin
                                                   // still needs to take
                                                   // measurements
                            break;
                        } else {
                            canPowerDown =
                                true;  // no other sensors on this pin need to
                                       // take measurements
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


/**
 * @brief Determines whether the variable at the given index is the last entry for its parent sensor.
 *
 * For calculated variables this always returns false since they do not belong to a sensor.
 *
 * @param arrayIndex Index of the variable within arrayOfVars.
 * @return true if no later variable in arrayOfVars references the same parent sensor, `false` otherwise.
 */
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


/**
 * @brief Retrieve a specific status bit from the sensor that owns the variable at the given index.
 *
 * @param arrayIndex Index of the variable in the array whose parent sensor's status bit will be read.
 * @param bitToGet The specific sensor status bit to retrieve.
 * @return bool `true` if the requested status bit is set on the parent sensor, `false` otherwise or if the index is out of range.
 */
bool VariableArray::getSensorStatusBit(int                        arrayIndex,
                                       Sensor::sensor_status_bits bitToGet) {
    if (arrayIndex < 0 || arrayIndex >= _variableCount) { return false; }
    return arrayOfVars[arrayIndex]->parentSensor->getStatusBit(bitToGet);
}


/**
 * @brief Validate assigned UUIDs for all variables and report issues.
 *
 * Checks each variable's UUID format if a UUID is present, detects non-unique
 * UUIDs among variables, and prints diagnostics. After validation it prints
 * all non-empty UUIDs alongside their variable codes.
 *
 * @return true if every assigned UUID is correctly formed and unique, false otherwise.
 */
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
        PRINTOUT(F("All variable UUID's appear to be correctly formed.\n"));
    // Print out all UUID's to check
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