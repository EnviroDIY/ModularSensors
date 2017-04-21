/*
 * MaximDS18.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 * This file is for the Maxim DS18B20 Temperature sensor.
 * It is dependent on Dallas Temperature library, which itself is dependent on
 * the OneWire library.
 *
 * The resolution of the temperature sensor is user-configurable to
 * 9, 10, 11, or 12 bits, corresponding to increments of
 * 0.5°C, 0.25°C, 0.125°C, and 0.0625°C, respectively.
 * The default resolution at power-up is 12-bit.
*/

#ifndef MaximDS18_h
#define MaximDS18_h

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "SensorBase.h"

#define DS18_NUM_MEASUREMENTS 1
#define DS18_TEMP_RESOLUTION 4

// The main class for the MaxBotix Sonar.  No sub-classes are needed
class MaximDS18_Temp : public virtual SensorBase
{
public:
    MaximDS18_Temp(DeviceAddress OneWireAddress, int powerPin, int dataPin);
    MaximDS18_Temp(int powerPin, int dataPin);

    bool update(void) override;

    float getValue(void) override;
protected:
    float sensorValue_temp;
    unsigned long sensorLastUpdated;
private:
    uint8_t* _OneWireAddress;
    bool _addressKnown;
    int result;
    bool stringComplete;
    int rangeAttempts;
};

#endif
