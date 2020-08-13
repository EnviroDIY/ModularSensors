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
/* clang-format off */
/**
 * @defgroup atlas_orp_group Atlas Scientific EZO-ORP Oxidation/Reduction Potential Circuit and Probes
 * Classes for the [Atlas EZO ORP circuit](@ref atlas_orp_page).
 *
 * @copydoc atlas_orp_page
 *
 * @ingroup atlas_group
 */
/* clang-format on */
/* clang-format off */
/**
 * @page atlas_orp_page Atlas Scientific EZO-ORP Oxidation/Reduction Potential Circuit and Probes
 *
 * @tableofcontents
 *
 * @section atlas_orp_datasheet Sensor Datasheet
 * Documentation on the circuit is available here:
 * https://www.atlas-scientific.com/circuits/ezo-orp-circuit/
 *
 * @section atlas_orp_sensor The Atlas ORP Sensor
 * @ctor_doc{AtlasScientificORP, int8_t powerPin, uint8_t i2cAddress, uint8_t measurementsToAverage}
 * @subsection atlas_orp_timing Sensor Timing
 *   - warms up in 850ms
 *      - 846 in SRGD tests
 *   - stable at completion of warm up
 *   - measurements take 1580ms to complete
 *      - Manual says measurement takes 900 ms, but in SRGD tests, no result was
 * available until after 1577 ms.
 *
 * ___
 * @section atlas_orp_orp Oxidation/Reduction Potential Output
 * @variabledoc{AtlasScientificORP,Potential}
 *   - Accuracy is ± 1 mV
 *   - Range is -1019.9mV − 1019.9mV
 *   - Resolution is 0.1 mV
 *   - Reported as millivolts (mV)
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasORP
 *
 * ___
 * @section atlas_orp_examples Example Code
 * The Atlas ORP sensor is used in the @menulink{atlas_orp} example.
 *
 * @menusnip{atlas_orp}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICORP_H_
#define SRC_SENSORS_ATLASSCIENTIFICORP_H_

// Included Dependencies
#include "sensors/AtlasParent.h"


/**
 * @brief Default I2C address is 0x62 (98)
 */
#define ATLAS_ORP_I2C_ADDR 0x62

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Atlas EZO ORP circuit can report 1 value.
#define ATLAS_ORP_NUM_VARIABLES 1

/**
 * @brief Sensor::_warmUpTime_ms; the Atlas EZO ORP circuit warms up in 850ms
 *
 * 846 in SRGD tests
 */
#define ATLAS_ORP_WARM_UP_TIME_MS 850
/**
 * @brief Sensor::_stabilizationTime_ms; the Atlas EZO ORP circuit is stable 0ms
 * after warm-up.
 */
#define ATLAS_ORP_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas EZO ORP circuit takes
 * 1580ms to complete a measurement.
 *
 * @note  Manual says measurement takes 900 ms, but in SRGD tests, no result was
 * available until after 1577 ms.
 */
#define ATLAS_ORP_MEASUREMENT_TIME_MS 1580

/// Decimals places in string representation; ORP should have 1.
#define ATLAS_ORP_RESOLUTION 1
/// Variable number; ORP is stored in sensorValues[0].
#define ATLAS_ORP_VAR_NUM 0

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific ORP (oxidation/reduction potential) sensor](@ref atlas_orp_page).
 *
 * @ingroup atlas_orp_group
 */
/* clang-format on */
class AtlasScientificORP : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific ORP object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas ORP
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @param i2cAddressHex The I2C address of the Atlas circuit; optional with
     * the Atlas-supplied default address of 0x62.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     */
    explicit AtlasScientificORP(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_ORP_I2C_ADDR,
                                uint8_t measurementsToAverage = 1)
        : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                      "AtlasScientificORP", ATLAS_ORP_NUM_VARIABLES,
                      ATLAS_ORP_WARM_UP_TIME_MS,
                      ATLAS_ORP_STABILIZATION_TIME_MS,
                      ATLAS_ORP_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Atlas Scientific ORP object
     */
    ~AtlasScientificORP() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [oxidation/reduction potential output](@ref atlas_orp_orp) from an
 * [Atlas Scientific EZO-ORP circuit](@ref atlas_orp_page).
 *
 * @ingroup atlas_orp_group
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
     * optional with a default value of AtlasORP
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
