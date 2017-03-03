/*
 *DecagonCTD.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Decagon Devices CTD-10
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 *http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
*/

#include "DecagonCTD.h"


// The constructor - need the SDI-12 address, the power pin, the data pin, and the number of readings
DecagonCTD::DecagonCTD(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin),
   DecagonSDI12(SDI12address, powerPin, dataPin, numReadings)
{}


// The static variables that need to be updated
float DecagonCTD::sensorValue_depth = 0;
float DecagonCTD::sensorValue_temp = 0;
float DecagonCTD::sensorValue_cond = 0;
unsigned long DecagonCTD::sensorLastUpdated = 0;
bool DecagonCTD::update()
{
    DecagonSDI12::update();
    DecagonCTD::sensorValue_depth = DecagonSDI12::sensorValues[0];
    DecagonCTD::sensorValue_temp = DecagonSDI12::sensorValues[1];
    DecagonCTD::sensorValue_cond = DecagonSDI12::sensorValues[2];
    // Make note of the last time updated
    DecagonCTD::sensorLastUpdated = millis();
    return true;
}




DecagonCTD_Depth::DecagonCTD_Depth(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F(""), F("waterDepth"), F("millimeter"), F("CTDdepth")),
   DecagonSDI12(SDI12address, powerPin, dataPin, numReadings),
   DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonCTD_Depth::getValue(void)
{
    checkForUpdate(DecagonCTD::sensorLastUpdated);
    return DecagonCTD::sensorValue_depth;
}




DecagonCTD_Temp::DecagonCTD_Temp(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F(""), F("temperature"), F("degreeCelsius"), F("CTDtemp")),
   DecagonSDI12(SDI12address, powerPin, dataPin, numReadings),
   DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonCTD_Temp::getValue(void)
{
    checkForUpdate(DecagonCTD::sensorLastUpdated);
    return DecagonCTD::sensorValue_temp;
}




DecagonCTD_Cond::DecagonCTD_Cond(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F(""), F("specificConductance"), F("microsiemenPerCentimeter"), F("CTDcond")),
   DecagonSDI12(SDI12address, powerPin, dataPin, numReadings),
   DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonCTD_Cond::getValue(void)
{
    checkForUpdate(DecagonCTD::sensorLastUpdated);
    return DecagonCTD::sensorValue_cond;
}
