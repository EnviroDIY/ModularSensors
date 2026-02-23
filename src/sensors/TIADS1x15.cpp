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


// ============================================================================
// TIADS1x15Base Constructors
// ============================================================================

// Constructor for single-ended measurements
TIADS1x15Base::TIADS1x15Base(uint8_t adsChannel, float voltageMultiplier,
                             adsGain_t adsGain, uint8_t i2cAddress,
                             float adsSupplyVoltage)
    : AnalogVoltageBase(adsChannel, voltageMultiplier, adsSupplyVoltage, -1),
      _adsGain(adsGain),
      _i2cAddress(i2cAddress) {
    // NOTE: We DO NOT validate the channel numbers in this constructor!  We
    // CANNOT print a warning here about invalid channel because the Serial
    // object may not be initialized yet, and we don't want to cause a crash.
    // The readVoltageSingleEnded and readVoltageDifferential functions will
    // handle validation and return false if the channel configuration is
    // invalid, but we can't do that here in the constructor
}

// Constructor for differential measurements
TIADS1x15Base::TIADS1x15Base(uint8_t adsChannel1, uint8_t adsChannel2,
                             float voltageMultiplier, adsGain_t adsGain,
                             uint8_t i2cAddress, float adsSupplyVoltage)
    : AnalogVoltageBase(adsChannel1, voltageMultiplier, adsSupplyVoltage,
                        adsChannel2),
      _adsGain(adsGain),
      _i2cAddress(i2cAddress) {
    // NOTE: We DO NOT validate the channel numbers and pairings in this
    // constructor!  We CANNOT print a warning here about invalid channel
    // because the Serial object may not be initialized yet, and we don't want
    // to cause a crash. The readVoltageSingleEnded and readVoltageDifferential
    // functions will handle validation and return false if the channel
    // configuration is invalid, but we can't do that here in the constructor
}


// ============================================================================
// TIADS1x15Base Functions
// ============================================================================

String TIADS1x15Base::getSensorLocation(void) {
#ifndef MS_USE_ADS1015
    String sensorLocation = F("ADS1115_0x");
#else
    String sensorLocation = F("ADS1015_0x");
#endif
    sensorLocation += String(_i2cAddress, HEX);
    if (isDifferential()) {
        sensorLocation += F("_Diff");
        sensorLocation += String(_analogChannel);
        sensorLocation += F("_");
        sensorLocation += String(_analogDifferentialChannel);
    } else {
        sensorLocation += F("_Channel");
        sensorLocation += String(_analogChannel);
    }
    return sensorLocation;
}

