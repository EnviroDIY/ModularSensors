/**
 * @file STSTC3100_Sensor.h
 * @copyright 2020 Neil Hancock, assigned to the Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library 
 * @author Neil Hancock.  Based on the AtlasParent.xx & TIINA219 of the Stroud Water Research Center
 * 
 *
 * @brief Contains the STSTC3100_Sensor sensor subclass which is itself the
 * parent class for all Stc3100 sensors.
 *
 * This depends on the Arduino core Wire library.  
 *
 */
/* clang-format off */
/**
 * @defgroup stc3100_group Stc3100 Circuit
 * The Sensor and Variable objects for Stc3100 circuits.
 *
 * @ingroup the_sensors
 *
 * This library currently supports the Stc3100 IC as a sensors
 *
 *
 * The device is connected to the LiIon battery and needs to run continously for recording energy used
 * It can be put into low power mode to save about 100uA (max spec). In low power it uses 2uA 
 *
 * The code in ModularSensors _**requires the Stc3100 Sensors to communicate over
 * I2C**_.  
 *
 * @section stc3100_group_flags Build flags
 *
 * - `-D MS_STSTC3100SENSOR_DEBUG`
 *      - switches on debug
 *
 * @warning **The STC3100 as a battery monitor is expected to be powered at all times.
 * It can be put into low power mode** 
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_STSTC3100SENSOR_H_
#define SRC_SENSORS_STSTC3100SENSOR_H_

// Debugging Statement
// #define MS_STSTC3100SENSOR_DEBUG

#ifdef MS_STSTC3100SENSOR_DEBUG
#define MS_DEBUGGING_STD "Stc3100sensor"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>
#include "STC3100dm.h"

// Sensor Specific Defines
/** @ingroup sensor_stc3100 */
/**@{*/

/// @brief Sensor::_numReturnedValues; the STC3100 can report 3 values at present - possibly more in future
////
#define STC3100_NUM_VARIABLES 4

// Sensor Specific Defines
/** @ingroup sensor_stc3100 */
/**@{*/

/// @brief Sensor::_numReturnedValues; the STC3100 can average readings
////
#define STC3100_NUM_MEASUREMENTS 1

/**
 * @anchor sensor_stc3100_timing
 * @name Sensor Timing
 * The sensor timing for a ST STC3100
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the STC3100 warms up in 0ms.
#define STC3100_WARM_UP_TIME_MS 0
/**
 * @brief Sensor::_stabilizationTime_ms; the STC3100 is stable after00ms.
 *
 * Stable numbers can be acheived after 500ms, but waiting up to 4s gave more
 * consistent numbers based on tests using STC3100timingTest.ino
 */
#define STC3100_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the STC3100 takes 0ms to complete a
 * measurement. 
 *
 * The internal registers are being continually updated. 
 */
#define STC3100_MEASUREMENT_TIME_MS 0

/**
 * @brief Sensor::invalid code
 * 
 * For any invalid reading, unless over-ridden 
 */
#if !defined STC3100_SENSOR_INVALID
#define STC3100_SENSOR_INVALID -9999
#endif
/**@}*/

/**
 * @anchor sensor_stc3100_volt
 * @name Bus Voltage
 * The bus voltage variable from a ST STC3100
 * - Range is 2.7 to 4.5V - 
 * - Accuracy is dependent on ADC resolution 
 *  12bits 47uV every 125mS  
 *  13bits 24uV       250mS
 *  14bits 12uV       500mS
 *  accuracy 0.5% @25C over temperature range 1%
 *
 * {{ @ref STSTC3100_Volt::STSTC3100_Volt }}
 */
/**@{*/
/// @brief Decimals places in string representation; bus voltage is 4 -
/// resolution is 0.001V.
#define STC3100_BUS_VOLTAGE_RESOLUTION 4
/// @brief Sensor variable number; bus voltage is stored in sensorValues[1].
#define STC3100_BUS_VOLTAGE_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define STC3100_BUS_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define STC3100_BUS_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "STSTC3100Volt"
#define STC3100_BUS_VOLTAGE_DEFAULT_CODE "STSTC3100Volt"
/**@}*/


/**
 * @anchor sensor_stc3100_current
 * @name Current
 * The current variable from a ST STC3100

 * - Range with 30milliOhms is between +/-7.0 Amps
 * - Absolute accuracy is range dependent, and 11.77uV measurement
 * For 0.030 Ohms is 0.353mA
 * For 0.100 Ohms is 0.117mA
 *
 * {{ @ref STSTC3100M_Current::STSTC3100M_Current }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; current is 1.
 * 
 */
