/**
 * @file BoschBME280.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the BoschBME280 sensor subclass and the variable subclasses
 * BoschBME280_Temp, BoschBME280_Humidity, BoschBME280_Pressure, and
 * BoschBME280_Altitude.
 *
 * These are used for the Bosch BME280 digital pressure and humidity sensor.
 *
 * This depends on the [Adafruit BME280
 * library](https://github.com/adafruit/Adafruit_BME280_Library).
 *
 * @copydetails BoschBME280
 */
/* clang-format off */
/**
 * @defgroup sensor_bme280 Bosch BME280
 * Classes for the Bosch BME280 environmental sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_bme280_intro Introduction
 * > The BME280 is a humidity sensor especially developed for mobile applications
 * > and wearables where size and low power consumption are key design parameters.
 * > The unit combines high linearity and high accuracy sensors and is perfectly
 * > feasible for low current consumption, long-term stability and high EMC
 * > robustness.  The humidity sensor offers an extremely fast response time and
 * > therefore supports performance requirements for emerging applications such as
 * > context awareness, and high accuracy over a wide temperature range.
 *
 * Although this sensor has the option of either I2C or SPI communication, this
 * library only supports I2C.  The default I2C address varies by manufacturer
 * and is either 0x77 or 0x76.  The Adafruit and Sparkfun defaults are both 0x77
 * and Seeed/Grove default is 0x76, though all can be changed by physical
 * modification of the sensor, if necessary (by cutting the board connection for
 * the manufacturer default and soldering the optional address jumpers).  To
 * connect two of these sensors to your system, you must ensure they are
 * soldered so as to have different I2C addresses.  No more than two can be
 * attached.  This module is likely to also work with the
 * [Bosch BMP280 Barometric Pressure Sensor](https://www.bosch-sensortec.com/bst/products/all_products/bmp280),
 * though it has not been tested on it.  These sensors should be attached to
 * a 1.7-3.6V power source and the power supply to the sensor can be stopped
 * between measurements.
 *
 * The [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library)
 * is used internally for communication with the BME280.
 *
 * @warning The I2C addresses used by the BME280 are the same as those of the BMP388,
 * BMP390, and MS5803!  If you are also using one of those sensors, make sure that the
 * address for that sensor does not conflict with the address of this sensor.
 *
 * @note Software I2C is *not* supported for the BME280.
 * A secondary hardware I2C on a SAMD board is supported.
 *
 * @section sensor_bme280_datasheet Sensor Datasheet
 * Documentation for the sensor can be found at:
 * https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/
 *
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BME280-Datasheet.pdf)
 *
 * @section sensor_bme280_flags Build flags
 * - ```-D SEALEVELPRESSURE_HPA```
 *      - use to adjust the sea level pressure used to calculate altitude from measured barometric pressure
 *      - if not defined, 1013.25 is used
 *      - The same sea level pressure flag is used for both the BMP3xx and the BME280.
 * Whatever you select will be used for both sensors.
 *
 * @section sensor_bme280_ctor Sensor Constructors
 * {{ @ref BoschBME280::BoschBME280(int8_t, uint8_t, uint8_t) }}
 * {{ @ref BoschBME280::BoschBME280(TwoWire*, int8_t, uint8_t, uint8_t) }}
 *
 * ___
 * @section sensor_bme280_examples Example Code
 * The BME280 is used in the @menulink{bosch_bme280} example.
 *
 * @menusnip{bosch_bme280}
 */
/* clang-format on */

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

/** @ingroup sensor_bme280 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the BME280 can report 4 values.
#define BME280_NUM_VARIABLES 4
/// @brief Sensor::_incCalcValues; altitude is calculted within the Adafruit
/// library.
#define BME280_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_bme280_timing
 * @name Sensor Timing
 * The sensor timing for a Bosch BME280
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; BME280 warms up in 100ms.
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
/**@}*/

/**
 * @anchor sensor_bme280_temp
 * @name Temperature
 * The temperature variable from a Bosch BME280
 * - Range is -40°C to +85°C
 * - Accuracy is ±0.5°C
 *
 * {{ @ref BoschBME280_Temp::BoschBME280_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define BME280_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[0].
#define BME280_TEMP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define BME280_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define BME280_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "BoschBME280Temp"
#define BME280_TEMP_DEFAULT_CODE "BoschBME280Temp"
/**@}*/

