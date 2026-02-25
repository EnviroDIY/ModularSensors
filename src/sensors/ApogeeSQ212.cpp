/**
 * @file ApogeeSQ212.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.cpp by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ApogeeSQ212 class.
 */


#include "ApogeeSQ212.h"
#include "TIADS1x15.h"


// The constructor - need the power pin and the data pin
ApogeeSQ212::ApogeeSQ212(int8_t powerPin, int8_t analogChannel,
                         uint8_t            measurementsToAverage,
                         AnalogVoltageBase* analogVoltageReader)
    : Sensor("ApogeeSQ212", SQ212_NUM_VARIABLES, SQ212_WARM_UP_TIME_MS,
             SQ212_STABILIZATION_TIME_MS, SQ212_MEASUREMENT_TIME_MS, powerPin,
             analogChannel, measurementsToAverage, SQ212_INC_CALC_VARIABLES),
      _analogVoltageReader(analogVoltageReader),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader = createTIADS1x15Base(_ownsAnalogVoltageReader);
    }
}

// Destructor
ApogeeSQ212::~ApogeeSQ212() {
    // Clean up the analog voltage reader if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
    }
}


String ApogeeSQ212::getSensorLocation(void) {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getAnalogLocation(_dataPin, -1);
    } else {
        return String("Unknown_AnalogVoltageReader");
    }
}


bool ApogeeSQ212::addSingleMeasurementResult(void) {
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

    float adcVoltage = -9999.0f;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Read the single-ended analog voltage using the AnalogVoltageBase
    // interface.
    // NOTE: All implementations of the AnalogVoltageBase class validate both
    // the input channel and the resulting voltage, so we can trust that a
    // successful read will give us a valid voltage value to work with.
    bool success = _analogVoltageReader->readVoltageSingleEnded(_dataPin,
                                                                adcVoltage);

    if (success) {
        // Apply the calibration factor.
        // The Apogee SQ-212 is factory calibrated with a calibration factor
        // of 1.0 μmol m-2 s-1 per mV.  If the user wants to use a custom value,
        // it must be set as a preprocessor definition when compiling the
        // library, e.g. by adding -DSQ212_CALIBRATION_FACTOR=0.95 to the
        // compiler flags.
        float calibResult = 1000.0f * adcVoltage * SQ212_CALIBRATION_FACTOR;
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(SQ212_PAR_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(SQ212_VOLTAGE_VAR_NUM, adcVoltage);
    } else {
        MS_DBG(F("  Failed to get valid voltage from analog reader"));
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
