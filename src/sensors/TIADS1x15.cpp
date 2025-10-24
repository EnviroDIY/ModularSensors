/**
 * @file TIADS1x15.cpp *
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief Implements the TIADS1x15 class.
 */


#include "TIADS1x15.h"
#include <Adafruit_ADS1X15.h>


// The constructor - need the power pin the data pin, and gain if non standard
TIADS1x15::TIADS1x15(int8_t powerPin, uint8_t adsChannel, float gain,
                     uint8_t i2cAddress, uint8_t measurementsToAverage)
    : Sensor("TIADS1x15", TIADS1X15_NUM_VARIABLES, TIADS1X15_WARM_UP_TIME_MS,
             TIADS1X15_STABILIZATION_TIME_MS, TIADS1X15_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage, TIADS1X15_INC_CALC_VARIABLES),
      _adsChannel(adsChannel),
      _gain(gain),
      _i2cAddress(i2cAddress) {}
// Destructor
TIADS1x15::~TIADS1x15() {}


String TIADS1x15::getSensorLocation(void) {
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


/**
 * @brief Perform a single ADC read on the configured ADS1x15 channel and record the calibrated result.
 *
 * Checks that a measurement was started, reads the configured ADS1x15 channel, converts raw counts to volts,
 * validates the voltage is within -0.3 to 3.6 V, scales the voltage by the configured gain, and records the
 * calibrated measurement. Updates internal measurement-attempt bookkeeping to reflect success or failure.
 *
 * @return `true` if a valid measurement was recorded and attempt bookkeeping was updated accordingly, `false` otherwise.
 */
bool TIADS1x15::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }
    bool    success     = false;
    int16_t adcCounts   = -9999;
    float   adcVoltage  = -9999;
    float   calibResult = -9999;

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
    ads.setGain(GAIN_ONE);
    // Begin ADC
    ads.begin(_i2cAddress);

    // Read Analog to Digital Converter (ADC)
    // Taking this reading includes the 8ms conversion delay.
    // Measure the ADC raw count
    adcCounts = ads.readADC_SingleEnded(_adsChannel);
    // Convert ADC raw counts value to voltage (V)
    adcVoltage = ads.computeVolts(adcCounts);
    MS_DBG(F("  ads.readADC_SingleEnded("), _adsChannel, F("):"), adcVoltage);

    if (adcVoltage < 3.6 && adcVoltage > -0.3) {
        // Skip results out of range
        // Apply the gain calculation, with a default gain of 10 V/V Gain
        calibResult = adcVoltage * _gain;
        MS_DBG(F("  calibResult:"), calibResult);
        verifyAndAddMeasurementResult(TIADS1X15_VAR_NUM, calibResult);
        success = true;
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}