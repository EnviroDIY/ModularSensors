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
 *https://github.com/EnviroDIY/TippingBucketRainCounter
 *
 * For Rainfall:
 *  Accuracy and resolution are dependent on the sensor used
 *  Standard resolution is 0.01" or 0.2mm of rainfall (depending on if sensor is set to english or metric)
 *
 * Assume sensor is immediately stable
*/

// Header Guards
#ifndef RainCounterI2C_h
#define RainCounterI2C_h

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
// NOTE:  This is a template class!  In order to support either software or
// hardware instances of "Wire" (I2C) we must use a template.
template<typename THEWIRE>
class RainCounterI2C : public Sensor
{
public:
    // The constructor - all arguments are optional
    // Address of I2C device is 0x08 by default
    // Depth of rain per tip event in mm is 0.2mm by default
    RainCounterI2C(THEWIRE theI2C, uint8_t i2cAddressHex = 0x08,
                   float rainPerTip = 0.2)
      : Sensor("RainCounterI2C", BUCKET_NUM_VARIABLES,
               BUCKET_WARM_UP_TIME_MS, BUCKET_STABILIZATION_TIME_MS, BUCKET_MEASUREMENT_TIME_MS,
               -1, -1, 1)
    {
        _i2cAddressHex = i2cAddressHex;
        _i2c = theI2C;
        _rainPerTip = rainPerTip;
    }
    RainCounterI2C(uint8_t i2cAddressHex = 0x08, float rainPerTip = 0.2)
      : Sensor("RainCounterI2C", BUCKET_NUM_VARIABLES,
               BUCKET_WARM_UP_TIME_MS, BUCKET_STABILIZATION_TIME_MS, BUCKET_MEASUREMENT_TIME_MS,
               -1, -1, 1)
    {
        _i2cAddressHex = i2cAddressHex;
        _i2c = Wire;
        _rainPerTip = rainPerTip;
    }
    // Destructor
    ~RainCounterI2C(){}

    String getSensorLocation(void) override
    {
        String address = F("I2C_0x");
        address += String(_i2cAddressHex, HEX);
        return address;
    }

    bool setup(void) override
    {
        _i2c.begin();  // Start the wire library (sensor power not required)
        // Eliminate any potential extra waits in the wire library
        // These waits would be caused by a readBytes or parseX being called
        // on wire after the Wire buffer has emptied.  The default stream
        // functions - used by wire - wait a timeout period after reading the
        // end of the buffer to see if an interrupt puts something into the
        // buffer.  In the case of the Wire library, that will never happen and
        // the timeout period is a useless delay.
        _i2c.setTimeout(0);
        return Sensor::setup();  // this will set pin modes and the setup status bit
    }

    bool addSingleMeasurementResult(void) override
    {
        //intialize values
        uint8_t Byte1 = 0;  // Low byte of data
        uint8_t Byte2 = 0;  // High byte of data

        float rain = -9999;  // Number of mm of rain
        int16_t tips = -9999;  // Number of tip events

        // Get data from external tip counter
        // if the 'requestFrom' returns 0, it means no bytes were received
        if (_i2c.requestFrom(int(_i2cAddressHex), 2))
        {
            MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

            Byte1 = _i2c.read();
            Byte2 = _i2c.read();

            tips = (Byte2 << 8) | (Byte1);  // Concatenate tip values
            rain = float(tips) * _rainPerTip;  // Multiply by tip coefficient (0.2 by default)

            if (tips < 0) tips = -9999;  // If negetive value results, return failure
            if (rain < 0) rain = -9999;  // If negetive value results, return failure

            MS_DBG(F("  Rain:"), rain);
            MS_DBG(F("  Tips:"), tips);
        }
        else MS_DBG(F("No bytes received from"), getSensorNameAndLocation());

        verifyAndAddMeasurementResult(BUCKET_RAIN_VAR_NUM, rain);
        verifyAndAddMeasurementResult(BUCKET_TIPS_VAR_NUM, tips);

        // Unset the time stamp for the beginning of this measurement
        _millisMeasurementRequested = 0;
        // Unset the status bits for a measurement request (bits 5 & 6)
        _sensorStatus &= 0b10011111;

        // Return true when finished
        return true;
    }


protected:
    float _rainPerTip;
    uint8_t _i2cAddressHex;  // Hardware slave address
    THEWIRE _i2c;  // Wire instance - hardware or software
};

// Defines the tip varible, shows the number of tips since last read
class RainCounterI2C_Tips : public Variable
{
public:
    RainCounterI2C_Tips(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "RainCounterI2CTips")
      : Variable(parentSense,
                 (const uint8_t)BUCKET_TIPS_VAR_NUM,
                 (uint8_t)BUCKET_TIPS_RESOLUTION,
                 "precipitation", "event",
                 varCode, uuid)
    {}
    RainCounterI2C_Tips()
      : Variable((const uint8_t)BUCKET_TIPS_VAR_NUM,
                 (uint8_t)BUCKET_TIPS_RESOLUTION,
                 "precipitation", "event", "RainCounterI2CTips")
    {}
    ~RainCounterI2C_Tips(){}
};

// Defines the depth of rain variable, shows the number of mm since the last read
class RainCounterI2C_Depth : public Variable
{
public:
    RainCounterI2C_Depth(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "RainCounterI2CVol")
      : Variable(parentSense,
                 (const uint8_t)BUCKET_RAIN_VAR_NUM,
                 (uint8_t)BUCKET_RAIN_RESOLUTION,
                 "precipitation", "millimeter",
                 varCode, uuid)
    {}
    RainCounterI2C_Depth()
      : Variable((const uint8_t)BUCKET_RAIN_VAR_NUM,
                 (uint8_t)BUCKET_RAIN_RESOLUTION,
                 "precipitation", "millimeter", "RainCounterI2CVol")
    {}
    ~RainCounterI2C_Depth(){}
};


#endif  // Header Guard
