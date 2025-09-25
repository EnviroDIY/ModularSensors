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


bool AnalogElecConductivity::addSingleMeasurementResult(void) {
    float sensorEC_uScm = -9999;

    if (getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        sensorEC_uScm = readEC(_dataPin);
        MS_DBG(F("Water EC (uSm/cm)"), sensorEC_uScm);
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                                  sensorEC_uScm);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    clearStatusBits(MEASUREMENT_ATTEMPTED, MEASUREMENT_SUCCESSFUL);

    // Return true when finished
    return true;
}

// cSpell:ignore AnalogElecConductivity Rseries_ohms sensorEC_Konst Rwater_ohms
// cSpell:ignore anlgEc
