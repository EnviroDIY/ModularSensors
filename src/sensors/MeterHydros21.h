/**
 * @file MeterHydros21.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MeterHydros21 subclass of the SDI12Sensors class along
 * with the variable subclasses MeterHydros21_Cond, MeterHydros21_Temp, and
 * MeterHydros21_Depth.
 *
 * These are used for the Meter Hydros 21.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 *
 * @note  This is identical to DecagonCTD.h in everything but the names.
 */
/* clang-format off */
/**
 * @defgroup sensor_hydros21 Meter Hydros 21
 * Classes for the Meter Hydros 21 conductivity, temperature, and depth sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_hydros21_intro Introduction
 *
 * > A compact 3.4 cm diameter sensor that fits into tight spaces, the
 * > HYDROS 21 is a low-cost, durable, and easy-to-use tool for monitoring EC,
 * > temperature, and depth in both groundwater and surface water.
 *
 * @note Meter Environmental was formerly known as Decagon Devices and sold a
 * very similar sensor to the current Hydros 21 as the CTD-10.
 *
 * The Hydros 21 is implemented as a sub-classes of the SDI12Sensors class.
 * It requires a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V.
 *
 * @note  This is identical to Meter Hydros 21 in everything but the names.
 *
 * @warning Coming from the factory, METER sensors are set at SDI-12 address
 * '0'.  They also output a "DDI" serial protocol string on each power up.
 * This library *disables the DDI output string* on all newer METER sensors
 * that support disabling it.  After using a METER sensor with ModularSensors,
 * you will need to manually re-enable the DDI output if you wish to use it.
 *
 * @section sensor_hydros21_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Hydros
 * 21 can be found at: http://library.metergroup.com/Manuals/13869_CTD_Web.pdf
 *
 * @section sensor_hydros21_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_hydros21_ctor Sensor Constructor
 * {{ @ref MeterHydros21::MeterHydros21 }}
 *
 * ___
 * @section sensor_hydros21_examples Example Code
 * The Meter Hydros21 is used in the @menulink{meter_hydros21} example.
 *
 * @menusnip{meter_hydros21}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_METERHYDROS21_H_
#define SRC_SENSORS_METERHYDROS21_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_hydros21 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Hydros 21 can report 3 values.
#define HYDROS21_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define HYDROS21_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_hydros21_timing
 * @name Sensor Timing
 * The sensor timing for a Meter Hydros 21
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; maximum warm-up time in SDI-12 mode: 500ms
#define HYDROS21_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; the Hydros 21 is stable as soon as it
/// warms up (0ms stabilization).
#define HYDROS21_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; maximum measurement duration: 1000ms.
 *
 * @note Older versions of the Hydro took less time (~500ms) to take a
 * measurement, but the current version, as of 5/2/2022, takes a full second
 * (1000ms) to consistently give results in concurrent measurement mode.
 * Somewhat strangely, it does give results in ~450ms when operated
 * non-concurrently. Depending on how many sensors you have, you may have faster
 * results by forcing non-concurrent operation with the newest versions of the
 * Hydros21. To do this compile with the [build flag](@ref sdi12_group_flags)
 * `-D MS_SDI12_NON_CONCURRENT`.
 */
