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
 *Documentation for the Keller Protocol commands and responses, along with
 *information about the various variables, can be found
 *in the EnviroDIY KellerModbus library at:
 * https://github.com/EnviroDIY/KellerModbus
*/

#include "KellerParent.h"

// The constructor - need the sensor type, modbus address, power pin, stream for data, and number of readings to average
KellerParent::KellerParent(byte modbusAddress, Stream* stream,
               int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage,
               kellerModel model, const char *sensName, int numVariables,
               uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, -1, measurementsToAverage)
{
    _model = model;
    _modbusAddress = modbusAddress;
    _stream = stream;
    _RS485EnablePin = enablePin;
    _powerPin2 = powerPin2;
}
KellerParent::KellerParent(byte modbusAddress, Stream& stream,
               int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage,
               kellerModel model, const char *sensName, int numVariables,
               uint32_t warmUpTime_ms, uint32_t stabilizationTime_ms, uint32_t measurementTime_ms)
    : Sensor(sensName, numVariables,
             warmUpTime_ms, stabilizationTime_ms, measurementTime_ms,
             powerPin, -1, measurementsToAverage)
{
    _model = model;
    _modbusAddress = modbusAddress;
    _stream = &stream;
    _RS485EnablePin = enablePin;
    _powerPin2 = powerPin2;
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
    bool retVal = Sensor::setup();  // this will set pin modes and the setup status bit
    if (_RS485EnablePin >= 0) pinMode(_RS485EnablePin, OUTPUT);
    if (_powerPin2 >= 0) pinMode(_powerPin2, OUTPUT);

    #if defined(DEEP_DEBUGGING_SERIAL_OUTPUT)
        sensor.setDebugStream(&DEEP_DEBUGGING_SERIAL_OUTPUT);
    #endif

    // This sensor begin is just setting more pin modes, etc, no sensor power required
    retVal &= sensor.begin(_modbusAddress, _stream, _RS485EnablePin);

    return retVal;
}


// This turns on sensor power
void KellerParent::powerUp(void)
{
    if (_powerPin >= 0)
    {
        MS_DBG(F("Powering "), getSensorName(), F(" at "), getSensorLocation(),
               F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    if (_powerPin2 >= 0)
    {
        MS_DBG(F("Applying secondary power to "), getSensorName(), F(" at "), getSensorLocation(),
               F(" with pin "), _powerPin2, F("\n"));
        digitalWrite(_powerPin2, HIGH);
    }
    if (_powerPin < 0 && _powerPin2 < 0)
    {
        MS_DBG(F("Power to "), getSensorName(), F(" at "), getSensorLocation(),
               F(" is not controlled by this library.\n"));
    }
    // Set the status bit for sensor power (bit 0)
    _sensorStatus |= 0b00000001;
}


// This turns off sensor power
void KellerParent::powerDown(void)
{
    if (_powerPin >= 0)
    {
        MS_DBG(F("Turning off power to "), getSensorName(), F(" at "),
               getSensorLocation(), F(" with pin "), _powerPin, F("\n"));
        digitalWrite(_powerPin, LOW);
        // Unset the power-on time
        _millisPowerOn = 0;
    }
    if (_powerPin2 >= 0)
    {
        MS_DBG(F("Turning off secondary power to "), getSensorName(), F(" at "),
               getSensorLocation(), F(" with pin "), _powerPin2, F("\n"));
        digitalWrite(_powerPin2, LOW);
    }
    if (_powerPin < 0 && _powerPin2 < 0)
    {
        MS_DBG(F("Power to "), getSensorName(), F(" at "), getSensorLocation(),
               F(" is not controlled by this library.\n"));
    }
    // Unset the status bits for sensor power (bit 0), warm-up (bit 2),
    // activation (bit 3), stability (bit 4), measurement request (bit 5), and
    // measurement completion (bit 6)
    _sensorStatus &= 0b10000010;
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
    verifyAndAddMeasurementResult(KELLER_PRESSURE_VAR_NUM, waterPressure_mBar);
    verifyAndAddMeasurementResult(KELLER_TEMP_VAR_NUM, waterTempertureC);
    verifyAndAddMeasurementResult(KELLER_HEIGHT_VAR_NUM, waterDepthM);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    // Return true when finished
    return success;
}
