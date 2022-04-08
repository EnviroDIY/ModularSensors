/**
 * @file YosemitechY510.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY510 sensor subclass and the variable
 * subclasses YosemitechY510_Turbidity and YosemitechY510_Temp.
 *
 * These are for Yosemitech Y510 Turbidity Sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y510 Yosemitech Y510 Turbidity Sensor
 * Classes for the Yosemitech Y510 turbidity sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y510_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y510-Turbidity_UserManual-v1.1.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y510-Turbidity_1.7-ModbusInstruction-en.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y510_ctor Sensor Constructor
 * {{ @ref YosemitechY510::YosemitechY510 }}
 *
 * ___
 * @section sensor_y510_examples Example Code
 * The Yosemitech Y510 Turbidity is used in the @menulink{yosemitech_y510} example.
 *
 * @menusnip{yosemitech_y510}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY510_H_
#define SRC_SENSORS_YOSEMITECHY510_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y510 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y510 can report 2 values.
#define Y510_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y510_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y510_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y510
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// 500ms.
#define Y510_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 22sec (22000ms).
#define Y510_STABILIZATION_TIME_MS 22000
/// @brief Sensor::_measurementTime_ms; the Y510 takes ~1700ms to complete a
/// measurement.
#define Y510_MEASUREMENT_TIME_MS 1700
/**@}*/

/**
 * @anchor sensor_y510_turb
 * @name Turbidity
 * The turbidity variable from a Yosemitch Y510
 * - Range is 0.1 to 1000 NTU
 * - Accuracy is ± 5 % or 0.3 NTU
 *
 * {{ @ref YosemitechY510_Turbidity::YosemitechY510_Turbidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; turbidity should have 2 -
/// resolution is 0.01 NTU.
#define Y510_TURB_RESOLUTION 2
/// @brief Sensor variable number; turbidity is stored in sensorValues[0].
#define Y510_TURB_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define Y510_TURB_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "nephelometricTurbidityUnit" (NTU)
#define Y510_TURB_UNIT_NAME "nephelometricTurbidityUnit"
/// @brief Default variable short code; "Y510Turbidity"
#define Y510_TURB_DEFAULT_CODE "Y510Turbidity"
/**@}*/

/**
 * @anchor sensor_y510_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y510
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY510_Temp::YosemitechY510_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y510_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y510_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y510_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y510_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y510Temp"
#define Y510_TEMP_DEFAULT_CODE "Y510Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y510-B optical turbidity sensor](@ref sensor_y510).
 *
 * @ingroup sensor_y510
 */
/* clang-format on */
class YosemitechY510 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y510 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y510.
     * Use -1 if it is continuously powered.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.
     * @note An RS485 adapter with integrated flow control is strongly
     * recommended.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    YosemitechY510(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y510,
                           "YosemitechY510", Y510_NUM_VARIABLES,
                           Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS,
                           Y510_MEASUREMENT_TIME_MS, Y510_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY510::YosemitechY510
     */
    YosemitechY510(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y510,
                           "YosemitechY510", Y510_NUM_VARIABLES,
                           Y510_WARM_UP_TIME_MS, Y510_STABILIZATION_TIME_MS,
                           Y510_MEASUREMENT_TIME_MS, Y510_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y510 object
     */
    ~YosemitechY510() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_y510_turb) from a
 * [Yosemitech Y510-B optical turbidity sensor](@ref sensor_y510).
 *
 * @ingroup sensor_y510
 */
/* clang-format on */
class YosemitechY510_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY510_Turbidity object.
     *
     * @param parentSense The parent YosemitechY510 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y510Turbidity".
     */
    explicit YosemitechY510_Turbidity(
        YosemitechY510* parentSense, const char* uuid = "",
        const char* varCode = Y510_TURB_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y510_TURB_VAR_NUM,
                   (uint8_t)Y510_TURB_RESOLUTION, Y510_TURB_VAR_NAME,
                   Y510_TURB_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY510_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY510 before it can be
     * used.
     */
    YosemitechY510_Turbidity()
        : Variable((const uint8_t)Y510_TURB_VAR_NUM,
                   (uint8_t)Y510_TURB_RESOLUTION, Y510_TURB_VAR_NAME,
                   Y510_TURB_UNIT_NAME, Y510_TURB_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY510_Turbidity object - no action needed.
     */
    ~YosemitechY510_Turbidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y510_temp) from a
 * [Yosemitech Y510-B optical turbidity sensor](@ref sensor_y510).
 *
 * @ingroup sensor_y510
 */
/* clang-format on */
class YosemitechY510_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY510_Temp object.
     *
     * @param parentSense The parent YosemitechY510 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y510Temp".
     */
    explicit YosemitechY510_Temp(YosemitechY510* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y510_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y510_TEMP_VAR_NUM,
                   (uint8_t)Y510_TEMP_RESOLUTION, Y510_TEMP_VAR_NAME,
                   Y510_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY510_Temp object.
     *
     * @note This must be tied with a parent YosemitechY510 before it can be
     * used.
     */
    YosemitechY510_Temp()
        : Variable((const uint8_t)Y510_TEMP_VAR_NUM,
                   (uint8_t)Y510_TEMP_RESOLUTION, Y510_TEMP_VAR_NAME,
                   Y510_TEMP_UNIT_NAME, Y510_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY510_Temp object - no action needed.
     */
    ~YosemitechY510_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY510_H_
