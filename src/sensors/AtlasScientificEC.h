/**
 * @file AtlasScientificEC.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificCO2 subclass of the AtlasParent sensor
 * class along with the variable subclasses AtlasScientificEC_Cond,
 * AtlasScientificEC_TDS, AtlasScientificEC_Salinity and
 * AtlasScientificEC_SpecificGravity.
 *
 * These are used for any sensor attached to an
 * [Atlas EZO conductivity
 * circuit](https://www.atlas-scientific.com/conductivity.html).
 */
/* clang-format off */
/**
 * @defgroup sensor_atlas_cond Atlas EZO-EC
 * Classes for the Atlas Scientific EZO-EC conductivity circuit and probes.
 *
 * @ingroup atlas_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * The Atlas Scientifc Conductivity sensor outputs raw conductivity, TDS,
 * salinity, and specific gravity
 * - Accuracy is ± 2%
 * - Range is 0.07 − 500,000+ μS/cm
 * - Resolution is 3 decimal places
 *
 * @note This library *DOES NOT* support using the built-in temperature compensation for conductivity.
 * The returned result is conductivity, not the more typically reported specific.
 * If you need specific conductance, you should calculate it separately, as shown in the example code below.
 *
 * @section sensor_atlas_cond_datasheet Sensor Datasheet
 *   - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_EZO_Datasheet.pdf)
 *   - [K0.1 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_0.1_probe.pdf)
 *   - [K1.0 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_1.0_probe.pdf)
 *   - [K10 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_10_probe.pdf)
 *
 * @section sensor_atlas_cond_ctor Sensor Constructors
 * {{ @ref AtlasScientificEC::AtlasScientificEC(int8_t, uint8_t, uint8_t) }}
 * {{ @ref AtlasScientificEC::AtlasScientificEC(TwoWire*, int8_t, uint8_t, uint8_t) }}
 *
 * ___
 * @section sensor_atlas_cond_examples Example Code
 * The Atlas conductivity sensor is used in the @menulink{atlas_scientific_ec} example.
 *
 * @menusnip{atlas_scientific_ec}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICEC_H_
#define SRC_SENSORS_ATLASSCIENTIFICEC_H_

// Debugging Statement
// #define MS_ATLASSCIENTIFICEC_DEBUG

#ifdef MS_ATLASSCIENTIFICEC_DEBUG
#define MS_DEBUGGING_STD "AtlasScientificEC"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/AtlasParent.h"


// Sensor Specific Defines
/** @ingroup sensor_atlas_cond */
/**@{*/

/// @brief Default I2C address is 0x64 (100)
#define ATLAS_COND_I2C_ADDR 0x64
/// @brief Sensor::_numReturnedValues; Atlas EZO conductivity circuit can report
/// 4 values.
#define ATLAS_COND_NUM_VARIABLES 4
/// @brief Sensor::_incCalcValues; we don't calculate any additional values -
/// though we recommend users include a temperature sensor and calculate
/// specific conductance in their own program.
#define ATLAS_COND_INC_CALC_VARIABLES 0


/**
 * @anchor sensor_atlas_cond_timing
 * @name Sensor Timing
 * The sensor timing for an Atlas EC (conducticity) sensor
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; Atlas EZO conductivity circuit warms up in
 * 745ms
 *
 * 739-740 in tests
 */
#define ATLAS_COND_WARM_UP_TIME_MS 745
/// @brief Sensor::_stabilizationTime_ms; Atlas EZO conductivity circuit is
/// stable 0ms after warm-up. (stable at completion of warm up)
#define ATLAS_COND_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; Atlas EZO conductivity circuit takes
 * 600ms to complete a measurement.
 *
 * only ~555 measurement time in tests, but keep the 600 recommended by manual
 */
#define ATLAS_COND_MEASUREMENT_TIME_MS 600
/**@}*/

/**
 * @anchor sensor_atlas_cond_cond
 * @name Conductivity
 * The conductivity variable from an Atlas EC (conducticity) sensor
 * - Accuracy is ± 2%
 * - Range is 0.07 − 500,000+ μS/cm
 *
 * {{ @ref AtlasScientificEC_Cond::AtlasScientificEC_Cond }}
 */
/**@{*/
/// @brief Decimals places in string representation; conductivity should have 3.
#define ATLAS_COND_RESOLUTION 3
/// @brief Sensor variable number; conductivity is stored in sensorValues[0].
#define ATLAS_COND_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricalConductivity"
#define ATLAS_COND_VAR_NAME "electricalConductivity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microsiemenPerCentimeter" (µS/cm)
#define ATLAS_COND_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "AtlasCond"
#define ATLAS_COND_DEFAULT_CODE "AtlasCond"
/**@}*/

