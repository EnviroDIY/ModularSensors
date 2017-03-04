/*
 *DecagonES2.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices ES-2 Electrical Conductivity Sensor
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon 5TM can be found at:
 * http://manuals.decagon.com/Integration%20Guides/ES-2%20Integrators%20Guide.pdf
*/

#ifndef DecagonES2_h
#define DecagonES2_h

#include "DecagonSDI12.h"

// The main class for the Decagon ES-2
class DecagonES2 : public virtual DecagonSDI12
{
public:
    DecagonES2(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    bool update(void) override;

    virtual float getValue(void) = 0;
protected:
    static unsigned long sensorLastUpdated;
    static float sensorValue_cond;
    static float sensorValue_temp;
};


// Defines the "Ea/Matric Potential Sensor"
class DecagonES2_Cond : public virtual DecagonES2
{
public:
    DecagonES2_Cond(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
};


// Defines the "Temperature Sensor"
class DecagonES2_Temp : public virtual DecagonES2
{
public:
    DecagonES2_Temp(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
};

#endif
