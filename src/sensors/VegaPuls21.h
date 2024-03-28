/**
 * @file VegaPuls21.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the VegaPuls21 sensor subclass and the variable
 * subclasses VegaPuls21_Stage, VegaPuls21_Distance, VegaPuls21_Reliability, and
 * VegaPuls21_ErrorCode.
 *
 * These are for the VEGAPULS C 21 digital SDI-12 radar level sensor.
 *
 * This depends on the SDI12Sensors parent class.
 */
/* clang-format off */
/**
 * @defgroup sensor_vega_puls21 VEGAPULS C 21
 * Classes for the [VEGAPULS C 21](https://www.vega.com/en-us/products/product-catalog/level/radar/vegapuls-c-21) radar sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_vega_puls21_intro Introduction
 *
 * > VEGAPULS C 21 is the ideal sensor for non-contact level measurement in simple applications
 * > where a high degree of protection is required. It is particularly suitable for use in water treatment,
 * > pumping stations and rain overflow basins, for flow measurement in open channels and level monitoring.
 * > In bulk solids the sensors are used in small bulk solids silos or open containers.
 *
 * The sensor is implemented as a sub-classes of the SDI12Sensors class.
 * It requires a 8 to 30 Vdc power supply, which can be turned off between measurements.
 * It pulls 25 mW in low-power mode and 100 mW in standard mode.
 *
 * @section sensor_vega_puls21_datasheet Sensor Datasheet
 * The specifications and datasheet are available at https://www.vega.com/api/sitecore/DocumentDownload/Handler?documentContainerId=1006748&languageId=2&fileExtension=pdf&softwareVersion=&documentGroupId=58354&version=03-04-2023
 *
 * @section sensor_vega_puls21_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_vega_puls21_ctor Sensor Constructor
 * {{ @ref VegaPuls21::VegaPuls21 }}
 *
 * ___
 * @section sensor_vega_puls21_examples Example Code
 * The VEGAPULS C 21 is used in the @menulink{vega_puls21} example.
 *
 * @menusnip{vega_puls21}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_VEGAPULS21_H_
#define SRC_SENSORS_VEGAPULS21_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_vega_puls21 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the VEGA PULS 21 can report 5 values
#define VEGAPULS21_NUM_VARIABLES 5
/// @brief Sensor::_incCalcValues;
#define VEGAPULS21_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_vega_puls21_timing
 * @name Sensor Timing
 * The sensor timing for a VEGAPULS C 21
 */
/**@{*/
/** @brief Sensor::_warmUpTime_ms; the VEGA PULS 21 warms up in ~4400ms.
 *
 * This is longer than the expected 250ms for a SDI-12 sensor, but I couldn't
 * get a response from the sensor faster than that.  The instruction sheet says
 * the warm-up is less than 10s.
 */
#define VEGAPULS21_WARM_UP_TIME_MS 4500
/// @brief Sensor::_stabilizationTime_ms; the VEGA PULS 21 is stable as soon as
/// it warms up (0ms stabilization).
#define VEGAPULS21_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the VEGA PULS 21 takes ~6000ms to
 * complete a measurement.
 *
 * Spec sheet says the measurement time is 250ms but when you ask the sensor it
 * says it won't return for 14s.  When taking a standard measurement I was
 * getting a result after about 5800ms.
 */
#define VEGAPULS21_MEASUREMENT_TIME_MS 6000
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The VEGA PULS 21 requires no extra time.
#define VEGAPULS21_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_vega_puls21_stage
 * @name Stage
 * The stage variable from a VEGAPULS C 21
 * - Accuracy is ≤ 2 mm (meas. distance > 0.25 m/0.8202 ft)
 *
 * {{ @ref VegaPuls21_Stage::VegaPuls21_Stage }}
 */
/**@{*/
/// @brief Decimals places in string representation; stage in meters should have
/// 3 - resolution is 1mm.
#define VEGAPULS21_STAGE_RESOLUTION 3
/// @brief Sensor variable number; stage is stored in sensorValues[0].
#define VEGAPULS21_STAGE_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "gageHeight"
#define VEGAPULS21_STAGE_VAR_NAME "gageHeight"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "meter" (m)
#define VEGAPULS21_STAGE_UNIT_NAME "meter"
/// @brief Default variable short code; "VegaPulsStage"
#define VEGAPULS21_STAGE_DEFAULT_CODE "VegaPulsStage"
/**@}*/

