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
 */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICRTD_H_
#define SRC_SENSORS_ATLASSCIENTIFICRTD_H_

// Included Dependencies
#include "sensors/AtlasParent.h"


/**
 * @brief Default I2C address is 0x66 (102)
 */
#define ATLAS_RTD_I2C_ADDR 0x66  // 102

// Sensor Specific Defines
#define ATLAS_RTD_NUM_VARIABLES 1

#define ATLAS_RTD_WARM_UP_TIME_MS 740  // 731-735 in tests
#define ATLAS_RTD_STABILIZATION_TIME_MS 0
// NOTE:  Manual says measurement time is 600, but in SRGD tests, didn't get a
// result until after 643 ms; AG got results as soon as 393ms.
#define ATLAS_RTD_MEASUREMENT_TIME_MS 650

#define ATLAS_RTD_RESOLUTION 3
#define ATLAS_RTD_VAR_NUM 0

/**
 * @brief The main class for the Atlas Scientific RTD temperature sensor - used
 * for any sensor attached to an Atlas EZO RTD circuit.
 *
 * For temperature:
 *   @copydetails AtlasScientificRTD_Temp
 */
class AtlasScientificRTD : public AtlasParent {
 public:
    explicit AtlasScientificRTD(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_RTD_I2C_ADDR,
                                uint8_t measurementsToAverage = 1)
        : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                      "AtlasScientificRTD", ATLAS_RTD_NUM_VARIABLES,
                      ATLAS_RTD_WARM_UP_TIME_MS,
                      ATLAS_RTD_STABILIZATION_TIME_MS,
                      ATLAS_RTD_MEASUREMENT_TIME_MS) {}
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
 *   - Result stored as sensorValues[0]
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
