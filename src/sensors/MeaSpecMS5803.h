/**
 * @file MeaSpecMS5803.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com> with help from Beth
 * Fisher, Evan Host and Bobby Schulz.
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MeaSpecMS5803 semsor subclass and the variable subclasses
 * MeaSpecMS5803_Temp and MeaSpecMS5803_Pressure.
 *
 * These are for the Measurement Specialties MS5803 pressure sensor, which is
 * used in the SparkFun Pressure Sensor Breakout - MS5803-14BA.
 *
 * This depends on the https://github.com/EnviroDIY/MS5803 library, which was
 * modified for ModularSensors based on a fork from the
 * https://github.com/NorthernWidget/MS5803 library, which itself expanded on
 * https://github.com/sparkfun/SparkFun_MS5803-14BA_Breakout_Arduino_Library
 */
/* clang-format off */
/**
 * @defgroup ms5803_group Measurement Specialties MS5803
 * Classes for the @ref ms5803_page
 *
 * @ingroup the_sensors
 *
 * @copydoc ms5803_page
 */
/* clang-format on */
/* clang-format off */
/**
 * @page ms5803_page Measurement Specialties MS5803
 *
 * @tableofcontents
 *
 * @section ms5803_intro Introduction
 *
 * The Measurement Specialties MS5803 is a miniature digital pressure sensor.
 * It comes in several different pressure ranges, with 14 bar being one of the
 * most common.  Although this sensor has the option of either I2C or SPI
 * communication, this library only supports I2C.  The sensor's I2C address is
 * determined the voltage level of the CSB pin; it will be either 0x76 or 0x77.
 * Breakout boards purchased from Sparkfun are set to 0x76 while those from
 * Northern Widget are variable depending on the model as outlined in their
 * [library](https://github.com/NorthernWidget/TP-Downhole_Library). To connect
 * two of these sensors to your system, you must ensure they are soldered so as
 * to have different I2C addresses.  No more than two can be attached.  These
 * sensors should be attached to a 1.7-3.6V power source and the power supply to
 * the sensor can be stopped between measurements.
 *
 * @warning These I2C addresses are the same as those available for the Bosch
 * BME280 Barometric Pressure Sensor!  If you are also using one of those
 * sensors, make sure that the address for that sensor does not conflict with
 * the address of this sensor.
 *
 * The lower level communication between the mcu and the MS5803 is handled by the
 * [Northern Widget MS5803 library](https://github.com/NorthernWidget/MS5803).
 *
 * @section ms5803_datasheet Sensor Datasheet
 *
 * Documentation for the sensor can be found at:
 * https://www.sparkfun.com/products/12909 and
 * https://cdn.sparkfun.com/datasheets/Sensors/Weather/ms5803_14ba.pdf
 *
 * The datasheet is also available here:
 * https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Measurement-Specialities-MS5803-14ba-Pressure-Sensor.pdf
 *
 * @section ms5803_sensor The MS5803 Sensor
 * @ctor_doc{MeaSpecMS5803, int8_t powerPin, uint8_t i2cAddressHex, int16_t maxPressure, uint8_t measurementsToAverage}
 * @subsection ms5803_timing Sensor Timing
 * - Sensor takes about 0.5 / 1.1 / 2.1 / 4.1 / 8.22 ms to respond
 * at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
 * - Assume sensor is immediately stable
 *
 * @section ms5803_temp Temperature Output
 * @variabledoc{MeaSpecMS5803,Temp}
 *   - Range is -40°C to +85°C
 *   - Accuracy is ±0.8°C
 *   - Result stored in sensorValues[0]
 *   - Resolution is <0.01°C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is MeaSpecMS5803Temp
 *
 * @section ms5803_pressure Pressure Output
 * @variabledoc{MeaSpecMS5803,Pressure}
 *   - Range is 0 to 14 bar
 *   - Accuracy between 0 and +40°C is:
 *      - 14ba: ±20mbar
 *      - 2ba: ±1.5mbar
 *      - 1ba:  ±1.5mbar
 *   - Long term stability is:
 *      - 14ba: -20 mbar/yr
 *      - 2ba: -1 mbar/yr
 *   - Result stored in sensorValues[1]
 *   - Resolution is: (at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively))
 *      - 14ba: 1 / 0.6 / 0.4 / 0.3 / 0.2 mbar (where 1 mbar = 100 pascals)
 *      - 2ba: 0.13 / 0.084 / 0.054 / 0.036 / 0.024
 *      - 1ba: 0.065 / 0.042 / 0.027 / 0.018 / 0.012
 *   - Reported as millibar (mb)
 *   - Default variable code is MeaSpecMS5803Pressure
 *
 * ___
 * @section ms5803_examples Example Code
 * The Measurement Specialties MS5803 is used in the @menulink{ms5803} example.
 *
 * @menusnip{ms5803}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_MEASPECMS5803_H_
#define SRC_SENSORS_MEASPECMS5803_H_

// Debugging Statement
// #define MS_MEASPECMS5803_DEBUG

#ifdef MS_MEASPECMS5803_DEBUG
#define MS_DEBUGGING_STD "MeaSpecMS5803"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <MS5803.h>

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the MS5803 can report 2 values.
#define MS5803_NUM_VARIABLES 2
/// Sensor::_warmUpTime_ms; the MS5803 warms up in 10ms.
#define MS5803_WARM_UP_TIME_MS 10
/// Sensor::_stabilizationTime_ms; the MS5803 is stable after 0ms.
#define MS5803_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the MS5803 takes 10ms to complete a
 * measurement.
 */
