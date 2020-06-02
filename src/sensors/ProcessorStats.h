/*
 *ProcessorStats.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for metadata on the processor functionality.
 *
 *For battery voltage:
 *  Range of 0-5V with 10bit ADC - resolution of 0.005
 *
 * If the processor is awake, it's ready to take a reading.
*/

// Header Guards
#ifndef ProcessorStats_h
#define ProcessorStats_h

// Debugging Statement
// #define MS_PROCESSORSTATS_DEBUG

#ifdef MS_PROCESSORSTATS_DEBUG
#define MS_DEBUGGING_STD "ProcessorStats"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define PROCESSOR_NUM_VARIABLES 3
#define PROCESSOR_WARM_UP_TIME_MS 0
#define PROCESSOR_STABILIZATION_TIME_MS 0
#define PROCESSOR_MEASUREMENT_TIME_MS 0

#define PROCESSOR_BATTERY_RESOLUTION 3
#define PROCESSOR_BATTERY_VAR_NUM 0

#define PROCESSOR_RAM_RESOLUTION 0
#define PROCESSOR_RAM_VAR_NUM 1

#define PROCESSOR_SAMPNUM_RESOLUTION 0
#define PROCESSOR_SAMPNUM_VAR_NUM 2

#define ProcessorStatsDef_Resolution 10
#define ProcAdc_Max ((1<< ProcessorStatsDef_Resolution)-1)

/* Battery Thresholds - may need adjusting for specific battery sizes and Mayfly loads.
   A LiIon manufactuer typically has a protection IC that will do a 
   Low Voltage Disconnect (LVD) at some voltage ~ 3.0V to protect the chemistry of the battery.
   As the current drawn takes the battery to the LVD thresshold, it will then drop to 0V.
   After that it will needs some conditioning - so the following is a first pass.
   It may require some hysteresis thresholds to be added.
*/


typedef enum {
   PSLR_0500mA=0, //500mA or less 
   PSLR_1000mA,
   PSLR_4000mA, //4000mA or more
   //Fut expanded to batterys
   PLSR_LiSi09,// LiSiOCL2  9Ahr        Pulse 100mA "C" cell - Nomonal 3.6 discharged at 3.2V
   PLSR_LiSi18,// LiSiOCL2 19Ahr/larger Pulse 150mA "D" cell - Nomonal 3.6 discharged at 3.2V

   // 3 MnO2 "D" cell   Nominal 4.8V, discharged at 3.2V, though has energy to 2.4V
   // 3 MnO2 "C" cell - higher impedance than "D" cell
   PSLR_NUM, ///Number of Battery types supported 
   PSLR_UNDEF,
} ps_liion_rating_t;
#define PSLR_LIION  PSLR_0500mA
#define PLSR_BAT_TYPE_DEF  PSLR_0500mA
typedef enum {
   PS_PWR_STATUS_REQ=0, //0 returns status
   //Order of following important and should map to ps_Lbatt_status_t
   PS_PWR_USEABLE_REQ,//1 returns status if 1 or above, or else 0
   PS_PWR_LOW_REQ,    //2 returns status if 2 or above, or else 0
   PS_PWR_MEDIUM_REQ, //3 returns status if 3 or above, or else 0
   PS_PWR_HEAVY_REQ,  //4 returns status if 4 or above, or else 0
   //End of regular STATUS 
} ps_pwr_req_t;
typedef enum {
   PS_LBATT_UNUSEABLE_STATUS=0, //0 PS_LBATT_REQUEST_STATUS,
   //Order of following important and should maps to  ps_pwr_req_t
   PS_LBATT_BARELYUSEABLE_STATUS,//1 returns status if above 1, or else 0
   PS_LBATT_LOW_STATUS,    //2 returns status if above 2, or else 0
   PS_LBATT_MEDIUM_STATUS, //3 returns status if above 3, or else 0
   PS_LBATT_HEAVY_STATUS,  //4 returns status if above 4, or else 0
   //End of regular STATUS 
} ps_Lbatt_status_t;

 
// The "Main" class for the Processor
// Only need a sleep and wake since these DON'T use the default of powering up and down
class ProcessorStats : public Sensor
{
public:
    // Need to know the Mayfly version because the battery resistor depends on it
    ProcessorStats(const char *version);
    ~ProcessorStats();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

