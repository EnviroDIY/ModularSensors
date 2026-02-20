/**
 * @file AlphasenseCO2.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written by Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * and Bella Henkel <bella.henkel@mnsu.edu>
 * Adapted from ApogeeSQ212.h and
 * https://github.com/bellahenkel/Soil-Sensing-Device
 *
 * @brief Implements the AlphasenseCO2 class.
 */


#include "AlphasenseCO2.h"
#include "TIADS1x15.h"


// The constructor - need the power pin and the differential mux configuration
AlphasenseCO2::AlphasenseCO2(int8_t powerPin, tiads1x15_adsDiffMux_t adsDiffMux,
                             uint8_t i2cAddress, uint8_t measurementsToAverage)
    : TIADS1x15(powerPin, adsDiffMux, ALPHASENSE_CO2_VOLTAGE_MULTIPLIER,
                GAIN_ONE, i2cAddress, measurementsToAverage) {
    // Override timing settings for Alphasense CO2-specific requirements
    _warmUpTime_ms        = ALPHASENSE_CO2_WARM_UP_TIME_MS;
    _stabilizationTime_ms = ALPHASENSE_CO2_STABILIZATION_TIME_MS;
    _measurementTime_ms   = ALPHASENSE_CO2_MEASUREMENT_TIME_MS;
    // Override variable counts from parent class defaults
    _numReturnedValues = ALPHASENSE_CO2_NUM_VARIABLES;
    _incCalcValues     = ALPHASENSE_CO2_INC_CALC_VARIABLES;
    // Set the sensor name
    _sensorName = "AlphasenseCO2";
}
// Destructor
AlphasenseCO2::~AlphasenseCO2() {}


String AlphasenseCO2::getSensorLocation(void) {
    // Use TIADS1x15's location with Alphasense CO2-specific identifier
    String sensorLocation = TIADS1x15::getSensorLocation();
    sensorLocation += F("_AlphasenseCO2");
    return sensorLocation;
}


bool AlphasenseCO2::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool  success     = false;
    float adcVoltage  = -9999;
    float co2Current  = -9999;
    float calibResult = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Use the TIADS1x15 differential voltage reading function
    // The voltage multiplier and gain settings are handled by the parent class
    if (readVoltageDifferential(adcVoltage)) {
        MS_DBG(F("  Differential voltage:"), String(adcVoltage, 3), F("V"));
        // Convert voltage to current (mA) - using series sense resistor
        co2Current = (adcVoltage / ALPHASENSE_CO2_SENSE_RESISTOR_OHM) * 1000;
        MS_DBG(F("  co2Current:"),
               co2Current);  // Convert current to ppm (using a formula
                             // recommended by the sensor
        // manufacturer)
        calibResult = ALPHASENSE_CO2_MFG_SCALE * co2Current -
            ALPHASENSE_CO2_MFG_OFFSET;
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(ALPHASENSE_CO2_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(ALPHASENSE_CO2_VOLTAGE_VAR_NUM,
                                      adcVoltage);
        success = true;
    } else {
        MS_DBG(F("  Failed to read differential voltage"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
