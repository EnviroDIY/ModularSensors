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
    sensorLocation += F("_adsDiffMux");
    sensorLocation += String(_adsDiffMux);
    return sensorLocation;
}


bool AlphasenseCO2::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    bool    success     = false;
    int16_t adcCounts   = -9999;
    float   adcVoltage  = -9999;
    float   co2Current  = -9999;
    float   calibResult = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

// Create an auxiliary ADD object
// We create and set up the ADC object here so that each sensor using the ADC
// may set the gain appropriately without effecting others.
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
    // Begin ADC, returns true if anything was detected at the address
    if (!ads.begin(_i2cAddress)) {
        MS_DBG(F("  ADC initialization failed at 0x"),
               String(_i2cAddress, HEX));
        return bumpMeasurementAttemptCount(false);
    }

    // Read Analog to Digital Converter (ADC)
    // Taking this reading includes the 8ms conversion delay.
    // Measure the voltage differential across the two voltage pins
    switch (_adsDiffMux) {
        case DIFF_MUX_0_1: {
            adcCounts = ads.readADC_Differential_0_1();
            break;
        }
        case DIFF_MUX_0_3: {
            adcCounts = ads.readADC_Differential_0_3();
            break;
        }
        case DIFF_MUX_1_3: {
            adcCounts = ads.readADC_Differential_1_3();
            break;
        }
        case DIFF_MUX_2_3: {
            adcCounts = ads.readADC_Differential_2_3();
            break;
        }
        default: {
            MS_DBG(F("  Invalid differential mux configuration"));
            return bumpMeasurementAttemptCount(false);
        }
    }
    // Convert ADC counts value to voltage (V)
    adcVoltage = ads.computeVolts(adcCounts);
    MS_DBG(F("  ads.readADC_Differential("), _adsDiffMux, F("):"),
           String(adcVoltage, 3));

    // @todo Verify the voltage range for the CO2 sensor
    // Here we are using the range of the ADS when it is powered at 3.3V
    if (adcVoltage < 3.6 && adcVoltage > -0.3) {
        // Convert voltage to current (mA) - assuming a 250 Ohm resistor is in
        // series
        co2Current = (adcVoltage / 250) * 1000;
        // Convert current to ppm (using a formula recommended by the sensor
        // manufacturer)
        calibResult = 312.5 * co2Current - 1250;
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(ALPHASENSE_CO2_VAR_NUM, calibResult);
        verifyAndAddMeasurementResult(ALPHASENSE_CO2_VOLTAGE_VAR_NUM,
                                      adcVoltage);
        success = true;
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}
