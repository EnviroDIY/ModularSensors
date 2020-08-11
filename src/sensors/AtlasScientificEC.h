/**
 * @file AtlasScientificEC.h
 * @copyright 2020 Stroud Water Research Center
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
 * @defgroup atlas_cond_group Atlas Scientific conductivity circuit
 * Classes for the [Atlas EZO conductivity circuit.](@ref atlas_cond_page)
 *
 * @copydoc atlas_cond_page
 *
 * @ingroup atlas_group
 */
/* clang-format on */
/* clang-format off */
/**
 * @page atlas_cond_page Atlas Scientific EZO-EC Conductivity Circuit and Probes
 *
 * @tableofcontents
 *
 * The Atlas Scientifc Conductivity sensor outputs raw conductivity, TDS,
 * salinity, and specific gravity
 *   - Accuracy is ± 2%
 *   - Range is 0.07 − 500,000+ μS/cm
 *   - Resolution is 3 decimal places
 *
 * @section atlas_cond_datasheet Sensor Datasheet
 *   - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_EZO_Datasheet.pdf)
 *   - [K0.1 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_0.1_probe.pdf)
 *   - [K1.0 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_1.0_probe.pdf)
 *   - [K10 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_10_probe.pdf)
 *
 *
 * @section atlas_cond_sensor The Atlas EC Sensor
 * @ctor_doc{AtlasScientificEC, int8_t powerPin, uint8_t i2cAddress, uint8_t measurementsToAverage}
 * @subsection atlas_cond_timing Sensor Timing
 *   - warms up in 745ms (739-740 in tests)
 *   - stable at completion of warm up
 *   - measurements take 600ms to complete (only ~555 measurement time in tests,
 * but we wait the full 600ms recommended by manual)
 *
 * ___
 * @section atlas_cond_cond Specific Conductance Output
 * @variabledoc{AtlasScientificEC,Cond}
 *   - Accuracy is ± 2%
 *   - Range is 0.07 − 500,000+ μS/cm
 *   - Resolution is 3 decimal places
 *   - Reported in microsiemens per centimeter
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasCond
 *
 * ___
 * @section atlas_cond_tds Total Dissolved Solids Output
 * @variabledoc{AtlasScientificEC,TDS}
 *   - Accuracy is ± 2%
 *   - Range is 0.07 − 500,000+ μS/cm
 *   - Resolution is 3 decimal places
 *   - Reported in parts per million
 *   - Result stored in sensorValues[1]
 *   - Default variable code is AtlasTDS
 *
 * ___
 * @section atlas_cond_salinity Salinity Output
 * @variabledoc{AtlasScientificEC,Salinity}
 *   - Accuracy is ± 2%
 *   - Range is 0.07 − 500,000+ μS/cm
 *   - Resolution is 3 decimal places
 *   - Reported in practical salinity units
 *   - Result stored in sensorValues[2]
 *   - Default variable code is AtlasSalinity
 *
 * ___
 * @section atlas_cond_sg Specific Gravity Output
 * @variabledoc{AtlasScientificEC,SpecificGravity}
 *   - Accuracy is ± 2%
 *   - Range is 0.07 − 500,000+ μS/cm
 *   - Resolution is 3 decimal places
 *   - Reported value is dimensionless
 *   - Result stored in sensorValues[3]
 *   - Default variable code is AtlasSpecGravity
 *
 * ___
 * @section atlas_cond_examples Example Code
 * The Atlas conductivity sensor is used in the @menulink{atlas_ec} example.
 *
 * @menusnip{atlas_ec}
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


/**
 * @brief Default I2C address is 0x64 (100)
 */
#define ATLAS_COND_I2C_ADDR 0x64

// Sensor Specific Defines
/**
 * @brief Sensor::_numReturnedValues; Atlas EZO conductivity circuit can report
 * 4 values.
 */
#define ATLAS_COND_NUM_VARIABLES 4

/**
 * @brief Sensor::_warmUpTime_ms; Atlas EZO conductivity circuit warms up in
 * 745ms
 *
 * 739-740 in tests
 */
#define ATLAS_COND_WARM_UP_TIME_MS 745
/**
 * @brief Sensor::_stabilizationTime_ms; Atlas EZO conductivity circuit is
 * stable 0ms after warm-up.
 */
#define ATLAS_COND_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; Atlas EZO conductivity circuit takes
 * 600ms to complete a measurement.
 *
 * 555 measurement time in tests, but keep the 600 recommended by manual
 */
#define ATLAS_COND_MEASUREMENT_TIME_MS 600

/// Decimals places in string representation; conductivity should have 3.
#define ATLAS_COND_RESOLUTION 3
/// Variable number; conductivity is stored in sensorValues[0].
#define ATLAS_COND_VAR_NUM 0

/// Decimals places in string representation; TDS should have 3.
#define ATLAS_TDS_RESOLUTION 3
/// Variable number; TDS is stored in sensorValues[1].
#define ATLAS_TDS_VAR_NUM 1

/// Decimals places in string representation; salinity should have 3.
#define ATLAS_SALINITY_RESOLUTION 3
/// Variable number; salinity is stored in sensorValues[2].
#define ATLAS_SALINITY_VAR_NUM 2

