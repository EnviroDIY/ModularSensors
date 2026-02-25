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


// The constructor - need the power pin and the data pin
AlphasenseCO2::AlphasenseCO2(int8_t powerPin, int8_t analogChannel,
                             int8_t             analogReferenceChannel,
                             uint8_t            measurementsToAverage,
                             AnalogVoltageBase* analogVoltageReader)
    : Sensor("AlphasenseCO2", ALPHASENSE_CO2_NUM_VARIABLES,
             ALPHASENSE_CO2_WARM_UP_TIME_MS,
             ALPHASENSE_CO2_STABILIZATION_TIME_MS,
             ALPHASENSE_CO2_MEASUREMENT_TIME_MS, powerPin, analogChannel,
             measurementsToAverage, ALPHASENSE_CO2_INC_CALC_VARIABLES),

      _analogReferenceChannel(analogReferenceChannel),
      _analogVoltageReader(analogVoltageReader),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader = createTIADS1x15Base(_ownsAnalogVoltageReader);
    }
}

// Destructor
AlphasenseCO2::~AlphasenseCO2() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
    }
}


String AlphasenseCO2::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getAnalogLocation(_dataPin,
                                                       _analogReferenceChannel);
    } else {
        return String("Unknown_AnalogVoltageReader");
    }
}


bool AlphasenseCO2::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    // Check if we have a valid analog voltage reader
    if (_analogVoltageReader == nullptr) {
        MS_DBG(getSensorNameAndLocation(),
               F("No analog voltage reader available"));
        return bumpMeasurementAttemptCount(false);
    }

    float adcVoltage = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Read differential voltage using the AnalogVoltageBase interface
    bool success = _analogVoltageReader->readVoltageDifferential(
        _dataPin, _analogReferenceChannel, adcVoltage);

    if (success) {
        // Convert voltage to current (mA) - assuming a 250 Ohm resistor is in
        // series
        float co2Current = (adcVoltage / ALPHASENSE_CO2_SENSE_RESISTOR_OHM) *
            1000;
        MS_DBG(F("  co2Current:"), co2Current);

        // Convert current to ppm (using a formula recommended by the sensor
        // manufacturer)
        float calibResult = (ALPHASENSE_CO2_MFG_SCALE * co2Current) -
            ALPHASENSE_CO2_MFG_OFFSET;
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(ALPHASENSE_CO2_VOLTAGE_VAR_NUM,
                                      adcVoltage);
        verifyAndAddMeasurementResult(ALPHASENSE_CO2_VAR_NUM, calibResult);
    } else {
        MS_DBG(F("  Failed to read differential voltage from analog reader"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
