/**
 * @file GroPointParent.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Implements the GroPointParent class.
 */

#include "GroPointParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for
// data, and number of readings to average
GroPointParent::GroPointParent(byte modbusAddress, Stream* stream,
                               int8_t powerPin, int8_t powerPin2,
                               int8_t enablePin, uint8_t measurementsToAverage,
                               gropointModel model, const char* sensName,
                               uint8_t numVariables, uint32_t warmUpTime_ms,
                               uint32_t stabilizationTime_ms,
                               uint32_t measurementTime_ms,
                               uint8_t  incCalcValues)
    : Sensor(sensName, numVariables, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, -1, measurementsToAverage,
             incCalcValues),
      _model(model),
      _modbusAddress(modbusAddress),
      _stream(stream),
      _RS485EnablePin(enablePin) {
    setSecondaryPowerPin(powerPin2);
}
GroPointParent::GroPointParent(byte modbusAddress, Stream& stream,
                               int8_t powerPin, int8_t powerPin2,
                               int8_t enablePin, uint8_t measurementsToAverage,
                               gropointModel model, const char* sensName,
                               uint8_t numVariables, uint32_t warmUpTime_ms,
                               uint32_t stabilizationTime_ms,
                               uint32_t measurementTime_ms,
                               uint8_t  incCalcValues)
    : Sensor(sensName, numVariables, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, -1, measurementsToAverage,
             incCalcValues),
      _model(model),
      _modbusAddress(modbusAddress),
      _stream(&stream),
      _RS485EnablePin(enablePin) {
    setSecondaryPowerPin(powerPin2);
}
// Destructor
GroPointParent::~GroPointParent() {}


// The sensor installation location on the Mayfly
String GroPointParent::getSensorLocation(void) {
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress < 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


bool GroPointParent::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit
    if (_RS485EnablePin >= 0) { pinMode(_RS485EnablePin, OUTPUT); }
#ifdef MS_GROPOINTPARENT_DEBUG_DEEP
    _gsensor.setDebugStream(&MS_SERIAL_OUTPUT);
#endif

    // This sensor begin is just setting more pin modes, etc, no sensor power
    // required This really can't fail so adding the return value is just for
    // show
    retVal &= _gsensor.begin(_model, _modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// The function to wake up a sensor
// Different from the standard in that it waits for warm up and starts
// measurements
bool GroPointParent::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    // Reset enable pin because pins are set to tri-state on sleep
    if (_RS485EnablePin >= 0) { pinMode(_RS485EnablePin, OUTPUT); }

    // Send the command to begin taking readings, trying up to 5 times
    bool    success = false;
    uint8_t ntries  = 0;
    MS_DBG(F("Start Measurement on"), getSensorNameAndLocation());
    while (!success && ntries < 5) {
        MS_DBG('(', ntries + 1, F("):"));
        success = _gsensor.startMeasurement();
        ntries++;
    }

    if (success) {
        // Update the time that the sensor was activated
        _millisSensorActivated = millis();
        MS_DBG(getSensorNameAndLocation(), F("activated and measuring."));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("was NOT activated!"));
        // Set the status error bit (bit 7)
        setStatusBit(ERROR_OCCURRED);
        // Make sure the activation time is zero and the wake success bit (bit
        // 4) is unset
        _millisSensorActivated = 0;
        clearStatusBit(WAKE_SUCCESSFUL);
    }

    return success;
}