/**
 * @anchor sensor_bme280_humidity
 * @name Humidity
 * The humidity variable from a Bosch BME280
 *
 * {{ @ref BoschBME280_Humidity::BoschBME280_Humidity }}
 */
/**@{*/
/// @brief Decimals places in string representation; humidity should have 3-
/// resolution is 0.008 % RH (16 bit).
#define BME280_HUMIDITY_RESOLUTION 3
/// @brief Sensor variable number; humidity is stored in sensorValues[1].
#define BME280_HUMIDITY_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "relativeHumidity"
#define BME280_HUMIDITY_VAR_NAME "relativeHumidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent" -
/// percent relative humidity (% RH)
#define BME280_HUMIDITY_UNIT_NAME "percent"
/// @brief Default variable short code; "BoschBME280Humidity"
#define BME280_HUMIDITY_DEFAULT_CODE "BoschBME280Humidity"
/**@}*/

/**
 * @anchor sensor_bme280_pressure
 * @name Barometric Pressure
 * The barometric pressure variable from a Bosch BME280
 * - Range is 300 to 1100 hPa
 *   - Absolute accuracy is ±1 hPa
 *   - Relative accuracy is ±0.12 hPa
 *
 * {{ @ref BoschBME280_Pressure::BoschBME280_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; barometric pressure should
/// have 2.
#define BME280_PRESSURE_RESOLUTION 2
/// @brief Sensor variable number; pressure is stored in sensorValues[2].
#define BME280_PRESSURE_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "barometricPressure"
#define BME280_PRESSURE_VAR_NAME "barometricPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "pascal"
/// (Pa)
#define BME280_PRESSURE_UNIT_NAME "pascal"
/// @brief Default variable short code; "BoschBME280Pressure"
#define BME280_PRESSURE_DEFAULT_CODE "BoschBME280Pressure"
/**@}*/

/**
 * @anchor sensor_bme280_altitude
 * @name Altitude
 * The altitude variable from a Bosch BME280
 *
 * {{ @ref BoschBME280_Altitude::BoschBME280_Altitude }}
 */
/**@{*/
/// @brief Decimals places in string representation; altitude should have 0 -
/// resolution is 1m.
#define BME280_ALTITUDE_RESOLUTION 0
/// @brief Sensor variable number; altitude is stored in sensorValues[3].
#define BME280_ALTITUDE_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "heightAboveSeaFloor"
#define BME280_ALTITUDE_VAR_NAME "heightAboveSeaFloor"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "meter"
#define BME280_ALTITUDE_UNIT_NAME "meter"
/// @brief Default variable short code; "BoschBME280Altitude"
#define BME280_ALTITUDE_DEFAULT_CODE "BoschBME280Altitude"
/**@}*/

/// The atmospheric pressure at sea level
#ifndef SEALEVELPRESSURE_HPA
#define SEALEVELPRESSURE_HPA (1013.25)
#endif

/* clang-format off */
/**
 * @brief The Sensor sub-class for the [Bosch BME280](@ref sensor_bme280).
 *
 * @ingroup sensor_bme280
 */
/* clang-format on */
class BoschBME280 : public Sensor {
 public:
    /**
     * @brief Construct a new Bosch BME280 object using a secondary *hardware*
     * I2C instance.
     *
     * @note Software I2C is *not* supported for the BME280.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling power to the BME280
     * Use -1 if it is continuously powered.
     * - The BME280 requires a 1.7 - 3.6V power source
     * @param i2cAddressHex The I2C address of the BME280; must be either 0x76
     * or 0x77.  The default value is 0x76.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    BoschBME280(TwoWire* theI2C, int8_t powerPin, uint8_t i2cAddressHex = 0x76,
                uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new Bosch BME280 object using the primary hardware I2C
     * instance.
     *
     * @param powerPin The pin on the mcu controlling power to the BME280
     * Use -1 if it is continuously powered.
     * - The BME280 requires a 1.7 - 3.6V power source
     * @param i2cAddressHex The I2C address of the BME280; must be either 0x76
     * or 0x77.  The default value is 0x76.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
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
     * Verifies that the power is on and updates the #_sensorStatus.  This also
     * sets the #_millisSensorActivated timestamp.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return **bool** True if the wake function completed successfully.
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
     * @return **bool** True if the setup was successful.
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

 private:
    /**
     * @brief Internal reference the the Adafruit BME object
     */
    Adafruit_BME280 bme_internal;
    /**
     * @brief The I2C address of the BME280.
     */
    uint8_t _i2cAddressHex;
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_bme280_temp)
 * from a [Bosch BME280](@ref sensor_bme280).
 *
 * @ingroup sensor_bme280
 */
