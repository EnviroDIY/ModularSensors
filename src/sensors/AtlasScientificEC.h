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
 * These are used for any sensor attached to an Atlas EZO conductivity circuit.
 *
 * @copydetails AtlasScientificEC
 */

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
#define ATLAS_COND_I2C_ADDR 0x64  // 100

// Sensor Specific Defines
#define ATLAS_COND_NUM_VARIABLES 4

#define ATLAS_COND_WARM_UP_TIME_MS 745  // 739-740 in tests
#define ATLAS_COND_STABILIZATION_TIME_MS 0
// 555 measurement time in tests, but keep the 600 recommended by manual
#define ATLAS_COND_MEASUREMENT_TIME_MS 600

#define ATLAS_COND_RESOLUTION 3
#define ATLAS_COND_VAR_NUM 0

#define ATLAS_TDS_RESOLUTION 3
#define ATLAS_TDS_VAR_NUM 1

#define ATLAS_SALINITY_RESOLUTION 3
#define ATLAS_SALINITY_VAR_NUM 2

#define ATLAS_SG_RESOLUTION 3
#define ATLAS_SG_VAR_NUM 3

/**
 * @brief The main class for the Atlas Scientific Conductivity sensor
 *
 * The Atlas Scientifc Conductivity sensor outputs raw conductivity, TDS,
 * salinity, and specific gravity
 *   - Accuracy is ± 2%
 *   - Range is 0.07 − 500,000+ μS/cm
 *   - Resolution is 3 decimal places
 */
class AtlasScientificEC : public AtlasParent {
 public:
    explicit AtlasScientificEC(int8_t  powerPin,
                               uint8_t i2cAddressHex = ATLAS_COND_I2C_ADDR,
                               uint8_t measurementsToAverage = 1);
    ~AtlasScientificEC();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C), tells the CO2
     * circuit to report all possible measurement parameters, and sets the
     * status bit if successful.  The circuit must be powered for setup.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;
};

/**
 * @brief The variable class used for Conductivity measured by an Atlas
 * Scientific EC EZO circuit.
 *
 * * @copydetails AtlasScientificEC
 */
class AtlasScientificEC_Cond : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificEC_Cond object.
     *
     * @param parentSense The parent AtlasScientificEC providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasCond
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

/**
 * @brief The variable class used for Total Dissolved Solids measured by an
 * Atlas Scientific EC EZO circuit.
 *
 * * @copydetails AtlasScientificEC
 */
class AtlasScientificEC_TDS : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificEC_TDS object.
     *
     * @param parentSense The parent AtlasScientificEC providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasTDS
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

/**
 * @brief The variable class used for Salinity measured by an Atlas Scientific
 * EC EZO circuit.
 *
 * * @copydetails AtlasScientificEC
 */
class AtlasScientificEC_Salinity : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificEC_Salinity object.
     *
     * @param parentSense The parent AtlasScientificEC providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasSalinity
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

/**
 * @brief The variable class used for Specific Gravity measured by an Atlas
 * Scientific EC EZO circuit.
 *
 * * @copydetails AtlasScientificEC
 */
class AtlasScientificEC_SpecificGravity : public Variable {
 public:
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
