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
AOSongAM2315::AOSongAM2315(int powerPin)
 : SensorBase(-1, powerPin)
{}

String AOSongAM2315::getSensorLocation(void){return F("I2C_0xB8");}

// The static variables that need to be updated
float AOSongAM2315::sensorValue_humidity = 0;
float AOSongAM2315::sensorValue_temp = 0;
unsigned long AOSongAM2315::sensorLastUpdated = 0;
bool AOSongAM2315::update(void)
{
    Adafruit_AM2315 am2315;  // create a sensor object
    Wire.begin();  // Start the wire library

    bool ret_val = am2315.readTemperatureAndHumidity(AOSongAM2315::sensorValue_temp, AOSongAM2315::sensorValue_humidity);
    // Make note of the last time updated
    AOSongAM2315::sensorLastUpdated = millis();
    return ret_val;
}




AOSongAM2315_Humidity::AOSongAM2315_Humidity(int powerPin)
 : SensorBase(-1, powerPin, F("AOSongAM2315"), F("relativeHumidity"), F("percent"), AM2315_HUMIDITY_RESOLUTION, F("AM2315Humidity")),
   AOSongAM2315(powerPin)
{}

float AOSongAM2315_Humidity::getValue(void)
{
    checkForUpdate(AOSongAM2315::sensorLastUpdated);
    return AOSongAM2315::sensorValue_humidity;
}





AOSongAM2315_Temp::AOSongAM2315_Temp(int powerPin)
 : SensorBase(-1, powerPin, F("AOSongAM2315"), F("temperature"), F("degreeCelsius"), AM2315_TEMP_RESOLUTION, F("AM2315YTemp")),
   AOSongAM2315(powerPin)
{}

float AOSongAM2315_Temp::getValue(void)
{
    checkForUpdate(AOSongAM2315::sensorLastUpdated);
    return AOSongAM2315::sensorValue_temp;
}
