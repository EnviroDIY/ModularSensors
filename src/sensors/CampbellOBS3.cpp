/*
 *CampbellOBS3.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Campbell Scientific OBS-3+
 *This is dependent on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * Ranges: (depends on sediment size, particle shape, and reflectivity)
 *  Turbidity (low/high): 250/1000 NTU; 500/2000 NTU; 1000/4000 NTU
 *  Mud: 5000 to 10,000 mg L–1
 *  Sand: 50,000 to 100,000 mg L–1
 * Accuracy: (whichever is larger)
 *  Turbidity: 2% of reading or 0.5 NTU
 *  Mud: 2% of reading or 1 mg L–1
 *  Sand: 4% of reading or 10 mg L–1
 * Resolution:
 *  16-bit ADC - This is what is supported!
 *      Turbidity: 0.004/0.01 NTU; 0.008/0.03 NTU; 0.01/0.06 NTU
 *  12-bit ADC
 *      Turbidity: 0.06/0.2 NTU; 0.1/0.5 NTU; 0.2/1.0 NTU
 *
 * Minimum stabilization time: 2s
 * Maximum data rate = 10Hz (100ms/sample)
*/


#include "CampbellOBS3.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin, the data pin, and the calibration info
CampbellOBS3::CampbellOBS3(int8_t powerPin, int8_t dataPin,
                           float x2_coeff_A, float x1_coeff_B, float x0_coeff_C,
                           uint8_t i2cAddress, uint8_t measurementsToAverage)
  : Sensor("CampbellOBS3", OBS3_NUM_VARIABLES,
           OBS3_WARM_UP_TIME_MS, OBS3_STABILIZATION_TIME_MS, OBS3_MEASUREMENT_TIME_MS,
           powerPin, dataPin, measurementsToAverage)
{
    _x2_coeff_A = x2_coeff_A;
    _x1_coeff_B = x1_coeff_B;
    _x0_coeff_C = x0_coeff_C;
    _i2cAddress = i2cAddress;
}
// Destructor
CampbellOBS3::~CampbellOBS3(){}


String CampbellOBS3::getSensorLocation(void)
{
    String sensorLocation = F("ADS1115_0x");
    sensorLocation += String(_i2cAddress, HEX);
     sensorLocation += F("_Pin");
    sensorLocation += String(_dataPin);
    return sensorLocation;
}


bool CampbellOBS3::addSingleMeasurementResult(void)
{
    // Variables to store the results in
    float adcVoltage = -9999;
    float calibResult = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        // Create an Auxillary ADD object
        // We create and set up the ADC object here so that any other sensor on the
        // ADC that may have set the gain differently does not cause problems.
        Adafruit_ADS1115 ads(_i2cAddress);     /* Use this for the 16-bit version */
        // ADS1115 Library default settings:
        //    - single-shot mode (powers down between conversions)
        //    - 128 samples per second (8ms conversion time)
        //    - 2/3 gain +/- 6.144V range
        //      (limited to VDD +0.3V max, so only really up to 3.6V when powered at 3.3V!)

        // Bump the gain up to 1x = +/- 4.096V range.  (Again, really only to 3.6V when powered at 3.3V)
        // Sensor return range is 0-2.5V, but the next gain option is 2x which only allows up to 2.048V
        ads.setGain(GAIN_ONE);
        // Begin ADC
        ads.begin();

        // Print out the calibration curve
        MS_DBG(F("Input calibration Curve: "));
        MS_DBG(_x2_coeff_A, F("x^2 + "), _x1_coeff_B, F("x + "), _x0_coeff_C, '\n');

        // Read Analog to Digital Converter (ADC)
        // Taking this reading includes the 8ms conversion delay.
        // We're allowing the ADS1115 library to do the bit-to-volts conversion for us
        adcVoltage = ads.readADC_SingleEnded_V(_dataPin);  // Getting the reading
        MS_DBG(F("ads.readADC_SingleEnded_V("), _dataPin, F("): "), adcVoltage, F("\t\t"));

        if (adcVoltage < 3.6 and adcVoltage > -0.3)  // Skip results out of range
        {
            // Apply the unique calibration curve for the given sensor
            calibResult = (_x2_coeff_A * sq(adcVoltage)) + (_x1_coeff_B * adcVoltage) + _x0_coeff_C;
            MS_DBG(F("calibResult: "), calibResult, '\n');
        }
        else  // set invalid voltages back to -9999
        {
            adcVoltage = -9999;
            MS_DBG('\n');
        }
    }
    else MS_DBG(getSensorName(), F(" at "), getSensorLocation(),
         F(" is not currently measuring!\n"));

    verifyAndAddMeasurementResult(OBS3_TURB_VAR_NUM, calibResult);
    verifyAndAddMeasurementResult(OBS3_VOLTAGE_VAR_NUM, adcVoltage);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    if (adcVoltage < 3.6 and adcVoltage > -0.3) return true;
    else return false;
}
