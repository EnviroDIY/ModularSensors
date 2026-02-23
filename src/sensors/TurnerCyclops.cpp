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
#include "ProcessorAnalog.h"
#include <Adafruit_ADS1X15.h>


// Primary constructor using AnalogVoltageBase abstraction
TurnerCyclops::TurnerCyclops(int8_t             powerPin,
                             AnalogVoltageBase* analogVoltageReader,
                             float conc_std, float volt_std, float volt_blank,
                             uint8_t measurementsToAverage)
    : Sensor("TurnerCyclops", CYCLOPS_NUM_VARIABLES, CYCLOPS_WARM_UP_TIME_MS,
             CYCLOPS_STABILIZATION_TIME_MS, CYCLOPS_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage, CYCLOPS_INC_CALC_VARIABLES),
      _analogVoltageReader(analogVoltageReader),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank),
      _ownsVoltageReader(false) {}

// Constructor that creates a TIADS1x15 object internally
TurnerCyclops::TurnerCyclops(int8_t powerPin, int8_t adsChannel, float conc_std,
                             float volt_std, float volt_blank,
                             float voltageMultiplier, adsGain_t adsGain,
                             uint8_t i2cAddress, uint8_t measurementsToAverage,
                             float adsSupplyVoltage)
    : Sensor("TurnerCyclops", CYCLOPS_NUM_VARIABLES, CYCLOPS_WARM_UP_TIME_MS,
             CYCLOPS_STABILIZATION_TIME_MS, CYCLOPS_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage, CYCLOPS_INC_CALC_VARIABLES),
      _analogVoltageReader(nullptr),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank),
      _ownsVoltageReader(true) {
    // Create a TIADS1x15 object for analog voltage reading
    _analogVoltageReader =
        new TIADS1x15(powerPin, adsChannel, voltageMultiplier, adsGain,
                      i2cAddress, measurementsToAverage, adsSupplyVoltage);
}

// Constructor that creates a ProcessorAnalog object internally
TurnerCyclops::TurnerCyclops(int8_t powerPin, int8_t dataPin, float conc_std,
                             float volt_std, float volt_blank,
                             float voltageMultiplier, float operatingVoltage,
                             uint8_t measurementsToAverage)
    : Sensor("TurnerCyclops", CYCLOPS_NUM_VARIABLES, CYCLOPS_WARM_UP_TIME_MS,
             CYCLOPS_STABILIZATION_TIME_MS, CYCLOPS_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage, CYCLOPS_INC_CALC_VARIABLES),
      _analogVoltageReader(nullptr),
      _conc_std(conc_std),
      _volt_std(volt_std),
      _volt_blank(volt_blank),
      _ownsVoltageReader(true) {
    // Create a ProcessorAnalog object for analog voltage reading
    _analogVoltageReader =
        new ProcessorAnalog(powerPin, dataPin, voltageMultiplier,
                            operatingVoltage, measurementsToAverage);
}
// Destructor
TurnerCyclops::~TurnerCyclops() {
    // Clean up owned voltage reader if created by legacy constructor
    if (_ownsVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
        _analogVoltageReader = nullptr;
    }
}


String TurnerCyclops::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getSensorLocation();
    } else {
        // Fallback for cases where voltage reader is not set
        return F("TurnerCyclops_UnknownLocation");
    }
}


bool TurnerCyclops::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    // Ensure we have a valid voltage reader
    if (_analogVoltageReader == nullptr) {
        MS_DBG(F("  No analog voltage reader configured!"));
        return bumpMeasurementAttemptCount(false);
    }

    // Print out the calibration curve and check that it is valid
    MS_DBG(F("  Input calibration Curve:"), _volt_std, F("V at"), _conc_std,
           F(".  "), _volt_blank, F("V blank."));
    const float epsilon = 1e-4f;  // tune to expected sensor precision
    if (fabs(_volt_std - _volt_blank) < epsilon) {
        MS_DBG(F("Invalid calibration: point voltage equals blank voltage"));
        return bumpMeasurementAttemptCount(false);
    }

    bool  success     = false;
    float adcVoltage  = -9999;
    float calibResult = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Use the abstracted voltage reading method
    success = _analogVoltageReader->readVoltageSingleEnded(adcVoltage);

    if (success) {
        // Apply the unique calibration curve for the given sensor
        calibResult = (_conc_std / (_volt_std - _volt_blank)) *
            (adcVoltage - _volt_blank);
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(CYCLOPS_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(CYCLOPS_VOLTAGE_VAR_NUM, adcVoltage);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
