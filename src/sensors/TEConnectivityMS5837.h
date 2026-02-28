/**
 * @file TEConnectivityMS5837.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TEConnectivityMS5837 sensor subclass and the variable
 * subclasses TEConnectivityMS5837_Temp, TEConnectivityMS5837_Pressure,
 * TEConnectivityMS5837_Depth, and TEConnectivityMS5837_Altitude.
 *
 * These are for the TE Connectivity MS5837 pressure sensor. This sensor is
 * commonly deployed in Blue Robotics Bar02/Bar30 pressure sensors for
 * underwater/high-pressure applications and is commonly used for depth
 * measurement. Rob Tillaart maintains the MS5837 Arduino library for
 * communication with this sensor.
 *
 * This depends on the Rob Tillaart MS5837 library at
 * https://github.com/RobTillaart/MS5837
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
 * @note This sensor supports both primary and secondary hardware I2C instances
 * through TwoWire* constructor parameters. Software I2C is not supported.
 *
 * The lower level communication between the mcu and the MS5837 is handled by the
 * [Rob Tillaart MS5837 library](https://github.com/RobTillaart/MS5837).
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
 * @section sensor_ms5837_flags Build flags
 * - ```-D MS5837_DEFAULT_FLUID_DENSITY=0.99802f```
 *      - Changes the default fluid density used for depth calculations. The
 * default value is for water at 20°C. For seawater, use approximately 1.025f.
 * For other fluids, consult density tables and enter the density in grams per
 * cm³.
 * - ```-D MS_SEA_LEVEL_PRESSURE_HPA=1013.25f```
 *      - Changes the default air pressure used for altitude and depth
 * calculations. The default value is standard atmospheric pressure at sea level
 * (1013.25 mBar). Adjust based on local atmospheric conditions or altitude for
 * more accurate depth measurements.
 *      - The same sea level pressure flag is used for BMP3xx, BME280, and
 * MS5837 sensors.
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
 * @anchor sensor_ms5837_config
 * @name Sensor Configuration
 * Build-time configuration for the MS5837
 */
/**@{*/
#if !defined(MS5837_DEFAULT_FLUID_DENSITY) || defined(DOXYGEN)
/**
 * @brief Default fluid density for depth calculations (grams/cm³)
 *
 * Water at 20°C = 0.99802 g/cm³. For seawater, use approximately 1.025 g/cm³.
 * For other fluids, consult density tables and enter the density in grams per
 * cm³. This can be overridden at compile time with -D
 * MS5837_DEFAULT_FLUID_DENSITY=value
 */
#define MS5837_DEFAULT_FLUID_DENSITY 0.99802f
#endif

// Static assert to validate fluid density is reasonable
static_assert(MS5837_DEFAULT_FLUID_DENSITY > 0.1f &&
                  MS5837_DEFAULT_FLUID_DENSITY <= 5.0f,
              "MS5837_DEFAULT_FLUID_DENSITY must be between 0.1 and 5.0 g/cm³ "
              "(reasonable fluid density range)");

#if !defined(MS5837_DEFAULT_OVERSAMPLING_RATIO) || defined(DOXYGEN)
/**
 * @brief Default oversampling ratio for pressure and temperature measurements
 *
 * Higher oversampling ratios provide better resolution and noise reduction but
 * increase measurement time. Valid values are: 256, 512, 1024, 2048, 4096,
 * 8192. Default is 4096 for good balance of accuracy and speed. This can be
 * overridden at compile time with -D MS5837_DEFAULT_OVERSAMPLING_RATIO=value
 */
#define MS5837_DEFAULT_OVERSAMPLING_RATIO 4096
#endif

// Static assert to validate oversampling ratio is one of the valid values
static_assert(
    MS5837_DEFAULT_OVERSAMPLING_RATIO == 256 ||
        MS5837_DEFAULT_OVERSAMPLING_RATIO == 512 ||
        MS5837_DEFAULT_OVERSAMPLING_RATIO == 1024 ||
        MS5837_DEFAULT_OVERSAMPLING_RATIO == 2048 ||
        MS5837_DEFAULT_OVERSAMPLING_RATIO == 4096 ||
        MS5837_DEFAULT_OVERSAMPLING_RATIO == 8192,
    "MS5837_DEFAULT_OVERSAMPLING_RATIO must be one of: 256, 512, 1024, "
    "2048, 4096, 8192 (valid MS5837 oversampling ratios)");
