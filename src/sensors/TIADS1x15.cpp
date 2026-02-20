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


// The constructor - need the power pin the data pin, and voltage multiplier if
// non standard
TIADS1x15::TIADS1x15(int8_t powerPin, uint8_t adsChannel,
                     float voltageMultiplier, adsGain_t adsGain,
                     uint8_t i2cAddress, uint8_t measurementsToAverage,
                     float adsSupplyVoltage)
    : Sensor("TIADS1x15", TIADS1X15_NUM_VARIABLES, TIADS1X15_WARM_UP_TIME_MS,
             TIADS1X15_STABILIZATION_TIME_MS, TIADS1X15_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage, TIADS1X15_INC_CALC_VARIABLES),
      _adsChannel(adsChannel),
      _voltageMultiplier(voltageMultiplier),
      _adsGain(adsGain),
      _i2cAddress(i2cAddress),
      _adsSupplyVoltage(adsSupplyVoltage) {}
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


bool TIADS1x15::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    float resultValue = -9999;
    bool  success     = readVoltageSingleEnded(resultValue);

    if (success) {
        verifyAndAddMeasurementResult(TIADS1X15_VAR_NUM, resultValue);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}

bool TIADS1x15::readVoltageSingleEnded(float& resultValue) {
    bool    success      = false;
    int16_t adcCounts    = -9999;
    float   adcVoltage   = -9999;
    float   scaledResult = -9999;

// Create an auxiliary ADC object
// We create and set up the ADC object here so that each sensor using
// the ADC may set the internal gain appropriately without affecting others.
#ifndef MS_USE_ADS1015
    Adafruit_ADS1115 ads;  // Use this for the 16-bit version
#else
    Adafruit_ADS1015 ads;  // Use this for the 12-bit version
#endif
    // ADS Library default settings:
    //  - TI ADS1115 (16 bit)
    //    - single-shot mode (powers down between conversions)
    //    - 128 samples per second (8ms conversion time)
    //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)
    //  - TI ADS1015 (12 bit)
    //    - single-shot mode (powers down between conversions)
    //    - 1600 samples per second (625µs conversion time)
    //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)

    // Set the internal gain according to user configuration
    ads.setGain(_adsGain);
    // Begin ADC, returns true if anything was detected at the address
    if (!ads.begin(_i2cAddress)) {
        MS_DBG(F("  ADC initialization failed at 0x"),
               String(_i2cAddress, HEX));
        return false;
    }

    // Read Analog to Digital Converter (ADC)
    // Taking this reading includes the 8ms conversion delay.
    // Measure the ADC raw count
    adcCounts = ads.readADC_SingleEnded(_adsChannel);
    // Convert ADC raw counts value to voltage (V)
    adcVoltage = ads.computeVolts(adcCounts);
    MS_DBG(F("  ads.readADC_SingleEnded("), _adsChannel, F("):"), adcCounts,
           '=', adcVoltage);

    // Verify the range based on the actual power supplied to the ADS.
    // Valid range is approximately -0.3V to (supply voltage + 0.3V) with
    // absolute maximum of 5.5V per datasheet
    float minValidVoltage = -0.3;
    float maxValidVoltage = _adsSupplyVoltage + 0.3;
    if (maxValidVoltage > 5.5) {
        maxValidVoltage = 5.5;  // Absolute maximum per datasheet
    }

    MS_DBG(F("  ADS supply voltage:"), _adsSupplyVoltage, F("V"));
    MS_DBG(F("  Valid voltage range:"), minValidVoltage, F("V to"),
           maxValidVoltage, F("V"));

    if (adcVoltage < maxValidVoltage && adcVoltage > minValidVoltage) {
        // Apply the voltage multiplier scaling, with a default multiplier of 1
        scaledResult = adcVoltage * _voltageMultiplier;
        MS_DBG(F("  scaledResult:"), scaledResult);
        resultValue = scaledResult;
        success     = true;
    } else {
        MS_DBG(F("  ADC voltage "), adcVoltage, F("V out of valid range"));
        resultValue = -9999;
    }

    return success;
}

// Setter and getter methods for ADS supply voltage
void TIADS1x15::setADSSupplyVoltage(float adsSupplyVoltage) {
    _adsSupplyVoltage = adsSupplyVoltage;
}

float TIADS1x15::getADSSupplyVoltage(void) {
    return _adsSupplyVoltage;
}

void TIADS1x15::setADSGain(adsGain_t adsGain) {
    _adsGain = adsGain;
}

adsGain_t TIADS1x15::getADSGain(void) {
    return _adsGain;
}
