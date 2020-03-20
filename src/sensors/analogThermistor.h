/*
 *analogThermistor.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *
 *This sensor encapsulates sensors using an anlog input .

 * Temperature from thermistor, Requires: adcPin, Source Resistance, thermistor type, 
 *
 *
 * By default all sensors defined are measured.
 * The sensors not used can be turned off so that they cause minimal runtime impacts.
 * The downside of this module is that all sensors are defined and take code, irrespective of if they actually exist
 * 
 */

// Header Guards
#ifndef analogThermistor_h
#define analogThermistor_h

// Debugging Statement
// #define MS_ANALOGTHERMISTOR_DEBUG

#ifdef MS_ANALOGTHERMISTOR_DEBUG
#define MS_DEBUGGING_STD "analogThermistor"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include "math.h"

// Sensor Specific Defines
#define ANALOGTHERMISTOR_NUM_VARIABLES 1
#define ANALOGTHERMISTOR_WARM_UP_TIME_MS 0
#define ANALOGTHERMISTOR_STABILIZATION_TIME_MS 0
#define ANALOGTHERMISTOR_MEASUREMENT_TIME_MS 0

//#define ANALOGTHERMISTOR_EC_RESOLUTION 3
//#define ANALOGTHERMISTOR_EC_VAR_NUM 0

#define ANALOGTHERMISTOR_TEMPERATURE_RESOLUTION 2
#define ANALOGTHERMISTOR_TEMPERATURE_VAR_NUM 0

//#define ANALOGTHERMISTOR_SAMPNUM_RESOLUTION 0
//#define ANALOGTHERMISTOR_SAMPNUM_VAR_NUM 2
#ifndef analogThermistorDef_Resolution
#define analogThermistorDef_Resolution 10
#endif //

#define analogThermistorAdc_Max ((1<< analogThermistorDef_Resolution)-1)
#define THERMISTOR_SENSOR_ADC_RANGE (1<< analogThermistorDef_Resolution)

//nalogThermistorDef_Resolution
/* Masks to control polling of sensors */

//#define ANALOGTHERMISTOR_POLLMASK_EC 0x01
//#define ANALOGTHERMISTOR_POLLMASK_TEMPERATURE 0x02
//#define ANALOGTHERMISTOR_POLLMASK_VAR 0x04
//#define ANALOGTHERMISTOR_POLLMASK_ALL (ANALOGTHERMISTOR_POLLMASK_A|ANALOGTHERMISTOR_POLLMASK_V|ANALOGTHERMISTOR_POLLMASK_W)
//#define ANALOGTHERMISTOR_POLLMASK_ALL (ANALOGTHERMISTOR_POLLMASK_EC|ANALOGTHERMISTOR_POLLMASK_TEMPERATURE)

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
 

class analogThermistor : public Sensor
{
public:
    // Need to know the  Mayfly version because the battery resistor depends on it
    analogThermistor(int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage=1);
    ~analogThermistor();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;
    //void set_active_sensors(uint8_t sensors_mask);
    //uint8_t which_sensors_active(void);
    //void setWaterTemperature(float  WaterTemperature_C); 
    //void setWaterTemperature(float  *WaterTemperature_C); 
    //void setEc_k(int8_t powerPin, int8_t adcPin, float  sourceResistance_ohms,float  appliedV_V, uint8_t probeType); 

    void setTemperature_k( uint8_t thermistorType=APTT_UNDEF,float  sourceResistance_ohms=AP_THERMISTOR_SERIES_R_OHMS); //,float  appliedV_V,
    //void setTemperature_k(int8_t powerPin=-1, int8_t adcPin=-1, uint8_t thermistorType=APTT_UNDEF,float  sourceResistance_ohms=AP_THERMISTOR_SERIES_R_OHMS); //,float  appliedV_V,

    //float _WaterTemperature_C;
    float *_ptrWaterTemperature_C;

private:
    const char *_version;
    int8_t _TemperaturePowerPin;
    int8_t _TemperatureAdcPin;
    int8_t _thermistorType;
    float _thermistorSieresResistance_ohms;

};


// Defines the "Free Ram" This is a board diagnostic
class analogThermistor_Temperature : public Variable
{
public:
    analogThermistor_Temperature(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "Temperature_UUID")
      : Variable(parentSense,
                 (const uint8_t)ANALOGTHERMISTOR_TEMPERATURE_VAR_NUM,
                 (uint8_t)ANALOGTHERMISTOR_TEMPERATURE_RESOLUTION,
                 "Temperature", "C",
                 varCode, uuid)
    {}
    analogThermistor_Temperature()
      : Variable((const uint8_t)ANALOGTHERMISTOR_TEMPERATURE_VAR_NUM,
                 (uint8_t)ANALOGTHERMISTOR_TEMPERATURE_RESOLUTION,
                 "Temperature", "C", "Temperature_UUID")
    {}
    ~analogThermistor_Temperature(){}
};

#endif //analogThermistor_h
