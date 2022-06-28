/**
 * @file SensorBase.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Sensor class.
 */

#include "SensorBase.h"
#include "VariableBase.h"

// ============================================================================
//  The class and functions for interfacing with a sensor
// ============================================================================

// The constructor
Sensor::Sensor(const char* sensorName, const uint8_t totalReturnedValues,
               uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms,
               uint32_t measurementTime_ms, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage, uint8_t incCalcValues)
    : _dataPin(dataPin),
      _powerPin(powerPin),
      _sensorName(sensorName),
      _numReturnedValues(totalReturnedValues),
      _measurementsToAverage(measurementsToAverage),
      _incCalcValues(incCalcValues),
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


// This concatentates and returns the name and location.
String Sensor::getSensorNameAndLocation(void) {
    return getSensorName() + " at " + getSensorLocation();
}


// This returns the number of the power pin
int8_t Sensor::getPowerPin(void) {
    return _powerPin;
}


// These functions get and set the number of readings to average for a sensor
// Generally these values should be set in the constructor
void Sensor::setNumberMeasurementsToAverage(uint8_t nReadings) {
    _measurementsToAverage = nReadings;
}
uint8_t Sensor::getNumberMeasurementsToAverage(void) {
    return _measurementsToAverage;
}


// This returns the 8-bit code for the current status of the sensor.
// Bit 0 - 0=Has NOT been set up, 1=Has been setup
// Bit 1 - 0=No attempt made to power sensor, 1=Attempt made to power sensor
// Bit 2 - 0=Power up attampt failed, 1=Power up attempt succeeded
// Bit 3 - 0=Wake/Activate not yet attempted, 1=Attempt made to wake sensor
// Bit 4 - 0=Wake/Activate failed, 1=Is awake/actively measuring
// bit 5 - 0=Measurement start attempted, 1=No measurements have been requested
// bit 6 - 0=Measurement start failed, 1=Measurement attempt succeeded
// Bit 7 - 0=No known errors, 1=Some sort of error has occured
uint8_t Sensor::getStatus(void) {
    return _sensorStatus;
}


