/**
 * @file ProcessorAnalog.cpp *
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
ProcessorAnalog::ProcessorAnalog(int8_t powerPin, uint8_t dataPin,
                                 float   voltageMultiplier,
                                 float   operatingVoltage,
                                 uint8_t measurementsToAverage)
    : Sensor("ProcessorAnalog", PROCESSOR_ANALOG_NUM_VARIABLES,
             PROCESSOR_ANALOG_WARM_UP_TIME_MS,
             PROCESSOR_ANALOG_STABILIZATION_TIME_MS,
             PROCESSOR_ANALOG_MEASUREMENT_TIME_MS, powerPin, dataPin,
             measurementsToAverage, PROCESSOR_ANALOG_INC_CALC_VARIABLES),
      _voltageMultiplier(voltageMultiplier),
      _operatingVoltage(operatingVoltage) {}
// Destructor
ProcessorAnalog::~ProcessorAnalog() {}


bool ProcessorAnalog::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }
    if (_dataPin < 0 && _voltageMultiplier <= 0) {
        MS_DBG(F("No analog pin or voltage divider specified!"));
        return bumpMeasurementAttemptCount(false);
    }

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    float sensorValue_analog = -9999;
    // Get the analog voltage
    MS_DBG(F("Getting analog voltage from pin"), _dataPin);
    pinMode(_dataPin, INPUT);
    analogRead(_dataPin);  // priming reading
    // The return value from analogRead() is IN BITS NOT IN VOLTS!!
    analogRead(_dataPin);  // another priming reading
    float rawAnalog = analogRead(_dataPin);
    MS_DBG(F("Raw analog pin reading in bits:"), rawAnalog);
    // convert bits to volts
    sensorValue_analog =
        (_operatingVoltage / static_cast<float>(PROCESSOR_ADC_MAX)) *
        _voltageMultiplier * rawAnalog;
    MS_DBG(F("Voltage:"), sensorValue_analog);
    // NOTE: We don't actually have any criteria for if the reading was any
    // good or not, so we mark it as successful no matter what.
    verifyAndAddMeasurementResult(PROCESSOR_ANALOG_VAR_NUM, sensorValue_analog);
    return bumpMeasurementAttemptCount(true);
}
