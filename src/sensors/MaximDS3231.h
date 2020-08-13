/**
 * @file MaximDS3231.h
 * @copyright 2020 Stroud Water Research Center
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
 * @defgroup ds3231_group Maxim DS3231 RTC
 * Classes for the Maxim DS3231 high-precision, temperature-corrected
 * real-time clock.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section ds3231_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Maxim-DS3231-Real-Time-Clock.pdf)
 *
 * @section ds3231_sensor The DS3231 Sensor
 * @ctor_doc{MaximDS3231, measurementsToAverage}
 * @subsection ds3231_timing Sensor Timing
 * - The RTC **must** be continuously powered
 * - A single temperature conversion takes 200ms.
 *
 * @section ds3231_temp Temperature Output
 * @variabledoc{MaximDS3231,Temp}
 *   - Range is -55°C to 125°C
 *   - Accuracy:
 *     - ± 3°C
 *   - Result stored in sensorValues[0]
 *   - Resolution:
 *     - 0.25°C (10 bit)
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is BoardTemp
 *
 * ___
 * @section ds3231_examples Example Code
 * The Maxim DS3231 RTC is used in nearly all of the examples, including the
 * @menulink{ds3231} example.
 *
 * @menusnip{ds3231}
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

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the DS3231 can report 1 value.
#define DS3231_NUM_VARIABLES 1
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
 * measurement.
 */
#define DS3231_MEASUREMENT_TIME_MS 200

/// Decimals places in string representation; temperature should have 2.
#define DS3231_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[0].
#define DS3231_TEMP_VAR_NUM 0


/**
 * @brief The Sensor sub-class for the [Maxim DS3231](@ref ds3231_group) when
 * used as a low-accuracy temperature sensor
 *
 * Only need a sleep and wake since these DON'T use the default of powering up
 * and down
 *
 * @ingroup ds3231_group
 *
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
 * [temperature output](@ref ds3231_temp) from a [DS3231](@ref ds3231_group).
 *
 * @ingroup ds3231_group
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
                              const char* varCode = "BoardTemp")
        : Variable(parentSense, (const uint8_t)DS3231_TEMP_VAR_NUM,
                   (uint8_t)DS3231_TEMP_RESOLUTION, "temperatureDatalogger",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new MaximDS3231_Temp object.
     *
     * @note This must be tied with a parent MaximDS3231 before it can be used.
     */
    MaximDS3231_Temp()
        : Variable((const uint8_t)DS3231_TEMP_VAR_NUM,
                   (uint8_t)DS3231_TEMP_RESOLUTION, "temperatureDatalogger",
                   "degreeCelsius", "BoardTemp") {}
    /**
     * @brief Destroy the MaximDS3231_Temp object - no action needed.
     */
    ~MaximDS3231_Temp() {}
};

#endif  // SRC_SENSORS_MAXIMDS3231_H_
