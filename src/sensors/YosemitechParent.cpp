/**
 * @file YosemitechParent.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the YosemitechParent class.
 */

#include "YosemitechParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for
/**
 * @brief Construct a YosemitechParent sensor instance with communication, power, timing, and measurement configuration.
 *
 * @param modbusAddress Modbus address of the Yosemitech sensor.
 * @param stream Pointer to the Stream used for sensor communication (e.g., Serial).
 * @param powerPin Primary power control pin (GPIO), or -1 if not used.
 * @param powerPin2 Secondary power control pin (GPIO), or -1 if not used.
 * @param enablePin RS485 enable pin (GPIO), or -1 if not used.
 * @param measurementsToAverage Number of readings to average for each reported measurement.
 * @param model Specific Yosemitech sensor model identifier.
 * @param sensName Sensor name passed to the base Sensor class.
 * @param numVariables Number of reported variables for the sensor.
 * @param warmUpTime_ms Warm-up time in milliseconds before measurements begin.
 * @param stabilizationTime_ms Stabilization time in milliseconds after wake before measuring.
 * @param measurementTime_ms Measurement duration in milliseconds.
 * @param incCalcValues Number of incremental calculated values maintained by the base Sensor.
 */
YosemitechParent::YosemitechParent(
    byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2,
    int8_t enablePin, uint8_t measurementsToAverage, yosemitechModel model,
    const char* sensName, uint8_t numVariables, uint32_t warmUpTime_ms,
    uint32_t stabilizationTime_ms, uint32_t measurementTime_ms,
    uint8_t incCalcValues)
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
 * @brief Construct a YosemitechParent sensor wrapper with communication and power configuration.
 *
 * Initializes the YosemitechParent instance with the sensor model and Modbus address, associates
 * the communication stream and RS-485 enable pin, configures the base Sensor with timing, power,
 * and averaging parameters, and assigns the secondary power pin.
 *
 * @param modbusAddress Modbus slave address for the sensor.
 * @param stream Reference to the Stream used for sensor communication.
 * @param powerPin Primary power control pin (passed to base Sensor).
 * @param powerPin2 Secondary power control pin managed by this class.
 * @param enablePin Digital pin used to enable the RS-485 transceiver.
 * @param measurementsToAverage Number of samples to average per reported measurement.
 * @param model Yosemitech sensor model identifier.
 * @param sensName Human-readable sensor name (passed to base Sensor).
 * @param numVariables Number of variables the sensor provides (passed to base Sensor).
 * @param warmUpTime_ms Time in milliseconds for sensor warm-up (passed to base Sensor).
 * @param stabilizationTime_ms Time in milliseconds to wait for sensor stabilization (passed to base Sensor).
 * @param measurementTime_ms Measurement duration in milliseconds (passed to base Sensor).
 * @param incCalcValues Number of incremental/calculated values (passed to base Sensor).
 */
YosemitechParent::YosemitechParent(
    byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2,
    int8_t enablePin, uint8_t measurementsToAverage, yosemitechModel model,
    const char* sensName, uint8_t numVariables, uint32_t warmUpTime_ms,
    uint32_t stabilizationTime_ms, uint32_t measurementTime_ms,
    uint8_t incCalcValues)
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
 * @brief Default destructor.
 *
 * No special cleanup or resource release is performed. The compiler-generated
 * behavior is sufficient for this class.
 */
YosemitechParent::~YosemitechParent() {}


