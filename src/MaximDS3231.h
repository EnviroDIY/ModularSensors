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
 * The temperature conversion typical takes 125 ms, with a max time of 200 ms.
*/

#ifndef MaximDS3231_h
#define MaximDS3231_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#define DS3231_NUM_VARIABLES 1
#define DS3231_WARM_UP 0
#define DS3231_STABILITY 0
#define DS3231_RESAMPLE 0  // The function to get a value forces the wait already
#define DS3231_TEMP_RESOLUTION 2
#define DS3231_TEMP_VAR_NUM 0


// The "Main" class for the DS3231
// Only need a sleep and wake since these DON'T use the default of powering up and down
class MaximDS3231 : public Sensor
{
public:
    // Only input is the number of readings to average
    MaximDS3231(int readingsToAverage = 1)
    : Sensor(F("MaximDS3231"), DS3231_NUM_VARIABLES,
             DS3231_WARM_UP, DS3231_STABILITY, DS3231_RESAMPLE,
             -1, -1, readingsToAverage)
    {}

    String getSensorLocation(void) override;

    // Do nothing for any of the power up/down or wake/sleep functions
    void powerUp(void) override {}
    bool wake(void) override {return true;}
    bool sleep(void) override {return true;}
    void powerDown(void) override {}

    SENSOR_STATUS setup(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;
};


class MaximDS3231_Temp : public Variable
{
public:
    MaximDS3231_Temp(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, DS3231_TEMP_VAR_NUM,
                 F("temperatureRTC"), F("degreeCelsius"),
                 DS3231_TEMP_RESOLUTION,
                 F("BoardTemp"), UUID, customVarCode)
    {}
};

#endif
