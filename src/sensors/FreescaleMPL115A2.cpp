/**
 * @file FreescaleMPL115A2.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the FreescaleMPL115A2 class.
 */

#include "FreescaleMPL115A2.h"


// The constructor - because this is I2C, only need the power pin
// This sensor has a set I2C address of 0x60.
FreescaleMPL115A2::FreescaleMPL115A2(TwoWire* theI2C, int8_t powerPin,
                                     uint8_t measurementsToAverage)
    : Sensor("FreescaleMPL115A2", MPL115A2_NUM_VARIABLES,
             MPL115A2_WARM_UP_TIME_MS, MPL115A2_STABILIZATION_TIME_MS,
             MPL115A2_MEASUREMENT_TIME_MS, powerPin, -1, measurementsToAverage),
      _i2c(theI2C) {}
FreescaleMPL115A2::FreescaleMPL115A2(int8_t  powerPin,
                                     uint8_t measurementsToAverage)
    : Sensor("FreescaleMPL115A2", MPL115A2_NUM_VARIABLES,
             MPL115A2_WARM_UP_TIME_MS, MPL115A2_STABILIZATION_TIME_MS,
             MPL115A2_MEASUREMENT_TIME_MS, powerPin, -1, measurementsToAverage,
             MPL115A2_INC_CALC_VARIABLES),
      _i2c(&Wire) {}
// Destructor
FreescaleMPL115A2::~FreescaleMPL115A2() {}


String FreescaleMPL115A2::getSensorLocation(void) {
    return F("I2C_0x60");
}


bool FreescaleMPL115A2::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // The MPL115A2's begin() reads required coefficients from the sensor.
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Run the sensor begin()
    // This doesn't return anything to indicate failure or success, we just have
    // to hope
    mpl115a2_internal.begin(_i2c);

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return retVal;
}


bool FreescaleMPL115A2::addSingleMeasurementResult(void) {
    // Initialize float variables
    float temp  = -9999;
    float press = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // Read values
        mpl115a2_internal.getPT(&press, &temp);

        if (isnan(temp)) temp = -9999;
        if (isnan(press)) press = -9999;

        if (press > 115.0 || temp < -40.0) {
            temp  = -9999;
            press = -9999;
        }

        MS_DBG(F("  Temperature:"), temp);
        MS_DBG(F("  Pressure:"), press);
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(MPL115A2_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(MPL115A2_PRESSURE_VAR_NUM, press);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // no way of knowing if successful, just return true
    return true;
}
