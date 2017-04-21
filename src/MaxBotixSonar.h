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
 */

#ifndef MaxBotixSonar_h
#define MaxBotixSonar_h

#include "SensorBase.h"

#define HRXL_RESOLUTION 0

// The main class for the MaxBotix Sonar.  No sub-classes are needed
class MaxBotixSonar_Range : public virtual SensorBase
{
public:
    MaxBotixSonar_Range(int powerPin, int dataPin, int triggerPin = -1);

    SENSOR_STATUS setup(void) override;

    bool update(void) override;

    float getValue(void) override;
protected:
    float sensorValue_depth;
    unsigned long sensorLastUpdated;
private:
    int _triggerPin;
    int result;
    bool stringComplete;
    int rangeAttempts;
};

#endif
