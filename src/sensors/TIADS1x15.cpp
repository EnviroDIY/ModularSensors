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

// Constructor
TIADS1x15Base::TIADS1x15Base(float voltageMultiplier, adsGain_t adsGain,
                             uint8_t i2cAddress, float adsSupplyVoltage,
                             uint16_t adsDataRate)
    : AnalogVoltageBase(voltageMultiplier, adsSupplyVoltage),
      _i2cAddress(i2cAddress) {
    // Clamp supply voltage to valid ADS1x15 range: 0.0V to 5.5V per datasheet
    // NOTE: This clamp is intentionally silent — Serial/MS_DBG is NOT safe to
    // call during construction (the Serial object may not be initialized yet on
    // Arduino targets). Use setSupplyVoltage() at runtime for logged clamping.
    if (_supplyVoltage < 0.0f) {
        _supplyVoltage = 0.0f;
    } else if (_supplyVoltage > 5.5f) {
        _supplyVoltage = 5.5f;
    }
    // ADS Library default settings:
    //  - TI ADS1115 (16 bit)
    //    - single-shot mode (powers down between conversions)
    //    - 128 samples per second (8ms conversion time)
    //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)
    //  - TI ADS1015 (12 bit)
    //    - single-shot mode (powers down between conversions)
    //    - 1600 samples per second (625µs conversion time)
    //    - 2/3 gain +/- 6.144V range (limited to VDD +0.3V max)

    // Initialize the per-instance ADS driver
    _ads.setGain(adsGain);
    _ads.setDataRate(adsDataRate);

    _ads.begin(_i2cAddress);
}


// ============================================================================
// TIADS1x15Base Functions
// ============================================================================

String TIADS1x15Base::getAnalogLocation(int8_t analogChannel,
                                        int8_t analogReferenceChannel) {
    String sensorLocation;
#ifndef MS_USE_ADS1015
    sensorLocation += F("ADS1115_0x");
#else
    sensorLocation += F("ADS1015_0x");
#endif
    sensorLocation += String(_i2cAddress, HEX);
    if (isValidDifferentialPair(analogChannel, analogReferenceChannel)) {
        sensorLocation += F("_Diff_");
        sensorLocation += String(analogChannel);
        sensorLocation += F("_");
        sensorLocation += String(analogReferenceChannel);
    } else {
        sensorLocation += F("_Channel");
        sensorLocation += String(analogChannel);
    }
    return sensorLocation;
}

bool TIADS1x15Base::readVoltageSingleEnded(int8_t analogChannel,
                                           float& resultValue) {
    bool    success      = false;
    int16_t adcCounts    = -9999;
    float   adcVoltage   = -9999.0f;
    float   scaledResult = -9999.0f;

    // Use the per-instance ADS driver (gain configured in constructor)

    // Verify ADC is available (already initialized in constructor)
    // Note: The ADS driver may return false if I2C communication fails
    if (!_ads.begin(_i2cAddress)) {
        MS_DBG(F("  ADC communication failed at 0x"), String(_i2cAddress, HEX));
        return false;
    }

    // Read Analog to Digital Converter (ADC)
    // Validate ADS1x15 channel range for single-ended measurements
    if (analogChannel < 0 || analogChannel > 3) {
        MS_DBG(F("  Invalid ADS1x15 channel "), analogChannel,
               F(", valid range is 0-3"));
        return false;
    }
    // Taking this reading includes the 8ms conversion delay.
    // Measure the ADC raw count
    adcCounts = _ads.readADC_SingleEnded(analogChannel);
    // Convert ADC raw counts value to voltage (V)
    adcVoltage = _ads.computeVolts(adcCounts);
    MS_DBG(F("  _ads.readADC_SingleEnded("), analogChannel, F("):"), adcCounts,
           F(" voltage:"), adcVoltage);

    // Verify the range based on both PGA full-scale range and supply voltage
    // For single-ended measurements, the valid range is constrained by:
    // 1. PGA full-scale range (±FSR based on gain setting)
    // 2. Supply voltage limits (approximately -0.3V to supply+0.3V)
    // 3. Absolute maximum of 5.5V per datasheet

    float pgaFullScaleRange = _ads.getFsRange();
    float minValidVoltage   = -0.3f;  // Per datasheet

    // Take the minimum of PGA FSR and supply-based limit
    float maxValidVoltage = pgaFullScaleRange;
    float supplyBasedMax  = _supplyVoltage + 0.3f;
    if (supplyBasedMax < maxValidVoltage) { maxValidVoltage = supplyBasedMax; }

    // Apply absolute maximum per datasheet
    if (maxValidVoltage > 5.5f) { maxValidVoltage = 5.5f; }

    MS_DBG(F("  ADS supply voltage:"), _supplyVoltage, F("V"));
    MS_DBG(F("  PGA full-scale range:"), pgaFullScaleRange, F("V"));
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
        resultValue = -9999.0f;
    }

    return success;
}

