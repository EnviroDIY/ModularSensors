/**
 * @file CampbellClariVUE10.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the CampbellClariVUE10 sensor subclass and the variable
 * subclasses CampbellClariVUE10_Temp, CampbellClariVUE10_Turbidity, and
 * CampbellClariVUE10_ErrorCode.
 *
 * These are for the Campbell ClariVUE10 digital SDI-12 turbidity sensor.
 *
 * This depends on the SDI12Sensors parent class.
 */
/* clang-format off */
/**
 * @defgroup sensor_clarivue Campbell ClariVUE10
 * Classes for the [Campbell ClariVUE10](https://www.campbellsci.com/clarivue10) turbidity sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_clarivue_intro Introduction
 *
 * > The ClariVUE™10 is an ISO 7027 compliant, side-scatter turbidity sensor. It
 * > returns data via SDI-12 to a data logger. Turbidity is a common surrogate
 * > measurement for suspended sediment concentration in aquatic systems. The
 * > ClariVUE™10 is designed to operate in fresh and saline environments. It
 * > will require regular maintenance to keep the optical window clean in high
 * > bio-fouling environments. The face of the sensor is made from copper to
 * > slow biological growth over the optical windows.
 *
 * The sensor is implemented as a sub-classes of the SDI12Sensors class.
 * It requires a 9.6 to 18 Vdc power supply, which can be turned off between measurements.
 * It draws < 300 µA when inactive and < 35 mA while measuring.
 *
 * @note While it is supported, you should not average measurements for this sensor.
 * The sensor takes a burst of 30 readings and returns the median of those.
 *
 * @section sensor_clarivue_datasheet Sensor Datasheet
 * The specifications and datasheet are available at https://www.campbellsci.com/clarivue10
 *
 * @section sensor_clarivue_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_clarivue_ctor Sensor Constructor
 * {{ @ref CampbellClariVUE10::CampbellClariVUE10 }}
 *
 * ___
 * @section sensor_clarivue_examples Example Code
 * The Campbell ClariVUE10 is used in the @menulink{campbell_clari_vue10} example.
 *
 * @menusnip{campbell_clari_vue10}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_CAMPBELLCLARIVUE10_H_
#define SRC_SENSORS_CAMPBELLCLARIVUE10_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_clarivue */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the ClariVUE10 can report 7 values
/// (although we only keep 3).
#define CLARIVUE10_NUM_VARIABLES 7
/// @brief Sensor::_incCalcValues; The ClariVUE calculates averages and other
/// stats on board, but we don't calculate any additional values.
#define CLARIVUE10_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_clarivue_timing
 * @name Sensor Timing
 * The sensor timing for a Campbell ClariVUE10
 */
/**@{*/
/** @brief Sensor::_warmUpTime_ms; the ClariVUE10 warms up in 5500ms.
 *
 * This is longer than the expected 250ms for a SDI-12 sensor, but I couldn't
 * get a response from the sensor faster than that.
 */
#define CLARIVUE10_WARM_UP_TIME_MS 8000
/// @brief Sensor::_stabilizationTime_ms; the ClariVUE10 is stable as soon as it
/// warms up (0ms stabilization).
#define CLARIVUE10_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the ClariVUE10 takes ~9500ms to
 * complete a measurement.
 *
 * Spec sheet says the measurement time is 9s.  When taking a standard
 * measurement I was not getting a result until after about 9335ms.
 */
