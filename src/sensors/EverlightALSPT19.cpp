/**
 * @file EverlightALSPT19.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the EverlightALSPT19 class.
 */

#include "EverlightALSPT19.h"
#include "ProcessorAnalog.h"


EverlightALSPT19::EverlightALSPT19(int8_t powerPin, int8_t dataPin,
                                   float supplyVoltage, float loadResistor,
                                   uint8_t            measurementsToAverage,
                                   AnalogVoltageBase* analogVoltageReader)
    : Sensor("Everlight ALS-PT19", ALSPT19_NUM_VARIABLES,
             ALSPT19_WARM_UP_TIME_MS, ALSPT19_STABILIZATION_TIME_MS,
             ALSPT19_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage, ALSPT19_INC_CALC_VARIABLES),
      _supplyVoltage(supplyVoltage),
      _loadResistor(loadResistor) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader     = new ProcessorAnalogBase();
        _ownsAnalogVoltageReader = true;
    } else {
        _analogVoltageReader     = analogVoltageReader;
        _ownsAnalogVoltageReader = false;
    }
}
#if (defined(BUILT_IN_ALS_POWER_PIN) && defined(BUILT_IN_ALS_DATA_PIN) && \
     defined(BUILT_IN_ALS_SUPPLY_VOLTAGE) &&                              \
     defined(BUILT_IN_ALS_LOADING_RESISTANCE)) ||                         \
    defined(DOXYGEN)
EverlightALSPT19::EverlightALSPT19(uint8_t            measurementsToAverage,
                                   AnalogVoltageBase* analogVoltageReader)
    : Sensor("Everlight ALS-PT19", ALSPT19_NUM_VARIABLES,
             ALSPT19_WARM_UP_TIME_MS, ALSPT19_STABILIZATION_TIME_MS,
             ALSPT19_MEASUREMENT_TIME_MS, BUILT_IN_ALS_POWER_PIN,
             BUILT_IN_ALS_DATA_PIN, measurementsToAverage,
             ALSPT19_INC_CALC_VARIABLES),
      _supplyVoltage(BUILT_IN_ALS_SUPPLY_VOLTAGE),
      _loadResistor(BUILT_IN_ALS_LOADING_RESISTANCE) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader     = new ProcessorAnalogBase();
        _ownsAnalogVoltageReader = true;
    } else {
        _analogVoltageReader     = analogVoltageReader;
        _ownsAnalogVoltageReader = false;
    }
}
#endif

// Destructor
EverlightALSPT19::~EverlightALSPT19() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
        _analogVoltageReader = nullptr;
    }
}


String EverlightALSPT19::getSensorLocation(void) {
    // NOTE: The constructor guarantees that _analogVoltageReader is not null
    String sensorLocation = _analogVoltageReader->getSensorLocation();
    sensorLocation += F("_");
    sensorLocation += String(_dataPin);
    return sensorLocation;
}


bool EverlightALSPT19::addSingleMeasurementResult(void) {
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

    // Read the analog voltage using the AnalogVoltageBase interface
    success = _analogVoltageReader->readVoltageSingleEnded(_dataPin,
                                                           adcVoltage);

    if (success) {
        // convert volts to current
        // resistance is entered in kΩ and we want µA
        float current_val = (adcVoltage / (_loadResistor * 1000)) * 1e6;
        MS_DBG(F("  Current:"), current_val, F("µA"));

        // convert current to illuminance
        // from sensor datasheet, typical 200µA current for 1000 Lux
        float calibResult = current_val * (1000. / 200.);
        MS_DBG(F("  Illuminance:"), calibResult, F("lux"));

        verifyAndAddMeasurementResult(ALSPT19_VOLTAGE_VAR_NUM, adcVoltage);
        verifyAndAddMeasurementResult(ALSPT19_CURRENT_VAR_NUM, current_val);
        verifyAndAddMeasurementResult(ALSPT19_ILLUMINANCE_VAR_NUM, calibResult);
    } else {
        MS_DBG(F("  Failed to get valid voltage from analog reader"));
    }
    return bumpMeasurementAttemptCount(success);
}
