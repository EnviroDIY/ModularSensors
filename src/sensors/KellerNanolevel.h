/**
 * @file KellerNanolevel.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com> and Neil
 * Hancock Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the KellerNanolevel sensor subclass and the
 * KellerNanolevel_Pressure, KellerNanolevel_Temp, and KellerNanolevel_Height
 * variable subclasses.
 *
 * These are for Keller Nanolevel capacitive level sensors.
 */
/* clang-format off */
/**
 * @defgroup sensor_nanolevel Keller Nanolevel
 * Classes for the Keller Nanolevel capacitive level sensor.
 *
 * @ingroup keller_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * These are for Keller Nanolevel capacitive level sensors.
 *
 * @section sensor_nanolevel_datasheet Sensor Datasheet
 * - [Manual](https://www.kelleramerica.com/manuals-and-software/manuals/Keller_America_Users_Guide.pdf)
 * - [Datasheet](https://www.kelleramerica.com/pdf-library/Very%20Low%20Range%20Submersible%20Transmitter%20Nanolevel.pdf)
 *
 * @section sensor_nanolevel_ctor Sensor Constructor
 * {{ @ref KellerNanolevel::KellerNanolevel }}
 *
 * ___
 * @section sensor_nanolevel_examples Example Code
 * The Keller Nanolevel is used in the @menulink{keller_nanolevel} example.
 *
 * @menusnip{keller_nanolevel}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_KELLERNANOLEVEL_H_
#define SRC_SENSORS_KELLERNANOLEVEL_H_

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines
/** @ingroup sensor_nanolevel */
/**@{*/

/**
 * @anchor sensor_nanolevel_timing
 * @name Sensor Timing
 * The sensor timing for a Keller Nanolevel
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the Nanolevel warms up in 500ms.
#define NANOLEVEL_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; the Nanolevel stabilization takes
/// about 5s (5000ms).
#define NANOLEVEL_STABILIZATION_TIME_MS 5000
/// @brief Sensor::_measurementTime_ms; the Nanolevel takes 1500ms to complete a
/// measurement.
#define NANOLEVEL_MEASUREMENT_TIME_MS 1500
/**@}*/

/**
 * @anchor sensor_nanolevel_pressure
 * @name Pressure
 * The pressure variable from a Keller Nanolevel
 * - Range is 0 to 300mbar
 * - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *
 * {{ @ref KellerNanolevel_Pressure::KellerNanolevel_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; pressure should have 5 -
/// resolution is 0.002%.
#define NANOLEVEL_PRESSURE_RESOLUTION 5
/// @brief Default variable short code; "kellerNanoPress"
#define NANOLEVEL_PRESSURE_DEFAULT_CODE "kellerNanoPress"
/**@}*/

/**
 * @anchor sensor_nanolevel_temp
 * @name Temperature
 * The temperature variable from a Keller Nanolevel
 * - Range is 10°C to 50°C
 * - Accuracy is not specified in the sensor datasheet
 *
 * {{ @ref KellerNanolevel_Temp::KellerNanolevel_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define NANOLEVEL_TEMP_RESOLUTION 2
/// @brief Default variable short code; "kellerNanoTemp"
#define NANOLEVEL_TEMP_DEFAULT_CODE "kellerNanoTemp"
/**@}*/

/**
 * @anchor sensor_nanolevel_height
 * @name Height
 * The height variable from a Keller Nanolevel
 * - Range is 0 to 120 inches
 * - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *
 * {{ @ref KellerNanolevel_Height::KellerNanolevel_Height }}
 */
/**@{*/
/// @brief Decimals places in string representation; height should have 4 -
/// resolution is 0.002%.
#define NANOLEVEL_HEIGHT_RESOLUTION 4
/// @brief Default variable short code; "kellerNanoHeight"
#define NANOLEVEL_HEIGHT_DEFAULT_CODE "kellerNanoHeight"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Keller nanolevel sensor](@ref sensor_nanolevel)
 *
 * @ingroup sensor_nanolevel
 */
