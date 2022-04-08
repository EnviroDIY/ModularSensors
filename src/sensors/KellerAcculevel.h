/**
 * @file KellerAcculevel.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the KellerAcculevel sensor subclass and the
 * KellerAcculevel_Pressure, KellerAcculevel_Temp, and KellerAcculevel_Height
 * variable subclasses.
 *
 * These are for Keller Series 30, Class 5, Group 20 sensors using Modbus
 * communication, that are software version 5.20-12.28 and later (i.e. made
 * after the 2012 in the 28th week).
 *
 * Only tested on the Acculevel.
 */
/* clang-format off */
/**
 * @defgroup sensor_acculevel Keller Acculevel
 * Classes for the Keller Acculevel.
 *
 * @ingroup keller_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * These are for Keller Series 30, Class 5, Group 20 sensors using Modbus
 * communication, that are software version 5.20-12.28 and later (i.e. made
 * after the 2012 in the 28th week).
 *
 * Only tested on the Acculevel.
 *
 * @section sensor_acculevel_datasheet Sensor Datasheet
 * - [Manual](https://www.kelleramerica.com/manuals-and-software/manuals/Keller_America_Users_Guide.pdf)
 * - [Datasheet](https://www.kelleramerica.com/pdf-library/High%20Accuracy%20Analog%20Digital%20Submersible%20Level%20Transmitters%20Acculevel.pdf)
 *
 * @section sensor_acculevel_ctor Sensor Constructor
 * {{ @ref KellerAcculevel::KellerAcculevel }}
 *
 * ___
 * @section sensor_acculevel_examples Example Code
 * The Keller Acculevel is used in the @menulink{keller_acculevel} example.
 *
 * @menusnip{keller_acculevel}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_KELLERACCULEVEL_H_
#define SRC_SENSORS_KELLERACCULEVEL_H_

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines
/** @ingroup sensor_acculevel */
/**@{*/

/**
 * @anchor sensor_acculevel_timing
 * @name Sensor Timing
 * The sensor timing for a Keller Acculevel
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the Acculevel takes about 500 ms to respond.
#define ACCULEVEL_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; the Acculevel is stable after about
/// 5s (5000ms).
#define ACCULEVEL_STABILIZATION_TIME_MS 5000
///@brief Sensor::_measurementTime_ms; the Acculevel takes 1500ms to complete a
/// measurement.
#define ACCULEVEL_MEASUREMENT_TIME_MS 1500
/**@}*/

/**
 * @anchor sensor_acculevel_pressure
 * @name Pressure
 * The pressure variable from a Keller Acculevel
 * - Range is 0 to 11 bar
 * - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *
 * {{ @ref KellerAcculevel_Pressure::KellerAcculevel_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; pressure should have 5 -
/// resolution is 0.002%.
#define ACCULEVEL_PRESSURE_RESOLUTION 5
/// @brief Default variable short code; "kellerAccuPress"
#define ACCULEVEL_PRESSURE_DEFAULT_CODE "kellerAccuPress"
/**@}*/

/**
 * @anchor sensor_acculevel_temp
 * @name Temperature
 * The temperature variable from a Keller Acculevel
 * - Range is -10°C to 60°C
 * - Accuracy is not specified in the sensor datasheet
 *
 * {{ @ref KellerAcculevel_Temp::KellerAcculevel_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define ACCULEVEL_TEMP_RESOLUTION 2
/// @brief Default variable short code; "kellerAccuTemp"
#define ACCULEVEL_TEMP_DEFAULT_CODE "kellerAccuTemp"
/**@}*/

/**
 * @anchor sensor_acculevel_height
 * @name Height
 * The height variable from a Keller Acculevel
 * - Range is 0 to 900 feet
 * - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *
 * {{ @ref KellerAcculevel_Height::KellerAcculevel_Height }}
 */
