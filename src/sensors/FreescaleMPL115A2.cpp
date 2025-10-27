/**
 * @file FreescaleMPL115A2.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
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
    delay(10);
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
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool  success = false;
    float temp    = -9999;
    float press   = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Read values
    mpl115a2_internal.getPT(&press, &temp);

    MS_DBG(F("  Temperature:"), temp);
    MS_DBG(F("  Pressure:"), press);

    if (!isnan(temp) && !isnan(press) && press >= 50.0 && press <= 115.0 &&
        temp >= -20.0 && temp <= 85.0) {
        verifyAndAddMeasurementResult(MPL115A2_TEMP_VAR_NUM, temp);
        verifyAndAddMeasurementResult(MPL115A2_PRESSURE_VAR_NUM, press);
        success = true;
    } else {
        MS_DBG(F("  Values outside expected range or invalid"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
