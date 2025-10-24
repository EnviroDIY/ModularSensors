/**
 * @file SensorBase.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Sensor class.
 */

#include "SensorBase.h"
#include "VariableBase.h"

// ============================================================================
//  The class and functions for interfacing with a sensor
// ============================================================================

/**
 * @brief Initialize a Sensor with its configuration and reset internal state.
 *
 * Constructs a Sensor instance configured with pin assignments, timing parameters,
 * and averaging/return-value counts, and initializes internal arrays and counters
 * to their default (unset) values.
 *
 * @param sensorName Human-readable sensor name.
 * @param totalReturnedValues Number of values the sensor will produce (returned values).
 * @param warmUpTime_ms Milliseconds the sensor requires after power-up before use.
 * @param stabilizationTime_ms Milliseconds the sensor requires after wake before measurements are stable.
 * @param measurementTime_ms Milliseconds to wait for an individual measurement to complete.
 * @param powerPin Primary power control pin (-1 if not controlled).
 * @param dataPin Data pin used to read the sensor (may be -1 if unused).
 * @param measurementsToAverage Number of measurement samples to collect and average.
 * @param incCalcValues Number of additional calculated values maintained by the sensor (beyond returned values).
 *
 * @note All variable slots are initialized: registered variable pointers set to `nullptr`,
 *       `sensorValues` set to -9999, and `numberGoodMeasurementsMade` set to 0.
 */
Sensor::Sensor(const char* sensorName, const uint8_t totalReturnedValues,
               uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms,
               uint32_t measurementTime_ms, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage, uint8_t incCalcValues)
    : _dataPin(dataPin),
      _powerPin(powerPin),
      _powerPin2(-1),
      _sensorName(sensorName),
      _numReturnedValues(totalReturnedValues),
      _incCalcValues(incCalcValues),
      _measurementsToAverage(measurementsToAverage),
      _warmUpTime_ms(warmUpTime_ms),
      _stabilizationTime_ms(stabilizationTime_ms),
      _measurementTime_ms(measurementTime_ms) {
    // Clear arrays
    for (uint8_t i = 0; i < MAX_NUMBER_VARS; i++) {
        variables[i]                  = nullptr;
        sensorValues[i]               = -9999;
        numberGoodMeasurementsMade[i] = 0;
    }
}
// Destructor
Sensor::~Sensor() {}


// This gets the place the sensor is installed ON THE MAYFLY (ie, pin number)
String Sensor::getSensorLocation(void) {
    String senseLoc = F("Pin");
    senseLoc += String(_dataPin);
    return senseLoc;
}


// This returns the name of the sensor.
String Sensor::getSensorName(void) {
    return _sensorName;
}


// This concatenates and returns the name and location.
String Sensor::getSensorNameAndLocation(void) {
    return getSensorName() + " at " + getSensorLocation();
}


/**
 * @brief Returns the configured primary power pin number for the sensor.
 *
 * @return int8_t Primary power pin number, or -1 if no primary power pin is configured.
 */
int8_t Sensor::getPowerPin(void) {
    return _powerPin;
}
/**
 * @brief Set the GPIO pin used to control the sensor's primary power supply.
 *
 * @param pin GPIO pin number to drive HIGH/LOW for power control; set to -1 to indicate the library should not control sensor power.
 */
void Sensor::setPowerPin(int8_t pin) {
    _powerPin = pin;
}
/**
 * @brief Get the configured secondary power pin for the sensor.
 *
 * @return int8_t The secondary power pin number, or -1 if no secondary power pin is configured.
 */
int8_t Sensor::getSecondaryPowerPin(void) {
    return _powerPin2;
}
/**
 * @brief Sets the secondary power control pin for the sensor.
 *
 * @param pin Arduino pin number used to control the sensor's secondary power; use -1 to indicate no secondary power pin is controlled.
 */
void Sensor::setSecondaryPowerPin(int8_t pin) {
    _powerPin2 = pin;
}


