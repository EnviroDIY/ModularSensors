/*
 *CampbellOBS3.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Campbell Scientific OBS-3+
 *This is dependent on the Adafruit ADS1015 library.
*/

#ifndef CampbellOBS3_h
#define CampbellOBS3_h

#include <Arduino.h>
#include "SensorBase.h"

// The main class for the Campbell OBS3
class CampbellOBS3 : public virtual SensorBase
{
public:
    CampbellOBS3(int powerPin, int dataPin,
                 float A, float B, float C);
    String getSensorLocation(void) override;

    bool update(void) override;

    float getValue(void) override;
protected:
    String sensorLocation;
    float _A;
    float _B;
    float _C;
    static float sensorValue;
    static unsigned long sensorLastUpdated;
};


// Subclasses are ONLY needed because of the different dreamhost column tags
// All that is needed for these are the constructors
// Defines the "Low Turbidity Sensor"
class CampbellOBS3_Turbidity : public virtual CampbellOBS3
{
public:
    CampbellOBS3_Turbidity(int powerPin, int dataPin,
                         float A, float B, float C);
};


// Defines the "High Turbidity Sensor"
class CampbellOBS3_TurbHigh : public virtual CampbellOBS3
{
public:
    CampbellOBS3_TurbHigh(int powerPin, int dataPin,
                          float A, float B, float C);
};

#endif