// This turns on sensor power
void Sensor::powerUp(void) {
    if (_powerPin >= 0) {
        MS_DBG(F("Powering"), getSensorNameAndLocation(), F("with pin"),
               _powerPin);
        // Set the pin mode, just in case
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    } else {
        MS_DBG(F("Power to"), getSensorNameAndLocation(),
               F("is not controlled by this library."));
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    _sensorStatus |= 0b00000110;
}


// This turns off sensor power
void Sensor::powerDown(void) {
    if (_powerPin >= 0) {
        MS_DBG(F("Turning off power to"), getSensorNameAndLocation(),
               F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000001;
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

    if (_powerPin >= 0) pinMode(_powerPin, OUTPUT);  // NOTE:  Not setting value
    if (_dataPin >= 0)
        pinMode(_dataPin, INPUT);  // NOTE:  Not turning on pull-up!

    // Set the status bit marking that the sensor has been set up (bit 0)
    _sensorStatus |= 0b00000001;

    return true;
}


// The function to wake up a sensor
bool Sensor::wake(void) {
    MS_DBG(F("Waking"), getSensorNameAndLocation());
    // Set the status bit for sensor activation attempt (bit 3)
    // Setting this bit even if the activation failed, to show the attempt was
    // made
    _sensorStatus |= 0b00001000;

    // Check if the sensor was successfully powered
    if (!bitRead(_sensorStatus, 2)) {
        MS_DBG(getSensorNameAndLocation(),
               F("doesn't have power and will never wake up!"));
        // Make sure that the wake time and wake success bit (bit 4) are unset
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
        return false;
    }

    // Mark the time that the sensor was activated
    _millisSensorActivated = millis();
    // Set the status bit for sensor wake/activation success (bit 4)
    _sensorStatus |= 0b00010000;

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
// that need no instructions to start a measurement.
bool Sensor::startSingleMeasurement(void) {
    bool success = true;

    // check if the sensor was successfully set up, run set up if not
    // NOTE:  We continue regardless of the success of this attempt
    if (!bitRead(_sensorStatus, 0)) {
        MS_DBG(getSensorNameAndLocation(),
               F("was never properly set up, attempting setup now!"));
        setup();
    }

    MS_DBG(F("Starting measurement on"), getSensorNameAndLocation());
    // Set the status bits for measurement requested (bit 5)
    // Setting this bit even if we failed to start a measurement to show that an
    // attempt was made.
    _sensorStatus |= 0b00100000;

    // Check if there was a successful wake (bit 4 set)
    // Only mark the measurement request time if it is
    if (bitRead(_sensorStatus, 4)) {
        // Mark the time that a measurement was requested
        _millisMeasurementRequested = millis();
        // Set the status bit for measurement start success (bit 6)
        _sensorStatus |= 0b01000000;
    } else {
        // Otherwise, make sure that the measurement start time and success bit
        // (bit 6) are unset
        MS_DBG(getSensorNameAndLocation(),
               F("isn't awake/active!  A measurement cannot be started."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
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


// This function just empties the value array
void Sensor::clearValues(void) {
    MS_DBG(F("Clearing value array for"), getSensorNameAndLocation());
    for (uint8_t i = 0; i < _numReturnedValues; i++) {
        sensorValues[i]               = -9999;
        numberGoodMeasurementsMade[i] = 0;
    }
}


// This verifies that a measurement is good before adding it to the values to be
// averaged
void Sensor::verifyAndAddMeasurementResult(uint8_t resultNumber,
                                           float   resultValue) {
    // If the new result is good and there was were only bad results, set the
    // result value as the new result and add 1 to the good result total
    if (sensorValues[resultNumber] == -9999 && resultValue != -9999) {
        MS_DBG(F("Putting"), resultValue, F("in result array for variable"),
               resultNumber, F("from"), getSensorNameAndLocation());
        sensorValues[resultNumber] = resultValue;
        numberGoodMeasurementsMade[resultNumber] += 1;
    } else if (sensorValues[resultNumber] != -9999 && resultValue != -9999) {
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
    } else if (sensorValues[resultNumber] != -9999 && resultValue == -9999) {
        // If the new result is bad and there were already good results, do
        // nothing
        MS_DBG(F("Ignoring bad result for variable"), resultNumber, F("from"),
               getSensorNameAndLocation(),
               F("; good results already in array."));
    }
}
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
    bool wasActive = bitRead(getStatus(), 3);
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


// This is a helper function to check if the power needs to be turned on
bool Sensor::checkPowerOn(bool debug) {
    if (debug) {
        MS_DBG(F("Checking power status:  Power to"),
               getSensorNameAndLocation());
    }
    if (_powerPin >= 0) {
        auto powerBitNumber =
            static_cast<int8_t>(log(digitalPinToBitMask(_powerPin)) / log(2));

        if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)),
                    powerBitNumber) == LOW) {
            if (debug) { MS_DBG(F("was off.")); }
            // Reset time of power on, in-case it was set to a value
            _millisPowerOn = 0;
            // Unset the status bits for sensor power (bits 1 & 2),
            // activation (bits 3 & 4), and measurement request (bits 5 & 6)
            _sensorStatus &= 0b10000001;
            return false;
        } else {
            if (debug) { MS_DBG(" was on."); }
            // Mark the power-on time, just in case it  had not been marked
            if (_millisPowerOn == 0) _millisPowerOn = millis();
            // Set the status bit for sensor power attempt (bit 1) and success
            // (bit 2)
            _sensorStatus |= 0b00000110;
            return true;
        }
    } else {
        if (debug) { MS_DBG(F("is not controlled by this library.")); }
        // Mark the power-on time, just in case it  had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
        // Set the status bit for sensor power attempt (bit 1) and success (bit
        // 2)
        _sensorStatus |= 0b00000110;
        return true;
    }
}


// This checks to see if enough time has passed for warm-up
bool Sensor::isWarmedUp(bool debug) {
    // If the sensor doesn't have power, then it will never be warmed up,
    // so the warm up time is essentially already passed.
    if (!bitRead(_sensorStatus, 2)) {
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


// This checks to see if enough time has passed for stability
bool Sensor::isStable(bool debug) {
    // If the sensor failed to activate, it will never stabilize, so the
    // stabilization time is essentially already passed
    if (!bitRead(_sensorStatus, 4)) {
        if (debug) {
            MS_DBG(getSensorNameAndLocation(),
                   F("is not active and cannot stabilize!"));
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
    if (!bitRead(_sensorStatus, 6)) {
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
// wait.
void Sensor::waitForMeasurementCompletion(void) {
    while (!isMeasurementComplete()) {
        // wait
    }
}
