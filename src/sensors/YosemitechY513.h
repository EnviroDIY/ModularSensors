/**
 * @file YosemitechY513.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Contains the YosemitechY513 sensor subclass and the variable
 * subclasses YosemitechY513_BGA and YosemitechY513_Temp.
 *
 * These are for the Yosemitech Y513 Blue Green Algae (BGA) sensor with wiper.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y513 Yosemitech Y513 self-cleaning Blue Green Algae (BGA) sensor.
 * Classes for the Yosemitech Y513 Blue Green Algae (BGA) sensor with wiper.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y513_datasheet Sensor Datasheet
 * - [Y513 product webpage](https://e.yosemitech.com/CHL/Y513-A.html)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y513_ctor Sensor Constructor
 * {{ @ref YosemitechY513::YosemitechY513 }}
 *
 * ___
 * @section sensor_y513_examples Example Code
 * The Yosemitech Y513 Blue Green Algae sensor is used in the @menulink{yosemitech_y513}
 * example.
 *
 * @menusnip{yosemitech_y513}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY513_H_
#define SRC_SENSORS_YOSEMITECHY513_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y513 */
/**@{*/

/**
 * @anchor sensor_y513_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by a Yosemitch Y514
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the Y513 can report 2 values.
#define Y513_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y513_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_y513_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y513
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power - 1.3
/// seconds (1300ms).
#define Y513_WARM_UP_TIME_MS 1300
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 8sec (8000ms).
#define Y513_STABILIZATION_TIME_MS 8000
/// @brief Sensor::_measurementTime_ms; the Y513 takes ~2000ms to complete a
/// measurement.
#define Y513_MEASUREMENT_TIME_MS 2000
/**@}*/

/**
 * @anchor sensor_y513_bga
 * @name Chlorophyll Concentration
 * The blue green algae concentration variable from a Yosemitch Y513
 * - Range is 0 to 400 µg/L or 0 to 100 RFU
 * - Accuracy is ± 1 %
 *
 * {{ @ref YosemitechY513_BGA::YosemitechY513_BGA }}
 */
/**@{*/
/// @brief Decimals places in string representation; blue green algae
/// concentration should have 1 - resolution is 0.1 µg/L / 0.1 RFU.
#define Y513_BGA_RESOLUTION 1
/// @brief Sensor variable number; blue green algae concentration is stored in
/// sensorValues[0].
#define Y513_BGA_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "blue_GreenAlgae_Cyanobacteria_Phycocyanin"
#define Y513_BGA_VAR_NAME "Blue-green algae (cyanobacteria), phycocyanin"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "countPerMilliliter" (cells/mL)
#define Y513_BGA_UNIT_NAME "countPerMilliliter"
/// @brief Default variable short code; "Y513BGA"
#define Y513_BGA_DEFAULT_CODE "Y513BGA"
/**@}*/

/**
 * @anchor sensor_y513_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y513
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY513_Temp::YosemitechY513_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y513_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y513_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y513_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y513_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y513Temp"
#define Y513_TEMP_DEFAULT_CODE "Y513Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y513 sensor](@ref sensor_y513).
 *
 * @ingroup sensor_y513
 */
/* clang-format on */
class YosemitechY513 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y513 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y513.
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
    YosemitechY513(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y513,
                           "YosemitechY513", Y513_NUM_VARIABLES,
                           Y513_WARM_UP_TIME_MS, Y513_STABILIZATION_TIME_MS,
                           Y513_MEASUREMENT_TIME_MS, Y513_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY513::YosemitechY513
     */
    YosemitechY513(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y513,
                           "YosemitechY513", Y513_NUM_VARIABLES,
                           Y513_WARM_UP_TIME_MS, Y513_STABILIZATION_TIME_MS,
                           Y513_MEASUREMENT_TIME_MS, Y513_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y513 object
     */
    ~YosemitechY513() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [blue green algae concentration output](@ref sensor_y513_bga) from a
 * [Yosemitech Y513 Blue Green Algae (BGA) sensor with wiper](@ref sensor_y513).
 *
 * @ingroup sensor_y513
 */
/* clang-format on */
class YosemitechY513_BGA : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY513_BGA object.
     *
     * @param parentSense The parent YosemitechY513 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y513BGA".
     */
    explicit YosemitechY513_BGA(YosemitechY513* parentSense,
                                const char*     uuid    = "",
                                const char*     varCode = Y513_BGA_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)Y513_BGA_VAR_NUM,
                   (uint8_t)Y513_BGA_RESOLUTION, Y513_BGA_VAR_NAME,
                   Y513_BGA_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY513_BGA object.
     *
     * @note This must be tied with a parent YosemitechY513 before it can be
     * used.
     */
    YosemitechY513_BGA()
        : Variable((uint8_t)Y513_BGA_VAR_NUM, (uint8_t)Y513_BGA_RESOLUTION,
                   Y513_BGA_VAR_NAME, Y513_BGA_UNIT_NAME,
                   Y513_BGA_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY513_BGA() object - no action
     * needed.
     */
    ~YosemitechY513_BGA() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y513_temp) from a
 * [Yosemitech Y513-A blue green algae sensor with wiper](@ref sensor_y513).
 *
 * @ingroup sensor_y513
 */
/* clang-format on */
class YosemitechY513_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY513_Temp object.
     *
     * @param parentSense The parent YosemitechY513 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y513Temp".
     */
    explicit YosemitechY513_Temp(YosemitechY513* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y513_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)Y513_TEMP_VAR_NUM,
                   (uint8_t)Y513_TEMP_RESOLUTION, Y513_TEMP_VAR_NAME,
                   Y513_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY513_Temp object.
     *
     * @note This must be tied with a parent YosemitechY513 before it can be
     * used.
     */
    YosemitechY513_Temp()
        : Variable((uint8_t)Y513_TEMP_VAR_NUM, (uint8_t)Y513_TEMP_RESOLUTION,
                   Y513_TEMP_VAR_NAME, Y513_TEMP_UNIT_NAME,
                   Y513_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY513_Temp object - no action needed.
     */
    ~YosemitechY513_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY513_H_
