/**
 * @file YosemitechY550.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY550 sensor subclass and the variable
 * subclasses YosemitechY550_COD, YosemitechY550_Temp, and
 * YosemitechY550_Turbidity.
 *
 * These are for the Yosemitech Y550 COD sensor with wiper.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y550 Yosemitech Y550 UV245/COD Sensor
 * Classes for the Yosemitech Y550 UV245/COD sensor with wiper.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y550_datasheet Sensor Datasheet
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y550-COD-UV254-1.5_ModbusInstruction-en.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y550_ctor Sensor Constructor
 * {{ @ref YosemitechY550::YosemitechY550 }}
 *
 * ___
 * @section sensor_y550_examples Example Code
 * The Yosemitech Y550 UV245/COD sensor is used in the @menulink{y550} example.
 *
 * @menusnip{y550}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY550_H_
#define SRC_SENSORS_YOSEMITECHY550_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

// Sensor Specific Defines
/** @ingroup sensor_y550 */
/**@{*/

/// @brief Sensor::_numReturnedValues; the Y550 can report 2 values.
#define Y550_NUM_VARIABLES 2

/**
 * @anchor sensor_y550_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y550
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// 1500ms.
#define Y550_WARM_UP_TIME_MS 1500
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 2sec.
#define Y550_STABILIZATION_TIME_MS 2000
/// @brief Sensor::_measurementTime_ms; the Y550 takes ~2000ms to complete a
/// measurement.
#define Y550_MEASUREMENT_TIME_MS 2000
/**@}*/

/**
 * @anchor sensor_y550_cod
 * @name Carbon Oxygen Demand
 * The COD variable from a Yosemitch Y550
 * - Range is:
 *     - 0.75 to 370 mg/L COD (equiv. KHP)
 *     - 0.2 to 150 mg/L TOC (equiv. KHP)
 * - Accuracy is not reported on sensor datasheet
 *
 * {{ @ref YosemitechY550_COD::YosemitechY550_COD }}
 */
/**@{*/
/// @brief Decimals places in string representation; cod should have 2 -
/// resolution is 0.01 mg/L COD.
#define Y550_COD_RESOLUTION 2
/// @brief Sensor variable number; COD is stored in sensorValues[0].
#define Y550_COD_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "COD"
#define Y550_COD_VAR_NAME "COD"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter" (mg/L)
#define Y550_COD_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "Y550COD"
#define Y550_COD_DEFAULT_CODE "Y550COD"
/**@}*/

/**
 * @anchor sensor_y550_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y550
 * - Range is 5°C to + 45°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY550_Temp::YosemitechY550_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define Y550_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y550_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y550_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y550_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y550Temp"
#define Y550_TEMP_DEFAULT_CODE "Y550Temp"
/**@}*/

/**
 * @anchor sensor_y550_turb
 * @name Turbidity
 * The turbidity variable from a Yosemitch Y550
 * - Range is 0.1~1000 NTU
 * - Accuracy is ＜5% or 0.3NTU
 *
 * {{ @ref YosemitechY550_Turbidity::YosemitechY550_Turbidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; turbidity should have 2 -
/// resolution is 0.01 NTU.
#define Y550_TURB_RESOLUTION 2
/// @brief Sensor variable number; turbidity is stored in sensorValues[2].
#define Y550_TURB_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "turbidity"
#define Y550_TURB_VAR_NAME "turbidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "nephelometricTurbidityUnit" (NTU)
#define Y550_TURB_UNIT_NAME "nephelometricTurbidityUnit"
/// @brief Default variable short code; "Y550Turbidity"
#define Y550_TURB_DEFAULT_CODE "Y550Turbidity"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref sensor_y550).
 *
 * @ingroup sensor_y550
 */
/* clang-format on */
class YosemitechY550 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y550 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Y550.
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
    YosemitechY550(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y550,
                           "YosemitechY550", Y550_NUM_VARIABLES,
                           Y550_WARM_UP_TIME_MS, Y550_STABILIZATION_TIME_MS,
                           Y550_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc YosemitechY550::YosemitechY550
     */
    YosemitechY550(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y550,
                           "YosemitechY550", Y550_NUM_VARIABLES,
                           Y550_WARM_UP_TIME_MS, Y550_STABILIZATION_TIME_MS,
                           Y550_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Yosemitech Y550 object
     */
    ~YosemitechY550() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [carbon oxygen demand (COD) output](@ref sensor_y550_cod) from a
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref sensor_y550).
 *
 * @ingroup sensor_y550
 */
/* clang-format on */
class YosemitechY550_COD : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY550_COD object.
     *
     * @param parentSense The parent YosemitechY550 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y550COD".
     */
    explicit YosemitechY550_COD(YosemitechY550* parentSense,
                                const char*     uuid    = "",
                                const char*     varCode = Y550_COD_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y550_COD_VAR_NUM,
                   (uint8_t)Y550_COD_RESOLUTION, Y550_COD_VAR_NAME,
                   Y550_COD_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY550_COD object.
     *
     * @note This must be tied with a parent YosemitechY550 before it can be
     * used.
     */
    YosemitechY550_COD()
        : Variable((const uint8_t)Y550_COD_VAR_NUM,
                   (uint8_t)Y550_COD_RESOLUTION, Y550_COD_VAR_NAME,
                   Y550_COD_UNIT_NAME, Y550_COD_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY550_COD object - no action needed.
     */
    ~YosemitechY550_COD() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y550_temp) from a
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref sensor_y550).
 *
 * @ingroup sensor_y550
 */
/* clang-format on */
class YosemitechY550_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY550_Temp object.
     *
     * @param parentSense The parent YosemitechY550 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y550Temp".
     */
    explicit YosemitechY550_Temp(YosemitechY550* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y550_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y550_TEMP_VAR_NUM,
                   (uint8_t)Y550_TEMP_RESOLUTION, Y550_TEMP_VAR_NAME,
                   Y550_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY550_Temp object.
     *
     * @note This must be tied with a parent YosemitechY550 before it can be
     * used.
     */
    YosemitechY550_Temp()
        : Variable((const uint8_t)Y550_TEMP_VAR_NUM,
                   (uint8_t)Y550_TEMP_RESOLUTION, Y550_TEMP_VAR_NAME,
                   Y550_TEMP_UNIT_NAME, Y550_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY550_Temp object - no action needed.
     */
    ~YosemitechY550_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [turbidity output](@ref sensor_y550_turb) from a
 * [Yosemitech Y550-B UV254/COD sensor with wiper](@ref sensor_y550).
 *
 * @ingroup sensor_y550
 */
/* clang-format on */
class YosemitechY550_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY550_Turbidity object.
     *
     * @param parentSense The parent YosemitechY550 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y550Turbidity".
     */
    explicit YosemitechY550_Turbidity(
        YosemitechY550* parentSense, const char* uuid = "",
        const char* varCode = Y550_TURB_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y550_TURB_VAR_NUM,
                   (uint8_t)Y550_TURB_RESOLUTION, Y550_TURB_VAR_NAME,
                   Y550_TURB_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY550_Turbidity object.
     *
     * @note This must be tied with a parent YosemitechY550 before it can be
     * used.
     */
    YosemitechY550_Turbidity()
        : Variable((const uint8_t)Y550_TURB_VAR_NUM,
                   (uint8_t)Y550_TURB_RESOLUTION, Y550_TURB_VAR_NAME,
                   Y550_TURB_UNIT_NAME, Y550_TURB_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY550_Turbidity object - no action needed.
     */
    ~YosemitechY550_Turbidity() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY550_H_
