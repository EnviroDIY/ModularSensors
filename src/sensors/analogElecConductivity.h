/*
 *analogElecConductivity.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *
 *This sensor encapsulates sensors using an anlog input .
 * EC from IEC probe, requires ADC_PIN, Source Resistance, Applied Voltage, water temperature. Requires switched power pin
 *     The water Temperature must be calculated seperately.
 * 
 *  Power on A0, with 500R to A[1,2,3,4,5] and then sensor
 *  
 * Parts & Wiring for Mayfly
 *  Mayfly 0.5b or later
 *  0.1" male headers - at least 2x2 for one connector, 2x5
 *  Sensor  AC Power Cord 12t with male IEC 320-C8 connector. 
 *  IEC 320-C8 Instrument connector so can be detached on instalation
 *  Rlmt eg 500R
 * 
 * 
 * Wiring
 *  Rlmt from J3-A0 to Sensor Pin J3-A1
 *  Sensors one side to Analog Sensor eg J3-A1
 *  Sensor  one side to GND Mayfly J3-GND
 * 
 *For EC calculation 
 *  (better to be ratio meteric))
 *  Range of 0-3V3 with 10bit ADC - resolution of 0.003
 * 
 */

// Header Guards
#ifndef analogElecConductivity_h
#define analogElecConductivity_h

// Debugging Statement
// #define MS_ANALOGELECCONDUCTIVITY_DEBUG

#ifdef MS_ANALOGELECCONDUCTIVITY_DEBUG
#define MS_DEBUGGING_STD "analogElecConductivity"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include "math.h"

// Sensor Specific Defines
#define ANALOGELECCONDUCTIVITY_NUM_VARIABLES 1
#define ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS 2
#define ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS 0
#define ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS 0

#define ANALOGELECCONDUCTIVITY_EC_RESOLUTION 3
#define ANALOGELECCONDUCTIVITY_EC_VAR_NUM 0

//#define ANALOGELECCONDUCTIVITY_TEMPERATURE_RESOLUTION 0
//#define ANALOGELECCONDUCTIVITY_TEMPERATURE_VAR_NUM 1

//#define ANALOGELECCONDUCTIVITY_SAMPNUM_RESOLUTION 0
//#define ANALOGELECCONDUCTIVITY_SAMPNUM_VAR_NUM 2

#define analogElecConductivityDef_Resolution 10
#define analogElecConductivityAdc_Max ((1<< analogElecConductivityDef_Resolution)-1)
#define EC_SENSOR_ADC_RANGE (1<< analogElecConductivityDef_Resolution)

#if !defined SENSOR_UNINIT_VAL
#define SENSOR_UNINIT_VAL -9999
#endif //SENSOR_UNINIT_VAL

#if 0
#define APTT_KELVIN_OFFSET 273.15	
#define AP_TYPES 4
#define AP_LPBATT_TBL_NUM (AP_TYPES+1)
#ifndef AP_THERMISTOR_SERIES_R_OHMS 
#define AP_THERMISTOR_SERIES_R_OHMS 75000
#endif//AP_THERMISTOR_SERIES_R_OHMS 
typedef enum {
   APTT_NCP15XH193F03RC=0, //Murata Thermistor 
   APTT_1,
   APTT_2, //
   APTT_NUM, ///Number of Thermistor Types supported 
   APTT_UNDEF,
} ac_type_thermistor_t;
const float AP_LBATT_TBL[APTT_NUM][AP_LPBATT_TBL_NUM] = {
//    0    1    2    3   
//   A     B   C   FUT
{0.0008746904041902967 , 0.0002532755006290475, 1.877479431169023e-7, 0.0},
    //{3.3, 3.4, 3.6, 3.8, 0.05},
    //{3.2, 3.3, 3.4, 3.7, 0.04},
    {0.0, 0.0, 0.0, 0.0, 0.0}
   };
 #endif //0
#if !defined ProcAdcDef_Reference
// one of eAnalogReference 
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
    const float Rseries_ohms=500; //that is R1 + any series port resistance
    const float TemperatureCoef=0.019; //depends on what chemical/transport is being measured    
    //********************** Cell Constant For Ec Measurements *********************//
    //Mine was around 2.9 with plugs being a standard size they should all be around the same
    //But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
    const float sensorEC_Konst=2.88;

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
