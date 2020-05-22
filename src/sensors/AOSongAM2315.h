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
 *
 * @copydetails AOSongAM2315
 */

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
#define AM2315_NUM_VARIABLES 2
#define AM2315_WARM_UP_TIME_MS 500
#define AM2315_STABILIZATION_TIME_MS 500
#define AM2315_MEASUREMENT_TIME_MS 2000

#define AM2315_HUMIDITY_RESOLUTION 1
#define AM2315_HUMIDITY_VAR_NUM 0

#define AM2315_TEMP_RESOLUTION 1
#define AM2315_TEMP_VAR_NUM 1


/**
 * @brief The main class for the AOSong AM2315 capacitive humidity and
 * temperature sensor.
 *
 * Documentation for the sensor can be found at:
 * www.aosong.com/asp_bin/Products/en/AM2315.pdf
 *
 * For Relative Humidity:
 *   - Resolution is 0.1 % RH (16 bit)
 *   - Accuracy is ± 2 % RH at 25°C
 *   - Range is 0 to 100% RH
 *   - Reported as percent RH
 *   - Result stored as sensorValues[0]
 *
 * For Temperature:
 *   - Resolution is 0.1°C (16 bit)
 *   - Accuracy is ±0.1°C
 *   - Range is -40°C to +125°C
 *   - Reported as degrees Celsius
 *   - Result stored as sensorValues[1]
 *
 * Warm up/stability/re-sampling time: 2sec
 */
class AOSongAM2315 : public Sensor {
 public:
    /**
     * @brief Construct a new AOSongAM2315 object - because this is I2C, only
     * need the power pin.
     *
     * @param powerPin The pin on the mcu controlling power to the AOSong
     * AM2315.  Use -1 if the sensor is continuously powered.
     * @param measurementsToAverage The number of measurements to average.
     */
    explicit AOSongAM2315(int8_t powerPin, uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the AOSongAM2315 object - no action needed.
     */
    ~AOSongAM2315();

    /**
     * @brief Get the pin or connection location between the mcu and the
     * sensor.  For the AM2315 reports the I2C address - which is always 0xB8.
     *
     * @return String Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * For the AOSong AM2315 this starts the Wire library.  The sensor does not
     * need to be powered.
     *
     * @return true The setup was successful.  For the AOSong AM2315 the result
     * will always be true.
     * @return false Some part of the setup failed.  For the AOSong AM2315 this
     * should not happen.
     */
    bool setup(void) override;

    bool addSingleMeasurementResult(void) override;
};


/**
 * @brief The variable class used for humidity measured by an AOSong AM2315.
 *
 * For Relative Humidity:
 *   - Resolution is 0.1 % RH (16 bit)
 *   - Accuracy is ± 2 % RH at 25°C
 *   - Range is 0 to 100% RH
 *   - Reported as percent RH
 *   - Result stored as sensorValues[0]
 */
class AOSongAM2315_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new AOSongAM2315_Humidity object.
     *
     * @param parentSense The parent AOSongAM2315 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AM2315Humidity.
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


/**
 * @brief The variable class used for temperature measured by an AOSong AM2315.
 *
 * For Temperature:
 *   - Resolution is 0.1°C (16 bit)
 *   - Accuracy is ±0.1°C
 *   - Range is -40°C to +125°C
 *   - Reported as degrees Celsius
 *   - Result stored as sensorValues[1]
 */
class AOSongAM2315_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AOSongAM2315_Temp object.
     *
     * @param parentSense The parent AOSongAM2315 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AM2315Temp.
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
