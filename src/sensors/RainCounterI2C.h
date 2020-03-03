/*
 *RainCounterI2C.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for an external tip counter, used to measure rainfall via a
 *tipping bucket rain gauge
 *
 *Documentation for the sensor can be found at:
 *https://github.com/EnviroDIY/TippingBucketRainCounter
 *
 * For Rainfall:
 *  Accuracy and resolution are dependent on the sensor used
 *  Standard resolution is 0.01" or 0.2mm of rainfall (depending on if sensor is
 *set to english or metric)
 *
 * Assume sensor is immediately stable
 */

// Header Guards
#ifndef SRC_SENSORS_RAINCOUNTERI2C_H_
#define SRC_SENSORS_RAINCOUNTERI2C_H_

// Debugging Statement
// #define MS_RAINCOUNTERI2C_DEBUG

#ifdef MS_RAINCOUNTERI2C_DEBUG
#define MS_DEBUGGING_STD "RainCounterI2C"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// Sensor Specific Defines
#define BUCKET_NUM_VARIABLES 2
#define BUCKET_WARM_UP_TIME_MS 0
#define BUCKET_STABILIZATION_TIME_MS 0
#define BUCKET_MEASUREMENT_TIME_MS 0

#define BUCKET_RAIN_RESOLUTION 2
#define BUCKET_RAIN_VAR_NUM 0

#define BUCKET_TIPS_RESOLUTION 0
#define BUCKET_TIPS_VAR_NUM 1

// The main class for the external tipping bucket counter
class RainCounterI2C : public Sensor {
 public:
    // The constructor - all arguments are optional
    // Address of I2C device is 0x08 by default
    // Depth of rain per tip event in mm is 0.2mm by default
    RainCounterI2C(uint8_t i2cAddressHex = 0x08, float rainPerTip = 0.2);
    // Destructor
    ~RainCounterI2C();

    bool   setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 protected:
    float   _rainPerTip;
    uint8_t _i2cAddressHex;
};

// Defines the tip varible, shows the number of tips since last read
class RainCounterI2C_Tips : public Variable {
 public:
    RainCounterI2C_Tips(Sensor* parentSense, const char* uuid = "",
                        const char* varCode = "RainCounterI2CTips")
        : Variable(parentSense, (const uint8_t)BUCKET_TIPS_VAR_NUM,
                   (uint8_t)BUCKET_TIPS_RESOLUTION, "precipitation", "event",
                   varCode, uuid) {}
    RainCounterI2C_Tips()
        : Variable((const uint8_t)BUCKET_TIPS_VAR_NUM,
                   (uint8_t)BUCKET_TIPS_RESOLUTION, "precipitation", "event",
                   "RainCounterI2CTips") {}
    ~RainCounterI2C_Tips() {}
};

// Defines the depth of rain variable, shows the number of mm since the last
// read
class RainCounterI2C_Depth : public Variable {
 public:
    RainCounterI2C_Depth(Sensor* parentSense, const char* uuid = "",
                         const char* varCode = "RainCounterI2CVol")
        : Variable(parentSense, (const uint8_t)BUCKET_RAIN_VAR_NUM,
                   (uint8_t)BUCKET_RAIN_RESOLUTION, "precipitation",
                   "millimeter", varCode, uuid) {}
    RainCounterI2C_Depth()
        : Variable((const uint8_t)BUCKET_RAIN_VAR_NUM,
                   (uint8_t)BUCKET_RAIN_RESOLUTION, "precipitation",
                   "millimeter", "RainCounterI2CVol") {}
    ~RainCounterI2C_Depth() {}
};


#endif  // SRC_SENSORS_RAINCOUNTERI2C_H_
