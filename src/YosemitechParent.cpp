/*
 *Yosemitech.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Yosemitech sensors that communicate via Modbus and are
 *set up in the EnviroDIY Yosemitech library.
 *
 *Documentation for the Yosemitech Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY Yosemitech library at:
 * https://github.com/EnviroDIY/Yosemitech
*/

#include "YosemitechParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for data, and number of readings to average
YosemitechParent::YosemitechParent(byte modbusAddress, Stream* stream,
                                   int powerPin, int enablePin, int readingsToAverage,
                                   yosemitechModel model, String sensName, int numVariables,
                                   int warmUpTime_ms, int stabilizationTime_ms, int remeasurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, remeasurementTime_ms,
             powerPin, -1, readingsToAverage)
{
    _model = model;
    _modbusAddress = modbusAddress;
    _stream = stream;
    _RS485EnablePin = enablePin;
}
YosemitechParent::YosemitechParent(byte modbusAddress, Stream& stream,
                                   int powerPin, int enablePin, int readingsToAverage,
                                   yosemitechModel model, String sensName, int numVariables,
                                   int warmUpTime_ms, int stabilizationTime_ms, int remeasurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, remeasurementTime_ms,
             powerPin, -1, readingsToAverage)
{
    _model = model;
    _modbusAddress = modbusAddress;
    _stream = &stream;
    _RS485EnablePin = enablePin;
}


// The sensor installation location on the Mayfly
String YosemitechParent::getSensorLocation(void)
{
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress< 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


SENSOR_STATUS YosemitechParent::setup(void)
{
    if (_powerPin > 0) pinMode(_powerPin, OUTPUT);
    if (_RS485EnablePin > 0) pinMode(_RS485EnablePin, OUTPUT);

    #if defined(DEBUGGING_SERIAL_OUTPUT)
        sensor.setDebugStream(&DEBUGGING_SERIAL_OUTPUT);
    #endif

    bool isSet = sensor.begin(_model, _modbusAddress, _stream, _RS485EnablePin);

    if (isSet)
    {
        MS_DBG(F("Set up Yosemitech sensor at "), getSensorLocation(), F("\n"));
        return SENSOR_READY;
    }
    else return SENSOR_ERROR;
}


// The function to wake up a sensor
// Different from the standard in that it waits for warm up and starts measurements
bool YosemitechParent::wake(void)
{
    if(!checkPowerOn()){powerUp();}

    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Send the command to begin taking readings, trying up to 5 times
    bool success = false;
    int ntries = 0;
    while (!success && ntries < 10)
    {
        success = sensor.startMeasurement();
        ntries++;
    }
    _isTakingMeasurements = success;
    if(_isTakingMeasurements)
    {
        _millisMeasurementStarted = millis();
        MS_DBG(F("Measurements started.\n"));
    }

    return success;
}


// The function to put the sensor to sleep
// Different from the standard in that it stops measurements
bool YosemitechParent::sleep(void)
{
    if(!checkPowerOn()){return true;}

    // Send the command to begin taking readings, trying up to 5 times
    bool success = false;
    int ntries = 0;
    while (!success && ntries < 2)
    {
        success = sensor.stopMeasurement();
        ntries++;
    }
    _isTakingMeasurements = !success;
    if(!_isTakingMeasurements)
    {
        _millisMeasurementStarted = 0;
        MS_DBG(F("Measurements stopped.\n"));
    }

    powerDown();

    return success;
}


// nothing needs to happen to start an individual measurement
bool YosemitechParent::startSingleMeasurement(void)
{
    _lastMeasurementRequested = millis();
    return true;
}


bool YosemitechParent::addSingleMeasurementResult(void)
{

    if (_isTakingMeasurements)
    {
        // Make sure we've waited long enough for a new reading to be available
        waitForNextMeasurement();

        // Initialize float variables
        float parmValue, tempValue, thirdValue;
        // Get Values
        sensor.getValues(parmValue, tempValue, thirdValue);
        // Put values into the array
        // All sensors but pH and DO will have -9999 as the third value
        sensorValues[0] += parmValue;
        MS_DBG(F("Parm: "), parmValue, F("\n"));
        sensorValues[1] += tempValue;
        MS_DBG(F("Temp: "), tempValue, F("\n"));
        sensorValues[2] += thirdValue;
        MS_DBG(F("Third: "), thirdValue, F("\n"));
    }

    else MS_DBG(F("Failed to start measuring!\n"));

    // Return true when finished
    return _isTakingMeasurements;
}
