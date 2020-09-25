/**
 * @file ProcessorStats.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ProcessorStats sensor subclass and the variable
 * subclasses ProcessorStats_Battery, ProcessorStats_FreeRam, and
 * ProcessorStats_SampleNumber.
 *
 * These are for metadata on the processor functionality.
 */
/* clang-format off */
/**
 * @defgroup processor_group Processor Metadata
 * Classes for the using the processor as a sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section processor_intro Introduction
 *
 * The processor can return the number of "samples" it has taken, the amount of
 * RAM it has available and, for some boards, the battery voltage (EnviroDIY
 * Mayfly, Sodaq Mbili, Ndogo, Autonomo, and One, Adafruit Feathers).  The
 * version of the board is required as input (ie, for a EnviroDIY Mayfly: "v0.3"
 * or "v0.4" or "v0.5").  Use a blank value (ie, "") for un-versioned boards.
 * Please note that while you cannot opt to average more than one sample, it really
 * makes no sense to do so for the processor.  These values are only intended to be
 * used as diagnostics.
 *
 * @section processor_datasheet Sensor Datasheet
 * - [Atmel ATmega1284P Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet-Summary.pdf)
 * - [Atmel ATmega1284P Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet.pdf)
 * - [Atmel SAMD21 Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet-Summary.pdf)
 * - [Atmel SAMD21 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet.pdf)
 * - [Atmel ATmega16U4 32U4 Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet-Summary.pdf)
 * - [Atmel ATmega16U4 32U4 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet.pdf)
 *
 * @section processor_sensor The Processor as a Sensor
 * @ctor_doc{ProcessorStats, const char* version}
 * @subsection processor_sensor_timing Sensor Timing
 * - Timing variables do not apply to the processor in the same way they do to
 * other sensors.
 *
 * @section processor_battery Battery Voltage
 * This is the voltage as measured on the battery attached to the MCU using the
 * inbuilt ADC, if applicable.
 * - Range is assumed to be 0 to 5V
 * - Accuracy is processor dependent
 * - Result stored in sensorValues[0]
 * - Resolution is 0.005V
 *   - 0-5V with a 10bit ADC
 * - Reported as volts (V)
 * - Default variable code is batteryVoltage
 * @variabledoc{processor_battery,ProcessorStats,Battery,batteryVoltage}
 *
 * @section processor_ram Free RAM
 * This is the amount of free space on the processor when running the program.
 * This is just a diagnostic value.  This number _**should always remain the
 * same for a single logger program**_.  If this number is not constant over
 * time, there is a memory leak and something wrong with your logging program.
 * - Range is 0 to full RAM available on processor
 * - Result stored in sensorValues[1]
 * - Resolution is 1 bit
 * - Reported in bits
 * - Default variable code is freeSRAM
 * @variabledoc{processor_ram,ProcessorStats,FreeRam,freeSRAM}
 *
 * @section processor_sampno Sample Number
 * This is a board diagnostic.  It is _**roughly**_ the number of samples
 * measured since the processor was last restarted.  This value simply
 * increments up by one every time the addSingleMeasurementResult() function is
 * called for the processor sensor.  It is intended only as a rough diagnostic
 * to show when the processor restarts.
 * - Result stored in sensorValues[2]
 * - Reported as a dimensionless sequence number
 * - Default variable code is SampNum
 * @variabledoc{processor_sampno,SampleNumber,FreeRam,SampNum}

 * ___
 * @section processor_sensor_examples Example Code
 * The processor is used as a sensor in all of the examples, including the
 * @menulink{processor_sensor} example.
 *
 * @menusnip{processor_sensor}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_PROCESSORSTATS_H_
#define SRC_SENSORS_PROCESSORSTATS_H_

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

/// Sensor::_numReturnedValues; the processor can report 3 values.
#define PROCESSOR_NUM_VARIABLES 3

/**
 * @brief Sensor::_warmUpTime_ms; the processor is never powered down - there is
 * no waiting for the processor to warmup.
 */
