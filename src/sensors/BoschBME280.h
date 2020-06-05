/**
 * @file BoschBME280.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the BoschBME280 sensor subclass and the variable subclasses
 * BoschBME280_Temp, BoschBME280_Humidity, BoschBME280_Pressure, and
 * BoschBME280_Altitude.
 *
 * These are used for the Bosch BME280 digital pressure and humidity sensor.
 *
 * This depends on the Adafruit BME280 Library.
 *
 * @copydetails #BoschBME280
 *
 * @defgroup bme280_group Bosch BME280
 * The Sensor and Variable objects for the Bosch BME280 environmental sensor.
 *
 * @copydetails #BoschBME280
 */

// Header Guards
#ifndef SRC_SENSORS_BOSCHBME280_H_
#define SRC_SENSORS_BOSCHBME280_H_

// Debugging Statement
// #define MS_BOSCHBME280_DEBUG

#ifdef MS_BOSCHBME280_DEBUG
#define MS_DEBUGGING_STD "BoschBME280"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_BME280.h>

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the BME280 can report 4 values.
#define BME280_NUM_VARIABLES 4
/// Sensor::_warmUpTime_ms; BME280 warms up in 100ms.
#define BME280_WARM_UP_TIME_MS 100
/**
 * @brief Sensor::_stabilizationTime_ms; BME280 is stable after 4000ms.
 *
 * 0.5 s for good numbers, but optimal at 4 s based on tests using
 * bme280timingTest.ino
 */
#define BME280_STABILIZATION_TIME_MS 4000
/**
 * @brief Sensor::_measurementTime_ms; BME280 takes 1100ms to complete a
 * measurement.
 *
 * 1.0 s according to datasheet, but slightly better stdev when 1.1 s
 * For details on BME280 stabilization time updates, include testing sketch and
 * link to data in Google Sheet, see:
 * https://github.com/EnviroDIY/ModularSensors/commit/27e3cb531162ed6971a41f3c38f5920d356089e9
 */
#define BME280_MEASUREMENT_TIME_MS 1100

/// Decimals places in string representation; temperature should have 2.
#define BME280_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[0].
#define BME280_TEMP_VAR_NUM 0

/// Decimals places in string representation; humidity should have 3.
#define BME280_HUMIDITY_RESOLUTION 3
/// Variable number; humidity is stored in sensorValues[1].
#define BME280_HUMIDITY_VAR_NUM 1

/// Decimals places in string representation; barometric pressure should have 2.
#define BME280_PRESSURE_RESOLUTION 2
/// Variable number; PREbarometric pressureSSURE is stored in sensorValues[2].
#define BME280_PRESSURE_VAR_NUM 2

/// Decimals places in string representation; altitude should have 0.
#define BME280_ALTITUDE_RESOLUTION 0
/// Variable number; altitude is stored in sensorValues[3].
#define BME280_ALTITUDE_VAR_NUM 3
/// The atmospheric pressure at sea level
#define SEALEVELPRESSURE_HPA (1013.25)

/**
 * @brief The main class for the Bosch BME280 sensor.
 *
 * Documentation for the sensor can be found at:
 * https://www.bosch-sensortec.com/bst/products/all_products/bme280
 *
 * Timing:
 *     - warm up is 100ms
 *     - stable after 4000ms
 *     - measurements take 1100ms to complete
 *
 * For Barometric Pressure:
 *     @copydetails BoschBME280_Pressure
 *
 * For Temperature:
 *     @copydetails BoschBME280_Temp
 *
 * For Humidity:
 *     @copydetails
 *
 * @ingroup bme280_group
 */
class BoschBME280 : public Sensor {
 public:
    /**
     * @brief Construct a new Bosch BME280 object
     *
     * @param powerPin The pin on the mcu controlling power to the BME280. Use
     * -1 if the sensor is continuously powered.
     * @param i2cAddressHex The I2C address of the BME280; default value is
     * 0x76.
     * @param measurementsToAverage The number of measurements to average
     */
    explicit BoschBME280(int8_t powerPin, uint8_t i2cAddressHex = 0x76,
                         uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the Bosch BME280 object
     */
    ~BoschBME280();

    /**
     * @brief Wake the sensor up, if necessary.  Do whatever it takes to get a
     * sensor in the proper state to begin a measurement.
     *
     * Verifies that the power is on and updates the #_sensorStatus. This also
     * sets the #_millisSensorActivated timestamp.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return **true** The wake function completed successfully.
     * @return **false** Wake did not complete successfully.
     */
    bool wake(void) override;
    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C), reads
     * calibration coefficients from the BME280, and updates the #_sensorStatus.
     * The BME280 must be powered for setup.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;
    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    // bool startSingleMeasurement(void) override;  // for forced mode
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 protected:
    /**
     * @brief Internal reference the the Adafruit BME object
     */
    Adafruit_BME280 bme_internal;
    /**
     * @brief The I2C address of the BME280
     */
    uint8_t _i2cAddressHex;
};


