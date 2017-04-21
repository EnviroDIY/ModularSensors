/*
 *Decagon5TM.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon 5TM can be found at:
 * http://manuals.decagon.com/Integration%20Guides/5TM%20Integrators%20Guide.pdf
 *
 * For Ea and VWC:
 *     Resolution is 0.0008 m3/m3 (0.08% VWC) from 0 – 50% VWC
 *     Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 *     Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
 *     Range is 0 – 1 m3/m3 (0 – 100% VWC)
 * For temp:
 *     Resolution is 0.1°C
 *     Accuracy is ± 1°C
 *     Range is - 40°C to + 50°C
*/

#ifndef Decagon5TM_h
#define Decagon5TM_h

#include "DecagonSDI12.h"

#define TM_NUM_MEASUREMENTS 2
#define TM_EA_RESOLUTION 4
#define TM_TEMP_RESOLUTION 1
#define TM_VWC_RESOLUTION 2

// The main class for the Decagon 5TM
class Decagon5TM : public virtual DecagonSDI12
{
public:
    Decagon5TM(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    bool update(void) override;

    virtual float getValue(void) = 0;
protected:
    static unsigned long sensorLastUpdated;
    static float sensorValue_ea;
    static float sensorValue_temp;
};


// Defines the "Ea/Matric Potential Sensor"
class Decagon5TM_Ea : public virtual Decagon5TM
{
public:
    Decagon5TM_Ea(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
};


// Defines the "Temperature Sensor"
class Decagon5TM_Temp : public virtual Decagon5TM
{
public:
    Decagon5TM_Temp(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
};


// Defines the "Volumetric Water Content Sensor"
class Decagon5TM_VWC : public virtual Decagon5TM
{
public:
    Decagon5TM_VWC(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
private:
    float ea;
    float sensorValue_VWC;
};

#endif
