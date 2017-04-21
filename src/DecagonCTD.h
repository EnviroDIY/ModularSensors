/*
 *DecagonCTD.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices CTD-10
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation for the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 * http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
 *
 * For Depth:
 *  Resolution is 2 mm
 *  Accuracy is ±0.05% of full scale
 *  Range is 0 to 5 m or 0 to 10 m, depending on model
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±1°C
 *  Range is -11°C to +49°C
 * For Specific Conductance:
 *  Resolution is 0.001 mS/cm = 1 µS/cm
 *  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
 *  Range is 0 – 120 mS/cm (bulk)
*/


#ifndef DecagonCTD_h
#define DecagonCTD_h

#include "DecagonSDI12.h"

#define CTD_NUM_MEASUREMENTS 3
#define CTD_COND_RESOLUTION 0
#define CTD_TEMP_RESOLUTION 1
#define CTD_DEPTH_RESOLUTION 0

// The main class for the Decagon CTD
class DecagonCTD : public virtual DecagonSDI12
{
public:
    DecagonCTD(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    bool update(void) override;

    virtual float getValue(void) = 0;
protected:
    static unsigned long sensorLastUpdated;
    static float sensorValue_depth;
    static float sensorValue_temp;
    static float sensorValue_cond;
};


// Defines the "Depth Sensor"
class DecagonCTD_Depth : public virtual DecagonCTD
{
public:
    DecagonCTD_Depth(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
};


// Defines the "Temperature Sensor"
class DecagonCTD_Temp : public virtual DecagonCTD
{
public:
    DecagonCTD_Temp(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
};


// Defines the "Conductivity Sensor"
class DecagonCTD_Cond : public virtual DecagonCTD
{
public:
    DecagonCTD_Cond(char SDI12address, int powerPin, int dataPin, int numReadings = 1);

    float getValue(void) override;
};

#endif
