/**
 * @file CampbellRainVUE10.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the CampbellRainVUE10 sensor subclass and the variable
 * subclasses CampbellRainVUE10_Temp, CampbellRainVUE10_Turbidity, and
 * CampbellRainVUE10_ErrorCode.
 *
 * These are for the Campbell RainVUE10 digital SDI-12 Precipitation sensor.
 *
 * This depends on the SDI12Sensors parent class.
 */
/* clang-format off */
/**
 * @defgroup sensor_rainvue Campbell RainVUE10
 * Classes for the [Campbell RainVUE10](https://www.campbellsci.com/rainvue10) turbidity sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_rainvue_intro Introduction
 *
 * > The RainVUETM10 is an SDI-12 tipping bucket rain gage in the RainVUE™ 
 * > family of products. Advanced algorithms and digital processing within the
 * > sensor compensate for errors caused by high-intensity rain and provide
 * > accurate precipitation and intensity measurements.
 *
 * The sensor is implemented as a sub-classes of the SDI12Sensors class.
 * It requires a continuous 6 to 18 Vdc power supply. As backup, ann internal
 * 240 mAh lithium battery (3V Coin Cell CR2032) provides up to 15 days of 
 * continual operation after power loss.
 * It draws < 80 µA when inactive and 1 mA while measuring.
 *
 * @note You should not average measurements for this sensor.
 * The sensor takes continuous readings and returns the cumulative value over
 * each logging interval.
 *
 * @section sensor_rainvue_datasheet Sensor Datasheet
 * The specifications and datasheet are available at https://www.campbellsci.com/rainvue10
 *
 * @section sensor_rainvue_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_rainvue_ctor Sensor Constructor
 * {{ @ref CampbellRainVUE10::CampbellRainVUE10 }}
 *
 * ___
 * @section sensor_rainvue_examples Example Code
 * The Campbell RainVUE10 is used in the @menulink{campbell_rain_vue10} example.
 *
 * @menusnip{campbell_rain_vue10}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_CAMPBELLRAINVUE10_H_
#define SRC_SENSORS_CAMPBELLRAINVUE10_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_rainvue */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the RainVUE10 can report 7 values
/// (although we only keep 3).
#define RAINVUE10_NUM_VARIABLES 7
/// @brief Sensor::_incCalcValues; The RainVUE calcualtes averages and other
/// stats on board, but we don't calculate any additional values.
#define RAINVUE10_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_rainvue_timing
 * @name Sensor Timing
 * The sensor timing for a Campbell RainVUE10
 */
/**@{*/
/** @brief Sensor::_warmUpTime_ms; the RainVUE10 warms up in 5500ms.
 *
 * This is longer than the expected 250ms for a SDI-12 sensor, but I couldn't
 * get a response from the sensor faster than that.
 */
#define RAINVUE10_WARM_UP_TIME_MS 5500
/// @brief Sensor::_stabilizationTime_ms; the RainVUE10 is stable as soon as it
/// warms up (0ms stabilization).
#define RAINVUE10_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the RainVUE10 takes ~9500ms to
 * complete a measurement.
 *
 * Spec sheet says the measurement time is 9s.  When taking a standard
 * measurement I was not getting a result until after about 9335ms.
 */
