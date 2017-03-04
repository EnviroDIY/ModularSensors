/*
 *DecagonES2.cpp
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

#include "DecagonES2.h"

// The constructor - need the SDI-12 address, the power pin, the data pin, and the number of readings
DecagonES2::DecagonES2(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin),
   DecagonSDI12(SDI12address, powerPin, dataPin, numReadings)
{}

// The static variables that need to be updated
float DecagonES2::sensorValue_cond = 0;
float DecagonES2::sensorValue_temp = 0;
unsigned long DecagonES2::sensorLastUpdated = 0;
bool DecagonES2::update(void)
{
    DecagonSDI12::update();
    DecagonES2::sensorValue_cond = DecagonSDI12::sensorValues[0];
    DecagonES2::sensorValue_temp = DecagonSDI12::sensorValues[1];
    // Make note of the last time updated
    DecagonES2::sensorLastUpdated = millis();
    return true;
}



DecagonES2_Cond::DecagonES2_Cond(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F("DecagonES2"), F("specificConductance"), F("microsiemenPerCentimeter"), F("ES2Cond")),
   DecagonSDI12(SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonES2_Cond::getValue(void)
{
    checkForUpdate(DecagonES2::sensorLastUpdated);
    return DecagonES2::sensorValue_cond;
}




DecagonES2_Temp::DecagonES2_Temp(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F("DecagonES2"), F("temperature"), F("degreeCelsius"), F("ES2temp")),
   DecagonSDI12(SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonES2_Temp::getValue(void)
{
    checkForUpdate(DecagonES2::sensorLastUpdated);
    return DecagonES2::sensorValue_temp;
}