#define STC3100_CURRENT_MA_RESOLUTION 1
/// @brief Sensor variable number; current is stored in sensorValues[0].
#define STC3100_CURRENT_MA_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricCurrent"
#define STC3100_CURRENT_MA_VAR_NAME "electricCurrent"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "milliamp"
#define STC3100_CURRENT_MA_UNIT_NAME "milliamp"
/// @brief Default variable short code; "STSTC3100Amp"
#define STC3100_CURRENT_MA_DEFAULT_CODE "STSTC3100Amp"
/**@}*/
/**
 * @anchor sensor_stc3100_used1_mah
 * @name used1_mAh
 * The energy variable from a ST STC3100
  * - Absolute accuracy is range dependent, and 6.70uV.h measurement
 * For 0.030 Ohms is 0.2mAh
 * For 0.100 ohms is 0.067mAh
 * 0.277mAh == 1Coulomb
 * 
 * {{ @ref STC3100M_mAh::STC3100M_mAh }}
 */
/**@{*/
/// @brief Decimals places in string representation; power draw is 2 -

#define STC3100_USED1_MAH_RESOLUTION 2
/// @brief Sensor variable number; power draw is stored in sensorValues[2].
#define STC3100_USED1_MAH_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricPower"
#define STC3100_USED1_MAH_VAR_NAME "electricEnergy"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "Coulomb"
#define STC3100_USED1_MAH_UNIT_NAME "milliAmpHour"
/// @brief Default variable short code; "STSTC3100Power"
#define STC3100_USED1_MAH_DEFAULT_CODE "STC3100usedmAhr"


/**
 * @anchor sensor_stc3100_avlbl_mah
 * @name available_mAh
 * The energy variable from a ST STC3100
  * - Absolute accuracy is range dependent, and 6.70uV.h measurement
 * For 0.030 Ohms is 0.2mAh
 * For 0.100 ohms is 0.067mAh
 * 0.277mAh == 1Coulomb
 * 
 * {{ @ref STC3100M_mAh::STC3100M_mAh }}
 */
/**@{*/
/// @brief Decimals places in string representation; power draw is 2 -

#define STC3100_AVLBL_MAH_RESOLUTION 2
/// @brief Sensor variable number; power draw is stored in sensorValues[3].
#define STC3100_AVLBL_MAH_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricPower"
#define STC3100_AVLBL_MAH_VAR_NAME "electricEnergy"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "Coulomb"
#define STC3100_AVLBL_MAH_UNIT_NAME "milliAmpHour"
/// @brief Default variable short code; "STSTC3100Power"
#define STC3100_AVLBL_MAH_DEFAULT_CODE "STC3100avblmAhr"

/**@}*/

/**
 * @brief Class for Stc3100 Device with internal sensors
 *
 * This contains the access functionality over I2C for all Stc3100 sensors.
 *
 * @ingroup stc3100_group
 */
class STSTC3100_Sensor : public Sensor {
 public:

    /**
     * @brief Construct a new Stc3100 Parent object using a secondary *hardware*
     * I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param sensorName The name of the sensor, defaults to Stc3100Sensor.
     * @param numReturnedVars The number of results returned by the sensor.
     * Defaults to 1.
     * @param warmUpTime_ms The time needed from the when a sensor has power
     * until it's ready to talk (_warmUpTime_ms).
     * @param stabilizationTime_ms The time needed from the when a sensor is
     * activated until the readings are stable (_stabilizationTime_ms).
     * @param measurementTime_ms The time needed from the when a sensor is told
     * to take a single reading until that reading is expected to be complete
     * (_measurementTime_ms)
     */
    STSTC3100_Sensor(TwoWire* theI2C, 
                uint8_t       measurementsToAverage = STC3100_NUM_MEASUREMENTS,
                const char*   sensorName            = "Stc3100Sensor",
                const uint8_t numReturnedVars = STC3100_NUM_VARIABLES, uint32_t warmUpTime_ms = STC3100_WARM_UP_TIME_MS,
                uint32_t stabilizationTime_ms = STC3100_STABILIZATION_TIME_MS,
                uint32_t measurementTime_ms   = STC3100_MEASUREMENT_TIME_MS);
    /**
     * @brief Construct a new Stc3100 Parent object using the primary hardware I2C
     * instance.
     *
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param sensorName The name of the sensor, defaults to Stc3100Sensor.
     * @param numReturnedVars The number of results returned by the sensor.
     * Defaults to 1.
     * @param warmUpTime_ms The time needed from the when a sensor has power
     * until it's ready to talk (_warmUpTime_ms).
     * @param stabilizationTime_ms The time needed from the when a sensor is
     * activated until the readings are stable (_stabilizationTime_ms).
     * @param measurementTime_ms The time needed from the when a sensor is told
     * to take a single reading until that reading is expected to be complete
     * (_measurementTime_ms)
     */
    STSTC3100_Sensor( 
                uint8_t       measurementsToAverage = STC3100_NUM_MEASUREMENTS,
                const char*   sensorName            = "Stc3100Sensor",
                const uint8_t numReturnedVars = STC3100_NUM_VARIABLES, uint32_t warmUpTime_ms = STC3100_WARM_UP_TIME_MS,
                uint32_t stabilizationTime_ms = STC3100_STABILIZATION_TIME_MS,
                uint32_t measurementTime_ms   = STC3100_MEASUREMENT_TIME_MS);

