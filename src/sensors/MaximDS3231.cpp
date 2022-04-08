/**
 * @file MaximDS3231.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the MaximDS18 class.
 */

#include <Sodaq_DS3231.h>
#include "MaximDS3231.h"

// Only input is the number of readings to average
MaximDS3231::MaximDS3231(uint8_t measurementsToAverage)
    : Sensor("MaximDS3231", DS3231_NUM_VARIABLES, DS3231_WARM_UP_TIME_MS,
             DS3231_STABILIZATION_TIME_MS, DS3231_MEASUREMENT_TIME_MS, -1, -1,
             measurementsToAverage, DS3231_INC_CALC_VARIABLES) {}
// Destructor
MaximDS3231::~MaximDS3231() {}

String MaximDS3231::getSensorLocation(void) {
    return F("I2C_0x68");
}


bool MaximDS3231::setup(void) {
    rtc.begin();  // NOTE:  This also turns off interrupts on the RTC!
    return Sensor::setup();  // this will set pin modes and the setup status bit
    // The clock should be continuously powered, so we never need to worry about
    // power up
}


// Sending the device a request to start temp conversion.
bool MaximDS3231::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    // force a temperature sampling and conversion
    // this function already has a forced wait for the conversion to complete
    // TODO(SRGDamia1):  Test how long the conversion takes, update DS3231 lib
    // accordingly!
    MS_DBG(F("Forcing new temperature reading by DS3231"));
    rtc.convertTemperature(false);

    return true;
}


bool MaximDS3231::addSingleMeasurementResult(void) {
    // get the temperature value
    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    float tempVal = rtc.getTemperature();
    MS_DBG(F("  Temp:"), tempVal, F("°C"));

    verifyAndAddMeasurementResult(DS3231_TEMP_VAR_NUM, tempVal);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return true;
}
