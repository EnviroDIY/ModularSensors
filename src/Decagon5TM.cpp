/*
 *Decagon5TM.cpp
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

#include "Decagon5TM.h"

// The constructor - need the SDI-12 address, the power pin, the data pin, and the number of readings
Decagon5TM::Decagon5TM(char SDI12address, int powerPin, int dataPin, int numReadings)
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings)
{}

// The static variables that need to be updated
unsigned long Decagon5TM::sensorLastUpdated = 0;
bool Decagon5TM::update(void)
{
    DecagonSDI12::update();
    // Make note of the last time updated
    Decagon5TM::sensorLastUpdated = millis();
    return true;
}



Decagon5TM_Ea::Decagon5TM_Ea(char SDI12address, int powerPin, int dataPin, int numReadings)
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings), Decagon5TM(SDI12address, powerPin, dataPin, numReadings)
{}

String Decagon5TM_Ea::getVarName(void)
{
    varName = F("waterPotential");
    return varName;
}

String Decagon5TM_Ea::getVarUnit(void)
{
    String unit = F("kilopascal");
    return unit;
}

float Decagon5TM_Ea::getValue(void)
{
    if (millis() > 30000 and millis() > Decagon5TM::sensorLastUpdated + 30000)
        {Decagon5TM::update();}
    return DecagonSDI12::sensorValues[0];
}

String Decagon5TM_Ea::getDreamHost(void)
{
    String column = F("soilEa");
    return column;
}




Decagon5TM_Temp::Decagon5TM_Temp(char SDI12address, int powerPin, int dataPin, int numReadings)
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings), Decagon5TM(SDI12address, powerPin, dataPin, numReadings)
{}

String Decagon5TM_Temp::getVarName(void)
{
    varName = F("temperature");
    return varName;
}

String Decagon5TM_Temp::getVarUnit(void)
{
    String unit = F("degreeCelsius");
    return unit;
}

float Decagon5TM_Temp::getValue(void)
{
    if (millis() > 30000 and millis() > Decagon5TM::sensorLastUpdated + 30000)
        {Decagon5TM::update();}
    return DecagonSDI12::sensorValues[1];
}

String Decagon5TM_Temp::getDreamHost(void)
{
    String column = F("soiltemp");
    return column;
}




Decagon5TM_VWC::Decagon5TM_VWC(char SDI12address, int powerPin, int dataPin, int numReadings)
 : DecagonSDI12(SDI12address, powerPin, dataPin, numReadings), Decagon5TM(SDI12address, powerPin, dataPin, numReadings)
{}

String Decagon5TM_VWC::getVarName(void)
{
    varName = F("volumetricWaterContent");
    return varName;
}

String Decagon5TM_VWC::getVarUnit(void)
{
    String unit = F("percent");
    return unit;
}

float Decagon5TM_VWC::getValue(void)
{
    if (millis() > 30000 and millis() > Decagon5TM::sensorLastUpdated + 30000)
        {Decagon5TM::update();}

    //the TOPP equation used to calculate VWC
    ea = DecagonSDI12::sensorValues[0];
    sensorValue_VWC = (4.3e-6*(ea*ea*ea))
                      - (5.5e-4*(ea*ea))
                      + (2.92e-2 * ea)
                      - 5.3e-2 ;
    return sensorValue_VWC;
}

String Decagon5TM_VWC::getDreamHost(void)
{
    String column = F("soilVWC");
    return column;
}
