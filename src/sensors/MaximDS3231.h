/**
 * @file MaximDS3231.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MaximDS3231 sensor subclass and the MaximDS3231_Temp
 * variable subclass.
 *
 * These are for the DS3231 real time clock which is required for all AVR
 * boards.
 *
 * This depends on the EnviroDIY DS3231 library.
 */
/* clang-format off */
/**
 * @defgroup sensor_ds3231 Maxim DS3231 RTC
 * Classes for the Maxim DS3231 high-precision, temperature-corrected
 * real-time clock.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_ds3231_intro Introduction
 *
 * The I2C [Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html)
 * real time clock (RTC) is absolutely required for time-keeping on all AVR boards.
 * This library also makes use of of the DS3231 for its on-board temperature
 * sensor.
 * This temperature is _not_ equivalent to an environmental temperature
 * measurement and should only be used to as a diagnostic. Internally, the DS3231
 * uses the temperature it measures to compensate for temperature variation in the
 * crystal speed. This improves the timekeeping accuracy and reduces time drift.
 *
 * The DS3231 requires a 3.3V power supply and most breakouts or boards that
 * integrate the DS3231 (including the Mayfly) use a coin battery to supply the
 * clock. Using separate power supply for the clock prevents the time from
 * resetting if the main board loses power.
 *
 * @section sensor_ds3231_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Maxim-DS3231-Real-Time-Clock.pdf)
 *
 * @section sensor_ds3231_ctor Sensor Constructor
 * {{ @ref MaximDS3231::MaximDS3231 }}
 *
 * ___
 * @section sensor_ds3231_examples Example Code
 * The Maxim DS3231 RTC is used in nearly all of the examples, including the
 * @menulink{maxim_ds3231} example.
 *
 * @menusnip{maxim_ds3231}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_MAXIMDS3231_H_
#define SRC_SENSORS_MAXIMDS3231_H_

// Debugging Statement
// #define MS_MAXIMDS3231_DEBUG

#ifdef MS_MAXIMDS3231_DEBUG
#define MS_DEBUGGING_STD "MaximDS3231"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_ds3231 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the DS3231 can report 1 value.
#define DS3231_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define DS3231_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_ds3231_timing
 * @name Sensor Timing
 * The sensor timing for a Maxim DS18 RTC
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the DS3231 should never be powered off so
 * there is no warm-up time.
 */
#define DS3231_WARM_UP_TIME_MS 0
/**
 * @brief Sensor::_stabilizationTime_ms; the DS3231 should never be powered off
 * so there is no stabilization time.
 */
#define DS3231_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the DS3231 takes 200ms to complete a
 * measurement - A single temperature conversion takes 200ms.
 */
#define DS3231_MEASUREMENT_TIME_MS 200
/**@}*/

/**
 * @anchor sensor_ds3231_temp
 * @name Temperature
 * The temperature variable from a Maxim DS18 RTC
 *   - Range is -55°C to 125°C
 *   - Accuracy: ± 3°C
 *
 * {{ @ref MaximDS3231_Temp::MaximDS3231_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is -0.25°C (10 bit).
#define DS3231_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[0].
#define DS3231_TEMP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperatureDatalogger"
#define DS3231_TEMP_VAR_NAME "temperatureDatalogger"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define DS3231_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "BoardTemp"
#define DS3231_TEMP_DEFAULT_CODE "BoardTemp"
/**@}*/

/**
 * @brief The Sensor sub-class for the [Maxim DS3231](@ref sensor_ds3231)
 * when used as a low-accuracy temperature sensor
 *
 * Only need a sleep and wake since these DON'T use the default of powering
 * up and down
 *
 * @ingroup sensor_ds3231
 */
class MaximDS3231 : public Sensor {
 public:
    /**
     * @brief Construct a new Maxim DS3231 object
     *
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit MaximDS3231(uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the Maxim DS3231 object
     */
    ~MaximDS3231();

    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This starts communication with the RTC (if it had not already been
     * started).  This will also turn off any interrupts currently scheduled on
     * the RTC.  The clock should be continuously powered, so we never need to
     * worry about power up.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;

    /**
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the #_millisMeasurementRequested timestamp.
     *
     * @note This function does NOT include any waiting for the sensor to be
     * warmed up or stable!
     *
     * @return **bool** True if the start measurement function completed
     * successfully. successfully.
     */
    bool startSingleMeasurement(void) override;
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_ds3231_temp) from a [DS3231](@ref sensor_ds3231).
 *
 * @ingroup sensor_ds3231
 */
/* clang-format on */
class MaximDS3231_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MaximDS3231_Temp object.
     *
     * @param parentSense The parent MaximDS3231 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoardTemp".
     */
    explicit MaximDS3231_Temp(MaximDS3231* parentSense, const char* uuid = "",
                              const char* varCode = DS3231_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)DS3231_TEMP_VAR_NUM,
                   (uint8_t)DS3231_TEMP_RESOLUTION, DS3231_TEMP_VAR_NAME,
                   DS3231_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MaximDS3231_Temp object.
     *
     * @note This must be tied with a parent MaximDS3231 before it can be used.
     */
    MaximDS3231_Temp()
        : Variable((const uint8_t)DS3231_TEMP_VAR_NUM,
                   (uint8_t)DS3231_TEMP_RESOLUTION, DS3231_TEMP_VAR_NAME,
                   DS3231_TEMP_UNIT_NAME, DS3231_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MaximDS3231_Temp object - no action needed.
     */
    ~MaximDS3231_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_MAXIMDS3231_H_
