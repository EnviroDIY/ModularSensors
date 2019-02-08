/*
 *AtlasScientificCond.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial developement for Atlas Sensors was done by Adam Gold
 *Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc Cond is the range in degrees C.
 *     Accuracy is ± 2%
 *     Range is 0.07 − 500,000+ μS/cm
 *
 * Warm up time to completion of header:  __ ms
 */

// Header Guards
#ifndef AtlasScientificCond_h
#define AtlasScientificCond_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "VariableBase.h"
#include "sensors/AtlasParent.h"

// I2C address
#define ATLAS_COND_I2C_ADDR 0x64  // 100

// Sensor Specific Defines
#define ATLAS_COND_NUM_VARIABLES 4
#define ATLAS_COND_WARM_UP_TIME_MS 0
#define ATLAS_COND_STABILIZATION_TIME_MS 0
#define ATLAS_COND_MEASUREMENT_TIME_MS 0

#define ATLAS_COND_RESOLUTION 3
#define ATLAS_COND_VAR_NUM 0

#define ATLAS_TDS_RESOLUTION 3
#define ATLAS_TDS_VAR_NUM 1

#define ATLAS_SALINITY_RESOLUTION 3
#define ATLAS_SALINITY_VAR_NUM 2

#define ATLAS_SG_RESOLUTION 3
#define ATLAS_SG_VAR_NUM 3

// The main class for the Atlas Scientific Conductivity sensor
class AtlasScientificCond : public AtlasParent
{
public:
    AtlasScientificCond(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_COND_I2C_ADDR, uint8_t measurementsToAverage = 1)
     : SDI12Sensors(powerPin, i2cAddressHex, measurementsToAverage,
                    "AtlasScientificCond", ATLAS_COND_NUM_VARIABLES,
                    ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS, ATLAS_COND_MEASUREMENT_TIME_MS)
    ~AtlasScientificCond();
};

// The class for the Conductivity Variable
class AtlasScientificCond_Cond : public Variable
{
public:
    AtlasScientificCond_Cond(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_COND_VAR_NUM,
                 "electricalConductivity", "microsiemenPerCentimeter",
                 ATLAS_COND_RESOLUTION,
                 "AtlasCond", UUID, customVarCode)
    {}
    ~AtlasScientificCond_Cond(){}
};

// The class for the Total Dissolved Solids Variable
class AtlasScientificCond_TDS : public Variable
{
public:
    AtlasScientificCond_TDS(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_TDS_VAR_NUM,
                 "electricalConductivity", "microsiemenPerCentimeter",
                 ATLAS_TDS_RESOLUTION,
                 "AtlasCond", UUID, customVarCode)
    {}
    ~AtlasScientificCond_TDS(){}
};

// The class for the Salinity Variable
class AtlasScientificCond_Salinity : public Variable
{
public:
    AtlasScientificCond_Salinity(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_SALINITY_VAR_NUM,
                 "salinity", "practicalSalinityUnit",
                 ATLAS_SALINITY_RESOLUTION,
                 "AtlasSalinity", UUID, customVarCode)
    {}
    ~AtlasScientificCond_Salinity(){}
};

// The class for the Specific Gravity Variable
class AtlasScientificCond_SpecificGravity : public Variable
{
public:
    AtlasScientificCond_SpecificGravity(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_SG_VAR_NUM,
                 "specificGravity", "dimensionless",
                 ATLAS_SG_RESOLUTION,
                 "AtlasSpecGravity", UUID, customVarCode)
    {}
    ~AtlasScientificCond_SpecificGravity(){}
};

#endif  // Header Guard
