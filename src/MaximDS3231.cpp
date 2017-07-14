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
 *For battery voltage:
 *  Range of 0-5V with 10bit ADC - resolution of 0.005
*/

#include <Sodaq_DS3231.h>
#include "MaximDS3231.h"

// Need to know the Mayfly version because the battery resistor depends on it
MaximDS3231::MaximDS3231(void)
: Sensor(-1, -1, F("MaximDS3231"), DS3231_NUM_MEASUREMENTS, DS3231_WARM_UP)
{}

String MaximDS3231::getSensorLocation(void) {return F("MaximDS3231");}
bool MaximDS3231::sleep(void) {return true;}
bool MaximDS3231::wake(void) {return true;}

bool MaximDS3231::update(void)
{
    // Clear values before starting loop
    clearValues();

    // Get the temperature from the Mayfly's real time clock
    DBGM(F("Getting DS3231 Temperature\n"));
    rtc.convertTemperature();  //convert current temperature into registers
    float tempVal = rtc.getTemperature();
    sensorValues[DS3231_TEMP_VAR_NUM] = tempVal;

    // Update the registered variables with the new values
    notifyVariables();

    // Return true when finished
    return true;
}
