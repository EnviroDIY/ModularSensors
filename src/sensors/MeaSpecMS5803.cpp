/**
 * @file MeaSpecMS5803.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com> with help from Beth
 * Fisher, Evan Host and Bobby Schulz.
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the MeaSpecMS5803 class.
 */

#include "MeaSpecMS5803.h"


// The constructor - because this is I2C, only need the power pin
MeaSpecMS5803::MeaSpecMS5803(int8_t powerPin, uint8_t i2cAddressHex,
                             int16_t maxPressure, uint8_t measurementsToAverage)
    : Sensor("MeaSpecMS5803", MS5803_NUM_VARIABLES, MS5803_WARM_UP_TIME_MS,
             MS5803_STABILIZATION_TIME_MS, MS5803_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, MS5803_INC_CALC_VARIABLES),
      _i2cAddressHex(i2cAddressHex),
      _maxPressure(maxPressure) {}
// Destructor
MeaSpecMS5803::~MeaSpecMS5803() {}


String MeaSpecMS5803::getSensorLocation(void) {
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool MeaSpecMS5803::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    delay(10);
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // This doesn't return anything to indicate failure or success, we just have
    // to hope
    MS5803_internal.begin(_i2cAddressHex, _maxPressure);
    MS5803_internal.reset();

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return retVal;
}


bool MeaSpecMS5803::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool  success = false;
    float temp    = -9999;
    float press   = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    // Read values
    // NOTE:  These functions actually include the request to begin
    // a measurement and the wait for said measurement to finish.
    // It's pretty fast (max of 11 ms) so we'll just wait.
    temp  = MS5803_internal.getTemperature(CELSIUS, ADC_512);
    press = MS5803_internal.getPressure(ADC_4096);

    MS_DBG(F("  Temperature:"), temp);
    MS_DBG(F("  Pressure:"), press);

    if (!isnan(temp) && !isnan(press) && temp > -50 && temp < 95 &&
        press != 0) {
        // Temperature Range is -40°C to +85°C
        // Pressure returns 0 when disconnected, which is highly unlikely to be
        // a real value.
        verifyAndAddMeasurementResult(MS5803_TEMP_VAR_NUM, temp);
        verifyAndAddMeasurementResult(MS5803_PRESSURE_VAR_NUM, press);
        success = true;
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