    /**
     * @brief Destroy the Stc3100 Parent object.  Also destroy the software I2C
     * instance if one was created.
     */
    virtual ~STSTC3100_Sensor();

    /**
     * @brief Return the I2C address of the sensor.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This  begins the Wire library (sets pin levels
     * and modes for I2C), and updates the #_sensorStatus.  No sensor power is
     * required.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;

    /**
     * @brief tbd can't put sensor to sleep, can do low power mode.  
     *
     * This also un-sets the #_millisSensorActivated timestamp (sets it to 0).
     * This does NOT power down the sensor!
     *
     * @return **bool** True if the sleep function completed successfully.
     */
    //tbd bool sleep(void) override;

    /**
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the #_millisMeasurementRequested timestamp.
     *
     * @note This function does NOT include any waiting for the sensor to be
     * warmed up or stable!
     *
     * @return **bool** True if the start measurement function completed
     * successfully.
     */
    //bool startSingleMeasurement(void) override;
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

    /**
     * @brief Get the stored sensor serial number 
     *
     * @return String The sensor serial number
     */
    String getSensorDetails(void) ;

    /**
     * @brief  setI2cAddress- call at beginning if need to change 
     * There only appears to be one IC that is commercially available,
     * but a number of IC addresses are defined in the manual
     * 
     */
    #if defined STC3100_USE_ADDR
        void setI2cAddress(int8_t i2cAddressHex=STC3100_BUS_ADDRESS) {_i2cAddressHex=i2cAddressHex;}
    #endif // STC3100_USE_ADDR

    STC3100dm stc3100_device;

 protected:
    /**
     * @brief The I2C address of the Stc3100 circuit.
     */
    #if !defined STC3100_USE_ADDR
    const 
    #endif //STC3100_USE_ADDR
    int8_t _i2cAddressHex=STC3100_BUS_ADDRESS;

    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;  // Hardware Wire

    /**
     * @brief Wait for a command to process
     *
     * The Stc3100 I2C commands reads register with data 
     *  have a 300ms processing time from the time the
     * command is written until it is possible to request a response or result,
     * except for the commands to take a calibration point or a reading which
     * have a 600ms processing/response time.
     *
     * @note This should ONLY be used as a wait when no response is expected
     * except a status code - the response will be "consumed" and become
     * unavailable.
     *
     * @param timeout The maximum amout of time to wait in ms.
     * @return **bool** True processing completed and a status code was returned
     * within the wait period.
     */
    // bool waitForProcessing(uint32_t timeout = 1000L);

};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [bus voltage output](@ref stc3100_volt) from a [ST STC3100 battery monitor](@ref stc3100_group).
 *
 * @ingroup stc3100_group
 */
/* clang-format on */
class STSTC3100_Volt : public Variable {
 public:
    /**
     * @brief Construct a new STSTC3100_Volt object.
     *
     * @param parentSense The parent STSTC3100 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "STSTC3100Volt".
     */
    //explicit STSTC3100_Volt(
    STSTC3100_Volt(
        STSTC3100_Sensor* parentSense, const char* uuid = "",
        const char* varCode = STC3100_BUS_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)STC3100_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)STC3100_BUS_VOLTAGE_RESOLUTION,
                   STC3100_BUS_VOLTAGE_VAR_NAME, STC3100_BUS_VOLTAGE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new STSTC3100_Volt object.
     *
     * @note This must be tied with a parent STSTC3100 before it can be used.
     */
    STSTC3100_Volt()
        : Variable((const uint8_t)STC3100_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)STC3100_BUS_VOLTAGE_RESOLUTION,
                   STC3100_BUS_VOLTAGE_VAR_NAME, STC3100_BUS_VOLTAGE_UNIT_NAME,
                   STC3100_BUS_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the STSTC3100_Volt object - no action needed.
     */
    ~STSTC3100_Volt() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [current output](@ref sensor_stc3100_current) from a [ST STC3100 battery monitor](@ref sensor_stc3100).
 *
 * @ingroup sensor_stc3100
 */
