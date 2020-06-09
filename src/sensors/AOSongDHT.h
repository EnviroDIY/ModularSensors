/**
 * @file AOSongDHT.cpp
 * @copyright 2020 Stroud Water Research Center
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
 *
 * @copydetails AOSongDHT
 *
 * @defgroup dht_group AOSong DHT
 * The Sensor and Variable objects for the AOSong DHT digital-output relative
 * humidity & temperature sensors.
 *
 * @copydetails AOSongDHT
 */

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
#include <DHT.h>

// Undefine these macros so I can use a typedef instead
#undef DHT11
#undef DHT21
#undef AM2301
#undef DHT22
#undef AM2302

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the DHT can report 3 values.
#define DHT_NUM_VARIABLES 3
/// Sensor::_warmUpTime_ms; DHT warms up in 1700ms.
#define DHT_WARM_UP_TIME_MS 1700
/// Sensor::_stabilizationTime_ms; DHT is stable after 0ms.
#define DHT_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; DHT takes 2000ms to complete a measurement.
#define DHT_MEASUREMENT_TIME_MS 2000

/// Decimals places in string representation; humidity should have 1.
#define DHT_HUMIDITY_RESOLUTION 1
/// Variable number; humidity is stored in sensorValues[0].
#define DHT_HUMIDITY_VAR_NUM 0

/// Decimals places in string representation; temperature should have 1.
#define DHT_TEMP_RESOLUTION 1
/// Variable number; temperature is stored in sensorValues[1].
#define DHT_TEMP_VAR_NUM 1

/// Decimals places in string representation; heat index should have 1.
#define DHT_HI_RESOLUTION 1
/// Variable number; HI is stored in sensorValues[2].
#define DHT_HI_VAR_NUM 2

/**
 * @brief The possible types of DHT
 */
typedef enum DHTtype {
    DHT11  = 11,
    DHT21  = 21,
    AM2301 = 21,
    DHT22  = 22,
    AM2302 = 22
} DHTtype;

/**
 * @brief The main class for the AOSong digital-output relative humidity &
 * temperature sensors/modules: DHT11, DHT21(AM2301), and DHT 22 (AM2302).
 *
 * Documentation for the sensor can be found at:
 * http://www.aosong.com/en/products/details.asp?id=117
 *
 * Timing:
 *     - warms up in 1.7sec
 *     - stable on completion of warm up
 *     - measurements take 2s to complete
 *
 * For Relative Humidity:
 *   @copydetails AOSongDHT_Humidity
 *
 * For Temperature:
 *   @copydetails AOSongDHT_Temp
 *
 * Heat index is calculated from temperature and humidity.
 *   @copydetails AOSongDHT_HI
 *
 * @ingroup dht_group
 *
 * @see dht_page
 */
class AOSongDHT : public Sensor {
 public:
    /**
     * @brief Construct a new AOSongDHT object - need the power pin, the data
     * pin, and the sensor type
     *
     * @param powerPin The pin on the mcu controlling power to the AOSong DHT.
     * Use -1 if the sensor is continuously powered.
     * @param dataPin The pin on the mcu receiving data from the AOSong DHT
     * @param type The type of DHT.  Possible values are DHT11, DHT21, AM2301,
     * DHT22, or AM2302.
     * @param measurementsToAverage The number of measurements to average.
     */
    AOSongDHT(int8_t powerPin, int8_t dataPin, DHTtype type,
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
    DHTtype _dhtType;
};


/**
 * @brief The variable class used for humidity measured by an AOSong DHT.
 *
 *   - Resolution is 0.1 % RH
 *   - Accuracy is ± 2 % RH
 *   - Range is 0 to 100 % RH
 *   - Reported as percent RH
 *   - Result stored in sensorValues[0]
 *   - Default variable code is DHTHumidity
 *
 * @ingroup dht_group
 */
class AOSongDHT_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new AOSongDHT_Humidity object.
     *
     * @param parentSense The parent AOSongDHT providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is DHTHumidity.
     */
    explicit AOSongDHT_Humidity(AOSongDHT* parentSense, const char* uuid = "",
                                const char* varCode = "DHTHumidity")
        : Variable(parentSense, (const uint8_t)DHT_HUMIDITY_VAR_NUM,
                   (uint8_t)DHT_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", varCode, uuid) {}
    /**
     * @brief Construct a new AOSongDHT_Humidity object.
     *
     * @note This must be tied with a parent AOSongDHT before it can be used.
     */
    AOSongDHT_Humidity()
        : Variable((const uint8_t)DHT_HUMIDITY_VAR_NUM,
                   (uint8_t)DHT_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", "DHTHumidity") {}
    /**
     * @brief Destroy the AOSongDHT_Humidity object - no action needed.
     */
    ~AOSongDHT_Humidity() {}
};


/**
 * @brief The variable class used for temperature measured by an AOSong DHT.
 *
 *   - Resolution is 0.1°C
 *   - Accuracy is ±0.5°C
 *   - Range is -40°C to +80°C
 *   - Reported as degrees Celsius
 *   - Result stored in sensorValues[1]
 *   - Default variable code is DHTTemp
 *
 * @ingroup dht_group
 */
class AOSongDHT_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AOSongDHT_Temp object.
     *
     * @param parentSense The parent AOSongDHT providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is DHTTemp.
     */
    explicit AOSongDHT_Temp(AOSongDHT* parentSense, const char* uuid = "",
                            const char* varCode = "DHTTemp")
        : Variable(parentSense, (const uint8_t)DHT_TEMP_VAR_NUM,
                   (uint8_t)DHT_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   varCode, uuid) {}
    /**
     * @brief Construct a new AOSongDHT_Temp object.
     *
     * @note This must be tied with a parent AOSongDHT before it can be used.
     */
    AOSongDHT_Temp()
        : Variable((const uint8_t)DHT_TEMP_VAR_NUM,
                   (uint8_t)DHT_TEMP_RESOLUTION, "temperature", "degreeCelsius",
                   "DHTTemp") {}
    /**
     * @brief Destroy the AOSongDHT_Temp object - no action needed.
     */
    ~AOSongDHT_Temp() {}
};


/**
 * @brief The variable class used for heat index calculated from measurements
 * made by an AOSong DHT.
 *
 *   - Heat index is calculated within the Adafruit library from the measured
 * temperature and humidity.
 *   - Reported as degrees celsius
 *   - Result stored in sensorValues[2]
 *   - Default variable code is heatIndex
 *
 * @ingroup dht_group
 */
class AOSongDHT_HI : public Variable {
 public:
    /**
     * @brief Construct a new AOSongDHT_HI object.
     *
     * @param parentSense The parent AOSongDHT providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is DHTHI.
     */
    explicit AOSongDHT_HI(AOSongDHT* parentSense, const char* uuid = "",
                          const char* varCode = "DHTHI")
        : Variable(parentSense, (const uint8_t)DHT_HI_VAR_NUM,
                   (uint8_t)DHT_HI_RESOLUTION, "heatIndex", "degreeCelsius",
                   varCode, uuid) {}
    /**
     * @brief Construct a new AOSongDHT_HI object.
     *
     * @note This must be tied with a parent AOSongDHT before it can be used.
     */
    AOSongDHT_HI()
        : Variable((const uint8_t)DHT_HI_VAR_NUM, (uint8_t)DHT_HI_RESOLUTION,
                   "heatIndex", "degreeCelsius", "DHTHI") {}
    /**
     * @brief Destroy the AOSongDHT_HI object - no action needed.
     */
    ~AOSongDHT_HI() {}
};

#endif  // SRC_SENSORS_AOSONGDHT_H_
