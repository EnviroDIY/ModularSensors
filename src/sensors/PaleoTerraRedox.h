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

// Header Guards
#ifndef PaleoTerraRedox_h
#define PaleoTerraRedox_h

// Debugging Statement
// #define MS_PALEOTERRAREDOX_DEBUG

#ifdef MS_PALEOTERRAREDOX_DEBUG
#define MS_DEBUGGING_STD "PaleoTerraRedox"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

#if defined MS_PALEOTERRA_SOFTWAREWIRE
#include <SoftwareWire.h>  // Testato's SoftwareWire
#endif

// Sensor Specific Defines
#define PTR_NUM_VARIABLES 1
#define PTR_WARM_UP_TIME_MS 1
#define PTR_STABILIZATION_TIME_MS 0
#define PTR_MEASUREMENT_TIME_MS 67

#define PTR_VOLT_RESOLUTION 2
#define PTR_VOLT_VAR_NUM 0

#define MCP3421_ADR 0x68

// The main class for the PaleoTerra Redox Sensor
class PaleoTerraRedox : public Sensor
{
public:
    // The constructor - need the power pin, optionally can give an instance
    // of TwoWire for I2C communbication, an address, and  a number of
    // measurements to average
    #if defined MS_PALEOTERRA_SOFTWAREWIRE
    PaleoTerraRedox(SoftwareWire *theI2C, int8_t powerPin,
                    uint8_t i2cAddressHex = MCP3421_ADR,
                    uint8_t measurementsToAverage = 1);
    PaleoTerraRedox(int8_t powerPin, int8_t dataPin, int8_t clockPin,
                    uint8_t i2cAddressHex = MCP3421_ADR,
                    uint8_t measurementsToAverage = 1);
    #else
    PaleoTerraRedox(TwoWire *theI2C, int8_t powerPin,
                    uint8_t i2cAddressHex = MCP3421_ADR,
                    uint8_t measurementsToAverage = 1);
    PaleoTerraRedox(int8_t powerPin,
                    uint8_t i2cAddressHex = MCP3421_ADR,
                    uint8_t measurementsToAverage = 1);
    #endif
    ~PaleoTerraRedox();

    bool setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    uint8_t _i2cAddressHex;
    #if defined MS_PALEOTERRA_SOFTWAREWIRE
    SoftwareWire *_i2c;  // Software Wire
    #else
    TwoWire *_i2c;  // Hardware Wire
    #endif
};


// Defines the Redox Potential Variable
class PaleoTerraRedox_Volt : public Variable
{
public:
    PaleoTerraRedox_Volt(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "PTRVoltage")
      : Variable(parentSense,
                 (const uint8_t)PTR_VOLT_VAR_NUM,
                 (uint8_t)PTR_VOLT_RESOLUTION,
                 "Voltage", "mV",
                 varCode, uuid)
    {}
    PaleoTerraRedox_Volt()
      : Variable((const uint8_t)PTR_VOLT_VAR_NUM,
                 (uint8_t)PTR_VOLT_RESOLUTION,
                 "Voltage", "mV", "PTRVoltage")
    {}
};
#endif