/**
 * @brief The variable class used for temperature measured by a Bosch BME280.
 *
 *   - Range is -40°C to +85°C
 *   - Accuracy is ±0.5°C
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.01°C
 *   - Reported as degrees Celsius
 *   - Default variable code is BoschBME280Temp
 *
 * @ingroup bme280_group
 */
class BoschBME280_Temp : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Temp object.
     *
     * @param parentSense The parent BoschBME280 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is BoschBME280Temp.
     */
    explicit BoschBME280_Temp(BoschBME280* parentSense, const char* uuid = "",
                              const char* varCode = "BoschBME280Temp")
        : Variable(parentSense, (const uint8_t)BME280_TEMP_VAR_NUM,
                   (uint8_t)BME280_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new BoschBME280_Temp object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Temp()
        : Variable((const uint8_t)BME280_TEMP_VAR_NUM,
                   (uint8_t)BME280_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "BoschBME280Temp") {}
    /**
     * @brief Destroy the BoschBME280_Temp object - no action needed.
     */
    ~BoschBME280_Temp() {}
};


/**
 * @brief The variable class used for relative humidity measured by a Bosch
 * BME280.
 *
 *   - Resolution is 0.008 % RH (16 bit)
 *   - Accuracy is ± 3 % RH
 *   - Range is 0 to 100% RH
 *   - Reported as percent relative humidity
 *   - Result stored in sensorValues[1]
 *   - Default variable code is BoschBME280Humidity
 *
 * @ingroup bme280_group
 */
class BoschBME280_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Humidity object.
     *
     * @param parentSense The parent BoschBME280 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is BoschBME280Humidity
     */
    explicit BoschBME280_Humidity(BoschBME280* parentSense,
                                  const char*  uuid    = "",
                                  const char*  varCode = "BoschBME280Humidity")
        : Variable(parentSense, (const uint8_t)BME280_HUMIDITY_VAR_NUM,
                   (uint8_t)BME280_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", varCode, uuid) {}
    /**
     * @brief Construct a new BoschBME280_Humidity object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Humidity()
        : Variable((const uint8_t)BME280_HUMIDITY_VAR_NUM,
                   (uint8_t)BME280_HUMIDITY_RESOLUTION, "relativeHumidity",
                   "percent", "BoschBME280Humidity") {}
    /**
     * @brief Destroy the BoschBME280_Humidity object - no action needed.
     */
    ~BoschBME280_Humidity() {}
};


/**
 * @brief The variable class used for atmospheric pressure measured by a Bosch
 * BME280.
 *
 *   - Range is 300 to 1100 hPa
 *   - Absolute accuracy is ±1hPa
 *   - Relative accuracy is ±0.12hPa
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.18hPa
 *   - Reported as hPa
 *   - Default variable code is BoschBME280Pressure
 *
 * @ingroup bme280_group
 */
class BoschBME280_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Pressure object.
     *
     * @param parentSense The parent BoschBME280 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is BoschBME280Pressure
     */
    explicit BoschBME280_Pressure(BoschBME280* parentSense,
                                  const char*  uuid    = "",
                                  const char*  varCode = "BoschBME280Pressure")
        : Variable(parentSense, (const uint8_t)BME280_PRESSURE_VAR_NUM,
                   (uint8_t)BME280_PRESSURE_RESOLUTION, "barometricPressure",
                   "pascal", varCode, uuid) {}
    /**
     * @brief Construct a new BoschBME280_Pressure object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Pressure()
        : Variable((const uint8_t)BME280_PRESSURE_VAR_NUM,
                   (uint8_t)BME280_PRESSURE_RESOLUTION, "barometricPressure",
                   "pascal", "BoschBME280Pressure") {}
};


/**
 * @brief The variable class used for altitude calculated from the measurements
 * made by a Bosch BME280.
 *
 *   - Result stored in sensorValues[3]
 *   - Resolution is 1m
 *   - Reported as meters
 *   - Default variable code is BoschBME280Altitude
 *
 * @ingroup bme280_group
 */
class BoschBME280_Altitude : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Altitude object.
     *
     * @param parentSense The parent BoschBME280 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is BoschBME280Altitude
     */
    explicit BoschBME280_Altitude(BoschBME280* parentSense,
                                  const char*  uuid    = "",
                                  const char*  varCode = "BoschBME280Altitude")
        : Variable(parentSense, (const uint8_t)BME280_ALTITUDE_VAR_NUM,
                   (uint8_t)BME280_ALTITUDE_RESOLUTION, "heightAboveSeaFloor",
                   "meter", varCode, uuid) {}
    /**
     * @brief Construct a new BoschBME280_Altitude object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Altitude()
        : Variable((const uint8_t)BME280_ALTITUDE_VAR_NUM,
                   (uint8_t)BME280_ALTITUDE_RESOLUTION, "heightAboveSeaFloor",
                   "meter", "BoschBME280Altitude") {}
};


#endif  // SRC_SENSORS_BOSCHBME280_H_
