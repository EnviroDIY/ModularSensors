/*
 *MaximDS3231.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the onboard "sensors" on the EnviroDIY Mayfly
 *It is dependent on the EnviroDIY DS3231 library.
 *
 *For temperature from the DS3231:
 *  Resolution is 0.25°C
 *  Accuracy is ±3°C
 *
 * The clock is always ready to take a reading.
*/

#ifndef MaximDS3231_h
#define MaximDS3231_h

#include "SensorBase.h"
#include "VariableBase.h"

// #define MODULES_DBG Serial
#include "ModSensorDebugger.h"

#define DS3231_NUM_MEASUREMENTS 1
#define DS3231_WARM_UP 0
#define DS3231_TEMP_RESOLUTION 2
#define DS3231_TEMP_VAR_NUM 0


// The "Main" class for the Mayfly
// Only need a sleep and wake since these DON'T use the default of powering up and down
class MaximDS3231 : public Sensor
{
public:
    // Need to know the Mayfly version because the battery resistor depends on it
    MaximDS3231(void);

    String getSensorLocation(void) override;
    bool sleep(void) override;
    bool wake(void) override;

    bool update(void) override;
};


class MaximDS3231_Temp : public Variable
{
public:
    MaximDS3231_Temp(Sensor *parentSense, String customVarCode = "")
      : Variable(parentSense, DS3231_TEMP_VAR_NUM,
                 F("temperatureRTC"), F("degreeCelsius"),
                 DS3231_TEMP_RESOLUTION,
                 F("BoardTemp"), customVarCode)
    {}
};

#endif
