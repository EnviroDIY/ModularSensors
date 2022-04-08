/**
 * @file YosemitechY560.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY560 sensor subclass and the variable
 * subclasses YosemitechY560_NH4_N, YosemitechY560_Temp, and
 * YosemitechY560_pH.
 *
 * These are for the Yosemitech Y560 Ammonium sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y560 Yosemitech Y560 Ammonium Sensor
 * Classes for the Yosemitech Y560 Ammonium sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y560_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y560-NH4_UserManual_v1.0.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y560-NH4_Modbus_v2020-05-11.pdf)
 *
 * @note The reported resolution (32 bit) gives more precision than significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y560_ctor Sensor Constructor
 * {{ @ref YosemitechY560::YosemitechY560 }}
 *
 * ___
 * @section sensor_y560_examples Example Code
 * The Yosemitech Y560 Ammonium sensor is used in the @menulink{yosemitech_y560} example.
 *
 * @menusnip{yosemitech_y560}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY560_H_
#define SRC_SENSORS_YOSEMITECHY560_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y560 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y560 can report 3 values.
#define Y560_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y560_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y560_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y560
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// <200ms for response, but need 2-10s to load capcitors for brush & measure.
#define Y560_WARM_UP_TIME_MS 7000
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading 20s in manual but this includes 15s for brushing.
/// Setting to 20s to allow for 5s after brushing completes, based on testing.
#define Y560_STABILIZATION_TIME_MS 18000
/// @brief Sensor::_measurementTime_ms; the Y560 takes 2s to complete a
/// measurement according to manual, but testing shows ~1.5s for a new number.
#define Y560_MEASUREMENT_TIME_MS 1500
/**@}*/

/**
 * @anchor sensor_y560_nh4
 * @name NH4_N
 * The NH4_N variable from a Yosemitch Y560
 * - Range is 0-10 or 0-100 mg/L NH4-N
 * - Accuracy is ±(5% + 0.2 mg/L)
 *
 * {{ @ref YosemitechY560_NH4_N::YosemitechY560_NH4_N }}
 */
/**@{*/
/// @brief Decimals places in string representation; NH4_N should have 1 -
/// resolution is 0.1 mg/L.
#define Y560_NH4_N_RESOLUTION 1
/// @brief Sensor variable number; NH4_N is stored in sensorValues[0].
#define Y560_NH4_N_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "nitrogen_NH4"
#define Y560_NH4_N_VAR_NAME "nitrogen_NH4"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter"
#define Y560_NH4_N_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "Y560NH4_N"
#define Y560_NH4_N_DEFAULT_CODE "Y560NH4_N"
/**@}*/

/**
 * @anchor sensor_y560_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y560
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY560_Temp::YosemitechY560_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y560_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y560_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y560_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y560_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y560Temp"
#define Y560_TEMP_DEFAULT_CODE "Y560Temp"
/**@}*/

/**
 * @anchor sensor_y560_pH
 * @name pH
 * The pH variable from a Yosemitch Y560
 * - Range is 2 to 12 pH units
 * - Accuracy is ± 0.1 pH units
 *
 * {{ @ref YosemitechY560_pH::YosemitechY560_pH }}
 */
/**@{*/
/// @brief Decimals places in string representation; pH should have 2 -
/// resolution is 0.01 pH units.
#define Y560_PH_RESOLUTION 2
/// @brief Sensor variable number; pH is stored in sensorValues[2].
#define Y560_PH_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/); "pH"
#define Y560_PH_VAR_NAME "pH"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "pH"
/// (dimensionless pH units)
#define Y560_PH_UNIT_NAME "pH"
/// @brief Default variable short code; "Y560pH"
#define Y560_PH_DEFAULT_CODE "Y560pH"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y560-A digital pH sensor](@ref sensor_y560).
 *
 * @ingroup sensor_y560
 */
