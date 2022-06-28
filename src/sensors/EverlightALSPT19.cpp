/**
 * @file EverlightALSPT19.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the EverlightALSPT19 class.
 */

#include "EverlightALSPT19.h"


// The constructor - because this is I2C, only need the power pin
// This sensor has a set I2C address of 0XB8
EverlightALSPT19::EverlightALSPT19(int8_t powerPin, int8_t dataPin,
                                   float supplyVoltage, float loadResistor,
                                   uint8_t measurementsToAverage)
    : Sensor("Everlight ALS-PT19", ALSPT19_NUM_VARIABLES,
             ALSPT19_WARM_UP_TIME_MS, ALSPT19_STABILIZATION_TIME_MS,
             ALSPT19_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage),
      _supplyVoltage(supplyVoltage),
      _loadResistor(loadResistor) {}
EverlightALSPT19::EverlightALSPT19(uint8_t measurementsToAverage)
    : Sensor("Everlight ALS-PT19", ALSPT19_NUM_VARIABLES,
             ALSPT19_WARM_UP_TIME_MS, ALSPT19_STABILIZATION_TIME_MS,
             ALSPT19_MEASUREMENT_TIME_MS, MAYFLY_ALS_POWER_PIN,
             MAYFLY_ALS_DATA_PIN, measurementsToAverage,
             ALSPT19_INC_CALC_VARIABLES),
      _supplyVoltage(MAYFLY_ALS_SUPPLY_VOLTAGE),
      _loadResistor(MAYFLY_ALS_LOADING_RESISTANCE) {}
EverlightALSPT19::~EverlightALSPT19() {}


bool EverlightALSPT19::addSingleMeasurementResult(void) {
    // Initialize float variables
    float volt_val    = -9999;
    float current_val = -9999;
    float lux_val     = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        // Set the resolution for the processor ADC, only applies to SAMD
        // boards.
#if !defined ARDUINO_ARCH_AVR
        analogReadResolution(ALSPT19_ADC_RESOLUTION);
#endif  // ARDUINO_ARCH_AVR
        // Set the analog reference mode for the voltage measurement.
        // If possible, to get the best results, an external reference should be
        // used.
        analogReference(ALSPT19_ADC_REFERENCE_MODE);
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // First measure the analog voltage.
        // The return value from analogRead() is IN BITS NOT IN VOLTS!!
        // Take a priming reading.
        // First reading will be low - discard
        analogRead(_dataPin);
        // Take the reading we'll keep
        uint32_t sensor_adc = analogRead(_dataPin);
        MS_DEEP_DBG("  ADC Bits:", sensor_adc);

        if (0 == sensor_adc) {
            // Prevent underflow, can never be ALSPT19_ADC_RANGE
            sensor_adc = 1;
        }
        // convert bits to volts
        volt_val = (_supplyVoltage / static_cast<float>(ALSPT19_ADC_MAX)) *
            static_cast<float>(sensor_adc);
        // convert volts to current
        // resistance is entered in kΩ and we want µA
        current_val = (volt_val / (_loadResistor * 1000)) * 1e6;
        // convert current to illuminance
        // from sensor datasheet, typical 200µA current for1000 Lux
        lux_val = current_val * (1000. / 200.);


        MS_DBG(F("  Voltage:"), volt_val, F("V"));
        MS_DBG(F("  Current:"), current_val, F("µA"));
        MS_DBG(F("  Illuminance:"), lux_val, F("lux"));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(ALSPT19_VOLTAGE_VAR_NUM, volt_val);
    verifyAndAddMeasurementResult(ALSPT19_CURRENT_VAR_NUM, current_val);
    verifyAndAddMeasurementResult(ALSPT19_ILLUMINANCE_VAR_NUM, lux_val);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return true;
}
