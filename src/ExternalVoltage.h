/*
 *ExternalVoltage.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Bobby Schulz <schu3119@umn.edu>
 * Adapted from ApogeeSQ212.h by Sara Damiano (sdamiano@stroudcenter.org) and 

 * This file is for the grove voltage divider (but will work with any voltage divider with an output in the range of 0 ~ 3.3v)
 * This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * The grove voltage divider is a simple voltage divider designed to measure high external voltages on a low voltage
 * ADC. This module employs a variable gain via two pairs of voltage dividers, and a unity gain amplification to reduce output
 * impedance of the module
 *
 * Range is either 0.3 ~ 12.9v (1/gain = 3x), or 1 ~ 43v (1/gain = 10x) 
 * Accuracy is < ± 1%
 * Resolution:
 *  16-bit ADC: < 0.65 mV
 *
 * Technical specifications for the Grove Voltage Divider can be found at:
 * http://wiki.seeedstudio.com/Grove-Voltage_Divider/
 *
 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
*/

#ifndef ExternalVoltage_h
#define ExternalVoltage_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "SensorBase.h"
#include "VariableBase.h"

#define ADS1115_ADDRESS (0x48) // 1001 000 (ADDR = GND)

#define EXT_VOLT_NUM_VARIABLES 1
#define EXT_VOLT_WARM_UP_TIME_MS 1  // Unknown!
#define EXT_VOLT_STABILIZATION_TIME_MS 1  // Unknown!
#define EXT_VOLT_MEASUREMENT_TIME_MS 1  // Unknown!

#define EXT_VOLT_VAR_NUM 0
#define EXT_VOLT_RESOLUTION 2 

// The main class for the external votlage monitor
class ExternalVoltage : public Sensor
{

public:
    // The constructor - need the power pin and the data pin and gain value is non-standard value is used (standard is 10x gain)
    ExternalVoltage(int8_t powerPin, int8_t dataPin, uint8_t i2cAddress = ADS1115_ADDRESS, uint8_t measurementsToAverage = 1, float gain = 10);

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    uint8_t _i2cAddress;
    float _gain;
};


class ExternalVoltage_Volt : public Variable
{
public:
    ExternalVoltage_Volt(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, EXT_VOLT_VAR_NUM,
               F("voltage"), F("volts"),
               EXT_VOLT_RESOLUTION,
               F("voltage divider"), UUID, customVarCode)
    {}
};

#endif