// These functions get and set the number of readings to average for a sensor
// Generally these values should be set in the constructor
void Sensor::setNumberMeasurementsToAverage(uint8_t nReadings) {
    _measurementsToAverage = nReadings;
}
/**
 * @brief Retrieve the configured number of measurements used for averaging sensor readings.
 *
 * @return uint8_t The number of measurements the sensor will average for a single reported value.
 */
uint8_t Sensor::getNumberMeasurementsToAverage(void) {
    return _measurementsToAverage;
}
/**
 * @brief Retrieve how many measurement attempts have been completed.
 *
 * @return uint8_t The count of completed measurement attempts.
 */
uint8_t Sensor::getNumberCompleteMeasurementsAttempts(void) {
    return _measurementAttemptsCompleted;
}
/**
 * @brief Retrieve the number of retry attempts made for the current measurement cycle.
 *
 * @return uint8_t Number of retry attempts already performed.
 */
uint8_t Sensor::getNumberRetryAttemptsMade(void) {
    return _retryAttemptsMade;
}
/**
 * @brief Set how many retry attempts are allowed for a single measurement.
 *
 * @param allowedMeasurementRetries Maximum number of retry attempts to perform before a measurement attempt is considered complete.
 */
void Sensor::setAllowedMeasurementRetries(uint8_t allowedMeasurementRetries) {
    _allowedMeasurementRetries = allowedMeasurementRetries;
}
/**
 * @brief Get the configured maximum number of retry attempts for a measurement.
 *
 * @return The maximum number of allowed retry attempts for a single measurement.
 */
uint8_t Sensor::getAllowedMeasurementRetries(void) {
    return _allowedMeasurementRetries;
}


/**
 * @brief Set the sensor warm-up duration.
 *
 * Specifies how long (in milliseconds) the sensor must be powered before it is
 * considered warmed up and ready for activation or measurements.
 *
 * @param warmUpTime_ms Warm-up time in milliseconds.
 */
void Sensor::setWarmUpTime(uint32_t warmUpTime_ms) {
    _warmUpTime_ms = warmUpTime_ms;
}
/**
 * @brief Get the sensor warm-up duration.
 *
 * @return Warm-up time in milliseconds.
 */
uint32_t Sensor::getWarmUpTime(void) {
    return _warmUpTime_ms;
}
/**
 * @brief Sets the sensor stabilization delay after activation.
 *
 * @param stabilizationTime_ms Delay in milliseconds to wait after the sensor is activated before measurements are considered stable.
 */
void Sensor::setStabilizationTime(uint32_t stabilizationTime_ms) {
    _stabilizationTime_ms = stabilizationTime_ms;
}
/**
 * @brief Gets how long the sensor requires to stabilize after activation.
 *
 * @return The stabilization time in milliseconds. 
 */
uint32_t Sensor::getStabilizationTime(void) {
    return _stabilizationTime_ms;
}
/**
 * @brief Set the duration the sensor waits for a measurement to complete.
 *
 * @param measurementTime_ms Measurement timeout in milliseconds used to determine when a requested measurement is considered complete.
 */
void Sensor::setMeasurementTime(uint32_t measurementTime_ms) {
    _measurementTime_ms = measurementTime_ms;
}
/**
 * @brief Retrieves the configured measurement duration for the sensor.
 *
 * @return Measurement duration in milliseconds.
 */
uint32_t Sensor::getMeasurementTime(void) {
    return _measurementTime_ms;
}

// This returns the 8-bit code for the current status of the sensor.
// Bit 0 - 0=Has NOT been set up, 1=Has been setup
// Bit 1 - 0=No attempt made to power sensor, 1=Attempt made to power sensor
// Bit 2 - 0=Power up attempt failed, 1=Power up attempt succeeded
// Bit 3 - 0=Wake/Activate not yet attempted, 1=Attempt made to wake sensor
// Bit 4 - 0=Wake/Activate failed, 1=Is awake/actively measuring
// bit 5 - 0=Measurement start attempted, 1=No measurements have been requested
// bit 6 - 0=Measurement start failed, 1=Measurement attempt succeeded
// Bit 7 - 0=No known errors, 1=Some sort of error has occurred
uint8_t Sensor::getStatus(void) {
    return _sensorStatus;
}

