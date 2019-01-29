/*
 *
 *This file was created by Sara Damiano and edited for use of Atlas Scientific Products by Adam Gold
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
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// I2C address
#define ATLAS_COND_I2C_ADDR 0x64

// Sensor Specific Defines
#define ATLAS_COND_NUM_VARIABLES 4
#define ATLAS_COND_WARM_UP_TIME_MS 0
#define ATLAS_COND_STABILIZATION_TIME_MS 0
#define ATLAS_COND_MEASUREMENT_TIME_MS 0

#define ATLAS_COND_RESOLUTION 4
#define ATLAS_COND_VAR_NUM 0

#define ATLAS_TDS_RESOLUTION 4
#define ATLAS_TDS_VAR_NUM 1

#define ATLAS_SALINITY_RESOLUTION 4
#define ATLAS_SALINITY_VAR_NUM 2

#define ATLAS_SG_RESOLUTION 4
#define ATLAS_SG_VAR_NUM 3

// The main class for the MaxBotix Sonar
class AtlasScientificCond : public Sensor
{
public:
    AtlasScientificCond(int8_t powerPin = 22, uint8_t measurementsToAverage = 1);
    ~AtlasScientificCond();
    String getSensorLocation(void) override;
    bool setup(void) override;
    // bool collectData(void);
    bool addSingleMeasurementResult(void) override;
};

// The class for the Cond Variable
class AtlasScientificCond_Cond : public Variable
{
public:
    AtlasScientificCond_Cond(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_COND_VAR_NUM,
                 "specificConductance", "microsiemenPerCentimeter",
                 ATLAS_COND_RESOLUTION,
                 "AtlasCond", UUID, customVarCode)
    {}
    ~AtlasScientificCond_Cond(){}
};

#endif  // Header Guard
