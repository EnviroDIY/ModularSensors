/**
 * @file MeterAtmos22.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Greg Cutrell <gcutrell@limno.com>
 * @note Backbone of code used from MeterAtmos21.h
 *
 * @brief Contains the MeterAtmos22 subclass of the SDI12Sensors class along
 * with the variable subclasses MeterAtmos22_Wspd, MeterAtmos22_Wgst, and
 * MeterAtmos22_Wdir.
 *
 * These are used for the Meter Atmos 22.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 *
 */
/* clang-format off */

/**
 * @defgroup sensor_atmos22 Meter Atmos 21
 * Classes for the Meter Atmos 21 wind speed, gust, and direction.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_atmos22_intro Introduction
 *
 * > The atmos 22 sonic anemometer was designed without moving parts, meaning
 * > there's never any mechanical wear. No oiling or replacing bearings. And 
 * > no recalibration required.
 *
 *
 * The Atmos 22 is implemented as a sub-classes of the SDI12Sensors class.
 * It requires a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V.
 *
 * @note  This is identical to Meter Atmos 21 in everything but the names.
 *
 * @warning Coming from the factory, METER sensors are set at SDI-12 address
 * '0'.  They also output a "DDI" serial protocol string on each power up.
 * This library *disables the DDI output string* on all newer METER sensors
 * that support disabling it.  After using a METER sensor with ModularSensors,
 * you will need to manually re-enable the DDI output if you wish to use it.
 *
 * @section sensor_atmos22_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Atmos
 * 21 can be found at: http://library.metergroup.com/Manuals/20419_ATMOS22_Manual_Web.pdf
 * http://publications.metergroup.com/Integrator%20Guide/18195%20ATMOS%2022%20Integrator%20Guide.pdf
 *
 * @note Using the M! sdi-12 command will return
 * a+<windSpeed>+<windDirection>+<gustWindSpeed>±<airTemperature>
 * The command must not be implemented faster than every 10 seconds. The atmos22 
 * automatically averages values in between measurement commands.
 * 
 * @section sensor_atmos22_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_atmos22_ctor Sensor Constructor
 * {{ @ref MeterAtmos22::MeterAtmos22 }}
 *
 * ___
 * @section sensor_atmos22_examples Example Code
 * The Meter Atmos22 is used in the @menulink{meter_atmos22} example.
 *
 * @menusnip{meter_atmos22}
 */
/* clang-format on */

/* clang-format off */
/**
 * @defgroup sensor_atmos22 Meter Atmos 21
 * Classes for the Meter Atmos 21 conductivity, temperature, and depth sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_atmos22_intro Introduction
 *
 * The Atmos 21 is implemented as a sub-classes of the SDI12Sensors class.
 * It requires a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V.
 *
 * @warning Coming from the factory, METER sensors are set at SDI-12 address
 * '0'.  They also output a "DDI" serial protocol string on each power up.
 * This library *disables the DDI output string* on all newer METER sensors
 * that support disabling it.  After using a METER sensor with ModularSensors,
 * you will need to manually re-enable the DDI output if you wish to use it.
 *
 * @section sensor_atmos22_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Atmos
 * 21 can be found at: http://library.metergroup.com/Manuals/13869_CTD_Web.pdf
 *
 * @section sensor_atmos22_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_atmos22_ctor Sensor Constructor
 * {{ @ref MeterAtmos22::MeterAtmos22 }}
 *
 * ___
 * @section sensor_atmos22_examples Example Code
 * The Meter Atmos22 is used in the @menulink{meter_atmos22} example.
 *
 * @menusnip{meter_atmos22}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_METERATMOS22_H_
#define SRC_SENSORS_METERATMOS22_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_atmos22 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Atmos 21 can report 4 values, but air temp not used.
#define ATMOS22_NUM_VARIABLES 4
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define ATMOS22_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_atmos22_timing
 * @name Sensor Timing
 * The sensor timing for a Meter Atmos 21
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; maximum warm-up time in SDI-12 mode: 500ms
#define ATMOS22_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; the Atmos 21 is stable as soon as it
/// warms up (0ms stabilization).
#define ATMOS22_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; maximum measurement duration: 500ms.
#define ATMOS22_MEASUREMENT_TIME_MS 500
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The Atmos 21 requires no extra time.
#define ATMOS22_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_atmos22_wspd
 * @name Wind speed
 * The wind speed variable from a Meter Atmos 22
 *  Resolution is 0.01 m/s
 *  Accuracy is the greater of 0.3 m/s or 3% of measurement
 *  Range is 0–30 m/s
 *
 * {{ @ref MeterAtmos22_Wspd::MeterAtmos22_Wspd }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; wind speed should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define ATMOS22_WSPD_RESOLUTION 2
/// @brief Sensor variable number; conductivity is stored in sensorValues[2].
#define ATMOS22_WSPD_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "windSpeed"
#define ATMOS22_WSPD_VAR_NAME "windSpeed"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "meterspersecond" 
#define ATMOS22_WSPD_UNIT_NAME "meterspersecond"
/// @brief Default variable short code; "Atmos22Wspd"
#define ATMOS22_WSPD_DEFAULT_CODE "Atmos22Wspd"
/**@}*/

