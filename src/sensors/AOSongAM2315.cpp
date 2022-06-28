/**
 * @file AOSongAM2315.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the AOSongAM2315 class.
 */

#include "AOSongAM2315.h"


// The constructor - because this is I2C, only need the power pin
// This sensor has a set I2C address of 0XB8
AOSongAM2315::AOSongAM2315(TwoWire* theI2C, int8_t powerPin,
                           uint8_t measurementsToAverage)
    : Sensor("AOSongAM2315", AM2315_NUM_VARIABLES, AM2315_WARM_UP_TIME_MS,
             AM2315_STABILIZATION_TIME_MS, AM2315_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage),
      _i2c(theI2C) {
    am2315ptr = new Adafruit_AM2315(_i2c);
}
AOSongAM2315::AOSongAM2315(int8_t powerPin, uint8_t measurementsToAverage)
    : Sensor("AOSongAM2315", AM2315_NUM_VARIABLES, AM2315_WARM_UP_TIME_MS,
             AM2315_STABILIZATION_TIME_MS, AM2315_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, AM2315_INC_CALC_VARIABLES),
      _i2c(&Wire) {
    am2315ptr = new Adafruit_AM2315(_i2c);
}
AOSongAM2315::~AOSongAM2315() {}


String AOSongAM2315::getSensorLocation(void) {
    return F("I2C_0xB8");
}


bool AOSongAM2315::setup(void) {
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


bool AOSongAM2315::addSingleMeasurementResult(void) {
    // Initialize float variables
    float temp_val  = -9999;
    float humid_val = -9999;
    bool  ret_val   = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        ret_val = am2315ptr->readTemperatureAndHumidity(&temp_val, &humid_val);

        if (!ret_val || isnan(temp_val)) temp_val = -9999;
        if (!ret_val || isnan(humid_val)) humid_val = -9999;

        MS_DBG(F("  Temp:"), temp_val, F("°C"));
        MS_DBG(F("  Humidity:"), humid_val, '%');
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(AM2315_TEMP_VAR_NUM, temp_val);
    verifyAndAddMeasurementResult(AM2315_HUMIDITY_VAR_NUM, humid_val);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return ret_val;
}
