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
                                   int powerPin, int enablePin, int measurementsToAverage,
                                   yosemitechModel model, String sensName, int numVariables,
                                   int warmUpTime_ms, int stabilizationTime_ms, int remeasurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, remeasurementTime_ms,
             powerPin, -1, measurementsToAverage)
{
    _model = model;
    _modbusAddress = modbusAddress;
    _stream = stream;
    _RS485EnablePin = enablePin;
}
YosemitechParent::YosemitechParent(byte modbusAddress, Stream& stream,
                                   int powerPin, int enablePin, int measurementsToAverage,
                                   yosemitechModel model, String sensName, int numVariables,
                                   int warmUpTime_ms, int stabilizationTime_ms, int remeasurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, remeasurementTime_ms,
             powerPin, -1, measurementsToAverage)
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
    SENSOR_STATUS retVal = Sensor::setup();
    if (_RS485EnablePin > 0) pinMode(_RS485EnablePin, OUTPUT);

    #if defined(DEEP_DEBUGGING_SERIAL_OUTPUT)
        sensor.setDebugStream(&DEEP_DEBUGGING_SERIAL_OUTPUT);
    #endif

    sensor.begin(_model, _modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// The function to wake up a sensor
// Different from the standard in that it waits for warm up and starts measurements
bool YosemitechParent::wake(void)
{
    if(!checkPowerOn()){powerUp();}

    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Send the command to begin taking readings, trying up to 5 times
    int ntries = 0;
    bool success = false;
    while (!success && ntries < 5)
    {
        MS_DBG(F("Start Measurement ("), ntries+1, F("): "));
        success = sensor.startMeasurement();
        ntries++;
    }
    if(success)
    {
        _millisMeasurementStarted = millis();
        MS_DBG(F("Measurements started.\n"));
    }
    else MS_DBG(F("Measurements NOT started!\n"));

    // Manually activate the brush
    // Needed for newer sensors that do not immediate activate on getting power
    if ( _model == Y511 or _model == Y514 or _model == Y550)
    {
        MS_DBG(F("Activate Brush: "));
        if (sensor.activateBrush()) MS_DBG(F("Brush activated.\n"));
        else MS_DBG(F("Brush NOT activated!\n"));
    }

    return success;
}


// The function to put the sensor to sleep
// Different from the standard in that it stops measurements
bool YosemitechParent::sleep(void)
{
    if(!checkPowerOn()){return true;}
    if(_millisMeasurementStarted == 0)
    {
        MS_DBG(F("Was not measuring!\n"));
        return true;
    }

    // Send the command to begin taking readings, trying up to 5 times
    bool success = false;
    int ntries = 0;
    while (!success && ntries < 5)
    {
        MS_DBG(F("Stop Measurement ("), ntries+1, F("): "));
        success = sensor.stopMeasurement();
        ntries++;
    }
    if(success)
    {
        _millisMeasurementStarted = 0;
        MS_DBG(F("Measurements stopped.\n"));
    }
    else MS_DBG(F("Measurements NOT stopped!\n"));

    return success;
}


// Want to just check that the sensor is active
bool YosemitechParent::startSingleMeasurement(void)
{
    bool success = true;
    if (_millisMeasurementStarted == 0) success = wake();
    if (_millisMeasurementStarted > 0) waitForStability();
    _lastMeasurementRequested = millis();
    return success;
}


bool YosemitechParent::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variables
    float parmValue = -9999;
    float tempValue = -9999;
    float thirdValue = -9999;

    if (_millisMeasurementStarted > 0)
    {
        // Make sure we've waited long enough for a new reading to be available
        waitForMeasurementCompletion();
        // Get Values
        MS_DBG(F("Get Values:\n"));
        success = sensor.getValues(parmValue, tempValue, thirdValue);

        // Fix not-a-number values
        if (!success or isnan(parmValue)) parmValue = -9999;
        if (!success or isnan(tempValue)) tempValue = -9999;
        if (!success or isnan(thirdValue)) thirdValue = -9999;

        // For conductivity, convert mS/cm to µS/cm
        if (_model == Y520 and parmValue != -9999) parmValue *= 1000;

        MS_DBG(F("    "), sensor.getParameter(), F(": "), parmValue, F("\n"));
        MS_DBG(F("    Temp: "), tempValue, F("\n"));

        // Not all sensors return a third value
        if (_numReturnedVars > 2)
        {
            MS_DBG(F("    Third: "), thirdValue, F("\n"));
        }
    }
    else MS_DBG(F("Sensor is not currently measuring!\n"));

    // Put values into the array
    verifyAndAddMeasurementResult(0, parmValue);
    verifyAndAddMeasurementResult(1, tempValue);
    verifyAndAddMeasurementResult(2, thirdValue);

    // Return true when finished
    return success;
}