/**@{*/
/// @brief Decimals places in string representation; height should have 4 -
/// resolution is 0.002%.
#define ACCULEVEL_HEIGHT_RESOLUTION 4
/// @brief Default variable short code; "kellerAccuHeight"
#define ACCULEVEL_HEIGHT_DEFAULT_CODE "kellerAccuHeight"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Keller Acculevel sensor](@ref sensor_acculevel).
 *
 * @ingroup sensor_acculevel
 */
/* clang-format on */
class KellerAcculevel : public KellerParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Keller Acculevel
     *
     * @param modbusAddress The modbus address of the Acculevel.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Acculevel.
     * Use -1 if it is continuously powered.
     * - The Acculevel requires a 9-28 VDC power supply.
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
    KellerAcculevel(byte modbusAddress, Stream* stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Acculevel_kellerModel, "KellerAcculevel",
              KELLER_NUM_VARIABLES, ACCULEVEL_WARM_UP_TIME_MS,
              ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc KellerAcculevel::KellerAcculevel
     */
    KellerAcculevel(byte modbusAddress, Stream& stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Acculevel_kellerModel, "KellerAcculevel",
              KELLER_NUM_VARIABLES, ACCULEVEL_WARM_UP_TIME_MS,
              ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~KellerAcculevel() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [gauge pressure (vented and barometric pressure corrected) output](@ref sensor_acculevel_pressure)
 * from a [Keller Acculevel](@ref sensor_acculevel).
 *
 * @ingroup sensor_acculevel
 */
/* clang-format on */
class KellerAcculevel_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Pressure object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerAccuPress".
     */
    explicit KellerAcculevel_Pressure(
        KellerAcculevel* parentSense, const char* uuid = "",
        const char* varCode = ACCULEVEL_PRESSURE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION,
                   KELLER_PRESSURE_VAR_NAME, KELLER_PRESSURE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Pressure object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Pressure()
        : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION,
                   KELLER_PRESSURE_VAR_NAME, KELLER_PRESSURE_UNIT_NAME,
                   ACCULEVEL_PRESSURE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the KellerAcculevel_Pressure object - no action needed.
     */
    ~KellerAcculevel_Pressure() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_acculevel_temp) from a
 * [Keller Acculevel](@ref sensor_acculevel).
 *
 * @ingroup sensor_acculevel
 */
/* clang-format on */
class KellerAcculevel_Temp : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Temp object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerAccuTemp".
     */
    explicit KellerAcculevel_Temp(
        KellerAcculevel* parentSense, const char* uuid = "",
        const char* varCode = ACCULEVEL_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)ACCULEVEL_TEMP_RESOLUTION, KELLER_TEMP_VAR_NAME,
                   KELLER_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Temp object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Temp()
        : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)ACCULEVEL_TEMP_RESOLUTION, KELLER_TEMP_VAR_NAME,
                   KELLER_TEMP_UNIT_NAME, ACCULEVEL_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the KellerAcculevel_Temp object - no action needed.
     */
    ~KellerAcculevel_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [gauge height (water level with regard to an arbitrary gage datum) output](@ref sensor_acculevel_height)
 * from a [Keller Acculevel](@ref sensor_acculevel).
 *
 * @ingroup sensor_acculevel
 */
/* clang-format on */
class KellerAcculevel_Height : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Height object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerAccuHeight".
     */
    explicit KellerAcculevel_Height(
        KellerAcculevel* parentSense, const char* uuid = "",
        const char* varCode = ACCULEVEL_HEIGHT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION, KELLER_HEIGHT_VAR_NAME,
                   KELLER_HEIGHT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Height object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Height()
        : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION, KELLER_HEIGHT_VAR_NAME,
                   KELLER_HEIGHT_UNIT_NAME, ACCULEVEL_HEIGHT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the KellerAcculevel_Height object - no action needed.
     */
    ~KellerAcculevel_Height() {}
};
/**@}*/
#endif  // SRC_SENSORS_KELLERACCULEVEL_H_
