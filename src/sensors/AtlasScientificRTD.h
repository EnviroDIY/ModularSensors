/**
 * @file AtlasScientificRTD.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificRTD subclass of the AtlasParent sensor
 * class along with the variable subclass AtlasScientificRTD_Temp.
 *
 * These are used for any sensor attached to an [Atlas EZO RTD
 * circuit](https://www.atlas-scientific.com/temperature.html).
 *
 * @copydetails AtlasScientificRTD
 */
/* clang-format off */
/**
 * @defgroup sensor_atlas_rtd Atlas EZO-RTD
 * Classes for the Atlas Scientific EZO-RTD temperature circuit and probes.
 *
 * @ingroup atlas_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_atlas_rtd_datasheet Sensor Datasheet
 *
 * Documentation on the probe is found here:
 * https://www.atlas-scientific.com/probes/dissolved-oxygen-probe/
 *
 * Documentation on the measurement circuit is found here:
 * https://www.atlas-scientific.com/circuits/ezo-dissolved-oxygen-circuit/
 *
 * @section sensor_atlas_rtd_ctor Sensor Constructor
 * {{ @ref AtlasScientificRTD::AtlasScientificRTD(int8_t, uint8_t, uint8_t) }}
 * {{ @ref AtlasScientificRTD::AtlasScientificRTD(TwoWire*, int8_t, uint8_t, uint8_t) }}
 *
 * ___
 * @section sensor_atlas_rtd_examples Example Code
 * The Atlas RTD sensor is used in the @menulink{atlas_scientific_rtd} example.
 *
 * @menusnip{atlas_scientific_rtd}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICRTD_H_
#define SRC_SENSORS_ATLASSCIENTIFICRTD_H_

// Included Dependencies
#include "sensors/AtlasParent.h"


/**
 * @brief Default I2C address is 0x66 (102)
 */
#define ATLAS_RTD_I2C_ADDR 0x66

// Sensor Specific Defines
/** @ingroup sensor_atlas_rtd */
/**@{*/
/**
 * @brief Sensor::_numReturnedValues; the Atlas EZO temperature circuit can
 * report 1 value.
 */
#define ATLAS_RTD_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define ATLAS_RTD_INC_CALC_VARIABLES 0


/**
 * @anchor sensor_atlas_rtd_timing
 * @name Sensor Timing
 * The sensor timing for an Atlas RTD (temperature) sensor
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the Atlas EZO temperature circuit warms up in
 * 740ms
 *
 * 731-735 in tests
 */
#define ATLAS_RTD_WARM_UP_TIME_MS 740
/**
 * @brief Sensor::_stabilizationTime_ms; the Atlas EZO temperature circuit is
 * stable 0ms after warm-up (stable at completion of warm up).
 */
#define ATLAS_RTD_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas EZO temperature circuit takes
 * 650ms to complete a measurement.
 *
 * @note Manual says measurement takes 600 ms, but in SRGD tests, didn't get a
 * result until after 643 ms; AG got results as soon as 393ms.
 */
#define ATLAS_RTD_MEASUREMENT_TIME_MS 650
/**@}*/

/**
 * @anchor sensor_atlas_rtd_temp
 * @name Temperature
 * The temperature variable from an Atlas RTD (temperature) sensor
 * - Accuracy is ± (0.10°C + 0.0017 x °C)
 * - Range is -126°C − 125°C
 *
 * {{ @ref AtlasScientificRTD_Temp::AtlasScientificRTD_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 3 -
/// resolution is 0.001°C.
#define ATLAS_RTD_RESOLUTION 3
/// @brief Sensor variable number; RTD is stored in sensorValues[0].
#define ATLAS_RTD_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define ATLAS_RTD_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define ATLAS_RTD_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "AtlasTemp"
#define ATLAS_RTD_DEFAULT_CODE "AtlasTemp"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific RTD temperature sensor](@ref sensor_atlas_rtd).
 *
 * @ingroup sensor_atlas_rtd
 */
/* clang-format on */
class AtlasScientificRTD : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific RTD object using a secondary
     * *hardware* I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling powering to the Atlas RTD
     * (temperature) circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x66.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    AtlasScientificRTD(TwoWire* theI2C, int8_t powerPin,
                       uint8_t i2cAddressHex         = ATLAS_RTD_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
        : AtlasParent(
              theI2C, powerPin, i2cAddressHex, measurementsToAverage,
              "AtlasScientificRTD", ATLAS_RTD_NUM_VARIABLES,
              ATLAS_RTD_WARM_UP_TIME_MS, ATLAS_RTD_STABILIZATION_TIME_MS,
              ATLAS_RTD_MEASUREMENT_TIME_MS, ATLAS_RTD_INC_CALC_VARIABLES) {}
    /**
     * @brief Construct a new Atlas Scientific RTD object using the primary
     * hardware I2C instance.
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas RTD
     * (temperature) circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x66.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit AtlasScientificRTD(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_RTD_I2C_ADDR,
                                uint8_t measurementsToAverage = 1)
        : AtlasParent(
              powerPin, i2cAddressHex, measurementsToAverage,
              "AtlasScientificRTD", ATLAS_RTD_NUM_VARIABLES,
              ATLAS_RTD_WARM_UP_TIME_MS, ATLAS_RTD_STABILIZATION_TIME_MS,
              ATLAS_RTD_MEASUREMENT_TIME_MS, ATLAS_RTD_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Atlas Scientific RTD object
     */
    ~AtlasScientificRTD() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_atlas_rtd_temp) from an
 * [Atlas Scientific RTD temperature sensor](@ref sensor_atlas_rtd).
 *
 * @ingroup sensor_atlas_rtd
 */
/* clang-format on */
class AtlasScientificRTD_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificRTD_Temp object.
     *
     * @param parentSense The parent AtlasScientificRTD providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasTemp".
     */
    explicit AtlasScientificRTD_Temp(
        AtlasScientificRTD* parentSense, const char* uuid = "",
        const char* varCode = ATLAS_RTD_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_RTD_VAR_NUM,
                   (uint8_t)ATLAS_RTD_RESOLUTION, ATLAS_RTD_VAR_NAME,
                   ATLAS_RTD_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificRTD_Temp object.
     *
     * @note This must be tied with a parent AtlasScientificRTD before it can be
     * used.
     */
    AtlasScientificRTD_Temp()
        : Variable((const uint8_t)ATLAS_RTD_VAR_NUM,
                   (uint8_t)ATLAS_RTD_RESOLUTION, ATLAS_RTD_VAR_NAME,
                   ATLAS_RTD_UNIT_NAME, ATLAS_RTD_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificRTD_Temp object - no action needed.
     */
    ~AtlasScientificRTD_Temp() {}
};
/**@}*/
#endif  // SRC_SENSORS_ATLASSCIENTIFICRTD_H_
