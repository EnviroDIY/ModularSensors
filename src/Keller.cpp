/*
 *Keller.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 *This file is for Modbus communication to  Keller Series 30, Class 5, Group 20 sensors,
 *that are Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)
 *Only tested the Acculevel
 *
 *Documentation for the Yosemitech Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY KellerModbus library at:
 * https://github.com/EnviroDIY/KellerModbus
*/

#include "Keller.h"

// The constructor - need the sensor type, modbus address, power pin, stream for data, and number of readings to average
Keller::Keller(byte modbusAddress, Stream* stream,
               int8_t powerPin, int8_t enablePin, uint8_t measurementsToAverage,
               String sensName, int numVariables,
               int warmUpTime_ms, int stabilizationTime_ms, int measurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, -1, measurementsToAverage)
{
    _modbusAddress = modbusAddress;
    _stream = stream;
    _RS485EnablePin = enablePin;
}
Keller::Keller(byte modbusAddress, Stream& stream,
               int8_t powerPin, int8_t enablePin, uint8_t measurementsToAverage,
               String sensName, int numVariables,
               int warmUpTime_ms, int stabilizationTime_ms, int measurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, -1, measurementsToAverage)
{
    _modbusAddress = modbusAddress;
    _stream = &stream;
    _RS485EnablePin = enablePin;
}


// The sensor installation location on the Mayfly
String Keller::getSensorLocation(void)
{
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress< 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


bool Keller::setup(void)
{
    bool retVal = Sensor::setup();  // sets time stamp and status bits
    if (_RS485EnablePin > 0) pinMode(_RS485EnablePin, OUTPUT);

    #if defined(DEEP_DEBUGGING_SERIAL_OUTPUT)
        sensor.setDebugStream(&DEEP_DEBUGGING_SERIAL_OUTPUT);
    #endif

    retVal &= sensor.begin(_model, _modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// The function to wake up a sensor
// Different from the standard in that it waits for warm up and starts measurements
bool Keller::wake(void)
{
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
        // Mark the time that the sensor was activated
        _millisSensorActivated = millis();
        // Set the status bit for sensor activation (bit 3)
        _sensorStatus |= 0b00001000;
        MS_DBG(F("Sensor activated and measuring.\n"));
    }
    else
    {
        // Make sure the activation time is not set
        _millisSensorActivated = 0;
        // Make sure the status bit for sensor activation (bit 3) is not set
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Sensor NOT activated!\n"));
    }

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
bool Keller::sleep(void)
{
    if(!checkPowerOn()){return true;}
    if(_millisSensorActivated == 0)
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
        // Unset the activation time
        _millisSensorActivated = 0;
        // Unset the activated status bit (bit 3), stability (bit 4), measurement
        // request (bit 5), and measurement completion (bit 6)
        _sensorStatus &= 0b10000111;
        MS_DBG(F("Measurements stopped.\n"));
    }
    else MS_DBG(F("Measurements NOT stopped!\n"));

    return success;
}



bool Keller::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variables
    float waterPressureBar = -9999.0;
    float waterTempertureC = -9999.0;
    float waterDepthM = -9999.0;
    float waterPressure_mBar = -9999.0

    if (_millisMeasurementRequested > 0)
    {
        // Get Values
        MS_DBG(F("Get Values:\n"));
        success = sensor.getValues(waterPressureBar, waterTempertureC);
        waterDepthM = sensor.calcWaterDepthM(waterPressureBar, waterTempertureC);  // float calcWaterDepthM(float waterPressureBar, float waterTempertureC)

        // Fix not-a-number values
        if (!success or isnan(waterPressureBar)) waterPressureBar = -9999;
        if (!success or isnan(waterTempertureC)) waterTempertureC = -9999;
        if (!success or isnan(waterDepthM)) waterDepthM = -9999;

        // For waterPressureBar, convert bar to millibar
        if (waterPressureBar != -9999) waterPressure_mBar = 1000*waterPressureBar;

        MS_DBG(F("    Pressure_mbar:"), waterPressure_mBar, F("\n"));
        MS_DBG(F("    Temp_C: "), waterTempertureC, F("\n"));
        MS_DBG(F("    Height_m:"), waterDepthM, F("\n"));
    }
    else MS_DBG(F("Sensor is not currently measuring!\n"));

    // Put values into the array
    verifyAndAddMeasurementResult(0, waterPressure_mBar);
    verifyAndAddMeasurementResult(1, waterTempertureC);
    verifyAndAddMeasurementResult(2, waterDepthM);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    // Return true when finished
    return success;
}