/* clang-format on */
class STSTC3100_Current : public Variable {
 public:
    /**
     * @brief Construct a new STSTC3100_Current object.
     *
     * @param parentSense The parent STSTC3100 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "STSTC3100Amp".
     */
    explicit STSTC3100_Current(
        STSTC3100_Sensor* parentSense, const char* uuid = "",
        const char* varCode = STC3100_CURRENT_MA_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)STC3100_CURRENT_MA_VAR_NUM,
                   (uint8_t)STC3100_CURRENT_MA_RESOLUTION,
                   STC3100_CURRENT_MA_VAR_NAME, STC3100_CURRENT_MA_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new STSTC3100_Current object.
     *
     * @note This must be tied with a parent STSTC3100 before it can be used.
     */
    STSTC3100_Current()
        : Variable((const uint8_t)STC3100_CURRENT_MA_VAR_NUM,
                   (uint8_t)STC3100_CURRENT_MA_RESOLUTION,
                   STC3100_CURRENT_MA_VAR_NAME, STC3100_CURRENT_MA_UNIT_NAME,
                   STC3100_CURRENT_MA_DEFAULT_CODE) {}
    /**
     * @brief Destroy the STSTC3100_Current object - no action needed.
     */
    ~STSTC3100_Current() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [energy use output](@ref sensor_stc3100_energy) calculated from the voltage
 * and current measured by a [ST STC3100 battery monitor](@ref sensor_stc3100).
 *
 * @ingroup sensor_stc3100
 */
/* clang-format on */
class STC3100_USED1_MAH : public Variable {
 public:
    /**
     * @brief Construct a new STC3100_USED1_MAH object.
     *
     * @param parentSense The parent STSTC3100 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "STSTC3100Energy".
     */
    explicit STC3100_USED1_MAH(STSTC3100_Sensor* parentSense, const char* uuid = "",
                             const char* varCode = STC3100_USED1_MAH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)STC3100_USED1_MAH_VAR_NUM,
                   (uint8_t)STC3100_USED1_MAH_RESOLUTION,
                   STC3100_USED1_MAH_VAR_NAME, STC3100_USED1_MAH_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new STC3100_USED1_MAH object.
     *
     * @note This must be tied with a parent STSTC3100 before it can be used.
     */
    STC3100_USED1_MAH()
        : Variable((const uint8_t)STC3100_USED1_MAH_VAR_NUM,
                   (uint8_t)STC3100_USED1_MAH_RESOLUTION,
                   STC3100_USED1_MAH_VAR_NAME, STC3100_USED1_MAH_UNIT_NAME,
                   STC3100_USED1_MAH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the STC3100_USED1_MAH object - no action needed.
     */
    ~STC3100_USED1_MAH() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [energy use output](@ref sensor_stc3100_energy) calculated from the voltage
 * and current measured by a [ST STC3100 battery monitor](@ref sensor_stc3100).
 *
 * @ingroup sensor_stc3100
 */
/* clang-format on */
class STC3100_AVLBL_MAH : public Variable {
 public:
    /**
     * @brief Construct a new STC3100_AVLBL_MAH object.
     *
     * @param parentSense The parent STSTC3100 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "STSTC3100Energy".
     */
    explicit STC3100_AVLBL_MAH(STSTC3100_Sensor* parentSense, const char* uuid = "",
                             const char* varCode = STC3100_AVLBL_MAH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)STC3100_AVLBL_MAH_VAR_NUM,
                   (uint8_t)STC3100_AVLBL_MAH_RESOLUTION,
                   STC3100_AVLBL_MAH_VAR_NAME, STC3100_AVLBL_MAH_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new STC3100_AVLBL_MAH object.
     *
     * @note This must be tied with a parent STSTC3100 before it can be used.
     */
    STC3100_AVLBL_MAH()
        : Variable((const uint8_t)STC3100_AVLBL_MAH_VAR_NUM,
                   (uint8_t)STC3100_AVLBL_MAH_RESOLUTION,
                   STC3100_AVLBL_MAH_VAR_NAME, STC3100_AVLBL_MAH_UNIT_NAME,
                   STC3100_AVLBL_MAH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the STC3100_AVLBL_MAH object - no action needed.
     */
    ~STC3100_AVLBL_MAH() {}
};

/**@}*/
#endif  // SRC_SENSORS_STSTC3100SENSOR_H_
