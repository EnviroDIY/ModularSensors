/*
 *
 *This file was created by Sara Damiano and edited for use of Atlas Scientific Products by Adam Gold
 *
 * The output from the Atlas Scientifc DO is the range in degrees C.
 *     Accuracy is ± __
 *     Range is __
 *
 * Warm up time to completion of header:  __ ms
 */

// Header Guards
#ifndef AtlasScientificDO_h
#define AtlasScientificDO_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// I2C address
#define DOaddress 97

// Sensor Specific Defines
#define ATLASDO_NUM_VARIABLES 1
#define ATLASDO_WARM_UP_TIME_MS 0
#define ATLASDO_STABILIZATION_TIME_MS 0
#define ATLASDO_MEASUREMENT_TIME_MS 0
#define ATLASDO_RESOLUTION 4
#define ATLASDO_VAR_NUM 0

// The main class for the MaxBotix Sonar
class AtlasScientificDO : public Sensor
{
public:
    AtlasScientificDO(int8_t powerPin = 22, uint8_t measurementsToAverage = 1);
    ~AtlasScientificDO();
    String getSensorLocation(void) override;
    bool setup(void) override;
    // bool collectData(void);
    bool addSingleMeasurementResult(void) override;
};

// The class for the DO Variable
class AtlasScientificDO_DO : public Variable
{
public:
    AtlasScientificDO_DO(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLASDO_VAR_NUM,
               "DO", "C",
               ATLASDO_RESOLUTION,
               "DORange", UUID, customVarCode)
    {}
    ~AtlasScientificDO_DO(){}
};

#endif  // Header Guard
