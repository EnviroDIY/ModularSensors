/*
 *AOSongDHT.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the AOSong Digital-output relative humidity & temperature sensor/modules:
 *DHT11, DHT21(AM2301), and DHT 22 (AM2302).  It is dependent on the Adafruit DHT Library
 *
 *Documentation for the sensor can be found at:
 *http://www.aosong.com/en/products/details.asp?id=117
 *
 * For Relative Humidity:
 *  Resolution is 0.1 % RH for DHT22 and 1 % RH for DHT11
 *  Accuracy is ± 2 % RH for DHT22 and ± 5 % RH for DHT11
 *  Range is 0 to 100 % RH
 *
 * For Temperature:
 *  Resolution is 0.1°C
 *  Accuracy is ±0.5°C for DHT22 and ± ±2°C for DHT11
 *  Range is -40°C to +80°C
 *
 * Warm up time: 1.7sec; assume stable on warm-up
 * Re-sampling time: 2.0sec
*/

#ifndef PaleoTerraRedox_h
#define PaleoTerraRedox_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#include <SoftI2CMaster.h>

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
    // The constructor - need the power pin, the data pin, and the sensor type
    PaleoTerraRedox(int8_t powerPin, int8_t dataPin, int8_t clockPin, uint8_t measurementsToAverage = 1);

    bool setup(void) override;
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

private:
    SoftI2CMaster i2c_soft;
    uint8_t _dataPin;
    uint8_t _clockPin;
};


// Defines the Humidity Variable
class PaleoTerraRedox_Volt : public Variable
{
public:
    PaleoTerraRedox_Volt(Sensor *parentSense,
                       String UUID = "", String customVarCode = "")
      : Variable(parentSense, PTR_VOLT_VAR_NUM,
               F("Voltage"), F("uV"),
               PTR_VOLT_RESOLUTION,
               F("PTRVoltage"), UUID, customVarCode)
    {}
};


#endif
