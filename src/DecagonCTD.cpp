/*
 *DecagonCTD.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Decagon Devices CTD-5 or CTD-10
 *It is dependent on the EnviroDIY SDI-12 library and the DecagonSDI12 super class.
 *
 *Documentation fo the SDI-12 Protocol commands and responses
 *for the Decagon CTD-10 can be found at:
 *http://manuals.decagon.com/Integration%20Guides/CTD%20Integrators%20Guide.pdf
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

#include "DecagonCTD.h"


// The constructor - need the SDI-12 address, the power pin, the data pin, and the number of readings
DecagonCTD::DecagonCTD(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin),
   DecagonSDI12(CTD_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings)
{}


// The static variables that need to be updated
float DecagonCTD::sensorValue_depth = 0;
float DecagonCTD::sensorValue_temp = 0;
float DecagonCTD::sensorValue_cond = 0;
unsigned long DecagonCTD::sensorLastUpdated = 0;
bool DecagonCTD::update()
{
    DecagonSDI12::update();
    for (int i = 0; i < 3; i++)
    {
        Serial.print(F("Sub Result #"));  // For debugging
        Serial.print(i);  // For debugging
        Serial.print(F(": "));  // For debugging
        Serial.println(sensorValues[i]);  // For debugging
    }
    DecagonCTD::sensorValue_depth = sensorValues[0];
    DecagonCTD::sensorValue_temp = sensorValues[1];
    DecagonCTD::sensorValue_cond = sensorValues[2];
    // Make note of the last time updated
    DecagonCTD::sensorLastUpdated = millis();
    return true;
}




DecagonCTD_Depth::DecagonCTD_Depth(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F("DecagonCTD"), F("waterDepth"), F("millimeter"), CTD_DEPTH_RESOLUTION, F("CTDdepth")),
   DecagonSDI12(CTD_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonCTD_Depth::getValue(void)
{
    checkForUpdate(DecagonCTD::sensorLastUpdated);
    return DecagonCTD::sensorValue_depth;
}





DecagonCTD_Temp::DecagonCTD_Temp(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F("DecagonCTD"), F("temperature"), F("degreeCelsius"), CTD_TEMP_RESOLUTION, F("CTDtemp")),
   DecagonSDI12(CTD_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonCTD_Temp::getValue(void)
{
    checkForUpdate(DecagonCTD::sensorLastUpdated);
    return DecagonCTD::sensorValue_temp;
}




DecagonCTD_Cond::DecagonCTD_Cond(char SDI12address, int powerPin, int dataPin, int numReadings)
 : SensorBase(dataPin, powerPin, F("DecagonCTD"), F("specificConductance"), F("microsiemenPerCentimeter"), CTD_COND_RESOLUTION, F("CTDcond")),
   DecagonSDI12(CTD_NUM_MEASUREMENTS, SDI12address, powerPin, dataPin, numReadings),
   DecagonCTD(SDI12address, powerPin, dataPin, numReadings)
{}

float DecagonCTD_Cond::getValue(void)
{
    checkForUpdate(DecagonCTD::sensorLastUpdated);
    return DecagonCTD::sensorValue_cond;
}