/**
 * @anchor sensor_vega_puls21_distance
 * @name Distance
 * The distance variable from a VEGAPULS C 21
 * - Accuracy is ≤ 2 mm (meas. distance > 0.25 m/0.8202 ft)
 *
 * {{ @ref VegaPuls21_Distance::VegaPuls21_Distance }}
 */
/**@{*/
/// @brief Decimals places in string representation; distance in meters should
/// have 3 - resolution is 1mm.
#define VEGAPULS21_DISTANCE_RESOLUTION 3
/// @brief Sensor variable number; stage is stored in sensorValues[1].
#define VEGAPULS21_DISTANCE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "distance"
#define VEGAPULS21_DISTANCE_VAR_NAME "distance"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "meter" (m)
#define VEGAPULS21_DISTANCE_UNIT_NAME "meter"
/// @brief Default variable short code; "VegaPulsDistance"
#define VEGAPULS21_DISTANCE_DEFAULT_CODE "VegaPulsDistance"
/**@}*/

/**
 * @anchor sensor_vega_puls21_temp
 * @name Temperature
 * The temperature variable from a VEGAPULS C 21
 *
 * {{ @ref VegaPuls21_Temp::VegaPuls21_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define VEGAPULS21_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[2].
#define VEGAPULS21_TEMP_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define VEGAPULS21_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define VEGAPULS21_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "VegaPulsTemp"
#define VEGAPULS21_TEMP_DEFAULT_CODE "VegaPulsTemp"
/**@}*/

/**
 * @anchor sensor_vega_puls21_reliability
 * @name Reliability
 * The reliability variable from a VEGAPULS C 21
 *
 * {{ @ref VegaPuls21_Reliability::VegaPuls21_Reliability }}
 */
/**@{*/
/// @brief Decimals places in string representation; reliability should have 1
/// (resolution is 0.1 dB).
#define VEGAPULS21_RELIABILITY_RESOLUTION 1
/// @brief Sensor variable number; reliability is stored in sensorValues[3]
#define VEGAPULS21_RELIABILITY_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "reliability"
#define VEGAPULS21_RELIABILITY_VAR_NAME "reliability"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "decibel" (dB)
#define VEGAPULS21_RELIABILITY_UNIT_NAME "decibel"
/// @brief Default variable short code; "VegaPulsReliability"
#define VEGAPULS21_RELIABILITY_DEFAULT_CODE "VegaPulsReliability"
/**@}*/

/**
 * @anchor sensor_vega_puls21_error
 * @name Error Code
 * The error code variable from a VEGAPULS C 21
 * - Significance of error code values is unknown.
 *
 * {{ @ref VegaPuls21_ErrorCode::VegaPuls21_ErrorCode }}
 */
/**@{*/
/// @brief Decimals places in string representation; the error code has 0.
#define VEGAPULS21_ERRORCODE_RESOLUTION 0
/// @brief Sensor variable number; error code is stored in sensorValues[4]
#define VEGAPULS21_ERRORCODE_VAR_NUM 4
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define VEGAPULS21_ERRORCODE_VAR_NAME "instrumentStatusCode"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define VEGAPULS21_ERRORCODE_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "VegaPulsError"
#define VEGAPULS21_ERRORCODE_DEFAULT_CODE "VegaPulsError"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [VEGAPULS C 21 radar level sensor](@ref sensor_vega_puls21).
 *
 * @ingroup sensor_vega_puls21
 */