/**@}*/

/**
 * @anchor sensor_ms5837_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by the MS5837
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the MS5837 can report 4 values.
#define MS5837_NUM_VARIABLES 4
/// @brief Sensor::_incCalcValues; we calculate depth and altitude values.
#define MS5837_INC_CALC_VARIABLES 2
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

/**
 * @anchor sensor_ms5837_depth
 * @name Depth
 * The depth variable calculated from a TE Connectivity MS5837
 * - Calculated from pressure using the configured fluid density
 * - Accuracy depends on pressure sensor accuracy and fluid density accuracy
 * - Resolution is 1mm (0.001m)
 *
 * {{ @ref TEConnectivityMS5837_Depth::TEConnectivityMS5837_Depth }}
 */
/**@{*/
/// @brief Decimals places in string representation; depth should have 3.
#define MS5837_DEPTH_RESOLUTION 3
/// @brief Sensor variable number; depth is stored in sensorValues[2].
#define MS5837_DEPTH_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "waterDepth"
#define MS5837_DEPTH_VAR_NAME "waterDepth"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "meter"
#define MS5837_DEPTH_UNIT_NAME "meter"
/// @brief Default variable short code; "TEConnectivityMS5837Depth"
#define MS5837_DEPTH_DEFAULT_CODE "TEConnectivityMS5837Depth"
/**@}*/

/**
 * @anchor sensor_ms5837_altitude
 * @name Altitude
 * The altitude variable calculated from a TE Connectivity MS5837
 * - Calculated from barometric pressure using standard atmosphere equations
 * - Accuracy depends on pressure sensor accuracy and reference air pressure
 * - Resolution is 0.01m
 *
 * {{ @ref TEConnectivityMS5837_Altitude::TEConnectivityMS5837_Altitude }}
 */
/**@{*/
/// @brief Decimals places in string representation; altitude should have 2.
#define MS5837_ALTITUDE_RESOLUTION 2
/// @brief Sensor variable number; altitude is stored in sensorValues[3].
#define MS5837_ALTITUDE_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "altitude"
#define MS5837_ALTITUDE_VAR_NAME "altitude"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "meter"
#define MS5837_ALTITUDE_UNIT_NAME "meter"
/// @brief Default variable short code; "TEConnectivityMS5837Altitude"
#define MS5837_ALTITUDE_DEFAULT_CODE "TEConnectivityMS5837Altitude"
/**@}*/

/**
 * @brief Supported MS5837/MS5803 sensor models
 *
 * These enum values correspond to the **math model** values used in the Rob
 * Tillaart MS5837 library.  They are **not** equivalent to the "type" values
 * defined in that library, which are not used in the MS5837 class.  The math
 * model values are used to set the correct calibration coefficients and
 * calculations for the different sensor models, which have different pressure
 * ranges and sensitivities.
 *
 * @ingroup sensor_ms5837
 */