/**
 * @anchor sensor_atlas_cond_tds
 * @name Total Dissolved Solids
 * The TDS variable from an Atlas EC (conducticity) sensor
 * - Accuracy is ± 2%
 * - Range is 0.07 − 500,000+ μS/cm
 *
 * {{ @ref AtlasScientificEC_TDS::AtlasScientificEC_TDS }}
 */
/**@{*/
/// @brief Decimals places in string representation; TDS should have 3.
#define ATLAS_TDS_RESOLUTION 3
/// @brief Sensor variable number; TDS is stored in sensorValues[1].
#define ATLAS_TDS_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "solidsTotalDissolved"
#define ATLAS_TDS_VAR_NAME "solidsTotalDissolved"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "partPerMillion" (ppm)
#define ATLAS_TDS_UNIT_NAME "partPerMillion"
/// @brief Default variable short code; "AtlasTDS"
#define ATLAS_TDS_DEFAULT_CODE "AtlasTDS"
/**@}*/

/**
 * @anchor sensor_atlas_cond_salinity
 * @name Salinity
 * The salinity variable from an Atlas EC (conducticity) sensor
 * - Accuracy is ± 2%
 * - Range is 0.07 − 500,000+ μS/cm
 *
 * {{ @ref AtlasScientificEC_Salinity::AtlasScientificEC_Salinity }}
 */
/**@{*/
/// @brief Decimals places in string representation; salinity should have 3.
#define ATLAS_SALINITY_RESOLUTION 3
/// @brief Sensor variable number; salinity is stored in sensorValues[2].
#define ATLAS_SALINITY_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "salinity"
#define ATLAS_SALINITY_VAR_NAME "salinity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "practicalSalinityUnit"
#define ATLAS_SALINITY_UNIT_NAME "practicalSalinityUnit"
/// @brief Default variable short code; "AtlasSalinity"
#define ATLAS_SALINITY_DEFAULT_CODE "AtlasSalinity"
/**@}*/

/* clang-format off */
/**
 * @anchor sensor_atlas_cond_sg
 * @name Specific Gravity
 * The specific gravity variable from an Atlas EC (conducticity) sensor
 * - Accuracy is ± 2%
 * - Range is 0.07 − 500,000+ μS/cm
 *
 * {{ @ref AtlasScientificEC_SpecificGravity::AtlasScientificEC_SpecificGravity }}
 */
/* clang-format on */
/**@{*/
/// @brief Decimals places in string representation; specific gravity should
/// have 3.
#define ATLAS_SG_RESOLUTION 3
/// @brief Sensor variable number; specific gravity is stored in
/// sensorValues[3].
#define ATLAS_SG_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "specificGravity"
#define ATLAS_SG_VAR_NAME "specificGravity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define ATLAS_SG_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "AtlasSpecGravity"
#define ATLAS_SG_DEFAULT_CODE "AtlasSpecGravity"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific conductivity circuit and sensor](@ref sensor_atlas_cond).
 *
 * @ingroup sensor_atlas_cond
 */
/* clang-format on */
class AtlasScientificEC : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific EC object using a secondary
     * *hardware* I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling powering to the Atlas EC
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x64.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    AtlasScientificEC(TwoWire* theI2C, int8_t powerPin,
                      uint8_t i2cAddressHex         = ATLAS_COND_I2C_ADDR,
                      uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new Atlas Scientific EC object using the primary
     * hardware I2C instance.
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas EC
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     * @param i2cAddressHex The I2C address of the Atlas circuit;
     * optional with the Atlas-supplied default address of 0x64.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit AtlasScientificEC(int8_t  powerPin,
                               uint8_t i2cAddressHex = ATLAS_COND_I2C_ADDR,
                               uint8_t measurementsToAverage = 1);

    /**
     * @brief Destroy the Atlas Scientific EC object
     */
    ~AtlasScientificEC();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C), tells the CO2
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
 * [conductivity output](@ref sensor_atlas_cond_cond) from an
 * [Atlas Scientific EC EZO circuit](@ref sensor_atlas_cond).
 *
 * @ingroup sensor_atlas_cond
 */
