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

#include "SensorBase.h"
#include "VariableBase.h"

// #define MODULES_DBG Serial
#include "ModSensorDebugger.h"

#define AM2315_NUM_MEASUREMENTS 2

#define AM2315_HUMIDITY_RESOLUTION 1
#define AM2315_HUMIDITY_VAR_NUM 0

#define AM2315_TEMP_RESOLUTION 1
#define AM2315_TEMP_VAR_NUM 1


// The main class for the AOSong AM2315
class AOSongAM2315 : public Sensor
{
public:
    // The constructor - because this is I2C, only need the power pin
    AOSongAM2315(int powerPin);

    String getSensorLocation(void) override;

    bool update(void) override;
};


// Defines the Humidity Variable
class AOSongAM2315_Humidity : public Variable
{
public:
    AOSongAM2315_Humidity(Sensor *parentSense) :
      Variable(parentSense, AM2315_HUMIDITY_VAR_NUM,
               F("relativeHumidity"), F("percent"),
               AM2315_HUMIDITY_RESOLUTION, F("AM2315Humidity"))
    {}
};


// Defines the Temperature Variable
class AOSongAM2315_Temp : public Variable
{
public:
    AOSongAM2315_Temp(Sensor *parentSense) :
      Variable(parentSense, AM2315_TEMP_VAR_NUM,
               F("temperature"), F("degreeCelsius"),
               AM2315_TEMP_RESOLUTION, F("AM2315Temp"))
    {}
};

#endif
