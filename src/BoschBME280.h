/*
 *BoschBME280.h
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

#ifndef BoschBME280_h
#define BoschBME280_h

#include "SensorBase.h"
#include <Adafruit_BME280.h>

#define BoschBME280_NUM_MEASUREMENTS 4
#define BoschBME280_TEMP_RESOLUTION 2
#define BoschBME280_HUMIDITY_RESOLUTION 3
#define BoschBME280_PRESSURE_RESOLUTION 2
#define BoschBME280_ALTITUDE_RESOLUTION 0
#define SEALEVELPRESSURE_HPA (1013.25)

// The main class for the AOSong BoschBME280
class BoschBME280 : public virtual SensorBase
{
public:
    BoschBME280(int powerPin, uint8_t i2c_addr = 0x76);

    SENSOR_STATUS setup(void) override;
    SENSOR_STATUS getStatus(void) override;
    String getSensorLocation(void) override;
protected:
    Adafruit_BME280 bme280;
    unsigned long sensorLastUpdated;
    uint8_t _i2c_addr;
};


// Defines the "Temperature Sensor"
class BoschBME280_Temp : public virtual BoschBME280
{
public:
    BoschBME280_Temp(int powerPin, uint8_t i2c_addr = 0x76);

    bool update(void) override;

    float getValue(void) override;

private:
    float sensorValue_temp;
};


// Defines the "Humidity Sensor"
class BoschBME280_Humidity : public virtual BoschBME280
{
public:
    BoschBME280_Humidity(int powerPin, uint8_t i2c_addr = 0x76);

    bool update(void) override;

    float getValue(void) override;

private:
    float sensorValue_humidity;
};


// Defines the "Pressure Sensor"
class BoschBME280_Pressure : public virtual BoschBME280
{
public:
    BoschBME280_Pressure(int powerPin, uint8_t i2c_addr = 0x76);

    bool update(void) override;

    float getValue(void) override;

private:
    float sensorValue_pressure;
};


// Defines the "Altitude Sensor"
class BoschBME280_Altitude : public virtual BoschBME280
{
public:
    BoschBME280_Altitude(int powerPin, uint8_t i2c_addr = 0x76);

    bool update(void) override;

    float getValue(void) override;

private:
    float sensorValue_altitude;
};


#endif
