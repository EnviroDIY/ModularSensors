/*
 *DecagonES2.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices ES-2 Electrical Conductivity Sensor
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon 5TM can be found at:
 * http://manuals.decagon.com/Integration%20Guides/ES-2%20Integrators%20Guide.pdf
 *
 * For Specific Conductance:
 *  Resolution is 0.001 mS/cm = 1 µS/cm
 *  Accuracy is ±0.01mS/cm or ±10% (whichever is greater)
 *  Range is 0 – 120 mS/cm (bulk)
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±1°C
 *  Range is -40°C to +50°C
*/

#include "DecagonES2.h"

// The constructor - need the SDI-12 address, the power pin, the data pin, and the number of readings
DecagonES2::DecagonES2(char SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin),
   DecagonSDI12(2, SDI12address, powerPin, dataPin, numReadings)
{
    // Create the subclasses for the parameters
    Cond = new DecagonES2_Cond(SDI12address, powerPin, dataPin, numReadings);
    Temp = new DecagonES2_Temp(SDI12address, powerPin, dataPin, numReadings);
}

DecagonES2::DecagonES2(char *SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin),
   DecagonSDI12(2, SDI12address, powerPin, dataPin, numReadings)
{
    // Create the subclasses for the parameters
    Cond = new DecagonES2_Cond(SDI12address, powerPin, dataPin, numReadings);
    Temp = new DecagonES2_Temp(SDI12address, powerPin, dataPin, numReadings);
}

DecagonES2::DecagonES2(int SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin),
   DecagonSDI12(2, SDI12address, powerPin, dataPin, numReadings)
{
    // Create the subclasses for the parameters
    Cond = new DecagonES2_Cond(SDI12address, powerPin, dataPin, numReadings);
    Temp = new DecagonES2_Temp(SDI12address, powerPin, dataPin, numReadings);
}

bool DecagonES2::update(void)
{
    DecagonSDI12::update();
    sensorValue_cond = DecagonSDI12::sensorValues[0];
    sensorValue_temp = DecagonSDI12::sensorValues[1];
    // Make note of the last time updated
    sensorLastUpdated = millis();
    Cond->updateValue(sensorLastUpdated, sensorValue_cond);
    Temp->updateValue(sensorLastUpdated, sensorValue_temp);
    return true;
}




DecagonES2_Cond::DecagonES2_Cond(char SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin, F("DecagonES2"), F("specificConductance"), F("microsiemenPerCentimeter"), ES2_COND_RESOLUTION, F("ES2Cond")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}
DecagonES2_Cond::DecagonES2_Cond(char *SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin, F("DecagonES2"), F("specificConductance"), F("microsiemenPerCentimeter"), ES2_COND_RESOLUTION, F("ES2Cond")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}
DecagonES2_Cond::DecagonES2_Cond(int SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin, F("DecagonES2"), F("specificConductance"), F("microsiemenPerCentimeter"), ES2_COND_RESOLUTION, F("ES2Cond")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}

void DecagonES2_Cond::updateValue(unsigned long updateTime, float value)
{
    sensorLastUpdated = updateTime;
    sensorValue_cond = value;
}

float DecagonES2_Cond::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_cond;
}





DecagonES2_Temp::DecagonES2_Temp(char SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin, F("DecagonES2"), F("temperature"), F("degreeCelsius"), ES2_TEMP_RESOLUTION, F("ES2temp")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}
DecagonES2_Temp::DecagonES2_Temp(char *SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin, F("DecagonES2"), F("temperature"), F("degreeCelsius"), ES2_TEMP_RESOLUTION, F("ES2temp")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}
DecagonES2_Temp::DecagonES2_Temp(int SDI12address, int powerPin, int dataPin, int numReadings)
 : Sensor(dataPin, powerPin, F("DecagonES2"), F("temperature"), F("degreeCelsius"), ES2_TEMP_RESOLUTION, F("ES2temp")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}

void DecagonES2_Temp::updateValue(unsigned long updateTime, float value)
{
    sensorLastUpdated = updateTime;
    sensorValue_temp = value;
}

float DecagonES2_Temp::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_temp;
}
