/*
 *RainCounterI2C.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for an external tip counter, used to measure rainfall via a tipping bucket
 *rain gauge
 *
 *Documentation for the sensor can be found at:
 *https://github.com/EnviroDIY/TippingBucketRainGauge
 *
 * For Rainfall:
 *  Accuracy and resolution are dependent on the sensor used
 *  Standard resolution is 0.01" or 0.2mm of rainfall (depending on if sensor is set to english or metric)
 *
 * Assume sensor is immediately stable
*/

#include "RainCounterI2C.h"


// The constructor - because this is I2C, only need the power pin and rain per event if a non-standard value is used
RainCounterI2C::RainCounterI2C(uint8_t i2cAddressHex, float rainPerTip)
     : Sensor(F("RainCounterI2C"), BUCKET_NUM_VARIABLES,
              BUCKET_WARM_UP_TIME_MS, BUCKET_STABILIZATION_TIME_MS, BUCKET_MEASUREMENT_TIME_MS,
              -1, -1, 1)
{
    _i2cAddressHex  = i2cAddressHex;
    _rainPerTip = rainPerTip;
}


String RainCounterI2C::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool RainCounterI2C::setup(void)
{
    Sensor::setup();
    Wire.begin(); //Initalize wire (I2C) functionality
    return true;
}


bool RainCounterI2C::addSingleMeasurementResult(void)
{
    //intialize values
    float rain = -9999; // Number of mm of rain
    int tips = -9999; // Number of tip events
    uint8_t Byte1 = 0; // Low byte of data
    uint8_t Byte2 = 0; // High byte of data

    Wire.requestFrom(int(_i2cAddressHex), 2); // Get data from external tip counter
    Byte1 = Wire.read();
    Byte2 = Wire.read();

    tips = (Byte2 << 8) | (Byte1);  // Concatenate tip values
    rain = float(tips) * _rainPerTip; // Multiply by tip coefficient (0.2 by default)

    if (tips < 0) rain = -9999; // If negetive value results, return failure
    if (rain < 0) rain = -9999; // If negetive value results, return failure

    MS_DBG(F("Rain: "), rain);
    MS_DBG(F("Tips: "), tips);

    verifyAndAddMeasurementResult(BUCKET_RAIN_VAR_NUM, rain);
    verifyAndAddMeasurementResult(BUCKET_TIPS_VAR_NUM, int(tips));

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    // Return true when finished
    return true;
}
