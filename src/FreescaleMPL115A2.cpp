/*
 *MPL115A2.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Bobby Schulz <schu3119@umn.edu>.
 *
 *This file is for the Freescale Semiconductor MPL115A2 Miniature I2C Digital
 *Barometer
 *It is dependent on the https://github.com/adafruit/Adafruit_MPL115A2 library
 *
 *Documentation for the sensor can be found at:
 *https://www.adafruit.com/product/992
 *https://github.com/adafruit/Adafruit-MPL115A2-Breakout-PCB
 *
 * For Pressure:
 *  Resolution is 1.5 hPa
 *  Accuracy ±10 hPa
 *  Range is 500-1150 hPa
 *
 * Sensor takes about 1.6 ms to respond
 * Assume sensor is immediately stable
*/

#include "FreescaleMPL115A2.h"


// The constructor - because this is I2C, only need the power pin
// This sensor has a set I2C address of 0x60.
MPL115A2::MPL115A2(int8_t powerPin, uint8_t measurementsToAverage)
     : Sensor(F("MPL115A2"), MPL115A2_NUM_VARIABLES,
              MPL115A2_WARM_UP_TIME_MS, MPL115A2_STABILIZATION_TIME_MS, MPL115A2_MEASUREMENT_TIME_MS,
              powerPin, -1, measurementsToAverage)
{}


String MPL115A2::getSensorLocation(void){return F("I2C_0x60");}


bool MPL115A2::setup(void)
{
    mpl115a2_internal.begin();
    return Sensor::setup();  // this will set timestamp and status bit
}


bool MPL115A2::addSingleMeasurementResult(void)
{
    // Initialize float variables
    float temp = -9999;
    float press = -9999;

    if (_millisMeasurementRequested > 0)
    {
        // Read values
        mpl115a2_internal.getPT(&press,&temp);

        if (isnan(temp)) temp = -9999;
        if (isnan(press)) press = -9999;

        if(press > 115.0 || temp < -40.0) {
            temp = -9999;
            press = -9999;
        }

        MS_DBG(F("Temperature: "), temp);
        MS_DBG(F("Pressure: "), press);
    }
    else MS_DBG(F("Sensor is not currently measuring!\n"));

    verifyAndAddMeasurementResult(MPL115A2_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(MPL115A2_PRESSURE_VAR_NUM, press);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    // no way of knowing if successful, just return true
    return true;
}
