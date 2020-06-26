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
 *
 * @defgroup atlas_orp_group Atlas Scientific ORP circuit
 * Classes for the Atlas EZO ORP circuit
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
#define ATLAS_ORP_I2C_ADDR 0x62

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Atlas EZO ORP circuit can report 1 value.
#define ATLAS_ORP_NUM_VARIABLES 1

/// Sensor::_warmUpTime_ms; the Atlas EZO ORP circuit warms up in 850ms (846 in
/// SRGD tests).
#define ATLAS_ORP_WARM_UP_TIME_MS 850
/// Sensor::_stabilizationTime_ms; the Atlas EZO ORP circuit is stable 0ms
/// after warm-up.
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

/**
 * @brief The main class for the Atlas Scientific ORP (oxidation/reduction
 * potential) sensor - used for any sensor attached to an Atlas EZO ORP circuit.
 *
 * Documentation on the circuit is available here:
 * https://www.atlas-scientific.com/circuits/ezo-orp-circuit/
 *
 * Timing:
 *     - warms up in 850ms
 *     - stable at completion of warm up
 *     - measurements take 1580ms to complete
 *
 * For oxidation/reduction potential:
 *   @copydetails AtlasScientificORP_Potential
 *
 * @ingroup atlas_group
 * @ingroup atlas_orp_group
 */
class AtlasScientificORP : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific ORP object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas
     * circuit.  Use -1 if the sensor is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit.  Defaults to
     * 0x62.
     * @param measurementsToAverage The number of measurements to average
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


/**
 * @brief The variable class used for oxidation/reduction potential measured by
 * an Atlas Scientific EZO circuit.
 *
 *   - Accuracy is ± 1 mV
 *   - Range is -1019.9mV − 1019.9mV
 *   - Resolution is 0.1 mV
 *   - Reported as millivolts
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasORP
 *
 * @ingroup atlas_group
 * @ingroup atlas_orp_group
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
