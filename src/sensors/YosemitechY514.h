/**
 * @file YosemitechY514.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY514 sensor subclass and the variable
 * subclasses YosemitechY514_Chlorophyll and YosemitechY514_Temp.
 *
 * These are for the Yosemitech Y514 chlorophyll sensor with wiper.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y514 Yosemitech Y514 Wipered Chlorophyll Sensor
 * Classes for the Yosemitech Y514 chlorophyll sensor with wiper.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y514_datasheet Sensor Datasheet
 * - [Y514 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y514-Chlorophyl+Wiper_UserManual-v1.0.pdf)
 * - [Y515 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y515-Chlorophyll_UserManual-v1.0_en.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y514-Chlorophyl+Wiper-v1.6_ModbusInstructions.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y514_ctor Sensor Constructor
 * {{ @ref YosemitechY514::YosemitechY514 }}
 *
 * ___
 * @section sensor_y514_examples Example Code
 * The Yosemitech Y514 chlorophyll sensor is used in the @menulink{yosemitech_y514}
 * example.
 *
 * @menusnip{yosemitech_y514}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY514_H_
#define SRC_SENSORS_YOSEMITECHY514_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y514 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y514 can report 2 values.
#define Y514_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y514_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y514_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y514
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power - 1.3
/// seconds (1300ms).
#define Y514_WARM_UP_TIME_MS 1300
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 8sec (8000ms).
#define Y514_STABILIZATION_TIME_MS 8000
/// @brief Sensor::_measurementTime_ms; the Y514 takes ~2000ms to complete a
/// measurement.
#define Y514_MEASUREMENT_TIME_MS 2000
/**@}*/

/**
 * @anchor sensor_y514_chloro
 * @name Chlorophyll Concentration
 * The chlorophyll concentration variable from a Yosemitch Y514
 * - Range is 0 to 400 µg/L or 0 to 100 RFU
 * - Accuracy is ± 1 %
 *
 * {{ @ref YosemitechY514_Chlorophyll::YosemitechY514_Chlorophyll }}
 */
/**@{*/
/// @brief Decimals places in string representation; chlorophyll concentration
/// should have 1 - resolution is 0.1 µg/L / 0.1 RFU.
#define Y514_CHLORO_RESOLUTION 1
/// @brief Sensor variable number; chlorophyll concentration is stored in
/// sensorValues[0].
#define Y514_CHLORO_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "chlorophyllFluorescence"
#define Y514_CHLORO_VAR_NAME "chlorophyllFluorescence"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microgramPerLiter" (µg/L)
#define Y514_CHLORO_UNIT_NAME "microgramPerLiter"
/// @brief Default variable short code; "Y514Chloro"
#define Y514_CHLORO_DEFAULT_CODE "Y514Chloro"
/**@}*/

/**
 * @anchor sensor_y514_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y514
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY514_Temp::YosemitechY514_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y514_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y514_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y514_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y514_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y514Temp"
#define Y514_TEMP_DEFAULT_CODE "Y514Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y514 sensor](@ref sensor_y514).
 *
 * @ingroup sensor_y514
 */
/* clang-format on */
class YosemitechY514 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y514 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y514.
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
    YosemitechY514(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y514,
                           "YosemitechY514", Y514_NUM_VARIABLES,
                           Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS,
                           Y514_MEASUREMENT_TIME_MS, Y514_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY514::YosemitechY514
     */
    YosemitechY514(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y514,
                           "YosemitechY514", Y514_NUM_VARIABLES,
                           Y514_WARM_UP_TIME_MS, Y514_STABILIZATION_TIME_MS,
                           Y514_MEASUREMENT_TIME_MS, Y514_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y514 object
     */
    ~YosemitechY514() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [chlorophyll concentration output](@ref sensor_y514_chloro) from a
 * [Yosemitech Y514-A chlorophyll sensor with wiper](@ref sensor_y514).
 *
 * @ingroup sensor_y514
 */
/* clang-format on */
class YosemitechY514_Chlorophyll : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY514_Chlorophyll object.
     *
     * @param parentSense The parent YosemitechY514 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y514Chloro".
     */
    explicit YosemitechY514_Chlorophyll(
        YosemitechY514* parentSense, const char* uuid = "",
        const char* varCode = Y514_CHLORO_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y514_CHLORO_VAR_NUM,
                   (uint8_t)Y514_CHLORO_RESOLUTION, Y514_CHLORO_VAR_NAME,
                   Y514_CHLORO_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY514_Chlorophyll object.
     *
     * @note This must be tied with a parent YosemitechY514 before it can be
     * used.
     */
    YosemitechY514_Chlorophyll()
        : Variable((const uint8_t)Y514_CHLORO_VAR_NUM,
                   (uint8_t)Y514_CHLORO_RESOLUTION, Y514_CHLORO_VAR_NAME,
                   Y514_CHLORO_UNIT_NAME, Y514_CHLORO_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY514_Chlorophyll() object - no action
     * needed.
     */
    ~YosemitechY514_Chlorophyll() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y514_temp) from a
 * [Yosemitech Y514-A chlorophyll sensor with wiper](@ref sensor_y514).
 *
 * @ingroup sensor_y514
 */
/* clang-format on */
class YosemitechY514_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY514_Temp object.
     *
     * @param parentSense The parent YosemitechY514 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y514Temp".
     */
    explicit YosemitechY514_Temp(YosemitechY514* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y514_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y514_TEMP_VAR_NUM,
                   (uint8_t)Y514_TEMP_RESOLUTION, Y514_TEMP_VAR_NAME,
                   Y514_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY514_Temp object.
     *
     * @note This must be tied with a parent YosemitechY514 before it can be
     * used.
     */
    YosemitechY514_Temp()
        : Variable((const uint8_t)Y514_TEMP_VAR_NUM,
                   (uint8_t)Y514_TEMP_RESOLUTION, Y514_TEMP_VAR_NAME,
                   Y514_TEMP_UNIT_NAME, Y514_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY514_Temp object - no action needed.
     */
    ~YosemitechY514_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY514_H_