bool Sensor::getStatusBit(sensor_status_bits bitToGet) {
    return bitRead(_sensorStatus, bitToGet);
}
void Sensor::setStatusBit(sensor_status_bits bitToSet) {
    _sensorStatus |= (1 << bitToSet);
}
void Sensor::clearStatusBit(sensor_status_bits bitToClear) {
    _sensorStatus &= ~(1 << bitToClear);
}


/**
 * @brief Powers the sensor hardware and records its power-on time.
 *
 * Attempts to drive configured primary and secondary power pins high (if set),
 * records the time the sensor was powered, and marks power-attempt and
 * power-success status bits.
 *
 * If no power pins are managed by the library, this still ensures the internal
 * power-on timestamp is set if it was previously zero and marks the same
 * status bits.
 */
void Sensor::powerUp(void) {
    if (_powerPin >= 0 || _powerPin2 >= 0) {
        // Reset power pin mode every power up because pins are set to tri-state
        // on sleep on SAMD boards
        if (_powerPin >= 0) {
            pinMode(_powerPin, OUTPUT);
            MS_DBG(F("Powering"), getSensorNameAndLocation(), F("with pin"),
                   _powerPin);
            digitalWrite(_powerPin, HIGH);
        }
        if (_powerPin2 >= 0) {
            pinMode(_powerPin2, OUTPUT);
            MS_DBG(F("Giving secondary power to"), getSensorNameAndLocation(),
                   F("with pin"), _powerPin2);
            digitalWrite(_powerPin2, HIGH);
        }
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    } else {
        MS_DBG(F("Power to"), getSensorNameAndLocation(),
               F("is not controlled by this library."));
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    setStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL);
}


/**
 * @brief Turns off sensor power and clears related timestamps and status bits.
 *
 * If one or more configured power pins are controlled by the library, drives
 * those pins LOW, resets internal timestamps for power-on, activation, and
 * measurement request, and clears the POWER_ATTEMPTED, POWER_SUCCESSFUL,
 * WAKE_ATTEMPTED, WAKE_SUCCESSFUL, MEASUREMENT_ATTEMPTED, and
 * MEASUREMENT_SUCCESSFUL status bits.
 *
 * If no power pin is managed by the library, no hardware or state is modified.
 */
void Sensor::powerDown(void) {
    if (_powerPin >= 0 || _powerPin2 >= 0) {
        // Reset power pin mode every power up because pins are set to tri-state
        // on sleep on SAMD boards
        if (_powerPin >= 0) {
            pinMode(_powerPin, OUTPUT);
            MS_DBG(F("Turning off"), getSensorNameAndLocation(), F("with pin"),
                   _powerPin);
            digitalWrite(_powerPin, LOW);
        }
        if (_powerPin2 >= 0) {
            pinMode(_powerPin2, OUTPUT);
            MS_DBG(F("Turning off secondary power to"),
                   getSensorNameAndLocation(), F("with pin"), _powerPin2);
            digitalWrite(_powerPin2, LOW);
        }
        // Unset the power-on time
        _millisPowerOn = 0;
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        clearStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL, WAKE_ATTEMPTED,
                        WAKE_SUCCESSFUL, MEASUREMENT_ATTEMPTED,
                        MEASUREMENT_SUCCESSFUL);
    } else {
        MS_DBG(F("Power to"), getSensorNameAndLocation(),
               F("is not controlled by this library."));
        // Do NOT unset any status bits or timestamps if we didn't really power
        // down!
    }
}


