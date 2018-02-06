/*
 *CampbellOBS3.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Campbell Scientific OBS-3+
 *This is dependent on the Adafruit ADS1015 library.
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
 *  16-bit ADC
 *      Turbidity: 0.004/0.01 NTU; 0.008/0.03 NTU; 0.01/0.06 NTU
 *  12-bit ADC
 *      Turbidity: 0.06/0.2 NTU; 0.1/0.5 NTU; 0.2/1.0 NTU
 *
 * Minimum stabilization time: 2s
 * Can return readings as fast as the ADC will return them (860/sec)
*/

#include "CampbellOBS3.h"
#include <Adafruit_ADS1015.h>


// The constructor - need the power pin, the data pin, and the calibration info
CampbellOBS3::CampbellOBS3(int powerPin, int dataPin, float A, float B, float C,
                           uint8_t i2cAddress, int readingsToAverage)
  : Sensor(F("CampbellOBS3"), OBS3_NUM_VARIABLES,
           OBS3_WARM_UP, OBS3_STABILITY, OBS3_RESAMPLE,
           powerPin, dataPin, readingsToAverage)
{
    _Avalue = A;
    _Bvalue = B;
    _Cvalue = C;
    _i2cAddress = i2cAddress;
}


String CampbellOBS3::getSensorLocation(void)
{
    String sensorLocation = F("ADS1115_Pin");
    sensorLocation += String(_dataPin);
    return sensorLocation;
}


bool CampbellOBS3::addSingleMeasurementResult(void)
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

    // now convert bits into millivolts
    // 3.3 is the voltage applied to the sensor (and its return range)
    // The 17585 is the default bit gain of the ADS1115
    voltage = (adcResult * 3.3) / 17585.0;
    MS_DBG("Voltage: ", String(voltage, 6), F("\t\t"));

    calibResult = (_Avalue * sq(voltage)) + (_Bvalue * voltage) + _Cvalue;
    MS_DBG(F("Calibration Curve: "));
    MS_DBG(_Avalue, F("x^2 + "), _Bvalue, F("x + "), _Cvalue, F("\n"));
    MS_DBG(F("calibResult: "), calibResult, F("\n"));

    sensorValues[OBS3_TURB_VAR_NUM] += calibResult;

    // Return true when finished
    return true;
}