bool TIADS1x15Base::readVoltageDifferential(int8_t analogChannel,
                                            int8_t analogReferenceChannel,
                                            float& resultValue) {
    bool    success      = false;
    int16_t adcCounts    = -9999;
    float   adcVoltage   = -9999.0f;
    float   scaledResult = -9999.0f;

    // Use the per-instance ADS driver (configured in constructor)
    // Verify ADC is available
    if (!_ads.begin(_i2cAddress)) {
        MS_DBG(F("  ADC communication failed at 0x"), String(_i2cAddress, HEX));
        return false;
    }

    // Validate differential channel combination
    if (!isValidDifferentialPair(analogChannel, analogReferenceChannel)) {
        MS_DBG(F("  Unsupported differential channel combination: "),
               analogChannel, F("-"), analogReferenceChannel);
        MS_DBG(F("  Use canonical ordered pairs: 0-1, 0-3, 1-3, or 2-3"));
        return false;
    }

    // Read differential voltage based on channel configuration
    // NOTE: Only canonical ordered pairs are supported (lower channel number
    // first) to ensure consistent polarity. Pairs like (1,0) are NOT supported
    // - use (0,1) instead.
    if (analogChannel == 0 && analogReferenceChannel == 1) {
        adcCounts = _ads.readADC_Differential_0_1();
    } else if (analogChannel == 0 && analogReferenceChannel == 3) {
        adcCounts = _ads.readADC_Differential_0_3();
    } else if (analogChannel == 1 && analogReferenceChannel == 3) {
        adcCounts = _ads.readADC_Differential_1_3();
    } else if (analogChannel == 2 && analogReferenceChannel == 3) {
        adcCounts = _ads.readADC_Differential_2_3();
    } else {
        // Should never reach here; isValidDifferentialPair must have been
        // widened without updating this dispatch table.
        MS_DBG(F(
            "  Internal error: unhandled differential pair after validation"));
        return false;
    }

    // Convert counts to voltage
    adcVoltage = _ads.computeVolts(adcCounts);
    MS_DBG(F("  Differential ADC counts:"), adcCounts, F(" voltage:"),
           adcVoltage);

    // Validate range - for differential measurements, use PGA full-scale range
    // Based on gain setting rather than supply voltage
    float fullScaleVoltage = _ads.getFsRange();
    float minValidVoltage  = -fullScaleVoltage;
    float maxValidVoltage  = fullScaleVoltage;

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
        resultValue = -9999.0f;
    }

    return success;
}

// Validation function for differential channel pairs
bool TIADS1x15Base::isValidDifferentialPair(int8_t channel1, int8_t channel2) {
    // Only canonical ordered pairs are valid (lower channel number first)
    // This ensures consistent polarity: channel1 is positive, channel2 is
    // negative Valid combinations are: 0-1, 0-3, 1-3, or 2-3 (in that order
    // only)
    if (channel1 >= channel2) return false;  // Reject reversed or equal pairs

    return (channel1 == 0 && (channel2 == 1 || channel2 == 3)) ||
        (channel1 == 1 && channel2 == 3) || (channel1 == 2 && channel2 == 3);
}

// Setter and getter methods for ADS gain
void TIADS1x15Base::setADSGain(adsGain_t adsGain) {
    // Update the per-instance driver with new gain setting
    _ads.setGain(adsGain);
}

adsGain_t TIADS1x15Base::getADSGain(void) {
    return _ads.getGain();
}

