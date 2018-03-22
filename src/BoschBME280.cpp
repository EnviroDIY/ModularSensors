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

#include "BoschBME280.h"


// The constructor - because this is I2C, only need the power pin
BoschBME280::BoschBME280(int8_t powerPin, uint8_t i2cAddressHex, uint8_t measurementsToAverage)
     : Sensor(F("BoschBME280"), BME280_NUM_VARIABLES,
              BME280_WARM_UP_TIME_MS, BME280_STABILIZATION_TIME_MS, BME280_MEASUREMENT_TIME_MS,
              powerPin, -1, measurementsToAverage)
{
    _i2cAddressHex  = i2cAddressHex;
}


String BoschBME280::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool BoschBME280::setup(void)
{
    bool retVal = Sensor::setup();  // this will set timestamp and status bit

    // Run begin fxn because it returns true or false for success in contact
    // Make 5 attempts
    uint8_t ntries = 0;
    bool success = false;
    while (!success and ntries < 5)
    {
        success = bme_internal.begin(_i2cAddressHex);
        ntries++;
    }
    if (!success)
    {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
    }
    retVal &= success;


    return retVal;
}


bool BoschBME280::wake(void)
{
    Sensor::wake();  // this will set timestamp and status bit
    waitForWarmUp();  // sensor must be warmed up before we can send begin command
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
    bme_internal.begin(_i2cAddressHex);

    // When the Adafruit library is updated to remove the built-in delay after
    // forcing a sample, it would be better to operate in forced mode.
    bme_internal.setSampling(Adafruit_BME280::MODE_NORMAL,  // sensor mode
    // bme_internal.setSampling(Adafruit_BME280::MODE_FORCED,  // sensor mode
                             Adafruit_BME280::SAMPLING_X16,  // temperature oversampling
                             Adafruit_BME280::SAMPLING_X16,  //  pressure oversampling
                             Adafruit_BME280::SAMPLING_X16,  //  humidity oversampling
                             Adafruit_BME280::FILTER_OFF, // built-in IIR filter
                             Adafruit_BME280::STANDBY_MS_1000);  // sleep time between measurements (N/A in forced mode)
    delay(100);  // Need this delay after changing sampling mode

    return true;
}

// For operating in forced mode
// bool BoschBME280::startSingleMeasurement(void)
// {
//     // waitForWarmUp();  // already done in wake
//     waitForStability();
//     bme_internal.takeForcedMeasurement(false);  // Don't want to wait to finish here
//     _millisMeasurementRequested = millis();
//     return true;
// }


bool BoschBME280::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForMeasurementCompletion();

    // Read values
    float temp = bme_internal.readTemperature();
    float press = bme_internal.readPressure();
    float alt = bme_internal.readAltitude(SEALEVELPRESSURE_HPA);
    float humid = bme_internal.readHumidity();

    if (isnan(temp)) temp = -9999;
    if (isnan(press)) press = -9999;
    if (isnan(alt)) alt = -9999;
    if (isnan(humid)) humid = -9999;

    if (temp == -140.85)  // This is the value returned if it's not attached
    {
        temp = press = alt = humid = -9999;
    }

    MS_DBG(F("Temperature: "), temp);
    MS_DBG(F(" Humidity: "), humid);
    MS_DBG(F(" Barometric Pressure: "), press);
    MS_DBG(F(" Calculated Altitude: "), alt, F("\n"));

    verifyAndAddMeasurementResult(BME280_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(BME280_HUMIDITY_VAR_NUM, humid);
    verifyAndAddMeasurementResult(BME280_PRESSURE_VAR_NUM, press);
    verifyAndAddMeasurementResult(BME280_ALTITUDE_VAR_NUM, alt);

    // Unset the time stamp for the beginning of this measurements
    _millisMeasurementRequested = 0;

    // Return true when finished
    return true;
}
