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
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings)
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
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings), DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

String DecagonCTD_Depth::getVarName(void)
{
    varName = F("waterDepth");
    return varName;
}

String DecagonCTD_Depth::getVarUnit(void)
{
    String unit = F("millimeter");
    return unit;
}

float DecagonCTD_Depth::getValue(void)
{
    if ((millis() > 30000 and millis() > DecagonCTD::sensorLastUpdated + 30000) or DecagonCTD::sensorLastUpdated == 0)
    {
        Serial.println(F("Value out of date, updating"));  // For debugging
        DecagonCTD::update();
    }
    return DecagonCTD::sensorValue_depth;
}

String DecagonCTD_Depth::getDreamHost(void)
{
String column = F("CTDdepth");
return column;
}




DecagonCTD_Temp::DecagonCTD_Temp(char SDI12address, int powerPin, int dataPin, int numReadings)
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings), DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

String DecagonCTD_Temp::getVarName(void)
{
    varName = F("temperature");
    return varName;
}

String DecagonCTD_Temp::getVarUnit(void)
{
    String unit = F("degreeCelsius");
    return unit;
}

float DecagonCTD_Temp::getValue(void)
{
    if ((millis() > 30000 and millis() > DecagonCTD::sensorLastUpdated + 30000) or DecagonCTD::sensorLastUpdated == 0)
    {
        Serial.println(F("Value out of date, updating"));  // For debugging
        DecagonCTD::update();
    }
    return DecagonCTD::sensorValue_temp;
}

String DecagonCTD_Temp::getDreamHost(void)
{
String column = F("CTDtemp");
return column;
}




DecagonCTD_Cond::DecagonCTD_Cond(char SDI12address, int powerPin, int dataPin, int numReadings)
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings), DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

String DecagonCTD_Cond::getVarName(void)
{
    varName = F("specificConductance");
    return varName;
}

String DecagonCTD_Cond::getVarUnit(void)
{
    String unit = F("microsiemenPerCentimeter");
    return unit;
}

float DecagonCTD_Cond::getValue(void)
{
    if ((millis() > 30000 and millis() > DecagonCTD::sensorLastUpdated + 30000) or DecagonCTD::sensorLastUpdated == 0)
    {
        Serial.println(F("Value out of date, updating"));  // For debugging
        DecagonCTD::update();
    }
    return DecagonCTD::sensorValue_cond;
}

String DecagonCTD_Cond::getDreamHost(void)
{
String column = F("CTDcond");
return column;
}
