/**
 * @file YosemitechY533.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY533 sensor subclass and the variable
 * subclasses YosemitechY533_ORP and YosemitechY533_Temp.
 *
 * These are for the Yosemitech Y533 ORP (Oxygen Reduction Potential) sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y533 Yosemitech Y533 ORP Sensor
 * Classes for the Yosemitech Y533 oxidation/reduction potential (ORP) sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y533_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH_UserManual-v1.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y532-pH-ORP-v1.7_ModbusInstructions.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y533_ctor Sensor Constructor
 * {{ @ref YosemitechY533::YosemitechY533 }}
 *
 * ___
 * @section sensor_y533_examples Example Code
 * The Yosemitech Y533 ORP sensor is used in the @menulink{yosemitech_y533} example.
 *
 * @menusnip{yosemitech_y533}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY533_H_
#define SRC_SENSORS_YOSEMITECHY533_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y533 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y533 can report 2 values.
#define Y533_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y533_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y533_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y533
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// 500ms.
#define Y533_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 4.5sec (4500ms).
#define Y533_STABILIZATION_TIME_MS 4500
/// @brief Sensor::_measurementTime_ms; the Y533 takes ~1800ms to complete a
/// measurement.
#define Y533_MEASUREMENT_TIME_MS 1800
/**@}*/

/**
 * @anchor sensor_y533_orp
 * @name pH
 * The ORP variable from a Yosemitch Y533
 * - Range is -999 to 999 mV
 * - Accuracy is ±20 mV
 * - Resolution is 1 mV
 *
 * {{ @ref YosemitechY533_ORP::YosemitechY533_ORP }}
 */
/**@{*/
/// @brief Decimals places in string representation; ph should have 2 -
/// resolution is 1 mV units.
#define Y533_ORP_RESOLUTION 0
/// @brief Sensor variable number; ORP is stored in sensorValues[0].
#define Y533_ORP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
/// NOTE: ORP should be added to ODM2 CVs, as ORP refers to a specific measure
///  of electron potential relative to a silver chloride reference electrode.
#define Y533_ORP_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "millivolt"
/// (mV)
#define Y533_ORP_UNIT_NAME "millivolt"
/// @brief Default variable short code; "Y533ORP"
#define Y533_ORP_DEFAULT_CODE "Y533ORP"
/**@}*/

/**
 * @anchor sensor_y533_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y533
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY533_Temp::YosemitechY533_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y533_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y533_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y533_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y533_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y533Temp"
#define Y533_TEMP_DEFAULT_CODE "Y533Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y533 ORP sensor](@ref sensor_y533).
 *
 * @ingroup sensor_y533
 */
/* clang-format on */
class YosemitechY533 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y533 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y533.
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
    YosemitechY533(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y533,
                           "YosemitechY533", Y533_NUM_VARIABLES,
                           Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS,
                           Y533_MEASUREMENT_TIME_MS, Y533_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY533::YosemitechY533
     */
    YosemitechY533(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y533,
                           "YosemitechY533", Y533_NUM_VARIABLES,
                           Y533_WARM_UP_TIME_MS, Y533_STABILIZATION_TIME_MS,
                           Y533_MEASUREMENT_TIME_MS, Y533_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y533 object
     */
    ~YosemitechY533() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pH output](@ref sensor_y533_orp)
 * from a [Yosemitech Y533 ORP sensor](@ref sensor_y533).
 *
 * @ingroup sensor_y533
 */
/* clang-format on */
class YosemitechY533_ORP : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_pH object.
     *
     * @param parentSense The parent YosemitechY533 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y533pH".
     */
    explicit YosemitechY533_ORP(YosemitechY533* parentSense,
                                const char*     uuid    = "",
                                const char*     varCode = Y533_ORP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y533_ORP_VAR_NUM,
                   (uint8_t)Y533_ORP_RESOLUTION, Y533_ORP_VAR_NAME,
                   Y533_ORP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY533_ORP object.
     *
     * @note This must be tied with a parent YosemitechY533 before it can be
     * used.
     */
    YosemitechY533_ORP()
        : Variable((const uint8_t)Y533_ORP_VAR_NUM,
                   (uint8_t)Y533_ORP_RESOLUTION, Y533_ORP_VAR_NAME,
                   Y533_ORP_UNIT_NAME, Y533_ORP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY533_ORP object - no action needed.
     */
    ~YosemitechY533_ORP() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y533_temp) from a
 * [Yosemitech Y533 ORP sensor](@ref sensor_y533).
 *
 * @ingroup sensor_y533
 */
/* clang-format on */
class YosemitechY533_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY533_Temp object.
     *
     * @param parentSense The parent YosemitechY533 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y533Temp".
     */
    explicit YosemitechY533_Temp(YosemitechY533* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y533_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y533_TEMP_VAR_NUM,
                   (uint8_t)Y533_TEMP_RESOLUTION, Y533_TEMP_VAR_NAME,
                   Y533_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY533_Temp object.
     *
     * @note This must be tied with a parent YosemitechY533 before it can be
     * used.
     */
    YosemitechY533_Temp()
        : Variable((const uint8_t)Y533_TEMP_VAR_NUM,
                   (uint8_t)Y533_TEMP_RESOLUTION, Y533_TEMP_VAR_NAME,
                   Y533_TEMP_UNIT_NAME, Y533_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY533_Temp object - no action needed.
     */
    ~YosemitechY533_Temp() {}
};

/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY533_H_
