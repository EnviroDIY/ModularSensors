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
 *
 * @defgroup atlas_pH_group Atlas Scientific pH circuit
 * The Sensor and Variable objects for the Atlas EZO pH circuit
 *
 * @copydetails AtlasScientificpH
 */

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

/// Sensor::_warmUpTime_ms; the Atlas EZO pH circuit warms up in 850ms (846 in
/// SRGD Tests).
#define ATLAS_PH_WARM_UP_TIME_MS 850
/// Sensor::_stabilizationTime_ms; the Atlas EZO pH circuit is stable 0ms
/// after warm-up.
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

/**
 * @brief The main class for the Atlas Scientific pH temperature sensor - used
 * for any sensor attached to an Atlas EZO pH circuit.
 *
 * Timing:
 *     - warms up in 850ms
 *     - stable at completion of warm up
 *     - measurements take 1660ms to complete
 *
 * For pH:
 *   @copydetails AtlasScientificpH_pH
 *
 * @ingroup atlas_group
 * @ingroup atlas_pH_group
 */
class AtlasScientificpH : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific pH object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas
     * circuit.  Use -1 if the sensor is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit.  Defaults to
     * 0x63.
     * @param measurementsToAverage The number of measurements to average
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


/**
 * @brief The variable class used for pH measured by an Atlas Scientific EZO pH
 * circuit.
 *
 *   - Accuracy is ± 0.002
 *   - Range is 0.001 − 14.000
 *   - Resolution is 0.001
 *   - Reported as dimensionless pH units
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlaspH
 *
 * @note Be careful not to mix the similar variable and sensor object names!
 *
 * @ingroup atlas_group
 * @ingroup atlas_pH_group
 */
class AtlasScientificpH_pH : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificpH_pH object.
     *
     * @param parentSense The parent AtlasScientificpH providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlaspH
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
