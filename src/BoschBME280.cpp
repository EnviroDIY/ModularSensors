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
*/

#include "BoschBME280.h"

// The constructor - because this is I2C, only need the power pin
BoschBME280::BoschBME280(int powerPin, uint8_t i2c_addr)
 : Sensor(-1, powerPin, F("BoschBME280"), BoschBME280_NUM_MEASUREMENTS)
{
    _i2c_addr  = i2c_addr;
}

String BoschBME280::getSensorLocation(void)
{
    String address = F("I2C_");
    address += String(_i2c_addr, HEX);
    return address;
}

SENSOR_STATUS BoschBME280::setup(void)
{
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    bool status = bme280.begin(_i2c_addr);  // Start the BME280
    if (!status) return SENSOR_ERROR;
    else return SENSOR_READY;
}

SENSOR_STATUS BoschBME280::getStatus(void)
{
    bool status = bme280.begin(_i2c_addr);  // Start the BME280
    if (!status) return SENSOR_ERROR;
    else return SENSOR_READY;
}

bool BoschBME280::update(void)
{
    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}  // powerUp function includes a 500ms delay

    // Clear values before starting loop
    clearValues();

    // Read temperature
    sensorValues[BoschBME280_TEMP_VAR_NUM] = bme280.readTemperature();
    // Read temperature
    sensorValues[BoschBME280_HUMIDITY_VAR_NUM] = bme280.readHumidity();
    // read pressure
    sensorValues[BoschBME280_PRESSURE_VAR_NUM] = bme280.readPressure();
    // Read the altitude
    sensorValues[BoschBME280_ALTITUDE_VAR_NUM] = bme280.readAltitude(SEALEVELPRESSURE_HPA);

    DBGM(F("Temperature: "), sensorValues[BoschBME280_TEMP_VAR_NUM]);
    DBGM(F(" Humidity: "), sensorValues[BoschBME280_HUMIDITY_VAR_NUM]);
    DBGM(F(" Barometric Pressure: "), sensorValues[BoschBME280_PRESSURE_VAR_NUM]);
    DBGM(F(" Calculated Altitude: "), sensorValues[BoschBME280_ALTITUDE_VAR_NUM], F("\n"));

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
