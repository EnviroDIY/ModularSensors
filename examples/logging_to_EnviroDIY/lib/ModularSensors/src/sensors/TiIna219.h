/*
 *TiIna219.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Neil hancock
  *
 *This file is for the TiIna219 Current/Voltage  Sensor
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
 *  Range is 0 to 5V(?)
 *
 *
 * Sensor reads instantenously over I2C bus 
*/

// Header Guards
#ifndef TiIna219_h
#define TiIna219_h

// Debugging Statement
#define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_INA219.h>

// Sensor Specific Defines
#define INA219_CURRENT_MA_RESOLUTION 2
#define INA219_CURRENT_MA_VAR_NUM 0

#define INA219_BUS_VOLTAGE_RESOLUTION 3
#define INA219_BUS_VOLTAGE_VAR_NUM 1

//#define INA219_POWER_MW_RESOLUTION 2
//#define INA219_POWER_MW_VAR_NUM 2
#if defined(INA219_POWER_MW_VAR_NUM)
#define INA219_NUM_VARIABLES 3
#else
#define INA219_NUM_VARIABLES 2
#endif //INA219_POWER_MW_VAR_NUM
#define INA219_WARM_UP_TIME_MS 100
#define INA219_STABILIZATION_TIME_MS 4000   // 0.5 s for good numbers, but optimal at 4 s based on tests using INA219timingTest.ino
#define INA219_MEASUREMENT_TIME_MS 1100     // 1.0 s according to datasheet, but slightly better stdev when 1.1 s

// The main class for the TexasInstruments INA219
class TiIna219 : public Sensor
{
public:
    TiIna219(int8_t powerPin, uint8_t i2cAddressHex = 0x40, uint8_t measurementsToAverage = 1);
    ~TiIna219();

    bool wake(void) override;
    bool setup(void) override;
    String getSensorLocation(void) override;

    // bool startSingleMeasurement(void) override;  // for forced mode
    bool addSingleMeasurementResult(void) override;
protected:
    Adafruit_INA219 ina219_phy;
    uint8_t _i2cAddressHex;
};


// Defines the Temperature Variable
class TiIna219_mA : public Variable
{
public:
    TiIna219_mA(Sensor *parentSense,
                     const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, INA219_CURRENT_MA_VAR_NUM,
               "amp", "mA",
               INA219_CURRENT_MA_RESOLUTION,
               "TiIna219Amp", UUID, customVarCode)
    {}
    ~TiIna219_mA(){};
};


// Defines the Volt Variable
class TiIna219_Volt : public Variable
{
public:
    TiIna219_Volt(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, INA219_BUS_VOLTAGE_VAR_NUM,
               "Volt", "V",
               INA219_BUS_VOLTAGE_RESOLUTION,
               "TiIna219Volt", UUID, customVarCode)
    {}
    ~TiIna219_Volt(){};
};

#if defined(INA219_POWER_MW_VAR_NUM)
// Defines the Power Variable
class TiIna219_Power : public Variable
{
public:
    TiIna219_Power(Sensor *parentSense,
                         const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, INA219_POWER_MW_VAR_NUM,
               "barometricPressure", "pascal",
               INA219_POWER_MW_RESOLUTION,
               "TiIna219Pressure", UUID, customVarCode)
    {}
};
#endif //INA219_POWER_MW_VAR_NUM



#endif  // Header Guard
