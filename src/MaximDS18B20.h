/*
 * MaximDS18B20.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 * This file is for the Maxim DS18B20 Temperature sensor.
 * It is dependent on Dallas Temperature library, which itself is dependent on
 * the OneWire library.
*/

#ifndef MaxBotixSonar_h
#define MaxBotixSonar_h

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "SensorBase.h"

// The main class for the MaxBotix Sonar.  No sub-classes are needed
class MaximDS18B20_Temp : public virtual SensorBase
{
public:
    MaximDS18B20_Temp(int powerPin, int dataPin);

    bool update(void) override;

    float getValue(void) override;
protected:
    float sensorValue_temp;
    unsigned long sensorLastUpdated;
private:
    int result;
    bool stringComplete;
    int rangeAttempts;
};

#endif
