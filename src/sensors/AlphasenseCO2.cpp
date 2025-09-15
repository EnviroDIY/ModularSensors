/**
 * @file AlphasenseCO2.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written by Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * and Bella Henkel <bella.henkel@mnsu.edu>
 * Adapted from ApogeeSQ212.h and
 * https://github.com/bellahenkel/Soil-Sensing-Device
 *
 * @brief Implements the AlphasenseCO2 class.
 */


#include "AlphasenseCO2.h"
#include <Adafruit_ADS1X15.h>


// The constructor - need the power pin and the data pin
AlphasenseCO2::AlphasenseCO2(int8_t powerPin, aco2_adsDiffMux_t adsDiffMux,
                             uint8_t i2cAddress, uint8_t measurementsToAverage)
    : Sensor("AlphasenseCO2", ALPHASENSE_CO2_NUM_VARIABLES,
             ALPHASENSE_CO2_WARM_UP_TIME_MS,
             ALPHASENSE_CO2_STABILIZATION_TIME_MS,
             ALPHASENSE_CO2_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, ALPHASENSE_CO2_INC_CALC_VARIABLES),
      _adsDiffMux(adsDiffMux),
      _i2cAddress(i2cAddress) {}

// Destructor
AlphasenseCO2::~AlphasenseCO2() {}


String AlphasenseCO2::getSensorLocation(void) {
#ifndef MS_USE_ADS1015
    String sensorLocation = F("ADS1115_0x");
#else
    String sensorLocation = F("ADS1015_0x");
#endif
    sensorLocation += String(_i2cAddress, HEX);
    sensorLocation += F("; differential between channels 2 and 3");
    return sensorLocation;
}


bool AlphasenseCO2::addSingleMeasurementResult(void) {
    // Variables to store the results in
    int16_t adcCounts   = -9999;
    float   adcVoltage  = -9999;
    float   co2Current  = -9999;
    float   calibResult = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

// Create an auxiliary ADD object
// We create and set up the ADC object here so that each sensor using
// the ADC may set the gain appropriately without effecting others.
#ifndef MS_USE_ADS1015
        Adafruit_ADS1115 ads;  // Use this for the 16-bit version
#else
        Adafruit_ADS1015 ads;  // Use this for the 12-bit version
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
        // Sensor return range is 0-2.5V, but the next gain option is 2x which
        // only allows up to 2.048V
        ads.setGain(GAIN_ONE);
        // Begin ADC
        ads.begin(_i2cAddress);

        // Read Analog to Digital Converter (ADC)
        // Taking this reading includes the 8ms conversion delay.
        // Measure the voltage differential across the two voltage pins
        adcCounts = ads.readADC_Differential_2_3();
        // Convert ADC counts value to voltage (V)
        adcVoltage = ads.computeVolts(adcCounts);
        MS_DBG(F("  ads.readADC_Differential_2_3() converted to volts:"),
               adcVoltage);

        if (adcVoltage < 3.6 && adcVoltage > -0.3) {
            // Skip results out of range
            // Convert voltage to current (mA) - assuming a 250 Ohm resistor is
            // in series
            co2Current = (adcVoltage / 250) * 1000;
            // Convert current to ppm (using a formula recommended by the sensor
            // manufacturer)
            calibResult = 312.5 * co2Current - 1250;
            MS_DBG(F("  calibResult:"), calibResult);
        } else {
            // set invalid voltages back to -9999
            adcVoltage = -9999;
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(ALPHASENSE_CO2_VAR_NUM, calibResult);
    verifyAndAddMeasurementResult(ALPHASENSE_CO2_VOLTAGE_VAR_NUM, adcVoltage);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    clearStatusBits(MEASUREMENT_ATTEMPTED, MEASUREMENT_SUCCESSFUL);

    if (adcVoltage < 3.6 && adcVoltage > -0.3) {
        return true;
    } else {
        return false;
    }
}
