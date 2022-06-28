/**
 * @file YosemitechParent.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the YosemitechParent class.
 */

#include "YosemitechParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for
// data, and number of readings to average
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
      _RS485EnablePin(enablePin),
      _powerPin2(powerPin2) {}
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
      _RS485EnablePin(enablePin),
      _powerPin2(powerPin2) {}
// Destructor
YosemitechParent::~YosemitechParent() {}


// The sensor installation location on the Mayfly
String YosemitechParent::getSensorLocation(void) {
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress < 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


bool YosemitechParent::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit
    if (_RS485EnablePin >= 0) pinMode(_RS485EnablePin, OUTPUT);
    if (_powerPin2 >= 0) pinMode(_powerPin2, OUTPUT);

#ifdef MS_YOSEMITECHPARENT_DEBUG_DEEP
    _ysensor.setDebugStream(&DEEP_DEBUGGING_SERIAL_OUTPUT);
#endif

    // This sensor begin is just setting more pin modes, etc, no sensor power
    // required This realy can't fail so adding the return value is just for
    // show
    retVal &= _ysensor.begin(_model, _modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// The function to wake up a sensor
// Different from the standard in that it waits for warm up and starts
// measurements
bool YosemitechParent::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

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
        // Make sure the activation time is zero and the wake success bit (bit
        // 4) is unset
        _millisSensorActivated = 0;
        _sensorStatus &= 0b11101111;
    }

    // Manually activate the brush
    // Needed for newer sensors that do not immediate activate on getting power
    if (_model == Y511 || _model == Y514 || _model == Y551 || _model == Y560 ||
        _model == Y4000) {
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
// Different from the standard in that it stops measurements
bool YosemitechParent::sleep(void) {
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
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Measurements stopped."));
    } else {
        MS_DBG(F("Measurements NOT stopped!"));
    }

    return success;
}


// This turns on sensor power
void YosemitechParent::powerUp(void) {
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
void YosemitechParent::powerDown(void) {
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


bool YosemitechParent::addSingleMeasurementResult(void) {
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
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
                success = _ysensor.getValues(DOmgL, Turbidity, Cond, pH, Temp,
                                             ORP, Chlorophyll, BGA);

                // Fix not-a-number values
                if (!success || isnan(DOmgL)) DOmgL = -9999;
                if (!success || isnan(Turbidity)) Turbidity = -9999;
                if (!success || isnan(Cond)) Cond = -9999;
                if (!success || isnan(pH)) pH = -9999;
                if (!success || isnan(Temp)) Temp = -9999;
                if (!success || isnan(ORP)) ORP = -9999;
                if (!success || isnan(Chlorophyll)) Chlorophyll = -9999;
                if (!success || isnan(BGA)) BGA = -9999;

                // For conductivity, convert mS/cm to µS/cm
                if (Cond != -9999) Cond *= 1000;

                MS_DBG(F("    "), _ysensor.getParameter());
                MS_DBG(F("    "), DOmgL, ',', Turbidity, ',', Cond, ',', pH,
                       ',', Temp, ',', ORP, ',', Chlorophyll, ',', BGA);

                // Put values into the array
                verifyAndAddMeasurementResult(0, DOmgL);
                verifyAndAddMeasurementResult(1, Turbidity);
                verifyAndAddMeasurementResult(2, Cond);
                verifyAndAddMeasurementResult(3, pH);
                verifyAndAddMeasurementResult(4, Temp);
                verifyAndAddMeasurementResult(5, ORP);
                verifyAndAddMeasurementResult(6, Chlorophyll);
                verifyAndAddMeasurementResult(7, BGA);

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

                // Fix not-a-number values
                if (!success || isnan(parmValue)) parmValue = -9999;
                if (!success || isnan(tempValue)) tempValue = -9999;
                if (!success || isnan(thirdValue)) thirdValue = -9999;

                // For conductivity, convert mS/cm to µS/cm
                if (_model == Y520 && parmValue != -9999) parmValue *= 1000;

                MS_DBG(F(" "), _ysensor.getParameter(), ':', parmValue);
                MS_DBG(F("  Temp:"), tempValue);

                // Not all sensors return a third value
                if (_numReturnedValues > 2) {
                    MS_DBG(F("  Third:"), thirdValue);
                }

                // Put values into the array
                verifyAndAddMeasurementResult(0, parmValue);
                verifyAndAddMeasurementResult(1, tempValue);
                verifyAndAddMeasurementResult(2, thirdValue);
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
