/**
 * @file CampbellOBS3.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the CampbellOBS3 class.
 */


#include "CampbellOBS3.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin, the data pin, and the calibration info
CampbellOBS3::CampbellOBS3(int8_t powerPin, uint8_t adsChannel,
                           float x2_coeff_A, float x1_coeff_B, float x0_coeff_C,
                           uint8_t i2cAddress, uint8_t measurementsToAverage)
    : Sensor("CampbellOBS3", OBS3_NUM_VARIABLES, OBS3_WARM_UP_TIME_MS,
             OBS3_STABILIZATION_TIME_MS, OBS3_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, OBS3_INC_CALC_VARIABLES),
      _adsChannel(adsChannel),
      _x2_coeff_A(x2_coeff_A),
      _x1_coeff_B(x1_coeff_B),
      _x0_coeff_C(x0_coeff_C),
      _i2cAddress(i2cAddress) {}
// Destructor
CampbellOBS3::~CampbellOBS3() {}


String CampbellOBS3::getSensorLocation(void) {
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


bool CampbellOBS3::addSingleMeasurementResult(void) {
    // Variables to store the results in
    float adcVoltage  = -9999;
    float calibResult = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
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
        // Sensor return range is 0-2.5V, but the next gain option is 2x which
        // only allows up to 2.048V
        ads.setGain(GAIN_ONE);
        // Begin ADC
        ads.begin();

        // Print out the calibration curve
        MS_DBG(F("  Input calibration Curve:"), _x2_coeff_A, F("x^2 +"),
               _x1_coeff_B, F("x +"), _x0_coeff_C);

        // Read Analog to Digital Converter (ADC)
        // Taking this reading includes the 8ms conversion delay.
        // We're allowing the ADS1115 library to do the bit-to-volts conversion
        // for us
        adcVoltage =
            ads.readADC_SingleEnded_V(_adsChannel);  // Getting the reading
        MS_DBG(F("  ads.readADC_SingleEnded_V("), _adsChannel, F("):"),
               adcVoltage);

        if (adcVoltage < 3.6 && adcVoltage > -0.3) {
            // Skip results out of range
            // Apply the unique calibration curve for the given sensor
            calibResult = (_x2_coeff_A * sq(adcVoltage)) +
                (_x1_coeff_B * adcVoltage) + _x0_coeff_C;
            MS_DBG(F("  calibResult:"), calibResult);
        } else {  // set invalid voltages back to -9999
            adcVoltage = -9999;
        }
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(OBS3_TURB_VAR_NUM, calibResult);
    verifyAndAddMeasurementResult(OBS3_VOLTAGE_VAR_NUM, adcVoltage);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    if (adcVoltage < 3.6 && adcVoltage > -0.3) {
        return true;
    } else {
        return false;
    }
}
