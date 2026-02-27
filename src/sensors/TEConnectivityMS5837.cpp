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
                                           uint8_t measurementsToAverage,
                                           float   fluidDensity,
                                           float   airPressure)
    : Sensor("TEConnectivityMS5837", MS5837_NUM_VARIABLES,
             MS5837_WARM_UP_TIME_MS, MS5837_STABILIZATION_TIME_MS,
             MS5837_MEASUREMENT_TIME_MS, powerPin, -1, measurementsToAverage,
             MS5837_INC_CALC_VARIABLES),
      _model(model),
      _fluidDensity(fluidDensity),
      _airPressure(airPressure) {}

// Destructor
TEConnectivityMS5837::~TEConnectivityMS5837() {}


String TEConnectivityMS5837::getSensorLocation(void) {
    String modelStr = F("I2C_0x76_");
    switch (_model) {
        case MS5837_TYPE_02: modelStr += F("02BA"); break;
        case MS5837_TYPE_30: modelStr += F("30BA"); break;
        case MS5803_TYPE_01: modelStr += F("01BA"); break;
        default: modelStr += F("Unknown"); break;
    }
    return modelStr;
}


bool TEConnectivityMS5837::setup(void) {
    bool success =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    delay(10);
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Set the sensor model and initialize the sensor
    success &= MS5837_internal.begin(_model);

    if (success) {
        // Set the fluid density for depth calculations
        MS5837_internal.setDensity(_fluidDensity);
    }

    // Turn the power back off if it had been turned on
    if (!wasOn) { powerDown(); }

    if (!success) {
        MS_DBG(getSensorNameAndLocation(), F("Failed to initialize sensor"));
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // UN-set the set-up bit (bit 0) since setup failed!
        clearStatusBit(SETUP_SUCCESSFUL);
    }

    return success;
}


bool TEConnectivityMS5837::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    // Validate configuration parameters
    if (_fluidDensity <= 0.0 || _fluidDensity > 5.0) {
        MS_DBG(F("Invalid fluid density:"), _fluidDensity,
               F("g/cm³. Expected range: 0.0-5.0"));
        return bumpMeasurementAttemptCount(false);
    }
    if (_airPressure < 500.0 || _airPressure > 1200.0) {
        MS_DBG(F("Invalid air pressure:"), _airPressure,
               F("mBar. Expected range: 500-1200"));
        return bumpMeasurementAttemptCount(false);
    }

    float temp  = -9999;
    float press = -9999;
    float depth = -9999;
    float alt   = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Read values from the sensor - returns 0 on success
    bool success = MS5837_internal.read() == 0;
    if (success) {
        // Get temperature in Celsius
        temp = MS5837_internal.getTemperature();

        // Get pressure in millibar
        press = MS5837_internal.getPressure();

        // Calculate altitude in meters using configured air pressure
        alt = MS5837_internal.getAltitude(_airPressure);

        // Calculate depth in meters
        // Note: fluidDensity is set in the MS5837_internal object at setup and
        // used in the getDepth() function. The fluidDensity is only set in the
        // constructor and cannot be changed, so there's no reason to re-pass
        // the value to the internal object here.
        depth = MS5837_internal.getDepth();
    } else {
        MS_DBG(F("  Read failed, error:"), MS5837_internal.getLastError());
        return bumpMeasurementAttemptCount(false);
    }

    MS_DBG(F("  Temperature:"), temp);
    MS_DBG(F("  Pressure:"), press);
    MS_DBG(F("  Depth:"), depth);
    MS_DBG(F("  Altitude:"), alt);

    // Validate the readings
    float maxPressure = 0;
    switch (_model) {
        case MS5803_TYPE_01: maxPressure = 1000.0; break;  // 1 bar = 1000 mbar
        case MS5837_TYPE_02: maxPressure = 2000.0; break;  // 2 bar = 2000 mbar
        case MS5837_TYPE_30:
            maxPressure = 30000.0;
            break;  // 30 bar = 30000 mbar
        default: maxPressure = 30000.0; break;
    }

    if (!isnan(temp) && !isnan(press) && temp >= -40.0 && temp <= 85.0 &&
        press > 0.0 &&
        press <= maxPressure * 1.05) {  // allow 5% over max pressure
        // Temperature Range is -40°C to +85°C
        // Pressure returns 0 when disconnected, which is highly unlikely to be
        // a real value.
        // Pressure range depends on the model
        verifyAndAddMeasurementResult(MS5837_TEMP_VAR_NUM, temp);
        verifyAndAddMeasurementResult(MS5837_PRESSURE_VAR_NUM, press);

        // Store calculated values if they are valid
        if (!isnan(depth) && depth >= -2000.0 && depth <= 2000.0) {
            // Reasonable depth range from -2000m to +2000m
            verifyAndAddMeasurementResult(MS5837_DEPTH_VAR_NUM, depth);
        } else if (!isnan(depth)) {
            MS_DBG(F("  Depth out of range:"), depth);
        }
        if (!isnan(alt) && alt >= -1000.0 && alt <= 10000.0) {
            // Reasonable altitude range from -1000m to +10000m
            verifyAndAddMeasurementResult(MS5837_ALTITUDE_VAR_NUM, alt);
        } else if (!isnan(alt)) {
            MS_DBG(F("  Altitude out of range:"), alt);
        }

        success = true;
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
