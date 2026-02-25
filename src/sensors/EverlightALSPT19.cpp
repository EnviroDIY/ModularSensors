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
                                   float alsSupplyVoltage, float loadResistor,
                                   uint8_t            measurementsToAverage,
                                   AnalogVoltageBase* analogVoltageReader)
    : Sensor("Everlight ALS-PT19", ALSPT19_NUM_VARIABLES,
             ALSPT19_WARM_UP_TIME_MS, ALSPT19_STABILIZATION_TIME_MS,
             ALSPT19_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage, ALSPT19_INC_CALC_VARIABLES),
      _alsSupplyVoltage((alsSupplyVoltage > 0.0f) ? alsSupplyVoltage
                                                  : OPERATING_VOLTAGE),
      _loadResistor(loadResistor),
      _analogVoltageReader(analogVoltageReader),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader = createProcessorAnalogBase(_ownsAnalogVoltageReader);
    }
}
#if (defined(BUILT_IN_ALS_POWER_PIN) && defined(BUILT_IN_ALS_DATA_PIN) && \
     defined(BUILT_IN_ALS_SUPPLY_VOLTAGE) &&                              \
     defined(BUILT_IN_ALS_LOADING_RESISTANCE)) ||                         \
    defined(DOXYGEN)
EverlightALSPT19::EverlightALSPT19(uint8_t            measurementsToAverage,
                                   AnalogVoltageBase* analogVoltageReader)
    : EverlightALSPT19(BUILT_IN_ALS_POWER_PIN, BUILT_IN_ALS_DATA_PIN,
                       BUILT_IN_ALS_SUPPLY_VOLTAGE,
                       BUILT_IN_ALS_LOADING_RESISTANCE, measurementsToAverage,
                       analogVoltageReader) {}
#endif

// Destructor
EverlightALSPT19::~EverlightALSPT19() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
    }
}


String EverlightALSPT19::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getAnalogLocation(_dataPin, -1);
    } else {
        return String("Unknown_AnalogVoltageReader");
    }
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

    // Check if we have a valid load resistor
    if (_loadResistor <= 0) {
        MS_DBG(getSensorNameAndLocation(), F("Invalid load resistor value"));
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
        verifyAndAddMeasurementResult(ALSPT19_VOLTAGE_VAR_NUM, adcVoltage);

        // From the datasheet:
        // The output voltage V(out) is the product of photocurrent I(PH) and
        // loading resistor R(L):
        // - V(out) = I(PH) * R(L)
        // At saturation:
        // - V(out) ＝ Vcc－0.4V
        if (adcVoltage > _alsSupplyVoltage - 0.4f) {
            MS_DBG(getSensorNameAndLocation(),
                   F("Light sensor has reached saturation!  Clamping current "
                     "and illumination values!"));
            adcVoltage = max(0.0f, _alsSupplyVoltage - 0.4f);
        }
        // convert volts to current
        // resistance is entered in kΩ and we want µA
        float current_val = (adcVoltage / (_loadResistor * 1000.0f)) * 1e6f;
        MS_DBG(F("  Current:"), current_val, F("µA"));

        // convert current to illuminance
        // from sensor datasheet, typical 200µA current for 1000 Lux
        float calibResult = current_val * (1000.0f / ALSPT19_UA_PER_1000LUX);
        MS_DBG(F("  Illuminance:"), calibResult, F("lux"));

        verifyAndAddMeasurementResult(ALSPT19_CURRENT_VAR_NUM, current_val);
        verifyAndAddMeasurementResult(ALSPT19_ILLUMINANCE_VAR_NUM, calibResult);
    } else {
        MS_DBG(F("  Failed to get valid voltage from analog reader"));
    }
    return bumpMeasurementAttemptCount(success);
}
