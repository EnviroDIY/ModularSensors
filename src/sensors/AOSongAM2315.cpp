/**
 * @file AOSongAM2315.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the AOSongAM2315 class.
 */

#include "AOSongAM2315.h"
#include <Adafruit_AM2315.h>


// The constructor - because this is I2C, only need the power pin
// This sensor has a set I2C address of 0XB8
AOSongAM2315::AOSongAM2315(TwoWire* theI2C, int8_t powerPin,
                           uint8_t measurementsToAverage)
    : Sensor("AOSongAM2315", AM2315_NUM_VARIABLES, AM2315_WARM_UP_TIME_MS,
             AM2315_STABILIZATION_TIME_MS, AM2315_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage),
      _i2c(theI2C != nullptr ? theI2C : &Wire) {}
// Delegating constructor
AOSongAM2315::AOSongAM2315(int8_t powerPin, uint8_t measurementsToAverage)
    : AOSongAM2315(&Wire, powerPin, measurementsToAverage) {}


String AOSongAM2315::getSensorLocation() {
    return F("I2C_0xB8");
}


bool AOSongAM2315::setup() {
    _i2c->begin();  // Start the wire library (sensor power not required)
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    _i2c->setTimeout(0);
    return Sensor::setup();  // this will set pin modes and the setup status bit
}


bool AOSongAM2315::addSingleMeasurementResult() {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return finalizeMeasurementAttempt(false);
    }

    bool  success   = false;
    float temp_val  = MS_INVALID_VALUE;
    float humid_val = MS_INVALID_VALUE;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    Adafruit_AM2315 am2315(_i2c);
    if (!am2315.begin()) {
        MS_DBG(getSensorNameAndLocation(), F("AM2315 begin() failed"));
        return finalizeMeasurementAttempt(false);
    }
    success = am2315.readTemperatureAndHumidity(&temp_val, &humid_val);


    success &= !isnan(temp_val) && temp_val != MS_INVALID_VALUE &&
        !isnan(humid_val) && humid_val != MS_INVALID_VALUE;

    MS_DBG(F("  Temp:"), temp_val, F("°C"));
    MS_DBG(F("  Humidity:"), humid_val, '%');

    if (success) {
        verifyAndAddMeasurementResult(AM2315_TEMP_VAR_NUM, temp_val);
        verifyAndAddMeasurementResult(AM2315_HUMIDITY_VAR_NUM, humid_val);
    }

    // Return success value when finished
    return finalizeMeasurementAttempt(success);
}
