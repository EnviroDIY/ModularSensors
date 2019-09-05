/*
 *ExternalVoltage.cpp
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


#include "ExternalVoltage.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin the data pin, and gain if non standard
ExternalVoltage::ExternalVoltage(int8_t powerPin, uint8_t adsChannel, float gain,
                                 uint8_t i2cAddress, uint8_t measurementsToAverage)
    : Sensor("ExternalVoltage", EXT_VOLT_NUM_VARIABLES,
             EXT_VOLT_WARM_UP_TIME_MS, EXT_VOLT_STABILIZATION_TIME_MS, EXT_VOLT_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage)
{
    _adsChannel = adsChannel;
    _gain = gain;
    _i2cAddress = i2cAddress;
}
// Destructor
ExternalVoltage::~ExternalVoltage(){}


String ExternalVoltage::getSensorLocation(void)
{
    #ifndef MS_USE_ADS1015
    String sensorLocation = F("ADS1115_0x");
    #else
    String sensorLocation = F("ADS1015_0x");
    #endif
    sensorLocation += String(_i2cAddress, HEX);
    sensorLocation += F("_Channel");
    sensorLocation += String(_adsChannel);
    return sensorLocation;
}


bool ExternalVoltage::addSingleMeasurementResult(void)
{
    // Variables to store the results in
    float adcVoltage = -9999;
    float calibResult = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // Create an Auxillary ADD object
        // We create and set up the ADC object here so that each sensor using
        // the ADC may set the gain appropriately without effecting others.
        #ifndef MS_USE_ADS1015
        Adafruit_ADS1115 ads(_i2cAddress);  // Use this for the 16-bit version
        #else
        Adafruit_ADS1015 ads(_i2cAddress);  // Use this for the 12-bit version
        #endif
        // ADS Library default settings:
        //  - TI1115 (16 bit)
        //    - single-shot mode (powers down between conversions)
        //    - 128 samples per second (8ms conversion time)
        //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)
        //  - TI1015 (12 bit)
        //    - single-shot mode (powers down between conversions)
        //    - 1600 samples per second (625µs conversion time)
        //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)

        // Bump the gain up to 1x = +/- 4.096V range
        ads.setGain(GAIN_ONE);
        // Begin ADC
        ads.begin();

        // Read Analog to Digital Converter (ADC)
        // Taking this reading includes the 8ms conversion delay.
        // We're allowing the ADS1115 library to do the bit-to-volts conversion for us
        adcVoltage = ads.readADC_SingleEnded_V(_adsChannel);  // Getting the reading
        MS_DBG(F("  ads.readADC_SingleEnded_V("), _adsChannel, F("):"), adcVoltage);

        if (adcVoltage < 3.6 and adcVoltage > -0.3)  // Skip results out of range
        {
            // Apply the gain calculation, with a defualt gain of 10 V/V Gain
            calibResult = adcVoltage * _gain ;
            MS_DBG(F("  calibResult:"), calibResult);
        }
        else  // set invalid voltages back to -9999
        {
            adcVoltage = -9999;
        }
    }
    else
    {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(EXT_VOLT_VAR_NUM, calibResult);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    if (adcVoltage < 3.6 and adcVoltage > -0.3)
    {
        return true;
    }
    else
    {
        return false;
    }
}