// The function to set up connection to a sensor.
// By default, sets pin modes and returns true
bool Sensor::setup(void) {
    MS_DBG(F("Setting up"), getSensorName(), F("attached at"),
           getSensorLocation(), F("which can return up to"), _numReturnedValues,
           F("variable[s]"), _incCalcValues,
           F("of which are calculated internally."));

    MS_DBG(F("It warms up in"), _warmUpTime_ms, F("ms, is stable after"),
           _stabilizationTime_ms, F("ms, and takes a single measurement in"),
           _measurementTime_ms, F("ms."));

    MS_DBG(_measurementsToAverage,
           F("individual measurements will be averaged for each reading."));

    if (_powerPin >= 0) {
        pinMode(_powerPin, OUTPUT);
    }  // NOTE:  Not setting value
    if (_dataPin >= 0) {
        pinMode(_dataPin, INPUT);
    }  // NOTE:  Not turning on processor pull-up or pull-down!

    // Set the status bit marking that the sensor has been set up (bit 0)
    setStatusBit(SETUP_SUCCESSFUL);

    return true;
}


/**
 * @brief Attempts to activate the sensor so it is ready for measurements.
 *
 * Performs observable wake actions: records a wake attempt (sets WAKE_ATTEMPTED),
 * configures the sensor data pin as an input if present, and, when power is
 * available, marks the sensor as activated (sets WAKE_SUCCESSFUL) and updates
 * the sensor activation timestamp.
 *
 * @return bool `true` if the sensor is marked activated and ready, `false` if
 * power is not available (the activation timestamp and WAKE_SUCCESSFUL are
 * cleared in this case).
 */
bool Sensor::wake(void) {
    MS_DBG(F("Waking"), getSensorNameAndLocation(), F("by doing nothing!"));
    // Set the status bit for sensor activation attempt (bit 3)
    // Setting this bit even if the activation failed, to show the attempt was
    // made
    setStatusBit(WAKE_ATTEMPTED);

    // Check if the sensor was successfully powered
    if (!getStatusBit(POWER_SUCCESSFUL)) {
        MS_DBG(getSensorNameAndLocation(),
               F("doesn't have power and will never wake up!"));
        // Make sure that the wake time and wake success bit (bit 4) are unset
        _millisSensorActivated = 0;
        clearStatusBit(WAKE_SUCCESSFUL);
        return false;
    }
    // Set the data pin mode on every wake because pins are set to tri-state on
    // sleep on SAMD boards
    if (_dataPin >= 0) {
        pinMode(_dataPin, INPUT);
    }  // NOTE:  Not turning on processor pull-up or pull-down!

    // Mark the time that the sensor was activated
    // NOTE: If we didn't do anything to wake the sensor, we **don't**
    // want to mark the time as **now** but as the last time we did do
    // something.  Since we didn't actively wake the sensor, we assume the
    // measurement was started at power on.
    _millisSensorActivated = _millisPowerOn;
    // Set the status bit for sensor wake/activation success (bit 4)
    setStatusBit(WAKE_SUCCESSFUL);

    return true;
}


// The function to put a sensor to sleep
// Does NOT power down the sensor!
bool Sensor::sleep(void) {
    // If nothing needs to be done to make the sensor go to sleep, we'll leave
    // the bits and time stamps set because running the sleep function doesn't
    // do anything.  If the sensor has a power pin and it is powered down, then
    // the activation/wake bits will be unset by the powerDown() function.
    return true;
}


// This is a place holder for starting a single measurement, for those sensors
/**
 * @brief Initiates a single measurement request for sensors that require no explicit trigger.
 *
 * Attempts setup if the sensor was never set up, marks that a measurement attempt occurred,
 * and — if the sensor is awake — records the measurement request time (based on the last
 * completed measurement or the sensor activation time) and marks the measurement as started.
 *
 * @return `true` if the sensor was awake and the measurement request was recorded, `false` otherwise.
 */
