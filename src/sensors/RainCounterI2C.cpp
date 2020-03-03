/*
 *RainCounterI2C.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for an external tip counter, used to measure rainfall via a
 *tipping bucket rain gauge
 *
 *Documentation for the sensor can be found at:
 *https://github.com/EnviroDIY/TippingBucketRainCounter
 *
 * For Rainfall:
 *  Accuracy and resolution are dependent on the sensor used
 *  Standard resolution is 0.01" or 0.2mm of rainfall (depending on if sensor is
 *set to english or metric)
 *
 * Assume sensor is immediately stable
 */

#include "RainCounterI2C.h"


// The constructor - because this is I2C, only need the power pin and rain per
// event if a non-standard value is used
RainCounterI2C::RainCounterI2C(uint8_t i2cAddressHex, float rainPerTip)
    : Sensor("RainCounterI2C", BUCKET_NUM_VARIABLES, BUCKET_WARM_UP_TIME_MS,
             BUCKET_STABILIZATION_TIME_MS, BUCKET_MEASUREMENT_TIME_MS, -1, -1,
             1) {
    _i2cAddressHex = i2cAddressHex;
    _rainPerTip    = rainPerTip;
}
// Destructor
RainCounterI2C::~RainCounterI2C() {}


String RainCounterI2C::getSensorLocation(void) {
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool RainCounterI2C::setup(void) {
    Wire.begin();  // Start the wire library (sensor power not required)
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);
    return Sensor::setup();  // this will set pin modes and the setup status bit
}


bool RainCounterI2C::addSingleMeasurementResult(void) {
    // intialize values
    uint8_t Byte1 = 0;  // Low byte of data
    uint8_t Byte2 = 0;  // High byte of data

    float   rain = -9999;  // Number of mm of rain
    int16_t tips = -9999;  // Number of tip events

    // Get data from external tip counter
    // if the 'requestFrom' returns 0, it means no bytes were received
    if (Wire.requestFrom(int(_i2cAddressHex), 2)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        Byte1 = Wire.read();
        Byte2 = Wire.read();

        tips = (Byte2 << 8) | (Byte1);  // Concatenate tip values
        rain = float(tips) *
            _rainPerTip;  // Multiply by tip coefficient (0.2 by default)

        if (tips < 0)
            tips = -9999;  // If negetive value results, return failure
        if (rain < 0)
            rain = -9999;  // If negetive value results, return failure

        MS_DBG(F("  Rain:"), rain);
        MS_DBG(F("  Tips:"), tips);
    } else {
        MS_DBG(F("No bytes received from"), getSensorNameAndLocation());
    }

    verifyAndAddMeasurementResult(BUCKET_RAIN_VAR_NUM, rain);
    verifyAndAddMeasurementResult(BUCKET_TIPS_VAR_NUM, tips);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
