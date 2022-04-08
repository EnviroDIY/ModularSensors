/**
 * @file YosemitechY551.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY551 sensor subclass and the variable
 * subclasses YosemitechY551_COD, YosemitechY551_Temp, and
 * YosemitechY551_Turbidity.
 *
 * These are for the Yosemitech Y551 COD sensor with wiper.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y551 Yosemitech Y551 UV245/COD Sensor
 * Classes for the Yosemitech Y551 UV245/COD sensor with wiper.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_Y551_datasheet Sensor Datasheet
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y551-UV254-COD_Modbus_v2020-05-11.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y551_ctor Sensor Constructor
 * {{ @ref YosemitechY551::YosemitechY551 }}
 *
 * ___
 * @section sensor_y551_examples Example Code
 * The Yosemitech Y551 UV245/COD sensor is used in the @menulink{yosemitech_y551} example.
 *
 * @menusnip{yosemitech_y551}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY551_H_
#define SRC_SENSORS_YOSEMITECHY551_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y551 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y551 can report 2 values.
#define Y551_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define Y551_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_y551_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y551
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// <500ms for response, but need >1000ms to load capcitors for brush & measure.
#define Y551_WARM_UP_TIME_MS 1000
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 2sec in manual & confirmed by testing.
#define Y551_STABILIZATION_TIME_MS 2000
/// @brief Sensor::_measurementTime_ms; the Y551 takes ~2000ms to complete a
/// measurement according to manual, but testing shows ~1s for a new number
/// but 4-12s to elimniate memory effects, potentially from internal averaging.
#define Y551_MEASUREMENT_TIME_MS 4000
/**@}*/

/**
 * @anchor sensor_y551_cod
 * @name Carbon Oxygen Demand
 * The COD variable from a Yosemitch Y551
 * - Range is:
 *     - 0.75 to 370 mg/L COD (equiv. KHP)
 *     - 0.2 to 150 mg/L TOC (equiv. KHP)
 * - Accuracy is not reported on sensor datasheet
 *
 * {{ @ref YosemitechY551_COD::YosemitechY551_COD }}
 */
/**@{*/
/// @brief Decimals places in string representation; cod should have 2 -
/// resolution is 0.01 mg/L COD.
#define Y551_COD_RESOLUTION 2
/// @brief Sensor variable number; COD is stored in sensorValues[0].
#define Y551_COD_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "COD"
#define Y551_COD_VAR_NAME "COD"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter" (mg/L)
#define Y551_COD_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "Y551COD"
#define Y551_COD_DEFAULT_CODE "Y551COD"
/**@}*/

/**
 * @anchor sensor_y551_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y551
 * - Range is 5°C to + 45°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY551_Temp::YosemitechY551_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define Y551_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y551_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y551_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y551_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y551Temp"
#define Y551_TEMP_DEFAULT_CODE "Y551Temp"
/**@}*/

/**
 * @anchor sensor_y551_turb
 * @name Turbidity
 * The turbidity variable from a Yosemitch Y551
 * - Range is 0.1~1000 NTU
 * - Accuracy is ＜5% or 0.3NTU
 *
 * {{ @ref YosemitechY551_Turbidity::YosemitechY551_Turbidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; turbidity should have 2 -
/// resolution is 0.01 NTU.
#define Y551_TURB_RESOLUTION 2
/// @brief Sensor variable number; turbidity is stored in sensorValues[2].
#define Y551_TURB_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define Y551_TURB_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "nephelometricTurbidityUnit" (NTU)
#define Y551_TURB_UNIT_NAME "nephelometricTurbidityUnit"
/// @brief Default variable short code; "Y551Turbidity"
#define Y551_TURB_DEFAULT_CODE "Y551Turbidity"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y551-B UV254/COD sensor with wiper](@ref sensor_y551).
 *
 * @ingroup sensor_y551
 */
