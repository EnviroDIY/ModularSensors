/*
 *CampbellOBS3.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Campbell Scientific OBS-3+
 *This is dependent on the Adafruit ADS1015 library.
*/

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "CampbellOBS3.h"

// The constructor - need the power pin, and the two data pins
CampbellOBS3::CampbellOBS3(int powerPin, int dataPin, float A, float B, float C)
  : SensorBase(dataPin, powerPin)
{
    _A = A;
    _B = B;
    _C = C;
    setup();
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

    int16_t adc0; // tells which channels are to be read

    adc0 = ads.readADC_SingleEnded(_dataPin);

    // now convert bits into millivolts
    // SRGD:  Where does this come from???
    float voltage = (adc0 * 3.3) / 17585.0;

    // calibration information below if only for instrument SN# S9743
    // TODO:  set this up so calibration can be input at top for each instrument
    sensorValue =  (4.6641 * square (voltage)) + (92.512 * voltage) - 0.38548;

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
