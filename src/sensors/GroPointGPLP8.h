/**
 * @file GroPointGPLP8.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Contains the GroPointGPLP8 sensor subclass and the variable
 * subclasses GroPointGPLP8_Moist and GroPointGPLP8_Temp
 *
 * These are for the GroPoint Profile GPLP-8 Eight-Segment Soil Moisture
 * and Temperature Profiling Probe.
 *
 * This depends on the GroPointParent super class.
 *
 * Documentation for the Modbus Protocol commands and responses can be found
 * within the documentation in the GroPointModbus library at:
 * https://github.com/EnviroDIY/GroPointModbus
 *
 * More detailed infromation on each variable can be found in the documentation
 * for the individual sensor probes
 */
/* clang-format off */
/**
 * @defgroup sensor_gplp8 GroPoint Profile GPLP-8 Soil Moisture & Temperature
 * Profiling Probe. Classes for the GroPoint Profile GPLP-8 Soil Moisture &
 * Temperature Probe.
 *
 * @ingroup gropoint_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_gplp8_datasheet Sensor Datasheet
 * - [GroPoint Profile User Manual](https://www.gropoint.com/s/2625-N-T-GroPoint-Profile-User-Manual-V113.pdf), including Modbus Instructions.
 * - [GroPoint Profile Technical Info](https://www.gropoint.com/s/GroPoint-Profile-Technical-Info.pdf)
 * *
 * @section sensor_gplp8_ctor Sensor Constructor
 * {{ @ref GroPointGPLP8::GroPointGPLP8 }}
 *
 * ___
 * @section sensor_gplp8_examples Example Code
 * The GPLP-8 Probe is used in the @menulink{gro_point_gplp8} example.
 *
 * @menusnip{gro_point_gplp8}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_GROPOINTGPLP8_H_
#define SRC_SENSORS_GROPOINTGPLP8_H_

// Included Dependencies
#include "sensors/GroPointParent.h"

/** @ingroup sensor_gplp8 */
/**@{*/

/**
 * @anchor sensor_gplp8_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by GPLP8
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the GPLP8 can report 8 values.
#define GPLP8_NUM_VARIABLES 21
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define GPLP8_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_gplp8_timing
 * @name Sensor Timing
 * The sensor timing for a GroPoint Profile GPLP-8
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; time before sensor responds after power - 1.6
 * seconds (1600ms).
 *
 * This is the time for communication to begin.
 */
#define GPLP8_WARM_UP_TIME_MS 350
/**
 * @brief Sensor::_stabilizationTime_ms; the GPLP-8 is stable after 100 ms.
 *
 */
#define GPLP8_STABILIZATION_TIME_MS 100
/// @brief Sensor::_measurementTime_ms; the GPLP-8 takes ~200 ms to complete a
/// measurement.
#define GPLP8_MEASUREMENT_TIME_MS 200
/**@}*/

/**
 * @anchor sensor_gplp8_moist
 * @name Moisture
 * The volumetric soil moisture variable from a GroPoint Profile GPLP-8
 * - Range is 0% to 50% volumetric water content
 * - Accuracy is ± 1%
 *
 * {{ @ref GroPointGPLP8_Moist::GroPointGPLP8_Moist }}
 */
/**@{*/
/// @brief Decimals places in string representation; soil moisture should have 1
/// - resolution is 0.1 %.
#define GPLP8_MOIST_RESOLUTION 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
#define GPLP8_MOIST_VAR_NAME "volumetricWaterContent"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
#define GPLP8_MOIST_UNIT_NAME "percent"
/// @brief Default variable short code; "GPLP8Moist"
#define GPLP8_MOIST_DEFAULT_CODE "GPLP8Moist"
/**@}*/

/**
 * @anchor sensor_gplp8_temp
 * @name Temperature
 * The temperature variable from a GroPoint Profile GPLP-8
 * - Range is -20°C to + 70°C
 * - Accuracy is ± 0.5°C
 *
 * {{ @ref GroPointGPLP8_Temp::GroPointGPLP8_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define GPLP8_TEMP_RESOLUTION 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define GPLP8_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define GPLP8_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "GPLP8Temp"
#define GPLP8_TEMP_DEFAULT_CODE "GPLP8Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [GroPoint Profile GPLP8 probe](@ref sensor_gplp8).
 *
 * @ingroup sensor_gplp8
 */
