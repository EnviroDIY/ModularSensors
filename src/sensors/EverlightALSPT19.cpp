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


/**
       * @brief Construct a sensor interface for an Everlight ALS-PT19 light sensor.
       *
       * Initializes the sensor instance with pins, electrical calibration values, and
       * the number of measurements to average for each reported sample.
       *
       * @param powerPin Digital pin used to switch sensor power (or -1 if always powered).
       * @param dataPin Analog input pin used to read the sensor voltage.
       * @param supplyVoltage Sensor supply voltage in volts.
       * @param loadResistor Load resistor value in kilo-ohms used for current conversion.
       * @param measurementsToAverage Number of consecutive readings to average per measurement.
       */
      EverlightALSPT19::EverlightALSPT19(int8_t powerPin, int8_t dataPin,
                                   float supplyVoltage, float loadResistor,
                                   uint8_t measurementsToAverage)
    : Sensor("Everlight ALS-PT19", ALSPT19_NUM_VARIABLES,
             ALSPT19_WARM_UP_TIME_MS, ALSPT19_STABILIZATION_TIME_MS,
             ALSPT19_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage),
      _supplyVoltage(supplyVoltage),
      _loadResistor(loadResistor) {}
#if defined(BUILT_IN_ALS_POWER_PIN) && defined(BUILT_IN_ALS_DATA_PIN) && \
    defined(BUILT_IN_ALS_SUPPLY_VOLTAGE) &&                              \
    defined(BUILT_IN_ALS_LOADING_RESISTANCE)
/**
       * @brief Construct an EverlightALS-PT19 sensor configured with board-default pins and hardware constants.
       *
       * Initializes the sensor using the built-in power pin, data pin, supply voltage, and load resistor
       * defined by the build configuration, and sets how many measurements to average per reading.
       *
       * @param measurementsToAverage Number of individual measurements to average for each reported sample.
       */
      EverlightALSPT19::EverlightALSPT19(uint8_t measurementsToAverage)
    : Sensor("Everlight ALS-PT19", ALSPT19_NUM_VARIABLES,
             ALSPT19_WARM_UP_TIME_MS, ALSPT19_STABILIZATION_TIME_MS,
             ALSPT19_MEASUREMENT_TIME_MS, BUILT_IN_ALS_POWER_PIN,
             BUILT_IN_ALS_DATA_PIN, measurementsToAverage,
             ALSPT19_INC_CALC_VARIABLES),
      _supplyVoltage(BUILT_IN_ALS_SUPPLY_VOLTAGE),
      _loadResistor(BUILT_IN_ALS_LOADING_RESISTANCE) {}
#endif
EverlightALSPT19::~EverlightALSPT19() {}


/**
 * @brief Perform a single sensor measurement and record voltage, current, and illuminance.
 *
 * Performs one measurement cycle if the measurement was started successfully; otherwise records
 * a failed attempt and returns. Reads the analog sensor value (with a priming read), converts
 * the ADC reading to voltage, converts that voltage to current in microamps (assuming the
 * configured load resistor value is in kilo-ohms), and converts current to illuminance in lux
 * using the sensor's typical datasheet relation (200 µA → 1000 lux). The computed voltage,
 * current, and illuminance values are added to the sensor's measurement results unconditionally.
 *
 * @returns `true` if the measurement attempt was recorded as successful, `false` otherwise.
 */
bool EverlightALSPT19::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    float volt_val    = -9999;
    float current_val = -9999;
    float lux_val     = -9999;

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
        // Prevent underflow, can never be outside of PROCESSOR_ADC_RANGE
        sensor_adc = 1;
    }
    // convert bits to volts
    volt_val = (_supplyVoltage / static_cast<float>(PROCESSOR_ADC_MAX)) *
        static_cast<float>(sensor_adc);
    // convert volts to current
    // resistance is entered in kΩ and we want µA
    current_val = (volt_val / (_loadResistor * 1000)) * 1e6;
    // convert current to illuminance
    // from sensor datasheet, typical 200µA current for 1000 Lux
    lux_val = current_val * (1000. / 200.);

    MS_DBG(F("  Voltage:"), volt_val, F("V"));
    MS_DBG(F("  Current:"), current_val, F("µA"));
    MS_DBG(F("  Illuminance:"), lux_val, F("lux"));

    // NOTE: We don't actually have any criteria for if the reading was any
    // good or not, so we mark it as successful no matter what.
    verifyAndAddMeasurementResult(ALSPT19_VOLTAGE_VAR_NUM, volt_val);
    verifyAndAddMeasurementResult(ALSPT19_CURRENT_VAR_NUM, current_val);
    verifyAndAddMeasurementResult(ALSPT19_ILLUMINANCE_VAR_NUM, lux_val);
    return bumpMeasurementAttemptCount(true);
}