enum class MS5837Model : uint8_t {
    MS5837_30BA = 0,  ///< MS5837-30BA: 30 bar range sensor
    MS5837_02BA = 1,  ///< MS5837-02BA: 2 bar range sensor
    MS5803_01BA = 2   ///< MS5803-01BA: 1 bar range sensor
};

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
     * @brief Construct a new TEConnectivityMS5837 object using the default
     * Hardware Wire instance.
     *
     * @param powerPin The pin on the mcu controlling power to the MS5837
     * Use -1 if it is continuously powered.
     * - The MS5837 requires a 1.7 - 3.6V power source
     * @param model The model of MS5837 sensor.
     *   - 0 for 30 bar range sensors (MS5837-30BA)
     *   - 1 for 2 bar range sensors (MS5837-02BA)
     *   - 2 for 1 bar range sensors (MS5803-01BA)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param fluidDensity The density of the fluid for depth calculations
     * (grams/cm³); optional with default value from
     * MS5837_DEFAULT_FLUID_DENSITY.
     * @param airPressure The air pressure for altitude/depth calculations
     * (mBar); optional with default value from MS_SEA_LEVEL_PRESSURE_HPA.
     * @param overSamplingRatio The oversampling ratio for pressure and
     * temperature measurements; optional with default value from
     * MS5837_DEFAULT_OVERSAMPLING_RATIO. Valid values: 256, 512, 1024, 2048,
     * 4096, 8192.
     *
     * @warning This can be used for the MS5803-01BA sensor, but **only** for
     * that exact model of MS5803.  For any other MS5803 model, use the
     * MeasSpecMS5803 class instead of this class.
     */
    explicit TEConnectivityMS5837(
        int8_t powerPin, uint8_t model, uint8_t measurementsToAverage = 1,
        uint16_t overSamplingRatio = MS5837_DEFAULT_OVERSAMPLING_RATIO,
        float    fluidDensity      = MS5837_DEFAULT_FLUID_DENSITY,
        float    airPressure       = MS_SEA_LEVEL_PRESSURE_HPA);
    /**
     * @brief Construct a new TEConnectivityMS5837 object using a secondary
     * *hardware* I2C instance.
     *
     * @copydetails TEConnectivityMS5837::TEConnectivityMS5837(int8_t, uint8_t,
     * uint8_t, uint16_t, float, float)
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     */
    TEConnectivityMS5837(
        TwoWire* theI2C, int8_t powerPin, uint8_t model,
        uint8_t  measurementsToAverage = 1,
        uint16_t overSamplingRatio     = MS5837_DEFAULT_OVERSAMPLING_RATIO,
        float    fluidDensity          = MS5837_DEFAULT_FLUID_DENSITY,
        float    airPressure           = MS_SEA_LEVEL_PRESSURE_HPA);

    /**
     * @brief Construct a new TEConnectivityMS5837 object using the default
     * Hardware Wire instance with enum model type.
     *
     * @param powerPin The pin on the mcu controlling power to the MS5837
     * Use -1 if it is continuously powered.
     * - The MS5837 requires a 1.7 - 3.6V power source
     * @param model The model of MS5837 sensor using enum type.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param overSamplingRatio The oversampling ratio for pressure and
     * temperature measurements; optional with default value from
     * MS5837_DEFAULT_OVERSAMPLING_RATIO. Valid values: 256, 512, 1024, 2048,
     * 4096, 8192.
     * @param fluidDensity The density of the fluid for depth calculations
     * (grams/cm³); optional with default value from
     * MS5837_DEFAULT_FLUID_DENSITY.
     * @param airPressure The air pressure for altitude/depth calculations
     * (mBar); optional with default value from MS_SEA_LEVEL_PRESSURE_HPA.
     *
     * @warning This can be used for the MS5803-01BA sensor, but **only** for
     * that exact model of MS5803.  For any other MS5803 model, use the
     * MeasSpecMS5803 class instead of this class.
     */
    explicit TEConnectivityMS5837(
        int8_t powerPin, MS5837Model model, uint8_t measurementsToAverage = 1,
        uint16_t overSamplingRatio = MS5837_DEFAULT_OVERSAMPLING_RATIO,
        float    fluidDensity      = MS5837_DEFAULT_FLUID_DENSITY,
        float    airPressure       = MS_SEA_LEVEL_PRESSURE_HPA);
    /**
     * @brief Construct a new TEConnectivityMS5837 object using a secondary
     * *hardware* I2C instance.
     *
     * @copydetails TEConnectivityMS5837::TEConnectivityMS5837(int8_t,
     * MS5837Model, uint8_t, uint16_t, float, float)
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     */
    TEConnectivityMS5837(
        TwoWire* theI2C, int8_t powerPin, MS5837Model model,
        uint8_t  measurementsToAverage = 1,
        uint16_t overSamplingRatio     = MS5837_DEFAULT_OVERSAMPLING_RATIO,
        float    fluidDensity          = MS5837_DEFAULT_FLUID_DENSITY,
        float    airPressure           = MS_SEA_LEVEL_PRESSURE_HPA);
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

    /**
     * @brief Wake the sensor and re-establish communication.
     *
     * This re-runs the MS5837_internal begin method to re-establish I2C
     * communication, re-read the sensor calibration constants, and ensure that
     * the sensor itself has loaded the calibration PROM into its internal
     * register.  This is required after every power cycle of the sensor.
     *
     * @return True if the wake was successful.
     */
    bool wake(void) override;

    String getSensorName(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief Private internal reference to the MS5837 object.
     */
    MS5837 MS5837_internal;
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _wire;  // Hardware Wire
    /**
     * @brief The model of the MS5837.
     */
    uint8_t _model;
    /**
     * @brief The fluid density for depth calculations (grams/cm³).
     */
    float _fluidDensity;
    /**
     * @brief The air pressure for altitude/depth calculations (mBar).
     */
    float _airPressure;
    /**
     * @brief The oversampling ratio for pressure and temperature measurements.
     */
    uint16_t _overSamplingRatio;

    /**
     * @brief Validates the configured sensor model against hardware and
     * corrects it if a mismatch is detected.
     *
     * This method reads the SENS_T1 calibration value from the sensor's PROM
     * and compares it against known sensitivity thresholds to determine if the
     * configured model matches the actual hardware. If a mismatch is detected
     * and the correct model can be determined, the model configuration is
     * automatically updated.
     *
     * @note This will only change the configuration if a valid SENS_T1 value is
     * returned, one of the MS5837 models is currently configured, and the
     * SENS_T1 value indicates the other MS5837 model based on experimentally
     * derived sensitivity thresholds. If the SENS_T1 cannot be retrieved, the
     * value is out of the expected range for both models, or a MS5803 is
     * configured, no changes will be made.
     *
     * The thresholds used for determining whether to change the model
     * configuration are taken from the Blue Robotics MS5837 library and are
     * based on experimental results posted here:
     * https://github.com/ArduPilot/ardupilot/pull/29122#issuecomment-2877269114
     *
     * @return True if the model value was changed based on the returned SENS_T1
     * value, false otherwise.
     */
    bool validateAndCorrectModel();
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


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [depth output](@ref sensor_ms5837_depth) calculated from a
 * [TE Connectivity MS5837 digital pressure sensor](@ref sensor_ms5837).
 *
 * @ingroup sensor_ms5837
 */
/* clang-format on */
class TEConnectivityMS5837_Depth : public Variable {
 public:
    /**
     * @brief Construct a new TEConnectivityMS5837_Depth object.
     *
     * @param parentSense The parent TEConnectivityMS5837 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TEConnectivityMS5837Depth".
     */
    explicit TEConnectivityMS5837_Depth(
        TEConnectivityMS5837* parentSense, const char* uuid = "",
        const char* varCode = MS5837_DEPTH_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)MS5837_DEPTH_VAR_NUM,
                   (uint8_t)MS5837_DEPTH_RESOLUTION, MS5837_DEPTH_VAR_NAME,
                   MS5837_DEPTH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new TEConnectivityMS5837_Depth object.
     *
     * @note This must be tied with a parent TEConnectivityMS5837 before it can
     * be used.
     */
    TEConnectivityMS5837_Depth()
        : Variable((uint8_t)MS5837_DEPTH_VAR_NUM,
                   (uint8_t)MS5837_DEPTH_RESOLUTION, MS5837_DEPTH_VAR_NAME,
                   MS5837_DEPTH_UNIT_NAME, MS5837_DEPTH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TEConnectivityMS5837_Depth object - no action needed.
     */
    ~TEConnectivityMS5837_Depth() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [altitude output](@ref sensor_ms5837_altitude) calculated from a
 * [TE Connectivity MS5837 digital pressure sensor](@ref sensor_ms5837).
 *
 * @ingroup sensor_ms5837
 */
/* clang-format on */
class TEConnectivityMS5837_Altitude : public Variable {
 public:
    /**
     * @brief Construct a new TEConnectivityMS5837_Altitude object.
     *
     * @param parentSense The parent TEConnectivityMS5837 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TEConnectivityMS5837Altitude".
     */
    explicit TEConnectivityMS5837_Altitude(
        TEConnectivityMS5837* parentSense, const char* uuid = "",
        const char* varCode = MS5837_ALTITUDE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)MS5837_ALTITUDE_VAR_NUM,
                   (uint8_t)MS5837_ALTITUDE_RESOLUTION,
                   MS5837_ALTITUDE_VAR_NAME, MS5837_ALTITUDE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new TEConnectivityMS5837_Altitude object.
     *
     * @note This must be tied with a parent TEConnectivityMS5837 before it can
     * be used.
     */
    TEConnectivityMS5837_Altitude()
        : Variable((uint8_t)MS5837_ALTITUDE_VAR_NUM,
                   (uint8_t)MS5837_ALTITUDE_RESOLUTION,
                   MS5837_ALTITUDE_VAR_NAME, MS5837_ALTITUDE_UNIT_NAME,
                   MS5837_ALTITUDE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TEConnectivityMS5837_Altitude object - no action
     * needed.
     */
    ~TEConnectivityMS5837_Altitude() {}
};
/**@}*/
#endif  // SRC_SENSORS_TECONNECTIVITYMS5837_H_
