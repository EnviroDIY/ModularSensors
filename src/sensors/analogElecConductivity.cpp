/**
 * @file analogElecConductivity.cpp
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library
 * @copyright 2020 Neil Hancock
 *
 * * @brief This encapsulates an Electrical Conductivity sensors using an anlog
 *input and onboard ADC and ADC ref.
 */

#include "analogElecConductivity.h"

// For Mayfly version; the battery resistor depends on it
analogElecConductivity::analogElecConductivity(int8_t powerPin, int8_t dataPin,
                                               uint8_t measurementsToAverage,
                                               float   Rseries_ohms)
    : Sensor("analogElecConductivity", ANALOGELECCONDUCTIVITY_NUM_VARIABLES,
             ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS,
             ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS,
             ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage) {
    //_version = version;
    _EcPowerPin            = powerPin;
    _EcAdcPin              = dataPin;
    _ptrWaterTemperature_C = NULL;
    _Rseries_ohms          = Rseries_ohms;
    /* clang-format off */
    /* Not checked
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY) ||
    defined(ARDUINO_AVR_SODAQ_MBILI) _EcAdcPin = A6;
#elif defined(ARDUINO_AVR_FEATHER32U4) ||
        defined(ARDUINO_SAMD_FEATHER_M0) ||
        defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
        _EcAdcPin = 9;
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
        _EcAdcPin = A6;  // 20;  //Dedicated PB01 V_DIV
#elif defined(ARDUINO_SODAQ_ONE) || defined(ARDUINO_SODAQ_ONE_BETA) ||
        defined(ARDUINO_AVR_SODAQ_NDOGO)
        _EcAdcPin = 10;
#elif defined(ARDUINO_SODAQ_AUTONOMO)
    if (strcmp(_version, "v0.1") == 0)
        _EcAdcPin   = 48;
    else _EcAdcPin  = 33;
#else
#info No board defined
    _EcAdcPin = -1;
#endif
    */
    /* clang-format on */
}
// Destructor
analogElecConductivity::~analogElecConductivity() {}

String analogElecConductivity::getSensorLocation(void) {
    String sensorLocation = F("anlgEc Proc Data/Pwr");
    sensorLocation += String(_EcAdcPin) + "/" + String(_EcPowerPin);
    return sensorLocation;
}


float analogElecConductivity::readEC() {
    return readEC(_EcAdcPin);
}


float analogElecConductivity::readEC(uint8_t analogPinNum) {
    uint32_t sensorEC_adc;
    float    Rwater_ohms;         // literal value of water
    float    EC_uScm, EC25_uScm;  // units are uS per cm

#if !defined ARDUINO_ARCH_AVR
    analogReadResolution(analogElecConductivityDef_Resolution);
    analogReference(AR_EXTERNAL);  // ratio metric for the EC resistor
// analogReference(PROC_ADC_DEF_REFERENCE); //VDDANA = 3V3
#endif  // ARDUINO_ARCH_AVR

    //************Estimates Resistance of Liquid ****************//
    // digitalWrite(_EcPowerPin,HIGH); //assume done by class Sensor
    delay(1);  // Total time is about 5mS

    // First reading will be low - discard
    sensorEC_adc = analogRead(analogPinNum);

    MS_DEEP_DBG("adc=", sensorEC_adc);

    //***************** Converts to EC **************************//

    // sensorEC_V= (SensorV* sensorEC_adc)/EC_SENSOR_ADC_RANGE;
    // Rwater_ohms=(sensorEC_V*Rseries_ohms)/(SensorV-sensorEC_V);

    /*Assuming sensorEC_adc is ratio metric - adc Reference is same as applied
     * to EC sensor. The Vcc ~ 3.3V can vary, as battery level gets low, so
     * would be nice to eliminate it in the calcs
     *
     *   raw_adc/EC_SENSOR_ADC_RANGE = Rwater_ohms/(Rwater_ohms+Rseries_ohms)
     */
    if (0 == sensorEC_adc) {
        // Prevent underflow, can never be EC_SENSOR_ADC_RANGE
        sensorEC_adc = 1;
    }

    Rwater_ohms = _Rseries_ohms /
        (((float)EC_SENSOR_ADC_RANGE / (float)sensorEC_adc) - 1);

    /* The Rwater is an absolute value, but is based on a defined size of the
     * plates of the sensor. Translating a magic "sensorsEC_konst" is used,
     * derived from others experiments
     */
    EC_uScm = 1000000 / (Rwater_ohms * sensorEC_Konst);

    //*************Compensating For Temperature********************//
    if (NULL != _ptrWaterTemperature_C) {
        EC25_uScm = EC_uScm /
            (1 + TemperatureCoef * (*_ptrWaterTemperature_C - 25.0));
    } else {
        EC25_uScm = EC_uScm;
    }

    // Note return Rwater_ohms if MS_ANALOGELECCONDUCTIVITY_DEBUG_DEEP
    MS_DEEP_DBG("ohms=", Rwater_ohms);
    return EC25_uScm;
}


bool analogElecConductivity::addSingleMeasurementResult(void) {
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
