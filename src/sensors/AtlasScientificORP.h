/**
 * @file AtlasScientificORP.h
 * @copyright 2020 Stroud Water Research Center
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

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICORP_H_
#define SRC_SENSORS_ATLASSCIENTIFICORP_H_

// Included Dependencies
#include "sensors/AtlasParent.h"


/**
 * @brief Default I2C address is 0x62 (98)
 */
#define ATLAS_ORP_I2C_ADDR 0x62  // 98

// Sensor Specific Defines
#define ATLAS_ORP_NUM_VARIABLES 1

#define ATLAS_ORP_WARM_UP_TIME_MS 850  // 846 in SRGD tests
#define ATLAS_ORP_STABILIZATION_TIME_MS 0
// NOTE:  Manual says measurement takes 900 ms, but in SRGD tests, no result was
// available until 1577 ms
#define ATLAS_ORP_MEASUREMENT_TIME_MS 1580

#define ATLAS_ORP_RESOLUTION 1
#define ATLAS_ORP_VAR_NUM 0

/**
 * @brief The main class for the Atlas Scientific ORP (oxidation/reduction
 * potential) sensor - used for any sensor attached to an Atlas EZO ORP circuit.
 *
 * Documentation on the circuit is available here:
 * https://www.atlas-scientific.com/circuits/ezo-orp-circuit/
 *
 * For oxidation/reduction potential:
 *   @AtlasScientificORP_Potential
 */
class AtlasScientificORP : public AtlasParent {
 public:
    explicit AtlasScientificORP(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_ORP_I2C_ADDR,
                                uint8_t measurementsToAverage = 1)
        : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                      "AtlasScientificORP", ATLAS_ORP_NUM_VARIABLES,
                      ATLAS_ORP_WARM_UP_TIME_MS,
                      ATLAS_ORP_STABILIZATION_TIME_MS,
                      ATLAS_ORP_MEASUREMENT_TIME_MS) {}
    ~AtlasScientificORP() {}
};


/**
 * @brief The variable class used for oxidation/reduction potential measured by
 * an Atlas Scientific EZO circuit.
 *
 *   - Accuracy is ± 1 mV
 *   - Range is -1019.9mV − 1019.9mV
 *   - Resolution is 0.1 mV
 *   - Reported as millivolts
 *   - Result stored as sensorValues[0]
 */
class AtlasScientificORP_Potential : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificORP_Potential object.
     *
     * @param parentSense The parent AtlasScientificORP providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasORP
     */
    explicit AtlasScientificORP_Potential(AtlasScientificORP* parentSense,
                                          const char*         uuid = "",
                                          const char* varCode      = "AtlasORP")
        : Variable(parentSense, (const uint8_t)ATLAS_ORP_VAR_NUM,
                   (uint8_t)ATLAS_ORP_RESOLUTION, "reductionPotential",
                   "millivolt", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificORP_Potential object.
     *
     * @note This must be tied with a parent AtlasScientificORP before it can be
     * used.
     */
    AtlasScientificORP_Potential()
        : Variable((const uint8_t)ATLAS_ORP_VAR_NUM,
                   (uint8_t)ATLAS_ORP_RESOLUTION, "reductionPotential",
                   "millivolt", "AtlasORP") {}
    /**
     * @brief Destroy the AtlasScientificORP_Potential() object - no action
     * needed.
     */
    ~AtlasScientificORP_Potential() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICORP_H_
