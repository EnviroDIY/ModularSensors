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


// The constructor - need the power pin, the data pin, the voltage divider
// value, and the operating voltage
ProcessorAnalog::ProcessorAnalog(int8_t powerPin, int8_t dataPin,
                                 float   voltageMultiplier,
                                 float   operatingVoltage,
                                 uint8_t measurementsToAverage)
    : Sensor("ProcessorAnalog", PROCESSOR_ANALOG_NUM_VARIABLES,
             PROCESSOR_ANALOG_WARM_UP_TIME_MS,
             PROCESSOR_ANALOG_STABILIZATION_TIME_MS,
             PROCESSOR_ANALOG_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage, PROCESSOR_ANALOG_INC_CALC_VARIABLES),
      ProcessorAnalogBase(dataPin, voltageMultiplier, operatingVoltage) {}
// Destructor
ProcessorAnalog::~ProcessorAnalog() {}


bool ProcessorAnalog::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    float resultValue = -9999;
    bool  success     = readVoltageSingleEnded(resultValue);

    if (success) {
        verifyAndAddMeasurementResult(PROCESSOR_ANALOG_VAR_NUM, resultValue);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}

bool ProcessorAnalogBase::readVoltageSingleEnded(float& resultValue) {
    // Validate parameters
    if (PROCESSOR_ADC_MAX <= 0) {
        MS_DBG(F("Processor ADC max value is not set or invalid!"));
        return false;
    }
    if (_analogChannel < 0 || _supplyVoltage <= 0 || _voltageMultiplier <= 0) {
        MS_DBG(F("Missing one or more required parameters: analog pin, "
                 "operating voltage, or voltage divider!"));
        return false;
    }

    // Get the analog voltage
    MS_DBG(F("Getting analog voltage from pin"), _analogChannel);
    pinMode(_analogChannel, INPUT);
    analogRead(_analogChannel);  // priming reading
    // The return value from analogRead() is IN BITS NOT IN VOLTS!!
    analogRead(_analogChannel);  // another priming reading
    float rawAnalog = analogRead(_analogChannel);
    MS_DBG(F("Raw analog pin reading in bits:"), rawAnalog);

    // convert bits to volts
    float sensorValue_analog =
        (_supplyVoltage / static_cast<float>(PROCESSOR_ADC_MAX)) *
        _voltageMultiplier * rawAnalog;
    MS_DBG(F("Voltage:"), sensorValue_analog);

    resultValue = sensorValue_analog;

    // NOTE: We don't actually have any criteria for if the reading was any
    // good or not, so we mark it as successful no matter what.
    return true;
}

String ProcessorAnalogBase::getSensorLocation(void) {
    String sensorLocation = F("ProcessorAnalog_Pin");
    sensorLocation += String(_analogChannel);
    return sensorLocation;
}
