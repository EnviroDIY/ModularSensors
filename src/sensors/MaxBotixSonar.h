/*
 *MaxBotixSonar.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the MaxBotix Sonar Library
 *
 * The output from the HRXL-MaxSonar-WRL sonar is the range in mm.
 *     Accuracy is ± 1%
 *     Range is 300-5000mm or 500 to 9999mm, depending on model
 *
 * Warm up time to completion of header:  160ms
 */

// Header Guards
#ifndef MaxBotixSonar_h
#define MaxBotixSonar_h

// Debugging Statement
// #define MS_MAXBOTIXSONAR_DEBUG

#ifdef MS_MAXBOTIXSONAR_DEBUG
    #define MS_DEBUGGING_STD "MaxBotixSonar"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define HRXL_NUM_VARIABLES 1
#define HRXL_WARM_UP_TIME_MS 160
#define HRXL_STABILIZATION_TIME_MS 0
#define HRXL_MEASUREMENT_TIME_MS 166
#define HRXL_RESOLUTION 0
#define HRXL_VAR_NUM 0

// The main class for the MaxBotix Sonar
class MaxBotixSonar : public Sensor
{
public:
    MaxBotixSonar(Stream *stream, int8_t powerPin, int8_t triggerPin = -1, uint8_t measurementsToAverage = 1);
    MaxBotixSonar(Stream &stream, int8_t powerPin, int8_t triggerPin = -1, uint8_t measurementsToAverage = 1);
    ~MaxBotixSonar();

    String getSensorLocation(void) override;

    bool setup(void) override;
    bool wake(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    int8_t _triggerPin;
    Stream *_stream;
};


// The class for the Range Variable
class MaxBotixSonar_Range : public Variable
{
public:
    MaxBotixSonar_Range(Sensor *parentSense, const char *uuid = "",
                        const char *varCode = "SonarRange")
        : Variable(parentSense,
                   (const uint8_t)HRXL_VAR_NUM,
                   (uint8_t)HRXL_RESOLUTION,
                   "distance", "millimeter",
                   varCode, uuid)
    {}
    MaxBotixSonar_Range()
        : Variable((const uint8_t)HRXL_VAR_NUM,
                   (uint8_t)HRXL_RESOLUTION,
                   "distance", "millimeter", "SonarRange")
    {}
    ~MaxBotixSonar_Range() {}
};

#endif  // Header Guard
