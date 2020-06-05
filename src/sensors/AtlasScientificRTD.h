/**
 * @file AtlasScientificRTD.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasScientificRTD subclass of the AtlasParent sensor
 * class along with the variable subclass AtlasScientificRTD_Temp.
 *
 * These are used for any sensor attached to an Atlas EZO RTD circuit.
 *
 * @copydetails AtlasScientificRTD
 *
 * @defgroup atlas_rtd_group Atlas Scientific RTD circuit
 * The Sensor and Variable objects for the Atlas EZO RTD circuit
 *
 * @copydetails AtlasScientificRTD
 */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICRTD_H_
#define SRC_SENSORS_ATLASSCIENTIFICRTD_H_

// Included Dependencies
#include "sensors/AtlasParent.h"


/**
 * @brief Default I2C address is 0x66 (102)
 */
#define ATLAS_RTD_I2C_ADDR 0x66

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the Atlas EZO temperature circuit can report 1
/// value.
#define ATLAS_RTD_NUM_VARIABLES 1

/// Sensor::_warmUpTime_ms; the Atlas EZO temperature circuit warms up in 740ms
/// (731-735 in tests).
#define ATLAS_RTD_WARM_UP_TIME_MS 740
/// Sensor::_stabilizationTime_ms; the Atlas EZO temperature circuit is stable
/// 0ms after warm-up.
#define ATLAS_RTD_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the Atlas EZO temperature circuit takes
 * 650ms to complete a measurement.
 *
 * @note Manual says measurement takes 600 ms, but in SRGD tests, didn't get a
 * result until after 643 ms; AG got results as soon as 393ms.
 */
#define ATLAS_RTD_MEASUREMENT_TIME_MS 650

/// Decimals places in string representation; temperature should have 3.
#define ATLAS_RTD_RESOLUTION 3
/// Variable number; RTD is stored in sensorValues[0].
#define ATLAS_RTD_VAR_NUM 0

/**
 * @brief The main class for the Atlas Scientific RTD temperature sensor - used
 * for any sensor attached to an Atlas EZO RTD circuit.
 *
 * Timing:
 *     - warms up in 740ms
 *     - stable at completion of warm up
 *     - measurements take 650ms to complete
 *
 * For temperature:
 *   @copydetails AtlasScientificRTD_Temp
 *
 * @ingroup atlas_group
 * @ingroup atlas_rtd_group
 */
class AtlasScientificRTD : public AtlasParent {
 public:
    /**
     * @brief Construct a new Atlas Scientific RTD object
     *
     * @param powerPin The pin on the mcu controlling powering to the Atlas
     * circuit.  Use -1 if the sensor is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit.  Defaults to
     * 0x66.
     * @param measurementsToAverage The number of measurements to average
     */
    explicit AtlasScientificRTD(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_RTD_I2C_ADDR,
                                uint8_t measurementsToAverage = 1)
        : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                      "AtlasScientificRTD", ATLAS_RTD_NUM_VARIABLES,
                      ATLAS_RTD_WARM_UP_TIME_MS,
                      ATLAS_RTD_STABILIZATION_TIME_MS,
                      ATLAS_RTD_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the Atlas Scientific RTD object
     */
    ~AtlasScientificRTD() {}
};

/**
 * @brief The variable class used for temperature measured by an Atlas
 * Scientific RTD temperature sensor.
 *
 *   - Accuracy is ± (0.10°C + 0.0017 x °C)
 *   - Range is -126.000 °C − 1254 °C
 *   - Resolution is 0.001 °C
 *   - Reported as degrees Celsius
 *   - Result stored in sensorValues[0]
 *   - Default variable code is AtlasTemp
 *
 * @ingroup atlas_group
 * @ingroup atlas_rtd_group
 */
class AtlasScientificRTD_Temp : public Variable {
 public:
    /**
     * @brief Construct a new AtlasScientificRTD_Temp object.
     *
     * @param parentSense The parent AtlasScientificRTD providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is AtlasTemp
     */
    explicit AtlasScientificRTD_Temp(AtlasScientificRTD* parentSense,
                                     const char*         uuid    = "",
                                     const char*         varCode = "AtlasTemp")
        : Variable(parentSense, (const uint8_t)ATLAS_RTD_VAR_NUM,
                   (uint8_t)ATLAS_RTD_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new AtlasScientificRTD_Temp object.
     *
     * @note This must be tied with a parent AtlasScientificRTD before it can be
     * used.
     */
    AtlasScientificRTD_Temp()
        : Variable((const uint8_t)ATLAS_RTD_VAR_NUM,
                   (uint8_t)ATLAS_RTD_RESOLUTION, "temperature",
                   "degreeCelsius", "AtlasTemp") {}
    /**
     * @brief Destroy the AtlasScientificRTD_Temp object - no action needed.
     */
    ~AtlasScientificRTD_Temp() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICRTD_H_
