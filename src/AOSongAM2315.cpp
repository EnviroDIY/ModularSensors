/*
 *AOSongAM2315.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the AOSong AM2315 Capacitive Humidity and Temperature sensor
 *It is dependent on the Adafruit AM2315 Library
 *
 *Documentation for the sensor can be found at: www.aosong.com/asp_bin/Products/en/AM2315.pdf
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH (16 bit)
 *  Accuracy is ± 2 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C (16 bit)
 *  Accuracy is ±0.1°C
 *  Range is -40°C to +125°C
*/

#include "AOSongAM2315.h"
#include <Adafruit_AM2315.h>

// The constructor - because this is I2C, only need the power pin
AOSongAM2315::AOSongAM2315(void){}

String AOSongAM2315::getSensorLocation(void){return F("I2C_0xB8");}
String AOSongAM2315::getSensorName(void){return F("AOSongAM2315");}


bool AOSongAM2315::update(void)
{
    Adafruit_AM2315 am2315;  // create a sensor object
    Wire.begin();  // Start the wire library

    bool ret_val = am2315.readTemperatureAndHumidity(Temp->sensorValue, Humid->sensorValue);
    // Make note of the last time updated
    sensorLastUpdated = millis();
    return ret_val;
}




AOSongAM2315_Humidity::AOSongAM2315_Humidity(void)
{}

float AOSongAM2315_Humidity::getValue(void)
{
    // checkForUpdate(AOSongAM2315::sensorLastUpdated);
    return sensorValue;
}





AOSongAM2315_Temp::AOSongAM2315_Temp(void)
{}

float AOSongAM2315_Temp::getValue(void)
{
    // checkForUpdate(AOSongAM2315::sensorLastUpdated);
    return sensorValue;
}
