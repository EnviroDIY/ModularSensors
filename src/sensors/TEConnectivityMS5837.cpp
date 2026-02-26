/**
 * @file TEConnectivityMS5837.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the TEConnectivityMS5837 class.
 */

#include "TEConnectivityMS5837.h"


// The constructor - because this is I2C, only need the power pin
TEConnectivityMS5837::TEConnectivityMS5837(int8_t powerPin, uint8_t model,
                                           uint8_t measurementsToAverage)
    : Sensor("TEConnectivityMS5837", MS5837_NUM_VARIABLES,
             MS5837_WARM_UP_TIME_MS, MS5837_STABILIZATION_TIME_MS,
             MS5837_MEASUREMENT_TIME_MS, powerPin, -1, measurementsToAverage,
             MS5837_INC_CALC_VARIABLES),
      _model(model) {}

// Destructor
TEConnectivityMS5837::~TEConnectivityMS5837() {}


String TEConnectivityMS5837::getSensorLocation(void) {
    String modelStr = F("I2C_0x76_");
    switch (_model) {
        case MS5837_02BA: modelStr += F("02BA"); break;
        case MS5837_30BA: modelStr += F("30BA"); break;
        default: modelStr += F("Unknown"); break;
    }
    return modelStr;
}


bool TEConnectivityMS5837::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    delay(10);
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Initialize the sensor
    MS5837_internal.init();

    // Set the sensor model
    if (!MS5837_internal.setModel(_model)) {
        MS_DBG(getSensorNameAndLocation(), F("Failed to set sensor model"));
        retVal = false;
    }

    // Turn the power back off if it had been turned on
    if (!wasOn) { powerDown(); }

    return retVal;
}


bool TEConnectivityMS5837::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool  success = false;
    float temp    = -9999;
    float press   = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Read values from the sensor
    MS5837_internal.read();

    // Get temperature in Celsius
    temp = MS5837_internal.temperature();

    // Get pressure in millibar
    press = MS5837_internal.pressure();

    MS_DBG(F("  Temperature:"), temp);
    MS_DBG(F("  Pressure:"), press);

    // Validate the readings
    float maxPressure = 0;
    switch (_model) {
        case MS5837_02BA: maxPressure = 2000.0; break;   // 2 bar = 2000 mbar
        case MS5837_30BA: maxPressure = 30000.0; break;  // 30 bar = 30000 mbar
        default: maxPressure = 30000.0; break;
    }

    if (!isnan(temp) && !isnan(press) && temp >= -40.0 && temp <= 85.0 &&
        press > 0.0 && press <= maxPressure) {
        // Temperature Range is -40°C to +85°C
        // Pressure returns 0 when disconnected, which is highly unlikely to be
        // a real value.
        // Pressure range depends on the model
        verifyAndAddMeasurementResult(MS5837_TEMP_VAR_NUM, temp);
        verifyAndAddMeasurementResult(MS5837_PRESSURE_VAR_NUM, press);
        success = true;
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
