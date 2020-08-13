/**
 * @file AtlasScientificpH.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificRTD subclass of the AtlasParent sensor
 * class along with the variable subclass AtlasScientificpH_pH.
 *
 * These are used for any sensor attached to an Atlas EZO pH circuit.
 *
 * @copydetails AtlasScientificpH
 */
/* clang-format off */
/**
 * @defgroup atlas_pH_group Atlas Scientific EZO-pH Circuit and Probe
 * Classes for the [Atlas EZO pH circuit](@ref atlas_ph_page).
 *
 * @ingroup atlas_group
 *
 * @copydoc atlas_ph_page
 */
/* clang-format on */
/* clang-format off */
/**
 * @page atlas_ph_page Atlas Scientific EZO-pH Circuit and Probe
 *
 * @tableofcontents
 *
 * @section atlas_ph_datasheet Sensor Datasheet
 * Documentation on the probe and measurement circuit are found here:
 * https://www.atlas-scientific.com/ph.html
 *
 * @section atlas_ph_sensor The Atlas pH Sensor
 * @ctor_doc{AtlasScientificpH, int8_t powerPin, uint8_t i2cAddress, uint8_t measurementsToAverage}
 * @subsection atlas_ph_timing Sensor Timing
 *   - warms up in 850ms
 *      - 846 in SRGD Tests
 *   - stable at completion of warm up
 *   - measurements take 1660ms to complete
 *      - Manual says measurement takes 900 ms, but in SRGD tests, no result was
 * available until after 1656 ms.
 *
 * ___
 * @section atlas_ph_ph pH Output
 * @variabledoc{AtlasScientificpH,pH}
 *   - Accuracy is ± 0.002
 *   - Range is 0.001 − 14.000
 *   - Resolution is 0.001
 *   - Reported as dimensionless pH units
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlaspH
 *
 * @note Be careful not to mix the similar variable and sensor object names!
 *
 * ___
 * @section atlas_ph_examples Example Code
 * The Atlas pH sensor is used in the @menulink{atlas_ph} example.
 *
 * @menusnip{atlas_ph}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICPH_H_
#define SRC_SENSORS_ATLASSCIENTIFICPH_H_

// Included Dependencies
#include "sensors/AtlasParent.h"


/**
 * @brief Default I2C address is 0x63 (99)
 */
#define ATLAS_PH_I2C_ADDR 0x63

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Atlas EZO pH circuit can report 1 value.
#define ATLAS_PH_NUM_VARIABLES 1

/**
 * @brief Sensor::_warmUpTime_ms; the Atlas EZO pH circuit warms up in 850ms.
 *
 * 846 in SRGD Tests
 */
#define ATLAS_PH_WARM_UP_TIME_MS 850
/**
 * @brief Sensor::_stabilizationTime_ms; the Atlas EZO pH circuit is stable 0ms
 * after warm-up.
 */
#define ATLAS_PH_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas EZO pH circuit takes
 * 1660ms to complete a measurement.
 *
 * @note Manual says measurement takes 900 ms, but in SRGD tests, no
 * result was available until after 1656 ms.
 */
#define ATLAS_PH_MEASUREMENT_TIME_MS 1660

/// Decimals places in string representation; pH should have 3.
#define ATLAS_PH_RESOLUTION 3
/// Variable number; pH is stored in sensorValues[0].
#define ATLAS_PH_VAR_NUM 0

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Atlas Scientific pH sensor](@ref atlas_ph_page).
 *
 * @note Be careful not to confuse the similar variable and sensor object names!
 *
 * @ingroup atlas_pH_group
 */
/* clang-format on */
class AtlasScientificpH : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific pH object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas pH
     * circuit.  Use -1 if it is continuously powered.
     * - Requires a 3.3V and 5V power supply
     * @param i2cAddressHex The I2C address of the Atlas circuit; optional with
     * the Atlas-supplied default address of 0x63.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @warning **You must isolate the data lines of all Atlas circuits from the
     * main I2C bus if you wish to turn off their power!**  If you do not
     * isolate them from your main I2C bus and you turn off power to the
     * circuits between measurements the I2C lines will be pulled down to ground
     * causing the I2C bus (and thus your logger) to crash.
     */
    explicit AtlasScientificpH(int8_t  powerPin,
                               uint8_t i2cAddressHex = ATLAS_PH_I2C_ADDR,
                               uint8_t measurementsToAverage = 1)
        : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                      "AtlasScientificpH", ATLAS_PH_NUM_VARIABLES,
                      ATLAS_PH_WARM_UP_TIME_MS, ATLAS_PH_STABILIZATION_TIME_MS,
                      ATLAS_PH_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Atlas Scientific pH object
     */
    ~AtlasScientificpH() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pH output](@ref atlas_ph_ph) from an
 * [Atlas Scientific EZO pH circuit](@ref atlas_ph_page).
 *
 * @note Be careful not to mix the similar variable and sensor object names!
 *
 * @ingroup atlas_pH_group
 */
/* clang-format on */
class AtlasScientificpH_pH : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificpH_pH object.
     *
     * @param parentSense The parent AtlasScientificpH providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of AtlaspH
     */
    explicit AtlasScientificpH_pH(AtlasScientificpH* parentSense,
                                  const char*        uuid    = "",
                                  const char*        varCode = "AtlaspH")
        : Variable(parentSense, (const uint8_t)ATLAS_PH_VAR_NUM,
                   (uint8_t)ATLAS_PH_RESOLUTION, "pH", "pH", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificpH_pH object.
     *
     * @note This must be tied with a parent AtlasScientificpH before it can be
     * used.
     */
    AtlasScientificpH_pH()
        : Variable((const uint8_t)ATLAS_PH_VAR_NUM,
                   (uint8_t)ATLAS_PH_RESOLUTION, "pH", "pH", "AtlaspH") {}
    /**
     * @brief Destroy the AtlasScientificpH_pH object - no action needed.
     */
    ~AtlasScientificpH_pH() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICPH_H_
