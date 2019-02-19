/*
 * AtlasScientificDO.cpp
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The Atlas Scientifc DO sensor outputs DO in both mg/L and percent saturation
 *     Accuracy is ± 0.05 mg/L
 *     Range is 0.01 − 100+ mg/L (0.1 − 400+ % saturation)
 *     Resolution is 0.01 mg/L or 0.1 % saturation
 */

// Included Dependencies
#include "AtlasScientificDO.h"

// Constructor
AtlasScientificDO::AtlasScientificDO(int8_t powerPin, uint8_t i2cAddressHex,
                                     uint8_t measurementsToAverage)
  : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                "AtlasScientificDO", ATLAS_DO_NUM_VARIABLES,
                ATLAS_DO_WARM_UP_TIME_MS, ATLAS_DO_STABILIZATION_TIME_MS,
                ATLAS_DO_MEASUREMENT_TIME_MS)
{}
// Destructor
AtlasScientificDO::~AtlasScientificDO(){}


// Setup
bool AtlasScientificDO::setup()
{
    bool success = Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // We want to turn on all possible measurement parameters
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    waitForWarmUp();

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report O2 concentration"));
    Wire.beginTransmission(_i2cAddressHex);  // Transmit to the sensor
    success &= Wire.write("O,mg,1");  // Enable concentration in mg/L
    success &= !Wire.endTransmission();  // Finish
    // NOTE: The return of 0 from endTransmission indicates success

    delay(300);  // Sorry!  There's a processing delay X-P
    // NOTE:  Even if we wanted to send the command and walk away without
    // checking if it was successful, we'd still have to wait the processing
    // delay here in the setup just to ensure that the sensor is powered long
    // enough to implement the change.

    // Check that it was successful
    Wire.requestFrom(_i2cAddressHex, 3, 1);
    success &= (Wire.read() == 1);

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report O2 % saturation"));
    Wire.beginTransmission(_i2cAddressHex);  // Transmit to the sensor
    success &= Wire.write("O,%,1");  // Enable percent saturation
    success &= !Wire.endTransmission();  // Finish

    delay(300);

    // Check that it was successful
    Wire.requestFrom(_i2cAddressHex, 3, 1);
    success &= (Wire.read() == 1);

    if (!success)
    {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    return success;
}