bool Sensor::startSingleMeasurement(void) {
    bool success = true;

    // check if the sensor was successfully set up, run set up if not
    // NOTE:  We continue regardless of the success of this attempt
    if (!getStatusBit(SETUP_SUCCESSFUL)) {
        MS_DBG(getSensorNameAndLocation(),
               F("was never properly set up, attempting setup now!"));
        setup();
    }

    MS_DBG(F("Starting measurement on"), getSensorNameAndLocation(),
           F("by doing nothing!"));
    // Set the status bits for measurement requested (bit 5)
    // Setting this bit even if we failed to start a measurement to show that an
    // attempt was made.
    setStatusBit(MEASUREMENT_ATTEMPTED);

    // Check if there was a successful wake (bit 4 set)
    // Only mark the measurement request time if it is
    if (getStatusBit(WAKE_SUCCESSFUL)) {
        // Mark the time that a measurement was requested
        // NOTE: If we didn't do anything to start a measurement, we **don't**
        // want to mark the time as **now** but as the last time we did do
        // something.  Since we didn't actively start the measurement, we assume
        // the measurement was started either at wake or at the time the last
        // measurement was finished.
        if (_millisMeasurementCompleted != 0) {
            _millisMeasurementRequested =
                _millisMeasurementCompleted;  // immediately after last
                                              // measurement
        } else {
            _millisMeasurementRequested = _millisSensorActivated;  // at wake
        }
        // Set the status bit for measurement start success (bit 6)
        setStatusBit(MEASUREMENT_SUCCESSFUL);
    } else {
        // Otherwise, make sure that the measurement start time and success bit
        // (bit 6) are unset
        MS_DBG(getSensorNameAndLocation(),
               F("isn't awake/active!  A measurement cannot be started."));
        _millisMeasurementRequested = 0;
        clearStatusBit(MEASUREMENT_SUCCESSFUL);
        success = false;
    }
    return success;
}


void Sensor::registerVariable(int sensorVarNum, Variable* var) {
    variables[sensorVarNum] = var;
}

void Sensor::notifyVariables(void) {
    MS_DBG(F("Notifying variables registered to"), getSensorNameAndLocation(),
           F("of value update."));

    // Notify variables of update
    for (uint8_t i = 0; i < _numReturnedValues; i++) {
        if (variables[i] != nullptr) {
            // Bad things happen if try to update nullptr
            MS_DBG(F("Sending value update from"), getSensorNameAndLocation(),
                   F("to variable"), i, F("which is"),
                   variables[i]->getVarName(), F("..."));
            variables[i]->onSensorUpdate(this);
        } else {
            MS_DBG(getSensorNameAndLocation(),
                   F("has no variable registered for return value"), i,
                   F("!  No update sent!"));
        }
    }
}


/**
 * @brief Resets sensor measurement state and clears stored values.
 *
 * Clears all returned-value slots and their per-value counters, resets
 * measurement-attempt and retry counters, clears power/wake/measurement
 * timestamps, and clears status bits related to power, wake, and measurement
 * while preserving setup and error status bits.
 */
void Sensor::clearValues(void) {
    MS_DBG(F("Clearing value array for"), getSensorNameAndLocation());
    for (uint8_t i = 0; i < _numReturnedValues; i++) {
        sensorValues[i]               = -9999;
        numberGoodMeasurementsMade[i] = 0;
    }
    // Reset measurement attempt counters
    _measurementAttemptsCompleted = 0;
    _retryAttemptsMade            = 0;
    // reset all timing values
    _millisPowerOn              = 0;
    _millisSensorActivated      = 0;
    _millisMeasurementRequested = 0;
    _millisMeasurementCompleted = 0;
    // Unset all status bits except setup (bit 0) and error (bit 7)
    clearStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL, WAKE_ATTEMPTED,
                    WAKE_SUCCESSFUL, MEASUREMENT_ATTEMPTED,
                    MEASUREMENT_SUCCESSFUL);
}


