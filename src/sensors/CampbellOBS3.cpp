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
#include "ProcessorAnalog.h"


// Primary constructor using AnalogVoltageBase abstraction
CampbellOBS3::CampbellOBS3(int8_t             powerPin,
                           AnalogVoltageBase* analogVoltageReader,
                           float x2_coeff_A, float x1_coeff_B, float x0_coeff_C,
                           uint8_t measurementsToAverage)
    : Sensor("CampbellOBS3", OBS3_NUM_VARIABLES, OBS3_WARM_UP_TIME_MS,
             OBS3_STABILIZATION_TIME_MS, OBS3_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, OBS3_INC_CALC_VARIABLES),
      _analogVoltageReader(analogVoltageReader),
      _ownsVoltageReader(false),
      _x2_coeff_A(x2_coeff_A),
      _x1_coeff_B(x1_coeff_B),
      _x0_coeff_C(x0_coeff_C) {}

// Constructor that creates a TIADS1x15 object internally
CampbellOBS3::CampbellOBS3(int8_t powerPin, uint8_t adsChannel,
                           float x2_coeff_A, float x1_coeff_B, float x0_coeff_C,
                           uint8_t measurementsToAverage,
                           float voltageMultiplier, adsGain_t adsGain,
                           uint8_t i2cAddress, float adsSupplyVoltage)
    : Sensor("CampbellOBS3", OBS3_NUM_VARIABLES, OBS3_WARM_UP_TIME_MS,
             OBS3_STABILIZATION_TIME_MS, OBS3_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, OBS3_INC_CALC_VARIABLES),
      _analogVoltageReader(nullptr),
      _ownsVoltageReader(true),
      _x2_coeff_A(x2_coeff_A),
      _x1_coeff_B(x1_coeff_B),
      _x0_coeff_C(x0_coeff_C) {
    // Create a TIADS1x15 object for analog voltage reading
    _analogVoltageReader =
        new TIADS1x15(powerPin, adsChannel, voltageMultiplier, adsGain,
                      i2cAddress, measurementsToAverage, adsSupplyVoltage);
}

// Constructor that creates a ProcessorAnalog object internally
CampbellOBS3::CampbellOBS3(int8_t powerPin, int8_t dataPin, float x2_coeff_A,
                           float x1_coeff_B, float x0_coeff_C,
                           float voltageMultiplier, float operatingVoltage,
                           uint8_t measurementsToAverage)
    : Sensor("CampbellOBS3", OBS3_NUM_VARIABLES, OBS3_WARM_UP_TIME_MS,
             OBS3_STABILIZATION_TIME_MS, OBS3_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, OBS3_INC_CALC_VARIABLES),
      _analogVoltageReader(nullptr),
      _ownsVoltageReader(true),
      _x2_coeff_A(x2_coeff_A),
      _x1_coeff_B(x1_coeff_B),
      _x0_coeff_C(x0_coeff_C) {
    // Create a ProcessorAnalog object for analog voltage reading
    _analogVoltageReader =
        new ProcessorAnalog(powerPin, dataPin, voltageMultiplier,
                            operatingVoltage, measurementsToAverage);
}
// Destructor
CampbellOBS3::~CampbellOBS3() {
    // Clean up owned voltage reader if created by constructor
    if (_ownsVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
        _analogVoltageReader = nullptr;
    }
}


String CampbellOBS3::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getSensorLocation();
    } else {
        // Fallback for cases where voltage reader is not set
        return F("CampbellOBS3_UnknownLocation");
    }
}


bool CampbellOBS3::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    // Ensure we have a valid voltage reader
    if (_analogVoltageReader == nullptr) {
        MS_DBG(F("  No analog voltage reader configured!"));
        return bumpMeasurementAttemptCount(false);
    }

    bool    success     = false;
    float   adcVoltage  = -9999;
    float   calibResult = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Print out the calibration curve
    MS_DBG(F("  Input calibration Curve:"), _x2_coeff_A, F("x^2 +"),
           _x1_coeff_B, F("x +"), _x0_coeff_C);

    // Use the abstracted voltage reading method
    success = _analogVoltageReader->readVoltageSingleEnded(adcVoltage);

    if (success) {
        // Apply the unique calibration curve for the given sensor
        calibResult = (_x2_coeff_A * sq(adcVoltage)) +
            (_x1_coeff_B * adcVoltage) + _x0_coeff_C;
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(OBS3_TURB_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(OBS3_VOLTAGE_VAR_NUM, adcVoltage);

    } else {
        MS_DBG(F("  Failed to read voltage from analog voltage reader"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
