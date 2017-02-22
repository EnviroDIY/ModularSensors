/*
 *CampbellOSB3.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.
 *
 *This file is for the Campbell Scientific OSB-3+
 *This is dependent on the Adafruit ADS1015 library.
*/

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "CampbellOSB3.h"

// The constructor - need the power pin, and the two data pins
CampbellOSB3::CampbellOSB3(int powerPin, int dataPin,
                           float A, float B, float C)
                         : SensorBase()
{
  _powerPin = powerPin;
  _dataPin = dataPin;
  _dataPin = dataPin;
  _A = A;
  _B = B;
  _C = C;
}

// The function to set up connection to a sensor.
SENSOR_STATUS CampbellOSB3::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    return SENSOR_READY;
}

// The function to put the sensor to sleep
bool CampbellOSB3::sleep(void)
{
    digitalWrite(_powerPin, LOW);
    return true;
}

// The function to wake up the sensor
bool CampbellOSB3::wake(void)
{
    digitalWrite(_powerPin, HIGH);
    return true;
}

// The sensor name
String CampbellOSB3::getSensorName(void)
{
    sensorName = F("CampbellOSB3+");
    return sensorName;
}

// The sensor installation location on the Mayfly
String CampbellOSB3::getSensorLocation(void)
{
    sensorLocation = "ads" + String(_dataPin);
    return sensorLocation;
}

// The static variables that need to be updated
float CampbellOSB3::sensorValue = 0;
unsigned long CampbellOSB3::sensorLastUpdated;

// Uses Auxillary ADD to convert data
bool CampbellOSB3::update(){

    // Start the Auxillary ADD
    Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */
    delay(500);

    // Check if the power is on, turn it on if not
    bool wasOff = false;
    int powerBitNumber = log(digitalPinToBitMask(_powerPin))/log(2);
    if (bitRead(*portInputRegister(digitalPinToPort(_powerPin)), powerBitNumber) == LOW)
    {
        wasOff = true;
        digitalWrite(_powerPin, HIGH);
        delay(1000);
    }

    int16_t adc0; // tells which channels are to be read

    adc0 = ads.readADC_SingleEnded(_dataPin);

    // now convert bits into millivolts
    // SRGD:  Where does this come from???
    float voltage = (adc0 * 3.3) / 17585.0;

    // calibration information below if only for instrument SN# S9743
    // TODO:  set this up so calibration can be input at top for each instrument
    sensorValue =  (4.6641 * square (voltage)) + (92.512 * voltage) - 0.38548;

    CampbellOSB3::sensorValue = sensorValue;
    CampbellOSB3::sensorLastUpdated = millis();

    // Turn the power back off it it had been turned on
    if (wasOff)
        {digitalWrite(_powerPin, LOW);}

    // Return true when finished
    return true;
}

String CampbellOSB3::getVarName(void)
{
    varName = F("turbidity");
    return varName;
}

String CampbellOSB3::getVarUnit(void)
{
    String unit = F("nephelometricTurbidityUnit");
    return unit;
}

float CampbellOSB3::getValue(void)
{
    if (millis() > 30000 and millis() > CampbellOSB3::sensorLastUpdated + 30000)
        {CampbellOSB3::update();}
    return CampbellOSB3::sensorValue;
}




CampbellOSB3_Turbidity::CampbellOSB3_Turbidity(int powerPin, int dataPin,
                                           float A, float B, float C)
 : CampbellOSB3(powerPin, dataPin, A, B, C)
{}

String CampbellOSB3_Turbidity::getDreamHost(void)
{
String column = F("TurbLow");
return column;
}



CampbellOSB3_TurbHigh::CampbellOSB3_TurbHigh(int powerPin, int dataPin,
                                             float A, float B, float C)
 : CampbellOSB3(powerPin, dataPin, A, B, C)
{}

String CampbellOSB3_TurbHigh::getDreamHost(void)
{
String column = F("TurbHigh");
return column;
}