#define PROCESSOR_WARM_UP_TIME_MS 0
/**
 * @brief Sensor::_stabilizationTime_ms; the processor is never powered down -
 * there is no waiting for the processor to stabilize.
 */
#define PROCESSOR_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the processor measurement times aren't
 * measurable
 */
#define PROCESSOR_MEASUREMENT_TIME_MS 0

/// Decimals places in string representation; battery voltage should have 3.
#define PROCESSOR_BATTERY_RESOLUTION 3
/// Battery voltage is stored in sensorValues[0]
#define PROCESSOR_BATTERY_VAR_NUM 0

/// Decimals places in string representation; RAM should have 0.
#define PROCESSOR_RAM_RESOLUTION 0
/// Free RAM is stored in sensorValues[1]
#define PROCESSOR_RAM_VAR_NUM 1

/// Decimals places in string representation; sample number should have 0.
#define PROCESSOR_SAMPNUM_RESOLUTION 0
/// Sample number is stored in sensorValues[2]
#define PROCESSOR_SAMPNUM_VAR_NUM 2

/* atl_extension */
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
    PSLR_ALL = 0,  // ALL works
    PSLR_0500mA,   // 500mA or less
    PSLR_1000mA,   // 1000mA or more
    // Fut expanded to batterys
    PLSR_LiSi18,  // LiSiOCL2 19Ahr/larger Pulse 150mA "D" cell - Nomonal 3.6
                  // discharged at 3.2V
    PLSR_3D,      // 3D * 1.6V MnO2 18AHR Pulse ?100mA "D" cell - Nomonal 4.8
                  // discharged at 2.4V
    // 3 MnO2 "C" cell - higher impedance than "D" cell
    PSLR_NUM,  /// Number of Battery types supported
    PSLR_UNDEF,
} ps_liion_rating_t;
#define PSLR_LIION PSLR_0500mA
// Default should be all to allow it to power up until set by user
#define PLSR_BAT_TYPE_DEF PSLR_ALL
typedef enum {
    PS_PWR_STATUS_REQ = 0,  // 0 returns status
    // Order of following important and should map to ps_Lbatt_status_t
    PS_PWR_USEABLE_REQ,  // 1 returns status if 1 or above, or else 0
    PS_PWR_LOW_REQ,      // 2 returns status if 2 or above, or else 0
    PS_PWR_MEDIUM_REQ,   // 3 returns status if 3 or above, or else 0
    PS_PWR_HEAVY_REQ,    // 4 returns status if 4 or above, or else 0
                         // End of regular STATUS
} ps_pwr_req_t;
typedef enum {
    PS_LBATT_UNUSEABLE_STATUS = 0,  // 0 PS_LBATT_REQUEST_STATUS,
    // Order of following important and should maps to  ps_pwr_req_t
    PS_LBATT_BARELYUSEABLE_STATUS,  // 1 returns status if above 1, or else 0
    PS_LBATT_LOW_STATUS,            // 2 returns status if above 2, or else 0
    PS_LBATT_MEDIUM_STATUS,         // 3 returns status if above 3, or else 0
    PS_LBATT_HEAVY_STATUS,          // 4 returns status if above 4, or else 0
                                    // End of regular STATUS
} ps_Lbatt_status_t;

// The main class for the Processor
// Only need a sleep and wake since these DON'T use the default of powering up
// and down
/**
 * @brief The main class to use the main processor (MCU) as a sensor.
 *
 * @ingroup processor_group
 */
class ProcessorStats : public Sensor {
 public:
    /**
     * @brief Construct a new Processor Stats object
     *
     * Need to know the Mayfly version because the battery resistor depends on
     * it
     *
     * @param version The version of the MCU, if applicable.
     * - For an EnviroDIY Mayfly, the version should be one of "v0.3", "v0.4",
     * "v0.5", or "v0.5b."  There *is* a difference between the versions!
     *
     * @note It is not possible to average more than one measurement for
     * processor variables - it just doesn't make sense for them.
     */
    explicit ProcessorStats(const char* version);
    /**
     * @brief Destroy the Processor Stats object
     */
    ~ProcessorStats();

