/*
 *TallyCounterI2C.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *Copyright 2020 Stroud Water Research Center (per LICENSE.md)
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for NorthernWidget's Tally external event counter
 *It is used to measure windspeed or rainfall, and dependent on Tally_Library
 *
 *Documentation for the sensor can be found at:
 * https://github.com/NorthernWidget-Skunkworks/Project-Tally​
 * https://github.com/NorthernWidget-Skunkworks/Tally_Library/tree/Dev_I2C
 *
 * For Wind Speed:
 *  Accuracy and resolution are dependent on the sensor used:
 *  Inspeed WS2R Version II Reed Switch Anemometer
 *  https://www.store.inspeed.com/Inspeed-Version-II-Reed-Switch-Anemometer-Sensor-Only-WS2R.htm
 *
 * Assume sensor is immediately stable
*/

// Header Guards
#ifndef SRC_SENSORS_TallyCounterI2C_H_
#define SRC_SENSORS_TallyCounterI2C_H_

// Debugging Statement
// #define MS_TALLYCOUNTERI2C_DEBUG

#ifdef MS_TALLYCOUNTERI2C_DEBUG
#define MS_DEBUGGING_STD "TallyCounterI2C"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Tally_I2C.h>

// Sensor Specific Defines
#define TALLY_NUM_VARIABLES 1
#define TALLY_WARM_UP_TIME_MS 10
#define TALLY_STABILIZATION_TIME_MS 10
#define TALLY_MEASUREMENT_TIME_MS 10

#define TALLY_EVENTS_RESOLUTION 0
#define TALLY_EVENTS_VAR_NUM 0




// The main class for the NorthernWidgetTally external event counter
class TallyCounterI2C : public Sensor {
public:
    explicit TallyCounterI2C(int8_t powerPin, uint8_t i2cAddressHex = 0x33);
    ~TallyCounterI2C();
    // Address of I2C device is 0x33 by default
    // powerPin is -1 by default because often deployed with power always on,
    //   but Tally also has a super capacitor that allows powering down.

    bool   wake(void) override;
    bool   setup(void) override;
    String getSensorLocation(void) override;

    bool   addSingleMeasurementResult(void) override;
protected:
    Tally_I2C counter_internal;
    uint8_t   _i2cAddressHex;
};

// Defines the Event varible, shows the number of Events since last read
class TallyCounterI2C_Events : public Variable {
public:
    explicit TallyCounterI2C_Events(TallyCounterI2C* parentSense,  const char* uuid = "",
                                    const char* varCode = "TallyCounterI2CEvents")
        : Variable(parentSense, (const uint8_t)TALLY_EVENTS_VAR_NUM,
                  (uint8_t)TALLY_EVENTS_RESOLUTION,
                  "counter", "event", varCode, uuid) {}
    TallyCounterI2C_Events()
      : Variable((const uint8_t)TALLY_EVENTS_VAR_NUM,
                 (uint8_t)TALLY_EVENTS_RESOLUTION, "counter",
                 "event", "TallyCounterI2CEvents") {}
    ~TallyCounterI2C_Events(){}
};

#endif // SRC_SENSORS_TallyCounterI2C_H_
