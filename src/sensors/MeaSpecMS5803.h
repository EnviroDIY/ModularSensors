/*
 *MeaSpecMS5803.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>.
 * with help from Beth Fisher, Evan Host and Bobby Schulz
 *
 *This file is for the Measurement Specialties MS5803 pressure sensor,
  for which is used in SparkFun Pressure Sensor Breakout - MS5803-14BA.
 *It is dependent on the https://github.com/EnviroDIY/MS5803 library, which was
  modified for EnviroDIY_Modular sensors based on a fork
  from the https://github.com/NorthernWidget/MS5803 library, which expanded on
 *https://github.com/sparkfun/SparkFun_MS5803-14BA_Breakout_Arduino_Library
 *
 *Documentation for the sensor can be found at:
 *https://www.sparkfun.com/products/12909
 *https://cdn.sparkfun.com/datasheets/Sensors/Weather/ms5803_14ba.pdf
 *
 * For Pressure (sensor designed for water pressure):
 *  Resolution is:
 *      14ba: 1 / 0.6 / 0.4 / 0.3 / 0.2 mbar (where 1 mbar = 100 pascals)
 *      2ba: 0.13 / 0.084 / 0.054 / 0.036 / 0.024
 *      1ba: 0.065 / 0.042 / 0.027 / 0.018 / 0.012
 *    at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
 *  Accuracy 0 to +40°C is:
 *      14ba: ±20mbar
 *      2ba: ±1.5mbar
 *      1ba:  ±1.5mbar
 *  Range is 0 to 14 bar
 *  Long term stability is
 *      14ba: -20 mbar/yr
 *      2ba: -1 mbar/yr
 *
 * For Temperature:
 *  Resolution is <0.01°C
 *  Accuracy is ±0.8°C
 *  Range is -40°C to +85°C
 *
 * Sensor takes about 0.5 / 1.1 / 2.1 / 4.1 / 8.22 ms to respond
 *  at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
 * Assume sensor is immediately stable
*/

// Header Guards
#ifndef MeaSpecMS5803_h
#define MeaSpecMS5803_h

// Debugging Statement
// #define MS_MEASSPECMS5803_DEBUG

#ifdef MS_MEASSPECMS5803_DEBUG
#define MS_DEBUGGING_STD
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <MS5803.h>

// Sensor Specific Defines
#define MS5803_NUM_VARIABLES 2
#define MS5803_WARM_UP_TIME_MS 10
#define MS5803_STABILIZATION_TIME_MS 0
#define MS5803_MEASUREMENT_TIME_MS 10

#define MS5803_TEMP_RESOLUTION 2
#define MS5803_TEMP_VAR_NUM 0

#define MS5803_PRESSURE_RESOLUTION 3
#define MS5803_PRESSURE_VAR_NUM 1


// The main class for the Measurement Specialties MS5803
class MeaSpecMS5803 : public Sensor
{
public:
    MeaSpecMS5803(int8_t powerPin, uint8_t i2cAddressHex = 0x76,
                  int16_t maxPressure = 14, uint8_t measurementsToAverage = 1);
    ~MeaSpecMS5803();

    bool setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;
protected:
    MS5803 MS5803_internal;
    uint8_t _i2cAddressHex;
    int16_t _maxPressure;
};


// Defines the Temperature Variable
class MeaSpecMS5803_Temp : public Variable
{
public:
    MeaSpecMS5803_Temp(Sensor *parentSense,
                       const char *uuid = "",
                       const char *varCode = "MeaSpecMS5803Temp")
      : Variable(parentSense,
                 (const uint8_t)MS5803_TEMP_VAR_NUM,
                 (uint8_t)MS5803_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    MeaSpecMS5803_Temp()
      : Variable((const uint8_t)MS5803_TEMP_VAR_NUM,
                 (uint8_t)MS5803_TEMP_RESOLUTION,
                 "temperature", "degreeCelsius", "MeaSpecMS5803Temp")
    {}
    ~MeaSpecMS5803_Temp(){}
};


// Defines the Pressure Variable
class MeaSpecMS5803_Pressure : public Variable
{
public:
    MeaSpecMS5803_Pressure(Sensor *parentSense,
                           const char *uuid = "",
                           const char *varCode = "MeaSpecMS5803Pressure")
      : Variable(parentSense,
                 (const uint8_t)MS5803_PRESSURE_VAR_NUM,
                 (uint8_t)MS5803_PRESSURE_RESOLUTION,
                 "barometricPressure", "millibar",
                 varCode, uuid)
    {}
    MeaSpecMS5803_Pressure()
      : Variable((const uint8_t)MS5803_PRESSURE_VAR_NUM,
                 (uint8_t)MS5803_PRESSURE_RESOLUTION,
                 "barometricPressure", "millibar", "MeaSpecMS5803Pressure")
    {}
    ~MeaSpecMS5803_Pressure(){}
};


#endif  // Header Guard