    /**
     * @copydoc Sensor::getSensorLocation()
     *
     * This returns the processor name as read from the compiler variable.
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    const char* _version;
    int8_t      _batteryPin;
    int16_t     sampNum;

    /* atl_extension */
    float LiIonBatt_V = -999.0;

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
    ps_Lbatt_status_t
         isBatteryStatusAbove(bool         newBattReading = false,
                              ps_pwr_req_t status_req     = PS_PWR_USEABLE_REQ);
    void setBatteryV(float newReading);
    void setBatteryType(ps_liion_rating_t LiionType);
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
    float getBatteryVm1(bool newBattReading);
    float getBatteryVm1(float* BattV);
    // float getBatteryVm2a(bool newBattReading);//snap float voltage
    // float getBatteryVm2b(bool newBattReading);//snap load voltage
    // float getBatteryVm2diff(bool newBattReading); //getDifference in measured
    // batteryV using internal reference

    /* atl_extension */
    float _batteryExt_V = 0;
    bool  _usebatExt    = false;

#define PS_TYPES 4
#define PS_LPBATT_TBL_NUM (PS_TYPES + 1)
    //
    ps_liion_rating_t _liion_type;
    // use EDIY_PROGMEM
    const float PS_LBATT_TBL[PSLR_NUM][PS_LPBATT_TBL_NUM] = {

//    0    1    2    3   Hyst
//   Use  Low  Med  Good
#if defined ARDUINO_AVR_ENVIRODIY_MAYFLY
        // Mayfly rev 0.5,0.4 : Proc ADC Reported V is that of the Vin, which is
        // only valid range 3.7V to 6V
        // actual/Mayfly uP Measures - one Mayfly non-linear mapping
        //  3.70/3.33 3.80/3.38  3.90/3.59 3.95/3.654
        //  4.00/3.79 4.05/3.87 4.10/3.96 4.15/4.09 4.20/4.12
        {0.1, 0.2, 0.3, 0.4, 0.05},      // 0 All readings return OK
        {3.5, 3.6, 3.7, 3.75, 0.04},     // 1 PSLR_0500mA
        {3.5, 3.6, 3.3, 3.7, 0.03},      // 2 PSLR_1000mA
        {3.35, 3.38, 3.42, 3.46, 0.03},  // 3 PLSR_LiSi18
        {2.4, 2.5, 2.60, 2.7, 0.03},     // 4 fut Test 3*D to 2.4 to 4.8V
    // There could possibly be a MAYFLY off the ExternalVoltage ADS1115, it
    // still is limited to 3.3V inpu
#else   //
        {3.3, 3.4, 3.6, 3.8, 0.05},      // PSLR_0500mA, //500mA or less
        {3.2, 3.3, 3.4, 3.7, 0.04},      // PSLR_1000mA
        {3.1, 3.2, 3.3, 3.6, 0.03},      // PSLR_4000mA
        {2.90, 3.10, 3.20, 3.30, 0.03},  // PLSR_LiSi18
        {3.5, 3.6, 4.00, 4.6, 0.03},     // 3*D to 2.4 to 4.8V
#endif  // ARDUINO_AVR_ENVIRODIY_MAYFLY
    };
// use EDIY_PROGMEM
#define PS_LBATT_USEABLE_V PS_LBATT_TBL[_liion_type][0]
#define PS_LBATT_LOW_V PS_LBATT_TBL[_liion_type][1]
#define PS_LBATT_MEDIUM_V PS_LBATT_TBL[_liion_type][2]
#define PS_LBATT_HEAVY_V PS_LBATT_TBL[_liion_type][3]
#define PS_LBATT_HYSTERESIS PS_LBATT_TBL[_liion_type][4]
#if 0
#define PS_LBATT_USEABLE_V 3.7
#define PS_LBATT_LOW_V 3.8
#define PS_LBATT_MEDIUM_V 3.9
#define PS_LBATT_HEAVY_V 4.0
#define PS_LBATT_HYSTERESIS 0.05
#endif
};


