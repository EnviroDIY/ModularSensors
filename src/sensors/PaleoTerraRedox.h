/*
 *PaleoTerraRedox.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the I2C Redox sensors made by Paleo Terra.  Because these
 *sensors all ship with the same I2C address, this module is also dependent on
 *a software I2C library to allow the use of multiple sensors.
 *
 *Documentation for the sensor can be found at:
 *https://paleoterra.nl/
*/

#ifndef PaleoTerraRedox_h
#define PaleoTerraRedox_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#include <SoftI2CMaster.h>
#include <Wire.h>

#define PTR_NUM_VARIABLES 1
#define PTR_WARM_UP_TIME_MS 1
#define PTR_STABILIZATION_TIME_MS 0
#define PTR_MEASUREMENT_TIME_MS 67

#define PTR_VOLT_RESOLUTION 2
#define PTR_VOLT_VAR_NUM 0

#define MCP3421_ADR 0x68

// The main class for the AOSong DHT
class PaleoTerraRedox : public Sensor
{
public:
    // The constructor - need the power pin, the I2C data pin (SDA), the I2C
    // clock pin (SCL), and optionally a number of measurements to average
    PaleoTerraRedox(int8_t powerPin, int8_t dataPin, int8_t clockPin, uint8_t measurementsToAverage = 1);

    PaleoTerraRedox(int8_t powerPin, uint8_t ADR, uint8_t measurementsToAverage = 1);
    
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    SoftI2CMaster i2c_soft;
    uint8_t _dataPin;
    uint8_t _clockPin;
    uint8_t _ADR; //Hardware slave address
    bool HardwareI2C = false; //Default hardware to false
};


// Defines the Redox Potential Variable
class PaleoTerraRedox_Volt : public Variable
{
public:
    PaleoTerraRedox_Volt(Sensor *parentSense,
                       const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, PTR_VOLT_VAR_NUM,
               "Voltage", "mV",
               PTR_VOLT_RESOLUTION,
               "PTRVoltage", UUID, customVarCode)
    {}
};


#endif