/* clang-format on */
class VegaPuls21 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new VEGAPULS C 21 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required
     * for the sensor constructor.  Optionally, you can include a number of
     * distinct readings to average.  The data pin must be a pin that
     * supports pin-change interrupts.
     *
     * @param SDI12address The SDI-12 address of the VEGA PULS 21; can be a
     * char, char*, or int.
     * @param powerPin The pin on the mcu controlling power to the
     * VEGA PULS 21 Use -1 if it is continuously powered.
     * - The VEGA PULS 21 requires a 8 to 30 Vdc power supply, which can be
     * turned off between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with
     * a default value of 1.
     */
    VegaPuls21(char SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "VEGAPULS C 21", VEGAPULS21_NUM_VARIABLES,
              VEGAPULS21_WARM_UP_TIME_MS, VEGAPULS21_STABILIZATION_TIME_MS,
              VEGAPULS21_MEASUREMENT_TIME_MS, VEGAPULS21_EXTRA_WAKE_TIME_MS,
              VEGAPULS21_INC_CALC_VARIABLES) {}
    /**
     * @copydoc VegaPuls21::VegaPuls21
     */
    VegaPuls21(char* SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "VEGAPULS C 21", VEGAPULS21_NUM_VARIABLES,
              VEGAPULS21_WARM_UP_TIME_MS, VEGAPULS21_STABILIZATION_TIME_MS,
              VEGAPULS21_MEASUREMENT_TIME_MS, VEGAPULS21_EXTRA_WAKE_TIME_MS,
              VEGAPULS21_INC_CALC_VARIABLES) {}
    /**
     * @copydoc VegaPuls21::VegaPuls21
     */
    VegaPuls21(int SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "VEGAPULS C 21", VEGAPULS21_NUM_VARIABLES,
              VEGAPULS21_WARM_UP_TIME_MS, VEGAPULS21_STABILIZATION_TIME_MS,
              VEGAPULS21_MEASUREMENT_TIME_MS, VEGAPULS21_EXTRA_WAKE_TIME_MS,
              VEGAPULS21_INC_CALC_VARIABLES) {}

    /**
     * @brief Destroy the VEGAPULS C 21 object
     */
    ~VegaPuls21() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_vega_puls21_stage) from a
 * [VEGAPULS C 21 radar level sensor](@ref sensor_vega_puls21).
 *
 * @ingroup sensor_vega_puls21
 */