/**
 * @brief The Variable sub-class used for the
 * [battery voltage output](@ref processor_battery) measured by the processor's
 * on-board ADC.
 *
 * @ingroup processor_group
 */
class ProcessorStats_Battery : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_Battery object.
     *
     * @param parentSense The parent ProcessorStats providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "batteryVoltage".
     */
    explicit ProcessorStats_Battery(ProcessorStats* parentSense,
                                    const char*     uuid    = "",
                                    const char*     varCode = "Battery")
        : Variable(parentSense, (const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                   (uint8_t)PROCESSOR_BATTERY_RESOLUTION, "batteryVoltage",
                   "volt", varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_Battery object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_Battery()
        : Variable((const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                   (uint8_t)PROCESSOR_BATTERY_RESOLUTION, "batteryVoltage",
                   "volt", "Battery") {}
    /**
     * @brief Destroy the ProcessorStats_Battery object - no action needed.
     */
    ~ProcessorStats_Battery() {}
};


/**
 * @brief The Variable sub-class used for the
 * [free RAM](@ref processor_ram) measured by the MCU.
 *
 * This is the amount of free space on the processor when running the program.
 * This is just a diagnostic value.  This number _**should always remain the
 * same for a single logger program**_.  If this number is not constant over
 * time, there is a memory leak and something wrong with your logging program.
 *
 * @ingroup processor_group
 */
class ProcessorStats_FreeRam : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_FreeRam object.
     *
     * @param parentSense The parent ProcessorStats providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "FreeRam".
     * @note While this variable is included, the value of it should never
     * change.  If it does change, that's a sign of a memory leak in your
     * program which will eventually cause your board to crash.
     */
    explicit ProcessorStats_FreeRam(ProcessorStats* parentSense,
                                    const char*     uuid    = "",
                                    const char*     varCode = "FreeRam")
        : Variable(parentSense, (const uint8_t)PROCESSOR_RAM_VAR_NUM,
                   (uint8_t)PROCESSOR_RAM_RESOLUTION, "freeSRAM", "Bit",
                   varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_FreeRam object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_FreeRam()
        : Variable((const uint8_t)PROCESSOR_RAM_VAR_NUM,
                   (uint8_t)PROCESSOR_RAM_RESOLUTION, "freeSRAM", "Bit",
                   "FreeRam") {}
    /**
     * @brief Destroy the ProcessorStats_FreeRam object - no action needed.
     */
    ~ProcessorStats_FreeRam() {}
};


/**
 * @brief The Variable sub-class used for the
 * [sample number output](@ref processor_sampno) from the main processor.
 *
 * This is a board diagnostic.  It is _**roughly**_ the number of samples
 * measured since the processor was last restarted.  This value simply
 * increments up by one every time the addSingleMeasurementResult() function is
 * called for the processor sensor.  It is intended only as a rough diagnostic
 * to show when the processor restarts.
 *
 * @ingroup processor_group
 */
class ProcessorStats_SampleNumber : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_SampleNumber object.
     *
     * @param parentSense The parent ProcessorStats providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SampNum".
     */
    explicit ProcessorStats_SampleNumber(ProcessorStats* parentSense,
                                         const char*     uuid    = "",
                                         const char*     varCode = "SampNum")
        : Variable(parentSense, (const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                   (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION, "sequenceNumber",
                   "Dimensionless", varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_SampleNumber object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_SampleNumber()
        : Variable((const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                   (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION, "sequenceNumber",
                   "Dimensionless", "SampNum") {}
    /**
     * @brief Destroy the ProcessorStats_SampleNumber() object - no action
     * needed.
     */
    ~ProcessorStats_SampleNumber() {}
};

#endif  // SRC_SENSORS_PROCESSORSTATS_H_