/* clang-format on */
class YosemitechY551 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y551 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y551.
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
    YosemitechY551(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y551,
                           "YosemitechY551", Y551_NUM_VARIABLES,
                           Y551_WARM_UP_TIME_MS, Y551_STABILIZATION_TIME_MS,
                           Y551_MEASUREMENT_TIME_MS, Y551_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY551::YosemitechY551
     */
    YosemitechY551(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y551,
                           "YosemitechY551", Y551_NUM_VARIABLES,
                           Y551_WARM_UP_TIME_MS, Y551_STABILIZATION_TIME_MS,
                           Y551_MEASUREMENT_TIME_MS, Y551_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y551 object
     */
    ~YosemitechY551() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [carbon oxygen demand (COD) output](@ref sensor_y551_cod) from a
 * [Yosemitech Y551-B UV254/COD sensor with wiper](@ref sensor_y551).
 *
 * @ingroup sensor_y551
 */
/* clang-format on */
class YosemitechY551_COD : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY551_COD object.
     *
     * @param parentSense The parent YosemitechY551 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y551COD".
     */
    explicit YosemitechY551_COD(YosemitechY551* parentSense,
                                const char*     uuid    = "",
                                const char*     varCode = Y551_COD_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y551_COD_VAR_NUM,
                   (uint8_t)Y551_COD_RESOLUTION, Y551_COD_VAR_NAME,
                   Y551_COD_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY551_COD object.
     *
     * @note This must be tied with a parent YosemitechY551 before it can be
     * used.
     */
    YosemitechY551_COD()
        : Variable((const uint8_t)Y551_COD_VAR_NUM,
                   (uint8_t)Y551_COD_RESOLUTION, Y551_COD_VAR_NAME,
                   Y551_COD_UNIT_NAME, Y551_COD_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY551_COD object - no action needed.
     */
    ~YosemitechY551_COD() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y551_temp) from a
 * [Yosemitech Y551-B UV254/COD sensor with wiper](@ref sensor_y551).
 *
 * @ingroup sensor_y551
 */
/* clang-format on */
class YosemitechY551_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY551_Temp object.
     *
     * @param parentSense The parent YosemitechY551 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y551Temp".
     */
    explicit YosemitechY551_Temp(YosemitechY551* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y551_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y551_TEMP_VAR_NUM,
                   (uint8_t)Y551_TEMP_RESOLUTION, Y551_TEMP_VAR_NAME,
                   Y551_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY551_Temp object.
     *
     * @note This must be tied with a parent YosemitechY551 before it can be
     * used.
     */
    YosemitechY551_Temp()
        : Variable((const uint8_t)Y551_TEMP_VAR_NUM,
                   (uint8_t)Y551_TEMP_RESOLUTION, Y551_TEMP_VAR_NAME,
                   Y551_TEMP_UNIT_NAME, Y551_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY551_Temp object - no action needed.
     */
    ~YosemitechY551_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_y551_turb) from a
 * [Yosemitech Y551-B UV254/COD sensor with wiper](@ref sensor_y551).
 *
 * @ingroup sensor_y551
 */
/* clang-format on */
class YosemitechY551_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY551_Turbidity object.
     *
     * @param parentSense The parent YosemitechY551 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y551Turbidity".
     */
    explicit YosemitechY551_Turbidity(
        YosemitechY551* parentSense, const char* uuid = "",
        const char* varCode = Y551_TURB_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y551_TURB_VAR_NUM,
                   (uint8_t)Y551_TURB_RESOLUTION, Y551_TURB_VAR_NAME,
                   Y551_TURB_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY551_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY551 before it can be
     * used.
     */
    YosemitechY551_Turbidity()
        : Variable((const uint8_t)Y551_TURB_VAR_NUM,
                   (uint8_t)Y551_TURB_RESOLUTION, Y551_TURB_VAR_NAME,
                   Y551_TURB_UNIT_NAME, Y551_TURB_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY551_Turbidity object - no action needed.
     */
    ~YosemitechY551_Turbidity() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY551_H_
