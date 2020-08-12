/**
 * @file KellerNanolevel.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com> and Neil
 * Hancock Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the KellerNanolevel sensor subclass and the
 * KellerNanolevel_Pressure, KellerNanolevel_Temp, and KellerNanolevel_Height
 * variable subclasses.
 *
 * These are for Keller Nanolevel capacitive level sensors.
 *
 * @defgroup nanolevel_group Keller Nanolevel
 * Classes for the @ref nanolevel_page
 *
 * @copydoc nanolevel_page
 *
 * @ingroup keller_group
 */
/* clang-format off */
/**
 * @page nanolevel_page Keller Nanolevel
 *
 * @tableofcontents
 *
 * @section nanolevel_datasheet Sensor Datasheet
 * [Manual](https://www.kelleramerica.com/manuals-and-software/manuals/Keller_America_Users_Guide.pdf)
 * [Datasheet](https://www.kelleramerica.com/pdf-library/Very%20Low%20Range%20Submersible%20Transmitter%20Nanolevel.pdf)
 *
 * @section nanolevel_sensor the Keller Nanolevel Sensor
 * @ctor_doc{KellerNanolevel, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection nanolevel_timing Sensor Timing
 *
 * @section nanolevel_pressure Pressure Output
 * @variabledoc{KellerNanolevel,Pressure}
 *   - Range is 0 to 300mbar
 *   - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.002%
 *   - Reported as millibar (mb)
 *   - Default variable code is kellerNanoPress
 *
 * @section nanolevel_temp Temperature Output
 * @variabledoc{KellerNanolevel,Temp}
 *   - Range is 10°C to 50°C
 *   - Accuracy is not specified in the sensor datasheet
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.01°C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is kellerNanoTemp
 *
 * @section nanolevel_height Height Output
 * @variabledoc{KellerNanolevel,Height}
 *   - Range is 0 to 120 inches
 *   - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.002%
 *   - Reported as meters (m)
 *   - Default variable code is kellerNanoHeight
 *
 *
 * ___
 * @section nanolevel_examples Example Code
 * The Keller Nanolevel is used in the @menulink{nanolevel} example.
 *
 * @menusnip{nanolevel}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_KELLERNANOLEVEL_H_
#define SRC_SENSORS_KELLERNANOLEVEL_H_

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines

/// Sensor::_warmUpTime_ms; the Nanolevel warms up in 500ms.
#define NANOLEVEL_WARM_UP_TIME_MS 500
/// Sensor::_stabilizationTime_ms; the Nanolevel is stable after 5000ms.
#define NANOLEVEL_STABILIZATION_TIME_MS 5000
/**
 * @brief Sensor::_measurementTime_ms; the Nanolevel takes 1500ms to complete a
 * measurement.
 */
#define NANOLEVEL_MEASUREMENT_TIME_MS 1500

/// Decimals places in string representation; pressure should have 5.
#define NANOLEVEL_PRESSURE_RESOLUTION 5

/// Decimals places in string representation; temperature should have 2.
#define NANOLEVEL_TEMP_RESOLUTION 2

/// Decimals places in string representation; height should have 4.
#define NANOLEVEL_HEIGHT_RESOLUTION 4


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Keller nanolevel sensor](@ref nanolevel_page)
 *
 * @ingroup nanolevel_group
 */
/* clang-format on */
class KellerNanolevel : public KellerParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Keller Nanolevel
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication.  See
     * [notes](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)
     * for more information on what streams can be used.
     * @param powerPin The pin on the mcu controlling power to the Nanolevel.
     * Use -1 if it is continuously powered.
     * - The Nanolevel requires a 9-28 VDC power supply.
     * @param powerPin2 The pin on the mcu controlling power to the RS485
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param enablePin The pin on the mcu controlling the direction enable on
     * the RS485 adapter, if necessary; use -1 or omit if not applicable.  An
     * RS485 adapter with integrated flow control is strongly recommended.
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
 * [gauge pressure (vented and barometric pressure corrected) output](@ref nanolevel_pressure)
 * from a [Keller Nanolevel ceramic capacitive level transmitter](@ref nanolevel_page).
 *
 * @ingroup nanolevel_group
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
     * optional with a default value of kellerNanoPress
     */
    explicit KellerNanolevel_Pressure(KellerNanolevel* parentSense,
                                      const char*      uuid = "",
                                      const char* varCode   = "kellerNanoPress")
        : Variable(parentSense, (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", varCode, uuid) {}
    /**
     * @brief Construct a new KellerNanolevel_Pressure object.
     *
     * @note This must be tied with a parent KellerNanolevel before it can be
     * used.
     */
    KellerNanolevel_Pressure()
        : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)NANOLEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", "kellerNanoPress") {}
    /**
     * @brief Destroy the KellerNanolevel_Pressure object - no action needed.
     */
    ~KellerNanolevel_Pressure() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref nanolevel_temp) from a
 * [Keller Nanolevel ceramic capacitive level transmitter](@ref nanolevel_page).
 *
 * @ingroup nanolevel_group
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
    explicit KellerNanolevel_Temp(KellerNanolevel* parentSense,
                                  const char*      uuid    = "",
                                  const char*      varCode = "kellerNanoTemp")
        : Variable(parentSense, (const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)NANOLEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new KellerNanolevel_Temp object.
     *
     * @note This must be tied with a parent KellerNanolevel before it can be
     * used.
     */
    KellerNanolevel_Temp()
        : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)NANOLEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "kellerNanoTemp") {}
    /**
     * @brief Destroy the KellerNanolevel_Temp object - no action needed.
     */
    ~KellerNanolevel_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [gauge height (water level with regard to an arbitrary gage datum) output](@ref nanolevel_height) from a
 * [Keller Nanolevel ceramic capacitive level transmitter](@ref nanolevel_page).
 *
 * @ingroup nanolevel_group
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
    explicit KellerNanolevel_Height(KellerNanolevel* parentSense,
                                    const char*      uuid = "",
                                    const char* varCode   = "kellerNanoHeight")
        : Variable(parentSense, (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   varCode, uuid) {}
    /**
     * @brief Construct a new KellerNanolevel_Height object.
     *
     * @note This must be tied with a parent KellerNanolevel before it can be
     * used.
     */
    KellerNanolevel_Height()
        : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)NANOLEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   "kellerNanoHeight") {}
    /**
     * @brief Destroy the KellerNanolevel_Height object - no action needed.
     */
    ~KellerNanolevel_Height() {}
};

#endif  // SRC_SENSORS_KELLERNANOLEVEL_H_
