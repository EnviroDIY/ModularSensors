/**
 * @file BatteryManagement.h
 * @copyright 2021 Neil Hancock Assigned Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library 
 * @author Neil Hancock
 *
 * @brief Contains the BatteryManagement Class
 *
 * These are for metadata on the processor functionality.
 */
/* clang-format off */
/**
 * @defgroup sensor_processor Processor Metadata
 * Classes for the using the processor as a sensor.
 *
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section power_maangement_intro Introduction
 *
 * The system relieas on power to run, and different actions require different levels of power.
 * Some actions - like communications - may be dropped for lack of power.
 * 
 * This provides an api to determine if there is power available for specific activities
 */
/* clang-format on */

// Header Guards
#ifndef SRC_BATTERYMANAGEMENT_H_
#define SRC_BATTERYMANAGEMENT_H_

// Debugging Statement
// #define MS_BATTERYMANAGEMENT_DEBUG

#ifdef MS_BATTERYMANAGEMENT_DEBUG
#define MS_DEBUGGING_STD "BatteryManagement"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD

/** @ingroup power_management */
/**@{*/


/**@}*/

#define ProcessorStatsDef_Resolution 10
#define ProcAdc_Max ((1 << ProcessorStatsDef_Resolution) - 1)

/* Battery Thresholds - may need adjusting for specific battery sizes and Mayfly
   loads. A LiIon manufactuer typically has a protection IC that will do a Low
   Voltage Disconnect (LVD) at some voltage ~ 3.0V to protect the chemistry of
   the battery. As the current drawn takes the battery to the LVD thresshold, it
   will then drop to 0V. After that it will needs some conditioning - so the
   following is a first pass. It may require some hysteresis thresholds to be
   added.
*/

typedef enum {
    //Maps to rows in BM_LBATT_TBL
    BMBR_ALL = 0,  // ALL works
    BMBR_0500mA,   // 500mA or less
    BMBR_2000mA,   // 2000mA 

    BMBR_LiSi18,  // LiSiOCL2 19Ahr/larger Pulse 150mA "D" cell - Nomonal 3.6
                  // discharged at 3.2V
    BMBR_3D,      // 3D * 1.6V MnO2 18AHR Pulse ?100mA "D" cell - Nomonal 4.8
                  // discharged at 2.4V
    // 3 MnO2 "C" cell - higher impedance than "D" cell
    BMBR_NUM,  /// Number of Battery types supported
    BMBR_UNDEF,
} bm_battery_type_rating_t;
#define BMBR_LIION BMBR_0500mA
// Default should be all to allow it to power up until set by user
#define BMBR_BAT_TYPE_DEF BMBR_ALL
typedef enum {
    BM_PWR_STATUS_REQ = 0,  // 0 returns status
    // Order of following important and should map to bm_Lbatt_status_t
    BM_PWR_USEABLE_REQ,  // 1 returns status if 1 or above, or else 0
    BM_PWR_LOW_REQ,      // 2 returns status if 2 or above, or else 0
    BM_PWR_MEDIUM_REQ,   // 3 returns status if 3 or above, or else 0
    BM_PWR_HEAVY_REQ,    // 4 returns status if 4 or above, or else 0
                         // End of regular STATUS
} bm_pwr_req_t;
typedef enum {
    BM_LBATT_UNUSEABLE_STATUS = 0,  // 0 BM_LBATT_REQUEST_STATUS,
    // Order of following important and should maps to  bm_pwr_req_t
    BM_LBATT_BARELYUSEABLE_STATUS,  // 1 ret status if >0, or 0 eg low W sensor
    BM_LBATT_LOW_STATUS,  // 2 returns status if >1, or else 0 eg high W sensor
    BM_LBATT_MEDIUM_STATUS,  // 3 returns status if >2, or else 0 eg power WiFi
    BM_LBATT_HEAVY_STATUS,   // 4 ret status if >3, or else 0 eg powering
                             // Cell Phone End
} bm_Lbatt_status_t;

// The main class for the Processor
// Only need a sleep and wake since these DON'T use the default of powering
// up and down
/**
 * @brief The main class to use the main processor (MCU) as a sensor.
 *
 * @ingroup sensor_processor
 */
class BatteryManagement 
{
 public:
    /**
     * @brief Construct a new Processor Stats object.
     *
     */
    explicit BatteryManagement();
    /**
     * @brief Destroy the Processor Stats object
     */
    ~BatteryManagement();


    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */

