/*
 *Boschbme_internal.cpp
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
BoschBME280::BoschBME280(int powerPin, uint8_t i2cAddressHex, int readingsToAverage)
     : Sensor(F("BoschBME280"), BME280_NUM_VARIABLES,
              BME280_WARM_UP, BME280_STABILITY, BME280_RESAMPLE,
              powerPin, -1, readingsToAverage)
{
    _i2cAddressHex  = i2cAddressHex;
}


String BoschBME280::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


SENSOR_STATUS BoschBME280::getStatus(void)
{
    // Check if the power is on, turn it on if not (Need power to get status)
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Run begin fxn because it returns true or false for success in contact
    delay(10); // let the sensor settle in after power-up
    bool status = bme_internal.begin(_i2cAddressHex);
    delay(100); // And now let the sensor boot up (time cannot be decreased)

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    if (!status) return SENSOR_ERROR;
    else return SENSOR_READY;
}


SENSOR_STATUS BoschBME280::setup(void)
{
    Sensor::setup();
    return getStatus();
}


bool BoschBME280::wake(void)
{
    Sensor::wake();
    waitForWarmUp();
    bme_internal.begin(0x76);  // Restart needed after power-up
    delay(100); // And now let the sensor boot up (time cannot be decreased)
    return true;
}


bool BoschBME280::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForNextMeasurement();

    // Read values
    float temp = bme_internal.readTemperature();
    float press = bme_internal.readPressure();
    float alt = bme_internal.readAltitude(SEALEVELPRESSURE_HPA);
    float humid = bme_internal.readHumidity();

    sensorValues[BME280_TEMP_VAR_NUM] += temp;
    sensorValues[BME280_HUMIDITY_VAR_NUM] += humid;
    sensorValues[BME280_PRESSURE_VAR_NUM] += press;
    sensorValues[BME280_ALTITUDE_VAR_NUM] += alt;

    MS_DBG(F("Temperature: "), temp);
    MS_DBG(F(" Humidity: "), humid);
    MS_DBG(F(" Barometric Pressure: "), press);
    MS_DBG(F(" Calculated Altitude: "), alt, F("\n"));

    // Return true when finished
    return true;
}
