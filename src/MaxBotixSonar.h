/*
 *MaxBotixSonar.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the MaxBotix Sonar Library
 *It is dependent on Software Serial.
 *
 * The output from the HRXL-MaxSonar-WRL sonar is the range in mm.
 *
 * Warm up time to completion of header:  160ms
 */

#ifndef MaxBotixSonar_h
#define MaxBotixSonar_h

#include "SensorBase.h"
#include "VariableBase.h"

// #define MODULES_DBG Serial
#include "ModSensorDebugger.h"

#define HRXL_NUM_MEASUREMENTS 1
#define HRXL_WARM_UP 160
#define HRXL_RESOLUTION 0
#define HRXL_VAR_NUM 0

// The main class for the MaxBotix Sonar
class MaxBotixSonar : public Sensor
{
public:
    MaxBotixSonar(int powerPin, Stream* stream, int triggerPin = -1);
    MaxBotixSonar(int powerPin, Stream& stream, int triggerPin = -1);

    String getSensorLocation(void) override;

    SENSOR_STATUS setup(void) override;

    bool update(void) override;

private:
    int _triggerPin;
    Stream* _stream;
};


// The class for the Range Variable
class MaxBotixSonar_Range : public Variable
{
public:
    MaxBotixSonar_Range(Sensor *parentSense, String customVarCode = "") :
      Variable(parentSense, HRXL_VAR_NUM,
               F("distance"), F("millimeter"),
               HRXL_RESOLUTION,
               F("SonarRange"), customVarCode)
    {}
};

#endif