/* clang-format on */
class YosemitechY560 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y560 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y560.
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
    YosemitechY560(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y560,
                           "YosemitechY560", Y560_NUM_VARIABLES,
                           Y560_WARM_UP_TIME_MS, Y560_STABILIZATION_TIME_MS,
                           Y560_MEASUREMENT_TIME_MS, Y560_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY560::YosemitechY560
     */
    YosemitechY560(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y560,
                           "YosemitechY560", Y560_NUM_VARIABLES,
                           Y560_WARM_UP_TIME_MS, Y560_STABILIZATION_TIME_MS,
                           Y560_MEASUREMENT_TIME_MS, Y560_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y560 object
     */
    ~YosemitechY560() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [NH4_N output](@ref sensor_y560_nh4)
 * from a  [Yosemitech Y560-Ammonium sensor](@ref sensor_y560).
 *
 * @ingroup sensor_y560
 */
/* clang-format on */
class YosemitechY560_NH4_N : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY560_NH4_N object.
     *
     * @param parentSense The parent YosemitechY560 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y560NH4_N".
     */
    explicit YosemitechY560_NH4_N(YosemitechY560* parentSense,
                                  const char*     uuid = "",
                                  const char* varCode = Y560_NH4_N_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y560_NH4_N_VAR_NUM,
                   (const uint8_t)Y560_NH4_N_RESOLUTION, Y560_NH4_N_VAR_NAME,
                   Y560_NH4_N_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY560_NH4_N object.
     *
     * @note This must be tied with a parent YosemitechY560 before it can be
     * used.
     */
    YosemitechY560_NH4_N()
        : Variable((const uint8_t)Y560_NH4_N_VAR_NUM,
                   (const uint8_t)Y560_NH4_N_RESOLUTION, Y560_NH4_N_VAR_NAME,
                   Y560_NH4_N_UNIT_NAME, Y560_NH4_N_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY560_NH4_N object - no action needed.
     */
    ~YosemitechY560_NH4_N() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y560_temp) from a
 * [Yosemitech Y560-Ammonium sensor](@ref sensor_y560).
 *
 * @ingroup sensor_y560
 */
/* clang-format on */
class YosemitechY560_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY560_Temp object.
     *
     * @param parentSense The parent YosemitechY560 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y560Temp".
     */
    explicit YosemitechY560_Temp(YosemitechY560* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y560_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y560_TEMP_VAR_NUM,
                   (uint8_t)Y560_TEMP_RESOLUTION, Y560_TEMP_VAR_NAME,
                   Y560_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY560_Temp object.
     *
     * @note This must be tied with a parent YosemitechY560 before it can be
     * used.
     */
    YosemitechY560_Temp()
        : Variable((const uint8_t)Y560_TEMP_VAR_NUM,
                   (uint8_t)Y560_TEMP_RESOLUTION, Y560_TEMP_VAR_NAME,
                   Y560_TEMP_UNIT_NAME, Y560_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY560_Temp object - no action needed.
     */
    ~YosemitechY560_Temp() {}
};


/* clang-format off */
/**
* @brief The Variable sub-class used for the
* [pH output](@ref sensor_y560_pH)
* from a  [Yosemitech Y560-Ammonium sensor](@ref sensor_y560).
*
* @ingroup sensor_y560
*/
/* clang-format on */
class YosemitechY560_pH : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY560_pH object.
     *
     * @param parentSense The parent YosemitechY560 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y560pH".
     */
    explicit YosemitechY560_pH(YosemitechY560* parentSense,
                               const char*     uuid    = "",
                               const char*     varCode = Y560_PH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y560_PH_VAR_NUM,
                   (uint8_t)Y560_PH_RESOLUTION, Y560_PH_VAR_NAME,
                   Y560_PH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY560_pH object.
     *
     * @note This must be tied with a parent YosemitechY560 before it can be
     * used.
     */
    YosemitechY560_pH()
        : Variable((const uint8_t)Y560_PH_VAR_NUM, (uint8_t)Y560_PH_RESOLUTION,
                   Y560_PH_VAR_NAME, Y560_PH_UNIT_NAME, Y560_PH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY560_pH object - no action needed.
     */
    ~YosemitechY560_pH() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY560_H_
