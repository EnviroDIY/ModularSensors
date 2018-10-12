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
// This sensor has a set I2C address of 0XB8
AOSongAM2315::AOSongAM2315(int8_t powerPin, uint8_t measurementsToAverage)
    : Sensor("AOSongAM2315", AM2315_NUM_VARIABLES,
             AM2315_WARM_UP_TIME_MS, AM2315_STABILIZATION_TIME_MS, AM2315_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage)
{}
AOSongAM2315::~AOSongAM2315(){}


String AOSongAM2315::getSensorLocation(void){return F("I2C_0xB8");}


bool AOSongAM2315::setup(void)
{
    Wire.begin();  // Start the wire library (sensor power not required)
    return Sensor::setup();  // this will set pin modes and the setup status bit
}


bool AOSongAM2315::addSingleMeasurementResult(void)
{
    // Initialize float variables
    float temp_val = -9999;
    float humid_val = -9999;
    bool ret_val = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        MS_DBG(F("Getting values from "), getSensorName());
        Adafruit_AM2315 am2315;  // create a sensor object
        ret_val = am2315.readTemperatureAndHumidity(temp_val, humid_val);

        if (!ret_val or isnan(temp_val)) temp_val = -9999;
        if (!ret_val or isnan(humid_val)) humid_val = -9999;

        MS_DBG(F("Temp is: "), temp_val, F("°C"));
        MS_DBG(F(" and humidity is: "), humid_val, F("%"));
    }
    else MS_DBG(getSensorNameAndLocation(), F(" is not currently measuring!"));

    verifyAndAddMeasurementResult(AM2315_TEMP_VAR_NUM, temp_val);
    verifyAndAddMeasurementResult(AM2315_HUMIDITY_VAR_NUM, humid_val);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return ret_val;
}
