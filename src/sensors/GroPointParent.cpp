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
/**
 * @brief Construct a GroPointParent sensor configured for a Modbus/RS-485 GroPoint device.
 *
 * Initializes base Sensor timing, power, and averaging configuration and stores
 * device-specific settings (Modbus address, stream, RS-485 enable pin and model).
 *
 * @param modbusAddress Modbus slave address of the GroPoint device.
 * @param stream Pointer to the Stream used for Modbus/RS-485 communication.
 * @param powerPin Primary power-control pin for the sensor.
 * @param powerPin2 Secondary power-control pin (e.g., for a separate power rail or enable line).
 * @param enablePin GPIO used to toggle the RS-485 transceiver DE/RE (driver/receiver enable).
 * @param measurementsToAverage Number of readings to average per reported measurement.
 * @param model Specific GroPoint model variant handled by this instance.
 * @param sensName Human-readable sensor name passed to the base Sensor.
 * @param numVariables Number of measurement variables exposed by this sensor.
 * @param warmUpTime_ms Warm-up time in milliseconds before measurements may be requested.
 * @param stabilizationTime_ms Stabilization delay in milliseconds between starting and reading measurements.
 * @param measurementTime_ms Suggested measurement duration in milliseconds.
 * @param incCalcValues Number of calculated/derived values to include in reported output.
 */
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
/**
 * @brief Constructs a GroPointParent sensor configured for a specific GroPoint model over Modbus/RS-485.
 *
 * Initializes the base Sensor with timing, averaging, and power parameters and stores Modbus/stream configuration
 * needed to communicate with the GroPoint device. Also sets the secondary power pin via setSecondaryPowerPin()
 * and records the RS-485 enable pin.
 *
 * @param modbusAddress Modbus slave address of the GroPoint device.
 * @param stream Stream used for Modbus/RS-485 communication with the device.
 * @param powerPin Primary power control pin for the sensor.
 * @param powerPin2 Secondary power/control pin (configured via setSecondaryPowerPin()).
 * @param enablePin Digital pin used to enable the RS-485 transceiver.
 * @param measurementsToAverage Number of raw measurements to average before producing a reported value.
 * @param model Specific GroPoint model variant to drive (selects supported channels/behavior).
 * @param sensName Human-readable sensor name passed to the base Sensor.
 * @param numVariables Number of reported variables managed by this sensor instance.
 * @param warmUpTime_ms Warm-up time in milliseconds before measurements are valid.
 * @param stabilizationTime_ms Stabilization time in milliseconds between power-up and measurement.
 * @param measurementTime_ms Measurement duration/timeout in milliseconds.
 * @param incCalcValues Number of internal calculated values to include in reported output.
 */
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
/**
 * @brief Cleans up resources used by the GroPointParent instance.
 *
 * Performs no special actions beyond normal object destruction.
 */
GroPointParent::~GroPointParent() {}


// The sensor installation location on the Mayfly
String GroPointParent::getSensorLocation(void) {
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress < 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


/**
 * @brief Configure hardware pins and initialize the GroPoint sensor handler.
 *
 * Sets up base Sensor state and pin modes, configures the RS-485 enable pin as OUTPUT
 * when provided, and initializes the internal GroPoint handler with model, Modbus
 * address, stream, and RS-485 enable pin.
 *
 * @return true if the base Sensor setup and GroPoint handler initialization both succeeded, false otherwise.
 */
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
/**
 * @brief Attempts to wake the sensor and start measurements on the GroPoint device.
 *
 * Ensures the sensor power is active and configures the RS-485 enable pin if present,
 * then attempts up to five times to start measurements on the attached GroPoint sensor.
 * On success, records the sensor activation timestamp. On failure, sets the ERROR_OCCURRED
 * status bit, clears the activation timestamp, and clears the WAKE_SUCCESSFUL status bit.
 *
 * @return true if the sensor began measuring, false otherwise.
 */
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
/**
 * @brief Stops active measurements, clears activation state, and flushes the Modbus stream.
 *
 * Attempts to stop ongoing measurements on the GroPoint device (retrying up to several times),
 * empties and flushes the associated Stream buffers before and after the command, and when successful
 * clears activation/measurement timestamps and related status bits.
 *
 * @return bool `true` if the sensor was already asleep or measurements were successfully stopped, `false` otherwise.
 */
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


/**
 * Acquire a single measurement from the GroPoint sensor and store the results when successful.
 *
 * If a measurement was not started, the function records a failed attempt and returns immediately.
 * For the GPLP8 model, it reads eight moisture values and thirteen temperature values and stores
 * them into the measurement result array at indices 0–7 (moisture M1–M8) and 8–20 (temperature T1–T13).
 * The function updates internal attempt bookkeeping before returning.
 *
 * @return `true` if both moisture and temperature reads succeeded (after attempt bookkeeping), `false` otherwise.
 */
bool GroPointParent::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool success  = false;
    bool successT = false;
    // Initialize moisture variables for each probe segment
    float M1, M2, M3, M4, M5, M6, M7, M8 = -9999;
    // Initialize temperature variables for each probe sensor
    float T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13 = -9999;

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

    if (success && successT) {
        // Put values into the array
        verifyAndAddMeasurementResult(0, M1);
        verifyAndAddMeasurementResult(1, M2);
        verifyAndAddMeasurementResult(2, M3);
        verifyAndAddMeasurementResult(3, M4);
        verifyAndAddMeasurementResult(4, M5);
        verifyAndAddMeasurementResult(5, M6);
        verifyAndAddMeasurementResult(6, M7);
        verifyAndAddMeasurementResult(7, M8);

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

    // Return success value when finished
    return bumpMeasurementAttemptCount((success && successT));
}

// cSpell:ignore gsensor