/**
 * @file SensirionSHT4x.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SensirionSHT4x sensor subclass and the variable
 * subclasses SensirionSHT4x_Humidity and SensirionSHT4x_Temp.
 *
 * These are used for the Sensirion SHT40, SHT41, and SHT45 capacitive humidity
 * and temperature sensor.
 *
 * This depends on the [Adafruit SHT40
 * library](https://github.com/adafruit/Adafruit_SHT4X).
 */
/* clang-format off */
/**
 * @defgroup sensor_sht4x Sensirion SHT40, SHT41, and SHT45
 * Classes for the Sensirion SHT40, SHT41, and SHT45 I2C humidity and temperature sensors.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_sht4x_intro Introduction
 *
 * > SHT4x is a digital sensor platform for measuring relative humidity and
 * > temperature at different accuracy classes. The I2C interface  provides
 * > several preconfigured I2C addresses and maintains an ultra-low power
 * > budget. The power-trimmed internal heater can be used at three heating
 * > levels thus enabling sensor operation in demanding environments.
 *

 * - The code for this library should work for 0x44 addressed SHT4x sensors - the SHT40, SHT41, and SHT45.
 * - Depends on the [Adafruit SHT4x Library](https://github.com/adafruit/Adafruit_SHT40).
 * - Communicates via I2C
 *   - There are versions of the SHT40, SHT40-AD1B with the I2C address 0x44
 * and SHT40-BD1B with the I2C address 0x45.
 * **This library only supports the 0x44 addressed sensor!**
 *   - The SHT41 and SHT45 only have one possible address, 0x44.
 * - **Only 1 can be connected to a single I2C bus at a time**
 * - Requires a 3.3 power source
 *
 * @note Software I2C is *not* supported for the SHT4x.
 * A secondary hardware I2C on a SAMD board is supported.
 *
 * @section sensor_sht4x_datasheet Sensor Datasheet
 * [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Sensirion_Humidity_Sensors_SHT4x_Datasheet.pdf)
 *
 * @section sensor_sht4x_ctor Sensor Constructors
 * {{ @ref SensirionSHT4x::SensirionSHT4x(int8_t, bool, uint8_t) }}
 * {{ @ref SensirionSHT4x::SensirionSHT4x(TwoWire*, int8_t, bool, uint8_t) }}
 *
 * @section sensor_sht4x_examples Example Code
 *
 * The SHT40 is used in the @menulink{sensirion_sht4x} example
 *
 * @menusnip{sensirion_sht4x}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_SENSIRIONSHT4X_H_
#define SRC_SENSORS_SENSIRIONSHT4X_H_

// Debugging Statement
// #define MS_SENSIRION_SHT4X_DEBUG

#ifdef MS_SENSIRION_SHT4X_DEBUG
#define MS_DEBUGGING_STD "SensirionSHT4x"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_SHT4x.h>

/** @ingroup sensor_sht4x */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the SHT4x can report 2 values.
#define SHT4X_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define SHT4X_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_sht4x_timing
 * @name Sensor Timing
 * The sensor timing for an Sensirion SHT4x
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; SHT4x warms up in 0.3ms (typical) and
/// soft-resets in 1ms (max).
#define SHT4X_WARM_UP_TIME_MS 1
/// @brief Sensor::_stabilizationTime_ms; SHT4x is assumed to be immediately
/// stable.
#define SHT4X_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; SHT4x takes 8.2ms (max) to complete a
/// measurement at the highest precision.  At medium precision measurement time
/// is 4.5ms (max) and it is 1.7ms (max) at low precision.
#define SHT4X_MEASUREMENT_TIME_MS 9
/**@}*/

/**
 * @anchor sensor_sht4x_humidity
 * @name Humidity
 * The humidity variable from an Sensirion SHT4x
 * - Range is 0 to 100% RH
 * - Accuracy is ± 1.8 % RH (typical)
 *
 * {{ @ref SensirionSHT4x_Humidity::SensirionSHT4x_Humidity }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; humidity should have 2 (0.01
 * % RH).
 *
 * @note This resolution is some-what silly in light of the ± 1.8 % RH accuracy.
 */
#define SHT4X_HUMIDITY_RESOLUTION 2
/// @brief Sensor variable number; humidity is stored in sensorValues[0].
#define SHT4X_HUMIDITY_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "relativeHumidity"
#define SHT4X_HUMIDITY_VAR_NAME "relativeHumidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent"
/// (percent relative humidity)
#define SHT4X_HUMIDITY_UNIT_NAME "percent"
/// @brief Default variable short code; "SHT4xHumidity"
#define SHT4X_HUMIDITY_DEFAULT_CODE "SHT4xHumidity"
/**@}*/

/**
 * @anchor sensor_sht4x_temperature
 * @name Temperature
 * The temperature variable from an Sensirion SHT4x
 * - Range is -40°C to +125°C
 * - Accuracy is ±0.2°C
 *
 * {{ @ref SensirionSHT4x_Temp::SensirionSHT4x_Temp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; humidity should have 2 (0.01
 * °C).
 *
 * @note This resolution is some-what silly in light of the ± 0.2°C accuracy.
 */
#define SHT4X_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define SHT4X_TEMP_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define SHT4X_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define SHT4X_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "SHT4xTemp"
#define SHT4X_TEMP_DEFAULT_CODE "SHT4xTemp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the [Sensirion SHT4x](@ref sensor_sht4x).
 */
