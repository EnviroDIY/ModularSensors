/*
 *Decagon5TM.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon 5TM can be found at:
 * http://manuals.decagon.com/Integration%20Guides/5TM%20Integrators%20Guide.pdf
*/

#ifndef Decagon5TM_h
#define Decagon5TM_h

#include "DecagonSDI12.h"

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
