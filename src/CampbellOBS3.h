/*
 *CampbellOBS3.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Campbell Scientific OBS-3+
 *This is dependent on the Adafruit ADS1015 library.
 *
 * Ranges: (depends on sediment size, particle shape, and reflectivity)
 *  Turbidity (low/high): 250/1000 NTU; 500/2000 NTU; 1000/4000 NTU
 *  Mud: 5000 to 10,000 mg L–1
 *  Sand: 50,000 to 100,000 mg L–1
 * Accuracy: (whichever is larger)
 *  Turbidity: 2% of reading or 0.5 NTU
 *  Mud: 2% of reading or 1 mg L–1
 *  Sand: 4% of reading or 10 mg L–1
 * Resolution:
 *  16-bit ADC
 *      Turbidity: 0.004/0.01 NTU; 0.008/0.03 NTU; 0.01/0.06 NTU
 *  12-bit ADC
 *      Turbidity: 0.06/0.2 NTU; 0.1/0.5 NTU; 0.2/1.0 NTU
*/

#ifndef CampbellOBS3_h
#define CampbellOBS3_h

#include <Arduino.h>
#include "SensorBase.h"

#define OBS3_RESOLUTION 3
#define OBS3_HR_RESOLUTION 2

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
    float sensorValue;
    unsigned long sensorLastUpdated;
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
