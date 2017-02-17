/*
 *This file is part of the DecagonCTD library for Arduino
 *It is dependent on the EnviroDIY SDI-12 library.
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 *http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
*/

#ifndef _DECAGONCTD_h
#define _DECAGONCTD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Sensor.h"

class DecagonCTD : public Sensor<float>
{
public:
    DecagonCTD(void);
    bool update(char CTDaddress, int numReadings, int dataPin);
    String getValueAsString();
private:
    char _CTDaddress;
    int _numReadings;
    int _dataPin;
};

#endif
