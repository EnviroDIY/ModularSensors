/**
 * @file AtlasScientificRTD.h
 * @brief Contains the AtlasScientificRTD subclass of the AtlasParent sensor
 * class along with the variable subclass AtlasScientificRTD_Temp.
 *
 * These are used for any sensor attached to an Atlas EZO RTD circuit.
 *
 * The output from the Atlas Scientifc RTD is the temperature in degrees C.
 *     Accuracy is ± (0.10°C + 0.0017 x °C)
 *     Range is -126.000 °C − 1254 °C
 *     Resolution is 0.001 °C
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICRTD_H_
#define SRC_SENSORS_ATLASSCIENTIFICRTD_H_

// Included Dependencies
#include "sensors/AtlasParent.h"

// I2C address
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

// The main class for the Atlas Scientific RTD temperature sensor
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

// The class for the Temp Variable
class AtlasScientificRTD_Temp : public Variable {
 public:
    explicit AtlasScientificRTD_Temp(AtlasScientificRTD* parentSense,
                                     const char*         uuid    = "",
                                     const char*         varCode = "AtlasTemp")
        : Variable(parentSense, (const uint8_t)ATLAS_RTD_VAR_NUM,
                   (uint8_t)ATLAS_RTD_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    AtlasScientificRTD_Temp()
        : Variable((const uint8_t)ATLAS_RTD_VAR_NUM,
                   (uint8_t)ATLAS_RTD_RESOLUTION, "temperature",
                   "degreeCelsius", "AtlasTemp") {}
    ~AtlasScientificRTD_Temp() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICRTD_H_
