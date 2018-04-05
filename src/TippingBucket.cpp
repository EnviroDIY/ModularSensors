/*
 *BoschBME280.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Bosch BME280 Digital Pressure and Humidity Sensor
 *It is dependent on the Adafruit BME280 Library
 *
 *Documentation for the sensor can be found at:
 *https://www.bosch-sensortec.com/bst/products/all_products/bme280
 *
 * For Barometric Pressure:
 *  Resolution is 0.18Pa
 *  Absolute Accuracy is ±1hPa
 *  Relative Accuracy is ±0.12hPa
 *  Range is 300 to 1100 hPa
 *
 * For Temperature:
 *  Resolution is 0.01°C
 *  Accuracy is ±0.5°C
 *  Range is -40°C to +85°C
 *
 * For Humidity:
 *  Resolution is 0.008 % RH (16 bit)
 *  Accuracy is ± 3 % RH
 *
 * Slowest response time (humidity): 1sec
*/

//FIX intro!!

#include "TippingBucket.h"


// The constructor - because this is I2C, only need the power pin
TippingBucket::TippingBucket(int8_t powerPin, uint8_t i2cAddressHex, uint8_t measurementsToAverage)
     : Sensor(F("TippingBucket"), BUKCET_NUM_VARIABLES,
              BUCKET_WARM_UP_TIME_MS, BUCKET_STABILIZATION_TIME_MS, BUCKET_MEASUREMENT_TIME_MS,
              powerPin, -1) //What is -1?? 
{
    _i2cAddressHex  = i2cAddressHex;
}


String TippingBucket::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


// bool TippingBucket::getStatus(void)
// {
//     // Check if the power is on, turn it on if not (Need power to get status)
//     bool wasOn = checkPowerOn();
//     if(!wasOn){powerUp();}
//     // Wait until the sensor is warmed up
//     waitForWarmUp();

//     // Run begin fxn because it returns true or false for success in contact
//     // bool status = bme_internal.begin(_i2cAddressHex);
//     Wire.beginTransmission(_i2cAddressHex);
//     bool status = false;
//     if(Wire.endTransmission() == 0) status = true; //Set status to true if device is found at address specified 

//     // Turn the power back off it it had been turned on
//     if(!wasOn){powerDown();}

//     if (!status) return false;
//     else return true;
// }


bool TippingBucket::setup(void)
{
    Sensor::setup();
    Wire.begin(); //Right place?? 
    return true;
}


bool TippingBucket::wake(void)
{
    Sensor::wake();
    waitForWarmUp();
    // Restart always needed after power-up
    // As of Adafruit library version 1.0.7, this function includes all of the
    // various delays to allow the chip to wake up, get calibrations, get
    // coefficients, and set sampling modes.
    // Currently this is using the settings that Adafruit considered to be 'default'
    //  - sensor mode = normal (sensor measures, sleeps for the "standby time" and then automatically remeasures
    //  - temperature oversampling = 16x
    //  - pressure oversampling = 16x
    //  - humidity oversampling = 16x
    //  - built-in IIR filter = off oversampling = 16x
    //  - sleep time between measurements = 0.5ms
    // bme_internal.begin(_i2cAddressHex);

    // // When the Adafruit library is updated to remove the built-in delay after
    // // forcing a sample, it would be better to operate in forced mode.
    // bme_internal.setSampling(Adafruit_BME280::MODE_NORMAL,  // sensor mode
    // // bme_internal.setSampling(Adafruit_BME280::MODE_FORCED,  // sensor mode
    //                          Adafruit_BME280::SAMPLING_X16,  // temperature oversampling
    //                          Adafruit_BME280::SAMPLING_X16,  //  pressure oversampling
    //                          Adafruit_BME280::SAMPLING_X16,  //  humidity oversampling
    //                          Adafruit_BME280::FILTER_OFF, // built-in IIR filter
    //                          Adafruit_BME280::STANDBY_MS_1000);  // sleep time between measurements (N/A in forced mode)
    // delay(100);  // Need this delay after changing sampling mode
    delay(1); //Wake is not needed strictly, remove??
    return true;
}

// For operating in forced mode
// bool BoschBME280::startSingleMeasurement(void)
// {
//     // waitForWarmUp();  // already done in wake
//     waitForStability();
//     bme_internal.takeForcedMeasurement(false);  // Don't want to wait to finish here
//     _lastMeasurementRequested = millis();
//     return true;
// }


bool TippingBucket::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForMeasurementCompletion();

    // Read values
    float rain = 0; 
    uint8_t Byte1 = 0;
    uint8_t Byte2 = 0;

    Wire.requestFrom(_i2cAddressHex, 2);
    Byte1 = Wire.read();
    Byte2 = Wire.read();

    rain = float((Byte2 << 8) | (Byte1)) * 0.01; //Concatonate to 16 bit result, multiply by 0.01 to get inches of rain, moddify to work with number of tips instead??

    if (rain < 0) rain = -9999; //Make sure this is correct failure condition!! 

    MS_DBG(F("Rain: "), rain);
    // MS_DBG(F(" Humidity: "), humid);
    // MS_DBG(F(" Barometric Pressure: "), press);
    // MS_DBG(F(" Calculated Altitude: "), alt, F("\n"));

    verifyAndAddMeasurementResult(BUCKET_RAIN_VAR_NUM, rain);

    // Return true when finished
    return true;
}
