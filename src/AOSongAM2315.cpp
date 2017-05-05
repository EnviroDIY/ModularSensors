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
: Sensor(-1, powerPin, F("AOSongAM2315"), AM2315_NUM_MEASUREMENTS)
{}

String AOSongAM2315::getSensorLocation(void){return F("I2C_0xB8");}

bool AOSongAM2315::update(void)
{
    Adafruit_AM2315 am2315;  // create a sensor object
    Wire.begin();  // Start the wire library

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    // Clear values before starting loop
    clearValues();

    float temp_val, humid_val;
    bool ret_val = am2315.readTemperatureAndHumidity(temp_val, humid_val);
    sensorValues[AM2315_TEMP_VAR_NUM] = temp_val;
    sensorValues[AM2315_HUMIDITY_VAR_NUM] = humid_val;

    DBGM(F("Temp is: "), sensorValues[AM2315_TEMP_VAR_NUM], F("°C"));
    DBGM(F(" and humidity is: "), sensorValues[AM2315_HUMIDITY_VAR_NUM], F("%\n"));

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    return ret_val;
}