/* clang-format on */
class AtlasScientificEC_Cond : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificEC_Cond object.
     *
     * @param parentSense The parent AtlasScientificEC providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasCond".
     */
    explicit AtlasScientificEC_Cond(
        AtlasScientificEC* parentSense, const char* uuid = "",
        const char* varCode = ATLAS_COND_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_COND_VAR_NUM,
                   (uint8_t)ATLAS_COND_RESOLUTION, ATLAS_COND_VAR_NAME,
                   ATLAS_COND_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_Cond object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_Cond()
        : Variable((const uint8_t)ATLAS_COND_VAR_NUM,
                   (uint8_t)ATLAS_COND_RESOLUTION, ATLAS_COND_VAR_NAME,
                   ATLAS_COND_UNIT_NAME, ATLAS_COND_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificEC_Cond object - no action needed.
     */
    ~AtlasScientificEC_Cond() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [total dissolved solids output](@ref sensor_atlas_cond_tds) from an
 * [Atlas Scientific EC EZO circuit](@ref sensor_atlas_cond).
 *
 * @ingroup sensor_atlas_cond
 */
/* clang-format on */
class AtlasScientificEC_TDS : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificEC_TDS object.
     *
     * @param parentSense The parent AtlasScientificEC providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasTDS".
     */
    explicit AtlasScientificEC_TDS(AtlasScientificEC* parentSense,
                                   const char*        uuid = "",
                                   const char* varCode = ATLAS_TDS_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_TDS_VAR_NUM,
                   (uint8_t)ATLAS_TDS_RESOLUTION, ATLAS_TDS_VAR_NAME,
                   ATLAS_TDS_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_TDS object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_TDS()
        : Variable((const uint8_t)ATLAS_TDS_VAR_NUM,
                   (uint8_t)ATLAS_TDS_RESOLUTION, ATLAS_TDS_VAR_NAME,
                   ATLAS_TDS_UNIT_NAME, ATLAS_TDS_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificEC_TDS object - no action needed.
     */
    ~AtlasScientificEC_TDS() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [salinity output](@ref sensor_atlas_cond_salinity) from an
 * [Atlas Scientific EC EZO circuit](@ref sensor_atlas_cond).
 *
 * @ingroup sensor_atlas_cond
 */
/* clang-format on */
class AtlasScientificEC_Salinity : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificEC_Salinity object.
     *
     * @param parentSense The parent AtlasScientificEC providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasSalinity".
     */
    explicit AtlasScientificEC_Salinity(
        AtlasScientificEC* parentSense, const char* uuid = "",
        const char* varCode = ATLAS_SALINITY_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_SALINITY_VAR_NUM,
                   (uint8_t)ATLAS_SALINITY_RESOLUTION, ATLAS_SALINITY_VAR_NAME,
                   ATLAS_SALINITY_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_Salinity object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_Salinity()
        : Variable((const uint8_t)ATLAS_SALINITY_VAR_NUM,
                   (uint8_t)ATLAS_SALINITY_RESOLUTION, ATLAS_SALINITY_VAR_NAME,
                   ATLAS_SALINITY_UNIT_NAME, ATLAS_SALINITY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificEC_Salinity() object - no action
     * needed.
     */
    ~AtlasScientificEC_Salinity() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [specific gravity output](@ref sensor_atlas_cond_sg) from an
 * [Atlas Scientific EC EZO circuit](@ref sensor_atlas_cond).
 *
 * @ingroup sensor_atlas_cond
 */
/* clang-format on */
class AtlasScientificEC_SpecificGravity : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificEC_SpecificGravity object.
     *
     * @param parentSense The parent AtlasScientificEC providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtlasSpecGravity".
     */
    explicit AtlasScientificEC_SpecificGravity(
        AtlasScientificEC* parentSense, const char* uuid = "",
        const char* varCode = ATLAS_SG_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATLAS_SG_VAR_NUM,
                   (uint8_t)ATLAS_SG_RESOLUTION, ATLAS_SG_VAR_NAME,
                   ATLAS_SG_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_SpecificGravity object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_SpecificGravity()
        : Variable((const uint8_t)ATLAS_SG_VAR_NUM,
                   (uint8_t)ATLAS_SG_RESOLUTION, ATLAS_SG_VAR_NAME,
                   ATLAS_SG_UNIT_NAME, ATLAS_SG_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AtlasScientificEC_SpecificGravity() object - no action
     * needed.
     */
    ~AtlasScientificEC_SpecificGravity() {}
};
/**@}*/
#endif  // SRC_SENSORS_ATLASSCIENTIFICEC_H_
