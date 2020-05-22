/**
 * @file AtlasScientificDO.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificCO2 subclass of the AtlasParent sensor
 * class along with the variable subclasses AtlasScientificDO_DOmgL and
 * AtlasScientificDO_DOpct.
 *
 * These are used for any sensor attached to an Atlas EZO DO circuit.
 *
 * This depends on the Arduino core Wire library.
 *
 * The Atlas Scientifc DO sensor outputs DO in both mg/L and percent saturation
 *     Accuracy is ± 0.05 mg/L
 *     Range is 0.01 − 100+ mg/L (0.1 − 400+ % saturation)
 *     Resolution is 0.01 mg/L or 0.1 % saturation
 */

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
#define ATLAS_DO_I2C_ADDR 0x61  // 97

// Sensor Specific Defines
#define ATLAS_DO_NUM_VARIABLES 2

#define ATLAS_DO_WARM_UP_TIME_MS 745  // 737-739 in tests
#define ATLAS_DO_STABILIZATION_TIME_MS 0
// 555 measurement time in tests, but keep the 600 recommended by manual
#define ATLAS_DO_MEASUREMENT_TIME_MS 600

#define ATLAS_DOMGL_RESOLUTION 2
#define ATLAS_DOMGL_VAR_NUM 0

#define ATLAS_DOPCT_RESOLUTION 1
#define ATLAS_DOPCT_VAR_NUM 1

/**
 * @brief The main class for the Atlas Scientific DO sensor
 */
AtlasParent {
 public:
    explicit AtlasScientificDO(int8_t  powerPin,
                               uint8_t i2cAddressHex = ATLAS_DO_I2C_ADDR,
                               uint8_t measurementsToAverage = 1);
    ~AtlasScientificDO();

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
 * @brief The variable class used for DO Concentration measured by a xxx.
 *
 * The value is in units of xxx and has resolution of xxx.  This is the xxx
 * value (array positon x) returned by the xxx.
 */
class AtlasScientificDO_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificDO_DOmgL object.
     *
     * @param parentSense The parent AtlasScientificDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
    explicit AtlasScientificDO_DOmgL(AtlasScientificDO* parentSense,
                                     const char*        uuid    = "",
                                     const char*        varCode = "AtlasDOmgL")
        : Variable(parentSense, (const uint8_t)ATLAS_DOMGL_VAR_NUM,
                   (uint8_t)ATLAS_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificDO_DOmgL object.
     *
     * @note This must be tied with a parent AtlasScientificDO before it can be
     * used.
     */
    AtlasScientificDO_DOmgL()
        : Variable((const uint8_t)ATLAS_DOMGL_VAR_NUM,
                   (uint8_t)ATLAS_DOMGL_RESOLUTION, "oxygenDissolved",
                   "milligramPerLiter", "AtlasDOmgL") {}
    ~AtlasScientificDO_DOmgL() {}
};

// The class for the DO Percent of Saturation Variable
class AtlasScientificDO_DOpct : public Variable {
 public:
    explicit AtlasScientificDO_DOpct(AtlasScientificDO* parentSense,
                                     const char*        uuid    = "",
                                     const char*        varCode = "AtlasDOpct")
        : Variable(parentSense, (const uint8_t)ATLAS_DOPCT_VAR_NUM,
                   (uint8_t)ATLAS_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent", varCode,
                   uuid) {}
    AtlasScientificDO_DOpct()
        : Variable((const uint8_t)ATLAS_DOPCT_VAR_NUM,
                   (uint8_t)ATLAS_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent",
                   "AtlasDOpct") {}
    ~AtlasScientificDO_DOpct() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICDO_H_
