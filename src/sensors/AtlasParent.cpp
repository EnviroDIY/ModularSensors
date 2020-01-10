/*
 * AtlasParent.cpp
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * Most I2C commands have a 300ms processing time from the time the command is
 * written until it is possible to request a response or result, except for the
 * commands to take a calibration point or a reading which have a 600ms
 * processing/response time.
 *
*/

#include "AtlasParent.h"
#include <Wire.h>


// The constructor - because this is I2C, only need the power pin
// This sensor has a set I2C address of 0X64, or 100
AtlasParent::AtlasParent(int8_t powerPin, uint8_t i2cAddressHex, uint8_t measurementsToAverage,
                         const char *sensorName, const uint8_t numReturnedVars,
                         uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensorName, numReturnedVars,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, -1, measurementsToAverage),
      _i2cAddressHex(i2cAddressHex)
{}
AtlasParent::~AtlasParent() {}


String AtlasParent::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool AtlasParent::setup(void)
{
    Wire.begin();  // Start the wire library (sensor power not required)
    // Eliminate any potential extra waits in the wire library
    // These waits would be caused by a readBytes or parseX being called
    // on wire after the Wire buffer has emptied.  The default stream
    // functions - used by wire - wait a timeout period after reading the
    // end of the buffer to see if an interrupt puts something into the
    // buffer.  In the case of the Wire library, that will never happen and
    // the timeout period is a useless delay.
    Wire.setTimeout(0);
    return Sensor::setup();  // this will set pin modes and the setup status bit
}


// The function to put the sensor to sleep
// The Atlas sensors must be told to sleep
bool AtlasParent::sleep(void)
{
    if (!checkPowerOn()) return true;
    if (_millisSensorActivated == 0)
    {
        MS_DBG(getSensorNameAndLocation(), F("was not measuring!"));
        return true;
    }

    bool success = true;
    MS_DBG(F("Putting"), getSensorNameAndLocation(), F("to sleep"));

    Wire.beginTransmission(_i2cAddressHex);
    success &= Wire.write((const uint8_t *)"Sleep", 5);  // Write "Sleep" to put it in low power mode
    success &= !Wire.endTransmission();
    // NOTE: The return of 0 from endTransmission indicates success

    if (success)
    {
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the measurement request time
        _millisMeasurementRequested = 0;
        // Unset the status bits for sensor activation (bits 3 & 4) and measurement
        // request (bits 5 & 6)
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Done"));
    }
    else
    {
        MS_DBG(getSensorNameAndLocation(), F("did not accept sleep command"));
    }

    return success;
}


// To start a measurement we write the command "R" to the sensor
// NOTE:  documentation says to use a capital "R" but the examples provided
// by Atlas use a lower case "r".
bool AtlasParent::startSingleMeasurement(void)
{
    // Sensor::startSingleMeasurement() checks that if it's awake/active and sets
    // the timestamp and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::startSingleMeasurement()) return false;

    bool success = true;
    MS_DBG(F("Starting measurement on"), getSensorNameAndLocation());

    Wire.beginTransmission(_i2cAddressHex);
    success &= Wire.write('r');  // Write "R" to start a reading
    int I2Cstatus = Wire.endTransmission();
    MS_DBG(F("I2Cstatus:"), I2Cstatus);
    success &= !I2Cstatus;
    // NOTE: The return of 0 from endTransmission indicates success

    if (success)
    {
        // Update the time that a measurement was requested
        _millisMeasurementRequested = millis();
    }
    // Otherwise, make sure that the measurement start time and success bit (bit 6) are unset
    else
    {
        MS_DBG(getSensorNameAndLocation(), F("did not successfully start a measurement."));
        _millisMeasurementRequested = 0;
        _sensorStatus &= 0b10111111;
    }

    return success;
}


bool AtlasParent::addSingleMeasurementResult(void)
{
    bool success = false;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        // call the circuit and request 40 bytes (this may be more than we need)
        Wire.requestFrom(_i2cAddressHex, 40, 1);
        // the first byte is the response code, we read this separately.
        uint8_t code = Wire.read();

        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
        // Parse the response code
        switch (code)
        {
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
        }
        // If the response code is successful, parse the remaining results
        if (success)
        {
            for (uint8_t i = 0; i < _numReturnedVars; i++)
            {
                float result = Wire.parseFloat();
                if (isnan(result)) result = -9999;
                if (result < -1020) result = -9999;
                MS_DBG(F("  Result #"), i, ':', result);
                verifyAndAddMeasurementResult(i, result);
            }
        }
    }
    else
    {
        // If there's no measurement, need to make sure we send over all
        // of the "failed" result values
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
        for (uint8_t i = 0; i < _numReturnedVars; i++)
        {
            verifyAndAddMeasurementResult(i, (float)-9999);
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
bool AtlasParent::waitForProcessing(uint32_t timeout)
{
    // Wait for the command to have been processed and implented
    bool processed = false;
    uint32_t start = millis();
    while (!processed && millis() - start < timeout)
    {
        Wire.requestFrom(_i2cAddressHex, 1, 1);
        uint8_t code = Wire.read();
        if (code == 1) processed = true;
    }
    return processed;
}
