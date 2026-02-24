/**
 * @file CampbellOBS3.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the CampbellOBS3 class.
 */


#include "CampbellOBS3.h"
#include "TIADS1x15.h"


// The constructor - need the power pin, the data pin, and the calibration info
CampbellOBS3::CampbellOBS3(int8_t powerPin, int8_t analogChannel,
                           float x2_coeff_A, float x1_coeff_B, float x0_coeff_C,
                           uint8_t            measurementsToAverage,
                           AnalogVoltageBase* analogVoltageReader)
    : Sensor("CampbellOBS3", OBS3_NUM_VARIABLES, OBS3_WARM_UP_TIME_MS,
             OBS3_STABILIZATION_TIME_MS, OBS3_MEASUREMENT_TIME_MS, powerPin,
             analogChannel, measurementsToAverage, OBS3_INC_CALC_VARIABLES),
      _x2_coeff_A(x2_coeff_A),
      _x1_coeff_B(x1_coeff_B),
      _x0_coeff_C(x0_coeff_C),
      // If no analog voltage reader was provided, create a default one
      _analogVoltageReader(analogVoltageReader ? analogVoltageReader
                                               : new TIADS1x15Base()),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {}

// Destructor
CampbellOBS3::~CampbellOBS3() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
        _analogVoltageReader = nullptr;
    }
}


String CampbellOBS3::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getSensorLocation() + F("_Channel") +
            String(_dataPin);
    } else {
        String sensorLocation = F("Unknown_AnalogVoltageReader_Channel");
        sensorLocation += String(_dataPin);
        return sensorLocation;
    }
}


bool CampbellOBS3::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    // Check if we have a valid analog voltage reader
    if (_analogVoltageReader == nullptr) {
        MS_DBG(getSensorNameAndLocation(),
               F("No analog voltage reader available"));
        return bumpMeasurementAttemptCount(false);
    }

    bool  success    = false;
    float adcVoltage = -9999.0f;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Print out the calibration curve
    MS_DBG(F("  Input calibration Curve:"), _x2_coeff_A, F("x^2 +"),
           _x1_coeff_B, F("x +"), _x0_coeff_C);

    // Read the single-ended analog voltage using the AnalogVoltageBase
    // interface.
    // NOTE: All implementations of the AnalogVoltageBase class validate both
    // the input channel and the resulting voltage, so we can trust that a
    // successful read will give us a valid voltage value to work with.
    success = _analogVoltageReader->readVoltageSingleEnded(_dataPin,
                                                           adcVoltage);
    if (success) {
        // Apply the unique calibration curve for the given sensor
        float calibResult = (_x2_coeff_A * sq(adcVoltage)) +
            (_x1_coeff_B * adcVoltage) + _x0_coeff_C;
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(OBS3_TURB_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(OBS3_VOLTAGE_VAR_NUM, adcVoltage);
    } else {
        MS_DBG(F("  Failed to get valid voltage from analog reader"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
