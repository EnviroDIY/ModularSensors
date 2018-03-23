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
 * The clock should have a separate power supply and never be turned off.
 * We assume it's always warmed up.
 * The temperature conversion typically takes 125 ms, with a max time of 200 ms.
*/

#include <Sodaq_DS3231.h>
#include "MaximDS3231.h"


String MaximDS3231::getSensorLocation(void) {return F("I2C_0x68");}


bool MaximDS3231::setup(void)
{
    rtc.begin();
    return Sensor::setup();
}


// Sending the device a request to start temp conversion.
bool MaximDS3231::startSingleMeasurement(void)
{
    waitForWarmUp();
    waitForStability();

    // force a temperature sampling and conversion
    // this function already has a forced wait for the conversion to complete
    // TODO:  Test how long the conversion takes, update DS3231 lib accordingly!
    MS_DBG(F("Forcing new temperature reading\n"));
    rtc.convertTemperature(false);

    // Mark the time that a measurement was requested
    _millisMeasurementRequested = millis();
    // Set the status bits for measurement requested (bit 5)
    _sensorStatus |= 0b00100000;
    // Verify that the status bit for a single measurement completion is not set (bit 6)
    _sensorStatus &= 0b10111111;
    return true;
}


bool MaximDS3231::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a reading to finish
    waitForMeasurementCompletion();

    // get the temperature value
    MS_DBG(F("Getting value\n"));
    float tempVal = rtc.getTemperature();
    MS_DBG(F("Current temp is "), tempVal, '\n');

    verifyAndAddMeasurementResult(DS3231_TEMP_VAR_NUM, tempVal);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Make sure the status bits for measurement request (bit 5) and measurement
    // completion (bit 6) are no longer set
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
