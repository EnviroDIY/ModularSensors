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
 * @defgroup am2315_group AOSong AM2315
 * Classes for the AOSong AM2315 encased I2C capacitive humidity and
 * temperature sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section am2315_notes Quick Notes
 * - Applies to both the AOSong AM2315 and CM2311 capacitive relative humidity
 * and temperature sensors
 * - Depends on the [Adafruit AM2315 Library](https://github.com/adafruit/Adafruit_AM2315).
 * - Communicate via I2C
 *   - only one address possible, 0xB8
 * - **Only 1 can be connected to a single I2C bus at a time**
 * - Requires a 3.3 - 5.5V power source
 *
 * @note Software I2C is *not* supported for the AM2315.
 * A secondary hardware I2C on a SAMD board is supported.
 *
 * @section am2315_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AOSong-AM2315-Product-Manual.pdf)
 *
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
#include <Adafruit_AM2315.h>

// Sensor Specific Defines
/** @ingroup am2315_group */
/**@{*/

/// @brief Sensor::_numReturnedValues; the AM2315 can report 2 values.
#define AM2315_NUM_VARIABLES 2

/**
 * @anchor am2315_timing_defines
 * @name Sensor Timing
 * Defines for the sensor timing for an AOSong AM2315
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; AM2315 warms up in 500ms (estimated).
#define AM2315_WARM_UP_TIME_MS 500
/// @brief Sensor::_stabilizationTime_ms; AM2315 is stable after 500ms
/// (estimated).
#define AM2315_STABILIZATION_TIME_MS 500
/// @brief Sensor::_measurementTime_ms; AM2315 takes 2000ms (2s) to complete a
/// measurement.
#define AM2315_MEASUREMENT_TIME_MS 2000
/**@}*/

/**
 * @anchor am2315_humidity_defines
 * @name Humidity
 * Defines for the humidity variable from an AOSong AM2315
 * - Range is 0 to 100% RH
 * - Accuracy is ± 2 % RH at 25°C
 */
/**@{*/
/// @brief Decimals places in string representation; humidity should have 1 (0.1
/// % RH for the 16 bit sensor).
#define AM2315_HUMIDITY_RESOLUTION 1
/// @brief Variable number; humidity is stored in sensorValues[0].
#define AM2315_HUMIDITY_VAR_NUM 0
/// @brief Variable name; "relativeHumidity"
#define AM2315_HUMIDITY_VAR_NAME "relativeHumidity"
/// @brief Variable unit name; "percent" (percent relative humidity)
#define AM2315_HUMIDITY_UNIT_NAME "percent"
/// @brief Default variable short code; "AM2315Humidity"
#define AM2315_HUMIDITY_DEFAULT_CODE "AM2315Humidity"
/**@}*/

/**
 * @anchor am2315_temperature_defines
 * @name Temperature
 * Defines for the temperature variable from an AOSong AM2315
 * - Range is -40°C to +125°C
 * - Accuracy is ±0.1°C
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1.
/// (0.1°C for the 16 bit sensor)
#define AM2315_TEMP_RESOLUTION 1
/// @brief Variable number; temperature is stored in sensorValues[1].
#define AM2315_TEMP_VAR_NUM 1
/// @brief Variable name; "temperature"
#define AM2315_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name; "degreeCelsius" (°C)
#define AM2315_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "AM2315Temp"
#define AM2315_TEMP_DEFAULT_CODE "AM2315Temp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the [AOSong AM2315](@ref am2315_group).
 *
 * @ingroup am2315_group
 */
/* clang-format on */
class AOSongAM2315 : public Sensor {
 public:
    /**
     * @brief Construct a new AOSongAM2315 object using a secondary *hardware*
     * I2C instance.
     * @ingroup am2315_group
     *
     * @note It is only possible to connect *one* AM2315 at a time on a single
     * I2C bus.  Software I2C is also not supported.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling power to the AOSong
     * AM2315.  Use -1 if it is continuously powered.
     * - The AM2315 requires a 3.3 - 5.5V power source
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    AOSongAM2315(TwoWire* theI2C, int8_t powerPin,
                 uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new AOSongAM2315 object using the primary hardware I2C
     * instance.
     * @ingroup am2315_group
     *
     * Because this is I2C and has only 1 possible address (0xB8), we only need
     * the power pin.
     *
     * @note It is only possible to connect *one* AM2315 at a time on a single
     * I2C bus.  Software I2C is also not supported.
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

 private:
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [relative humidity output](@ref am2315_humidity) from an
 * [AOSong AM2315](@ref am2315_group).
 *
 * @ingroup am2315_group
 */
/* clang-format on */
class AOSongAM2315_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new AOSongAM2315_Humidity object.
     * @ingroup am2315_group
     *
     * @param parentSense The parent AOSongAM2315 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AM2315Humidity".
     */
    explicit AOSongAM2315_Humidity(
        AOSongAM2315* parentSense, const char* uuid = "",
        const char* varCode = AM2315_HUMIDITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)AM2315_HUMIDITY_VAR_NUM,
                   (uint8_t)AM2315_HUMIDITY_RESOLUTION,
                   AM2315_HUMIDITY_VAR_NAME, AM2315_HUMIDITY_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new AOSongAM2315_Humidity object.
     *
     * @note This must be tied with a parent AOSongAM2315 before it can be used.
     */
    AOSongAM2315_Humidity()
        : Variable((const uint8_t)AM2315_HUMIDITY_VAR_NUM,
                   (uint8_t)AM2315_HUMIDITY_RESOLUTION,
                   AM2315_HUMIDITY_VAR_NAME, AM2315_HUMIDITY_UNIT_NAME,
                   AM2315_HUMIDITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AOSongAM2315_Humidity object - no action needed.
     */
    ~AOSongAM2315_Humidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref am2315_temperature) from an
 * [AOSong AM2315](@ref am2315_group).
 *
 * @ingroup am2315_group
 */
/* clang-format on */
class AOSongAM2315_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AOSongAM2315_Temp object.
     * @ingroup am2315_group
     *
     * @param parentSense The parent AOSongAM2315 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AM2315Temp".
     */
    explicit AOSongAM2315_Temp(AOSongAM2315* parentSense, const char* uuid = "",
                               const char* varCode = AM2315_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)AM2315_TEMP_VAR_NUM,
                   (uint8_t)AM2315_TEMP_RESOLUTION, AM2315_TEMP_VAR_NAME,
                   AM2315_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AOSongAM2315_Temp object.
     *
     * @note This must be tied with a parent AOSongAM2315 before it can be used.
     */
    AOSongAM2315_Temp()
        : Variable((const uint8_t)AM2315_TEMP_VAR_NUM,
                   (uint8_t)AM2315_TEMP_RESOLUTION, AM2315_TEMP_VAR_NAME,
                   AM2315_TEMP_UNIT_NAME, AM2315_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AOSongAM2315_Temp object - no action needed.
     */
    ~AOSongAM2315_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_AOSONGAM2315_H_