// This verifies that a measurement is good before adding it to the values to be
// averaged
void Sensor::verifyAndAddMeasurementResult(uint8_t resultNumber,
                                           float   resultValue) {
    // If the new result is good and there was were only bad results, set the
    // result value as the new result and add 1 to the good result total
    if ((sensorValues[resultNumber] == -9999 ||
         isnan(sensorValues[resultNumber])) &&
        (resultValue != -9999 && !isnan(resultValue))) {
        MS_DBG(F("Putting"), resultValue, F("in result array for variable"),
               resultNumber, F("from"), getSensorNameAndLocation());
        sensorValues[resultNumber] = resultValue;
        numberGoodMeasurementsMade[resultNumber] += 1;
    } else if ((sensorValues[resultNumber] == -9999 ||
                isnan(sensorValues[resultNumber])) &&
               (resultValue != -9999 && !isnan(resultValue))) {
        // If the new result is good and there were already good results in
        // place add the new results to the total and add 1 to the good result
        // total
        MS_DBG(F("Adding"), resultValue, F("to result array for variable"),
               resultNumber, F("from"), getSensorNameAndLocation());
        sensorValues[resultNumber] += resultValue;
        numberGoodMeasurementsMade[resultNumber] += 1;
    } else if (sensorValues[resultNumber] == -9999 && resultValue == -9999) {
        // If the new result is bad and there were only bad results, do nothing
        MS_DBG(F("Ignoring bad result for variable"), resultNumber, F("from"),
               getSensorNameAndLocation(), F("; no good results yet."));
    } else if ((sensorValues[resultNumber] == -9999 ||
                isnan(sensorValues[resultNumber])) &&
               resultValue == -9999) {
        // If the new result is bad and there were already good results, do
        // nothing
        MS_DBG(F("Ignoring bad result for variable"), resultNumber, F("from"),
               getSensorNameAndLocation(),
               F("; good results already in array."));
    }
}
/// @todo Fix measurement value array to handle int16_t and int32_t directly so
/// no casting is needed and large values will not be truncated or mashed.
void Sensor::verifyAndAddMeasurementResult(uint8_t resultNumber,
                                           int16_t resultValue) {
    float float_val = resultValue;  // cast the int16_t to a float
    verifyAndAddMeasurementResult(resultNumber, float_val);
}
void Sensor::verifyAndAddMeasurementResult(uint8_t resultNumber,
                                           int32_t resultValue) {
    auto float_val =
        static_cast<float>(resultValue);  // cast the int32_t to a float
    verifyAndAddMeasurementResult(resultNumber, float_val);
}


void Sensor::averageMeasurements(void) {
    MS_DBG(F("Averaging results from"), getSensorNameAndLocation(), F("over"),
           _measurementsToAverage, F("reading[s]"));
    for (uint8_t i = 0; i < _numReturnedValues; i++) {
        if (numberGoodMeasurementsMade[i] > 0)
            sensorValues[i] /= numberGoodMeasurementsMade[i];
        MS_DBG(F("    ->Result #"), i, ':', sensorValues[i]);
    }
}


// This updates a sensor value by checking it's power, waking it, taking as many
// readings as requested, then putting the sensor to sleep and powering down.
bool Sensor::update(void) {
    bool ret_val = true;

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }

    // Check if it's awake/active, activate it if not
    bool wasActive = getStatusBit(WAKE_SUCCESSFUL);
    if (!wasActive) {
        // NOT yet awake
        // wait for the sensor to have been powered for long enough to respond
        waitForWarmUp();
        ret_val &= wake();
    }
    // bail if the wake failed
    if (!ret_val) return ret_val;

    // Clear values before starting loop
    clearValues();

    // Wait for the sensor to stabilize
    waitForStability();

    // loop through as many measurements as requested
    for (uint8_t j = 0; j < _measurementsToAverage; j++) {
        // start a measurement
        ret_val &= startSingleMeasurement();
        // wait for the measurement to finish
        waitForMeasurementCompletion();
        // get the measurement result
        ret_val &= addSingleMeasurementResult();
    }

    averageMeasurements();

    // Put the sensor back to sleep if it had been activated
    if (wasActive) { sleep(); }

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    // Update the registered variables with the new values
    notifyVariables();

    return ret_val;
}


