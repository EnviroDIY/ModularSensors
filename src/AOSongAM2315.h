/*
 *AOSongAM2315.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the AOSong AM2315 Capacitive Humidity and Temperature sensor
 *It is dependent on the Adafruit AM2315 Library
 *
 *Documentation for the sensor can be found at: www.aosong.com/asp_bin/Products/en/AM2315.pdf
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH (16 bit)
 *  Accuracy is ± 2 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C (16 bit)
 *  Accuracy is ±0.1°C
 *  Range is -40°C to +125°C
*/

#ifndef AOSongAM2315_h
#define AOSongAM2315_h

#include "DecagonSDI12.h"

#define AM2315_NUM_MEASUREMENTS 2
#define AM2315_HUMIDITY_RESOLUTION 1
#define AM2315_TEMP_RESOLUTION 1

// The main class for the AOSong AM2315
class AOSongAM2315 : public virtual SensorBase
{
public:
    AOSongAM2315(int powerPin);

    bool update(void) override;

    virtual float getValue(void) = 0;
protected:
    static unsigned long sensorLastUpdated;
    static float sensorValue_humidity;
    static float sensorValue_temp;
};


// Defines the "Humidity Sensor"
class AOSongAM2315_Humidity : public virtual AOSongAM2315
{
public:
    AOSongAM2315_Humidity(int powerPin);

    float getValue(void) override;
};


// Defines the "Temperature Sensor"
class AOSongAM2315_Temp : public virtual AOSongAM2315
{
public:
    AOSongAM2315_Temp(int powerPin);

    float getValue(void) override;
};

#endif
