/**
 * @file ProcessorAnalog.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the ProcessorAnalog class.
 */


#include "ProcessorAnalog.h"


// ============================================================================
// ProcessorAnalogBase Constructor
// ============================================================================

ProcessorAnalogBase::ProcessorAnalogBase(float voltageMultiplier,
                                         float operatingVoltage)
    : AnalogVoltageBase(voltageMultiplier, operatingVoltage) {}


// ============================================================================
// ProcessorAnalogBase Functions
// ============================================================================

bool ProcessorAnalogBase::begin(void) {
    // For processor analog systems, no special initialization is required
    // beyond what is done in the constructor
    return true;
}

bool ProcessorAnalogBase::readVoltageSingleEnded(int8_t analogChannel,
                                                 float& resultValue) {
    // Compile-time validation of ADC configuration
    static_assert(PROCESSOR_ADC_MAX > 0,
                  "PROCESSOR_ADC_MAX must be greater than 0. Check "
                  "MS_PROCESSOR_ADC_RESOLUTION configuration.");

    // Validate parameters
    if (analogChannel < 0 || analogChannel > MS_PROCESSOR_ANALOG_MAX_CHANNEL ||
        _supplyVoltage <= 0 || _voltageMultiplier <= 0) {
        MS_DBG(F("Invalid configuration: either the analog channel, the supply "
                 "voltage, or the voltage multiplier is not set correctly!"));
        resultValue = -9999.0f;
        return false;
    }

    // Get the analog voltage
    MS_DBG(F("Getting analog voltage from pin"), analogChannel);
    pinMode(analogChannel, INPUT);
    analogRead(analogChannel);  // priming reading
    // The return value from analogRead() is IN BITS NOT IN VOLTS!!
    analogRead(analogChannel);  // another priming reading
    int   rawAdc    = analogRead(analogChannel);
    float rawAnalog = static_cast<float>(rawAdc);
    MS_DBG(F("Raw analog pin reading in bits:"), rawAnalog);

    // convert bits to volts
    // Use (PROCESSOR_ADC_MAX + 1) as divisor for correct 2^n scaling
    resultValue =
        (_supplyVoltage / (static_cast<float>(PROCESSOR_ADC_MAX) + 1.0f)) *
        _voltageMultiplier * rawAnalog;
    MS_DBG(F("Voltage:"), resultValue);

    // NOTE: We don't actually have any criteria for if the reading was any
    // good or not, so we mark it as successful no matter what.
    return true;
}

String
ProcessorAnalogBase::getAnalogLocation(int8_t analogChannel,
                                       int8_t /*analogReferenceChannel*/) {
    String sensorLocation;
    sensorLocation += F("ProcessorAnalog_Pin");
    sensorLocation += String(analogChannel);
    return sensorLocation;
}

bool ProcessorAnalogBase::readVoltageDifferential(
    int8_t /*analogChannel*/, int8_t /*analogReferenceChannel*/,
    float& resultValue) {
    // ProcessorAnalog does not support differential measurements
    MS_DBG(F("ProcessorAnalog does not support differential measurements"));
    resultValue = -9999.0f;
    return false;
}

float ProcessorAnalogBase::calculateAnalogResolutionVolts(void) {
    // Use the configured processor ADC resolution
    uint8_t resolutionBits = MS_PROCESSOR_ADC_RESOLUTION;

    // For processor ADCs, the full scale range is the supply voltage
    // (single-ended measurements from 0V to supply voltage)
    float fullScaleRangeVolts = _supplyVoltage;

    if (resolutionBits == 0 || resolutionBits >= 32 ||
        fullScaleRangeVolts <= 0.0f) {
        MS_DBG(F("Invalid ADC configuration - bits: "), resolutionBits,
               F(", supply voltage: "), fullScaleRangeVolts, F("V"));
        return -9999.0f;
    }

    // Calculate the total number of ADC codes
    uint32_t totalCodes = 1UL << resolutionBits;  // 2^resolutionBits

    // Voltage resolution is the full scale range divided by total codes
    float resolutionVolts = fullScaleRangeVolts /
        static_cast<float>(totalCodes);

    MS_DBG(F("Processor ADC resolution calculation:"));
    MS_DBG(F("  ADC resolution: "), resolutionBits, F(" bits"));
    MS_DBG(F("  Supply voltage: "), fullScaleRangeVolts, F("V"));
    MS_DBG(F("  Total codes: "), totalCodes);
    MS_DBG(F("  Voltage resolution: "), resolutionVolts, F("V/LSB"));

    return resolutionVolts;
}


// ============================================================================
// ProcessorAnalog Functions
// ============================================================================

// The constructor - need the power pin, the data pin, the voltage divider
// value, and the operating voltage
ProcessorAnalog::ProcessorAnalog(int8_t powerPin, int8_t dataPin,
                                 uint8_t              measurementsToAverage,
                                 ProcessorAnalogBase* analogVoltageReader)
    : Sensor("ProcessorAnalog", PROCESSOR_ANALOG_NUM_VARIABLES,
             PROCESSOR_ANALOG_WARM_UP_TIME_MS,
             PROCESSOR_ANALOG_STABILIZATION_TIME_MS,
             PROCESSOR_ANALOG_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage, PROCESSOR_ANALOG_INC_CALC_VARIABLES),
      // If no analog base provided, create one with default settings
      _analogVoltageReader(analogVoltageReader == nullptr
                               ? new ProcessorAnalogBase()
                               : analogVoltageReader),
      _ownsAnalogVoltageReader(analogVoltageReader == nullptr) {}

// Destructor
ProcessorAnalog::~ProcessorAnalog() {
    // Clean up the analog base object if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
        _analogVoltageReader = nullptr;
    }
}

String ProcessorAnalog::getSensorLocation() {
    if (_analogVoltageReader != nullptr) {
        return _analogVoltageReader->getAnalogLocation(_dataPin, -1);
    } else {
        return String(F("Unknown_AnalogVoltageReader"));
    }
}

bool ProcessorAnalog::addSingleMeasurementResult(void) {
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

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    float resultValue = -9999.0f;
    bool  success     = _analogVoltageReader->readVoltageSingleEnded(_dataPin,
                                                                     resultValue);

    if (success) {
        verifyAndAddMeasurementResult(PROCESSOR_ANALOG_VAR_NUM, resultValue);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
