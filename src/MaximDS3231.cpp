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
 *
 * The clock is always ready to take a reading.
*/

#include <Sodaq_DS3231.h>
#include "MaximDS3231.h"


String MaximDS3231::getSensorLocation(void) {return F("I2C_0x68");}

SENSOR_STATUS MaximDS3231::setup(void)
{
    if (_powerPin > 0) pinMode(_powerPin, OUTPUT);
    rtc.begin();

    MS_DBG(F("Set up "));
    MS_DBG(getSensorName());
    MS_DBG(F(" attached at "));
    MS_DBG(getSensorLocation());
    MS_DBG(F(" which can return up to "));
    MS_DBG(_numReturnedVars);
    MS_DBG(F(" variable[s].\n"));

    return SENSOR_READY;
}

bool MaximDS3231::update(void)
{
    // Clear values before starting loop
    clearValues();

    // Get the temperature from the Mayfly's real time clock
    MS_DBG(F("Forcing new temperature reading\n"));
    rtc.convertTemperature();  // force a temperature sampling and conversion
    MS_DBG(F("Getting value\n"));
    float tempVal = rtc.getTemperature();  // get the temperature value
    MS_DBG(F("Current temp is "), tempVal, '\n');
    sensorValues[DS3231_TEMP_VAR_NUM] = tempVal;

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