 public:
    /* Battery Usage Level definitions
     * LiIon (and any battery) has a charge and internal resistance.
     * For the Logger there are different types of loads that need to be mapped
     * to what the battery is capable of delivering. In terms of priority the
     * system needs to: Determine battery charge status and what loads are
     * possible
     * - UNUSEABLE charge so low, it should immediately sleep and wait till
     * charge improves (solar?)
     * - USEABLE charge to run a basic logger, basic sensors and SD card
     * logging, but no internet comms
     * - LOW Battery, that can run sensors that use more power, and SD card
     * logging, but no internet comms
     * - MEDIUM Battery, that can support low power communications (eg
     * WiFi/900Mz/ CatNB?) and all sensors & SD card logging
     * - Heavy power demand, that can support cellular comms, and all sensors
     * and SD card logging
     */
    bm_Lbatt_status_t
         isBatteryStatusAbove(bool         newBattReading = false,
                              bm_pwr_req_t status_req     = BM_PWR_USEABLE_REQ);
    void setBatteryV(float newReading);
    void setBatteryType(bm_battery_type_rating_t batteryType);
    void printBatteryThresholds(void);
    /* Get Battery voltage
     * This measures a complex Voltage, which when there is no other external
     * voltage, is the LiIon Battery. What is really desired, is to measure the
     * charge remaining in the LiIon battery (this is the API to do that). One
     * way of measuring available charge is to measure both the float volt (with
     * a small inconsequential load) and load voltage (voltage with system load)
     * If the USB V is present, then it measures this voltage, but that
     * typically means there is a good powersupply. There are a number of ways
     * of measuring the voltage with degrees of accuracy. methdod1:Using the
     * external 3.3V as reference, results in the most accurate absolute voltage
     * providing the measured voltage range is 3.8V-15V. Its inacurrate beneath
     * some threshold due to the different losses of the circuit parts. Ptactise
     * has seen it become less accurate under 3.9V method2: determing an
     * accurate voltage drop from an applied load, the Mayfly's mega1284P
     * internal 2.4V reference can be used.
     */
    float getBatteryVm1() {return _batteryExt_V;}
    // float getBatteryVm2a(bool newBattReading);//snap float voltage
    // float getBatteryVm2b(bool newBattReading);//snap load voltage
    // float getBatteryVm2diff(bool newBattReading); //getDifference in measured
    // batteryV using internal reference

    /* atl_extension */
    float _batteryExt_V = 0;

#define BM_TYPES 4
#define BM_LPBATT_TBL_NUM (BM_TYPES + 1)
    //
    bm_battery_type_rating_t _battery_type;
    // use EDIY_PROGMEM
    const float BM_LBATT_TBL[BMBR_NUM][BM_LPBATT_TBL_NUM] = {

//    0    1    2    3   Hyst
//   Use  Low  Med  Good
#if defined ARDUINO_AVR_ENVIRODIY_MAYFLY
        // Mayfly rev 0.5,0.4 : Proc ADC Reported V is that of the Vin, which is
        // only valid range 3.7V to 6V
        // actual/Mayfly uP Measures - one Mayfly non-linear mapping
        //  3.70/3.33 3.80/3.38  3.90/3.59 3.95/3.654
        //  4.00/3.79 4.05/3.87 4.10/3.96 4.15/4.09 4.20/4.12
        // USE Low  Med  Heavy Hyst see BM_LBATT_xx
        // 1    2    3    4
        {0.1, 0.2, 0.3, 0.4, 0.05},      // 0 All readings return OK
        {3.5, 3.6, 3.85, 4.00, 0.04},    // 1 BMBR_0500mA
        {3.5, 3.6, 3.7, 3.8, 0.03},      // 2 BMBR_?000mA Uncalibrated
        {3.35, 3.38, 3.42, 3.46, 0.03},  // 3 PLSR_LiSi18
        {2.4, 2.5, 2.60, 2.7, 0.03},     // 4 fut Test 3*D to 2.4 to 4.8V
    // There could possibly be a MAYFLY off the ExternalVoltage ADS1115, it
    // still is limited to 3.3V inpu
#else   //
        {0.1, 0.2, 0.3, 0.4, 0.05},      // 0 All readings return OK
        {3.7,  3.8, 3.90, 4.00, 0.05},   // BMBR_0500mA, //500mA or less
        {3.2,  3.6, 3.85, 3.95, 0.04},   // BMBR_2000mA
        {2.90, 3.1, 3.20, 3.30, 0.03},   // PLSR_LiSi18
        {3.5, 3.6, 4.00, 4.6, 0.03},     // 3*D to 2.4 to 4.8V
#endif  // ARDUINO_AVR_ENVIRODIY_MAYFLY
    };
// use EDIY_PROGMEM
#define BM_LBATT_USEABLE_V BM_LBATT_TBL[_battery_type][0]
#define BM_LBATT_LOW_V BM_LBATT_TBL[_battery_type][1]
#define BM_LBATT_MEDIUM_V BM_LBATT_TBL[_battery_type][2]
#define BM_LBATT_HEAVY_V BM_LBATT_TBL[_battery_type][3]
#define BM_LBATT_HYSTERESIS BM_LBATT_TBL[_battery_type][4]
#if 0
#define BM_LBATT_USEABLE_V 3.7
#define BM_LBATT_LOW_V 3.8
#define BM_LBATT_MEDIUM_V 3.9
#define BM_LBATT_HEAVY_V 4.0
#define BM_LBATT_HYSTERESIS 0.05
#endif
};



/**@}*/
#endif  // SRC_SENSORS_PROCESSORSTATS_H_
