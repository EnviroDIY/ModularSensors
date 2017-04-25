/*
 *DecagonCTD.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices CTD-10
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
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
#define CTD_COND_VAR_NUM 0

#define CTD_TEMP_RESOLUTION 1
#define CTD_TEMP_VAR_NUM 1

#define CTD_DEPTH_RESOLUTION 0
#define CTD_DEPTH_VAR_NUM 2

// The main class for the Decagon CTD
class DecagonCTD : public  DecagonSDI12
{
public:
    // Constructors with overloads
    DecagonCTD(char SDI12address, int powerPin, int dataPin, int numReadings = 1)
     : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings,
                    F("DecagonCTD"), CTD_NUM_MEASUREMENTS)
    {}
    DecagonCTD(char *SDI12address, int powerPin, int dataPin, int numReadings = 1)
     : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings,
                    F("DecagonCTD"), CTD_NUM_MEASUREMENTS)
    {}
    DecagonCTD(int SDI12address, int powerPin, int dataPin, int numReadings = 1)
     : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings,
                    F("DecagonCTD"), CTD_NUM_MEASUREMENTS)
    {}
};


// Defines the "Depth Sensor"
class DecagonCTD_Depth : public  Variable
{
public:
    DecagonCTD_Depth(Sensor *parentSense)
     : Variable(parentSense, CTD_DEPTH_VAR_NUM,
                F("waterDepth"), F("millimeter"),
                CTD_DEPTH_RESOLUTION, F("CTDdepth"))
    {}
};


// Defines the "Temperature Sensor"
class DecagonCTD_Temp : public  Variable
{
public:
    DecagonCTD_Temp(Sensor *parentSense)
     : Variable(parentSense, CTD_TEMP_VAR_NUM,
                F("temperature"), F("degreeCelsius"),
                CTD_TEMP_RESOLUTION, F("CTDtemp"))
    {}
};


// Defines the "Conductivity Sensor"
class DecagonCTD_Cond : public  Variable
{
public:
    DecagonCTD_Cond(Sensor *parentSense)
     : Variable(parentSense, CTD_COND_VAR_NUM,
                F("specificConductance"), F("microsiemenPerCentimeter"),
                CTD_COND_RESOLUTION, F("CTDcond"))
    {}
};

#endif
