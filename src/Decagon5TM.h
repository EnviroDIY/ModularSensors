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
 *
 * For Temperature:
 *     Resolution is 0.1°C
 *     Accuracy is ± 1°C
 *     Range is - 40°C to + 50°C
*/

#ifndef Decagon5TM_h
#define Decagon5TM_h

#include "DecagonSDI12.h"
#include "VariableBase.h"

#define TM_NUM_MEASUREMENTS 3

#define TM_EA_RESOLUTION 4
#define TM_EA_VAR_NUM 0

#define TM_TEMP_RESOLUTION 1
#define TM_TEMP_VAR_NUM 1

#define TM_VWC_RESOLUTION 2
#define TM_VWC_VAR_NUM 2

// The main class for the Decagon 5TM
class Decagon5TM : public  DecagonSDI12
{
public:
    // Constructors with overloads
    Decagon5TM(char SDI12address, int powerPin, int dataPin, int numReadings = 1)
     : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings,
                    F("Decagon5TM"), TM_NUM_MEASUREMENTS)
    {}
    Decagon5TM(char *SDI12address, int powerPin, int dataPin, int numReadings = 1)
     : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings,
                    F("Decagon5TM"), TM_NUM_MEASUREMENTS)
    {}
    Decagon5TM(int SDI12address, int powerPin, int dataPin, int numReadings = 1)
     : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings,
                    F("Decagon5TM"), TM_NUM_MEASUREMENTS)
    {}

    bool update(void) override
    {
        DecagonSDI12::update();

        //the TOPP equation used to calculate VWC
        float ea = sensorValues[TM_EA_VAR_NUM];
        float sensorValue_VWC = (4.3e-6*(ea*ea*ea))
                                - (5.5e-4*(ea*ea))
                                + (2.92e-2 * ea)
                                - 5.3e-2 ;
        sensorValues[TM_VWC_VAR_NUM] = sensorValue_VWC;

        // Re-update the variables with the new VWC value
        notifyVariables();

        return true;
    }
};


// Defines the "Ea/Matric Potential Sensor"
class Decagon5TM_Ea : public  Variable
{
public:
    Decagon5TM_Ea(Sensor *parentSense)
     : Variable(parentSense, TM_EA_VAR_NUM,
                F("permittivity"), F("Farad per Meter"),
                TM_EA_RESOLUTION, F("SoilEa"))
    {}
};


// Defines the "Temperature Sensor"
class Decagon5TM_Temp : public  Variable
{
public:
    Decagon5TM_Temp(Sensor *parentSense)
     : Variable(parentSense, TM_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                TM_TEMP_RESOLUTION, F("SoilTemp"))
    {}
};


// Defines the "Volumetric Water Content Sensor"
class Decagon5TM_VWC : public  Variable
{
public:
    Decagon5TM_VWC(Sensor *parentSense)
     : Variable(parentSense, TM_VWC_VAR_NUM,
                F("volumetricWaterContent"), F("percent"),
                TM_VWC_RESOLUTION, F("SoilVWC"))
    {}
};

#endif