// Setter and getter methods for ADS data rate
void TIADS1x15Base::setADSDataRate(uint16_t adsDataRate) {
    // Update the per-instance driver with new data rate setting
    _ads.setDataRate(adsDataRate);
}

uint16_t TIADS1x15Base::getADSDataRate(void) {
    return _ads.getDataRate();
}

// Override setSupplyVoltage in TIADS1x15Base to validate ADS range
void TIADS1x15Base::setSupplyVoltage(float supplyVoltage) {
    // Validate supply voltage range: 0.0V to 5.5V per datasheet
    if (supplyVoltage < 0.0f) {
        MS_DBG(F("ADS supply voltage "), supplyVoltage,
               F("V is below minimum, clamping to 0.0V"));
        _supplyVoltage = 0.0f;
    } else if (supplyVoltage > 5.5f) {
        MS_DBG(F("ADS supply voltage "), supplyVoltage,
               F("V exceeds maximum, clamping to 5.5V"));
        _supplyVoltage = 5.5f;
    } else {
        _supplyVoltage = supplyVoltage;
    }
}

// ============================================================================
// TIADS1x15 Functions
// ============================================================================

// The constructor - need the power pin the data pin, and voltage multiplier if
// non standard
TIADS1x15::TIADS1x15(int8_t powerPin, int8_t adsChannel,
                     int8_t         analogReferenceChannel,
                     uint8_t        measurementsToAverage,
                     TIADS1x15Base* analogVoltageReader)
    : Sensor("TIADS1x15", TIADS1X15_NUM_VARIABLES, TIADS1X15_WARM_UP_TIME_MS,
             TIADS1X15_STABILIZATION_TIME_MS, TIADS1X15_MEASUREMENT_TIME_MS,
             powerPin, adsChannel, measurementsToAverage,
             TIADS1X15_INC_CALC_VARIABLES),
      _analogReferenceChannel(analogReferenceChannel) {
    // If no analog voltage reader was provided, create a default one
    if (analogVoltageReader == nullptr) {
        _analogVoltageReader = createTIADS1x15Base(_ownsAnalogVoltageReader);
    } else {
        _analogVoltageReader     = analogVoltageReader;
        _ownsAnalogVoltageReader = false;
    }

    // NOTE: We DO NOT validate the channel numbers and pairings in this
    // constructor!  We CANNOT print a warning here about invalid channel
    // because the Serial object may not be initialized yet, and we don't want
    // to cause a crash. The readVoltageSingleEnded and readVoltageDifferential
    // functions will handle validation and return false if the channel
    // configuration is invalid, but we can't do that here in the constructor
}

// Destructor
TIADS1x15::~TIADS1x15() {
    // Clean up the ADS base object if we created it
    if (_ownsAnalogVoltageReader && _analogVoltageReader != nullptr) {
        delete _analogVoltageReader;
        _analogVoltageReader = nullptr;
    }
}

String TIADS1x15::getSensorLocation(void) {
    return _analogVoltageReader->getAnalogLocation(_dataPin,
                                                   _analogReferenceChannel);
}

bool TIADS1x15::addSingleMeasurementResult(void) {
    // Immediately quit if the measurement was not successfully started
    if (!getStatusBit(MEASUREMENT_SUCCESSFUL)) {
        return bumpMeasurementAttemptCount(false);
    }

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    float resultValue = -9999.0f;
    bool  success     = false;

    // Use differential or single-ended reading based on configuration
    if (_analogVoltageReader->isValidDifferentialPair(
            _dataPin, _analogReferenceChannel)) {
        success = _analogVoltageReader->readVoltageDifferential(
            _dataPin, _analogReferenceChannel, resultValue);
    } else {
        if (_analogReferenceChannel >= 0 && _analogReferenceChannel <= 3) {
            MS_DBG(F("  Warning: reference channel "), _analogReferenceChannel,
                   F(" set but pair is not a valid differential config;"
                     " falling back to single-ended on channel "),
                   _dataPin);
        }
        success = _analogVoltageReader->readVoltageSingleEnded(_dataPin,
                                                               resultValue);
    }

    if (success) {
        verifyAndAddMeasurementResult(TIADS1X15_VAR_NUM, resultValue);
    }

    // Return success value when finished
    return bumpMeasurementAttemptCount(success);
}

// cspell:words GAIN_TWOTHIRDS