bool TIADS1x15Base::readVoltageSingleEnded(float& resultValue) {
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
    // Validate ADS1x15 channel range for single-ended measurements
    if (_analogChannel > 3) {
        MS_DBG(F("  Invalid ADS1x15 channel "), _analogChannel,
               F(", valid range is 0-3"));
        return false;
    }
    // Taking this reading includes the 8ms conversion delay.
    // Measure the ADC raw count
    adcCounts = ads.readADC_SingleEnded(_analogChannel);
    // Convert ADC raw counts value to voltage (V)
    adcVoltage = ads.computeVolts(adcCounts);
    MS_DBG(F("  ads.readADC_SingleEnded("), _analogChannel, F("):"), adcCounts,
           F(" voltage:"), adcVoltage);
    // Verify the range based on the actual power supplied to the ADS.
    // Valid range is approximately -0.3V to (supply voltage + 0.3V) with
    // absolute maximum of 5.5V per datasheet
    float minValidVoltage = -0.3;
    float maxValidVoltage = _supplyVoltage + 0.3;
    if (maxValidVoltage > 5.5) {
        maxValidVoltage = 5.5;  // Absolute maximum per datasheet
    }

    MS_DBG(F("  ADS supply voltage:"), _supplyVoltage, F("V"));
    MS_DBG(F("  Valid voltage range:"), minValidVoltage, F("V to"),
           maxValidVoltage, F("V"));

    if (adcVoltage <= maxValidVoltage && adcVoltage >= minValidVoltage) {
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

bool TIADS1x15Base::readVoltageDifferential(float& resultValue) {
    bool    success      = false;
    int16_t adcCounts    = -9999;
    float   adcVoltage   = -9999;
    float   scaledResult = -9999;

// Create an auxiliary ADC object
#ifndef MS_USE_ADS1015
    Adafruit_ADS1115 ads;
#else
    Adafruit_ADS1015 ads;
#endif

    // Set the internal gain according to user configuration
    ads.setGain(_adsGain);

    if (!ads.begin(_i2cAddress)) {
        MS_DBG(F("  ADC initialization failed at 0x"),
               String(_i2cAddress, HEX));
        return false;
    }

    // Validate differential channel combination
    if (!isValidDifferentialPair(_analogChannel, _analogDifferentialChannel)) {
        MS_DBG(F("  Invalid differential channel pair: "), _analogChannel,
               F("-"), _analogDifferentialChannel);
        return false;
    }

    // Read differential voltage based on channel configuration
    // NOTE: Only canonical ordered pairs are supported (lower channel number
    // first) to ensure consistent polarity. Pairs like (1,0) are NOT supported
    // - use (0,1) instead.
    if (_analogChannel == 0 && _analogDifferentialChannel == 1) {
        adcCounts = ads.readADC_Differential_0_1();
    } else if (_analogChannel == 0 && _analogDifferentialChannel == 3) {
        adcCounts = ads.readADC_Differential_0_3();
    } else if (_analogChannel == 1 && _analogDifferentialChannel == 3) {
        adcCounts = ads.readADC_Differential_1_3();
    } else if (_analogChannel == 2 && _analogDifferentialChannel == 3) {
        adcCounts = ads.readADC_Differential_2_3();
    } else {
        MS_DBG(F("  Unsupported differential channel combination: "),
               _analogChannel, F("-"), _analogDifferentialChannel);
        MS_DBG(F("  Use canonical ordered pairs: 0-1, 0-3, 1-3, or 2-3"));
        return false;
    }

    // Convert counts to voltage
    adcVoltage = ads.computeVolts(adcCounts);
    MS_DBG(F("  Differential ADC counts:"), adcCounts, F(" voltage:"),
           adcVoltage);

    // Validate range - for differential measurements, use PGA full-scale range
    // Based on gain setting rather than supply voltage
    float fullScaleVoltage = 4.096;  // Default for GAIN_ONE
    switch (_adsGain) {
        case GAIN_TWOTHIRDS: fullScaleVoltage = 6.144; break;
        case GAIN_ONE: fullScaleVoltage = 4.096; break;
        case GAIN_TWO: fullScaleVoltage = 2.048; break;
        case GAIN_FOUR: fullScaleVoltage = 1.024; break;
        case GAIN_EIGHT: fullScaleVoltage = 0.512; break;
        case GAIN_SIXTEEN: fullScaleVoltage = 0.256; break;
        default:
            MS_DBG(F("  Unknown ADS gain value:"), _adsGain,
                   F(" using conservative 4.096V range"));
            fullScaleVoltage = 4.096;  // Conservative fallback
            break;
    }
    float minValidVoltage = -fullScaleVoltage;
    float maxValidVoltage = fullScaleVoltage;

    MS_DBG(F("  ADS gain setting determines full-scale range"));
    MS_DBG(F("  Valid differential voltage range:"), minValidVoltage, F("V to"),
           maxValidVoltage, F("V"));

    if (adcVoltage <= maxValidVoltage && adcVoltage >= minValidVoltage) {
        scaledResult = adcVoltage * _voltageMultiplier;
        MS_DBG(F("  scaledResult:"), scaledResult);
        resultValue = scaledResult;
        success     = true;
    } else {
        MS_DBG(F("  Differential voltage out of valid range"));
        resultValue = -9999;
    }

    return success;
}

// Validation function for differential channel pairs
bool TIADS1x15Base::isValidDifferentialPair(uint8_t channel1,
                                            uint8_t channel2) {
    // Only canonical ordered pairs are valid (lower channel number first)
    // This ensures consistent polarity: channel1 is positive, channel2 is
    // negative Valid combinations are: 0-1, 0-3, 1-3, or 2-3 (in that order
    // only)
    if (channel1 >= channel2) return false;  // Reject reversed or equal pairs

    if (channel1 == 0 && channel2 == 1) return true;
    if (channel1 == 0 && channel2 == 3) return true;
    if (channel1 == 1 && channel2 == 3) return true;
    if (channel1 == 2 && channel2 == 3) return true;
    return false;
}

// Setter and getter methods for ADS gain
void TIADS1x15Base::setADSGain(adsGain_t adsGain) {
    _adsGain = adsGain;
}

adsGain_t TIADS1x15Base::getADSGain(void) const {
    return _adsGain;
}

// ============================================================================
// TIADS1x15 Functions
// ============================================================================

// The constructor - need the power pin the data pin, and voltage multiplier if
// non standard
TIADS1x15::TIADS1x15(int8_t powerPin, uint8_t adsChannel,
                     float voltageMultiplier, adsGain_t adsGain,
                     uint8_t i2cAddress, uint8_t measurementsToAverage,
                     float adsSupplyVoltage)
    : Sensor("TIADS1x15", TIADS1X15_NUM_VARIABLES, TIADS1X15_WARM_UP_TIME_MS,
             TIADS1X15_STABILIZATION_TIME_MS, TIADS1X15_MEASUREMENT_TIME_MS,
             powerPin, adsChannel, measurementsToAverage,
             TIADS1X15_INC_CALC_VARIABLES),
      TIADS1x15Base(adsChannel, voltageMultiplier, adsGain, i2cAddress,
                    adsSupplyVoltage) {
    // NOTE: We DO NOT validate the channel numbers in this constructor!  We
    // CANNOT print a warning here about invalid channel because the Serial
    // object may not be initialized yet, and we don't want to cause a crash.
    // The readVoltageSingleEnded and readVoltageDifferential functions will
    // handle validation and return false if the channel configuration is
    // invalid, but we can't do that here in the constructor
}

// Constructor for differential measurements
TIADS1x15::TIADS1x15(int8_t powerPin, uint8_t adsChannel1, uint8_t adsChannel2,
                     float voltageMultiplier, adsGain_t adsGain,
                     uint8_t i2cAddress, uint8_t measurementsToAverage,
                     float adsSupplyVoltage)
    : Sensor("TIADS1x15", TIADS1X15_NUM_VARIABLES, TIADS1X15_WARM_UP_TIME_MS,
             TIADS1X15_STABILIZATION_TIME_MS, TIADS1X15_MEASUREMENT_TIME_MS,
             powerPin, -1, measurementsToAverage, TIADS1X15_INC_CALC_VARIABLES),
      TIADS1x15Base(adsChannel1, adsChannel2, voltageMultiplier, adsGain,
                    i2cAddress, adsSupplyVoltage) {
    // NOTE: We DO NOT validate the channel numbers and pairings in this
    // constructor!  We CANNOT print a warning here about invalid channel
    // because the Serial object may not be initialized yet, and we don't want
    // to cause a crash. The readVoltageSingleEnded and readVoltageDifferential
    // functions will handle validation and return false if the channel
    // configuration is invalid, but we can't do that here in the constructor
}

// Destructor
TIADS1x15::~TIADS1x15() {}

String TIADS1x15::getSensorLocation(void) {
    return TIADS1x15Base::getSensorLocation();
}

bool TIADS1x15::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    float resultValue = -9999;
    bool  success     = false;

    // Use differential or single-ended reading based on configuration
    if (isDifferential()) {
        success = readVoltageDifferential(resultValue);
    } else {
        success = readVoltageSingleEnded(resultValue);
    }

    if (success) {
        verifyAndAddMeasurementResult(TIADS1X15_VAR_NUM, resultValue);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}

// Override setSupplyVoltage in TIADS1x15 to validate range
void TIADS1x15::setSupplyVoltage(float supplyVoltage) {
    // Validate supply voltage range: 0.0V to 5.5V per datasheet
    if (supplyVoltage < 0.0) {
        MS_DBG(F("ADS supply voltage "), supplyVoltage,
               F("V is below minimum, clamping to 0.0V"));
        _supplyVoltage = 0.0;
    } else if (supplyVoltage > 5.5) {
        MS_DBG(F("ADS supply voltage "), supplyVoltage,
               F("V exceeds maximum, clamping to 5.5V"));
        _supplyVoltage = 5.5;
    } else {
        _supplyVoltage = supplyVoltage;
    }
}

// cspell:words GAIN_TWOTHIRDS
