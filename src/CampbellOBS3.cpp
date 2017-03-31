/*
 *CampbellOBS3.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Campbell Scientific OBS-3+
 *This is dependent on the Adafruit ADS1015 library.
*/

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "CampbellOBS3.h"

// The constructor - need the power pin, the data pin, and the calibration info
CampbellOBS3::CampbellOBS3(int powerPin, int dataPin, float A, float B, float C)
  : SensorBase(dataPin, powerPin)
{
    _A = A;
    _B = B;
    _C = C;
}

// The sensor installation location on the Mayfly
String CampbellOBS3::getSensorLocation(void)
{
    sensorLocation = "ads" + String(_dataPin);
    return sensorLocation;
}

// The static variables that need to be updated
float CampbellOBS3::sensorValue = 0;
unsigned long CampbellOBS3::sensorLastUpdated;

// Uses Auxillary ADD to convert data
bool CampbellOBS3::update(){

    // Start the Auxillary ADD
    Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */
    delay(500);

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    int16_t adcChannel; // tells which channels are to be read

    adcChannel = ads.readADC_SingleEnded(_dataPin);

    // now convert bits into millivolts
    // 3.3 is the voltage applied to the sensor (and its returun range)
    // The 17585 is the default bit gain of the ADS1115
    float voltage = (adcChannel * 3.3) / 17585.0;

    sensorValue =  (_A * square (voltage)) + (_B * voltage) - _C;

    CampbellOBS3::sensorValue = sensorValue;
    CampbellOBS3::sensorLastUpdated = millis();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Return true when finished
    return true;
}

float CampbellOBS3::getValue(void)
{
    checkForUpdate(CampbellOBS3::sensorLastUpdated);
    return CampbellOBS3::sensorValue;
}




CampbellOBS3_Turbidity::CampbellOBS3_Turbidity(int powerPin, int dataPin, float A, float B, float C)
  : SensorBase(dataPin, powerPin, F("CampbellOBS3+"), F("turbidity"), F("nephelometricTurbidityUnit"), F("TurbLow")),
    CampbellOBS3(powerPin, dataPin, A, B, C)
{}




CampbellOBS3_TurbHigh::CampbellOBS3_TurbHigh(int powerPin, int dataPin, float A, float B, float C)
  : SensorBase(dataPin, powerPin, F("CampbellOBS3+"), F("turbidity"), F("nephelometricTurbidityUnit"), F("TurbHigh")),
    CampbellOBS3(powerPin, dataPin, A, B, C)
{}
