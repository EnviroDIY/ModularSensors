/**
 * @file TEConnectivityMS5837.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TEConnectivityMS5837 sensor subclass and the variable
 * subclasses TEConnectivityMS5837_Temp and TEConnectivityMS5837_Pressure.
 *
 * These are for the TE Connectivity MS5837 pressure sensor. This sensor is
 * commonly deployed in Blue Robotics Bar02/Bar30 pressure sensors for
 * underwater/high-pressure applications and is commonly used for depth
 * measurement. Blue Robotics maintains the MS5837 Arduino library for
 * communication with this sensor.
 *
 * This depends on the Blue Robotics MS5837 library at
 * https://github.com/bluerobotics/BlueRobotics_MS5837_Library
 */
/* clang-format off */
/**
 * @defgroup sensor_ms5837 TE Connectivity MS5837
 * Classes for the TE Connectivity MS5837 digital pressure sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_ms5837_intro Introduction
 *
 * The TE Connectivity MS5837 is a miniature digital pressure sensor designed for
 * underwater and high-pressure applications. It is commonly deployed in Blue Robotics
 * Bar02/Bar30 pressure sensors and is frequently used for depth measurement.
 * The MS5837 comes in several different pressure ranges, with 2 bar and 30 bar being
 * the most common for underwater applications. The sensor communicates via I2C at
 * address 0x76. These sensors should be attached to a 1.7-3.6V power source and the
 * power supply to the sensor can be stopped between measurements.
 *
 * @warning The I2C address (0x76) is the same as some configurations of the
 * Measurement Specialties MS5803, Bosch BME280, BMP388, and BMP390 sensors!
 * If you are also using one of those sensors, make sure that the address for
 * that sensor does not conflict with the address of this sensor.
 *
 * @note Neither secondary hardware nor software I2C is supported for the MS5837.
 * Only the primary hardware I2C defined in the Arduino core can be used.
 *
 * The lower level communication between the mcu and the MS5837 is handled by the
 * [Blue Robotics MS5837 library](https://github.com/bluerobotics/BlueRobotics_MS5837_Library).
 *
 * @section sensor_ms5837_datasheet Sensor Datasheet
 *
 * Documentation for the sensor can be found at:
 * https://www.te.com/en/product-CAT-BLPS0017.html
 *
 * Blue Robotics deployable versions:
 * https://bluerobotics.com/store/sensors-sonars-cameras/sensors/bar02-sensor-r1-rp/
 * https://bluerobotics.com/store/sensors-sonars-cameras/sensors/bar30-sensor-r1/
 *
 * @section sensor_ms5837_ctor Sensor Constructor
 * {{ @ref TEConnectivityMS5837::TEConnectivityMS5837 }}
 *
 * ___
 * @section sensor_ms5837_examples Example Code
 * The TE Connectivity MS5837 is used in the @menulink{te_connectivity_ms5837} example.
 *
 * @menusnip{te_connectivity_ms5837}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_TECONNECTIVITYMS5837_H_
#define SRC_SENSORS_TECONNECTIVITYMS5837_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_TECONNECTIVITYMS5837_DEBUG
#define MS_DEBUGGING_STD "TEConnectivityMS5837"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include <MS5837.h>

/** @ingroup sensor_ms5837 */
/**@{*/

/**
 * @anchor sensor_ms5837_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by the MS5837
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the MS5837 can report 2 values.
#define MS5837_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define MS5837_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_ms5837_timing
 * @name Sensor Timing
 * The sensor timing for a Blue Robotics MS5837
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the MS5837 warms up in 10ms.
#define MS5837_WARM_UP_TIME_MS 10
/// @brief Sensor::_stabilizationTime_ms; the MS5837 is stable as soon as it
/// warms up (0ms stabilization).
#define MS5837_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the MS5837 takes 20ms to complete a
 * measurement.
 * - Sensor takes about 0.5 / 1.1 / 2.1 / 4.1 / 8.22 ms to respond
 * at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
 */
#define MS5837_MEASUREMENT_TIME_MS 20
/**@}*/

/**
 * @anchor sensor_ms5837_temp
 * @name Temperature
 * The temperature variable from a TE Connectivity MS5837
 * - Range is -40°C to +85°C
 * - Accuracy is ±2.0°C
 *
 * {{ @ref TEConnectivityMS5837_Temp::TEConnectivityMS5837_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is <0.01°C.
#define MS5837_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[0].
#define MS5837_TEMP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define MS5837_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define MS5837_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "TEConnectivityMS5837Temp"
#define MS5837_TEMP_DEFAULT_CODE "TEConnectivityMS5837Temp"
/**@}*/

