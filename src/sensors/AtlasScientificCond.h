/*
 *
 *This file was created by Sara Damiano and edited for use of Atlas Scientific Products by Adam Gold
 *
 * The output from the Atlas Scientifc Cond is the range in degrees C.
 *     Accuracy is ± __
 *     Range is __
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
#define Condaddress 100

// Sensor Specific Defines
#define ATLASCond_NUM_VARIABLES 1
#define ATLASCond_WARM_UP_TIME_MS 0
#define ATLASCond_STABILIZATION_TIME_MS 0
#define ATLASCond_MEASUREMENT_TIME_MS 0
#define ATLASCond_RESOLUTION 4
#define ATLASCond_VAR_NUM 0

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
      : Variable(parentSense, ATLASCond_VAR_NUM,
               "Cond", "C",
               ATLASCond_RESOLUTION,
               "CondRange", UUID, customVarCode)
    {}
    ~AtlasScientificCond_Cond(){}
};

#endif  // Header Guard
