/*
 *ExternalVoltage.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Bobby Schulz <schu3119@umn.edu>
 * Adapted from ApogeeSQ212.h by Sara Damiano (sdamiano@stroudcenter.org)

 * This file is for the grove voltage divider (but will work with any voltage divider with an output in the range of 0 ~ 3.3v)
 * This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * The grove voltage divider is a simple voltage divider designed to measure high external voltages on a low voltage
 * ADC. This module employs a variable gain via two pairs of voltage dividers, and a unity gain amplification to reduce output
 * impedance of the module
 *
 * Range:
 *   without voltage divider:  0 - 3.6V
 *   1/gain = 3x: 0.3 ~ 12.9v
 *   1/gain = 10x: 1 ~ 43v
 * Accuracy is < ± 1%
 * Resolution: 16-bit ADC:
 *   without voltage divider:  0.05mV
 *   1/gain = 3x: 0.2mV
 *   1/gain = 10x: 0.65 mV
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
ExternalVoltage::ExternalVoltage(int8_t powerPin, int8_t dataPin, float gain,
                                 uint8_t i2cAddress, uint8_t measurementsToAverage)
    : Sensor(F("ExternalVoltage"), EXT_VOLT_NUM_VARIABLES,
             EXT_VOLT_WARM_UP_TIME_MS, EXT_VOLT_STABILIZATION_TIME_MS, EXT_VOLT_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage)
{
    _gain = gain;
    _i2cAddress = i2cAddress;
}


String ExternalVoltage::getSensorLocation(void)
{
    String sensorLocation = F("ADS1115_0x");
    sensorLocation += String(_i2cAddress, HEX);
     sensorLocation += F("_Pin");
    sensorLocation += String(_dataPin);
    return sensorLocation;
}


bool ExternalVoltage::addSingleMeasurementResult(void)
{
    // We're actually only starting a measurment within the addSingleMeasurementResult
    // function.  The measurements are very fast (8ms) so we're not going to worry
    // about the time we're losing.  Doing it this way means that any other sensor
    // that uses the same ADD will be able to set the gain properly and will not
    // have that gain setting over-written here.

    // Create an Auxillary ADD object
    Adafruit_ADS1115 ads(_i2cAddress);     /* Use this for the 16-bit version */
    // ADS1115 Library default settings:
    //    - single-shot mode (powers down between conversions
    //    - 128 samples per second (8ms conversion time)
    //    - 2/3 gain +/- 6.144V range
    //      (limited to VDD +0.3V max, so only really up to 3.6V when powered at 3.3V!)

    // Bump the gain up to 1x = +/- 4.096V range.  (Again, really only to 3.6V when powered at 3.3V)
    ads.setGain(GAIN_ONE);
    // Begin ADC
    ads.begin();
    // Mark the time that the measurement started
    // Again, we're resetting this here because we only just started the ADD!
    _millisMeasurementRequested = millis();

    // Make sure we've waited long enough for a new reading to be available
    // We're actually doing the wait here, because unlike most sensors we only
    // started the measurement in this step.
    waitForMeasurementCompletion();

    // Variables to store the results in
    float adcVoltage = -9999;
    float calibResult = -9999;

    // Read Analog to Digital Converter (ADC)
    // Taking this reading includes the 8ms conversion delay.  Since it is so
    // short, I'm not making any effort to avoid it.
    // In this, we're allowing the library to do the bit-to-volts conversion for us
    adcVoltage = ads.readADC_SingleEnded_V(_dataPin);  // Getting the reading
    MS_DBG(F("ads.readADC_SingleEnded_V("), _dataPin, F("): "), adcVoltage, F("\t\t"));

    if (adcVoltage < 3.6 and adcVoltage > -0.3)  // Skip results out of range
    {
        // Apply the gain calculation, with a defualt gain of 10 V/V Gain
        calibResult = adcVoltage * _gain ;
        MS_DBG(F("calibResult: "), calibResult, F("\n"));
    }
    else MS_DBG(F("\n"));

    verifyAndAddMeasurementResult(EXT_VOLT_VAR_NUM, calibResult);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bit for a measurement having been requested (bit 5)
    _sensorStatus &= 0b11011111;
    // Set the status bit for measurement completion (bit 6)
    _sensorStatus |= 0b01000000;

    if (adcVoltage < 3.6 and adcVoltage > -0.3) return true;
    else return false;
}
