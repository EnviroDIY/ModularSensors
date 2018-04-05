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

#ifndef TippingBucket_h
#define TippingBucket_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"


#include "SensorBase.h"
#include "VariableBase.h"
#include <Wire.h>

#define BUKCET_NUM_VARIABLES 1
#define BUCKET_WARM_UP_TIME_MS 0
#define BUCKET_STABILIZATION_TIME_MS 0
#define BUCKET_MEASUREMENT_TIME_MS 0

#define BUCKET_RAIN_RESOLUTION 2  //Are these utilized??
#define BUCKET_RAIN_VAR_NUM 0

// The main class for the Bosch BME280
class TippingBucket : public Sensor
{
public:
    TippingBucket(int8_t powerPin, uint8_t i2cAddressHex = 0x08, uint8_t measurementsToAverage = 1); //Initalze to address 0x08, use only a single mesurment, no averaging 

    bool wake(void) override;
    bool setup(void) override;
    // bool getStatus(void) override;
    String getSensorLocation(void) override;

    // bool startSingleMeasurement(void) override;  // for forced mode
    bool addSingleMeasurementResult(void) override;
protected:
    uint8_t _i2cAddressHex;
};

class TippingBucket_Tips : public Variable
{
public:
    TippingBucket_Tips(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, BUCKET_RAIN_VAR_NUM,
               F("rain"), F("in"),
               BUCKET_RAIN_RESOLUTION,
               F("tipping bucket"), UUID, customVarCode)
    {}
};

// Defines the Temperature Variable
// class TippingBucket_Rain : public Variable
// {
// public:
//     BoschBME280_Temp(Sensor *parentSense,
//                      String UUID = "", String customVarCode = "")
//       : Variable(parentSense, BME280_TEMP_VAR_NUM,
//                F("temperature"), F("degreeCelsius"),
//                BME280_TEMP_RESOLUTION,
//                F("BoschBME280Temp"), UUID, customVarCode)
//     {}
// };


// // Defines the Humidity Variable
// class BoschBME280_Humidity : public Variable
// {
// public:
//     BoschBME280_Humidity(Sensor *parentSense,
//                          String UUID = "", String customVarCode = "")
//       : Variable(parentSense, BME280_HUMIDITY_VAR_NUM,
//                F("relativeHumidity"), F("percent"),
//                BME280_HUMIDITY_RESOLUTION,
//                F("BoschBME280Humidity"), UUID, customVarCode)
//     {}
// };


// // Defines the Pressure Variable
// class BoschBME280_Pressure : public Variable
// {
// public:
//     BoschBME280_Pressure(Sensor *parentSense,
//                          String UUID = "", String customVarCode = "")
//       : Variable(parentSense, BME280_PRESSURE_VAR_NUM,
//                F("barometricPressure"), F("pascal"),
//                BME280_PRESSURE_RESOLUTION,
//                F("BoschBME280Pressure"), UUID, customVarCode)
//     {}
// };


// // Defines the Altitude Variable
// class BoschBME280_Altitude : public Variable
// {
// public:
//     BoschBME280_Altitude(Sensor *parentSense,
//                          String UUID = "", String customVarCode = "")
//       : Variable(parentSense, BME280_ALTITUDE_VAR_NUM,
//                F("heightAboveSeaFloor"), F("meter"),
//                BME280_ALTITUDE_RESOLUTION,
//                F("BoschBME280Altitude"), UUID, customVarCode)
//     {}
// };


#endif
