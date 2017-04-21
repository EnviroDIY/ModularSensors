/*
 *BoschBMP280.h
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

#ifndef BoschBMP280_h
#define BoschBMP280_h

#include "SensorBase.h"

#define BoschBMP280_NUM_MEASUREMENTS 3
#define BoschBMP280_PRESSURE_RESOLUTION 2
#define BoschBMP280_TEMP_RESOLUTION 2
#define BoschBMP280_ALTITUDE_RESOLUTION 0

// The main class for the AOSong BoschBMP280
class BoschBMP280 : public virtual SensorBase
{
public:
    BoschBMP280(int powerPin);

    String getSensorLocation(void) override;
};


// Defines the "Pressure Sensor"
class BoschBMP280_Pressure : public virtual BoschBMP280
{
public:
    BoschBMP280_Pressure(int powerPin);

    bool update(void) override;


    float getValue(void) override;

private:
    unsigned long sensorLastUpdated;
    float sensorValue_pressure;
};


// Defines the "Temperature Sensor"
class BoschBMP280_Temp : public virtual BoschBMP280
{
public:
    BoschBMP280_Temp(int powerPin);

    bool update(void) override;

    float getValue(void) override;

private:
    unsigned long sensorLastUpdated;
    float sensorValue_temp;
};


// Defines the "Altitude Sensor"
class BoschBMP280_Altitude : public virtual BoschBMP280
{
public:
    BoschBMP280_Altitude(int powerPin);

    bool update(void) override;

    float getValue(void) override;

private:
    unsigned long sensorLastUpdated;
    float sensorValue_altitude;
};


#endif
