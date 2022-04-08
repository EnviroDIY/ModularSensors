/**
 * @file DecagonES2.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DecagonES2 subclass of the SDI12Sensors class along with
 * the variable subclasses DecagonES2_Cond and DecagonES2_Temp.
 *
 * These are used for the discontinued Decagon Devices ES-2 Electrical
 * Conductivity Sensor.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 */
/* clang-format off */
/**
 * @defgroup sensor_es2 Decagon ES-2
 * Classes for the Decagon ES-2 conductivity and temperature sensor.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_es2_intro Introduction
 *
 * @warning This sensor is no longer manufactured!
 *
 * If you still have one of these sensors, it is implemented as a sub-classes of
 * the SDI12Sensors class. It requires a 3.5-12V power supply, which can be
 * turned off between measurements. While contrary to the manual, they will run
 * with power as low as 3.3V. On the version with a stereo cable, the power is
 * connected to the tip, data to the ring, and ground to the sleeve. On the
 * bare-wire version, the power is connected to the _white_ cable, data to
 * _red_, and ground to the unshielded cable.
 *
 * @section sensor_es2_datasheet Sensor Datasheet
 * - [Manual](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Decagon-ES-2-Manual.pdf)
 * - [Integrator's Guide](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Decagon-ES-2-Integrators-Guide.pdf)
 *
 * @section sensor_es2_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_es2_ctor Sensor Constructor
 * {{ @ref DecagonES2::DecagonES2 }}
 *
 * ___
 * @section sensor_es2_examples Example Code
 * The Decagon ES-2 is used in the @menulink{decagon_es2} example.
 *
 * @menusnip{decagon_es2}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_DECAGONES2_H_
#define SRC_SENSORS_DECAGONES2_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_es2 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the ES2 can report 2 values.
#define ES2_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define ES2_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_es2_timing
 * @name Sensor Timing
 * The sensor timing for a Decagon ES-2
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; maximum warm-up time in SDI-12 mode: 250ms.
#define ES2_WARM_UP_TIME_MS 250
/// @brief Sensor::_stabilizationTime_ms; the ES2 is stable as soon as it warms
/// up (0ms stabilization).
#define ES2_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; maximum measurement duration: 250ms.
#define ES2_MEASUREMENT_TIME_MS 250
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The ES-2 requires no extra time.
#define ES2_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_es2_cond
 * @name Conductivity
 * The conductivity variable from a Decagon ES-2
 * - Range is 0 – 120 mS/cm (bulk)
 * - Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
 *
 * {{ @ref DecagonES2_Cond::DecagonES2_Cond }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; conductivity should have 1.
 *
 * 0 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.001 mS/cm = 1 µS/cm
 */
#define ES2_COND_RESOLUTION 1
/// @brief Sensor variable number; conductivity is stored in sensorValues[0].
#define ES2_COND_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "specificConductance"
#define ES2_COND_VAR_NAME "specificConductance"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microsiemenPerCentimeter" (µS/cm)
#define ES2_COND_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "ES2Cond"
#define ES2_COND_DEFAULT_CODE "ES2Cond"
/**@}*/

/**
 * @anchor sensor_es2_temp
 * @name Temperature
 * The temperature variable from a Decagon ES-2
 * - Range is -40°C to +50°C
 * - Accuracy is ±1°C
 *
 * {{ @ref DecagonES2_Temp::DecagonES2_Temp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; temperature should have 2.
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.1°C
 */
#define ES2_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define ES2_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define ES2_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define ES2_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "ES2Temp"
#define ES2_TEMP_DEFAULT_CODE "ES2Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the [Decagon ES-2 sensor](@ref sensor_es2)
 *
 * @ingroup sensor_es2
 */
/* clang-format on */
class DecagonES2 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Decagon ES2 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the ES-2; can be a char, char*,
     * or int.
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the ES-2 can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the ES2
     * Use -1 if it is continuously powered.
     * -  The ES2 requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    DecagonES2(char SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS,
                       ES2_EXTRA_WAKE_TIME_MS, ES2_INC_CALC_VARIABLES) {}
    /**
     * @copydoc DecagonES2::DecagonES2
     */
    DecagonES2(char* SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS,
                       ES2_EXTRA_WAKE_TIME_MS, ES2_INC_CALC_VARIABLES) {}
    /**
     * @copydoc DecagonES2::DecagonES2
     */
    DecagonES2(int SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "DecagonES2", ES2_NUM_VARIABLES, ES2_WARM_UP_TIME_MS,
                       ES2_STABILIZATION_TIME_MS, ES2_MEASUREMENT_TIME_MS,
                       ES2_EXTRA_WAKE_TIME_MS, ES2_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Decagon ES2 object
     */
    ~DecagonES2() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [conductivity output](@ref sensor_es2_cond) from a
 * [Decagon ES-2 electrical conductivity sensor](@ref sensor_es2).
 *
 * @ingroup sensor_es2
 */
/* clang-format on */
class DecagonES2_Cond : public Variable {
 public:
    /**
     * @brief Construct a new DecagonES2_Cond object.
     *
     * @param parentSense The parent DecagonES2 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ES2Cond".
     */
    explicit DecagonES2_Cond(DecagonES2* parentSense, const char* uuid = "",
                             const char* varCode = ES2_COND_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ES2_COND_VAR_NUM,
                   (uint8_t)ES2_COND_RESOLUTION, ES2_COND_VAR_NAME,
                   ES2_COND_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new DecagonES2_Cond object.
     *
     * @note This must be tied with a parent DecagonES2 before it can be used.
     */
    DecagonES2_Cond()
        : Variable((const uint8_t)ES2_COND_VAR_NUM,
                   (uint8_t)ES2_COND_RESOLUTION, ES2_COND_VAR_NAME,
                   ES2_COND_UNIT_NAME, ES2_COND_DEFAULT_CODE) {}
    /**
     * @brief Destroy the DecagonES2_Cond object - no action needed.
     */
    ~DecagonES2_Cond() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_es2_temp) from a
 * [Decagon ES-2 electrical conductivity sensor](@ref sensor_es2).
 *
 * @ingroup sensor_es2
 */
/* clang-format on */
class DecagonES2_Temp : public Variable {
 public:
    /**
     * @brief Construct a new DecagonES2_Temp object.
     *
     * @param parentSense The parent DecagonES2 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ES2Temp".
     */
    explicit DecagonES2_Temp(DecagonES2* parentSense, const char* uuid = "",
                             const char* varCode = ES2_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ES2_TEMP_VAR_NUM,
                   (uint8_t)ES2_TEMP_RESOLUTION, ES2_TEMP_VAR_NAME,
                   ES2_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new DecagonES2_Temp object.
     *
     * @note This must be tied with a parent DecagonES2 before it can be used.
     */
    DecagonES2_Temp()
        : Variable((const uint8_t)ES2_TEMP_VAR_NUM,
                   (uint8_t)ES2_TEMP_RESOLUTION, ES2_TEMP_VAR_NAME,
                   ES2_TEMP_UNIT_NAME, ES2_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the DecagonES2_Temp object - no action needed.
     */
    ~DecagonES2_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_DECAGONES2_H_
