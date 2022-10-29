/**
 * @file YosemitechY700.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Contains the YosemitechY700 sensor subclass and the variable
 * subclasses YosemitechY700_Pressure and YosemitechY700_Temp.
 *
 * These are for the Yosemitech Y700 Pressure sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y700 Yosemitech Y700 Pressure Sensor
 * Classes for the Yosemitech Y700 pressure sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y700_datasheet Sensor Datasheet
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y700_ctor Sensor Constructor
 * {{ @ref YosemitechY700::YosemitechY700 }}
 *
 * ___
 * @section sensor_y700_examples Example Code
 * The Yosemitech Y700 pressure sensor is used in the @menulink{yosemitech_y700}
 * example.
 *
 * @menusnip{yosemitech_y700}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY700_H_
#define SRC_SENSORS_YOSEMITECHY700_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y700 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y700 can report 2 values.
#define Y700_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y700_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y700_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y700
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// 1000 ms.
#define Y700_WARM_UP_TIME_MS 1000
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - Y700 takes 4 s to get stability <1 mm, 
/// but 12 s for <0.1 mm. If highest precision is required, increase to 12000.
#define Y700_STABILIZATION_TIME_MS 4000
/// @brief Sensor::_measurementTime_ms; the Y700 takes <1 s for new values.
///  but >1 s for values that don't seem autocorrelated.  
#define Y700_MEASUREMENT_TIME_MS 1000
/**@}*/

/**
 * @anchor sensor_y700_pres
 * @name Pressure
 * The Pressure variable from a Yosemitch Y700
 * - Range is 0mH2O, ~2mH2O, or 100mH2O, depending on model
 * - Accuracy is ± 0.1 % Full Scale
 *
 * {{ @ref YosemitechY700_Pressure::YosemitechY700_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; Pressure should have 1
/// - resolution is 0.01 mm.
#define Y700_PRES_RESOLUTION 2
/// @brief Sensor variable number; pressure is stored in sensorValues[0].
#define Y700_PRES_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "pressureGauge"
#define Y700_PRES_VAR_NAME "pressureGauge"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "millimeterOfWater" (mmH2O)
#define Y700_PRES_UNIT_NAME "millimeterOfWater"
/// @brief Default variable short code; "Y700Pres"
#define Y700_PRES_DEFAULT_CODE "Y700Pres"
/**@}*/

/**
 * @anchor sensor_y700_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y700
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY700_Temp::YosemitechY700_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y700_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y700_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y700_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y700_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y700Temp"
#define Y700_TEMP_DEFAULT_CODE "Y700Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y700 pressure sensor](@ref sensor_y700).
 *
 * @ingroup sensor_y700
 */
/* clang-format on */
class YosemitechY700 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y700 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y700.
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
    YosemitechY700(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y700,
                           "YosemitechY700", Y700_NUM_VARIABLES,
                           Y700_WARM_UP_TIME_MS, Y700_STABILIZATION_TIME_MS,
                           Y700_MEASUREMENT_TIME_MS, Y700_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY700::YosemitechY700
     */
    YosemitechY700(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y700,
                           "YosemitechY700", Y700_NUM_VARIABLES,
                           Y700_WARM_UP_TIME_MS, Y700_STABILIZATION_TIME_MS,
                           Y700_MEASUREMENT_TIME_MS, Y700_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y700 object
     */
    ~YosemitechY700() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pressure output](@ref sensor_y700_pres) from a
 * [Yosemitech Y700 pressure sensor](@ref sensor_y700).
 *
 * @ingroup sensor_y700
 */
/* clang-format on */
class YosemitechY700_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY700_Pressure object.
     *
     * @param parentSense The parent YosemitechY700 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y700Pres".
     */
    explicit YosemitechY700_Pressure(YosemitechY700* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y700_PRES_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y700_PRES_VAR_NUM,
                   (uint8_t)Y700_PRES_RESOLUTION, Y700_PRES_VAR_NAME,
                   Y700_PRES_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY700_Pressure object.
     *
     * @note This must be tied with a parent YosemitechY700 before it can be
     * used.
     */
    YosemitechY700_Pressure()
        : Variable((const uint8_t)Y700_PRES_VAR_NUM,
                   (uint8_t)Y700_PRES_RESOLUTION, Y700_PRES_VAR_NAME,
                   Y700_PRES_UNIT_NAME, Y700_PRES_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY700_Pressure object - no action needed.
     */
    ~YosemitechY700_Pressure() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y700_temp) from a
 * [Yosemitech Y700 pressure sensor](@ref sensor_y700).
 *
 * @ingroup sensor_y700
 */
/* clang-format on */
class YosemitechY700_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY700_Temp object.
     *
     * @param parentSense The parent YosemitechY700 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y700Temp".
     */
    explicit YosemitechY700_Temp(YosemitechY700* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y700_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y700_TEMP_VAR_NUM,
                   (uint8_t)Y700_TEMP_RESOLUTION, Y700_TEMP_VAR_NAME,
                   Y700_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY700_Temp object.
     *
     * @note This must be tied with a parent YosemitechY700 before it can be
     * used.
     */
    YosemitechY700_Temp()
        : Variable((const uint8_t)Y700_TEMP_VAR_NUM,
                   (uint8_t)Y700_TEMP_RESOLUTION, Y700_TEMP_VAR_NAME,
                   Y700_TEMP_UNIT_NAME, Y700_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY700_Temp object - no action needed.
     */
    ~YosemitechY700_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY700_H_
