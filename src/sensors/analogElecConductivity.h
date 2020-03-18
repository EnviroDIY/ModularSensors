/*
 *analogElecConductivity.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *
 *This sensor encapsulates sensors using an anlog input .
 * EC from IEC probe, requires ADC_PIN, Source Resistance, Applied Voltage, water temperature. Requires switched power pin
 *     The water Temperature must be calculated seperately.
 *
 *For EC calculation 
 *  Range of 0-5V with 10bit ADC - resolution of 0.005
 *
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
#define ANALOGELECCONDUCTIVITY_NUM_VARIABLES 3
#define ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS 0
#define ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS 0
#define ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS 0

#define ANALOGELECCONDUCTIVITY_EC_RESOLUTION 3
#define ANALOGELECCONDUCTIVITY_EC_VAR_NUM 0

#define ANALOGELECCONDUCTIVITY_TEMPERATURE_RESOLUTION 0
#define ANALOGELECCONDUCTIVITY_TEMPERATURE_VAR_NUM 1

#define ANALOGELECCONDUCTIVITY_SAMPNUM_RESOLUTION 0
#define ANALOGELECCONDUCTIVITY_SAMPNUM_VAR_NUM 2

#define analogElecConductivityDef_Resolution 10
#define ProcAdc_Max ((1<< analogElecConductivityDef_Resolution)-1)


#if !defined SENSOR_UNINIT_VAL
#define SENSOR_UNINIT_VAL -9999
#endif //SENSOR_UNINIT_VAL


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
 

class analogElecConductivity : public Sensor
{
public:
    // Need to know the  Mayfly version because the battery resistor depends on it
    analogElecConductivity(const char *version);
    ~analogElecConductivity();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;
    void set_active_sensors(uint8_t sensors_mask);
    uint8_t which_sensors_active(void);
    //void setWaterTemperature(float  WaterTemperature_C); 
    void setWaterTemperature(float  *WaterTemperature_C); 
    //void setEc_k(int8_t powerPin, int8_t adcPin, float  sourceResistance_ohms,float  appliedV_V, uint8_t probeType); 



private:
    const char *_version;
    int8_t _EcPowerPin;
    int8_t _EcAdcPin;

    //float _WaterTemperature_C;
    float *_ptrWaterTemperature_C;

};


// For the battery supplying power to the processor
class analogElecConductivity_EC : public Variable
{
public:
    analogElecConductivity_EC(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "EC_UUID")
      : Variable(parentSense,
                 (const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                 (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                 "ElectricalConductivity", "uS",
                 varCode, uuid)
    {}
    analogElecConductivity_EC()
      : Variable((const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                 (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                 "ElectricalConductivity", "uS", "EC_UUID")
    {}
    ~analogElecConductivity_EC(){}
};







#endif  //analogElecConductivity
