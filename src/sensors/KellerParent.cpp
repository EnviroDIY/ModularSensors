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
/**
 * @brief Construct a KellerParent sensor with Modbus communication and timing/power configuration.
 *
 * @param modbusAddress Modbus slave address of the Keller sensor.
 * @param stream Pointer to the Stream used for Modbus communication (e.g., Serial).
 * @param powerPin Primary power-control pin for the sensor.
 * @param powerPin2 Secondary power-control pin (configured via setSecondaryPowerPin).
 * @param enablePin RS-485 driver enable/control pin; use a negative value if unused.
 * @param measurementsToAverage Number of consecutive readings to average per measurement.
 * @param model Keller sensor model identifier.
 * @param sensName Human-readable sensor name passed to the base Sensor.
 * @param numVariables Number of sensor data variables exposed by this sensor instance.
 * @param warmUpTime_ms Time in milliseconds allocated for sensor warm-up before measurements.
 * @param stabilizationTime_ms Time in milliseconds to wait for readings to stabilize.
 * @param measurementTime_ms Time in milliseconds allocated to perform a single measurement.
 */
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
/**
 * @brief Construct a KellerParent sensor instance configured for Modbus over a provided stream.
 *
 * Initializes the base Sensor with timing, power and averaging settings, stores the Keller model,
 * Modbus address, stream reference, and RS485 enable pin, and configures the secondary power pin.
 *
 * @param modbusAddress Modbus device address (1 byte).
 * @param stream Reference to a Stream used for Modbus communication.
 * @param powerPin Primary power control pin (or -1 if not used).
 * @param powerPin2 Secondary power control pin (or -1 if not used); configured via setSecondaryPowerPin.
 * @param enablePin Digital pin used to control RS-485 transceiver enable (or -1 if not used).
 * @param measurementsToAverage Number of measurements to average per reported reading.
 * @param model Keller device model (kellerModel enum) used to initialize protocol/behavior.
 * @param sensName Human-readable sensor name passed to the base Sensor.
 * @param numVariables Number of sensor variables the instance will register/report.
 * @param warmUpTime_ms Warm-up duration in milliseconds before first measurement.
 * @param stabilizationTime_ms Stabilization time in milliseconds between power-up and valid reading.
 * @param measurementTime_ms Measurement duration in milliseconds to collect a single reading.
 */
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
/**
 * @brief Destroys the KellerParent instance.
 *
 * Performs class teardown; no additional cleanup is required on destruction.
 */
KellerParent::~KellerParent() {}


// The sensor installation location on the Mayfly
String KellerParent::getSensorLocation(void) {
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress < 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


/**
 * @brief Initialize the Keller sensor subsystem and configure the RS485 enable pin.
 *
 * Performs the class-level setup, configures the RS485 enable pin (if provided),
 * and initializes the underlying Keller communication wrapper.
 *
 * @return `true` if both the base sensor setup and the Keller sensor initialization succeeded, `false` otherwise.
 */
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
/**
 * @brief Puts the sensor into sleep mode after clearing and flushing the associated Stream.
 *
 * Discards any pending data in the attached Stream's input buffer and flushes the stream output before putting the sensor into its low-power state.
 *
 * @return bool `true` if the sensor entered sleep mode, `false` otherwise.
 */
bool KellerParent::sleep(void) {
    // empty then flush the buffer
    while (_stream->available()) { _stream->read(); }
    _stream->flush();
    return Sensor::sleep();
};


/**
 * @brief Attempt a single sensor measurement, compute derived values, and store results if valid.
 *
 * Queries the Keller sensor for pressure and temperature, computes water depth and pressure in millibar,
 * and stores those three measurements when all are valid. If the measurement was not started (status bit
 * MEASUREMENT_SUCCESSFUL not set) the function records a failed attempt and returns immediately.
 *
 * @returns `true` if the measurement values were valid and stored, `false` otherwise.
 */
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