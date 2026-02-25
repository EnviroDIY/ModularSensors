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
#include "ProcessorAnalog.h"

// For Mayfly version; the battery resistor depends on it
AnalogElecConductivity::AnalogElecConductivity(
    int8_t powerPin, int8_t dataPin, float Rseries_ohms, float sensorEC_Konst,
    uint8_t measurementsToAverage, AnalogVoltageBase* analogVoltageReader)
    : Sensor("AnalogElecConductivity", ANALOGELECCONDUCTIVITY_NUM_VARIABLES,
             ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS,
             ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS,
             ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage, ANALOGELECCONDUCTIVITY_INC_CALC_VARIABLES),
      _Rseries_ohms(Rseries_ohms),
      _sensorEC_Konst(sensorEC_Konst),
      // If no analog voltage reader was provided, create a default one
      _analogVoltageReader(analogVoltageReader ? analogVoltageReader
                                               : new ProcessorAnalogBase()),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {}

// Destructor
AnalogElecConductivity::~AnalogElecConductivity() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
    }
}


String AnalogElecConductivity::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        String sensorLocation = _analogVoltageReader->getSensorLocation();
        sensorLocation += F("_anlgEc_");
        sensorLocation += String(_dataPin);
        sensorLocation += F("_Pwr");
        sensorLocation += String(_powerPin);
        return sensorLocation;
    } else {
        String sensorLocation = F("Unknown_AnalogVoltageReader_anlgEc_");
        sensorLocation += String(_dataPin);
        sensorLocation += F("_Pwr");
        sensorLocation += String(_powerPin);
        return sensorLocation;
    }
}


bool AnalogElecConductivity::addSingleMeasurementResult(void) {
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
        // Estimate Resistance of Liquid
        // see the header for an explanation of this calculation
        // Convert voltage back to ADC equivalent for existing calculation
        // NOTE: The supplyVoltage is already clamped by the
        // _analogVoltageReader
        float supplyVoltage = _analogVoltageReader->getSupplyVoltage();
        float adcRatio      = adcVoltage / supplyVoltage;

        if (adcRatio >= 1.0) {
            // Prevent division issues when voltage reaches supply voltage
            MS_DBG(F("  ADC ratio clamped from"), adcRatio, F("to"),
                   ANALOGELECCONDUCTIVITY_ADC_MAX_RATIO);
            adcRatio = ANALOGELECCONDUCTIVITY_ADC_MAX_RATIO;
        }

        float Rwater_ohms = _Rseries_ohms * adcRatio / (1.0 - adcRatio);
        MS_DBG(F("  Resistance:"), Rwater_ohms, F("ohms"));

        // Convert to EC
        float EC_uScm = -9999.0f;  // units are uS per cm
        if (Rwater_ohms > 0) {
            EC_uScm = 1000000.0f / (Rwater_ohms * _sensorEC_Konst);
            MS_DBG(F("Water EC (uS/cm)"), EC_uScm);
            verifyAndAddMeasurementResult(ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                                          EC_uScm);
        } else {
            MS_DBG(F("  Invalid resistance; cannot calculate EC"));
            success = false;
        }
    } else {
        MS_DBG(F("  Failed to get valid voltage from analog reader"));
    }
    return bumpMeasurementAttemptCount(success);
}

// cSpell:ignore AnalogElecConductivity Rseries_ohms sensorEC_Konst Rwater_ohms
// cSpell:ignore anlgEc