/* clang-format on */
class GroPointGPLP8 : public GroPointParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new GroPoint GPLP8 object.
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the GPLP-8.
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
    GroPointGPLP8(byte modbusAddress, Stream* stream, int8_t powerPin,
                  int8_t powerPin2 = -1, int8_t enablePin = -1,
                  uint8_t measurementsToAverage = 1)
        : GroPointParent(modbusAddress, stream, powerPin, powerPin2, enablePin,
                         measurementsToAverage, GPLP8, "GroPointGPLP8",
                         GPLP8_NUM_VARIABLES, GPLP8_WARM_UP_TIME_MS,
                         GPLP8_STABILIZATION_TIME_MS, GPLP8_MEASUREMENT_TIME_MS,
                         GPLP8_INC_CALC_VARIABLES) {}
    /**
     * @copydoc GroPointGPLP8::GroPointGPLP8
     */
    GroPointGPLP8(byte modbusAddress, Stream& stream, int8_t powerPin,
                  int8_t powerPin2 = -1, int8_t enablePin = -1,
                  uint8_t measurementsToAverage = 1)
        : GroPointParent(modbusAddress, stream, powerPin, powerPin2, enablePin,
                         measurementsToAverage, GPLP8, "GroPointGPLP8",
                         GPLP8_NUM_VARIABLES, GPLP8_WARM_UP_TIME_MS,
                         GPLP8_STABILIZATION_TIME_MS, GPLP8_MEASUREMENT_TIME_MS,
                         GPLP8_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the GroPoint GPLP8 object
     */
    ~GroPointGPLP8() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [soil moisture output](@ref sensor_gplp8_moist) from a
 * [GroPoint Profile GPLP8 probe](@ref sensor_gplp8).
 *
 * @ingroup sensor_gplp8
 */
/* clang-format on */
class GroPointGPLP8_Moist : public Variable {
 public:
    /**
     * @brief Construct a new GroPointGPLP8_Moist object.
     *
     * @param parentSense The parent GroPointGPLP8 providing the result
     * values.
     * @param sensorVarNum The position the variable result holds in the
     * variable result array.  The GroPoint GPLP8 can have up to 8 soil moisture
     * results. When creating the variable for soil moisture, you must specify
     * the output number from the sensor.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "GPLP8Moist".
     */
    explicit GroPointGPLP8_Moist(GroPointGPLP8* parentSense,
                                 const uint8_t  sensorVarNum,
                                 const char*    uuid = "",
                                 const char* varCode = GPLP8_MOIST_DEFAULT_CODE)
        : Variable(parentSense, sensorVarNum, (uint8_t)GPLP8_MOIST_RESOLUTION,
                   GPLP8_MOIST_VAR_NAME, GPLP8_MOIST_UNIT_NAME, varCode, uuid) {
    }
    /**
     * @brief Construct a new GroPointGPLP8_Moist object.
     *
     * @param sensorVarNum The position the variable result holds in the
     * variable result array.  The GroPoint GPLP8 can have up to 8 soil moisture
     * results. When creating the variable for soil moisture, you must specify
     * the output number from the sensor.
     *
     * @note This must be tied with a parent GroPointGPLP8 before it can be
     * used.
     */
    GroPointGPLP8_Moist(const uint8_t sensorVarNum)
        : Variable(sensorVarNum, (uint8_t)GPLP8_MOIST_RESOLUTION,
                   GPLP8_MOIST_VAR_NAME, GPLP8_MOIST_UNIT_NAME,
                   GPLP8_MOIST_DEFAULT_CODE) {}
    /**
     * @brief Destroy the GroPointGPLP8_Moist object - no action needed.
     */
    ~GroPointGPLP8_Moist() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_gplp8_temp) from a
 * [GroPoint Profile GPLP8 probe](@ref sensor_gplp8).
 *
 * @ingroup sensor_gplp8
 */
/* clang-format on */
class GroPointGPLP8_Temp : public Variable {
 public:
    /**
     * @brief Construct a new GroPointGPLP8_Temp object.
     *
     * @param parentSense The parent GroPointGPLP8 providing the result
     * values.
     * @param sensorVarNum The position the variable result holds in the
     * variable result array.  The GroPoint GPLP8 can have up to 8 temperature
     * results. When creating the variable for temperature, you must specify the
     * output number from the sensor.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "GPLP8Temp".
     */
    explicit GroPointGPLP8_Temp(GroPointGPLP8* parentSense,
                                const uint8_t  sensorVarNum,
                                const char*    uuid = "",
                                const char* varCode = GPLP8_TEMP_DEFAULT_CODE)
        : Variable(parentSense, sensorVarNum, (uint8_t)GPLP8_TEMP_RESOLUTION,
                   GPLP8_TEMP_VAR_NAME, GPLP8_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new GroPointGPLP8_Temp object.
     *
     * @param sensorVarNum The position the variable result holds in the
     * variable result array.  The GroPoint GPLP8 can have up to 8 temperature
     * results. When creating the variable for temperature, you must specify the
     * output number from the sensor.
     *
     * @note This must be tied with a parent GroPointGPLP8 before it can be
     * used.
     */
    GroPointGPLP8_Temp(const uint8_t sensorVarNum)
        : Variable(sensorVarNum, (uint8_t)GPLP8_TEMP_RESOLUTION,
                   GPLP8_TEMP_VAR_NAME, GPLP8_TEMP_UNIT_NAME,
                   GPLP8_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the GroPointGPLP8_Temp object - no action needed.
     */
    ~GroPointGPLP8_Temp() {}
};

/**@}*/
#endif  // SRC_SENSORS_GROPOINTGPLP8_H_
