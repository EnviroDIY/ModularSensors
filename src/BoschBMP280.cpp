/*
 *BoschBMP280.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Bosch BMP280 Digital Pressure Sensor
 *It is dependent on the Adafruit BMP280 Library
 *
 *Documentation for the sensor can be found at:
 *https://learn.adafruit.com/adafruit-bmp280-barometric-pressure-plus-temperature-sensor-breakout/downloads
 *
 * For Relative Pressure:
 *  Resolution is 0.1 % RH (16 bit)
 *  Accuracy is ± 2 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C (16 bit)
 *  Accuracy is ±0.1°C
 *  Range is -40°C to +125°C
*/

#include "BoschBMP280.h"
#include <Adafruit_BMP280.h>

// The constructor - because this is I2C, only need the power pin
BoschBMP280::BoschBMP280(int powerPin)
 : SensorBase(-1, powerPin)
{}

String BoschBMP280::getSensorLocation(void){return F("I2C_0x77");}




BoschBMP280_Pressure::BoschBMP280_Pressure(int powerPin)
 : SensorBase(-1, powerPin, F("BoschBMP280"), F("barometricPressure"), F("pascal"), BoschBMP280_PRESSURE_RESOLUTION, F("BoschBMP280Pressure")),
   BoschBMP280(powerPin)
{}

bool BoschBMP280_Pressure::update(void)
{
    Adafruit_BMP280 bmp280;  // create a sensor object
    bmp280.begin();  // Start the BMP280
    sensorValue_pressure = bmp280.readPressure();  // read pressure
    sensorLastUpdated = millis();  // Update the time
    return true;
}

float BoschBMP280_Pressure::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_pressure;
}





BoschBMP280_Temp::BoschBMP280_Temp(int powerPin)
 : SensorBase(-1, powerPin, F("BoschBMP280"), F("temperature"), F("degreeCelsius"), BoschBMP280_TEMP_RESOLUTION, F("BoschBMP280Temp")),
   BoschBMP280(powerPin)
{}

bool BoschBMP280_Temp::update(void)
{
    Adafruit_BMP280 bmp280;  // create a sensor object
    bmp280.begin();  // Start the BMP280
    sensorValue_temp = bmp280.readTemperature();  // Read temperature
    sensorLastUpdated = millis();  // Update the time
    return true;
}

float BoschBMP280_Temp::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_temp;
}





BoschBMP280_Altitude::BoschBMP280_Altitude(int powerPin)
 : SensorBase(-1, powerPin, F("BoschBMP280"), F("heightAboveSeaFloor"), F("meter"), BoschBMP280_ALTITUDE_RESOLUTION, F("BoschBMP280Altitude")),
   BoschBMP280(powerPin)
{}

bool BoschBMP280_Altitude::update(void)
{
    Adafruit_BMP280 bmp280;  // create a sensor object
    bmp280.begin();  // Start the BMP280
    sensorValue_altitude = bmp280.readAltitude();  // Read the altitude
    sensorLastUpdated = millis();  // Update the time
    return true;
}

float BoschBMP280_Altitude::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue_altitude;
}
