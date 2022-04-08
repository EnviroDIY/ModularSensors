/**
 * @file AtlasScientificDO.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificCO2 subclass of the AtlasParent sensor
 * class along with the variable subclasses AtlasScientificDO_DOmgL and
 * AtlasScientificDO_DOpct.
 *
 * These are used for any sensor attached to an [Atlas EZO DO
 * circuit](https://www.atlas-scientific.com/circuits/ezo-dissolved-oxygen-circuit/).
 */
/* clang-format off */
/**
 * @defgroup sensor_atlas_do Atlas EZO-DO
 * Classes for the Atlas Scientific EZO-DO dissolved oxygen circuit and probe.
 *
 * @ingroup atlas_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * The Atlas Scientifc DO sensor outputs DO in both mg/L and percent saturation.
 *
 * @section sensor_atlas_do_datasheet Sensor Datasheet
 *
 * Documentation on the probe is found here:
 * https://www.atlas-scientific.com/probes/dissolved-oxygen-probe/
 *
 * Documentation on the measurement circuit is found here:
 * https://www.atlas-scientific.com/circuits/ezo-dissolved-oxygen-circuit/
 *
 * @section sensor_atlas_do_ctor Sensor Constructors
 * {{ @ref AtlasScientificDO::AtlasScientificDO(int8_t, uint8_t, uint8_t) }}
 * {{ @ref AtlasScientificDO::AtlasScientificDO(TwoWire*, int8_t, uint8_t, uint8_t) }}
 *
 * ___
 * @section sensor_atlas_do_examples Example Code
 * The Atlas DO sensor is used in the @menulink{atlas_scientific_do} example.
 *
 * @menusnip{atlas_scientific_do}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICDO_H_
#define SRC_SENSORS_ATLASSCIENTIFICDO_H_

// Debugging Statement
// #define MS_ATLASSCIENTIFICDO_DEBUG

#ifdef MS_ATLASSCIENTIFICDO_DEBUG
#define MS_DEBUGGING_STD "AtlasScientificDO"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

/**
 * @brief Default I2C address is 0x61 (97)
 */
#define ATLAS_DO_I2C_ADDR 0x61

/** @ingroup sensor_atlas_do */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Atlas DO sensor can report 2 values.
#define ATLAS_DO_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define ATLAS_DO_INC_CALC_VARIABLES 0


/**
 * @anchor sensor_atlas_do_timing
 * @name Sensor Timing
 * The sensor timing for an Atlas DO sensor
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; the Atlas DO sensor warms up in 745ms
 *
 * 737-739 in tests
 */
#define ATLAS_DO_WARM_UP_TIME_MS 745
/**
 * @brief Sensor::_stabilizationTime_ms; the Atlas DO sensor is stable at
 * completion of warm up (0ms after warm-up).
 */
#define ATLAS_DO_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas DO sensor takes 600ms to
 * complete a measurement.
 *
 * only ~555 measurement time in tests, but keep the 600 recommended by manual
 */
#define ATLAS_DO_MEASUREMENT_TIME_MS 600
/**@}*/

/**
 * @anchor sensor_atlas_do_concentration
 * @name DO Concentration
 * The dissolved oxygen concentration from an Atlas DO sensor
 * - Accuracy is ± 0.05 mg/L
 * - Range is 0.01 − 100+ mg/L
 *
 * {{ @ref AtlasScientificDO_DOmgL::AtlasScientificDO_DOmgL }}
 */
/**@{*/
/// @brief Decimals places in string representation; dissolved oxygen
/// concentration should have 2 - resolution is 0.01 mg/L.
#define ATLAS_DOMGL_RESOLUTION 2
/// @brief Sensor variable number; dissolved oxygen concentration is stored in
/// sensorValues[0].
#define ATLAS_DOMGL_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "oxygenDissolved"
#define ATLAS_DOMGL_VAR_NAME "oxygenDissolved"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter" (mg/L)
#define ATLAS_DOMGL_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "AtlasDOmgL"
#define ATLAS_DOMGL_DEFAULT_CODE "AtlasDOmgL"
/**@}*/

/**
 * @anchor sensor_atlas_do_percent
 * @name DO Percent Saturation
 * The dissolved oxygen percent saturation for an Atlas DO sensor
 * - Accuracy is ± 0.05 mg/L
 * - Range is 0.1 − 400+ % saturation
 *
 * {{ @ref AtlasScientificDO_DOpct::AtlasScientificDO_DOpct }}
 */
