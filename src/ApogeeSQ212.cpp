/*
 *ApogeeSQ212.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Adapted from CampbellOBS3.h by Sara Damiano (sdamiano@stroudcenter.org)

 * This file is for the Apogee SQ-212 Quantum Light sensor
 * This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * Apogee SQ-212 Quantum Light sensor measures photosynthetically active radiation (PAR)
 and is typically defined as total radiation across a range of 400 to 700 nm.
 PAR is often expressed as photosynthetic photon flux density (PPFD):
 photon flux in units of micromoles per square meter per second (μmol m-2 s-1,
 equal to microEinsteins per square meter per second) summed from 400 to 700 nm.
 *
 * Range is 0 to 2500 µmol m-2 s-1
 * Accuracy is ± 0.5%
 * Resolution:
 *  16-bit ADC: 0.04 µmol m-2 s-1 - This is what is supported!
 *  12-bit ADC: 2.44 µmol m-2 s-1
 *
 * Technical specifications for the Apogee SQ-212 can be found at:
 * https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/
 *
 * Power supply: 5-24 V DC with a nominal current draw of 300 μA
 *
 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
*/


#include "ApogeeSQ212.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin and the data pin
ApogeeSQ212::ApogeeSQ212(int8_t powerPin, int8_t dataPin, uint8_t i2cAddress, uint8_t measurementsToAverage)
    : Sensor(F("ApogeeSQ212"), SQ212_NUM_VARIABLES,
             SQ212_WARM_UP_TIME_MS, SQ212_STABILIZATION_TIME_MS, SQ212_MEASUREMENT_TIME_MS,
             powerPin, dataPin, measurementsToAverage)
{
    _i2cAddress = i2cAddress;
}


String ApogeeSQ212::getSensorLocation(void)
{
    String sensorLocation = F("ADS1115_0x");
    sensorLocation += String(_i2cAddress, HEX);
     sensorLocation += F("_Pin");
    sensorLocation += String(_dataPin);
    return sensorLocation;
}


bool ApogeeSQ212::addSingleMeasurementResult(void)
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
    // Sensor return range is 0-2.5V, but the next gain option is 2x which only allows up to 2.048V
    ads.setGain(GAIN_ONE);
    // Begin ADC
    ads.begin();
    // Mark the time that the measurement started
    // Again, we're resetting this here because we only just started the ADD!
    _millisMeasurementRequested = millis();

    // Make sure we've waited long enough for a new reading to be available
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
        // Apogee SQ-212 Calibration Factor = 1.0 μmol m-2 s-1 per mV;
        calibResult = 1 * adcVoltage * 1000 ;  // in units of μmol m-2 s-1 (microeinsteinPerSquareMeterPerSecond)
        MS_DBG(F("calibResult: "), calibResult, F("\n"));
    }
    else MS_DBG(F("\n"));

    verifyAndAddMeasurementResult(SQ212_PAR_VAR_NUM, calibResult);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Make sure the status bit for measurement completion (bit 5) is no longer set
    _sensorStatus &= 0b11011111;

    if (adcVoltage < 3.6 and adcVoltage > -0.3) return true;
    else return false;
}
