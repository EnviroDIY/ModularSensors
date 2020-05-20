/*
 *analogElecConductivity.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *
 *This sensor encapsulates sensors using an anlog input and onboard ADC and ADC ref.
 * EC from IEC probe, requires ADC_PIN, Source Resistance, Applied Voltage, water temperature. Requires switched power pin
 *     The water Temperature must be suplied seperately for a calculation.
 * 
 *  To use, must switch power to series resistor  499R, then measurement from anlog pin A[0-7] and then sensor
 *  
 * Parts & Wiring for Mayfly
 *  Mayfly 0.5b or later
 *  Sensor  AC Power Cord 12t with male IEC 320-C8 connector. 
 *  2x10 J3 for any pins A0-A7
 *  Rlmt eg 499R series 1/4W
 * 
 *  Analog JP8 JP9 for ADC 16bit/ADS1115 uses different algorithim
 * 
 * Wiring
 * Will use J3-Pin2 A0 but could be any of A0-A3,A5 
 *  and one pin switches pwr eg AA (PwrPin)
 *  A7 by default is tied to DS3231 Timer Int
 *  Rlmt/500ohms to A7 and Sensor; J3-Pin10 to Sensor J3-Pin2
 *  Sensors one side; J3-Pin2
 *  Sensor  one side to GND Mayfly J3-Pin20
 * 
 *For EC calculation 
 *  (better to be ratio meteric))
 *  Range of 0-3V3 with 10bit ADC - resolution of 0.003
 * 
 */

// Header Guards
#ifndef analogElecConductivity_h
#define analogElecConductivity_h

#ifdef MS_ANALOGELECCONDUCTIVITY_DEBUG
#define MS_DEBUGGING_STD "analogElecConductivity"
#endif
#ifdef MS_ANALOGELECCONDUCTIVITY_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "analogElecConductivity"
#endif
// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "SensorBase.h"
#include "math.h"

// Sensor Specific Defines
#define ANALOGELECCONDUCTIVITY_NUM_VARIABLES 1
#define ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS 2
#define ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS 0
#define ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS 0

#define ANALOGELECCONDUCTIVITY_EC_RESOLUTION 1
#define ANALOGELECCONDUCTIVITY_EC_VAR_NUM 0

#define analogElecConductivityDef_Resolution 10  //Default for all boards, change through API as needed
#define analogElecConductivityAdc_Max ((1<< analogElecConductivityDef_Resolution)-1)
#define EC_SENSOR_ADC_RANGE (1<< analogElecConductivityDef_Resolution)

#if !defined SENSOR_UNINIT_VAL
#define SENSOR_UNINIT_VAL -0.1
#endif //SENSOR_UNINIT_VAL

#if !defined ProcAdcDef_Reference
// one of eAnalogReference for all host platforms
#define ProcAdcDef_Reference AR_DEFAULT 
#endif //ProcAdcDef_Reference

class analogElecConductivity : public Sensor
{
public:
    // Need to know the  Mayfly version because the battery resistor depends on it
    analogElecConductivity(int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage=1);
    ~analogElecConductivity();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;
    //void set_active_sensors(uint8_t sensors_mask);
    //uint8_t which_sensors_active(void);
    //void setWaterTemperature(float  WaterTemperature_C); 
    void setWaterTemperature(float  *WaterTemperature_C); 
    //void setEc_k(int8_t powerPin, int8_t adcPin, float  sourceResistance_ohms,float  appliedV_V, uint8_t probeType); 
    float readEC(uint8_t analogPinNum);


private:
    const char *_version;
    int8_t _EcPowerPin;
    int8_t _EcAdcPin;

    //float _WaterTemperature_C;
    float *_ptrWaterTemperature_C;
    const float SensorV= 3.3;
    #if !defined RSERIES_OHMS_DEF
    #define RSERIES_OHMS_DEF 499
    #endif //RSERIES_OHMS_DEF
    const float Rseries_ohms=RSERIES_OHMS_DEF; //that is R1 + any series port resistance
    #if !defined TEMPERATURECOEF_DEF
    #define TEMPERATURECOEF_DEF 0.019
    #endif //TEMPERATURECOEF_DEF
    const float TemperatureCoef=TEMPERATURECOEF_DEF; //depends on what chemical/transport is being measured    
    //********************** Cell Constant For Ec Measurements *********************//
    //Mine was around 2.9 with plugs being a standard size they should all be around the same
    //But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
    #if !defined SENSOREC_KONST_DEF
    #define SENSOREC_KONST_DEF 2.88
    #endif //SENSOREC_KONST_DEF
    const float sensorEC_Konst=SENSOREC_KONST_DEF;

};


// For the battery supplying power to the processor
class analogElecConductivity_EC : public Variable
{
public:
    analogElecConductivity_EC(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "anlgEc")
      : Variable(parentSense,
                 (const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                 (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                 "electricalConductivity", "uScm",
                 varCode, uuid)
    {}
    analogElecConductivity_EC()
      : Variable((const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                 (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                 "ElectricalConductivity", "uScm", "anlgEc")
    {}
    ~analogElecConductivity_EC(){}
};


#endif  //analogElecConductivity
