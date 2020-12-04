/**
 * @file AnalogElecConductivityM.cpp
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library
 * @copyright 2020 Neil Hancock
 *
 * * @brief This encapsulates an Electrical Conductivity sensors using an anlog
 *input and onboard ADC and ADC ref.
 */

#include "AnalogElecConductivityM.h"
#include "ms_cfg.h"

// For Mayfly version; the battery resistor depends on it
AnalogElecConductivityM::AnalogElecConductivityM(int8_t  powerPin,
                                                 int8_t  dataPin,
                                                 float   Rseries_ohms,
                                                 float   sensorEC_Konst,
                                                 uint8_t measurementsToAverage)
    : Sensor("AnalogElecConductivityM", ANALOGELECCONDUCTIVITY_NUM_VARIABLES,
             ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS,
             ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS,
             ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage) {
    _EcPowerPin            = powerPin;
    _EcAdcPin              = dataPin;
    _Rseries_ohms          = Rseries_ohms;
    _ptrWaterTemperature_C = NULL;
}
// Destructor
AnalogElecConductivityM::~AnalogElecConductivityM() {}

String AnalogElecConductivityM::getSensorLocation(void) {
    String sensorLocation = F("anlgEc Proc Data/Pwr");
    sensorLocation += String(_EcAdcPin) + "/" + String(_EcPowerPin);
    return sensorLocation;
}


float AnalogElecConductivityM::readEC() {
    return readEC(_EcAdcPin);
}


float AnalogElecConductivityM::readEC(uint8_t analogPinNum) {
    uint32_t sensorEC_adc;
    float    Rwater_ohms;      // literal value of water
    float    EC_uScm = -9999;  // units are uS per cm

    // Set the resolution for the processor ADC, only applies to SAMD boards.
#if !defined ARDUINO_ARCH_AVR
    analogReadResolution(ANALOG_EC_ADC_RESOLUTION);
    analogReference(AR_EXTERNAL);  // ratio metric for the EC resistor
// analogReference(ProcAdcDef_Reference); //VDDANA = 3V3
#else  // ARDUINO_ARCH_AVR
    // Set the analog reference mode for the voltage measurement.
    // If possible, to get the best results, an external reference should be
    // used.
    analogReference(ANALOG_EC_ADC_REFERENCE_MODE);
#endif

#if defined ARD_ANALOLG_EXTENSION_PINS
    uint8_t useAdcChannel = analogPinNum;
    if ((thisVariantNumPins + ARD_DIGITAL_EXTENSION_PINS) < analogPinNum) {
        // ARD_COMMON_PIN on SAMD51
        if (ARD_ANLAOG_MULTIPLEX_PIN != useAdcChannel) {
            // Setup mutliplexer
            MS_DBG("  adc_Single Setup Multiplexer ", useAdcChannel, "-->",
                   ARD_ANLAOG_MULTIPLEX_PIN);
            digitalWrite(useAdcChannel, 1);
            // useAdcChannel = ARD_ANLAOG__MULTIPLEX_PIN;
        }
        analogPinNum = ARD_ANLAOG_MULTIPLEX_PIN;
    }
#endif  // ARD_ANALOLG_EXTENSION_PINS
    //************Estimates Resistance of Liquid ****************//
    // digitalWrite(_EcPowerPin,HIGH); //assume done by class Sensor
    delay(1);  // Total time is about 5mS

    // First reading will be low - discard
    analogRead(analogPinNum);
    // Take the reading we'll keep
    sensorEC_adc = analogRead(analogPinNum);
    // digitalWrite(_EcPowerPin,LOW);

#if defined ARD_ANALOLG_EXTENSION_PINS
    digitalWrite(useAdcChannel, 0);  // Turn off Mux

#endif  // ARD_ANALOLG_EXTENSION_PINS

    MS_DEEP_DBG("adc bits=", sensorEC_adc);

    if (0 == sensorEC_adc) {
        // Prevent underflow, can never be ANALOG_EC_ADC_RANGE
        sensorEC_adc = 1;
    }

    // Estimate Resistance of Liquid

    // see the header for an explanation of this calculation
    Rwater_ohms = _Rseries_ohms /
        (((float)ANALOG_EC_ADC_RANGE / (float)sensorEC_adc) - 1);
    MS_DEEP_DBG("ohms=", Rwater_ohms);

    /* The Rwater is an absolute value, but is based on a defined size of the
     * plates of the sensor. Translating a magic "sensorsEC_konst" is used,
     * derived from others experiments
     */
    // Convert to EC
    EC_uScm = 1000000 / (Rwater_ohms * _sensorEC_Konst);
    MS_DEEP_DBG("cond=", EC_uScm);

    //*************Compensating For Temperature********************//
    if (NULL != _ptrWaterTemperature_C) {
        float EC25_uScm = EC_uScm /
            (1 + TemperatureCoef * (*_ptrWaterTemperature_C - 25.0));
        EC_uScm = EC25_uScm;
    }

    // Note return Rwater_ohms if MS_ANALOGELECCONDUCTIVITY_DEBUG_DEEP
    // MS_DEEP_DBG("ohms=", Rwater_ohms);
    return EC_uScm;
}


bool AnalogElecConductivityM::addSingleMeasurementResult(void) {
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
