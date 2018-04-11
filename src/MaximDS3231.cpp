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


// Do nothing for the power down and sleep functions
// The clock never sleeps or powers down
bool MaximDS3231::sleep(void)
{return true;}
void MaximDS3231::powerDown(void)
{}


// Sending the device a request to start temp conversion.
bool MaximDS3231::startSingleMeasurement(void)
{
    bool success = true;

    // Check if activated, wake if not
    // if (_millisSensorActivated == 0 || bitRead(_sensorStatus, 3))
    //     success = wake();

    // Check again if activated, only wait if it is
    if (_millisSensorActivated > 0 && bitRead(_sensorStatus, 3))
    {
        // waitForStability();

        // force a temperature sampling and conversion
        // this function already has a forced wait for the conversion to complete
        // TODO:  Test how long the conversion takes, update DS3231 lib accordingly!
        MS_DBG(F("Forcing new temperature reading\n"));
        rtc.convertTemperature(false);

        // Mark the time that a measurement was requested
        _millisMeasurementRequested = millis();
    }
    // Make sure that the time of a measurement request is not set
    else _millisMeasurementRequested = 0;

    // Even if we failed to start a measurement, we still want to set the status
    // bit to show that we attempted to start the measurement.
    // Set the status bits for measurement requested (bit 5)
    _sensorStatus |= 0b00100000;
    // Verify that the status bit for a single measurement completion is not set (bit 6)
    _sensorStatus &= 0b10111111;
    return success;
}


bool MaximDS3231::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a reading to finish
    // waitForMeasurementCompletion();

    // get the temperature value
    MS_DBG(F("Getting value\n"));
    float tempVal = rtc.getTemperature();
    MS_DBG(F("Current temp is "), tempVal, '\n');

    verifyAndAddMeasurementResult(DS3231_TEMP_VAR_NUM, tempVal);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    // Return true when finished
    return true;
}
