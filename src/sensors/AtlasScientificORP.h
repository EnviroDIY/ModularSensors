/**
 * @file AtlasScientificORP.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificRTD subclass of the AtlasParent sensor
 * class along with the variable subclass AtlasScientificORP_Potential.
 *
 * These are used for any sensor attached to an Atlas EZO ORP circuit.
 *
 * @copydetails AtlasScientificORP
 */
/* clang-format off */
/**
 * @defgroup sensor_atlas_orp Atlas EZO-ORP
 * Classes for the Atlas Scientific EZO-ORP oxidation/reduction potential circuit and probes.
 *
 * @ingroup atlas_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_atlas_orp_datasheet Sensor Datasheet
 * Documentation on the circuit is available here:
 * https://www.atlas-scientific.com/circuits/ezo-orp-circuit/
 *
 * @section sensor_atlas_orp_ctor Sensor Constructors
 * {{ @ref AtlasScientificORP::AtlasScientificORP(int8_t, uint8_t, uint8_t) }}
 * {{ @ref AtlasScientificORP::AtlasScientificORP(TwoWire*, int8_t, uint8_t, uint8_t) }}
 *
 * ___
 * @section sensor_atlas_orp_examples Example Code
 * The Atlas ORP sensor is used in the @menulink{atlas_scientific_orp} example.
 *
 * @menusnip{atlas_scientific_orp}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICORP_H_
#define SRC_SENSORS_ATLASSCIENTIFICORP_H_

// Included Dependencies
#include "sensors/AtlasParent.h"


// Sensor Specific Defines
/** @ingroup sensor_atlas_orp */
/**@{*/

/// @brief Default I2C address is 0x62 (98)
#define ATLAS_ORP_I2C_ADDR 0x62

/// @brief Sensor::_numReturnedValues; the Atlas EZO ORP circuit can report 1
/// value.
#define ATLAS_ORP_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define ATLAS_ORP_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_atlas_orp_timing
 * @name Sensor Timing
 * The sensor timing for an Atlas ORP (redox) sensor
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the Atlas EZO ORP circuit warms up in 850ms
 *
 * 846 in SRGD tests
 */
#define ATLAS_ORP_WARM_UP_TIME_MS 850
/**
 * @brief Sensor::_stabilizationTime_ms; the Atlas EZO ORP circuit is stable 0ms
 * after warm-up (stable at completion of warm up).
 */
#define ATLAS_ORP_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas EZO ORP circuit takes
 * 1580ms to complete a measurement.
 *
 * @note Manual says measurement takes 900 ms, but in SRGD tests, no result was
 * available until after 1577 ms.
 */
#define ATLAS_ORP_MEASUREMENT_TIME_MS 1580
/**@}*/

/**
 * @anchor sensor_atlas_orp_orp
 * @name ORP
 * The ORP variable from an Atlas ORP (redox) sensor
 * - Accuracy is ± 1 mV
 * - Range is -1019.9mV − 1019.9mV
 *
 * {{ @ref AtlasScientificORP_Potential::AtlasScientificORP_Potential }}
 */
/**@{*/
/// @brief Decimals places in string representation; ORP should have 1 -
/// resolution is 0.1 mV.
#define ATLAS_ORP_RESOLUTION 1
/// @brief Sensor variable number; ORP is stored in sensorValues[0].
#define ATLAS_ORP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "reductionPotential"
#define ATLAS_ORP_VAR_NAME "reductionPotential"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "millivolt"
/// (mV)
#define ATLAS_ORP_UNIT_NAME "millivolt"
/// @brief Default variable short code; "AtlasORP"
#define ATLAS_ORP_DEFAULT_CODE "AtlasORP"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific ORP (oxidation/reduction potential) sensor](@ref sensor_atlas_orp).
 *
 * @ingroup sensor_atlas_orp
 */
/* clang-format on */
class AtlasScientificORP : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific ORP object using a secondary
     * *hardware* I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling powering to the Atlas ORP
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x62.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    AtlasScientificORP(TwoWire* theI2C, int8_t powerPin,
                       uint8_t i2cAddressHex         = ATLAS_ORP_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
        : AtlasParent(
              theI2C, powerPin, i2cAddressHex, measurementsToAverage,
              "AtlasScientificORP", ATLAS_ORP_NUM_VARIABLES,
              ATLAS_ORP_WARM_UP_TIME_MS, ATLAS_ORP_STABILIZATION_TIME_MS,
              ATLAS_ORP_MEASUREMENT_TIME_MS, ATLAS_ORP_INC_CALC_VARIABLES) {}
    /**
     * @brief Construct a new Atlas Scientific ORP object using the primary
     * hardware I2C instance.
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas ORP
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x62.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit AtlasScientificORP(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_ORP_I2C_ADDR,
                                uint8_t measurementsToAverage = 1)
        : AtlasParent(
              powerPin, i2cAddressHex, measurementsToAverage,
              "AtlasScientificORP", ATLAS_ORP_NUM_VARIABLES,
              ATLAS_ORP_WARM_UP_TIME_MS, ATLAS_ORP_STABILIZATION_TIME_MS,
              ATLAS_ORP_MEASUREMENT_TIME_MS, ATLAS_ORP_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Atlas Scientific ORP object
     */
    ~AtlasScientificORP() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [oxidation/reduction potential output](@ref sensor_atlas_orp_orp) from an
 * [Atlas Scientific EZO-ORP circuit](@ref sensor_atlas_orp).
 *
 * @ingroup sensor_atlas_orp
 */
/* clang-format on */
class AtlasScientificORP_Potential : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificORP_Potential object.
     *
     * @param parentSense The parent AtlasScientificORP providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasORP".
     */
    explicit AtlasScientificORP_Potential(
        AtlasScientificORP* parentSense, const char* uuid = "",
        const char* varCode = ATLAS_ORP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_ORP_VAR_NUM,
                   (uint8_t)ATLAS_ORP_RESOLUTION, ATLAS_ORP_VAR_NAME,
                   ATLAS_ORP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificORP_Potential object.
     *
     * @note This must be tied with a parent AtlasScientificORP before it can be
     * used.
     */
    AtlasScientificORP_Potential()
        : Variable((const uint8_t)ATLAS_ORP_VAR_NUM,
                   (uint8_t)ATLAS_ORP_RESOLUTION, ATLAS_ORP_VAR_NAME,
                   ATLAS_ORP_UNIT_NAME, ATLAS_ORP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificORP_Potential() object - no action
     * needed.
     */
    ~AtlasScientificORP_Potential() {}
};
/**@}*/
#endif  // SRC_SENSORS_ATLASSCIENTIFICORP_H_