  /* Battery Usage Level definitions
   * LiIon (and any battery) has a charge and internal resistance.
   * For the Logger there are different types of loads that need to be mapped to what the battery is capable of delivering.
   * In terms of priority the system needs to:
   * Determine battery charge status and what loads are possible
   * - UNUSEABLE charge so low, it should immediately sleep and wait till charge improves (solar?)
   * - USEABLE charge to run a basic logger, basic sensors and SD card logging, but no internet comms
   * - LOW Battery, that can run sensors that use more power, and SD card logging, but no internet comms
   * - MEDIUM Battery, that can support low power communications (eg WiFi/900Mz/ CatNB?) and all sensors & SD card logging
   * - Heavy power demand, that can support cellular comms, and all sensors and SD card logging
   */
    ps_Lbatt_status_t  isBatteryStatusAbove(bool newBattReading=false,ps_pwr_req_t status_req=PS_PWR_USEABLE_REQ);
    void setBatteryType(ps_liion_rating_t LiionType);
    void printBatteryThresholds(void);
    /* Get Battery voltage
     * This measures a complex Voltage, which when there is no other external voltage, is the LiIon Battery.
     * What is really desired, is to measure the charge remaining in the LiIon battery (this is the API to do that).
     *   One way of measuring available charge is to measure both the float volt (with a small inconsequential load) and load voltage (voltage with system load) 
     * If the USB V is present, then it measures this voltage, but that typically means there is a good powersupply.
     * There are a number of ways of measuring the voltage with degrees of accuracy.
     * methdod1:Using the external 3.3V as reference, results in the most accurate absolute voltage providing the measured voltage range is 3.8V-15V.
     *   Its inacurrate beneath some threshold due to the different losses of the circuit parts. Ptactise has seen it become less accurate under 3.9V
     * method2: determing an accurate voltage drop from an applied load, the Mayfly's mega1284P internal 2.4V reference can be used.
    */
    float getBatteryVm1(bool newBattReading);
    float getBatteryVm1(float *BattV);
    //float getBatteryVm2a(bool newBattReading);//snap float voltage
    //float getBatteryVm2b(bool newBattReading);//snap load voltage
    //float getBatteryVm2diff(bool newBattReading); //getDifference in measured batteryV using internal reference

#define PS_TYPES 4
#define PS_LPBATT_TBL_NUM (PS_TYPES+1)
//    
 ps_liion_rating_t _liion_type;
//use EDIY_PROGMEM
const float PS_LBATT_TBL[PSLR_NUM][PS_LPBATT_TBL_NUM] = {
//    0    1    2    3   Hyst
//   Use  Low  Med  Good
    {3.3, 3.4, 3.6, 3.8, 0.05}, //PSLR_0500mA, //500mA or less 
    {3.2, 3.3, 3.4, 3.7, 0.04}, //PSLR_1000mA
    {3.1, 3.2, 3.3, 3.6, 0.03}, //PSLR_4000mA
    {3.10, 3.20, 3.30, 3.40, 0.03}, //PLSR_LiSi09
    {2.90, 3.10, 3.20, 3.30, 0.03}, //PLSR_LiSi18
   };
//use EDIY_PROGMEM
#define PS_LBATT_USEABLE_V  PS_LBATT_TBL[_liion_type][0]
#define PS_LBATT_LOW_V      PS_LBATT_TBL[_liion_type][1]
#define PS_LBATT_MEDIUM_V   PS_LBATT_TBL[_liion_type][2]
#define PS_LBATT_HEAVY_V    PS_LBATT_TBL[_liion_type][3]
#define PS_LBATT_HYSTERESIS PS_LBATT_TBL[_liion_type][4]
#if 0
#define PS_LBATT_USEABLE_V 3.7  
#define PS_LBATT_LOW_V 3.8
#define PS_LBATT_MEDIUM_V 3.9
#define PS_LBATT_HEAVY_V 4.0
#define PS_LBATT_HYSTERESIS 0.05 
#endif
private:
    const char *_version;
    int8_t _batteryPin;
    int16_t sampNum;
    float LiIonBatt_V=-999.0;
};


// For the battery supplying power to the processor
class ProcessorStats_Battery : public Variable
{
public:
    ProcessorStats_Battery(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "Battery")
      : Variable(parentSense,
                 (const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                 (uint8_t)PROCESSOR_BATTERY_RESOLUTION,
                 "batteryVoltage", "volt",
                 varCode, uuid)
    {}
    ProcessorStats_Battery()
      : Variable((const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                 (uint8_t)PROCESSOR_BATTERY_RESOLUTION,
                 "batteryVoltage", "volt", "Battery")
    {}
    ~ProcessorStats_Battery(){}
};


// Defines the "Free Ram" This is a board diagnostic
class ProcessorStats_FreeRam : public Variable
{
public:
    ProcessorStats_FreeRam(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "FreeRam")
      : Variable(parentSense,
                 (const uint8_t)PROCESSOR_RAM_VAR_NUM,
                 (uint8_t)PROCESSOR_RAM_RESOLUTION,
                 "freeSRAM", "Bit",
                 varCode, uuid)
    {}
    ProcessorStats_FreeRam()
      : Variable((const uint8_t)PROCESSOR_RAM_VAR_NUM,
                 (uint8_t)PROCESSOR_RAM_RESOLUTION,
                 "freeSRAM", "Bit", "FreeRam")
    {}
    ~ProcessorStats_FreeRam(){}
};


// Defines the "Sample Number" This is a board diagnostic
class ProcessorStats_SampleNumber : public Variable
{
public:
    ProcessorStats_SampleNumber(Sensor *parentSense,
                                const char *uuid = "",
                                const char *varCode = "SampNum")
      : Variable(parentSense,
                 (const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                 (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION,
                 "sequenceNumber", "Dimensionless",
                 varCode, uuid)
    {}
    ProcessorStats_SampleNumber()
      : Variable((const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                 (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION,
                 "sequenceNumber", "Dimensionless", "SampNum")
    {}
    ~ProcessorStats_SampleNumber(){}
};

#endif  // Header Guard
