/*
 *ExternalVoltage.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Bobby Schulz <schu3119@umn.edu>
 * Adapted from ApogeeSQ212.h by Sara Damiano (sdamiano@stroudcenter.org)
 *
 * This file is for the grove voltage divider (but will work with any voltage
 * divider with an output in the range of 0 ~ 3.3v)
 * This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * The grove voltage divider is a simple voltage divider designed to measure
 * high external voltages on a low voltage ADC. This module employs a variable
 * gain via two pairs of voltage dividers, and a unity gain amplification to
 * reduce output impedance of the module.
 *
 * Range:
 *   NOTE:  Range is determined by supply voltage - No more than VDD + 0.3 V or
 *          5.5 V (whichever is smaller) must be applied to this device.
 *   without voltage divider:  0 - 3.6V [when ADC is powered at 3.3V]
 *   1/gain = 3x: 0.3 ~ 12.9V
 *   1/gain = 10x: 1 ~ 43V
 * Accuracy:
 *   16-bit ADC: < 0.25% (gain error), <0.25 LSB (offset errror)
 *   12-bit ADC: < 0.15% (gain error), <3 LSB (offset errror)
 * Resolution:
 *   NOTE:  1 bit of resolution is lost in single-ended reading.  The maximum
 *          possible resolution is over the differential range from
 *          negative to positive full scale, a single ended reading is only over
 *          the range from 0 to positive full scale)
 *   NOTE:  Assuming the ADC is powered at 3.3V, this program sets the the ACD's
 *          inbuilt gain to 1, which divides the bit resolution over range of
 *          0-4.096V
 *   16-bit ADC:
 *     without voltage divider:  0.125 mV
 *     1/gain = 3x: 0.375 mV
 *     1/gain = 10x: 1.25 mV
 *   12-bit ADC:
 *     without voltage divider:  2 mV
 *     1/gain = 3x: 6 mV
 *     1/gain = 10x: 20 mV
 *
 * Technical specifications for the Grove Voltage Divider can be found at:
 * http://wiki.seeedstudio.com/Grove-Voltage_Divider
 *
 * Technical specifications for the TI ADS1115 can be found at:
 * http://www.ti.com/product/ADS1115
 *
 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
*/

// Header Guards
#ifndef ExternalVoltage_h
#define ExternalVoltage_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
#define ADS1115_ADDRESS 0x48
// 1001 000 (ADDR = GND)

#define EXT_VOLT_NUM_VARIABLES 1
// Using the warm-up time of the ADS1115
#define EXT_VOLT_WARM_UP_TIME_MS 2
// Assume a voltage is instantly ready
#define EXT_VOLT_STABILIZATION_TIME_MS 0
#define EXT_VOLT_MEASUREMENT_TIME_MS 0

#define EXT_VOLT_VAR_NUM 0
#ifdef MS_USE_ADS1015
#define EXT_VOLT_RESOLUTION 1
#else
#define EXT_VOLT_RESOLUTION 4
#endif

// The main class for the external votlage monitor
class ExternalVoltage : public Sensor
{

public:
    // The constructor - need the power pin and the data channel on the ADS1x15
    // The gain value, I2C address, and number of measurements to average are optional
    // If nothing is given a 1x gain is used.
    ExternalVoltage(int8_t powerPin, uint8_t adsChannel, float gain = 1,
                    uint8_t i2cAddress = ADS1115_ADDRESS, uint8_t measurementsToAverage = 1);
    // Destructor
    ~ExternalVoltage();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    uint8_t _adsChannel;
    float _gain;
    uint8_t _i2cAddress;
};


// The single available variable is voltage
class ExternalVoltage_Volt : public Variable
{
public:
    ExternalVoltage_Volt()
      : Variable(EXT_VOLT_VAR_NUM, EXT_VOLT_RESOLUTION,
                 "voltage", "volt", "extVoltage")
    {}
    ~ExternalVoltage_Volt(){}
};

#endif  // Header Guard