/**
 * @brief Checks whether the sensor's power is currently on and updates internal state accordingly.
 *
 * @param debug If true, emit diagnostic messages during the check.
 * @return true if power is detected or (when no power pins are managed) considered on; false if power is off.
 *
 * @details
 * If no power pins are managed by the library, the function marks the sensor as powered and records the power-on time.
 * When power is present it ensures the power-on timestamp is set and sets POWER_ATTEMPTED and POWER_SUCCESSFUL status bits.
 * When power is absent it clears the power-on, activation, and measurement-request timestamps and clears power/wake/measurement related status bits.
 */
bool Sensor::checkPowerOn(bool debug) {
    if (debug) {
        MS_DBG(F("Checking power status:  Power to"),
               getSensorNameAndLocation());
    }
    if (_powerPin < 0 && _powerPin2 < 0) {
        if (debug) { MS_DBG(F("is not controlled by this library.")); }
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
        // Set the status bit for sensor power attempt (bit 1) and success (bit
        // 2)
        setStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL);
        return true;
    }
    bool pp1_off = false;
    bool pp2_off = false;
    if (_powerPin >= 0) {
        auto powerBitNumber =
            static_cast<int8_t>(log(digitalPinToBitMask(_powerPin)) / log(2));
        pp1_off = bitRead(*portInputRegister(digitalPinToPort(_powerPin)),
                          powerBitNumber) == LOW;
    }
    if (_powerPin2 >= 0) {
        auto powerBitNumber2 =
            static_cast<int8_t>(log(digitalPinToBitMask(_powerPin2)) / log(2));
        pp2_off = bitRead(*portInputRegister(digitalPinToPort(_powerPin2)),
                          powerBitNumber2) == LOW;
    }

    if (pp1_off || pp2_off) {
        if (debug) { MS_DBG(F("was off.")); }
        // Unset time of power on, in-case it was set to a value
        _millisPowerOn = 0;
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        clearStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL, WAKE_ATTEMPTED,
                        WAKE_SUCCESSFUL, MEASUREMENT_ATTEMPTED,
                        MEASUREMENT_SUCCESSFUL);
        return false;
    } else {
        if (debug) { MS_DBG(" was on."); }
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
        // Set the status bit for sensor power attempt (bit 1) and success
        // (bit 2)
        setStatusBits(POWER_ATTEMPTED, POWER_SUCCESSFUL);
        return true;
    }
}


// This checks to see if enough time has passed for warm-up
bool Sensor::isWarmedUp(bool debug) {
    // If the sensor doesn't have power, then it will never be warmed up,
    // so the warm up time is essentially already passed.
    if (!getStatusBit(POWER_SUCCESSFUL)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("does not have power and cannot warm up!"));
        }
        return true;
    }

    uint32_t elapsed_since_power_on = millis() - _millisPowerOn;
    // If the sensor has power and enough time has elapsed, it's warmed up
    if (elapsed_since_power_on > _warmUpTime_ms) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_power_on, F("ms, and"),
                   getSensorNameAndLocation(), F("should be warmed up!"));
        }
        return true;
    } else {
        // If the sensor has power but the time hasn't passed, we still need to
        // wait
        return false;
    }
}

// This delays until enough time has passed for the sensor to "warm up" - that
// is - to be ready to communicate and to be asked to take readings
// NOTE:  This is "blocking" - that is, nothing else can happen during this
// wait.
void Sensor::waitForWarmUp(void) {
    while (!isWarmedUp()) {
        // wait
    }
}


