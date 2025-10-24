/**
 * @file AnalogElecConductivity.cpp
 * @copyright Stroud Water Research Center and Neil Hancock
 * Part of the EnviroDIY ModularSensors library
 * This library is published under the BSD-3 license.
 * @author Written By: Neil Hancock <neilh20+aec2008@wLLw.net>; Edited by Sara
 * Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief This encapsulates an Electrical Conductivity sensors using an analog
 * input and onboard ADC and ADC ref.
 */

#include "AnalogElecConductivity.h"

// For Mayfly version; the battery resistor depends on it
AnalogElecConductivity::AnalogElecConductivity(int8_t powerPin, int8_t dataPin,
                                               float   Rseries_ohms,
                                               float   sensorEC_Konst,
                                               uint8_t measurementsToAverage)
    : Sensor("AnalogElecConductivity", ANALOGELECCONDUCTIVITY_NUM_VARIABLES,
             ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS,
             ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS,
             ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage, ANALOGELECCONDUCTIVITY_INC_CALC_VARIABLES),
      _Rseries_ohms(Rseries_ohms),
      _sensorEC_Konst(sensorEC_Konst) {}
// Destructor
AnalogElecConductivity::~AnalogElecConductivity() {}

String AnalogElecConductivity::getSensorLocation(void) {
    String sensorLocation = F("anlgEc Proc Data/Pwr");
    sensorLocation += String(_dataPin) + "/" + String(_powerPin);
    return sensorLocation;
}


float AnalogElecConductivity::readEC() {
    return readEC(_dataPin);
}


float AnalogElecConductivity::readEC(uint8_t analogPinNum) {
    uint32_t sensorEC_adc;
    float    Rwater_ohms;      // literal value of water
    float    EC_uScm = -9999;  // units are uS per cm

    // First measure the analog voltage.
    // The return value from analogRead() is IN BITS NOT IN VOLTS!!
    // Take a priming reading.
    // First reading will be low - discard
    analogRead(analogPinNum);
    // Take the reading we'll keep
    sensorEC_adc = analogRead(analogPinNum);
    MS_DEEP_DBG("adc bits=", sensorEC_adc);

    if (0 == sensorEC_adc) {
        // Prevent underflow, can never be outside of PROCESSOR_ADC_RANGE
        sensorEC_adc = 1;
    }

    // Estimate Resistance of Liquid

    // see the header for an explanation of this calculation
    Rwater_ohms = _Rseries_ohms /
        ((static_cast<float>(PROCESSOR_ADC_RANGE) /
          static_cast<float>(sensorEC_adc)) -
         1);
    MS_DEEP_DBG("ohms=", Rwater_ohms);

    // Convert to EC
    EC_uScm = 1000000 / (Rwater_ohms * _sensorEC_Konst);
    MS_DEEP_DBG("cond=", EC_uScm);

    return EC_uScm;
}


/**
 * @brief Perform a single electrical conductivity measurement, record the value, and update attempt counters.
 *
 * If the measurement was not successfully started, the method updates the attempt counter for a failed attempt and returns.
 * Otherwise it reads conductivity from the configured analog data pin, records the measured conductivity value for the sensor,
 * and updates the attempt counter for a successful attempt. Measured values are recorded without additional validity filtering.
 *
 * @return `true` if the attempt counter was bumped for a successful measurement, `false` if it was bumped for a failed start.
 */
bool AnalogElecConductivity::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    float sensorEC_uScm = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    sensorEC_uScm = readEC(_dataPin);
    MS_DBG(F("Water EC (uSm/cm)"), sensorEC_uScm);

    // NOTE: We don't actually have any criteria for if the reading was any good
    // or not, so we mark it as successful no matter what.
    verifyAndAddMeasurementResult(ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                                  sensorEC_uScm);
    return bumpMeasurementAttemptCount(true);
}

// cSpell:ignore AnalogElecConductivity Rseries_ohms sensorEC_Konst Rwater_ohms
// cSpell:ignore anlgEc