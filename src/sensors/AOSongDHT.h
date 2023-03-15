/**
 * @file AOSongDHT.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AOSongDHT sensor subclass and the variable subclasses
 * AOSongDHT_Humidity, AOSongDHT_Temp, and AOSongDHT_HI.
 *
 * These are used for the AOSong digital-output relative humidity & temperature
 * sensors/modules: DHT11, DHT21(AM2301), and DHT 22 (AM2302).
 *
 * This file is dependent on the Adafruit DHT Library.
 */
/* clang-format off */
/**
 * @defgroup sensor_dht AOSong DHT
 * Classes for the AOSong DHT digital-output relative humidity and temperature
 * sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_dht_notes Quick Notes
 * - Applies to AOSong modules:
 *   - [DHT11/CHT11](http://www.aosong.com/en/products/details.asp?id=109),
 *   - DHT21/AM2301, and
 *   - [DHT22/AM2302/CM2302](http://www.aosong.com/en/products/details.asp?id=117)
 * - Depends on the [Adafruit DHT library](https://github.com/adafruit/DHT-sensor-library)
 * - Communicate with a single-bus single wire digital signaling protocol
 *   - Can be connected to any digital pin on a Arduino
 *   - The communication with these sensors is slow and _interrupts are turned
 * off during communication_.
 *   - See the Adafruit DHT library's DHT.cpp for details.
 *   - Could possibly cause issues if used in combination with other
 * interrupt-driven sensors.
 * - Only 1 can be connected to a system at a time
 * - Requires a 3.3 - 6V power source
 * - Sensors should not be polled more frequently than once every 2 seconds
 * - Uses a Maxim DS18 sensor internally
 *
 * @section sensor_dht_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AOSong-DHT22-Datasheet.pdf)
 *
 * @section sensor_dht_ctor Sensor Constructor
 * {{ @ref AOSongDHT::AOSongDHT }}
 *
 * ___
 * @section sensor_dht_examples Example Code
 * The DHT is used in the @menulink{ao_song_dht} example.
 *
 * @menusnip{ao_song_dht}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_AOSONGDHT_H_
#define SRC_SENSORS_AOSONGDHT_H_

// Debugging Statement
// #define MS_AOSONGDHT_DEBUG

#ifdef MS_AOSONGDHT_DEBUG
#define MS_DEBUGGING_STD "AOSongDHT"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include "DHT.h"

#ifdef DHT11
// In older versions of the DHT library, defines were used
// Undefine these macros and convert to static const for compatibility with the
// newer versions of the library
#undef DHT11
#undef DHT21
#undef AM2301
#undef DHT22
#undef AM2302

/* Define types of sensors. */
static const uint8_t DHT11{11};  /**< DHT TYPE 11 */
static const uint8_t DHT12{12};  /**< DHY TYPE 12 */
static const uint8_t DHT21{21};  /**< DHT TYPE 21 */
static const uint8_t DHT22{22};  /**< DHT TYPE 22 */
static const uint8_t AM2301{21}; /**< AM2301 */
#endif

/** @ingroup sensor_dht */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the DHT can report 3 values.
#define DHT_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define DHT_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_dht_timing
 * @name Sensor Timing
 * The sensor timing for an AOSong DHT
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; DHT warms up in 1700ms (1.7sec).
#define DHT_WARM_UP_TIME_MS 1700
/// @brief Sensor::_stabilizationTime_ms; We assume the sensor is stable
/// immediately after warm-up
#define DHT_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; DHT takes 2000ms (2s) to complete a
/// measurement.
#define DHT_MEASUREMENT_TIME_MS 2000
/**@}*/

/**
 * @anchor sensor_dht_humidity
 * @name Humidity
 * The humidity variable from an AOSong DHT
 * - Accuracy is ± 2 % RH for DHT22 and ± 5 % RH for DHT11
 * - Range is 0 to 100 % RH
 *
 * {{ @ref AOSongDHT_Humidity::AOSongDHT_Humidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; humidity should have 1 (0.1
/// % RH for DHT22 and 1 % RH for DHT11)
#define DHT_HUMIDITY_RESOLUTION 1
/// @brief Sensor variable number; humidity is stored in sensorValues[0].
#define DHT_HUMIDITY_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "relativeHumidity"
#define DHT_HUMIDITY_VAR_NAME "relativeHumidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent"
/// (percent relative humidity or % RH)
#define DHT_HUMIDITY_UNIT_NAME "percent"
/// @brief Default variable short code; "DHTHumidity"
#define DHT_HUMIDITY_DEFAULT_CODE "DHTHumidity"
/**@}*/

/**
 * @anchor sensor_dht_temperature
 * @name Temperature
 * The temperature variable from an AOSong DHT
 * - Accuracy is ±0.5°C for DHT22 and ± ±2°C for DHT11
 * - Range is -40°C to +80°C
 *
 * {{ @ref AOSongDHT_Temp::AOSongDHT_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 1 -
/// resolution is 0.1°C.
#define DHT_TEMP_RESOLUTION 1
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define DHT_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define DHT_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (degrees Celsius, °C)
#define DHT_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "DHTTemp"
#define DHT_TEMP_DEFAULT_CODE "DHTTemp"
/**@}*/

/**
 * @anchor sensor_dht_hi
 * @name Heat Index
 * The heat index variable from an AOSong DHT
 * - Heat index is calculated within the Adafruit library from the measured
 * temperature and humidity.
 *
 * {{ @ref AOSongDHT_HI::AOSongDHT_HI }}
 */
