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
 *
 * Warm up/stability/re-sampling time: 2sec
*/
#include "AOSongAM2315.h"
#include <Adafruit_AM2315.h>


// The constructor - because this is I2C, only need the power pin
AOSongAM2315::AOSongAM2315(int powerPin, int readingsToAverage)
    : Sensor(F("AOSongAM2315"), AM2315_NUM_VARIABLES,
             AM2315_WARM_UP, AM2315_STABILITY, AM2315_RESAMPLE,
             powerPin, -1, readingsToAverage)
{}


String AOSongAM2315::getSensorLocation(void){return F("I2C_0xB8");}


SENSOR_STATUS AOSongAM2315::setup(void)
{
    Wire.begin();  // Start the wire library
    return Sensor::setup();
}


bool AOSongAM2315::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForNextMeasurement();

    Adafruit_AM2315 am2315;  // create a sensor object

    float temp_val, humid_val;
    bool ret_val = am2315.readTemperatureAndHumidity(temp_val, humid_val);

    sensorValues[AM2315_TEMP_VAR_NUM] += temp_val;
    sensorValues[AM2315_HUMIDITY_VAR_NUM] += humid_val;

    MS_DBG(F("Temp is: "), temp_val, F("°C"));
    MS_DBG(F(" and humidity is: "), humid_val, F("%\n"));

    return ret_val;
}
