/**
 * @file CampbellRainVUE10.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the CampbellRainVUE10 sensor subclass and the variable
 * subclasses CampbellRainVUE10_Precipitation, CampbellRainVUE10_Tips,
 * CampbellRainVUE10_RainRateAve, and CampbellRainVUE10_RainRateMax.
 *
 * These are for the Campbell RainVUE10 digital SDI-12 Precipitation sensor.
 *
 * This depends on the SDI12Sensors parent class.
 */
/* clang-format off */
/**
 * @defgroup sensor_rainvue Campbell RainVUE10
 * Classes for the [Campbell RainVUE10](https://www.campbellsci.com/rainvue10) precipitation sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_rainvue_intro Introduction
 *
 * The RainVUETM10 is an SDI-12 tipping bucket rain gage in the RainVUE™
 * family of products. Advanced algorithms and digital processing within the
 * sensor compensate for errors caused by high-intensity rain and provide
 * accurate precipitation and intensity measurements.
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
/// @brief Sensor::_numReturnedValues; the RainVUE10 can report 5 values
/// (although we only keep 4).
#define RAINVUE10_NUM_VARIABLES 5
/// @brief Sensor::_incCalcValues; The RainVUE calculates averages and other
/// stats on board, but we don't calculate any additional values.
#define RAINVUE10_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_rainvue_timing
 * @name Sensor Timing
 * The sensor timing for a Campbell RainVUE10
 * - Readings transferred from the tipping bucket to the logger are from past
 * tips, so there is no need to wait for stability or measuring.

 */
/**@{*/
/** @brief Sensor::_warmUpTime_ms; the RainVUE10 warms up in >100ms.
 */
#define RAINVUE10_WARM_UP_TIME_MS 100
/// @brief Sensor::_stabilizationTime_ms; the RainVUE10 is stable as soon as it
/// warms up (0ms stabilization).
#define RAINVUE10_STABILIZATION_TIME_MS 0
/** @brief Sensor::_measurementTime_ms; the RainVUE10 takes >100ms to
 * complete a measurement.
 */
#define RAINVUE10_MEASUREMENT_TIME_MS 100
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The RainVUE requires no extra time.
#define RAINVUE10_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_rainvue_precipitation
 * @name Precipitation Accumulated Depth
 * The intensity-corrected accumulation variable from a Campbell RainVUE10
 * For the "-IN" Option (English units)
 * - Range is 0 to 1000 mm/h (0 to 40 in./h).
 * - Resolution is 0.254 mm (0.01 in.)
 * - Accuracy is ±1% at 0 to 500 mm/h intensity (0 to 19.7 in./h intensity)
 * For the "-MM" Option (Metric units)
 * - Range is 0 to 500 mm/h (0 to 19.7 in./h)
 * - Resolution is 0.1 mm (0.004 in.)
 * - Accuracy is ±3% at 0 to 300 mm/h intensity (0 to 11.8 in./h intensity)
 *               ±5% at 300 to 500 mm/h intensity (11.8 to 19.7 in./h intensity)
 *
 * {{ @ref CampbellRainVUE10_Preciptation::CampbellRainVUE10_Precipitation }}
 */
/**@{*/
/// @brief Decimals places in string representation; depth should have 2
/// (resolution is 0.01 inches).
#define RAINVUE10_PRECIPITATION_RESOLUTION 2
/// @brief Sensor variable number; precipitation is stored in sensorValues[0]
#define RAINVUE10_PRECIPITATION_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "precipitation"
#define RAINVUE10_PRECIPITATION_VAR_NAME "precipitation"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "inch" (in_i)
#define RAINVUE10_PRECIPITATION_UNIT_NAME "inch"
/// @brief Default variable short code; "RainVUEPrecipitation"
#define RAINVUE10_PRECIPITATION_DEFAULT_CODE "RainVUEPrecipitation"
/**@}*/

/**
 * @anchor sensor_rainvue_tips
 * @name Tip Count
 * Defines for tip count variable from a tipping bucket counter
 * - Range and accuracy depend on the tipping bucket used.
 *
 * {{ @ref CampbellRainVUE10_Tips::CampbellRainVUE10_Tips }}
 */
/**@{*/
/// @brief Decimals places in string representation; the number of tips should
/// have 0 - resolution is 1 tip.
#define RAINVUE10_TIPS_RESOLUTION 0
/// @brief Sensor variable number; tips is stored in sensorValues[1].
#define RAINVUE10_TIPS_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "precipitation"
#define RAINVUE10_TIPS_VAR_NAME "precipitation"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "event"
#define RAINVUE10_TIPS_UNIT_NAME "event"
/// @brief Default variable short code; "RainVUETips"
#define RAINVUE10_TIPS_DEFAULT_CODE "RainVUETips"
/**@}*/

