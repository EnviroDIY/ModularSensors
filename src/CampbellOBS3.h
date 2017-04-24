/*
 *CampbellOBS3.h
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

#ifndef CampbellOBS3_h
#define CampbellOBS3_h

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
#include "SensorBase.h"

#define OBS3_NUM_VARIABLES 1  // low and high range are treated as completely independent
#define OBS3_TURB_VAR_NUM 0
#define OBS3_RESOLUTION 3
#define OBS3_HR_RESOLUTION 2

// The main class for the Campbell OBS3
class CampbellOBS3 : public virtual Sensor
{
public:
    // The constructor - need the power pin, the data pin, and the calibration info
    CampbellOBS3(int powerPin, int dataPin, float A, float B, float C)
      : Sensor(dataPin, powerPin, F("CampbellOBS3+"), OBS3_NUM_VARIABLES)
    {
        _A = A;
        _B = B;
        _C = C;
    }

    String getSensorLocation(void) override
    {
        String sensorLocation = "ads" + String(_dataPin);
        return sensorLocation;
    }

    bool update(void) override
    {

        // Start the Auxillary ADD
        Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */
        ads.begin();

        // Check if the power is on, turn it on if not
        bool wasOn = checkPowerOn();
        if(!wasOn){powerUp();}

        // Clear values before starting loop
        clearValues();

        // Variables to store the results in
        int16_t adcResult = 0;
        float voltage = 0;
        float calibResult = 0;

        adcResult = ads.readADC_SingleEnded(_dataPin);  // Getting the reading

        // Serial.print(F("ads.readADC_SingleEnded("));  // For debugging
        // Serial.print(_dataPin);  // For debugging
        // Serial.print(F("): "));  // For debugging
        // Serial.println(ads.readADC_SingleEnded(_dataPin));  // For debugging

        // now convert bits into millivolts
        // 3.3 is the voltage applied to the sensor (and its returun range)
        // The 17585 is the default bit gain of the ADS1115
        voltage = (adcResult * 3.3) / 17585.0;
        // Serial.print("Voltage: ");  // For debugging
        // Serial.println(String(voltage, 6));  // For debugging

        calibResult = (_A * square (voltage)) + (_B * voltage) - _C;
        // Serial.print(F("Calibration Curve: "));  // For debugging
        // Serial.print(_A);  // For debugging
        // Serial.print(F("x^2 + "));  // For debugging
        // Serial.print(_B);  // For debugging
        // Serial.print(F("x + "));  // For debugging
        // Serial.println(_C);  // For debugging
        // Serial.print(F("calibResult: "));  // For debugging
        // Serial.println(calibResult);  // For debugging

        sensorValues[0] = calibResult;

        // Turn the power back off it it had been turned on
        if(!wasOn){powerDown();}

        // Update the registered variables with the new values
        notifyVariables();

        // Return true when finished
        return true;
    }

protected:
    float _A;
    float _B;
    float _C;
};


// Subclasses are ONLY needed because of the different dreamhost column tags
// All that is needed for these are the constructors
// Defines the "Low Turbidity Sensor"
class CampbellOBS3_Turbidity : public virtual Variable
{
public:
    CampbellOBS3_Turbidity(Sensor *parentSense)
      : Variable(parentSense, OBS3_TURB_VAR_NUM,
                 F("turbidity"), F("nephelometricTurbidityUnit"),
                 OBS3_RESOLUTION, F("TurbLow"))
    {}
};


// Defines the "High Turbidity Sensor"
class CampbellOBS3_TurbHigh : public virtual Variable
{
public:
    CampbellOBS3_TurbHigh(Sensor *parentSense)
      : Variable(parentSense, OBS3_TURB_VAR_NUM,
                 F("turbidity"), F("nephelometricTurbidityUnit"),
                 OBS3_HR_RESOLUTION, F("TurbHigh"))
    {}
};

#endif
