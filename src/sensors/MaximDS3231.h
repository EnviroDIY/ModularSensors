/*
 *MaximDS3231.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for required DS3231 real time clock.
 *It is dependent on the EnviroDIY DS3231 library.
 *
 *For temperature from the DS3231:
 *  Resolution is 0.25°C
 *  Accuracy is ±3°C
 *  Range is 0°C to +70°C
 *
 * The clock should have a separate power supply and never be turned off.
 * We assume it's always warmed up and stable.
 * The temperature conversion typically takes 125 ms, with a max time of 200 ms.
*/

// Header Guards
#ifndef MaximDS3231_h
#define MaximDS3231_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define DS3231_NUM_VARIABLES 1
#define DS3231_WARM_UP_TIME_MS 0
#define DS3231_STABILIZATION_TIME_MS 0
#define DS3231_MEASUREMENT_TIME_MS 200

#define DS3231_TEMP_RESOLUTION 2
#define DS3231_TEMP_VAR_NUM 0


// The "Main" class for the DS3231
// Only need a sleep and wake since these DON'T use the default of powering up and down
class MaximDS3231 : public Sensor
{
public:
    // Only input is the number of readings to average
    MaximDS3231(uint8_t measurementsToAverage = 1);
    // Destructor
    ~MaximDS3231();

    String getSensorLocation(void) override;

    bool setup(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;
};


class MaximDS3231_Temp : public Variable
{
public:
    MaximDS3231_Temp(Sensor *parentSense,
                     const char *uuid = "",
                     const char *varCode = "BoardTemp")
      : Variable(parentSense,
                 (const uint8_t)DS3231_TEMP_VAR_NUM,
                 (uint8_t)DS3231_TEMP_RESOLUTION,
                 "temperatureDatalogger", "degreeCelsius",
                 varCode, uuid)
    {}
    MaximDS3231_Temp()
      : Variable((const uint8_t)DS3231_TEMP_VAR_NUM,
                 (uint8_t)DS3231_TEMP_RESOLUTION,
                 "temperatureDatalogger", "degreeCelsius", "BoardTemp")
    {}
    ~MaximDS3231_Temp(){}
};

#endif  // Header Guard