/// Decimals places in string representation; specific gravity should have 3.
#define ATLAS_SG_RESOLUTION 3
/// Variable number; specific gravity is stored in sensorValues[3].
#define ATLAS_SG_VAR_NUM 3

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific conductivity circuit and sensor](@ref atlas_cond_page).
 *
 * @ingroup atlas_cond_group
 */
/* clang-format on */
class AtlasScientificEC : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific EC object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas
     * circuit.  Use -1 if the sensor is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @param i2cAddressHex The I2C address of the Atlas circuit; optional with
     * the Atlas-supplied default address of 0x64.
     * @param measurementsToAverage The number of measurements to average;
     * optional with default value of 1.
     *@warning **You must isolate the data lines of all Atlas circuits from the
     *main I2C bus if you wish to turn off their power!**  If you do not isolate
     *them from your main I2C bus and you turn off power to the circuits between
     *measurements the I2C lines will be pulled down to ground causing the I2C
     *bus (and thus your logger) to crash.
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
 * [conductivity output](@ref atlas_cond_cond) from an
 * [Atlas Scientific EC EZO circuit](@ref atlas_cond_page).
 *
 * @ingroup atlas_cond_group
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
     * optional with a default value of AtlasCond
     */
    explicit AtlasScientificEC_Cond(AtlasScientificEC* parentSense,
                                    const char*        uuid    = "",
                                    const char*        varCode = "AtlasCond")
        : Variable(parentSense, (const uint8_t)ATLAS_COND_VAR_NUM,
                   (uint8_t)ATLAS_COND_RESOLUTION, "electricalConductivity",
                   "microsiemenPerCentimeter", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_Cond object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_Cond()
        : Variable((const uint8_t)ATLAS_COND_VAR_NUM,
                   (uint8_t)ATLAS_COND_RESOLUTION, "electricalConductivity",
                   "microsiemenPerCentimeter", "AtlasCond") {}
    /**
     * @brief Destroy the AtlasScientificEC_Cond object - no action needed.
     */
    ~AtlasScientificEC_Cond() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [total dissolved solids output](@ref atlas_cond_tds) from an
 * [Atlas Scientific EC EZO circuit](@ref atlas_cond_page).
 *
 * @ingroup atlas_cond_group
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
     * optional with a default value of AtlasTDS
     */
    explicit AtlasScientificEC_TDS(AtlasScientificEC* parentSense,
                                   const char*        uuid    = "",
                                   const char*        varCode = "AtlasTDS")
        : Variable(parentSense, (const uint8_t)ATLAS_TDS_VAR_NUM,
                   (uint8_t)ATLAS_TDS_RESOLUTION, "solidsTotalDissolved",
                   "partPerMillion", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_TDS object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_TDS()
        : Variable((const uint8_t)ATLAS_TDS_VAR_NUM,
                   (uint8_t)ATLAS_TDS_RESOLUTION, "solidsTotalDissolved",
                   "partPerMillion", "AtlasTDS") {}
    /**
     * @brief Destroy the AtlasScientificEC_TDS object - no action needed.
     */
    ~AtlasScientificEC_TDS() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [salinity output](@ref atlas_cond_salinity) from an
 * [Atlas Scientific EC EZO circuit](@ref atlas_cond_page).
 *
 * @ingroup atlas_cond_group
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
     * optional with a default value of AtlasSalinity
     */
    explicit AtlasScientificEC_Salinity(AtlasScientificEC* parentSense,
                                        const char*        uuid = "",
                                        const char* varCode = "AtlasSalinity")
        : Variable(parentSense, (const uint8_t)ATLAS_SALINITY_VAR_NUM,
                   (uint8_t)ATLAS_SALINITY_RESOLUTION, "salinity",
                   "practicalSalinityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_Salinity object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_Salinity()
        : Variable((const uint8_t)ATLAS_SALINITY_VAR_NUM,
                   (uint8_t)ATLAS_SALINITY_RESOLUTION, "salinity",
                   "practicalSalinityUnit", "AtlasSalinity") {}
    /**
     * @brief Destroy the AtlasScientificEC_Salinity() object - no action
     * needed.
     */
    ~AtlasScientificEC_Salinity() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [specific gravity output](@ref atlas_cond_sg) from an
 * [Atlas Scientific EC EZO circuit](@ref atlas_cond_page).
 *
 * @ingroup atlas_cond_group
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
     * optional with a default value of AtlasSpecGravity
     */
    explicit AtlasScientificEC_SpecificGravity(
        AtlasScientificEC* parentSense, const char* uuid = "",
        const char* varCode = "AtlasSpecGravity")
        : Variable(parentSense, (const uint8_t)ATLAS_SG_VAR_NUM,
                   (uint8_t)ATLAS_SG_RESOLUTION, "specificGravity",
                   "dimensionless", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificEC_SpecificGravity object.
     *
     * @note This must be tied with a parent AtlasScientificEC before it can be
     * used.
     */
    AtlasScientificEC_SpecificGravity()
        : Variable((const uint8_t)ATLAS_SG_VAR_NUM,
                   (uint8_t)ATLAS_SG_RESOLUTION, "specificGravity",
                   "dimensionless", "AtlasSpecGravity") {}
    /**
     * @brief Destroy the AtlasScientificEC_SpecificGravity() object - no action
     * needed.
     */
    ~AtlasScientificEC_SpecificGravity() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICEC_H_
