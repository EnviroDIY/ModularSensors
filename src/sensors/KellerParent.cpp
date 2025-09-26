/**
 * @file KellerParent.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the KellerParent class.
 */

#include "KellerParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for
// data, and number of readings to average
KellerParent::KellerParent(byte modbusAddress, Stream* stream, int8_t powerPin,
                           int8_t powerPin2, int8_t enablePin,
                           uint8_t measurementsToAverage, kellerModel model,
                           const char* sensName, uint8_t numVariables,
                           uint32_t warmUpTime_ms,
                           uint32_t stabilizationTime_ms,
                           uint32_t measurementTime_ms)
    : Sensor(sensName, numVariables, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, -1, measurementsToAverage,
             KELLER_INC_CALC_VARIABLES),
      _model(model),
      _modbusAddress(modbusAddress),
      _stream(stream),
      _RS485EnablePin(enablePin) {
    setSecondaryPowerPin(powerPin2);
}
KellerParent::KellerParent(byte modbusAddress, Stream& stream, int8_t powerPin,
                           int8_t powerPin2, int8_t enablePin,
                           uint8_t measurementsToAverage, kellerModel model,
                           const char* sensName, uint8_t numVariables,
                           uint32_t warmUpTime_ms,
                           uint32_t stabilizationTime_ms,
                           uint32_t measurementTime_ms)
    : Sensor(sensName, numVariables, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, -1, measurementsToAverage,
             KELLER_INC_CALC_VARIABLES),
      _model(model),
      _modbusAddress(modbusAddress),
      _stream(&stream),
      _RS485EnablePin(enablePin) {
    setSecondaryPowerPin(powerPin2);
}
// Destructor
KellerParent::~KellerParent() {}


// The sensor installation location on the Mayfly
String KellerParent::getSensorLocation(void) {
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress < 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


bool KellerParent::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit
    if (_RS485EnablePin >= 0) { pinMode(_RS485EnablePin, OUTPUT); }

#ifdef MS_KELLERPARENT_DEBUG_DEEP
    _ksensor.setDebugStream(&MS_SERIAL_OUTPUT);
#endif

    // This sensor begin is just setting more pin modes, etc, no sensor power
    // required This really can't fail so adding the return value is just for
    // show
    retVal &= _ksensor.begin(_model, _modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// The function to put the sensor to sleep
// Different from the standard in that empties and flushes the stream.
bool KellerParent::sleep(void) {
    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();
    return Sensor::sleep();
};


bool KellerParent::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool  success            = false;
    float waterPressureBar   = -9999;
    float waterTemperatureC  = -9999;
    float waterDepthM        = -9999;
    float waterPressure_mBar = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Get Values
    success     = _ksensor.getValues(waterPressureBar, waterTemperatureC);
    waterDepthM = _ksensor.calcWaterDepthM(
        waterPressureBar,
        waterTemperatureC);  // float calcWaterDepthM(float
                             // waterPressureBar, float waterTemperatureC)

    // For waterPressureBar, convert bar to millibar
    if (!isnan(waterPressureBar) && waterPressureBar != -9999)
        waterPressure_mBar = 1000 * waterPressureBar;

    MS_DBG(F("  Pressure_mbar:"), waterPressure_mBar);
    MS_DBG(F("  Temp_C:"), waterTemperatureC);
    MS_DBG(F("  Height_m:"), waterDepthM);

    success &= (waterPressureBar != -9999 && waterTemperatureC != -9999 &&
                waterDepthM != -9999);

    if (success) {
        // Put values into the array
        verifyAndAddMeasurementResult(KELLER_PRESSURE_VAR_NUM,
                                      waterPressure_mBar);
        verifyAndAddMeasurementResult(KELLER_TEMP_VAR_NUM, waterTemperatureC);
        verifyAndAddMeasurementResult(KELLER_HEIGHT_VAR_NUM, waterDepthM);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}

// cSpell:ignore ksensor
