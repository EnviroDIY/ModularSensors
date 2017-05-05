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

#include "SensorBase.h"
#include "VariableBase.h"

// #define MODULES_DBG Serial
#include "ModSensorDebugger.h"


#define OBS3_NUM_VARIABLES 1  // low and high range are treated as completely independent
#define OBS3_TURB_VAR_NUM 0
#define OBS3_RESOLUTION 3
#define OBS3_HR_RESOLUTION 2

// The main class for the Campbell OBS3
class CampbellOBS3 : public Sensor
{
public:
    // The constructor - need the power pin, the data pin, and the calibration info
    CampbellOBS3(int powerPin, int dataPin, float A, float B, float C);

    String getSensorLocation(void) override;

    bool update(void) override;

protected:
    float _A;
    float _B;
    float _C;
};


// Two different vars are ONLY needed because of the different dreamhost column tags
// All that is needed for these are the constructors
// Defines the "Low Turbidity Sensor"
class CampbellOBS3_Turbidity : public Variable
{
public:
    CampbellOBS3_Turbidity(Sensor *parentSense, String customVarCode = "")
      : Variable(parentSense, OBS3_TURB_VAR_NUM,
                 F("turbidity"), F("nephelometricTurbidityUnit"),
                 OBS3_RESOLUTION,
                 F("Turbidity"), customVarCode)
    {}
};

#endif
