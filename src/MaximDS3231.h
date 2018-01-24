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
 * The clock is always ready to take a reading.
*/

#ifndef MaximDS3231_h
#define MaximDS3231_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#define DS3231_NUM_MEASUREMENTS 1
#define DS3231_WARM_UP 0
#define DS3231_TEMP_RESOLUTION 2
#define DS3231_TEMP_VAR_NUM 0


// The "Main" class for the DS3231
// Only need a sleep and wake since these DON'T use the default of powering up and down
class MaximDS3231 : public Sensor
{
public:
    // No inputs for constructor
    // TODO:  Figure out why this doesn't work with "void"
    MaximDS3231(int unnecessary_var = 1)
    : Sensor(-1, -1, F("MaximDS3231"), DS3231_NUM_MEASUREMENTS, DS3231_WARM_UP)
    {}

    String getSensorLocation(void) override;
    SENSOR_STATUS setup(void) override;
    bool sleep(void) override;
    bool wake(void) override;

    bool update(void) override;
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