/* clang-format on */
class KellerNanolevel : public KellerParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Keller Nanolevel instance
     *
     * @param modbusAddress The modbus address of the Nanolevel.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Nanolevel.
     * Use -1 if it is continuously powered.
     * - The Nanolevel requires a 9-28 VDC power supply.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor. Use -1 or
     * omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.
     * @note An RS485 adapter with integrated flow control is strongly
     * recommended.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    KellerNanolevel(byte modbusAddress, Stream* stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Nanolevel_kellerModel, "KellerNanolevel",
              KELLER_NUM_VARIABLES, NANOLEVEL_WARM_UP_TIME_MS,
              NANOLEVEL_STABILIZATION_TIME_MS, NANOLEVEL_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc KellerNanolevel::KellerNanolevel
     */
    KellerNanolevel(byte modbusAddress, Stream& stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Nanolevel_kellerModel, "KellerNanolevel",
              KELLER_NUM_VARIABLES, NANOLEVEL_WARM_UP_TIME_MS,
              NANOLEVEL_STABILIZATION_TIME_MS, NANOLEVEL_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~KellerNanolevel() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [gauge pressure (vented and barometric pressure corrected) output](@ref sensor_nanolevel_pressure)
 * from a [Keller Nanolevel ceramic capacitive level transmitter](@ref sensor_nanolevel).
 *
 * @ingroup sensor_nanolevel
 */
/* clang-format on */
class KellerNanolevel_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new KellerNanolevel_Pressure object.
     *
     * @param parentSense The parent KellerNanolevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerNanoPress".
     */
    explicit KellerNanolevel_Pressure(
        KellerNanolevel* parentSense, const char* uuid = "",
        const char* varCode = NANOLEVEL_PRESSURE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION,
                   KELLER_PRESSURE_VAR_NAME, KELLER_PRESSURE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new KellerNanolevel_Pressure object.
     *
     * @note This must be tied with a parent KellerNanolevel before it can be
     * used.
     */
    KellerNanolevel_Pressure()
        : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION,
                   KELLER_PRESSURE_VAR_NAME, KELLER_PRESSURE_UNIT_NAME,
                   NANOLEVEL_PRESSURE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the KellerNanolevel_Pressure object - no action needed.
     */
    ~KellerNanolevel_Pressure() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_nanolevel_temp) from a
 * [Keller Nanolevel ceramic capacitive level transmitter](@ref sensor_nanolevel).
 *
 * @ingroup sensor_nanolevel
 */
/* clang-format on */
class KellerNanolevel_Temp : public Variable {
 public:
    /**
     * @brief Construct a new KellerNanolevel_Temp object.
     *
     * @param parentSense The parent KellerNanolevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerNanoTemp".
     */
    explicit KellerNanolevel_Temp(
        KellerNanolevel* parentSense, const char* uuid = "",
        const char* varCode = NANOLEVEL_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)NANOLEVEL_TEMP_RESOLUTION, KELLER_TEMP_VAR_NAME,
                   KELLER_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new KellerNanolevel_Temp object.
     *
     * @note This must be tied with a parent KellerNanolevel before it can be
     * used.
     */
    KellerNanolevel_Temp()
        : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)NANOLEVEL_TEMP_RESOLUTION, KELLER_TEMP_VAR_NAME,
                   KELLER_TEMP_UNIT_NAME, NANOLEVEL_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the KellerNanolevel_Temp object - no action needed.
     */
    ~KellerNanolevel_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [gauge height (water level with regard to an arbitrary gage datum) output](@ref sensor_nanolevel_height) from a
 * [Keller Nanolevel ceramic capacitive level transmitter](@ref sensor_nanolevel).
 *
 * @ingroup sensor_nanolevel
 */
/* clang-format on */
class KellerNanolevel_Height : public Variable {
 public:
    /**
     * @brief Construct a new KellerNanolevel_Height object.
     *
     * @param parentSense The parent KellerNanolevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerNanoHeight".
     */
    explicit KellerNanolevel_Height(
        KellerNanolevel* parentSense, const char* uuid = "",
        const char* varCode = NANOLEVEL_HEIGHT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION, KELLER_HEIGHT_VAR_NAME,
                   KELLER_HEIGHT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new KellerNanolevel_Height object.
     *
     * @note This must be tied with a parent KellerNanolevel before it can be
     * used.
     */
    KellerNanolevel_Height()
        : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION, KELLER_HEIGHT_VAR_NAME,
                   KELLER_HEIGHT_UNIT_NAME, NANOLEVEL_HEIGHT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the KellerNanolevel_Height object - no action needed.
     */
    ~KellerNanolevel_Height() {}
};
/**@}*/
#endif  // SRC_SENSORS_KELLERNANOLEVEL_H_