/**
 * @anchor sensor_ms5837_pressure
 * @name Pressure
 * The pressure variable from a TE Connectivity MS5837
 *   - Range depends on sensor model:
 *     - Bar02: 0 to 2 bar
 *     - Bar30: 0 to 30 bar
 *   - Accuracy:
 *     - Bar02: ±1.5mbar
 *     - Bar30: ±20mbar
 *   - Resolution is: (at oversampling ratios: 256 / 512 / 1024 / 2048 /
 * 4096, respectively))
 *     - Bar02: 0.13 / 0.084 / 0.054 / 0.036 / 0.024 mbar
 *     - Bar30: 1 / 0.6 / 0.4 / 0.3 / 0.2 mbar (where 1 mbar = 100 pascals)
 *     - @m_span{m-dim}@ref #MS5837_PRESSURE_RESOLUTION = 3@m_endspan
 *
 * {{ @ref TEConnectivityMS5837_Pressure::TEConnectivityMS5837_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; pressure should have 3.
#define MS5837_PRESSURE_RESOLUTION 3
/// @brief Sensor variable number; pressure is stored in sensorValues[1].
#define MS5837_PRESSURE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "barometricPressure"
#define MS5837_PRESSURE_VAR_NAME "barometricPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "millibar"
#define MS5837_PRESSURE_UNIT_NAME "millibar"
/// @brief Default variable short code; "TEConnectivityMS5837Pressure"
#define MS5837_PRESSURE_DEFAULT_CODE "TEConnectivityMS5837Pressure"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [TE Connectivity MS5837 sensor](@ref sensor_ms5837).
 *
 * @ingroup sensor_ms5837
 */
/* clang-format on */
class TEConnectivityMS5837 : public Sensor {
 public:
    /**
     * @brief Construct a new TEConnectivityMS5837 object.
     *
     * @note Neither secondary hardware nor software I2C is supported for the
     * MS5837. Only the primary hardware I2C defined in the Arduino core can be
     * used.
     *
     * @param powerPin The pin on the mcu controlling power to the MS5837
     * Use -1 if it is continuously powered.
     * - The MS5837 requires a 1.7 - 3.6V power source
     * @param model The model of MS5837 sensor. Use MS5837::MS5837_02BA for
     * 2 bar range sensors or MS5837::MS5837_30BA for 30 bar range sensors.
     * Default is MS5837::MS5837_30BA.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit TEConnectivityMS5837(int8_t  powerPin,
                                  uint8_t model = MS5837::MS5837_02BA,
                                  uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the TEConnectivityMS5837 object
     */
    ~TEConnectivityMS5837();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the pin modes for #_powerPin and I2C, initializes the MS5837,
     * and updates the #_sensorStatus.  The MS5837 must be powered for setup.
     *
     * @return True if the setup was successful.
     */
    bool setup(void) override;

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief Private internal reference to the MS5837 object.
     */
    MS5837 MS5837_internal;
    /**
     * @brief The model of the MS5837.
     */
    uint8_t _model;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_ms5837_temp) from a
 * [TE Connectivity MS5837 digital pressure sensor](@ref sensor_ms5837).
 *
 * @ingroup sensor_ms5837
 */
/* clang-format on */
class TEConnectivityMS5837_Temp : public Variable {
 public:
    /**
     * @brief Construct a new TEConnectivityMS5837_Temp object.
     *
     * @param parentSense The parent TEConnectivityMS5837 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TEConnectivityMS5837Temp".
     */
    explicit TEConnectivityMS5837_Temp(
        TEConnectivityMS5837* parentSense, const char* uuid = "",
        const char* varCode = MS5837_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)MS5837_TEMP_VAR_NUM,
                   (uint8_t)MS5837_TEMP_RESOLUTION, MS5837_TEMP_VAR_NAME,
                   MS5837_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new TEConnectivityMS5837_Temp object.
     *
     * @note This must be tied with a parent TEConnectivityMS5837 before it can
     * be used.
     */
    TEConnectivityMS5837_Temp()
        : Variable((uint8_t)MS5837_TEMP_VAR_NUM,
                   (uint8_t)MS5837_TEMP_RESOLUTION, MS5837_TEMP_VAR_NAME,
                   MS5837_TEMP_UNIT_NAME, MS5837_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TEConnectivityMS5837_Temp object - no action needed.
     */
    ~TEConnectivityMS5837_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pressure output](@ref sensor_ms5837_pressure) from a
 * [TE Connectivity MS5837 digital pressure sensor](@ref sensor_ms5837).
 *
 * @ingroup sensor_ms5837
 */
/* clang-format on */
class TEConnectivityMS5837_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new TEConnectivityMS5837_Pressure object.
     *
     * @param parentSense The parent TEConnectivityMS5837 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TEConnectivityMS5837Pressure".
     */
    explicit TEConnectivityMS5837_Pressure(
        TEConnectivityMS5837* parentSense, const char* uuid = "",
        const char* varCode = MS5837_PRESSURE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)MS5837_PRESSURE_VAR_NUM,
                   (uint8_t)MS5837_PRESSURE_RESOLUTION,
                   MS5837_PRESSURE_VAR_NAME, MS5837_PRESSURE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new TEConnectivityMS5837_Pressure object.
     *
     * @note This must be tied with a parent TEConnectivityMS5837 before it can
     * be used.
     */
    TEConnectivityMS5837_Pressure()
        : Variable((uint8_t)MS5837_PRESSURE_VAR_NUM,
                   (uint8_t)MS5837_PRESSURE_RESOLUTION,
                   MS5837_PRESSURE_VAR_NAME, MS5837_PRESSURE_UNIT_NAME,
                   MS5837_PRESSURE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TEConnectivityMS5837_Pressure object - no action
     * needed.
     */
    ~TEConnectivityMS5837_Pressure() {}
};
/**@}*/
#endif  // SRC_SENSORS_TECONNECTIVITYMS5837_H_
