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
 *
 * Warm up/stability: unknown
 * Measurement time: 2sec
*/

#ifndef AOSongAM2315_h
#define AOSongAM2315_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "../SensorBase.h"
#include "VariableBase.h"

#define AM2315_NUM_VARIABLES 2
#define AM2315_WARM_UP_TIME_MS 500
#define AM2315_STABILIZATION_TIME_MS 500
#define AM2315_MEASUREMENT_TIME_MS 2000

#define AM2315_HUMIDITY_RESOLUTION 1
#define AM2315_HUMIDITY_VAR_NUM 0

#define AM2315_TEMP_RESOLUTION 1
#define AM2315_TEMP_VAR_NUM 1


// The main class for the AOSong AM2315
class AOSongAM2315 : public Sensor
{
public:
    // The constructor - because this is I2C, only need the power pin
    AOSongAM2315(int8_t powerPin, uint8_t measurementsToAverage = 1);
    ~AOSongAM2315(){};

    String getSensorLocation(void) override;

    bool setup(void) override;

    bool addSingleMeasurementResult(void) override;
};


// Defines the Humidity Variable
class AOSongAM2315_Humidity : public Variable
{
public:
    AOSongAM2315_Humidity(Sensor *parentSense,
                          const char *UUID = "", const char *customVarCode = "") :
      Variable(parentSense, AM2315_HUMIDITY_VAR_NUM,
               "relativeHumidity", "percent",
               AM2315_HUMIDITY_RESOLUTION,
               "AM2315Humidity", UUID, customVarCode)
    {}
    ~AOSongAM2315_Humidity(){};
};


// Defines the Temperature Variable
class AOSongAM2315_Temp : public Variable
{
public:
    AOSongAM2315_Temp(Sensor *parentSense,
                      const char *UUID = "", const char *customVarCode = "") :
      Variable(parentSense, AM2315_TEMP_VAR_NUM,
               "temperature", "degreeCelsius",
               AM2315_TEMP_RESOLUTION,
               "AM2315Temp", UUID, customVarCode)
    {}
    ~AOSongAM2315_Temp(){};
};

#endif