/**
 * @anchor sensor_rainvue_rainrateave
 * @name Rainfall Rate Average
 * The average rainfall rate variable from a Campbell RainVUE10,
 * defined as average precipitation intensity since last measurement.
 * For the "-IN" Option (English units)
 * - Range is 0.01 to 1000 mm/h (0.0004 to 39.4 in./h)
 * - Resolution is 0.254 mm (0.01 in.)
 * - Accuracy is ±1%
 * For the "-MM" Option (Metric units)
 * - Range is 0.01 to 500 mm/h (0.0004 to 19.7 in./h)
 * - Resolution is 0.1 mm (0.004 in.)
 * - Accuracy is ±3.5% at 0 to 300 mm/h (0 to 11.8 in./h)
 *               ±5% at 300 to 500 mm/h (11.8 to 19.7 in./h)
 *
 * {{ @ref CampbellRainVUE10_RainRateAve::CampbellRainVUE10_RainRateAve }}
 */
/**@{*/
/// @brief Decimals places in string representation; the rainfall intensity
/// has 2.
#define RAINVUE10_RAINRATEAVE_RESOLUTION 2
/// @brief Sensor variable number; average intensity is stored in
/// sensorValues[3]
#define RAINVUE10_RAINRATEAVE_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define RAINVUE10_RAINRATEAVE_VAR_NAME "rainfallRate"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "inchPerHour"
#define RAINVUE10_RAINRATEAVE_UNIT_NAME "inchPerHour"
/// @brief Default variable short code; "RainVUERateAve"
#define RAINVUE10_RAINRATEAVE_DEFAULT_CODE "RainVUERateAve"
/**@}*/

/**
 * @anchor sensor_rainvue_rainratemax
 * @name Rainfall Rate Maximum
 * The maximum rainfall rate variable from a Campbell RainVUE10,
 * defined as maximum precipitation intensity since last measurement.
 * - Range & Accuracy same as for sensor_rainvue_rainratemax
 * {{ @ref CampbellRainVUE10_RainRateMax::CampbellRainVUE10_RainRateMax }}
 */