/**
 * @brief Determine whether the sensor is currently stable and ready for measurement.
 *
 * Returns `true` when the sensor should be considered stable: if the sensor is not active (wake not successful),
 * if a retry attempt is in progress, or if the elapsed time since activation is greater than the configured
 * stabilization time. Returns `false` while the sensor is active and the required stabilization interval has not elapsed.
 *
 * @param debug If `true`, emit debug messages describing stability checks.
 * @return bool `true` if the sensor should be considered stable (ready), `false` otherwise.
 */
bool Sensor::isStable(bool debug) {
    // If the sensor failed to activate, it will never stabilize, so the
    // stabilization time is essentially already passed
    if (!getStatusBit(WAKE_SUCCESSFUL)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("is not active and cannot stabilize!"));
        }
        return true;
    }

    // If we're taking a repeat measurement, we may have already waited for
    // stabilization after the initial wake, so we can skip this wait.
    if (_retryAttemptsMade != 0) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("is retrying and doesn't need to stabilize again."));
        }
        return true;
    }

    uint32_t elapsed_since_wake_up = millis() - _millisSensorActivated;
    // If the sensor has been activated and enough time has elapsed, it's stable
    if (elapsed_since_wake_up > _stabilizationTime_ms) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_wake_up, F("ms, and"),
                   getSensorNameAndLocation(), F("should be stable!"));
        }
        return true;
    } else {
        // If the sensor has been activated but the time hasn't passed, we still
        // need to wait
        return false;
    }
}

// This delays until enough time has passed for the sensor to stabilize before
// taking readings
// NOTE:  This is "blocking" - that is, nothing else can happen during this
// wait.
void Sensor::waitForStability(void) {
    while (!isStable()) {
        // wait
    }
}


// This checks to see if enough time has passed for measurement completion
bool Sensor::isMeasurementComplete(bool debug) {
    // If a measurement failed to start, the sensor will never return a result,
    // so the measurement time is essentially already passed
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("is not measuring and will not return a value!"));
        }
        return true;
    }

    uint32_t elapsed_since_meas_start = millis() - _millisMeasurementRequested;
    // If the sensor is measuring and enough time has elapsed, the reading is
    // finished
    if (elapsed_since_meas_start > _measurementTime_ms) {
        if (debug) {
            MS_DBG(F("It's been"), elapsed_since_meas_start,
                   F("ms, and measurement by"), getSensorNameAndLocation(),
                   F("should be complete!"));
        }
        return true;
    } else {
        // If the sensor is measuring but the time hasn't passed, we still need
        // to wait
        return false;
    }
}

// This delays until enough time has passed for the sensor to give a new value
// NOTE:  This is "blocking" - that is, nothing else can happen during this
/**
 * @brief Block until the current measurement has finished.
 *
 * Spins until the sensor reports the measurement is complete.
 */
void Sensor::waitForMeasurementCompletion(void) {
    while (!isMeasurementComplete()) {
        // wait
    }
}


/**
 * @brief Update measurement-attempt bookkeeping after a measurement attempt.
 *
 * Records the completion timestamp, clears the measurement-request timestamp and related
 * status bits, increments the retry-attempt counter, and—if the attempt succeeded or
 * the retry count exceeded the allowed retries—increments the completed-attempts
 * counter and resets the retry counter.
 *
 * @param wasSuccessful `true` if the measurement attempt produced a valid result.
 * @return `true` if the measurement was successful, `false` otherwise.
 */
bool Sensor::bumpMeasurementAttemptCount(bool wasSuccessful) {
    // Record the time that the measurement was completed
    _millisMeasurementCompleted = millis();
    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    clearStatusBits(MEASUREMENT_ATTEMPTED, MEASUREMENT_SUCCESSFUL);
    // Bump the number of attempted retries
    _retryAttemptsMade++;

    if (wasSuccessful || _retryAttemptsMade > _allowedMeasurementRetries) {
        // Bump the number of completed measurement attempts - we've succeeded
        // or failed but exceeded retries
        _measurementAttemptsCompleted++;
        // Reset the number of retries made for the next measurement attempt
        _retryAttemptsMade = 0;
    }
    return wasSuccessful;
}