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
TurnerCyclops::TurnerCyclops(int8_t powerPin, uint8_t analogChannel,
                             float conc_std, float volt_std, float volt_blank,
                             uint8_t            measurementsToAverage,
                             AnalogVoltageBase* analogVoltageReader)
    : Sensor("TurnerCyclops", CYCLOPS_NUM_VARIABLES, CYCLOPS_WARM_UP_TIME_MS,
             CYCLOPS_STABILIZATION_TIME_MS, CYCLOPS_MEASUREMENT_TIME_MS,
             powerPin, analogChannel, measurementsToAverage,
             CYCLOPS_INC_CALC_VARIABLES),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader     = new TIADS1x15Base();
        if (_analogVoltageReader != nullptr) {
            _ownsAnalogVoltageReader = true;
        } else {
            _ownsAnalogVoltageReader = false;
        }
    } else {
        _analogVoltageReader     = analogVoltageReader;
        _ownsAnalogVoltageReader = false;
    }
}

// Destructor
TurnerCyclops::~TurnerCyclops() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
        _analogVoltageReader = nullptr;
    }
}


String TurnerCyclops::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getSensorLocation() + F("_Channel") +
            String(_dataPin);
    } else {
        String sensorLocation = F("Unknown_AnalogVoltageReader_Channel");
        sensorLocation += String(_dataPin);
        return sensorLocation;
    }
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

    bool  success     = false;
    float adcVoltage  = -9999.0f;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Print out the calibration curve
    MS_DBG(F("  Input calibration Curve:"), _volt_std, F("V at"), _conc_std,
           F(".  "), _volt_blank, F("V blank."));
    const float epsilon = 1e-4f;  // tune to expected sensor precision
    if (fabs(_volt_std - _volt_blank) < epsilon) {
        MS_DBG(F("Invalid calibration: point voltage equals blank voltage"));
        return bumpMeasurementAttemptCount(false);
    }

    // Read voltage using the AnalogVoltageBase interface
    success = _analogVoltageReader->readVoltageSingleEnded(_dataPin,
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