/* clang-format on */
class BoschBME280_Temp : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Temp object.
     *
     * @param parentSense The parent BoschBME280 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoschBME280Temp".
     */
    explicit BoschBME280_Temp(BoschBME280* parentSense, const char* uuid = "",
                              const char* varCode = BME280_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BME280_TEMP_VAR_NUM,
                   (uint8_t)BME280_TEMP_RESOLUTION, BME280_TEMP_VAR_NAME,
                   BME280_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new BoschBME280_Temp object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Temp()
        : Variable((const uint8_t)BME280_TEMP_VAR_NUM,
                   (uint8_t)BME280_TEMP_RESOLUTION, BME280_TEMP_VAR_NAME,
                   BME280_TEMP_UNIT_NAME, BME280_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the BoschBME280_Temp object - no action needed.
     */
    ~BoschBME280_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [relative humidity output](@ref sensor_bme280_humidity) from a
 * [Bosch BME280](@ref sensor_bme280).
 *
 * @ingroup sensor_bme280
 */
/* clang-format on */
class BoschBME280_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Humidity object.
     *
     * @param parentSense The parent BoschBME280 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoschBME280Humidity".
     */
    explicit BoschBME280_Humidity(
        BoschBME280* parentSense, const char* uuid = "",
        const char* varCode = BME280_HUMIDITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BME280_HUMIDITY_VAR_NUM,
                   (uint8_t)BME280_HUMIDITY_RESOLUTION,
                   BME280_HUMIDITY_VAR_NAME, BME280_HUMIDITY_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new BoschBME280_Humidity object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Humidity()
        : Variable((const uint8_t)BME280_HUMIDITY_VAR_NUM,
                   (uint8_t)BME280_HUMIDITY_RESOLUTION,
                   BME280_HUMIDITY_VAR_NAME, BME280_HUMIDITY_UNIT_NAME,
                   BME280_HUMIDITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the BoschBME280_Humidity object - no action needed.
     */
    ~BoschBME280_Humidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [atmospheric pressure output](@ref sensor_bme280_pressure) from a
 * [Bosch BME280](@ref sensor_bme280).
 *
 * @ingroup sensor_bme280
 */
/* clang-format on */
class BoschBME280_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Pressure object.
     *
     * @param parentSense The parent BoschBME280 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoschBME280Pressure".
     */
    explicit BoschBME280_Pressure(
        BoschBME280* parentSense, const char* uuid = "",
        const char* varCode = BME280_PRESSURE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BME280_PRESSURE_VAR_NUM,
                   (uint8_t)BME280_PRESSURE_RESOLUTION,
                   BME280_PRESSURE_VAR_NAME, BME280_PRESSURE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new BoschBME280_Pressure object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Pressure()
        : Variable((const uint8_t)BME280_PRESSURE_VAR_NUM,
                   (uint8_t)BME280_PRESSURE_RESOLUTION,
                   BME280_PRESSURE_VAR_NAME, BME280_PRESSURE_UNIT_NAME,
                   BME280_PRESSURE_DEFAULT_CODE) {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [altitude](@ref sensor_bme280_altitude) calculated from the measurements
 * made by a [Bosch BME280](@ref sensor_bme280).
 *
 * @ingroup sensor_bme280
 */
/* clang-format on */
class BoschBME280_Altitude : public Variable {
 public:
    /**
     * @brief Construct a new BoschBME280_Altitude object.
     *
     * @param parentSense The parent BoschBME280 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoschBME280Altitude".
     */
    explicit BoschBME280_Altitude(
        BoschBME280* parentSense, const char* uuid = "",
        const char* varCode = BME280_ALTITUDE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BME280_ALTITUDE_VAR_NUM,
                   (uint8_t)BME280_ALTITUDE_RESOLUTION,
                   BME280_ALTITUDE_VAR_NAME, BME280_ALTITUDE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new BoschBME280_Altitude object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    BoschBME280_Altitude()
        : Variable((const uint8_t)BME280_ALTITUDE_VAR_NUM,
                   (uint8_t)BME280_ALTITUDE_RESOLUTION,
                   BME280_ALTITUDE_VAR_NAME, BME280_ALTITUDE_UNIT_NAME,
                   BME280_ALTITUDE_DEFAULT_CODE) {}
};
/**@}*/
#endif  // SRC_SENSORS_BOSCHBME280_H_
