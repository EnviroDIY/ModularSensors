/**
 * @file GroPointParent.cpp
 * @copyright 2017-2023 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Implements the GroPointParent class.
 */

#include "GroPointParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for
// data, and number of readings to average
GroPointParent::GroPointParent(
    byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2,
    int8_t enablePin, uint8_t measurementsToAverage, gropointModel model,
    const char* sensName, uint8_t numVariables, uint32_t warmUpTime_ms,
    uint32_t stabilizationTime_ms, uint32_t measurementTime_ms,
    uint8_t incCalcValues)
    : Sensor(sensName, numVariables, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, -1, measurementsToAverage,
             incCalcValues),
            _model(model),
            _modbusAddress(modbusAddress),
            _stream(stream),
            _RS485EnablePin(enablePin),
            _powerPin2(powerPin2) {}
GroPointParent::GroPointParent(
    byte modbusAddress, Stream& stream, int8_t powerPin, int8_t powerPin2,
    int8_t enablePin, uint8_t measurementsToAverage, gropointModel model,
    const char* sensName, uint8_t numVariables, uint32_t warmUpTime_ms,
    uint32_t stabilizationTime_ms, uint32_t measurementTime_ms,
    uint8_t incCalcValues)
    : Sensor(sensName, numVariables, warmUpTime_ms, stabilizationTime_ms,
             measurementTime_ms, powerPin, -1, measurementsToAverage,
             incCalcValues),
            _model(model),
            _modbusAddress(modbusAddress),
            _stream(&stream),
            _RS485EnablePin(enablePin),
            _powerPin2(powerPin2) {}
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
    if (_RS485EnablePin >= 0) pinMode(_RS485EnablePin, OUTPUT);
    if (_powerPin2 >= 0) pinMode(_powerPin2, OUTPUT);

#ifdef MS_GROPOINTPARENT_DEBUG_DEEP
    _gsensor.setDebugStream(&DEEP_DEBUGGING_SERIAL_OUTPUT);
#endif

    // This sensor begin is just setting more pin modes, etc, no sensor power
    // required This realy can't fail so adding the return value is just for
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
        // Make sure the activation time is zero and the wake success bit (bit
        // 4) is unset
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
    }

    return success;
}


// The function to put the sensor to sleep
// Different from the standard in that it stops measurements
bool GroPointParent::sleep(void) {
    if (!checkPowerOn()) { return true; }
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
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Measurements stopped."));
    } else {
        MS_DBG(F("Measurements NOT stopped!"));
    }

    return success;
}


// This turns on sensor power
void GroPointParent::powerUp(void) {
    if (_powerPin >= 0) {
        MS_DBG(F("Powering"), getSensorNameAndLocation(), F("with pin"),
               _powerPin);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    if (_powerPin2 >= 0) {
        MS_DBG(F("Applying secondary power to"), getSensorNameAndLocation(),
               F("with pin"), _powerPin2);
        digitalWrite(_powerPin2, HIGH);
    }
    if (_powerPin < 0 && _powerPin2 < 0) {
        MS_DBG(F("Power to"), getSensorNameAndLocation(),
               F("is not controlled by this library."));
    }
    // Set the status bit for sensor power attempt (bit 1) and success (bit 2)
    _sensorStatus |= 0b00000110;
}


// This turns off sensor power
void GroPointParent::powerDown(void) {
    if (_powerPin >= 0) {
        MS_DBG(F("Turning off power to"), getSensorNameAndLocation(),
               F("with pin"), _powerPin);
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor power (bits 1 & 2),
        // activation (bits 3 & 4), and measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000001;
    }
    if (_powerPin2 >= 0) {
        MS_DBG(F("Turning off secondary power to"), getSensorNameAndLocation(),
               F("with pin"), _powerPin2);
        digitalWrite(_powerPin2, LOW);
    }
    if (_powerPin < 0 && _powerPin2 < 0) {
        MS_DBG(F("Power to"), getSensorNameAndLocation(),
               F("is not controlled by this library."));
        // Do NOT unset any status bits or timestamps if we didn't really power
        // down!
    }
}


bool GroPointParent::addSingleMeasurementResult(void) {
    bool success = false;
    // Initialize float variables
    float valueM1 = -9999;
    float valueM2 = -9999;
    float valueM3 = -9999;
    float valueM4 = -9999;
    float valueM5 = -9999;
    float valueM6 = -9999;
    float valueM7 = -9999;
    float valueM8 = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        switch (_model) {
            case GPLP8: {
                // Get Values
                MS_DBG(F("Get Values from"), getSensorNameAndLocation());
                success = _gsensor.getValues(valueM1, valueM2, valueM3,
                                             valueM4, valueM5, valueM6, 
                                             valueM7, valueM8);

                // Fix not-a-number values
                if (!success || isnan(valueM1)) valueM1 = -9999;
                if (!success || isnan(valueM2)) valueM2 = -9999;
                if (!success || isnan(valueM3)) valueM3 = -9999;
                if (!success || isnan(valueM4)) valueM4 = -9999;
                if (!success || isnan(valueM5)) valueM5 = -9999;
                if (!success || isnan(valueM6)) valueM6 = -9999;
                if (!success || isnan(valueM7)) valueM7 = -9999;
                if (!success || isnan(valueM8)) valueM8 = -9999;

                MS_DBG(F("    "), _gsensor.getParameter());
                MS_DBG(F("    "), valueM1, ',', valueM2, ',', valueM3, ',',
                    valueM4, ',', valueM5, ',', valueM6, ',', valueM7, ',', 
                    valueM8);

                // Put values into the array
                verifyAndAddMeasurementResult(0, valueM1);
                verifyAndAddMeasurementResult(1, valueM2);
                verifyAndAddMeasurementResult(2, valueM3);
                verifyAndAddMeasurementResult(3, valueM4);
                verifyAndAddMeasurementResult(4, valueM5);
                verifyAndAddMeasurementResult(5, valueM6);
                verifyAndAddMeasurementResult(6, valueM7);
                verifyAndAddMeasurementResult(7, valueM8);

                break;
            }
            default: {
                // Get Values
                MS_DBG(F("Other GroPoint models not yet implemented."));
            }
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    // Return true when finished
    return success;
}
