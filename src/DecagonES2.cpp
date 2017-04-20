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
 : SensorBase(dataPin, powerPin),
   DecagonSDI12(2, SDI12address, powerPin, dataPin, numReadings)
{}

// The static variables that need to be updated
float DecagonES2::sensorValue_cond = 0;
float DecagonES2::sensorValue_temp = 0;
unsigned long DecagonES2::sensorLastUpdated = 0;
String DecagonES2::sensorAddress = "";  // Hack to make sure has the same address
bool DecagonES2::update(void)
{
    DecagonSDI12::update();
    DecagonES2::sensorValue_cond = DecagonSDI12::sensorValues[0];
    DecagonES2::sensorValue_temp = DecagonSDI12::sensorValues[1];
    // Make note of the last time updated
    DecagonES2::sensorLastUpdated = millis();
    DecagonES2::sensorAddress = getSensorLocation();  // Hack to make sure has the same address
    return true;
}




DecagonES2_Cond::DecagonES2_Cond(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F("DecagonES2"), F("specificConductance"), F("microsiemenPerCentimeter"), ES2_COND_RESOLUTION, F("ES2Cond")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonES2_Cond::getValue(void)
{
    if (getSensorLocation() != DecagonES2::sensorAddress) DecagonES2::sensorLastUpdated = 0;
    checkForUpdate(DecagonES2::sensorLastUpdated);
    return DecagonES2::sensorValue_cond;
}





DecagonES2_Temp::DecagonES2_Temp(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F("DecagonES2"), F("temperature"), F("degreeCelsius"), ES2_TEMP_RESOLUTION, F("ES2temp")),
   DecagonSDI12(ES2_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonES2(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonES2_Temp::getValue(void)
{
    if (getSensorLocation() != DecagonES2::sensorAddress) DecagonES2::sensorLastUpdated = 0;
    checkForUpdate(DecagonES2::sensorLastUpdated);
    return DecagonES2::sensorValue_temp;
}
