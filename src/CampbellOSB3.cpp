/*
 *CampbellOSB3.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Work in progress by Sara Damiano taken from code written
 *by Shannon Hicks and templates from USU.

 *
 *This file is for the Decagon Devices 5TM Soil Moisture probe
 *It is dependent on the EnviroDIY SDI-12 library.
*/

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "CampbellOSB3.h"

// The constructor - need the power pin, and the two data pins
CampbellOSB3::CampbellOSB3(int powerPin, int dataPinLow, int dataPinHigh) : SensorBase()
{
  _powerPin = powerPin;
  _dataPinLow = dataPinLow;
  _dataPinHigh = dataPinHigh;
}

// The function to set up connection to a sensor.
SENSOR_STATUS CampbellOSB3::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    return SENSOR_READY;
}

// The sensor name
String CampbellOSB3::getSensorName(void)
{
    sensorName = F("CampbellOSB3");
    return sensorName;
}

// The sensor installation location on the Mayfly
String CampbellOSB3::getSensorLocation(void)
{
    sensorLocation = "ads" + String(_dataPinLow) + "_" + String(_dataPinHigh);
    return sensorLocation;
}

// The static variables that need to be updated
float CampbellOSB3::sensorValue_TurbLow = 0;
float CampbellOSB3::sensorValue_TurbHigh = 0;

// Uses SDI-12 to communicate with a Decagon Devices CTD
bool CampbellOSB3::update(){

  // Start the Auxillary ADD
  Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */

    // Turn on power to the sensor
    delay(500);
    digitalWrite(_powerPin, HIGH);
    delay(1000);

    int16_t adc0, adc1; // tells which channels are to be read

    adc0 = ads.readADC_SingleEnded(_dataPinLow);
    adc1 = ads.readADC_SingleEnded(_dataPinHigh);

    //now convert bits into millivolts
    float lowvoltage = (adc0 * 3.3) / 17585.0;
    float highvoltage = (adc1 * 3.3) / 17585.0;

    // calibration information below if only for instrument SN# S9743
    // TODO:  set this up so calibration can be input at top for each instrument
    sensorValue_TurbLow =  (4.6641 * square (lowvoltage)) + (92.512 * lowvoltage) - 0.38548;
    sensorValue_TurbHigh = (53.845 * square (highvoltage)) + (383.18 * highvoltage) - 1.3555;

    CampbellOSB3::sensorValue_TurbLow = sensorValue_TurbLow;
    CampbellOSB3::sensorValue_TurbHigh = sensorValue_TurbHigh;

    // Turn off power to the sensor
    digitalWrite(_powerPin, LOW);
    delay(100);

    // Return true when finished
    return true;
}




CampbellOSB3_TurbLow::CampbellOSB3_TurbLow(int powerPin, int dataPinLow, int dataPinHigh)
 : CampbellOSB3(powerPin, dataPinLow, dataPinHigh)
{}

String CampbellOSB3_TurbLow::getVarName(void)
{
    varName = F("turbidity");
    return varName;
}

String CampbellOSB3_TurbLow::getVarUnit(void)
{
    String unit = F("nephelometricTurbidityUnit");
    return unit;
}

float CampbellOSB3_TurbLow::getValue(void)
{
    return sensorValue_TurbLow;
}

String CampbellOSB3_TurbLow::getDreamHost(void)
{
String column = F("TurbLow");
return column;
}



CampbellOSB3_TurbHigh::CampbellOSB3_TurbHigh(int powerPin, int dataPinLow, int dataPinHigh)
 : CampbellOSB3(powerPin, dataPinLow, dataPinHigh)
{}

String CampbellOSB3_TurbHigh::getVarName(void)
{
    varName = F("turbidity");
    return varName;
}

String CampbellOSB3_TurbHigh::getVarUnit(void)
{
    String unit = F("nephelometricTurbidityUnit");
    return unit;
}

float CampbellOSB3_TurbHigh::getValue(void)
{
    return sensorValue_TurbHigh;
}

String CampbellOSB3_TurbHigh::getDreamHost(void)
{
String column = F("TurbHigh");
return column;
}
