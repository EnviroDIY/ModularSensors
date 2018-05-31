/*
 *FreescaleMPL115A2.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Bobby Schulz <schu3119@umn.edu>.
 *
 *This file is for the Freescale Semiconductor MPL115A2 Miniature I2C Digital
 *Barometer
 *It is dependent on the https://github.com/adafruit/Adafruit_MPL115A2 library
 *
 *Documentation for the sensor can be found at:
 *https://www.adafruit.com/product/992
 *https://github.com/adafruit/Adafruit-MPL115A2-Breakout-PCB
 *
 * For Pressure:
 *  Resolution is 1.5 hPa
 *  Accuracy ±10 hPa
 *  Range is 500-1150 hPa
 *
 * Sensor takes about 1.6 ms to respond
 * Assume sensor is immediately stable
*/

#ifndef FreescaleMPL115A2_h
#define FreescaleMPL115A2_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"


#include "SensorBase.h"
#include "VariableBase.h"
#include <Adafruit_MPL115A2.h>

#define MPL115A2_NUM_VARIABLES 2
#define MPL115A2_WARM_UP_TIME_MS 6
#define MPL115A2_STABILIZATION_TIME_MS 0
#define MPL115A2_MEASUREMENT_TIME_MS 3

#define MPL115A2_TEMP_RESOLUTION 2
#define MPL115A2_TEMP_VAR_NUM 0

#define MPL115A2_PRESSURE_RESOLUTION 2
#define MPL115A2_PRESSURE_VAR_NUM 1


// The main class for the MPL115A2
class MPL115A2 : public Sensor
{
public:
    MPL115A2(int8_t powerPin, uint8_t measurementsToAverage = 1);

    bool setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;
protected:
    Adafruit_MPL115A2 mpl115a2_internal;
    uint8_t _i2cAddressHex;
};


// Defines the Temperature Variable
class MPL115A2_Temp : public Variable
{
public:
    MPL115A2_Temp(Sensor *parentSense,
                     const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, MPL115A2_TEMP_VAR_NUM,
               "temperature", "degreeCelsius",
               MPL115A2_TEMP_RESOLUTION,
               "MPL115A2_Temp", UUID, customVarCode)
    {}
};


// Defines the Pressure Variable
class MPL115A2_Pressure : public Variable
{
public:
    MPL115A2_Pressure(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, MPL115A2_PRESSURE_VAR_NUM,
               "atmosphericPressure", "kilopascal",
               MPL115A2_PRESSURE_RESOLUTION,
               "MPL115A2_Pressure", UUID, customVarCode)
    {}
};


#endif
