/*
 * AtlasScientificORP.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc ORP is the temperature in degrees C.
 *     Accuracy is ± 1 mV
 *     Range is -1019.9mV − 1019.9mV
 *     Resolution is 0.1 mV
 */

// Header Guards
#ifndef AtlasScientificORP_h
#define AtlasScientificORP_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_ORP_I2C_ADDR 0x62  // 98

// Sensor Specific Defines
#define ATLAS_ORP_NUM_VARIABLES 1

#define ATLAS_ORP_WARM_UP_TIME_MS 0
#define ATLAS_ORP_STABILIZATION_TIME_MS 0
#define ATLAS_ORP_MEASUREMENT_TIME_MS 900

#define ATLAS_ORP_RESOLUTION 1
#define ATLAS_ORP_VAR_NUM 0

// The main class for the Atlas Scientific ORP temperature sensor
class AtlasScientificORP : public AtlasParent
{
public:
    AtlasScientificORP(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_ORP_I2C_ADDR,
                       uint8_t measurementsToAverage = 1)
     : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificORP", ATLAS_ORP_NUM_VARIABLES,
                    ATLAS_ORP_WARM_UP_TIME_MS, ATLAS_ORP_STABILIZATION_TIME_MS,
                    ATLAS_ORP_MEASUREMENT_TIME_MS)
    {}
    ~AtlasScientificORP(){}
};

// The class for the oxidation/reduction potential Variable
class AtlasScientificORP_Potential : public Variable
{
public:
    AtlasScientificORP_Potential(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_ORP_VAR_NUM,
                 "reductionPotential", "millivolt",
                 ATLAS_ORP_RESOLUTION,
                 "AtlasORP", UUID, customVarCode)
    {}
    ~AtlasScientificORP_Potential(){}
};

#endif  // Header Guard