/* clang-format on */
class VegaPuls21_Stage : public Variable {
 public:
    /**
     * @brief Construct a new VegaPuls21_Stage object.
     *
     * @param parentSense The parent VegaPuls21 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "VegaPulsStage".
     */
    explicit VegaPuls21_Stage(
        VegaPuls21* parentSense, const char* uuid = "",
        const char* varCode = VEGAPULS21_STAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)VEGAPULS21_STAGE_VAR_NUM,
                   (uint8_t)VEGAPULS21_STAGE_RESOLUTION,
                   VEGAPULS21_STAGE_VAR_NAME, VEGAPULS21_STAGE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new VegaPuls21_Stage object.
     *
     * @note This must be tied with a parent VegaPuls21 before it can be
     * used.
     */
    VegaPuls21_Stage()
        : Variable((const uint8_t)VEGAPULS21_STAGE_VAR_NUM,
                   (uint8_t)VEGAPULS21_STAGE_RESOLUTION,
                   VEGAPULS21_STAGE_VAR_NAME, VEGAPULS21_STAGE_UNIT_NAME,
                   VEGAPULS21_STAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the VegaPuls21_Stage object - no action needed.
     */
    ~VegaPuls21_Stage() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_vega_puls21_distance) from a
 * [VEGAPULS C 21 radar level sensor](@ref sensor_vega_puls21).
 *
 * @ingroup sensor_vega_puls21
 */
/* clang-format on */
class VegaPuls21_Distance : public Variable {
 public:
    /**
     * @brief Construct a new VegaPuls21_Distance object.
     *
     * @param parentSense The parent VegaPuls21 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "VegaPulsDistance".
     */
    explicit VegaPuls21_Distance(
        VegaPuls21* parentSense, const char* uuid = "",
        const char* varCode = VEGAPULS21_DISTANCE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)VEGAPULS21_DISTANCE_VAR_NUM,
                   (uint8_t)VEGAPULS21_DISTANCE_RESOLUTION,
                   VEGAPULS21_DISTANCE_VAR_NAME, VEGAPULS21_DISTANCE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new VegaPuls21_Distance object.
     *
     * @note This must be tied with a parent VegaPuls21 before it can be
     * used.
     */
    VegaPuls21_Distance()
        : Variable((const uint8_t)VEGAPULS21_DISTANCE_VAR_NUM,
                   (uint8_t)VEGAPULS21_DISTANCE_RESOLUTION,
                   VEGAPULS21_DISTANCE_VAR_NAME, VEGAPULS21_DISTANCE_UNIT_NAME,
                   VEGAPULS21_DISTANCE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the VegaPuls21_Distance object - no action needed.
     */
    ~VegaPuls21_Distance() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_vega_puls21_temp) from a
 * [VEGAPULS C 21 radar level sensor](@ref sensor_vega_puls21).
 *
 * @ingroup sensor_vega_puls21
 */
/* clang-format on */
class VegaPuls21_Temp : public Variable {
 public:
    /**
     * @brief Construct a new VegaPuls21_Temp object.
     *
     * @param parentSense The parent VegaPuls21 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "VegaPulsTemp".
     */
    explicit VegaPuls21_Temp(VegaPuls21* parentSense, const char* uuid = "",
                             const char* varCode = VEGAPULS21_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)VEGAPULS21_TEMP_VAR_NUM,
                   (uint8_t)VEGAPULS21_TEMP_RESOLUTION,
                   VEGAPULS21_TEMP_VAR_NAME, VEGAPULS21_TEMP_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new VegaPuls21_Temp object.
     *
     * @note This must be tied with a parent VegaPuls21 before it can be
     * used.
     */
    VegaPuls21_Temp()
        : Variable((const uint8_t)VEGAPULS21_TEMP_VAR_NUM,
                   (uint8_t)VEGAPULS21_TEMP_RESOLUTION,
                   VEGAPULS21_TEMP_VAR_NAME, VEGAPULS21_TEMP_UNIT_NAME,
                   VEGAPULS21_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the VegaPuls21_Temp object - no action needed.
     */
    ~VegaPuls21_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [reliability output](@ref sensor_vega_puls21_reliability)
 * from a [VEGAPULS C 21 radar level sensor](@ref sensor_vega_puls21).
 *
 * @ingroup sensor_vega_puls21
 */
/* clang-format on */
class VegaPuls21_Reliability : public Variable {
 public:
    /**
     * @brief Construct a new VegaPuls21_Reliability object.
     *
     * @param parentSense The parent VegaPuls21 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "VegaPulsReliability".
     */
    explicit VegaPuls21_Reliability(
        VegaPuls21* parentSense, const char* uuid = "",
        const char* varCode = VEGAPULS21_RELIABILITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)VEGAPULS21_RELIABILITY_VAR_NUM,
                   (uint8_t)VEGAPULS21_RELIABILITY_RESOLUTION,
                   VEGAPULS21_RELIABILITY_VAR_NAME,
                   VEGAPULS21_RELIABILITY_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new VegaPuls21_Reliability object.
     *
     * @note This must be tied with a parent VegaPuls21 before it can be
     * used.
     */
    VegaPuls21_Reliability()
        : Variable((const uint8_t)VEGAPULS21_RELIABILITY_VAR_NUM,
                   (uint8_t)VEGAPULS21_RELIABILITY_RESOLUTION,
                   VEGAPULS21_RELIABILITY_VAR_NAME,
                   VEGAPULS21_RELIABILITY_UNIT_NAME,
                   VEGAPULS21_RELIABILITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the VegaPuls21_Reliability object - no action
     * needed.
     */
    ~VegaPuls21_Reliability() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [error code output](@ref sensor_vega_puls21_error) from a
 * [VEGAPULS C 21 radar level sensor](@ref sensor_vega_puls21).
 *
 * @ingroup sensor_vega_puls21
 */
/* clang-format on */
class VegaPuls21_ErrorCode : public Variable {
 public:
    /**
     * @brief Construct a new VegaPuls21_ErrorCode object.
     *
     * @param parentSense The parent VegaPuls21 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "VegaPulsError".
     */
    explicit VegaPuls21_ErrorCode(
        VegaPuls21* parentSense, const char* uuid = "",
        const char* varCode = VEGAPULS21_ERRORCODE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)VEGAPULS21_ERRORCODE_VAR_NUM,
                   (uint8_t)VEGAPULS21_ERRORCODE_RESOLUTION,
                   VEGAPULS21_ERRORCODE_VAR_NAME,
                   VEGAPULS21_ERRORCODE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new VegaPuls21_ErrorCode object.
     *
     * @note This must be tied with a parent VegaPuls21 before it can be
     * used.
     */
    VegaPuls21_ErrorCode()
        : Variable((const uint8_t)VEGAPULS21_ERRORCODE_VAR_NUM,
                   (uint8_t)VEGAPULS21_ERRORCODE_RESOLUTION,
                   VEGAPULS21_ERRORCODE_VAR_NAME,
                   VEGAPULS21_ERRORCODE_UNIT_NAME,
                   VEGAPULS21_ERRORCODE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the VegaPuls21_ErrorCode object - no action
     * needed.
     */
    ~VegaPuls21_ErrorCode() {}
};
/**@}*/
#endif  // SRC_SENSORS_VEGAPULS21_H_
