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
    return Sensor::setup();  // this will set pin modes and the setup status bit
    // The clock should be continuously powered, so we never need to worry about power up
}


// Do nothing for the power down and sleep functions
// The clock never sleeps or powers down
bool MaximDS3231::sleep(void)
{return true;}
void MaximDS3231::powerDown(void)
{}


// Sending the device a request to start temp conversion.
bool MaximDS3231::startSingleMeasurement(void)
{
    // this will check that it's awake/active and set timestamp and status bit
    bool success = Sensor::startSingleMeasurement();

    // if the sensor::startSingleMeasurement() failed, bail
    if (!success) return success;

    // force a temperature sampling and conversion
    // this function already has a forced wait for the conversion to complete
    // TODO:  Test how long the conversion takes, update DS3231 lib accordingly!
    MS_DBG(F("Forcing new temperature reading by DS3231\n"));
    rtc.convertTemperature(false);

    return success;
}


bool MaximDS3231::addSingleMeasurementResult(void)
{
    // get the temperature value
    MS_DBG(F("Getting temperature from DS3231\n"));
    float tempVal = rtc.getTemperature();
    MS_DBG(F("Current temp is "), tempVal, '\n');

    verifyAndAddMeasurementResult(DS3231_TEMP_VAR_NUM, tempVal);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
