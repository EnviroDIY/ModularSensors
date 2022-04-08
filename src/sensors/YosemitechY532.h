/**
 * @file YosemitechY532.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY532 sensor subclass and the variable
 * subclasses YosemitechY532_pH, YosemitechY532_Temp, and
 * YosemitechY532_Voltage.
 *
 * These are for the Yosemitech Y532 pH sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y532 Yosemitech Y532 pH Sensor
 * Classes for the Yosemitech Y532 pH sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y532_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH_UserManual-v1.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH-ORP-v1.7_ModbusInstructions.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y532_ctor Sensor Constructor
 * {{ @ref YosemitechY532::YosemitechY532 }}
 *
 * ___
 * @section sensor_y532_examples Example Code
 * The Yosemitech Y532 pH sensor is used in the @menulink{yosemitech_y532} example.
 *
 * @menusnip{yosemitech_y532}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY532_H_
#define SRC_SENSORS_YOSEMITECHY532_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y532 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y532 can report 3 values.
#define Y532_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y532_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y532_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y532
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// 500ms.
#define Y532_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 4.5sec (4500ms).
#define Y532_STABILIZATION_TIME_MS 4500
/// @brief Sensor::_measurementTime_ms; the Y532 takes ~1800ms to complete a
/// measurement.
#define Y532_MEASUREMENT_TIME_MS 1800
/**@}*/

/**
 * @anchor sensor_y532_ph
 * @name pH
 * The pH variable from a Yosemitch Y532
 * - Range is 2 to 12 pH units
 * - Accuracy is ± 0.1 pH units
 *
 * {{ @ref YosemitechY532_pH::YosemitechY532_pH }}
 */
/**@{*/
/// @brief Decimals places in string representation; pH should have 2 -
/// resolution is 0.01 pH units.
#define Y532_PH_RESOLUTION 2
/// @brief Sensor variable number; pH is stored in sensorValues[0].
#define Y532_PH_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/); "pH"
#define Y532_PH_VAR_NAME "pH"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "pH"
/// (dimensionless pH units)
#define Y532_PH_UNIT_NAME "pH"
/// @brief Default variable short code; "Y532pH"
#define Y532_PH_DEFAULT_CODE "Y532pH"
/**@}*/

/**
 * @anchor sensor_y532_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y532
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY532_Temp::YosemitechY532_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y532_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y532_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y532_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y532_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y532Temp"
#define Y532_TEMP_DEFAULT_CODE "Y532Temp"
/**@}*/

/**
 * @anchor sensor_y532_volt
 * @name Voltage
 * The voltage variable from a Yosemitch Y532
 * - Range is -999 ~ 999 mV
 * - Accuracy is ± 20 mV
 *
 * {{ @ref YosemitechY532_Voltage::YosemitechY532_Voltage }}
 */
/**@{*/
/// @brief Decimals places in string representation; voltage should have 0 -
/// resolution is 1mV.
#define Y532_VOLTAGE_RESOLUTION 0
/// @brief Sensor variable number; voltage is stored in sensorValues[2].
#define Y532_VOLTAGE_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define Y532_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "millivolt"
/// (mV)
#define Y532_VOLTAGE_UNIT_NAME "millivolt"
/// @brief Default variable short code; "Y532Potential"
#define Y532_VOLTAGE_DEFAULT_CODE "Y532Potential"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y532-A digital pH sensor](@ref sensor_y532).
 *
 * @ingroup sensor_y532
 */
/* clang-format on */
class YosemitechY532 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y532 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y532.
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
    YosemitechY532(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y532,
                           "YosemitechY532", Y532_NUM_VARIABLES,
                           Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS,
                           Y532_MEASUREMENT_TIME_MS, Y532_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY532::YosemitechY532
     */
    YosemitechY532(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y532,
                           "YosemitechY532", Y532_NUM_VARIABLES,
                           Y532_WARM_UP_TIME_MS, Y532_STABILIZATION_TIME_MS,
                           Y532_MEASUREMENT_TIME_MS, Y532_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y532 object
     */
    ~YosemitechY532() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pH output](@ref sensor_y532_ph)
 * from a  [Yosemitech Y532-A digital pH sensor](@ref sensor_y532).
 *
 * @ingroup sensor_y532
 */
/* clang-format on */
class YosemitechY532_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY532_pH object.
     *
     * @param parentSense The parent YosemitechY532 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y532pH".
     */
    explicit YosemitechY532_pH(YosemitechY532* parentSense,
                               const char*     uuid    = "",
                               const char*     varCode = Y532_PH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y532_PH_VAR_NUM,
                   (uint8_t)Y532_PH_RESOLUTION, Y532_PH_VAR_NAME,
                   Y532_PH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY532_pH object.
     *
     * @note This must be tied with a parent YosemitechY532 before it can be
     * used.
     */
    YosemitechY532_pH()
        : Variable((const uint8_t)Y532_PH_VAR_NUM, (uint8_t)Y532_PH_RESOLUTION,
                   Y532_PH_VAR_NAME, Y532_PH_UNIT_NAME, Y532_PH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY532_pH object - no action needed.
     */
    ~YosemitechY532_pH() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y532_temp) from a
 * [Yosemitech Y532-A digital pH sensor](@ref sensor_y532).
 *
 * @ingroup sensor_y532
 */
/* clang-format on */
class YosemitechY532_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY532_Temp object.
     *
     * @param parentSense The parent YosemitechY532 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y532Temp".
     */
    explicit YosemitechY532_Temp(YosemitechY532* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y532_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y532_TEMP_VAR_NUM,
                   (uint8_t)Y532_TEMP_RESOLUTION, Y532_TEMP_VAR_NAME,
                   Y532_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY532_Temp object.
     *
     * @note This must be tied with a parent YosemitechY532 before it can be
     * used.
     */
    YosemitechY532_Temp()
        : Variable((const uint8_t)Y532_TEMP_VAR_NUM,
                   (uint8_t)Y532_TEMP_RESOLUTION, Y532_TEMP_VAR_NAME,
                   Y532_TEMP_UNIT_NAME, Y532_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY532_Temp object - no action needed.
     */
    ~YosemitechY532_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [electrode electrical potential output](@ref sensor_y532_volt) from a
 * [Yosemitech Y532-A digital pH sensor](@ref sensor_y532).
 *
 * @ingroup sensor_y532
 */
/* clang-format on */
class YosemitechY532_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY532_Voltage object.
     *
     * @param parentSense The parent YosemitechY532 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y532Potential".
     */
    explicit YosemitechY532_Voltage(
        YosemitechY532* parentSense, const char* uuid = "",
        const char* varCode = Y532_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y532_VOLTAGE_VAR_NUM,
                   (uint8_t)Y532_VOLTAGE_RESOLUTION, Y532_VOLTAGE_VAR_NAME,
                   Y532_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY532_Voltage object.
     *
     * @note This must be tied with a parent YosemitechY532 before it can be
     * used.
     */
    YosemitechY532_Voltage()
        : Variable((const uint8_t)Y532_VOLTAGE_VAR_NUM,
                   (uint8_t)Y532_VOLTAGE_RESOLUTION, Y532_VOLTAGE_VAR_NAME,
                   Y532_VOLTAGE_UNIT_NAME, Y532_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY532_Voltage object - no action needed.
     */
    ~YosemitechY532_Voltage() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY532_H_