#define CLARIVUE10_MEASUREMENT_TIME_MS 11000
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The ClariVUE requires no extra time.
#define CLARIVUE10_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_clarivue_turbidity
 * @name Turbidity
 * The turbidity variable from a Campbell ClariVUE10
 * - Range is 0 to 4000 FNU.
 * - Accuracy ±2% or 0.5 FNU (whichever is greater).
 *
 * {{ @ref CampbellClariVUE10_Turbidity::CampbellClariVUE10_Turbidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; turbidity should have 1
/// (resolution is 0.2 FNU).
#define CLARIVUE10_TURBIDITY_RESOLUTION 1
/// @brief Sensor variable number; turbidity is stored in sensorValues[0]
#define CLARIVUE10_TURBIDITY_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define CLARIVUE10_TURBIDITY_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "formazinNephelometricUnit" (FNU)
#define CLARIVUE10_TURBIDITY_UNIT_NAME "formazinNephelometricUnit"
/// @brief Default variable short code; "ClariVUETurbidity"
#define CLARIVUE10_TURBIDITY_DEFAULT_CODE "ClariVUETurbidity"
/**@}*/

/**
 * @anchor sensor_clarivue_temp
 * @name Temperature
 * The temperature variable from a Campbell ClariVUE10
 * - Range is -2° to +40°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref CampbellClariVUE10_Temp::CampbellClariVUE10_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define CLARIVUE10_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[5].
#define CLARIVUE10_TEMP_VAR_NUM 5
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define CLARIVUE10_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define CLARIVUE10_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "ClariVUETempC"
#define CLARIVUE10_TEMP_DEFAULT_CODE "ClariVUETempC"
/**@}*/

/**
 * @anchor sensor_clarivue_error
 * @name Error Code
 * The error code variable from a Campbell ClariVUE10
 * - Significance of error code values is unknown.
 *
 * {{ @ref CampbellClariVUE10_ErrorCode::CampbellClariVUE10_ErrorCode }}
 */
/**@{*/
/// @brief Decimals places in string representation; the error code has 0.
#define CLARIVUE10_ERRORCODE_RESOLUTION 0
/// @brief Sensor variable number; error code is stored in sensorValues[2]
#define CLARIVUE10_ERRORCODE_VAR_NUM 6
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define CLARIVUE10_ERRORCODE_VAR_NAME "instrumentStatusCode"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define CLARIVUE10_ERRORCODE_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "ClariVUEError"
#define CLARIVUE10_ERRORCODE_DEFAULT_CODE "ClariVUEError"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Campbell ClariVUE10 turbidity sensor](@ref sensor_clarivue).
 *
 * @ingroup sensor_clarivue
 */
/* clang-format on */
class CampbellClariVUE10 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Campbell ClariVUE10 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required
     * for the sensor constructor.  Optionally, you can include a number of
     * distinct readings to average.  The data pin must be a pin that
     * supports pin-change interrupts.
     *
     * @param SDI12address The SDI-12 address of the ClariVUE10; can be a
     * char, char*, or int.
     * @param powerPin The pin on the mcu controlling power to the
     * ClariVUE10 Use -1 if it is continuously powered.
     * - The ClariVUE10 requires a 9.6 to 18 Vdc power supply, which can be
     * turned off between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with
     * a default value of 1.
     */
    CampbellClariVUE10(char SDI12address, int8_t powerPin, int8_t dataPin,
                       uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "Campbell ClariVUE10", CLARIVUE10_NUM_VARIABLES,
              CLARIVUE10_WARM_UP_TIME_MS, CLARIVUE10_STABILIZATION_TIME_MS,
              CLARIVUE10_MEASUREMENT_TIME_MS, CLARIVUE10_EXTRA_WAKE_TIME_MS,
              CLARIVUE10_INC_CALC_VARIABLES) {}
    /**
     * @copydoc CampbellClariVUE10::CampbellClariVUE10
     */
    CampbellClariVUE10(char* SDI12address, int8_t powerPin, int8_t dataPin,
                       uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "Campbell ClariVUE10", CLARIVUE10_NUM_VARIABLES,
              CLARIVUE10_WARM_UP_TIME_MS, CLARIVUE10_STABILIZATION_TIME_MS,
              CLARIVUE10_MEASUREMENT_TIME_MS, CLARIVUE10_EXTRA_WAKE_TIME_MS,
              CLARIVUE10_INC_CALC_VARIABLES) {}
    /**
     * @copydoc CampbellClariVUE10::CampbellClariVUE10
     */
    CampbellClariVUE10(int SDI12address, int8_t powerPin, int8_t dataPin,
                       uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "Campbell ClariVUE10", CLARIVUE10_NUM_VARIABLES,
              CLARIVUE10_WARM_UP_TIME_MS, CLARIVUE10_STABILIZATION_TIME_MS,
              CLARIVUE10_MEASUREMENT_TIME_MS, CLARIVUE10_EXTRA_WAKE_TIME_MS,
              CLARIVUE10_INC_CALC_VARIABLES) {}

    /**
     * @brief Destroy the Campbell ClariVUE10 object
     */
    ~CampbellClariVUE10() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_clarivue_turbidity)
 * from a [Campbell ClariVUE10 turbidity sensor](@ref sensor_clarivue).
 *
 * @ingroup sensor_clarivue
 */
/* clang-format on */
class CampbellClariVUE10_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new CampbellClariVUE10_Turbidity object.
     *
     * @param parentSense The parent CampbellClariVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ClariVUETurbidity".
     */
    explicit CampbellClariVUE10_Turbidity(
        CampbellClariVUE10* parentSense, const char* uuid = "",
        const char* varCode = CLARIVUE10_TURBIDITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)CLARIVUE10_TURBIDITY_VAR_NUM,
                   (uint8_t)CLARIVUE10_TURBIDITY_RESOLUTION,
                   CLARIVUE10_TURBIDITY_VAR_NAME,
                   CLARIVUE10_TURBIDITY_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellClariVUE10_Turbidity object.
     *
     * @note This must be tied with a parent CampbellClariVUE10 before it can be
     * used.
     */
    CampbellClariVUE10_Turbidity()
        : Variable((const uint8_t)CLARIVUE10_TURBIDITY_VAR_NUM,
                   (uint8_t)CLARIVUE10_TURBIDITY_RESOLUTION,
                   CLARIVUE10_TURBIDITY_VAR_NAME,
                   CLARIVUE10_TURBIDITY_UNIT_NAME,
                   CLARIVUE10_TURBIDITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellClariVUE10_Turbidity object - no action
     * needed.
     */
    ~CampbellClariVUE10_Turbidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_clarivue_temp) from a
 * [Campbell ClariVUE10 turbidity sensor](@ref sensor_clarivue).
 *
 * @ingroup sensor_clarivue
 */
/* clang-format on */
class CampbellClariVUE10_Temp : public Variable {
 public:
    /**
     * @brief Construct a new CampbellClariVUE10_Temp object.
     *
     * @param parentSense The parent CampbellClariVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ClariVUETempC".
     */
    explicit CampbellClariVUE10_Temp(
        CampbellClariVUE10* parentSense, const char* uuid = "",
        const char* varCode = CLARIVUE10_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)CLARIVUE10_TEMP_VAR_NUM,
                   (uint8_t)CLARIVUE10_TEMP_RESOLUTION,
                   CLARIVUE10_TEMP_VAR_NAME, CLARIVUE10_TEMP_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new CampbellClariVUE10_Temp object.
     *
     * @note This must be tied with a parent CampbellClariVUE10 before it can be
     * used.
     */
    CampbellClariVUE10_Temp()
        : Variable((const uint8_t)CLARIVUE10_TEMP_VAR_NUM,
                   (uint8_t)CLARIVUE10_TEMP_RESOLUTION,
                   CLARIVUE10_TEMP_VAR_NAME, CLARIVUE10_TEMP_UNIT_NAME,
                   CLARIVUE10_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellClariVUE10_Temp object - no action needed.
     */
    ~CampbellClariVUE10_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [error code output](@ref sensor_clarivue_error) from a
 * [Campbell ClariVUE10 turbidity sensor](@ref sensor_clarivue).
 *
 * @ingroup sensor_clarivue
 */
/* clang-format on */
class CampbellClariVUE10_ErrorCode : public Variable {
 public:
    /**
     * @brief Construct a new CampbellClariVUE10_ErrorCode object.
     *
     * @param parentSense The parent CampbellClariVUE10 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ClariVUEError".
     */
    explicit CampbellClariVUE10_ErrorCode(
        CampbellClariVUE10* parentSense, const char* uuid = "",
        const char* varCode = CLARIVUE10_ERRORCODE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)CLARIVUE10_ERRORCODE_VAR_NUM,
                   (uint8_t)CLARIVUE10_ERRORCODE_RESOLUTION,
                   CLARIVUE10_ERRORCODE_VAR_NAME,
                   CLARIVUE10_ERRORCODE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new CampbellClariVUE10_ErrorCode object.
     *
     * @note This must be tied with a parent CampbellClariVUE10 before it can be
     * used.
     */
    CampbellClariVUE10_ErrorCode()
        : Variable((const uint8_t)CLARIVUE10_ERRORCODE_VAR_NUM,
                   (uint8_t)CLARIVUE10_ERRORCODE_RESOLUTION,
                   CLARIVUE10_ERRORCODE_VAR_NAME,
                   CLARIVUE10_ERRORCODE_UNIT_NAME,
                   CLARIVUE10_ERRORCODE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the CampbellClariVUE10_ErrorCode object - no action
     * needed.
     */
    ~CampbellClariVUE10_ErrorCode() {}
};
/**@}*/
#endif  // SRC_SENSORS_CAMPBELLCLARIVUE10_H_