/**@{*/
/// @brief Decimals places in string representation; the rainfall intensity
/// has 2.
#define RAINVUE10_RAINRATEMAX_RESOLUTION 2
/// @brief Sensor variable number; average intensity is stored in
/// sensorValues[4]
#define RAINVUE10_RAINRATEMAX_VAR_NUM 4
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define RAINVUE10_RAINRATEMAX_VAR_NAME "rainfallRate"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "inchPerHour"
#define RAINVUE10_RAINRATEMAX_UNIT_NAME "inchPerHour"
/// @brief Default variable short code; "RainVUERateAve"
#define RAINVUE10_RAINRATEMAX_DEFAULT_CODE "RainVUERateMax"
/**@}*/

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Campbell RainVUE10 precipitation sensor](@ref sensor_rainvue).
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
 * [precipitation output](@ref sensor_rainvue_precipitation)
 * from a [Campbell RainVUE10 precipitation sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */
/* clang-format on */
class CampbellRainVUE10_Precipitation : public Variable {
 public:
    /**
     * @brief Construct a new CampbellRainVUE10_Precipitation object.
     *
     * @param parentSense The parent CampbellRainVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainVUEPrecipitation".
     */
    explicit CampbellRainVUE10_Precipitation(
        CampbellRainVUE10* parentSense, const char* uuid = "",
        const char* varCode = RAINVUE10_PRECIPITATION_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)RAINVUE10_PRECIPITATION_VAR_NUM,
                   (uint8_t)RAINVUE10_PRECIPITATION_RESOLUTION,
                   RAINVUE10_PRECIPITATION_VAR_NAME,
                   RAINVUE10_PRECIPITATION_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellRainVUE10_Precipitation object.
     *
     * @note This must be tied with a parent CampbellRainVUE10 before it can be
     * used.
     */
    CampbellRainVUE10_Precipitation()
        : Variable((const uint8_t)RAINVUE10_PRECIPITATION_VAR_NUM,
                   (uint8_t)RAINVUE10_PRECIPITATION_RESOLUTION,
                   RAINVUE10_PRECIPITATION_VAR_NAME,
                   RAINVUE10_PRECIPITATION_UNIT_NAME,
                   RAINVUE10_PRECIPITATION_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellRainVUE10_Precipitation object - no action
     * needed.
     */
    ~CampbellRainVUE10_Precipitation() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [tip count output](@ref sensor_rainvue_tips) from a
 * [Campbell RainVUE10 precipitation sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */
/* clang-format on */
class CampbellRainVUE10_Tips : public Variable {
 public:
    /**
     * @brief Construct a new CampbellRainVUE10_Tips object.
     *
     * @param parentSense The parent CampbellRainVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainVUETips".
     */
    explicit CampbellRainVUE10_Tips(
        CampbellRainVUE10* parentSense, const char* uuid = "",
        const char* varCode = RAINVUE10_TIPS_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)RAINVUE10_TIPS_VAR_NUM,
                   (uint8_t)RAINVUE10_TIPS_RESOLUTION, RAINVUE10_TIPS_VAR_NAME,
                   RAINVUE10_TIPS_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellRainVUE10_Tips object.
     *
     * @note This must be tied with a parent CampbellRainVUE10 before it can be
     * used.
     */
    CampbellRainVUE10_Tips()
        : Variable((const uint8_t)RAINVUE10_TIPS_VAR_NUM,
                   (uint8_t)RAINVUE10_TIPS_RESOLUTION, RAINVUE10_TIPS_VAR_NAME,
                   RAINVUE10_TIPS_UNIT_NAME, RAINVUE10_TIPS_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellRainVUE10_Tips object - no action needed.
     */
    ~CampbellRainVUE10_Tips() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [average rainfall rate output](@ref sensor_rainvue_rainrateave) from a
 * [Campbell RainVUE10 precipitation sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */
/* clang-format on */
class CampbellRainVUE10_RainRateAve : public Variable {
 public:
    /**
     * @brief Construct a new CampbellRainVUE10_RainRateAve object.
     *
     * @param parentSense The parent CampbellRainVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainVUEError".
     */
    explicit CampbellRainVUE10_RainRateAve(
        CampbellRainVUE10* parentSense, const char* uuid = "",
        const char* varCode = RAINVUE10_RAINRATEAVE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)RAINVUE10_RAINRATEAVE_VAR_NUM,
                   (uint8_t)RAINVUE10_RAINRATEAVE_RESOLUTION,
                   RAINVUE10_RAINRATEAVE_VAR_NAME,
                   RAINVUE10_RAINRATEAVE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellRainVUE10_RainRateAve object.
     *
     * @note This must be tied with a parent CampbellRainVUE10 before it can be
     * used.
     */
    CampbellRainVUE10_RainRateAve()
        : Variable((const uint8_t)RAINVUE10_RAINRATEAVE_VAR_NUM,
                   (uint8_t)RAINVUE10_RAINRATEAVE_RESOLUTION,
                   RAINVUE10_RAINRATEAVE_VAR_NAME,
                   RAINVUE10_RAINRATEAVE_UNIT_NAME,
                   RAINVUE10_RAINRATEAVE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellRainVUE10_RainRateAve object - no action
     * needed.
     */
    ~CampbellRainVUE10_RainRateAve() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [maximum rainfall rate output](@ref sensor_rainvue_rainratemax) from a
 * [Campbell RainVUE10 precipitation sensor](@ref sensor_rainvue).
 *
 * @ingroup sensor_rainvue
 */


/* clang-format on */
class CampbellRainVUE10_RainRateMax : public Variable {
 public:
    /**
     * @brief Construct a new CampbellRainVUE10_RainRateMax object.
     *
     * @param parentSense The parent CampbellRainVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainVUERateAve".
     */
    explicit CampbellRainVUE10_RainRateMax(
        CampbellRainVUE10* parentSense, const char* uuid = "",
        const char* varCode = RAINVUE10_RAINRATEMAX_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)RAINVUE10_RAINRATEMAX_VAR_NUM,
                   (uint8_t)RAINVUE10_RAINRATEMAX_RESOLUTION,
                   RAINVUE10_RAINRATEMAX_VAR_NAME,
                   RAINVUE10_RAINRATEMAX_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellRainVUE10_RainRateMax object.
     *
     * @note This must be tied with a parent CampbellRainVUE10 before it can be
     * used.
     */
    CampbellRainVUE10_RainRateMax()
        : Variable((const uint8_t)RAINVUE10_RAINRATEMAX_VAR_NUM,
                   (uint8_t)RAINVUE10_RAINRATEMAX_RESOLUTION,
                   RAINVUE10_RAINRATEMAX_VAR_NAME,
                   RAINVUE10_RAINRATEMAX_UNIT_NAME,
                   RAINVUE10_RAINRATEMAX_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellRainVUE10_RainRateMax object - no action
     * needed.
     */
    ~CampbellRainVUE10_RainRateMax() {}
};
/**@}*/
#endif  // SRC_SENSORS_CAMPBELLRAINVUE10_H_
