/**
 * @file TurnerCyclops.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the TurnerCyclops class.
 */


#include "TurnerCyclops.h"
#include "TIADS1x15.h"


// The constructor - need the power pin, the data pin, and the calibration info
TurnerCyclops::TurnerCyclops(int8_t powerPin, int8_t analogChannel,
                             float conc_std, float volt_std, float volt_blank,
                             uint8_t            measurementsToAverage,
                             AnalogVoltageBase* analogVoltageReader)
    : Sensor("TurnerCyclops", CYCLOPS_NUM_VARIABLES, CYCLOPS_WARM_UP_TIME_MS,
             CYCLOPS_STABILIZATION_TIME_MS, CYCLOPS_MEASUREMENT_TIME_MS,
             powerPin, analogChannel, measurementsToAverage,
             CYCLOPS_INC_CALC_VARIABLES),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank),
      // If no analog voltage reader was provided, create a default one
      _analogVoltageReader(analogVoltageReader == nullptr
                               ? new TIADS1x15Base()
                               : analogVoltageReader),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {}

// Destructor
TurnerCyclops::~TurnerCyclops() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
    }
}


String TurnerCyclops::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getAnalogLocation(_dataPin, -1);
    } else {
        return String(F("Unknown_AnalogVoltageReader"));
    }
}


bool TurnerCyclops::setup(void) {
    bool sensorSetupSuccess         = Sensor::setup();
    bool analogVoltageReaderSuccess = false;

    if (_analogVoltageReader != nullptr) {
        analogVoltageReaderSuccess = _analogVoltageReader->begin();
        if (!analogVoltageReaderSuccess) {
            MS_DBG(getSensorNameAndLocation(),
                   F("Analog voltage reader initialization failed"));
        }
    } else {
        MS_DBG(getSensorNameAndLocation(),
               F("No analog voltage reader to initialize"));
    }

    return sensorSetupSuccess && analogVoltageReaderSuccess;
}


bool TurnerCyclops::addSingleMeasurementResult(void) {
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

    // Print out the calibration curve
    MS_DBG(F("  Input calibration Curve:"), _volt_std, F("V at"), _conc_std,
           F(".  "), _volt_blank, F("V blank."));
    if (fabsf(_volt_std - _volt_blank) < CYCLOPS_CALIBRATION_EPSILON) {
        MS_DBG(F("Invalid calibration: point voltage equals blank voltage"));
        return bumpMeasurementAttemptCount(false);
    }

    float adcVoltage = -9999.0f;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Read the single-ended analog voltage using the AnalogVoltageBase
    // interface.
    // NOTE: All implementations of the AnalogVoltageBase class validate both
    // the input channel and the resulting voltage, so we can trust that a
    // successful read will give us a valid voltage value to work with.
    bool success = _analogVoltageReader->readVoltageSingleEnded(_dataPin,
                                                                adcVoltage);
    if (success) {
        // Apply the unique calibration curve for the given sensor
        float calibResult = (_conc_std / (_volt_std - _volt_blank)) *
            (adcVoltage - _volt_blank);
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(CYCLOPS_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(CYCLOPS_VOLTAGE_VAR_NUM, adcVoltage);

    } else {
        MS_DBG(F("  Failed to get valid voltage from analog reader"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