#define MS5803_MEASUREMENT_TIME_MS 10

/// Decimals places in string representation; temperature should have 2.
#define MS5803_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[0].
#define MS5803_TEMP_VAR_NUM 0

/// Decimals places in string representation; pressure should have 3.
#define MS5803_PRESSURE_RESOLUTION 3
/// Variable number; pressure is stored in sensorValues[1].
#define MS5803_PRESSURE_VAR_NUM 1


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Measurement Specialties MS5803 sensor](@ref ms5803_page).
 *
 * @ingroup ms5803_group
 */
/* clang-format on */
class MeaSpecMS5803 : public Sensor {
 public:
    /**
     * @brief Construct a new MeaSpecMS5803
     *
     * @param powerPin The pin on the mcu controlling power to the BME280.  Use
     * -1 if it is continuously powered.
     * - The MS5803 requires a 1.7 - 3.6V power source
     * @param i2cAddressHex The I2C address of the MS5803; must be either 0x76
     * or 0x77.  The default value is 0x76.
     * @param maxPressure The maximum pressure of the specific MS5803 in bar.
     * The sensors are maufactured with maximum pressures of 1.1 bar, 1.3 bar, 5
     * bar, 7 bar, 14 bar, and 30 bar. Use "1" for the 1.1 bar module and "2"
     * for the 1.3 bar module.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit MeaSpecMS5803(int8_t powerPin, uint8_t i2cAddressHex = 0x76,
                           int16_t maxPressure           = 14,
                           uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the MeaSpecMS5803 object
     */
    ~MeaSpecMS5803();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the pin modes for #_powerPin and I2C, reads coefficients
     * from the MS5803, and updates the #_sensorStatus.  The MS5803 must be
     * powered for setup.  The wrapped function reading the coefficients doesn't
     * return anything to indicate failure or success, we just have to hope it
     * succeeded.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;
    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    MS5803  MS5803_internal;
    uint8_t _i2cAddressHex;
    int16_t _maxPressure;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref ms5803_temp) from a
 * [Measurement Specialties MS5803 digital pressure sensor](@ref ms5803_page).
 *
 * @ingroup ms5803_group
 */
/* clang-format on */
class MeaSpecMS5803_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MeaSpecMS5803_Temp object.
     *
     * @param parentSense The parent MeaSpecMS5803 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of MeaSpecMS5803Temp
     */
    explicit MeaSpecMS5803_Temp(MeaSpecMS5803* parentSense,
                                const char*    uuid    = "",
                                const char*    varCode = "MeaSpecMS5803Temp")
        : Variable(parentSense, (const uint8_t)MS5803_TEMP_VAR_NUM,
                   (uint8_t)MS5803_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new MeaSpecMS5803_Temp object.
     *
     * @note This must be tied with a parent MeaSpecMS5803 before it can be
     * used.
     */
    MeaSpecMS5803_Temp()
        : Variable((const uint8_t)MS5803_TEMP_VAR_NUM,
                   (uint8_t)MS5803_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "MeaSpecMS5803Temp") {}
    /**
     * @brief Destroy the MeaSpecMS5803_Temp object - no action needed.
     */
    ~MeaSpecMS5803_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pressure output](@ref ms5803_pressure) from a
 * [Measurement Specialties MS5803 digital pressure sensor](@ref ms5803_page).
 *
 * @ingroup ms5803_group
 */
/* clang-format on */
class MeaSpecMS5803_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new MeaSpecMS5803_Pressure object.
     *
     * @param parentSense The parent MeaSpecMS5803 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of th a default value of
     * MeaSpecMS5803Pressure
     */
    explicit MeaSpecMS5803_Pressure(
        MeaSpecMS5803* parentSense, const char* uuid = "",
        const char* varCode = "MeaSpecMS5803Pressure")
        : Variable(parentSense, (const uint8_t)MS5803_PRESSURE_VAR_NUM,
                   (uint8_t)MS5803_PRESSURE_RESOLUTION, "barometricPressure",
                   "millibar", varCode, uuid) {}
    /**
     * @brief Construct a new MeaSpecMS5803_Pressure object.
     *
     * @note This must be tied with a parent MeaSpecMS5803 before it can be
     * used.
     */
    MeaSpecMS5803_Pressure()
        : Variable((const uint8_t)MS5803_PRESSURE_VAR_NUM,
                   (uint8_t)MS5803_PRESSURE_RESOLUTION, "barometricPressure",
                   "millibar", "MeaSpecMS5803Pressure") {}
    /**
     * @brief Destroy the MeaSpecMS5803_Pressure object - no action needed.
     */
    ~MeaSpecMS5803_Pressure() {}
};


#endif  // SRC_SENSORS_MEASPECMS5803_H_
