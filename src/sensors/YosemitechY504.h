/**
 * @file YosemitechY504.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the YosemitechY504 sensor subclass and the variable
 * subclasses YosemitechY504_DOpct, YosemitechY504_Temp, and
 * YosemitechY504_DOmgL.
 *
 * These are for the Yosemitech Y502-A or Y504-A optical dissolved oxygen
 * sensor.
 *
 * This depends on the YosemitechParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the YosemitechModbus library at:
 * https://github.com/EnviroDIY/YosemitechModbus
 */
/* clang-format off */
/**
 * @defgroup sensor_y504 Yosemitech Y504 Dissolved Oxygen Sensor
 * Classes for the Yosemitech Y502-A or Y504-A optical dissolved oxygen
 * sensor.
 *
 * @ingroup yosemitech_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_y504_datasheet Sensor Datasheet
 * - [Y504 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y504-DO_UserManual-v1.1.pdf)
 * - [Y504 Manual](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y505-DO_UserManual-v1.2.pdf)
 * - [Modbus Instructions](https://github.com/EnviroDIY/YosemitechModbus/tree/master/doc/Y504-DO-v6.2_ModbusInstructions.pdf)
 *
 * @note The reported resolution (32 bit) gives far more precision than is significant
 * based on the specified accuracy of the sensor, so the resolutions kept in the
 * string representation of the variable values is based on the accuracy not the
 * maximum reported resolution of the sensor.
 *
 * @section sensor_y504_ctor Sensor Constructor
 * {{ @ref YosemitechY504::YosemitechY504 }}
 *
 * ___
 * @section sensor_y504_examples Example Code
 * The Yosemitech Y504 Dissolved Oxygen is used in the @menulink{yosemitech_y504} example.
 *
 * @menusnip{yosemitech_y504}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_YOSEMITECHY504_H_
#define SRC_SENSORS_YOSEMITECHY504_H_

// Included Dependencies
#include "sensors/YosemitechParent.h"

/** @ingroup sensor_y504 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Y504 can report 3 values.
#define Y504_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we calculated DO concentration from the
/// percent saturation and the temperature.
#define Y504_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_y504_timing
 * @name Sensor Timing
 * The sensor timing for a Yosemitch Y504
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; time before sensor responds after power -
/// 375ms.
#define Y504_WARM_UP_TIME_MS 375
/// @brief Sensor::_stabilizationTime_ms; time between "StartMeasurement"
/// command and stable reading - 8sec (8000ms).
#define Y504_STABILIZATION_TIME_MS 8000
/// @brief Sensor::_measurementTime_ms; the Y504 takes ~1700ms to complete a
/// measurement.
#define Y504_MEASUREMENT_TIME_MS 1700
/**@}*/

/**
 * @anchor sensor_y504_dopercent
 * @name Dissolved Oxygen Percent Saturation
 * The dissolved oxygen percent saturation variable from a Yosemitch Y504
 * - Range is 0-20mg/L or 0-200% Saturation
 * - Accuracy is ± 1 %
 *
 * {{ @ref YosemitechY504_DOpct::YosemitechY504_DOpct }}
 */
/**@{*/
/// @brief Decimals places in string representation; dissolved oxygen percent
/// should have 1 - resolution is 0.1%.
#define Y504_DOPCT_RESOLUTION 1
/// @brief Sensor variable number; dissolved oxygen percent is stored in
/// sensorValues[0]
#define Y504_DOPCT_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "oxygenDissolvedPercentOfSaturation"
#define Y504_DOPCT_VAR_NAME "oxygenDissolvedPercentOfSaturation"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter" (mg/L)
#define Y504_DOPCT_UNIT_NAME "percent"
/// @brief Default variable short code; "Y504DOmgL"
#define Y504_DOPCT_DEFAULT_CODE "Y504DOpct"
/**@}*/

/**
 * @anchor sensor_y504_temp
 * @name Temperature
 * The temperature variable from a Yosemitch Y504
 * - Range is 0°C to + 50°C
 * - Accuracy is ± 0.2°C
 *
 * {{ @ref YosemitechY504_Temp::YosemitechY504_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define Y504_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define Y504_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define Y504_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define Y504_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Y504Temp"
#define Y504_TEMP_DEFAULT_CODE "Y504Temp"
/**@}*/

/**
 * @anchor sensor_y504_domgl
 * @name Dissolved Oxygen Concentration
 * The dissolved oxygen concentration variable from a Yosemitch Y504
 * - Range is 0-20mg/L or 0-200% Saturation
 * - Accuracy is ± 1 %
 *
 * {{ @ref YosemitechY504_DOmgL::YosemitechY504_DOmgL }}
 */
/**@{*/
/// @brief Decimals places in string representation; dissolved oxygen
/// concentration should have 2 - resolution is 0.01 mg/L.
#define Y504_DOMGL_RESOLUTION 2
/// @brief Sensor variable number; dissolved oxygen concentration is stored in
/// sensorValues[2]
#define Y504_DOMGL_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "oxygenDissolved"
#define Y504_DOMGL_VAR_NAME "oxygenDissolved"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter" (mg/L)
#define Y504_DOMGL_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "Y504DOmgL"
#define Y504_DOMGL_DEFAULT_CODE "Y504DOmgL"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Yosemitech Y502-A or Y504-A optical dissolved oxygen sensors](@ref sensor_y504).
 *
 * @ingroup sensor_y504
 */
