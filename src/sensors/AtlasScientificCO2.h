/*
 * AtlasScientificCO2.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 * Copyright 2020 Stroud Water Research Center
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc CO2 is the temperature in degrees C.
 *     Accuracy is ± 3% or ± 30 ppm
 *     Range is 0 − 10000 ppm
 *     Resolution is 1 ppm
 */

// Header Guards
#ifndef SRC_SENSORS_ATLASSCIENTIFICCO2_H_
#define SRC_SENSORS_ATLASSCIENTIFICCO2_H_

// Debugging Statement
// #define MS_ATLASSCIENTIFICCO2_DEBUG

#ifdef MS_ATLASSCIENTIFICCO2_DEBUG
#define MS_DEBUGGING_STD "AtlasScientificCO2"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_CO2_I2C_ADDR 0x69  // 105

// Sensor Specific Defines
#define ATLAS_CO2_NUM_VARIABLES 2
// TODO(SRGDamia1):  Test timing with sensor
#define ATLAS_CO2_WARM_UP_TIME_MS 850
// NOTE:  This has a long stabilization time!
#define ATLAS_CO2_STABILIZATION_TIME_MS 10000
#define ATLAS_CO2_MEASUREMENT_TIME_MS 900

#define ATLAS_CO2_RESOLUTION 1
#define ATLAS_CO2_VAR_NUM 0

#define ATLAS_CO2TEMP_RESOLUTION 0
#define ATLAS_CO2TEMP_VAR_NUM 1

// The main class for the Atlas Scientific CO2 temperature sensor
class AtlasScientificCO2 : public AtlasParent {
 public:
    explicit AtlasScientificCO2(int8_t  powerPin,
                                uint8_t i2cAddressHex = ATLAS_CO2_I2C_ADDR,
                                uint8_t measurementsToAverage = 1);
    ~AtlasScientificCO2();

    bool setup(void) override;
};

// The class for the CO2 Concentration Variable
class AtlasScientificCO2_CO2 : public Variable {
 public:
    explicit AtlasScientificCO2_CO2(AtlasScientificCO2* parentSense,
                                    const char*         uuid    = "",
                                    const char*         varCode = "AtlasCO2ppm")
        : Variable(parentSense, (const uint8_t)ATLAS_CO2_VAR_NUM,
                   (uint8_t)ATLAS_CO2_RESOLUTION, "carbonDioxide",
                   "partPerMillion", varCode, uuid) {}
    AtlasScientificCO2_CO2()
        : Variable((const uint8_t)ATLAS_CO2_VAR_NUM,
                   (uint8_t)ATLAS_CO2_RESOLUTION, "carbonDioxide",
                   "partPerMillion", "AtlasCO2ppm") {}
    ~AtlasScientificCO2_CO2() {}
};

// The class for the Temp Variable
class AtlasScientificCO2_Temp : public Variable {
 public:
    explicit AtlasScientificCO2_Temp(AtlasScientificCO2* parentSense,
                                     const char*         uuid = "",
                                     const char* varCode      = "AtlasCO2Temp")
        : Variable(parentSense, (const uint8_t)ATLAS_CO2TEMP_VAR_NUM,
                   (uint8_t)ATLAS_CO2TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    AtlasScientificCO2_Temp()
        : Variable((const uint8_t)ATLAS_CO2TEMP_VAR_NUM,
                   (uint8_t)ATLAS_CO2TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "AtlasCO2Temp") {}
    ~AtlasScientificCO2_Temp() {}
};

#endif  // SRC_SENSORS_ATLASSCIENTIFICCO2_H_
