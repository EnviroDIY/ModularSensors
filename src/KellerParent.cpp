/*
 *KellerParent.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 *This file is for Modbus communication to  Keller pressure and water level sensors
 *of Series 30, Class 5, Group 20 (such as the KellerAcculevel)
 *that are Software version 5.20-12.28 and later (i.e. made after the 2012 in the 28th week)
 *Only tested the Acculevel
*
 *Documentation for the Yosemitech Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY KellerModbus library at:
 * https://github.com/EnviroDIY/KellerModbus
*/

#include "KellerParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for data, and number of readings to average
KellerParent::KellerParent(byte modbusAddress, Stream* stream,
               int8_t powerPin, int8_t enablePin, uint8_t measurementsToAverage,
               kellerModel model, String sensName, int numVariables,
               int warmUpTime_ms, int stabilizationTime_ms, int measurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, -1, measurementsToAverage)
{
    _model = model;
    _modbusAddress = modbusAddress;
    _stream = stream;
    _RS485EnablePin = enablePin;
}
KellerParent::KellerParent(byte modbusAddress, Stream& stream,
               int8_t powerPin, int8_t enablePin, uint8_t measurementsToAverage,
               kellerModel model, String sensName, int numVariables,
               int warmUpTime_ms, int stabilizationTime_ms, int measurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, -1, measurementsToAverage)
{
    _model = model;
    _modbusAddress = modbusAddress;
    _stream = &stream;
    _RS485EnablePin = enablePin;
}


// The sensor installation location on the Mayfly
String KellerParent::getSensorLocation(void)
{
    String sensorLocation = F("modbus_0x");
    if (_modbusAddress< 16) sensorLocation += "0";
    sensorLocation += String(_modbusAddress, HEX);
    return sensorLocation;
}


bool KellerParent::setup(void)
{
    bool retVal = Sensor::setup();  // sets time stamp and status bits
    if (_RS485EnablePin > 0) pinMode(_RS485EnablePin, OUTPUT);

    #if defined(DEEP_DEBUGGING_SERIAL_OUTPUT)
        sensor.setDebugStream(&DEEP_DEBUGGING_SERIAL_OUTPUT);
    #endif

    retVal &= sensor.begin(_modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// The function to wake up a sensor
bool KellerParent::wake(void)
{
    Sensor::wake();  // this will set timestamp and status bit

    return true;
}


// The function to put the sensor to sleep
bool KellerParent::sleep(void)
{
    Sensor::sleep();
    return true;
}



bool KellerParent::addSingleMeasurementResult(void)
{
    bool success = false;

    // Initialize float variables
    float waterPressureBar = -9999;
    float waterTempertureC = -9999;
    float waterDepthM = -9999;
    float waterPressure_mBar = -9999;

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