#define RAINVUE10_MEASUREMENT_TIME_MS 9500
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The RainVUE requires no extra time.
#define RAINIVUE10_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_rainvue_turbidity
 * @name Turbidity
 * The turbidity variable from a Campbell RainVUE10
 * - Range is 0 to 4000 FNU.
 * - Accuracy ±2% or 0.5 FNU (whichever is greater).
 *
 * {{ @ref CampbellRainVUE10_Turbidity::CampbellRainVUE10_Turbidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; turbidity should have 1
/// (resolution is 0.2 FNU).
#define RAINVUE10_TURBIDITY_RESOLUTION 1
/// @brief Sensor variable number; turbidity is stored in sensorValues[0]
#define RAINVUE10_TURBIDITY_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define RAINVUE10_TURBIDITY_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "formazinNephelometricUnit" (FNU)
#define RAINVUE10_TURBIDITY_UNIT_NAME "formazinNephelometricUnit"
/// @brief Default variable short code; "RainVUETurbidity"
#define RAINVUE10_TURBIDITY_DEFAULT_CODE "RainVUETurbidity"
/**@}*/

/**
 * @anchor sensor_rainvue_temp
 * @name Temperature
 * The temperature variable from a Campbell RainVUE10
 * - Range is -2° to +40°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref CampbellRainVUE10_Temp::CampbellRainVUE10_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define RAINVUE10_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[5].
#define RAINVUE10_TEMP_VAR_NUM 5
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define RAINVUE10_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define RAINVUE10_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "RainVUETempC"
#define RAINVUE10_TEMP_DEFAULT_CODE "RainVUETempC"
/**@}*/

/**
 * @anchor sensor_rainvue_error
 * @name Error Code
 * The error code variable from a Campbell RainVUE10
 * - Significance of error code values is unknown.
 *
 * {{ @ref CampbellRainVUE10_ErrorCode::CampbellRainVUE10_ErrorCode }}
 */
/**@{*/
/// @brief Decimals places in string representation; the error code has 0.
#define RAINVUE10_ERRORCODE_RESOLUTION 0
/// @brief Sensor variable number; error code is stored in sensorValues[2]
#define RAINVUE10_ERRORCODE_VAR_NUM 6
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define RAINVUE10_ERRORCODE_VAR_NAME "instrumentStatusCode"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define RAINVUE10_ERRORCODE_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "RainVUEError"
#define RAINVUE10_ERRORCODE_DEFAULT_CODE "RainVUEError"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Campbell RainVUE10 turbidity sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */
/* clang-format on */
class CampbellRainVUE10 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Campbell RainVUE10 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required
     * for the sensor constructor.  Optionally, you can include a number of
     * distinct readings to average.  The data pin must be a pin that
     * supports pin-change interrupts.
     *
     * @param SDI12address The SDI-12 address of the RainVUE10; can be a
     * char, char*, or int.
     * @param powerPin The pin on the mcu controlling power to the
     * RainVUE10 Use -1 if it is continuously powered.
     * - The RainVUE10 requires a 9.6 to 18 Vdc power supply, which can be
     * turned off between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with
     * a default value of 1.
     */
    CampbellRainVUE10(char SDI12address, int8_t powerPin, int8_t dataPin,
                       uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "Campbell RainVUE10", RAINVUE10_NUM_VARIABLES,
              RAINVUE10_WARM_UP_TIME_MS, RAINVUE10_STABILIZATION_TIME_MS,
              RAINVUE10_MEASUREMENT_TIME_MS, RAINVUE10_EXTRA_WAKE_TIME_MS,
              RAINVUE10_INC_CALC_VARIABLES) {}
    /**
     * @copydoc CampbellRainVUE10::CampbellRainVUE10
     */
    CampbellRainVUE10(char* SDI12address, int8_t powerPin, int8_t dataPin,
                       uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "Campbell RainVUE10", RAINVUE10_NUM_VARIABLES,
              RAINVUE10_WARM_UP_TIME_MS, RAINVUE10_STABILIZATION_TIME_MS,
              RAINVUE10_MEASUREMENT_TIME_MS, RAINVUE10_EXTRA_WAKE_TIME_MS,
              RAINVUE10_INC_CALC_VARIABLES) {}
    /**
     * @copydoc CampbellRainVUE10::CampbellRainVUE10
     */
    CampbellRainVUE10(int SDI12address, int8_t powerPin, int8_t dataPin,
                       uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "Campbell RainVUE10", RAINVUE10_NUM_VARIABLES,
              RAINVUE10_WARM_UP_TIME_MS, RAINVUE10_STABILIZATION_TIME_MS,
              RAINVUE10_MEASUREMENT_TIME_MS, RAINVUE10_EXTRA_WAKE_TIME_MS,
              RAINVUE10_INC_CALC_VARIABLES) {}

    /**
     * @brief Destroy the Campbell RainVUE10 object
     */
    ~CampbellRainVUE10() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_rainvue_turbidity)
 * from a [Campbell RainVUE10 turbidity sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */
/* clang-format on */
class CampbellRainVUE10_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new CampbellRainVUE10_Turbidity object.
     *
     * @param parentSense The parent CampbellRainVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainVUETurbidity".
     */
    explicit CampbellRainVUE10_Turbidity(
        CampbellRainVUE10* parentSense, const char* uuid = "",
        const char* varCode = RAINVUE10_TURBIDITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)RAINVUE10_TURBIDITY_VAR_NUM,
                   (uint8_t)RAINVUE10_TURBIDITY_RESOLUTION,
                   RAINVUE10_TURBIDITY_VAR_NAME,
                   RAINVUE10_TURBIDITY_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellRainVUE10_Turbidity object.
     *
     * @note This must be tied with a parent CampbellRainVUE10 before it can be
     * used.
     */
    CampbellRainVUE10_Turbidity()
        : Variable((const uint8_t)RAINVUE10_TURBIDITY_VAR_NUM,
                   (uint8_t)RAINVUE10_TURBIDITY_RESOLUTION,
                   RAINVUE10_TURBIDITY_VAR_NAME,
                   RAINVUE10_TURBIDITY_UNIT_NAME,
                   RAINVUE10_TURBIDITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellRainVUE10_Turbidity object - no action
     * needed.
     */
    ~CampbellRainVUE10_Turbidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_rainvue_temp) from a
 * [Campbell RainVUE10 turbidity sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */
/* clang-format on */
class CampbellRainVUE10_Temp : public Variable {
 public:
    /**
     * @brief Construct a new CampbellRainVUE10_Temp object.
     *
     * @param parentSense The parent CampbellRainVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainVUETempC".
     */
    explicit CampbellRainVUE10_Temp(
        CampbellRainVUE10* parentSense, const char* uuid = "",
        const char* varCode = RAINVUE10_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)RAINVUE10_TEMP_VAR_NUM,
                   (uint8_t)RAINVUE10_TEMP_RESOLUTION,
                   RAINVUE10_TEMP_VAR_NAME, RAINVUE10_TEMP_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new CampbellRainVUE10_Temp object.
     *
     * @note This must be tied with a parent CampbellRainVUE10 before it can be
     * used.
     */
    CampbellRainVUE10_Temp()
        : Variable((const uint8_t)RAINVUE10_TEMP_VAR_NUM,
                   (uint8_t)RAINVUE10_TEMP_RESOLUTION,
                   RAINVUE10_TEMP_VAR_NAME, RAINVUE10_TEMP_UNIT_NAME,
                   RAINVUE10_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellRainVUE10_Temp object - no action needed.
     */
    ~CampbellRainVUE10_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [error code output](@ref sensor_rainvue_error) from a
 * [Campbell RainVUE10 turbidity sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */
/* clang-format on */
class CampbellRainVUE10_ErrorCode : public Variable {
 public:
    /**
     * @brief Construct a new CampbellRainVUE10_ErrorCode object.
     *
     * @param parentSense The parent CampbellRainVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainVUEError".
     */
    explicit CampbellRainVUE10_ErrorCode(
        CampbellRainVUE10* parentSense, const char* uuid = "",
        const char* varCode = RAINVUE10_ERRORCODE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)RAINVUE10_ERRORCODE_VAR_NUM,
                   (uint8_t)RAINVUE10_ERRORCODE_RESOLUTION,
                   RAINVUE10_ERRORCODE_VAR_NAME,
                   RAINVUE10_ERRORCODE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellRainVUE10_ErrorCode object.
     *
     * @note This must be tied with a parent CampbellRainVUE10 before it can be
     * used.
     */
    CampbellRainVUE10_ErrorCode()
        : Variable((const uint8_t)RAINVUE10_ERRORCODE_VAR_NUM,
                   (uint8_t)RAINVUE10_ERRORCODE_RESOLUTION,
                   RAINVUE10_ERRORCODE_VAR_NAME,
                   RAINVUE10_ERRORCODE_UNIT_NAME,
                   RAINVUE10_ERRORCODE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellRainVUE10_ErrorCode object - no action
     * needed.
     */
    ~CampbellRainVUE10_ErrorCode() {}
};
/**@}*/
#endif  // SRC_SENSORS_CAMPBELLRAINVUE10_H_