#define HYDROS21_MEASUREMENT_TIME_MS 1000
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The Hydros 21 requires no extra time.
#define HYDROS21_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_hydros21_cond
 * @name Conductivity
 * The conductivity variable from a Meter Hydros 21
 * - Range is 0 – 120 mS/cm (bulk)
 * - Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
 *
 * {{ @ref MeterHydros21_Cond::MeterHydros21_Cond }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; conductivity should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.001 mS/cm = 1 µS/cm
 */
#define HYDROS21_COND_RESOLUTION 1
/// @brief Sensor variable number; conductivity is stored in sensorValues[2].
#define HYDROS21_COND_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "specificConductance"
#define HYDROS21_COND_VAR_NAME "specificConductance"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microsiemenPerCentimeter" (µS/cm)
#define HYDROS21_COND_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "Hydros21cond"
#define HYDROS21_COND_DEFAULT_CODE "Hydros21cond"
/**@}*/

/**
 * @anchor sensor_hydros21_temp
 * @name Temperature
 * The temperature variable from a Meter Hydros 21
 * - Range is -11°C to +49°C
 * - Accuracy is ±1°C
 *
 * {{ @ref MeterHydros21_Temp::MeterHydros21_Temp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; temperature should have 2.
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging  - resolution is 0.1°C
 */
#define HYDROS21_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define HYDROS21_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define HYDROS21_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define HYDROS21_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "Hydros21temp"
#define HYDROS21_TEMP_DEFAULT_CODE "Hydros21temp"
/**@}*/

/**
 * @anchor sensor_hydros21_depth
 * @name Water Depth
 * The water depth variable from a Meter Hydros 21
 * - Range is 0 to 5 m or 0 to 10 m, depending on model
 * - Accuracy is ±0.05% of full scale
 *
 * {{ @ref MeterHydros21_Depth::MeterHydros21_Depth }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; depth should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 2 mm
 */
#define HYDROS21_DEPTH_RESOLUTION 1
/// @brief Sensor variable number; depth is stored in sensorValues[0].
#define HYDROS21_DEPTH_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "waterDepth"
#define HYDROS21_DEPTH_VAR_NAME "waterDepth"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "millimeter"
#define HYDROS21_DEPTH_UNIT_NAME "millimeter"
/// @brief Default variable short code; "Hydros21depth"
#define HYDROS21_DEPTH_DEFAULT_CODE "Hydros21depth"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter Hydros 21 conductivity, temperature, and depth sensor](@ref sensor_hydros21)
 *
 * @ingroup sensor_hydros21
 */
/* clang-format on */
class MeterHydros21 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Meter Hydros 21 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the Hydros 21; can be a char,
     * char*, or int.
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the Hydros 21 can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the Hydros 21.
     * Use -1 if it is continuously powered.
     * - The Hydros 21 requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    MeterHydros21(char SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterHydros21", HYDROS21_NUM_VARIABLES, HYDROS21_WARM_UP_TIME_MS,
              HYDROS21_STABILIZATION_TIME_MS, HYDROS21_MEASUREMENT_TIME_MS,
              HYDROS21_EXTRA_WAKE_TIME_MS, HYDROS21_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterHydros21::MeterHydros21
     */
    MeterHydros21(char* SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterHydros21", HYDROS21_NUM_VARIABLES, HYDROS21_WARM_UP_TIME_MS,
              HYDROS21_STABILIZATION_TIME_MS, HYDROS21_MEASUREMENT_TIME_MS,
              HYDROS21_EXTRA_WAKE_TIME_MS, HYDROS21_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterHydros21::MeterHydros21
     */
    MeterHydros21(int SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterHydros21", HYDROS21_NUM_VARIABLES, HYDROS21_WARM_UP_TIME_MS,
              HYDROS21_STABILIZATION_TIME_MS, HYDROS21_MEASUREMENT_TIME_MS,
              HYDROS21_EXTRA_WAKE_TIME_MS, HYDROS21_INC_CALC_VARIABLES) {}

    /**
     * @brief Destroy the Meter Hydros 21 object
     */
    ~MeterHydros21() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [conductivity output](@ref sensor_hydros21_cond) from a
 * [Meter Hydros 21 3-in-1 water level sensor.](@ref sensor_hydros21)
 *
 * @ingroup sensor_hydros21
 */
/* clang-format on */
class MeterHydros21_Cond : public Variable {
 public:
    /**
     * @brief Construct a new MeterHydros21_Cond object.
     *
     * @param parentSense The parent MeterHydros21 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Hydros21cond".
     */
    explicit MeterHydros21_Cond(
        MeterHydros21* parentSense, const char* uuid = "",
        const char* varCode = HYDROS21_COND_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)HYDROS21_COND_VAR_NUM,
                   (uint8_t)HYDROS21_COND_RESOLUTION, HYDROS21_COND_VAR_NAME,
                   HYDROS21_COND_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterHydros21_Cond object.
     *
     * @note This must be tied with a parent MeterHydros21 before it can be
     * used.
     */
    MeterHydros21_Cond()
        : Variable((const uint8_t)HYDROS21_COND_VAR_NUM,
                   (uint8_t)HYDROS21_COND_RESOLUTION, HYDROS21_COND_VAR_NAME,
                   HYDROS21_COND_UNIT_NAME, HYDROS21_COND_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterHydros21_Cond object - no action needed.
     */
    ~MeterHydros21_Cond() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_hydros21_temp) from a
 * [Meter Hydros 21 3-in-1 water level sensor.](@ref sensor_hydros21)
 *
 * @ingroup sensor_hydros21
 */
/* clang-format on */
class MeterHydros21_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MeterHydros21_Temp object.
     *
     * @param parentSense The parent MeterHydros21 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Hydros21temp".
     */
    explicit MeterHydros21_Temp(
        MeterHydros21* parentSense, const char* uuid = "",
        const char* varCode = HYDROS21_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)HYDROS21_TEMP_VAR_NUM,
                   (uint8_t)HYDROS21_TEMP_RESOLUTION, HYDROS21_TEMP_VAR_NAME,
                   HYDROS21_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterHydros21_Temp object.
     *
     * @note This must be tied with a parent MeterHydros21 before it can be
     * used.
     */
    MeterHydros21_Temp()
        : Variable((const uint8_t)HYDROS21_TEMP_VAR_NUM,
                   (uint8_t)HYDROS21_TEMP_RESOLUTION, HYDROS21_TEMP_VAR_NAME,
                   HYDROS21_TEMP_UNIT_NAME, HYDROS21_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterHydros21_Temp object - no action needed.
     */
    ~MeterHydros21_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [depth output](@ref sensor_hydros21_depth) from a
 * [Meter Hydros 21 3-in-1 water level sensor.](@ref sensor_hydros21)
 *
 * @ingroup sensor_hydros21
 */
/* clang-format on */
class MeterHydros21_Depth : public Variable {
 public:
    /**
     * @brief Construct a new MeterHydros21_Depth object.
     *
     * @param parentSense The parent MeterHydros21 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Hydros21depth".
     */
    explicit MeterHydros21_Depth(
        MeterHydros21* parentSense, const char* uuid = "",
        const char* varCode = HYDROS21_DEPTH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)HYDROS21_DEPTH_VAR_NUM,
                   (uint8_t)HYDROS21_DEPTH_RESOLUTION, HYDROS21_DEPTH_VAR_NAME,
                   HYDROS21_DEPTH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterHydros21_Depth object.
     *
     * @note This must be tied with a parent MeterHydros21 before it can be
     * used.
     */
    MeterHydros21_Depth()
        : Variable((const uint8_t)HYDROS21_DEPTH_VAR_NUM,
                   (uint8_t)HYDROS21_DEPTH_RESOLUTION, HYDROS21_DEPTH_VAR_NAME,
                   HYDROS21_DEPTH_UNIT_NAME, HYDROS21_DEPTH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterHydros21_Depth object - no action needed.
     */
    ~MeterHydros21_Depth() {}
};
/**@}*/
#endif  // SRC_SENSORS_METERHYDROS21_H_