// The sensor installation location on the Mayfly
String YosemitechParent::getSensorLocation(void) {
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress < 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


/**
 * @brief Configure hardware pins and initialize the Yosemitech sensor instance.
 *
 * Configures the RS485 enable pin (if valid) as an output, calls the base Sensor setup,
 * and initializes the internal Yosemitech sensor object with the configured model,
 * Modbus address, and communication stream.
 *
 * @return true if both base setup and the Yosemitech sensor initialization report success, `false` otherwise.
 */
bool YosemitechParent::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit
    if (_RS485EnablePin >= 0) { pinMode(_RS485EnablePin, OUTPUT); }

#ifdef MS_YOSEMITECHPARENT_DEBUG_DEEP
    _ysensor.setDebugStream(&MS_SERIAL_OUTPUT);
#endif

    // This sensor begin is just setting more pin modes, etc, no sensor power
    // required This really can't fail so adding the return value is just for
    // show
    retVal &= _ysensor.begin(_model, _modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// The function to wake up a sensor
// Different from the standard in that it waits for warm up and starts
/**
 * @brief Power-activates the Yosemitech sensor and begins measurements.
 *
 * Configures the RS485 enable pin as OUTPUT (if valid), attempts to start
 * measurements up to five times, and records activation state and timestamp
 * on success. On failure, sets the ERROR_OCCURRED status bit, clears the
 * WAKE_SUCCESSFUL status bit, and resets the activation timestamp. For
 * certain models (Y511, Y513, Y514, Y551, Y560, Y4000) this also attempts to
 * manually activate the sensor brush after starting measurements.
 *
 * @return true if the sensor successfully began measuring, `false` otherwise.
 */
bool YosemitechParent::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;
    if (_RS485EnablePin >= 0) { pinMode(_RS485EnablePin, OUTPUT); }

    // Send the command to begin taking readings, trying up to 5 times
    bool    success = false;
    uint8_t ntries  = 0;
    MS_DBG(F("Start Measurement on"), getSensorNameAndLocation());
    while (!success && ntries < 5) {
        MS_DBG('(', ntries + 1, F("):"));
        success = _ysensor.startMeasurement();
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

    // Manually activate the brush
    // Needed for newer sensors that do not immediate activate on getting power
    if (_model == Y511 || _model == Y513 || _model == Y514 || _model == Y551 ||
        _model == Y560 || _model == Y4000) {
        MS_DBG(F("Activate Brush on"), getSensorNameAndLocation());
        if (_ysensor.activateBrush()) {
            MS_DBG(F("Brush activated."));
        } else {
            MS_DBG(F("Brush NOT activated!"));
        }
    }

    return success;
}


// The function to put the sensor to sleep
// Different from the standard in that it empties and flushes the stream and
/**
 * @brief Stops active measurements and places the sensor back to sleep.
 *
 * Attempts to stop measurements (up to five attempts), flushes any pending
 * bytes from the communication stream before and after the stop attempts,
 * and, on success, clears the sensor activation and measurement request
 * timestamps as well as related status bits.
 *
 * If the sensor is already unpowered or was never activated, the function
 * treats it as already asleep and returns success.
 *
 * @return true if measurements were stopped or the sensor was already asleep,
 *         false if the stop operation failed.
 */
bool YosemitechParent::sleep(void) {
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
        success = _ysensor.stopMeasurement();
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
 * @brief Retrieve and record a single measurement from the Yosemitech sensor according to its model.
 *
 * Queries the underlying Yosemitech sensor for its reported values, converts conductivity from mS/cm to µS/cm for models that return conductivity in mS/cm (Y4000 and Y520), and stores the returned parameters into the measurement results array at the model-appropriate indices. If the measurement was not started successfully, this function updates the measurement attempt count as a failed attempt and returns. The final return value reflects the outcome after updating the measurement attempt counter.
 *
 * @return bool `true` if the measurement attempt is considered successful after updating the attempt count, `false` otherwise.
 */
bool YosemitechParent::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool success = false;

    switch (_model) {
        case Y4000: {
            // Initialize float variables
            float DOmgL       = -9999;
            float Turbidity   = -9999;
            float Cond        = -9999;
            float pH          = -9999;
            float Temp        = -9999;
            float ORP         = -9999;
            float Chlorophyll = -9999;
            float BGA         = -9999;

            // Get Values
            MS_DBG(F("Get Values from"), getSensorNameAndLocation());
            success = _ysensor.getValues(DOmgL, Turbidity, Cond, pH, Temp, ORP,
                                         Chlorophyll, BGA);

            // For conductivity, convert mS/cm to µS/cm
            if (success && !isnan(Cond)) Cond *= 1000;

            MS_DBG(F("    "), _ysensor.getParameter());
            MS_DBG(F("    "), DOmgL, ',', Turbidity, ',', Cond, ',', pH, ',',
                   Temp, ',', ORP, ',', Chlorophyll, ',', BGA);

            // NOTE: Success depends on getting values, not on them being valid
            // numbers!
            if (success) {
                // Put values into the array
                verifyAndAddMeasurementResult(0, DOmgL);
                verifyAndAddMeasurementResult(1, Turbidity);
                verifyAndAddMeasurementResult(2, Cond);
                verifyAndAddMeasurementResult(3, pH);
                verifyAndAddMeasurementResult(4, Temp);
                verifyAndAddMeasurementResult(5, ORP);
                verifyAndAddMeasurementResult(6, Chlorophyll);
                verifyAndAddMeasurementResult(7, BGA);
            }
            break;
        }
        default: {
            // Initialize float variables
            float parmValue  = -9999;
            float tempValue  = -9999;
            float thirdValue = -9999;

            // Get Values
            MS_DBG(F("Get Values from"), getSensorNameAndLocation());
            success = _ysensor.getValues(parmValue, tempValue, thirdValue);

            // For conductivity, convert mS/cm to µS/cm
            if (_model == Y520 && !isnan(parmValue)) parmValue *= 1000;

            MS_DBG(F(" "), _ysensor.getParameter(), ':', parmValue);
            MS_DBG(F("  Temp:"), tempValue);

            // Not all sensors return a third value
            if (_numReturnedValues > 2) { MS_DBG(F("  Third:"), thirdValue); }


            // NOTE: Success depends on getting values, not on them being valid
            // numbers!
            if (success) {
                // Put values into the array
                verifyAndAddMeasurementResult(0, parmValue);
                verifyAndAddMeasurementResult(1, tempValue);
                verifyAndAddMeasurementResult(2, thirdValue);
            }
            break;
        }
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}

// cSpell:ignore ysensor