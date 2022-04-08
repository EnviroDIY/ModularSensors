/**
 * @file YosemitechY520.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY520 sensor subclass and the variable
 * subclasses YosemitechY520_Cond and YosemitechY520_Temp.
 *
 * These are for the Yosemitech Y520 4-pole conductivity sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y520 Yosemitech Y520 Conductivity Sensor
 * Classes for the Yosemitech Y520 4-pole conductivity sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y520_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y520-Conductivity_UserManual-v1.1.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y520-Conductivity-v1.8_ModbusInstructions.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y520_ctor Sensor Constructor
 * {{ @ref YosemitechY520::YosemitechY520 }}
 *
 * ___
 * @section sensor_y520_examples Example Code
 * The Yosemitech Y520 conductivity sensor is used in the @menulink{yosemitech_y520}
 * example.
 *
 * @menusnip{yosemitech_y520}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY520_H_
#define SRC_SENSORS_YOSEMITECHY520_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y520 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y520 can report 2 values.
#define Y520_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y520_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y520_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y520
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// 1600 ms.
#define Y520_WARM_UP_TIME_MS 1600
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 10sec (10,000ms).
#define Y520_STABILIZATION_TIME_MS 10000
/// @brief Sensor::_measurementTime_ms; the Y520 takes ~2700ms to complete a
/// measurement.
#define Y520_MEASUREMENT_TIME_MS 2700
/**@}*/

/**
 * @anchor sensor_y520_cond
 * @name Conductivity
 * The conductivity variable from a Yosemitch Y520
 * - Range is 1 µS/cm to 200 mS/cm
 * - Accuracy is ± 1 % Full Scale
 *
 * {{ @ref YosemitechY520_Cond::YosemitechY520_Cond }}
 */
/**@{*/
/// @brief Decimals places in string representation; conductivity should have 1
/// - resolution is 0.1 µS/cm.
#define Y520_COND_RESOLUTION 1
/// @brief Sensor variable number; conductivity is stored in sensorValues[0].
#define Y520_COND_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "specificConductance"
#define Y520_COND_VAR_NAME "specificConductance"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microsiemenPerCentimeter" (µS/cm)
#define Y520_COND_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "Y520Cond"
#define Y520_COND_DEFAULT_CODE "Y520Cond"
/**@}*/

/**
 * @anchor sensor_y520_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y520
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY520_Temp::YosemitechY520_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y520_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y520_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y520_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y520_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y520Temp"
#define Y520_TEMP_DEFAULT_CODE "Y520Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y520-A 4-electrode conductivity sensor](@ref sensor_y520).
 *
 * @ingroup sensor_y520
 */
/* clang-format on */
class YosemitechY520 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y520 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y520.
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
    YosemitechY520(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y520,
                           "YosemitechY520", Y520_NUM_VARIABLES,
                           Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS,
                           Y520_MEASUREMENT_TIME_MS, Y520_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY520::YosemitechY520
     */
    YosemitechY520(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y520,
                           "YosemitechY520", Y520_NUM_VARIABLES,
                           Y520_WARM_UP_TIME_MS, Y520_STABILIZATION_TIME_MS,
                           Y520_MEASUREMENT_TIME_MS, Y520_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y520 object
     */
    ~YosemitechY520() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [conductivity output](@ref sensor_y520_cond) from a
 * [Yosemitech Y520-A 4-electrode conductivity sensor](@ref sensor_y520).
 *
 * @ingroup sensor_y520
 */
/* clang-format on */
class YosemitechY520_Cond : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY520_Cond object.
     *
     * @param parentSense The parent YosemitechY520 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y520Cond".
     */
    explicit YosemitechY520_Cond(YosemitechY520* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y520_COND_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y520_COND_VAR_NUM,
                   (uint8_t)Y520_COND_RESOLUTION, Y520_COND_VAR_NAME,
                   Y520_COND_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY520_Cond object.
     *
     * @note This must be tied with a parent YosemitechY520 before it can be
     * used.
     */
    YosemitechY520_Cond()
        : Variable((const uint8_t)Y520_COND_VAR_NUM,
                   (uint8_t)Y520_COND_RESOLUTION, Y520_COND_VAR_NAME,
                   Y520_COND_UNIT_NAME, Y520_COND_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY520_Cond object - no action needed.
     */
    ~YosemitechY520_Cond() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y520_temp) from a
 * [Yosemitech Y520-A 4-electrode conductivity sensor](@ref sensor_y520).
 *
 * @ingroup sensor_y520
 */
/* clang-format on */
class YosemitechY520_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY520_Temp object.
     *
     * @param parentSense The parent YosemitechY520 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y520Temp".
     */
    explicit YosemitechY520_Temp(YosemitechY520* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y520_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y520_TEMP_VAR_NUM,
                   (uint8_t)Y520_TEMP_RESOLUTION, Y520_TEMP_VAR_NAME,
                   Y520_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY520_Temp object.
     *
     * @note This must be tied with a parent YosemitechY520 before it can be
     * used.
     */
    YosemitechY520_Temp()
        : Variable((const uint8_t)Y520_TEMP_VAR_NUM,
                   (uint8_t)Y520_TEMP_RESOLUTION, Y520_TEMP_VAR_NAME,
                   Y520_TEMP_UNIT_NAME, Y520_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY520_Temp object - no action needed.
     */
    ~YosemitechY520_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY520_H_
