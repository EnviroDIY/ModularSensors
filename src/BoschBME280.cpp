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
 : SensorBase(-1, powerPin)
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



BoschBME280_Temp::BoschBME280_Temp(int powerPin, uint8_t i2c_addr)
 : SensorBase(-1, powerPin, F("BoschBME280"), F("temperature"), F("degreeCelsius"), BoschBME280_TEMP_RESOLUTION, F("BoschBME280Temp")),
   BoschBME280(powerPin, i2c_addr)
{}

bool BoschBME280_Temp::update(void)
{
    sensorValue_temp = bme280.readTemperature();  // Read temperature
    sensorLastUpdated = millis();  // Update the time
    return true;
}

float BoschBME280_Temp::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_temp;
}




BoschBME280_Humidity::BoschBME280_Humidity(int powerPin, uint8_t i2c_addr)
 : SensorBase(-1, powerPin, F("BoschBME280"), F("relativeHumidity"), F("percent"), BoschBME280_HUMIDITY_RESOLUTION, F("BoschBME280Humidity")),
   BoschBME280(powerPin, i2c_addr)
{}

bool BoschBME280_Humidity::update(void)
{
    sensorValue_humidity = bme280.readHumidity();  // Read temperature
    sensorLastUpdated = millis();  // Update the time
    return true;
}

float BoschBME280_Humidity::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_humidity;
}




BoschBME280_Pressure::BoschBME280_Pressure(int powerPin, uint8_t i2c_addr)
 : SensorBase(-1, powerPin, F("BoschBME280"), F("barometricPressure"), F("pascal"), BoschBME280_PRESSURE_RESOLUTION, F("BoschBME280Pressure")),
   BoschBME280(powerPin, i2c_addr)
{}

bool BoschBME280_Pressure::update(void)
{
    sensorValue_pressure = bme280.readPressure();  // read pressure
    sensorLastUpdated = millis();  // Update the time
    return true;
}

float BoschBME280_Pressure::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_pressure;
}





BoschBME280_Altitude::BoschBME280_Altitude(int powerPin, uint8_t i2c_addr)
 : SensorBase(-1, powerPin, F("BoschBME280"), F("heightAboveSeaFloor"), F("meter"), BoschBME280_ALTITUDE_RESOLUTION, F("BoschBME280Altitude")),
   BoschBME280(powerPin, i2c_addr)
{}

bool BoschBME280_Altitude::update(void)
{
    sensorValue_altitude = bme280.readAltitude(SEALEVELPRESSURE_HPA);  // Read the altitude
    sensorLastUpdated = millis();  // Update the time
    return true;
}

float BoschBME280_Altitude::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_altitude;
}