/**@{*/
/// @brief Decimals places in string representation; dissolved oxygen percent
/// should have 1 - resolution is 0.1 % saturation.
#define ATLAS_DOPCT_RESOLUTION 1
/// @brief Sensor variable number; dissolved oxygen percent is stored in
/// sensorValues[1]
#define ATLAS_DOPCT_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "oxygenDissolvedPercentOfSaturation"
#define ATLAS_DOPCT_VAR_NAME "oxygenDissolvedPercentOfSaturation"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent"
/// (percent saturation)
#define ATLAS_DOPCT_UNIT_NAME "percent"
/// @brief Default variable short code; "AtlasDOpct"
#define ATLAS_DOPCT_DEFAULT_CODE "AtlasDOpct"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific EZO dissolved oxygen circuit](@ref sensor_atlas_do).
 *
 * @ingroup sensor_atlas_do
 */
/* clang-format on */
class AtlasScientificDO : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific DO object using a secondary
     * *hardware* I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling powering to the Atlas DO
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x61.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    AtlasScientificDO(TwoWire* theI2C, int8_t powerPin,
                      uint8_t i2cAddressHex         = ATLAS_DO_I2C_ADDR,
                      uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new Atlas Scientific DO object using the primary
     * hardware I2C instance.
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas DO
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x61.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit AtlasScientificDO(int8_t  powerPin,
                               uint8_t i2cAddressHex = ATLAS_DO_I2C_ADDR,
                               uint8_t measurementsToAverage = 1);

    /**
     * @brief Destroy the Atlas Scientific DO object
     */
    ~AtlasScientificDO();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C), tells the O2
     * circuit to report all possible measurement parameters, and sets the
     * status bit if successful.  The circuit must be powered for setup.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref sensor_atlas_do_concentration) from an
 * [Atlas Scientific EZO dissolved oxygen circuit](@ref sensor_atlas_do).
 *
 * @ingroup sensor_atlas_do
 */
/* clang-format on */
class AtlasScientificDO_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificDO_DOmgL object.
     *
     * @param parentSense The parent AtlasScientificDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasDOmgL".
     */
    explicit AtlasScientificDO_DOmgL(
        AtlasScientificDO* parentSense, const char* uuid = "",
        const char* varCode = ATLAS_DOMGL_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_DOMGL_VAR_NUM,
                   (uint8_t)ATLAS_DOMGL_RESOLUTION, ATLAS_DOMGL_VAR_NAME,
                   ATLAS_DOMGL_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificDO_DOmgL object.
     *
     * @note This must be tied with a parent AtlasScientificDO before it can be
     * used.
     */
    AtlasScientificDO_DOmgL()
        : Variable((const uint8_t)ATLAS_DOMGL_VAR_NUM,
                   (uint8_t)ATLAS_DOMGL_RESOLUTION, ATLAS_DOMGL_VAR_NAME,
                   ATLAS_DOMGL_UNIT_NAME, ATLAS_DOMGL_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificDO_DOmgL object - no action needed.
     */
    ~AtlasScientificDO_DOmgL() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen percent of saturation output](@ref sensor_atlas_do_percent) from an
 * [Atlas Scientific EZO dissolved oxygen circuit](@ref sensor_atlas_do).
 *
 * @ingroup sensor_atlas_do
 */
/* clang-format on */
class AtlasScientificDO_DOpct : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificDO_DOpct object.
     *
     * @param parentSense The parent AtlasScientificDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasDOpct".
     */
    explicit AtlasScientificDO_DOpct(
        AtlasScientificDO* parentSense, const char* uuid = "",
        const char* varCode = ATLAS_DOPCT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_DOPCT_VAR_NUM,
                   (uint8_t)ATLAS_DOPCT_RESOLUTION, ATLAS_DOPCT_VAR_NAME,
                   ATLAS_DOPCT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificDO_DOpct object.
     *
     * @note This must be tied with a parent AtlasScientificDO before it can be
     * used.
     */
    AtlasScientificDO_DOpct()
        : Variable((const uint8_t)ATLAS_DOPCT_VAR_NUM,
                   (uint8_t)ATLAS_DOPCT_RESOLUTION, ATLAS_DOPCT_VAR_NAME,
                   ATLAS_DOPCT_UNIT_NAME, ATLAS_DOPCT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificDO_DOpct object - no action needed.
     */
    ~AtlasScientificDO_DOpct() {}
};
/**@}*/
#endif  // SRC_SENSORS_ATLASSCIENTIFICDO_H_
