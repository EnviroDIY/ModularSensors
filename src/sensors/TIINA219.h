/*
 *TIINA219.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Neil hancock
  *
 *This file is for the TIINA219 Current/Voltage  Sensor
 *It is dependent on the Adafruit Ina219 Library
 *
 *Documentation for the sensor can be found at:
 *https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout
 *http://www.ti.com/product/INA219
 *
 * For Current Sensor with 0.1Ohms:
 *  Full scale ranges are
 *   +-3.2Amps resolution is 0.8mA
 *   +-0.4Amps resolution is 0.1mA
 *  Absolute Accuracy is range dependent, and approx 2LSB (R accuracy unknown)
 *
 * For Voltage :
 *  Resolution is +-0.001V
 *  Accuracy is ?
 *  Range is 0 to 26V
 *
 * A single conversion takes >532 µs (586 µs typical) at 12 bit resolution
*/

// Header Guards
#ifndef TIINA219_h
#define TIINA219_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_INA219.h>

// Sensor Specific Defines

#define INA219_NUM_VARIABLES 3

#define INA219_WARM_UP_TIME_MS 100
#define INA219_STABILIZATION_TIME_MS 4000   // 0.5 s for good numbers, but optimal at 4 s based on tests using INA219timingTest.ino
#define INA219_MEASUREMENT_TIME_MS 1100     // 1.0 s according to datasheet, but slightly better stdev when 1.1 s

#define INA219_CURRENT_MA_RESOLUTION 4
#define INA219_CURRENT_MA_VAR_NUM 0

#define INA219_BUS_VOLTAGE_RESOLUTION 4
#define INA219_BUS_VOLTAGE_VAR_NUM 1

#define INA219_POWER_MW_RESOLUTION 2
#define INA219_POWER_MW_VAR_NUM 2
#define INA219_ADDRESS_BASE 0x40

// The main class for the TexasInstruments INA219
class TIINA219 : public Sensor
{
public:
    TIINA219(int8_t powerPin, uint8_t i2cAddressHex = INA219_ADDRESS_BASE, uint8_t measurementsToAverage = 1);
    ~TIINA219();

    bool wake(void) override;
    bool setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;
protected:
    Adafruit_INA219 ina219_phy;
    uint8_t _i2cAddressHex;
};


// Defines the Temperature Variable
class TIINA219_Current : public Variable
{
public:
    TIINA219_Current(Sensor *parentSense,
                     const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, INA219_CURRENT_MA_VAR_NUM,
               "amp", "mA",
               INA219_CURRENT_MA_RESOLUTION,
               "TIINA219Amp", UUID, customVarCode)
    {}
    ~TIINA219_Current(){};
};


// Defines the Volt Variable
class TIINA219_Volt : public Variable
{
public:
    TIINA219_Volt(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, INA219_BUS_VOLTAGE_VAR_NUM,
               "Volt", "V",
               INA219_BUS_VOLTAGE_RESOLUTION,
               "TIINA219Volt", UUID, customVarCode)
    {}
    ~TIINA219_Volt(){};
};


// Defines the Power Variable
class TIINA219_Power : public Variable
{
public:
    TIINA219_Power(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, INA219_POWER_MW_VAR_NUM,
               "milliwatt", "mW",
               INA219_POWER_MW_RESOLUTION,
               "TIINA219Power", UUID, customVarCode)
    {}
};

#endif  // Header Guard