// The function to put the sensor to sleep
// Different from the standard in that it stops measurements and empties and
// flushes the stream.
bool GroPointParent::sleep(void) {
    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();

    // if it's not powered, it's asleep
    if (!checkPowerOn()) { return true; }
    // if it was never awake, it's probably asleep
    if (_millisSensorActivated == 0) {
        MS_DBG(getSensorNameAndLocation(), F("was not measuring!"));
        return true;
    }

    // Send the command to begin taking readings, trying up to 5 times
    bool    success = false;
    uint8_t ntries  = 0;
    MS_DBG(F("Stop Measurement on"), getSensorNameAndLocation());
    while (!success && ntries < 5) {
        MS_DBG('(', ntries + 1, F("):"));
        success = _gsensor.stopMeasurement();
        ntries++;
    }
    if (success) {
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor activation (bits 3 & 4) and
        // measurement request (bits 5 & 6)
        clearStatusBits(WAKE_ATTEMPTED, WAKE_SUCCESSFUL, MEASUREMENT_ATTEMPTED,
                        MEASUREMENT_SUCCESSFUL);
        MS_DBG(F("Measurements stopped."));
    } else {
        MS_DBG(F("Measurements NOT stopped!"));
    }

    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();

    return success;
}


bool GroPointParent::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool success  = false;
    bool successT = false;
    // Initialize moisture variables for each probe segment
    float M1 = -9999, M2 = -9999, M3 = -9999, M4 = -9999, M5 = -9999,
          M6 = -9999, M7 = -9999, M8 = -9999;
    // Initialize temperature variables for each probe sensor
    float T1 = -9999, T2 = -9999, T3 = -9999, T4 = -9999, T5 = -9999,
          T6 = -9999, T7 = -9999, T8 = -9999, T9 = -9999, T10 = -9999,
          T11 = -9999, T12 = -9999, T13 = -9999;

    switch (_model) {
        case GPLP8: {
            // Get Moisture Values
            MS_DBG(F("Get Values from"), getSensorNameAndLocation());
            success = _gsensor.getValues(M1, M2, M3, M4, M5, M6, M7, M8);

            MS_DBG(F("    "), _gsensor.getParameter());
            MS_DBG(F("    "), _gsensor.getUnits());
            MS_DBG(F("    "), M1, ',', M2, ',', M3, ',', M4, ',', M5, ',', M6,
                   ',', M7, ',', M8);

            // Get Temperature Values
            successT = _gsensor.getTemperatureValues(
                T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13);

            MS_DBG(F("    "), _gsensor.getParameter1());
            MS_DBG(F("    "), _gsensor.getUnits1());
            MS_DBG(F("    "), T1, ',', T2, ',', T3, ',', T4, ',', T5, ',', T6,
                   ',', T7, ',', T8, ',', T9, ',', T10, ',', T11, ',', T12, ',',
                   T13);

            break;
        }
        default: {
            // Get Values
            MS_DBG(F("Other GroPoint models not yet implemented."));
        }
    }

    if (success) {
        // Put values into the array
        verifyAndAddMeasurementResult(0, M1);
        verifyAndAddMeasurementResult(1, M2);
        verifyAndAddMeasurementResult(2, M3);
        verifyAndAddMeasurementResult(3, M4);
        verifyAndAddMeasurementResult(4, M5);
        verifyAndAddMeasurementResult(5, M6);
        verifyAndAddMeasurementResult(6, M7);
        verifyAndAddMeasurementResult(7, M8);
    }
    if (successT) {
        verifyAndAddMeasurementResult(8, T1);
        verifyAndAddMeasurementResult(9, T2);
        verifyAndAddMeasurementResult(10, T3);
        verifyAndAddMeasurementResult(11, T4);
        verifyAndAddMeasurementResult(12, T5);
        verifyAndAddMeasurementResult(13, T6);
        verifyAndAddMeasurementResult(14, T7);
        verifyAndAddMeasurementResult(15, T8);
        verifyAndAddMeasurementResult(16, T9);
        verifyAndAddMeasurementResult(17, T10);
        verifyAndAddMeasurementResult(18, T11);
        verifyAndAddMeasurementResult(19, T12);
        verifyAndAddMeasurementResult(20, T13);
    }

    // Return success value when finished.  Success requires both the moisture
    // and temperature values to be successfully retrieved
    return bumpMeasurementAttemptCount((success && successT));
}

// cSpell:ignore gsensor
