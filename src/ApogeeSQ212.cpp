/*
 *ApogeeSQ212.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Written By:  Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Adapted from CampbellOBS3.h by Sara Damiano (sdamiano@stroudcenter.org)

 * This file is for the Apogee SQ-212 Quantum Light sensor
 * This is dependent on the Adafruit ADS1015 library.
 *
 * Apogee SQ-212 Quantum Light sensor measures photosynthetically active radiation (PAR)
 and is typically defined as total radiation across a range of 400 to 700 nm.
 PAR is often expressed as photosynthetic photon flux density (PPFD):
 photon flux in units of micromoles per square meter per second (μmol m-2 s-1,
 equal to microEinsteins per square meter per second) summed from 400 to 700 nm.

 * Technical specifications for the Apogee SQ-212 can be found at:
 * https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/
 *
 * Power supply: 5-24 V DC with a nominal current draw of 300 μA

 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
*/


#include "ApogeeSQ212.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin and the data pin
ApogeeSQ212::ApogeeSQ212(int powerPin, int dataPin, uint8_t i2cAddress, int readingsToAverage)
    : Sensor(F("ApogeeSQ212"), SQ212_NUM_VARIABLES,
             SQ212_WARM_UP, SQ212_STABILITY, SQ212_RESAMPLE,
             powerPin, dataPin, readingsToAverage)
{
    _i2cAddress = i2cAddress;
}


String ApogeeSQ212::getSensorLocation(void)
{
    String sensorLocation = F("ADS1115_Pin");
    sensorLocation += String(_dataPin);
    return sensorLocation;
}


// nothing needs to happen to start an individual measurement
bool ApogeeSQ212::startSingleMeasurement(void)
{
    _lastMeasurementRequested = millis();
    return true;
}


bool ApogeeSQ212::addSingleMeasurementResult(void)
{
    // Start the Auxillary ADD
    Adafruit_ADS1115 ads(_i2cAddress);     /* Use this for the 16-bit version */
    ads.begin();

    // Variables to store the results in
    int16_t adcResult = 0;
    float voltage = 0;
    float calibResult = 0;

    // Read Analog to Digital Converter (ADC)
    adcResult = ads.readADC_SingleEnded(_dataPin);  // Getting the reading
    MS_DBG(F("ads.readADC_SingleEnded("), _dataPin, F("): "), ads.readADC_SingleEnded(_dataPin), F("\t\t"));

    // Convert bits into volts
    voltage = adcResult * 0.1875/1000;  // in units of V. See Adafruit_ADS1X15 library for AD conversion factors.
    // Alternately could use:
    // voltage = (adcResult * 3.3) / 17585.0;
    // where 3.3V is the voltage of the Mayfly board and
    // 17585 is the number of register bits for the ADS1115 at standard (2/3) gain
    // 17585 ~ 17600 = 3.3V / 0.1875 mV/bit (17585 vs 17600 based on actually tests)

    MS_DBG("Voltage: ", String(voltage, 6), F("\t\t"));
    // Apogee SQ-212 Calibration Factor = 1.0 μmol m-2 s-1 per mV;
    calibResult = 1 * voltage * 1000 ;  // in units of μmol m-2 s-1 (microeinsteinPerSquareMeterPerSecond)
    MS_DBG(F("calibResult: "), calibResult, F("\n"));

    sensorValues[SQ212_PAR_VAR_NUM] += calibResult;

    // Return true when finished
    return true;
}