/**
 * @anchor sensor_atmos22_wgst
 * @name Wind Gust
 * The wind gust variable from a Meter Atmos 22
 *  Resolution is 0.01 m/s
 *  Accuracy is the greater of 0.3 m/s or 3% of measurement
 *  Range is 0–30 m/s
 *
 * {{ @ref MeterAtmos22_Wgst::MeterAtmos22_Wgst }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; wind speed should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define ATMOS22_WGST_RESOLUTION 2
/// @brief Sensor variable number; conductivity is stored in sensorValues[2].
#define ATMOS22_WGST_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "windGust"
#define ATMOS22_WGST_VAR_NAME "windGust"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "meterspersecond"
#define ATMOS22_WGST_UNIT_NAME "meterspersecond"
/// @brief Default variable short code; "Atmos22Wgst"
#define ATMOS22_WGST_DEFAULT_CODE "Atmos22Wgst"
/**@}*/

/**
 * @anchor sensor_atmos22_wdir
 * @name Wind Direction
 * The wind speed variable from a Meter Atmos 22
 *  Resolution is 1°
 *  Accuracy is ±5°
 *  Range is 0°–359°
 *
 * {{ @ref MeterAtmos22_Wdir::MeterAtmos22_Wdir }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; wind speed should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define ATMOS22_WDIR_RESOLUTION 1
/// @brief Sensor variable number; conductivity is stored in sensorValues[2].
#define ATMOS22_WDIR_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "windDirection"
#define ATMOS22_WDIR_VAR_NAME "windDirection"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degrees"
#define ATMOS22_WDIR_UNIT_NAME "degrees"
/// @brief Default variable short code; "Atmos22Wdir"
#define ATMOS22_WDIR_DEFAULT_CODE "Atmos22Wdir"
/**@}*/

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter Atmos 22 wind speed, gust, and direction sensor](@ref sensor_atmos22)
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Meter Atmos 22 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the Atmos 22; can be a char,
     * char*, or int.
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the Atmos 22 can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the Atmos 22.
     * Use -1 if it is continuously powered.
     * - The Atmos 22 requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    MeterAtmos22(char SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterAtmos22", ATMOS22_NUM_VARIABLES, ATMOS22_WARM_UP_TIME_MS,
              ATMOS22_STABILIZATION_TIME_MS, ATMOS22_MEASUREMENT_TIME_MS,
              ATMOS22_EXTRA_WAKE_TIME_MS, ATMOS22_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterAtmos22::MeterAtmos22
     */
    MeterAtmos22(char* SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterAtmos22", ATMOS22_NUM_VARIABLES, ATMOS22_WARM_UP_TIME_MS,
              ATMOS22_STABILIZATION_TIME_MS, ATMOS22_MEASUREMENT_TIME_MS,
              ATMOS22_EXTRA_WAKE_TIME_MS, ATMOS22_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterAtmos22::MeterAtmos22
     */
    MeterAtmos22(int SDI12address, int8_t powerPin, int8_t dataPin,
                  uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "MeterAtmos22", ATMOS22_NUM_VARIABLES, ATMOS22_WARM_UP_TIME_MS,
              ATMOS22_STABILIZATION_TIME_MS, ATMOS22_MEASUREMENT_TIME_MS,
              ATMOS22_EXTRA_WAKE_TIME_MS, ATMOS22_INC_CALC_VARIABLES) {}

    /**
     * @brief Destroy the Meter Atmos 22 object
     */
    ~MeterAtmos22() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [wind speed output](@ref sensor_atmos22_wspd) from a
 * [Meter Atmos 22 wind sensor.](@ref sensor_atmos22)
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22_Wspd : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos22_Wspd object.
     *
     * @param parentSense The parent MeterAtmos22 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Atmos22wspd".
     */
    explicit MeterAtmos22_Wspd(
        MeterAtmos22* parentSense, const char* uuid = "",
        const char* varCode = ATMOS22_WSPD_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS22_WSPD_VAR_NUM,
                   (uint8_t)ATMOS22_WSPD_RESOLUTION, ATMOS22_WSPD_VAR_NAME,
                   ATMOS22_WSPD_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos22_Wspd object.
     *
     * @note This must be tied with a parent MeterAtmos22 before it can be
     * used.
     */
    MeterAtmos22_Wspd()
        : Variable((const uint8_t)ATMOS22_WSPD_VAR_NUM,
                   (uint8_t)ATMOS22_WSPD_RESOLUTION, ATMOS22_WSPD_VAR_NAME,
                   ATMOS22_WSPD_UNIT_NAME, ATMOS22_WSPD_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos22_Wspd object - no action needed.
     */
    ~MeterAtmos22_Wspd() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [wind speed output](@ref sensor_atmos22_wgst) from a
 * [Meter Atmos 22 wind sensor.](@ref sensor_atmos22)
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22_Wgst : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos22_Wgst object.
     *
     * @param parentSense The parent MeterAtmos22 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Atmos22wgst".
     */
    explicit MeterAtmos22_Wgst(
        MeterAtmos22* parentSense, const char* uuid = "",
        const char* varCode = ATMOS22_WGST_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS22_WGST_VAR_NUM,
                   (uint8_t)ATMOS22_WGST_RESOLUTION, ATMOS22_WGST_VAR_NAME,
                   ATMOS22_WGST_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos22_Wgst object.
     *
     * @note This must be tied with a parent MeterAtmos22 before it can be
     * used.
     */
    MeterAtmos22_Wgst()
        : Variable((const uint8_t)ATMOS22_WGST_VAR_NUM,
                   (uint8_t)ATMOS22_WGST_RESOLUTION, ATMOS22_WGST_VAR_NAME,
                   ATMOS22_WGST_UNIT_NAME, ATMOS22_WGST_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos22_Wgst object - no action needed.
     */
    ~MeterAtmos22_Wgst() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [wind speed output](@ref sensor_atmos22_wdir) from a
 * [Meter Atmos 22 wind sensor.](@ref sensor_atmos22)
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22_Wdir : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos22_Wdir object.
     *
     * @param parentSense The parent MeterAtmos22 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Atmos22wdir".
     */
    explicit MeterAtmos22_Wdir(
        MeterAtmos22* parentSense, const char* uuid = "",
        const char* varCode = ATMOS22_WDIR_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS22_WDIR_VAR_NUM,
                   (uint8_t)ATMOS22_WDIR_RESOLUTION, ATMOS22_WDIR_VAR_NAME,
                   ATMOS22_WDIR_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos22_Wgst object.
     *
     * @note This must be tied with a parent MeterAtmos22 before it can be
     * used.
     */
    MeterAtmos22_Wdir()
        : Variable((const uint8_t)ATMOS22_WDIR_VAR_NUM,
                   (uint8_t)ATMOS22_WDIR_RESOLUTION, ATMOS22_WDIR_VAR_NAME,
                   ATMOS22_WDIR_UNIT_NAME, ATMOS22_WDIR_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos22_Wgst object - no action needed.
     */
    ~MeterAtmos22_Wdir() {}
};

#endif  // Header Guard
