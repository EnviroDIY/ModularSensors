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
 *
 * Sensor takes about 100ms to respond
 * Slowest response time (humidity): 1sec
 * Assume sensor is immediately stable
*/

// Header Guards
#ifndef BoschBME280_h
#define BoschBME280_h

// Debugging Statement
// #define MS_BOSCHBME280_DEBUG

#ifdef MS_BOSCHBME280_DEBUG
    #define MS_DEBUGGING_STD "BoschBME280"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_BME280.h>

// Sensor Specific Defines
#define BME280_NUM_VARIABLES 4
#define BME280_WARM_UP_TIME_MS 100
#define BME280_STABILIZATION_TIME_MS 4000  // 0.5 s for good numbers, but optimal at 4 s based on tests using bme280timingTest.ino
#define BME280_MEASUREMENT_TIME_MS 1100    // 1.0 s according to datasheet, but slightly better stdev when 1.1 s
// For details on BME280 stabilization time updates, include testing sketch and link to data in Google Sheet,
//  see https://github.com/EnviroDIY/ModularSensors/commit/27e3cb531162ed6971a41f3c38f5920d356089e9

#define BME280_TEMP_RESOLUTION 2
#define BME280_TEMP_VAR_NUM 0

#define BME280_HUMIDITY_RESOLUTION 3
#define BME280_HUMIDITY_VAR_NUM 1

#define BME280_PRESSURE_RESOLUTION 2
#define BME280_PRESSURE_VAR_NUM 2

#define BME280_ALTITUDE_RESOLUTION 0
#define BME280_ALTITUDE_VAR_NUM 3
#define SEALEVELPRESSURE_HPA (1013.25)

// The main class for the Bosch BME280
class BoschBME280 : public Sensor
{
public:
    BoschBME280(int8_t powerPin, uint8_t i2cAddressHex = 0x76, uint8_t measurementsToAverage = 1);
    ~BoschBME280();

    bool wake(void) override;
    bool setup(void) override;
    String getSensorLocation(void) override;

    // bool startSingleMeasurement(void) override;  // for forced mode
    bool addSingleMeasurementResult(void) override;

protected:
    Adafruit_BME280 bme_internal;
    uint8_t _i2cAddressHex;
};


// Defines the Temperature Variable
class BoschBME280_Temp : public Variable
{
public:
    BoschBME280_Temp(Sensor *parentSense,
                     const char *uuid = "",
                     const char *varCode = "BoschBME280Temp")
        : Variable(parentSense,
                   (const uint8_t)BME280_TEMP_VAR_NUM,
                   (uint8_t)BME280_TEMP_RESOLUTION,
                   "temperature", "degreeCelsius",
                   varCode, uuid)
    {}
    BoschBME280_Temp()
        : Variable((const uint8_t)BME280_TEMP_VAR_NUM,
                   (uint8_t)BME280_TEMP_RESOLUTION,
                   "temperature", "degreeCelsius", "BoschBME280Temp")
    {}
    ~BoschBME280_Temp(){};
};


// Defines the Humidity Variable
class BoschBME280_Humidity : public Variable
{
public:
    BoschBME280_Humidity(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "BoschBME280Humidity")
        : Variable(parentSense,
                   (const uint8_t)BME280_HUMIDITY_VAR_NUM,
                   (uint8_t)BME280_HUMIDITY_RESOLUTION,
                   "relativeHumidity", "percent",
                   varCode, uuid)
    {}
    BoschBME280_Humidity()
        : Variable((const uint8_t)BME280_HUMIDITY_VAR_NUM,
                   (uint8_t)BME280_HUMIDITY_RESOLUTION,
                   "relativeHumidity", "percent", "BoschBME280Humidity")
    {}
    ~BoschBME280_Humidity(){};
};


// Defines the Pressure Variable
class BoschBME280_Pressure : public Variable
{
public:
    BoschBME280_Pressure(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "BoschBME280Pressure")
        : Variable(parentSense,
                   (const uint8_t)BME280_PRESSURE_VAR_NUM,
                   (uint8_t)BME280_PRESSURE_RESOLUTION,
                   "barometricPressure", "pascal",
                   varCode, uuid)
    {}
    BoschBME280_Pressure()
        : Variable((const uint8_t)BME280_PRESSURE_VAR_NUM,
                   (uint8_t)BME280_PRESSURE_RESOLUTION,
                   "barometricPressure", "pascal", "BoschBME280Pressure")
    {}
};


// Defines the Altitude Variable
class BoschBME280_Altitude : public Variable
{
public:
    BoschBME280_Altitude(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "BoschBME280Altitude")
        : Variable(parentSense,
                   (const uint8_t)BME280_ALTITUDE_VAR_NUM,
                   (uint8_t)BME280_ALTITUDE_RESOLUTION,
                   "heightAboveSeaFloor", "meter",
                   varCode, uuid)
    {}
    BoschBME280_Altitude()
        : Variable((const uint8_t)BME280_ALTITUDE_VAR_NUM,
                   (uint8_t)BME280_ALTITUDE_RESOLUTION,
                   "heightAboveSeaFloor", "meter", "BoschBME280Altitude")
    {}
};


#endif  // Header Guard