/* clang-format on */
class YosemitechY504 : public YosemitechParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Yosemitech Y504 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Y504.
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
    YosemitechY504(byte modbusAddress, Stream* stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y504,
                           "YosemitechY504", Y504_NUM_VARIABLES,
                           Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS,
                           Y504_MEASUREMENT_TIME_MS, Y504_INC_CALC_VARIABLES) {}
    /**
     * @copydoc YosemitechY504::YosemitechY504
     */
    YosemitechY504(byte modbusAddress, Stream& stream, int8_t powerPin,
                   int8_t powerPin2 = -1, int8_t enablePin = -1,
                   uint8_t measurementsToAverage = 1)
        : YosemitechParent(modbusAddress, stream, powerPin, powerPin2,
                           enablePin, measurementsToAverage, Y504,
                           "YosemitechY504", Y504_NUM_VARIABLES,
                           Y504_WARM_UP_TIME_MS, Y504_STABILIZATION_TIME_MS,
                           Y504_MEASUREMENT_TIME_MS, Y504_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Yosemitech Y504 object
     */
    ~YosemitechY504() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen percent saturation output](@ref sensor_y504_dopercent) from a
 * [Yosemitech Y502-A or Y504-A optical dissolved oxygen sensor](@ref sensor_y504).
 *
 * @ingroup sensor_y504
 */
/* clang-format on */
class YosemitechY504_DOpct : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY504_DOpct object.
     *
     * @param parentSense The parent YosemitechY504 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y504DOpct".
     */
    explicit YosemitechY504_DOpct(YosemitechY504* parentSense,
                                  const char*     uuid = "",
                                  const char* varCode = Y504_DOPCT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y504_DOPCT_VAR_NUM,
                   (uint8_t)Y504_DOPCT_RESOLUTION, Y504_DOPCT_VAR_NAME,
                   Y504_DOPCT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY504_DOpct object.
     *
     * @note This must be tied with a parent YosemitechY504 before it can be
     * used.
     */
    YosemitechY504_DOpct()
        : Variable((const uint8_t)Y504_DOPCT_VAR_NUM,
                   (uint8_t)Y504_DOPCT_RESOLUTION, Y504_DOPCT_VAR_NAME,
                   Y504_DOPCT_UNIT_NAME, Y504_DOPCT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY504_DOpct object - no action needed.
     */
    ~YosemitechY504_DOpct() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_y504_temp) from a
 * [Yosemitech Y504 optical dissolved oxygen sensor](@ref sensor_y504).
 *
 * @ingroup sensor_y504
 */
/* clang-format on */
class YosemitechY504_Temp : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY504_Temp object.
     *
     * @param parentSense The parent YosemitechY504 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y504Temp".
     */
    explicit YosemitechY504_Temp(YosemitechY504* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = Y504_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y504_TEMP_VAR_NUM,
                   (uint8_t)Y504_TEMP_RESOLUTION, Y504_TEMP_VAR_NAME,
                   Y504_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY504_Temp object.
     *
     * @note This must be tied with a parent YosemitechY504 before it can be
     * used.
     */
    YosemitechY504_Temp()
        : Variable((const uint8_t)Y504_TEMP_VAR_NUM,
                   (uint8_t)Y504_TEMP_RESOLUTION, Y504_TEMP_VAR_NAME,
                   Y504_TEMP_UNIT_NAME, Y504_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY504_Temp object - no action needed.
     */
    ~YosemitechY504_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref sensor_y504_domgl) from a
 * [Yosemitech Y502-A or Y504-A optical dissolved oxygen sensor](@ref sensor_y504).
 *
 * @ingroup sensor_y504
 */
/* clang-format on */
class YosemitechY504_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new YosemitechY504_DOmgL object.
     *
     * @param parentSense The parent YosemitechY504 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Y504DOmgL".
     */
    explicit YosemitechY504_DOmgL(YosemitechY504* parentSense,
                                  const char*     uuid = "",
                                  const char* varCode = Y504_DOMGL_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)Y504_DOMGL_VAR_NUM,
                   (uint8_t)Y504_DOMGL_RESOLUTION, Y504_DOMGL_VAR_NAME,
                   Y504_DOMGL_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new YosemitechY504_DOmgL object.
     *
     * @note This must be tied with a parent YosemitechY504 before it can be
     * used.
     */
    YosemitechY504_DOmgL()
        : Variable((const uint8_t)Y504_DOMGL_VAR_NUM,
                   (uint8_t)Y504_DOMGL_RESOLUTION, Y504_DOMGL_VAR_NAME,
                   Y504_DOMGL_UNIT_NAME, Y504_DOMGL_DEFAULT_CODE) {}
    /**
     * @brief Destroy the YosemitechY504_DOmgL object - no action needed.
     */
    ~YosemitechY504_DOmgL() {}
};
/**@}*/
#endif  // SRC_SENSORS_YOSEMITECHY504_H_
