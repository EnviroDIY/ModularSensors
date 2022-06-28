/**
 * @file AnalogElecConductivity.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library
 * @copyright 2020 Neil Hancock
 *
 * * @brief This encapsulates an Electrical Conductivity sensors using an anlog
 *input and onboard ADC and ADC ref.
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
      _EcPowerPin(powerPin),
      _EcAdcPin(dataPin),
      _Rseries_ohms(Rseries_ohms),
      _sensorEC_Konst(sensorEC_Konst) {}
// Destructor
AnalogElecConductivity::~AnalogElecConductivity() {}

String AnalogElecConductivity::getSensorLocation(void) {
    String sensorLocation = F("anlgEc Proc Data/Pwr");
    sensorLocation += String(_EcAdcPin) + "/" + String(_EcPowerPin);
    return sensorLocation;
}


float AnalogElecConductivity::readEC() {
    return readEC(_EcAdcPin);
}


float AnalogElecConductivity::readEC(uint8_t analogPinNum) {
    uint32_t sensorEC_adc;
    float    Rwater_ohms;      // literal value of water
    float    EC_uScm = -9999;  // units are uS per cm

    // Set the resolution for the processor ADC, only applies to SAMD boards.
#if !defined ARDUINO_ARCH_AVR
    analogReadResolution(ANALOG_EC_ADC_RESOLUTION);
#endif  // ARDUINO_ARCH_AVR
    // Set the analog reference mode for the voltage measurement.
    // If possible, to get the best results, an external reference should be
    // used.
    analogReference(ANALOG_EC_ADC_REFERENCE_MODE);

    // First measure the analog voltage.
    // The return value from analogRead() is IN BITS NOT IN VOLTS!!
    // Take a priming reading.
    // First reading will be low - discard
    analogRead(analogPinNum);
    // Take the reading we'll keep
    sensorEC_adc = analogRead(analogPinNum);
    MS_DEEP_DBG("adc bits=", sensorEC_adc);

    if (0 == sensorEC_adc) {
        // Prevent underflow, can never be ANALOG_EC_ADC_RANGE
        sensorEC_adc = 1;
    }

    // Estimate Resistance of Liquid

    // see the header for an explanation of this calculation
    Rwater_ohms = _Rseries_ohms /
        ((static_cast<float>(ANALOG_EC_ADC_RANGE) /
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

    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        sensorEC_uScm = readEC(_EcAdcPin);
        MS_DBG(F("Water EC (uSm/cm)"), sensorEC_uScm);
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                                  sensorEC_uScm);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
