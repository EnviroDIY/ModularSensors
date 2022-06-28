/**
 * @file AtlasParent.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the AtlasParent class.
 */

#include "AtlasParent.h"
#include <Wire.h>


// The constructors
AtlasParent::AtlasParent(TwoWire* theI2C, int8_t powerPin,
                         uint8_t i2cAddressHex, uint8_t measurementsToAverage,
                         const char*   sensorName,
                         const uint8_t totalReturnedValues,
                         uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms,
                         uint32_t measurementTime_ms, uint8_t incCalcValues)
    : Sensor(sensorName, totalReturnedValues, warmUpTime_ms,
             stabilizationTime_ms, measurementTime_ms, powerPin, -1,
             measurementsToAverage, incCalcValues),
      _i2cAddressHex(i2cAddressHex),
      _i2c(theI2C) {}
AtlasParent::AtlasParent(int8_t powerPin, uint8_t i2cAddressHex,
                         uint8_t measurementsToAverage, const char* sensorName,
                         const uint8_t totalReturnedValues,
                         uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms,
                         uint32_t measurementTime_ms, uint8_t incCalcValues)
    : Sensor(sensorName, totalReturnedValues, warmUpTime_ms,
             stabilizationTime_ms, measurementTime_ms, powerPin, -1,
             measurementsToAverage, incCalcValues),
      _i2cAddressHex(i2cAddressHex),
      _i2c(&Wire) {}
// Destructors
AtlasParent::~AtlasParent() {}


String AtlasParent::getSensorLocation(void) {
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool AtlasParent::setup(void) {
    _i2c->begin();  // Start the wire library (sensor power not required)
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    _i2c->setTimeout(0);
    return Sensor::setup();  // this will set pin modes and the setup status bit
}


// The function to put the sensor to sleep
// The Atlas sensors must be told to sleep
bool AtlasParent::sleep(void) {
    if (!checkPowerOn()) { return true; }
    if (_millisSensorActivated == 0) {
        MS_DBG(getSensorNameAndLocation(), F("was not measuring!"));
        return true;
    }

    bool success = true;
    MS_DBG(F("Putting"), getSensorNameAndLocation(), F("to sleep"));

    _i2c->beginTransmission(_i2cAddressHex);
    success &= static_cast<bool>(
        _i2c->write((const uint8_t*)"Sleep",
                    5));  // Write "Sleep" to put it in low power mode
    success &= !static_cast<bool>(_i2c->endTransmission());
    // NOTE: The return of 0 from endTransmission indicates success

    if (success) {
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor activation (bits 3 & 4) and
        // measurement request (bits 5 & 6)
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Done"));
    } else {
        MS_DBG(getSensorNameAndLocation(), F("did not accept sleep command"));
    }

    return success;
}


// To start a measurement we write the command "R" to the sensor
// NOTE:  documentation says to use a capital "R" but the examples provided
// by Atlas use a lower case "r".
bool AtlasParent::startSingleMeasurement(void) {
    // Sensor::startSingleMeasurement() checks that if it's awake/active and
    // sets the timestamp and status bits.  If it returns false, there's no
    // reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    bool success = true;
    MS_DBG(F("Starting measurement on"), getSensorNameAndLocation());

    _i2c->beginTransmission(_i2cAddressHex);
    // Write "R" to start a reading
    success &= static_cast<bool>(_i2c->write('r'));
    int I2Cstatus = _i2c->endTransmission();
    MS_DBG(F("I2Cstatus:"), I2Cstatus);
    success &= !I2Cstatus;
    // NOTE: The return of 0 from endTransmission indicates success

    if (success) {
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
    } else {
        // Otherwise, make sure that the measurement start time and success bit
        // (bit 6) are unset
        MS_DBG(getSensorNameAndLocation(),
               F("did not successfully start a measurement."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
    }

    return success;
}


bool AtlasParent::addSingleMeasurementResult(void) {
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        // call the circuit and request 40 bytes (this may be more than we need)
        _i2c->requestFrom(static_cast<int>(_i2cAddressHex), 40, 1);
        // the first byte is the response code, we read this separately.
        int code = _i2c->read();

        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
        // Parse the response code
        switch (code) {
            case 1:  // the command was successful.
                MS_DBG(F("  Measurement successful"));
                success = true;
                break;

            case 2:  // the command has failed.
                MS_DBG(F("  Measurement Failed"));
                break;

            case 254:  // the command has not yet been finished calculating.
                MS_DBG(F("  Measurement Pending"));
                break;

            case 255:  // there is no further data to send.
                MS_DBG(F("  No Data"));
                break;

            default: break;
        }
        // If the response code is successful, parse the remaining results
        if (success) {
            for (uint8_t i = 0; i < _numReturnedValues; i++) {
                float result = _i2c->parseFloat();
                if (isnan(result)) { result = -9999; }
                if (result < -1020) { result = -9999; }
                MS_DBG(F("  Result #"), i, ':', result);
                verifyAndAddMeasurementResult(i, result);
            }
        }
    } else {
        // If there's no measurement, need to make sure we send over all
        // of the "failed" result values
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
        for (uint8_t i = 0; i < _numReturnedValues; i++) {
            verifyAndAddMeasurementResult(i, static_cast<float>(-9999));
        }
    }

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}


// Wait for a command to process
// NOTE:  This should ONLY be used as a wait when no response is
// expected except a status code - the response will be "consumed"
// and become unavailable.
bool AtlasParent::waitForProcessing(uint32_t timeout) {
    // Wait for the command to have been processed and implented
    bool     processed = false;
    uint32_t start     = millis();
    while (!processed && millis() - start < timeout) {
        _i2c->requestFrom(static_cast<int>(_i2cAddressHex), 1, 1);
        auto code = static_cast<uint8_t>(_i2c->read());
        if (code == 1) processed = true;
    }
    return processed;
}
