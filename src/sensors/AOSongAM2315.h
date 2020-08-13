/**
 * @file AOSongAM2315.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AOSongAM2315 sensor subclass and the variable subclasses
 * AOSongAM2315_Humidity and AOSongAM2315_Temp.
 *
 * These are used for the AOSong AM2315 capacitive humidity and temperature
 * sensor.
 *
 * This file is dependent on the Adafruit AM2315 Library.
 */
/* clang-format off */
/**
 * @defgroup am2315_group AOSong AM2315 Encased I2C Temperature and Humidity %Sensor
 * Classes for the [AOSong AM2315](@ref am2315_page) encased I2C
 * capacitive humidity and temperature sensor.
 *
 * @ingroup the_sensors
 *
 * @copydoc am2315_page
 */
/* clang-format on */
/* clang-format off */
/**
 * @page am2315_page AOSong AM2315 Encased I2C Temperature and Humidity %Sensor
 *
 * @tableofcontents
 *
 * @section am2315_notes Quick Notes
 * - Applies to both the AOSong AM2315 and CM2311 capacitive relative humidity
 * and temperature sensors
 * - Depends on the [Adafruit AM2315 Library](https://github.com/adafruit/Adafruit_AM2315).
 * - Communicate via I2C
 *   - only one address possible, 0xB8
 * - **Only 1 can be connected to a system at a time**
 * - Requires a 3.3 - 5.5V power source
 *
 * @section am2315_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AOSong-AM2315-Product-Manual.pdf)
 *
 * @section am2315_sensor The AM2315 Sensor
 * @ctor_doc{AOSongAM2315, int8_t powerPin, uint8_t measurementsToAverage}
 * @subsection am2315_timing Sensor Timing
 * - warm up estimated at 500ms
 * - stabilization estimated at 500ms
 * - measurements take 2s to complete
 *
 * ___
 * @section am2315_temperature Temperature Output
 * @variabledoc{AOSongAM2315,Temp}
 * - Range is -40°C to +125°C
 * - Accuracy is ±0.1°C
 * - Result stored in sensorValues[1]
 * - Resolution is 0.1°C (16 bit)
 * - Reported as degrees Celsius
 * - Default variable code is AM2315Temp
 *
 * ___
 * @section am2315_humidity Relative Humidity Output
 * @variabledoc{AOSongAM2315,Humidity}
 * - Range is 0 to 100% RH
 * - Accuracy is ± 2 % RH at 25°C
 * - Result stored in sensorValues[0]
 * - Resolution is 0.1 % RH (16 bit)
 * - Reported as percent relative humidity
 * - Default variable code is AM2315Humidity
 *
 * ___
 * @section am2315_examples Example Code
 *
 * The AM2315 is used in the
 * [double logger](@ref double_log_am2315)
 * and @menulink{am2315} example
 *
 * @menusnip{am2315}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_AOSONGAM2315_H_
#define SRC_SENSORS_AOSONGAM2315_H_

// Debugging Statement
// #define MS_AOSONGAM2315_DEBUG

#ifdef MS_AOSONGAM2315_DEBUG
#define MS_DEBUGGING_STD "AOSongAM2315"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the AM2315 can report 2 values.
#define AM2315_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; AM2315 warms up in 500ms.
#define AM2315_WARM_UP_TIME_MS 500
/// Sensor::_stabilizationTime_ms; AM2315 is stable after 500ms.
#define AM2315_STABILIZATION_TIME_MS 500
/// Sensor::_measurementTime_ms; AM2315 takes 2000ms to complete a measurement.
#define AM2315_MEASUREMENT_TIME_MS 2000

/// Decimals places in string representation; humidity should have 1.
#define AM2315_HUMIDITY_RESOLUTION 1
/// Variable number; humidity is stored in sensorValues[0].
#define AM2315_HUMIDITY_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define AM2315_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define AM2315_TEMP_VAR_NUM 1


/* clang-format off */
/**
 * @brief The Sensor sub-class for the [AOSong AM2315](@ref am2315_page).
 *
 * @ingroup am2315_group
 */
/* clang-format on */
class AOSongAM2315 : public Sensor {
 public:
    /**
     * @brief Construct a new AOSongAM2315 object
     *
     * Because this is I2C and has only 1 possible address (0xB8), we only need
     * the power pin.
     *
     * @note It is only possible to connect *one* AM2315 at a time!
     *
     * @param powerPin The pin on the mcu controlling power to the AOSong
     * AM2315.  Use -1 if it is continuously powered.
     * - The AM2315 requires a 3.3 - 5.5V power source
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit AOSongAM2315(int8_t powerPin, uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the AOSongAM2315 object - no action needed.
     */
    ~AOSongAM2315();

    /**
     * @brief Report the I2C address of the AM2315 - which is always 0xB8.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the #_powerPin mode, begins the Wire library (sets pin levels
     * and modes for I2C), and updates the #_sensorStatus.  No sensor power is
     * required.
     *
     * @return **bool** True if the setup was successful.  For the AOSong AM2315
     * the result will always be true.
     */
    bool setup(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [relative humidity output](@ref am2315_humidity) from an
 * [AOSong AM2315](@ref am2315_page).
 *
 * @ingroup am2315_group
 */
/* clang-format on */
class AOSongAM2315_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new AOSongAM2315_Humidity object.
     *
     * @param parentSense The parent AOSongAM2315 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AM2315Humidity".
     */
    explicit AOSongAM2315_Humidity(AOSongAM2315* parentSense,
                                   const char*   uuid    = "",
                                   const char*   varCode = "AM2315Humidity")
        : Variable(parentSense, (const uint8_t)AM2315_HUMIDITY_VAR_NUM,
                   (uint8_t)AM2315_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", varCode, uuid) {}
    /**
     * @brief Construct a new AOSongAM2315_Humidity object.
     *
     * @note This must be tied with a parent AOSongAM2315 before it can be used.
     */
    AOSongAM2315_Humidity()
        : Variable((const uint8_t)AM2315_HUMIDITY_VAR_NUM,
                   (uint8_t)AM2315_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", "AM2315Humidity") {}
    /**
     * @brief Destroy the AOSongAM2315_Humidity object - no action needed.
     */
    ~AOSongAM2315_Humidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref am2315_temperature) from an
 * [AOSong AM2315](@ref am2315_page).
 *
 * @ingroup am2315_group
 */
/* clang-format on */
class AOSongAM2315_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AOSongAM2315_Temp object.
     *
     * @param parentSense The parent AOSongAM2315 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AM2315Temp".
     */
    explicit AOSongAM2315_Temp(AOSongAM2315* parentSense, const char* uuid = "",
                               const char* varCode = "AM2315Temp")
        : Variable(parentSense, (const uint8_t)AM2315_TEMP_VAR_NUM,
                   (uint8_t)AM2315_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new AOSongAM2315_Temp object.
     *
     * @note This must be tied with a parent AOSongAM2315 before it can be used.
     */
    AOSongAM2315_Temp()
        : Variable((const uint8_t)AM2315_TEMP_VAR_NUM,
                   (uint8_t)AM2315_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "AM2315Temp") {}
    /**
     * @brief Destroy the AOSongAM2315_Temp object - no action needed.
     */
    ~AOSongAM2315_Temp() {}
};

#endif  // SRC_SENSORS_AOSONGAM2315_H_