/**@{*/
/// @brief Decimals places in string representation; heat index should have 1 -
/// resolution is 0.1°C
#define DHT_HI_RESOLUTION 1
/// @brief Sensor variable number; HI is stored in sensorValues[2].
#define DHT_HI_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "heatIndex"
#define DHT_HI_VAR_NAME "heatIndex"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define DHT_HI_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "DHTHI"
#define DHT_HI_DEFAULT_CODE "DHTHI"
/**@}*/

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [AOSong digital-output relative humidity and temperature sensor modules](@ref sensor_dht).
 *
 * @ingroup sensor_dht
 */
/* clang-format on */
class AOSongDHT : public Sensor {
 public:
    /**
     * @brief Construct a new AOSongDHT object - need the power pin, the data
     * pin, and the sensor type
     *
     * @param powerPin The pin on the mcu controlling power to the AOSong DHT.
     * Use -1 if it is continuously powered.
     * - The DHT requires a 3.3 - 6V power source
     * @param dataPin The pin on the mcu receiving data from the AOSong DHT
     * @param type The type of DHT.  Possible values are DHT11, DHT21, AM2301,
     * DHT22, or AM2302.
     * - NOTE:  The DHT type should be entered _without quotation marks_ since
     * the values are members of an enum rather than a string.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    AOSongDHT(int8_t powerPin, int8_t dataPin, const uint8_t type,
              uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the AOSongDHT object - no action needed.
     */
    ~AOSongDHT();

    /**
     * @copydoc Sensor::setup()
     */
    bool setup(void) override;

    /**
     * @copydoc Sensor::getSensorName()
     */
    String getSensorName(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    DHT     dht_internal;
    uint8_t _dhtType;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [humidity output](@ref sensor_dht_humidity) from an [AOSong DHT](@ref sensor_dht).
 *
 * @ingroup sensor_dht
 */
/* clang-format on */
class AOSongDHT_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new AOSongDHT_Humidity object.
     *
     * @param parentSense The parent AOSongDHT providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "DHTHumidity".
     */
    explicit AOSongDHT_Humidity(AOSongDHT* parentSense, const char* uuid = "",
                                const char* varCode = DHT_HUMIDITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)DHT_HUMIDITY_VAR_NUM,
                   (uint8_t)DHT_HUMIDITY_RESOLUTION, DHT_HUMIDITY_VAR_NAME,
                   DHT_HUMIDITY_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AOSongDHT_Humidity object.
     *
     * @note This must be tied with a parent AOSongDHT before it can be used.
     */
    AOSongDHT_Humidity()
        : Variable((const uint8_t)DHT_HUMIDITY_VAR_NUM,
                   (uint8_t)DHT_HUMIDITY_RESOLUTION, DHT_HUMIDITY_VAR_NAME,
                   DHT_HUMIDITY_UNIT_NAME, DHT_HUMIDITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AOSongDHT_Humidity object - no action needed.
     */
    ~AOSongDHT_Humidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_dht_temperature) from an
 * [AOSong DHT](@ref sensor_dht).
 *
 * @ingroup sensor_dht
 */
/* clang-format on */
class AOSongDHT_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AOSongDHT_Temp object.
     *
     * @param parentSense The parent AOSongDHT providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "DHTTemp".
     */
    explicit AOSongDHT_Temp(AOSongDHT* parentSense, const char* uuid = "",
                            const char* varCode = DHT_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)DHT_TEMP_VAR_NUM,
                   (uint8_t)DHT_TEMP_RESOLUTION, DHT_TEMP_VAR_NAME,
                   DHT_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AOSongDHT_Temp object.
     *
     * @note This must be tied with a parent AOSongDHT before it can be used.
     */
    AOSongDHT_Temp()
        : Variable((const uint8_t)DHT_TEMP_VAR_NUM,
                   (uint8_t)DHT_TEMP_RESOLUTION, DHT_TEMP_VAR_NAME,
                   DHT_TEMP_UNIT_NAME, DHT_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AOSongDHT_Temp object - no action needed.
     */
    ~AOSongDHT_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [heat index output](@ref sensor_dht_hi) calculated from measurements made
 * by an [AOSong DHT](@ref sensor_dht).
 *
 * @ingroup sensor_dht
 */
/* clang-format on */
class AOSongDHT_HI : public Variable {
 public:
    /**
     * @brief Construct a new AOSongDHT_HI object.
     *
     * @param parentSense The parent AOSongDHT providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "DHTHI".
     */
    explicit AOSongDHT_HI(AOSongDHT* parentSense, const char* uuid = "",
                          const char* varCode = DHT_HI_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)DHT_HI_VAR_NUM,
                   (uint8_t)DHT_HI_RESOLUTION, DHT_HI_VAR_NAME,
                   DHT_HI_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AOSongDHT_HI object.
     *
     * @note This must be tied with a parent AOSongDHT before it can be used.
     */
    AOSongDHT_HI()
        : Variable((const uint8_t)DHT_HI_VAR_NUM, (uint8_t)DHT_HI_RESOLUTION,
                   DHT_HI_VAR_NAME, DHT_HI_UNIT_NAME, DHT_HI_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AOSongDHT_HI object - no action needed.
     */
    ~AOSongDHT_HI() {}
};
/**@}*/
#endif  // SRC_SENSORS_AOSONGDHT_H_
