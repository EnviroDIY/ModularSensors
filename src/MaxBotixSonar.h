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

#ifndef MaxBotixSonar_h
#define MaxBotixSonar_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

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
    MaxBotixSonar(Stream* stream, int8_t powerPin, int8_t triggerPin = -1, uint8_t measurementsToAverage = 1);
    MaxBotixSonar(Stream& stream, int8_t powerPin, int8_t triggerPin = -1, uint8_t measurementsToAverage = 1);

    String getSensorLocation(void) override;

    SENSOR_STATUS setup(void) override;
    bool wake(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    int _triggerPin;
    Stream* _stream;
};


// The class for the Range Variable
class MaxBotixSonar_Range : public Variable
{
public:
    MaxBotixSonar_Range(Sensor *parentSense,
                        String UUID = "", String customVarCode = "")
      : Variable(parentSense, HRXL_VAR_NUM,
               F("distance"), F("millimeter"),
               HRXL_RESOLUTION,
               F("SonarRange"), UUID, customVarCode)
    {}
};

#endif
