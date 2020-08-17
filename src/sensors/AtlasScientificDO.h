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
 * These are used for any sensor attached to an [Atlas EZO DO
 * circuit](https://www.atlas-scientific.com/circuits/ezo-dissolved-oxygen-circuit/).
 */
/* clang-format off */
/**
 * @defgroup atlas_do_group Atlas EZO-DO
 * Classes for the Atlas Scientific EZO-DO dissolved oxygen circuit and probe.
 *
 * @ingroup atlas_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * The Atlas Scientifc DO sensor outputs DO in both mg/L and percent saturation.
 *
 * @section atlas_do_datasheet Sensor Datasheet
 *
 * Documentation on the probe is found here:
 * https://www.atlas-scientific.com/probes/dissolved-oxygen-probe/
 *
 * Documentation on the measurement circuit is found here:
 * https://www.atlas-scientific.com/circuits/ezo-dissolved-oxygen-circuit/
 *
 * @section atlas_do_sensor The Atlas DO Sensor
 * @ctor_doc{AtlasScientificDO, int8_t powerPin, uint8_t i2cAddress, uint8_t measurementsToAverage}
 * @subsection atlas_do_timing Sensor Timing
 *   - warms up in 745ms (737-739 in tests)
 *   - stable at completion of warm up
 *   - measurements take 600ms to complete (only ~555 measurement time in tests,
 * but we wait the full 600ms recommended by manual)
 *
 * ___
 * @section atlas_do_concentration Dissolved Oxygen Concentration Output
 *   - Accuracy is ± 0.05 mg/L
 *   - Range is 0.01 − 100+ mg/L
 *   - Resolution is 0.01 mg/L
 *   - Reported in percent saturation
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasDOmgL
 * @variabledoc{atlas_do_concentration,AtlasScientificDO,DOmgL,AtlasDOmgL}
 *
 * ___
 * @section atlas_do_percent Percent Oxygen Saturation Output
 *   - Accuracy is ± 0.05 mg/L
 *   - Range is 0.1 − 400+ % saturation
 *   - Resolution is 0.1 % saturation
 *   - Reported in percent saturation
 *   - Result stored in sensorValues[1]
 *   - Default variable code is AtlasDOpct
 * @variabledoc{atlas_do_percent,AtlasScientificDO,DOpct,AtlasDOpct}
 *
 * ___
 * @section atlas_do_examples Example Code
 * The Atlas DO sensor is used in the @menulink{atlas_do} example.
 *
 * @menusnip{atlas_do}
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

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Atlas DO sensor can report 2 values.
#define ATLAS_DO_NUM_VARIABLES 2

/**
 * @brief Sensor::_warmUpTime_ms; the Atlas DO sensor warms up in 745ms
 *
 * 737-739 in tests
 */
#define ATLAS_DO_WARM_UP_TIME_MS 745
/**
 * @brief Sensor::_stabilizationTime_ms; the Atlas DO sensor is stable 0ms after
 * warm-up.
 */
#define ATLAS_DO_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas DO sensor takes 600ms to
 * complete a measurement.
 *
 * 555 measurement time in tests, but keep the 600 recommended by manual
 */
#define ATLAS_DO_MEASUREMENT_TIME_MS 600

/**
 * @brief Decimals places in string representation; dissolved oxygen
 * concentration should have 2.
 */
#define ATLAS_DOMGL_RESOLUTION 2
/**
 * @brief Variable number; dissolved oxygen concentration is stored in
 * sensorValues[0].
 */
#define ATLAS_DOMGL_VAR_NUM 0

/**
 * @brief Decimals places in string representation; dissolved oxygen percent
 * should have 1.
 */
#define ATLAS_DOPCT_RESOLUTION 1
/// Variable number; dissolved oxygen percent is stored in sensorValues[1]
#define ATLAS_DOPCT_VAR_NUM 1

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific EZO dissolved oxygen circuit](@ref atlas_do_group).
 *
 * @ingroup atlas_do_group
 */
/* clang-format on */
class AtlasScientificDO : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific DO object
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
 * [dissolved oxygen concentration output](@ref atlas_do_concentration) from an
 * [Atlas Scientific EZO dissolved oxygen circuit](@ref atlas_do_group).
 *
 * @ingroup atlas_do_group
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

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen percent of saturation output](@ref atlas_do_percent) from an
 * [Atlas Scientific EZO dissolved oxygen circuit](@ref atlas_do_group).
 *
 * @ingroup atlas_do_group
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
     * optional with a default value of AtlasDOpct
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
