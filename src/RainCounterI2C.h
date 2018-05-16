/*
 *RainCounterI2C.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for an external tip counter, used to measure rainfall via a tipping bucket
 *rain gauge
 *
 *Documentation for the sensor can be found at:
 *https://github.com/EnviroDIY/TippingBucketRainGauge
 *
 * For Rainfall:
 *  Accuracy and resolution are dependent on the sensor used
 *  Standard resolution is 0.01" or 0.2mm of rainfall (depending on if sensor is set to english or metric)
 *
 * Assume sensor is immediately stable
*/

#ifndef RainCounterI2C_h
#define RainCounterI2C_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"


#include "SensorBase.h"
#include "VariableBase.h"
#include <Wire.h>

#define BUCKET_NUM_VARIABLES 2
#define BUCKET_WARM_UP_TIME_MS 0
#define BUCKET_STABILIZATION_TIME_MS 0
#define BUCKET_MEASUREMENT_TIME_MS 0

#define BUCKET_RAIN_RESOLUTION 2
#define BUCKET_RAIN_VAR_NUM 0

#define BUCKET_TIPS_RESOLUTION 0
#define BUCKET_TIPS_VAR_NUM 1

// The main class for the external tipping bucket counter
class RainCounterI2C : public Sensor
{
public:
    // The constructor - all arguments are optional
    // Address of I2C device is 0x08 by default
    // Depth of rain per tip event in mm is 0.2mm by default
    RainCounterI2C(uint8_t i2cAddressHex = 0x08, float rainPerTip = 0.2);

    bool setup(void) override;
    bool wake(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;
protected:
    float _rainPerTip;
    uint8_t _i2cAddressHex;
};

// Defines the tip varible, shows the number of tips since last read
class RainCounterI2C_Tips : public Variable
{
public:
    RainCounterI2C_Tips(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, BUCKET_TIPS_VAR_NUM,
               F("precipitation"), F("event"),
               BUCKET_TIPS_RESOLUTION,
               F("RainCounterI2CTips"), UUID, customVarCode)
    {}
};

// Defines the depth of rain variable, shows the number of mm since the last read
class RainCounterI2C_Depth : public Variable
{
public:
    RainCounterI2C_Depth(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, BUCKET_RAIN_VAR_NUM,
               F("precipitation"), F("millimeter"),
               BUCKET_RAIN_RESOLUTION,
               F("RainCounterI2CVol"), UUID, customVarCode)
    {}
};


#endif
