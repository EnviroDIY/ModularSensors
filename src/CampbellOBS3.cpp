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
*/

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "CampbellOBS3.h"

// The constructor - need the power pin, the data pin, and the calibration info
CampbellOBS3::CampbellOBS3(int powerPin, int dataPin, float A, float B, float C)
  : SensorBase(dataPin, powerPin)
{
    _A = A;
    _B = B;
    _C = C;
}

// The sensor installation location on the Mayfly
String CampbellOBS3::getSensorLocation(void)
{
    sensorLocation = "ads" + String(_dataPin);
    return sensorLocation;
}

// The static variables that need to be updated
// float CampbellOBS3::sensorValue = 0;
// unsigned long CampbellOBS3::sensorLastUpdated;

// Uses Auxillary ADD to convert data
bool CampbellOBS3::update(){

    // Start the Auxillary ADD
    Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */
    ads.begin();

    // Check if the power is on, turn it on if not
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}

    // Variables to store the results in
    int16_t adcResult = 0;
    float voltage = 0;
    float calibResult = 0;

    adcResult = ads.readADC_SingleEnded(_dataPin);  // Getting the reading

    // Serial.print("ads.readADC_SingleEnded(");  // For debugging
    // Serial.print(_dataPin);  // For debugging
    // Serial.print("): ");  // For debugging
    // Serial.println(ads.readADC_SingleEnded(_dataPin));  // For debugging

    // now convert bits into millivolts
    // 3.3 is the voltage applied to the sensor (and its returun range)
    // The 17585 is the default bit gain of the ADS1115
    voltage = (adcResult * 3.3) / 17585.0;
    // Serial.print("Voltage: ");  // For debugging
    // Serial.println(String(voltage, 6));  // For debugging

    calibResult = (_A * square (voltage)) + (_B * voltage) - _C;
    // Serial.print("calibResult: ");  // For debugging
    // Serial.println(calibResult);  // For debugging

    sensorValue = calibResult;
    // Serial.print("CampbellOBS3::sensorValue: ");  // For debugging
    // Serial.println(CampbellOBS3::sensorValue);  // For debugging
    sensorLastUpdated = millis();

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    // Return true when finished
    return true;
}

float CampbellOBS3::getValue(void)
{
    checkForUpdate(sensorLastUpdated);
    return sensorValue;
}




CampbellOBS3_Turbidity::CampbellOBS3_Turbidity(int powerPin, int dataPin, float A, float B, float C)
  : SensorBase(dataPin, powerPin, 3, F("CampbellOBS3+"), F("turbidity"), F("nephelometricTurbidityUnit"), F("TurbLow")),
    CampbellOBS3(powerPin, dataPin, A, B, C)
{}




CampbellOBS3_TurbHigh::CampbellOBS3_TurbHigh(int powerPin, int dataPin, float A, float B, float C)
  : SensorBase(dataPin, powerPin, 2, F("CampbellOBS3+"), F("turbidity"), F("nephelometricTurbidityUnit"), F("TurbHigh")),
    CampbellOBS3(powerPin, dataPin, A, B, C)
{}
