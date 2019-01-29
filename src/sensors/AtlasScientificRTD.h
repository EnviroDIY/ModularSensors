/*
 *
 *This file was created by Sara Damiano and edited for use of Atlas Scientific Products by Adam Gold
 *
 * The output from the Atlas Scientifc RTD is the range in degrees C.
 *     Accuracy is ± __
 *     Range is __
 *
 * Warm up time to completion of header:  __ ms
 */

// Header Guards
#ifndef AtlasScientificRTD_h
#define AtlasScientificRTD_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// I2C address
#define RTDaddress 102

// Sensor Specific Defines
#define ATLAS_RTD_NUM_VARIABLES 1
#define ATLAS_RTD_WARM_UP_TIME_MS 0
#define ATLAS_RTD_STABILIZATION_TIME_MS 0
#define ATLAS_RTD_MEASUREMENT_TIME_MS 0
#define ATLAS_RTD_RESOLUTION 4
#define ATLAS_RTD_VAR_NUM 0

// The main class for the MaxBotix Sonar
class AtlasScientificRTD : public Sensor
{
public:
    AtlasScientificRTD(int8_t powerPin = 22, uint8_t measurementsToAverage = 1);
    ~AtlasScientificRTD();
    String getSensorLocation(void) override;
    bool setup(void) override;
    // float collectData(void); //Added this back in
    bool addSingleMeasurementResult(void) override;
};

// The class for the Temp Variable
class AtlasScientificRTD_Temp : public Variable
{
public:
    AtlasScientificRTD_Temp(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_RTD_VAR_NUM,
               "Temp", "C",
               ATLAS_RTD_RESOLUTION,
               "TempRange", UUID, customVarCode)
    {}
    ~AtlasScientificRTD_Temp(){}
};

#endif  // Header Guard
