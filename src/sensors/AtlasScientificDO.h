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
 * @copydetails AtlasScientificDO
 *
 * @defgroup atlas_do_group Atlas Scientific DO circuit
 * Classes for the Atlas EZO DO circuit
 *
 * @copydetails AtlasScientificDO
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
#define ATLAS_DO_I2C_ADDR 0x61

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Atlas DO sensor can report 2 values.
#define ATLAS_DO_NUM_VARIABLES 2

/// Sensor::_warmUpTime_ms; the Atlas DO sensor warms up in 745ms (737-739 in
/// tests).
#define ATLAS_DO_WARM_UP_TIME_MS 745
/// Sensor::_stabilizationTime_ms; the Atlas DO sensor is stable 0ms after
/// warm-up.
#define ATLAS_DO_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas DO sensor takes 600ms to
 * complete a measurement.
 *
 * 555 measurement time in tests, but keep the 600 recommended by manual
 */
#define ATLAS_DO_MEASUREMENT_TIME_MS 600

/// Decimals places in string representation; dissolved oxygen concentration
/// should have 2.
#define ATLAS_DOMGL_RESOLUTION 2
/// Variable number; dissolved oxygen concentration is stored in
/// sensorValues[0].
#define ATLAS_DOMGL_VAR_NUM 0

/// Decimals places in string representation; dissolved oxygen percent should
/// have 1.
#define ATLAS_DOPCT_RESOLUTION 1
/// Variable number; dissolved oxygen percent is stored in sensorValues[1]
#define ATLAS_DOPCT_VAR_NUM 1

/**
 * @brief The main class for the Atlas Scientific DO sensor - used for any
 * sensor attached to an Atlas EZO DO circuit.
 *
 * Documentation on the probe is found here:
 * https://www.atlas-scientific.com/probes/dissolved-oxygen-probe/
 *
 * Documentation on the measurement circuit is found here:
 * https://www.atlas-scientific.com/circuits/ezo-dissolved-oxygen-circuit/
 *
 * Timing:
 *     - warms up in 745ms
 *     - stable at completion of warm up
 *     - measurements take 600ms to complete
 *
 * The Atlas Scientifc DO sensor outputs DO in both mg/L and percent saturation
 * For concentration:
 *   @copydetails AtlasScientificDO_DOmgL
 *
 * For saturation:
 *   @copydetails AtlasScientificDO_DOpct
 *
 * @ingroup atlas_group
 * @ingroup atlas_do_group
 */
class AtlasScientificDO : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific DO object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas
     * circuit.  Use -1 if the sensor is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit.  Defaults to
     * 0x61.
     * @param measurementsToAverage The number of measurements to average
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
 * @brief The variable class used for dissolved oxygen concentration measured by
 * an Atlas Scientific EZO circuit.
 *
 *   - Accuracy is ± 0.05 mg/L
 *   - Range is 0.01 − 100+ mg/L
 *   - Resolution is 0.01 mg/L
 *   - Reported in percent saturation
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasDOmgL
 *
 * @ingroup atlas_group
 * @ingroup atlas_do_group
 */
class AtlasScientificDO_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificDO_DOmgL object.
     *
     * @param parentSense The parent AtlasScientificDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasDOmgL.
     */
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
    /**
     * @brief Destroy the AtlasScientificDO_DOmgL object - no action needed.
     */
    ~AtlasScientificDO_DOmgL() {}
};

/**
 * @brief The variable class used for dissolved oxygen percent of saturation
 * measured by an Atlas Scientific EZO circuit.
 *
 * The Atlas Scientifc DO sensor outputs DO in both mg/L and percent saturation
 *   - Accuracy is ± 0.05 mg/L
 *   - Range is 0.1 − 400+ % saturation
 *   - Resolution is 0.1 % saturation
 *   - Reported in percent saturation
 *   - Result stored in sensorValues[1]
 *   - Default variable code is AtlasDOpct
 *
 * @ingroup atlas_group
 * @ingroup atlas_do_group
 */
class AtlasScientificDO_DOpct : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificDO_DOpct object.
     *
     * @param parentSense The parent AtlasScientificDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasDOpct
     */
    explicit AtlasScientificDO_DOpct(AtlasScientificDO* parentSense,
                                     const char*        uuid    = "",
                                     const char*        varCode = "AtlasDOpct")
        : Variable(parentSense, (const uint8_t)ATLAS_DOPCT_VAR_NUM,
                   (uint8_t)ATLAS_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent", varCode,
                   uuid) {}
    /**
     * @brief Construct a new AtlasScientificDO_DOpct object.
     *
     * @note This must be tied with a parent AtlasScientificDO before it can be
     * used.
     */
    AtlasScientificDO_DOpct()
        : Variable((const uint8_t)ATLAS_DOPCT_VAR_NUM,
                   (uint8_t)ATLAS_DOPCT_RESOLUTION,
                   "oxygenDissolvedPercentOfSaturation", "percent",
                   "AtlasDOpct") {}
    /**
     * @brief Destroy the AtlasScientificDO_DOpct object - no action needed.
     */
    ~AtlasScientificDO_DOpct() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICDO_H_
