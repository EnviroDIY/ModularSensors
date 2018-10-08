/*
 *DecagonCTD.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices CTD-10
 *It is dependent on the EnviroDIY SDI-12 library and the SDI12Sensors super class.
 *
 *Documentation for the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 * http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
 *
 * For Specific Conductance:
 *  Resolution is 0.001 mS/cm = 1 µS/cm
 *  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
 *  Range is 0 – 120 mS/cm (bulk)
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±1°C
 *  Range is -11°C to +49°C
 *
 * For Depth:
 *  Resolution is 2 mm
 *  Accuracy is ±0.05% of full scale
 *  Range is 0 to 5 m or 0 to 10 m, depending on model
 *
 * Maximum warm-up time in SDI-12 mode: 500ms, assume stability at warm-up
 * Maximum measurement duration: 500ms
*/

#include "sensors/DecagonCTD.h"

// Constructors with overloads
DecagonCTD::DecagonCTD(char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
  : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                "DecagonCTD", CTD_NUM_VARIABLES,
                CTD_WARM_UP_TIME_MS, CTD_STABILIZATION_TIME_MS, CTD_MEASUREMENT_TIME_MS)
{}
DecagonCTD::DecagonCTD(char *SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
  : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                "DecagonCTD", CTD_NUM_VARIABLES,
                CTD_WARM_UP_TIME_MS, CTD_STABILIZATION_TIME_MS, CTD_MEASUREMENT_TIME_MS)
{}
DecagonCTD::DecagonCTD(int SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage)
  : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                "DecagonCTD", CTD_NUM_VARIABLES,
                CTD_WARM_UP_TIME_MS, CTD_STABILIZATION_TIME_MS, CTD_MEASUREMENT_TIME_MS)
{}
// Destructor
DecagonCTD::~DecagonCTD(){}

/***
// Defines the Conductivity Variable
class DecagonCTD_Cond : public Variable
{
public:
    DecagonCTD_Cond(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, CTD_COND_VAR_NUM,
                "specificConductance", "microsiemenPerCentimeter",
                CTD_COND_RESOLUTION,
                "CTDcond", UUID, customVarCode)
    {}
    ~DecagonCTD_Cond(){}
};


// Defines the Temperature Variable
class DecagonCTD_Temp : public Variable
{
public:
    DecagonCTD_Temp(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, CTD_TEMP_VAR_NUM,
                "temperature", "degreeCelsius",
                CTD_TEMP_RESOLUTION,
                "CTDtemp", UUID, customVarCode)
    {}
    ~DecagonCTD_Temp(){}
};


// Defines the Depth Variable
class DecagonCTD_Depth : public Variable
{
public:
    DecagonCTD_Depth(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, CTD_DEPTH_VAR_NUM,
                "waterDepth", "millimeter",
                CTD_DEPTH_RESOLUTION,
                "CTDdepth", UUID, customVarCode)
    {}
    ~DecagonCTD_Depth(){}
};
***/