/* clang-format on */
class SensirionSHT4x : public Sensor {
 public:
    /**
     * @brief Construct a new SensirionSHT4x object using a secondary *hardware*
     * I2C instance.
     *
     * This is only applicable to SAMD boards that are able to have multiple
     * hardware I2C ports in use via SERCOMs.
     *
     * @note It is only possible to connect *one* SHT4x at a time on a single
     * I2C bus.  Only the 0x44 addressed version is supported.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling power to the Sensirion
     * SHT4x.  Use -1 if it is continuously powered.
     * - The SHT4x requires a 3.3V power source
     * @param useHeater Whether or not to run the internal heater of the SHT4x
     * when shutting down the sensor; optional with a default value of true. The
     * internal heater is designed to remove condensed water from the sensor -
     * which will make the sensor stop responding to air humidity changes - and
     * to allow creep-free operation in high humidity environments.  The longest
     * the internal heater can run at a time is 1s and the maximum duty load is
     * 5%.  Running only 1s per measurment cycle probably isn't enough to help
     * with more than very minimal condensation, but it's probably the best we
     * can easily do.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    SensirionSHT4x(TwoWire* theI2C, int8_t powerPin, bool useHeater = true,
                   uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new SensirionSHT4x object using the primary hardware
     * I2C instance.
     *
     * Because this is I2C and has only 1 possible address (0x44), we only need
     * the power pin.
     *
     * @note It is only possible to connect *one* SHT4x at a time on a single
     * I2C bus.  Only the 0x44 addressed version is supported.
     *
     * @param powerPin The pin on the mcu controlling power to the Sensirion
     * SHT4x.  Use -1 if it is continuously powered.
     * - The SHT4x requires a 3.3V power source
     * @param useHeater Whether or not to run the internal heater of the SHT4x
     * when shutting down the sensor; optional with a default value of true. The
     * internal heater is designed to remove condensed water from the sensor -
     * which will make the sensor stop responding to air humidity changes - and
     * to allow creep-free operation in high humidity environments.  The longest
     * the internal heater can run at a time is 1s and the maximum duty load is
     * 5%.  Running only 1s per measurment cycle probably isn't enough to help
     * with more than very minimal condensation, but it's probably the best we
     * can easily do.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit SensirionSHT4x(int8_t powerPin, bool useHeater = true,
                            uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the SensirionSHT4x object - no action needed.
     */
    ~SensirionSHT4x();

    /**
     * @brief Report the I2C address of the SHT4x - which is always 0x44.
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
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

    /**
     * @copydoc Sensor::sleep()
     *
     * If opted for, we run the SHT4x's internal heater for 1s before going to
     * sleep.
     */
    bool sleep(void) override;

 private:
    /**
     * @brief Internal variable for the heating setting
     */
    bool _useHeater;
    /**
     * @brief Internal reference the the Adafruit BME object
     */
    Adafruit_SHT4x sht4x_internal;
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [relative humidity output](@ref sensor_sht4x_humidity) from an
 * [Sensirion SHT4x](@ref sensor_sht4x).
 */
/* clang-format on */
class SensirionSHT4x_Humidity : public Variable {
 public:
    /**
     * @brief Construct a new SensirionSHT4x_Humidity object.
     *
     * @param parentSense The parent SensirionSHT4x providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SHT4xHumidity".
     */
    explicit SensirionSHT4x_Humidity(
        SensirionSHT4x* parentSense, const char* uuid = "",
        const char* varCode = SHT4X_HUMIDITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SHT4X_HUMIDITY_VAR_NUM,
                   (uint8_t)SHT4X_HUMIDITY_RESOLUTION, SHT4X_HUMIDITY_VAR_NAME,
                   SHT4X_HUMIDITY_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new SensirionSHT4x_Humidity object.
     *
     * @note This must be tied with a parent SensirionSHT4x before it can be
     * used.
     */
    SensirionSHT4x_Humidity()
        : Variable((const uint8_t)SHT4X_HUMIDITY_VAR_NUM,
                   (uint8_t)SHT4X_HUMIDITY_RESOLUTION, SHT4X_HUMIDITY_VAR_NAME,
                   SHT4X_HUMIDITY_UNIT_NAME, SHT4X_HUMIDITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the SensirionSHT4x_Humidity object - no action needed.
     */
    ~SensirionSHT4x_Humidity() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_sht4x_temperature) from an
 * [Sensirion SHTx0](@ref sensor_sht4x).
 */
/* clang-format on */
class SensirionSHT4x_Temp : public Variable {
 public:
    /**
     * @brief Construct a new SensirionSHT4x_Temp object.
     *
     * @param parentSense The parent SensirionSHT4x providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SHT4xTemp".
     */
    explicit SensirionSHT4x_Temp(SensirionSHT4x* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode  = SHT4X_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)SHT4X_TEMP_VAR_NUM,
                   (uint8_t)SHT4X_TEMP_RESOLUTION, SHT4X_TEMP_VAR_NAME,
                   SHT4X_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new SensirionSHT4x_Temp object.
     *
     * @note This must be tied with a parent SensirionSHT4x before it can be
     * used.
     */
    SensirionSHT4x_Temp()
        : Variable((const uint8_t)SHT4X_TEMP_VAR_NUM,
                   (uint8_t)SHT4X_TEMP_RESOLUTION, SHT4X_TEMP_VAR_NAME,
                   SHT4X_TEMP_UNIT_NAME, SHT4X_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the SensirionSHT4x_Temp object - no action needed.
     */
    ~SensirionSHT4x_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_SENSIRIONSHT4X_H_
