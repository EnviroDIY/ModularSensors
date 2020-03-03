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

// Header Guards
#ifndef SRC_SENSORS_FREESCALEMPL115A2_H_
#define SRC_SENSORS_FREESCALEMPL115A2_H_

// Debugging Statement
// #define MS_FREESCALEMPL115A2_DEBUG

#ifdef MS_FREESCALEMPL115A2_DEBUG
#define MS_DEBUGGING_STD "FreescaleMPL115A2"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_MPL115A2.h>

// Sensor Specific Defines
#define MPL115A2_NUM_VARIABLES 2
#define MPL115A2_WARM_UP_TIME_MS 6
#define MPL115A2_STABILIZATION_TIME_MS 0
#define MPL115A2_MEASUREMENT_TIME_MS 3

#define MPL115A2_TEMP_RESOLUTION 2
#define MPL115A2_TEMP_VAR_NUM 0

#define MPL115A2_PRESSURE_RESOLUTION 2
#define MPL115A2_PRESSURE_VAR_NUM 1


// The main class for the MPL115A2
class MPL115A2 : public Sensor {
 public:
    explicit MPL115A2(int8_t powerPin, uint8_t measurementsToAverage = 1);
    ~MPL115A2();

    bool   setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 protected:
    Adafruit_MPL115A2 mpl115a2_internal;
    uint8_t           _i2cAddressHex;
};


// Defines the Temperature Variable
class MPL115A2_Temp : public Variable {
 public:
    explicit MPL115A2_Temp(MPL115A2* parentSense, const char* uuid = "",
                           const char* varCode = "MPL115A2_Temp")
        : Variable(parentSense, (const uint8_t)MPL115A2_TEMP_VAR_NUM,
                   (uint8_t)MPL115A2_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    MPL115A2_Temp()
        : Variable((const uint8_t)MPL115A2_TEMP_VAR_NUM,
                   (uint8_t)MPL115A2_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "MPL115A2_Temp") {}
    ~MPL115A2_Temp() {}
};


// Defines the Pressure Variable
class MPL115A2_Pressure : public Variable {
 public:
    explicit MPL115A2_Pressure(MPL115A2* parentSense, const char* uuid = "",
                               const char* varCode = "MPL115A2_Pressure")
        : Variable(parentSense, (const uint8_t)MPL115A2_PRESSURE_VAR_NUM,
                   (uint8_t)MPL115A2_PRESSURE_RESOLUTION, "atmosphericPressure",
                   "kilopascal", varCode, uuid) {}
    MPL115A2_Pressure()
        : Variable((const uint8_t)MPL115A2_PRESSURE_VAR_NUM,
                   (uint8_t)MPL115A2_PRESSURE_RESOLUTION, "atmosphericPressure",
                   "kilopascal", "MPL115A2_Pressure") {}
    ~MPL115A2_Pressure() {}
};


#endif  // SRC_SENSORS_FREESCALEMPL115A2_H_
