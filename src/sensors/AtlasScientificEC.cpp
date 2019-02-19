/*
 * AtlasScientificEC.cpp
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The Atlas Scientifc Conductivity sensor outputs raw conductivity, TDS,
 * salinity, and specific gravity
 *     Accuracy is ± 2%
 *     Range is 0.07 − 500,000+ μS/cm
 *     Resolution is 3 decimal places
 */

// Included Dependencies
#include "AtlasScientificEC.h"

// Constructor
AtlasScientificEC::AtlasScientificEC(int8_t powerPin, uint8_t i2cAddressHex,
                  uint8_t measurementsToAverage)
 : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                "AtlasScientificEC", ATLAS_COND_NUM_VARIABLES,
                ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS,
                ATLAS_COND_MEASUREMENT_TIME_MS)
{}
// Destructor
AtlasScientificEC::~AtlasScientificEC(){}


// Setup
bool AtlasScientificEC::setup()
{
    bool success = Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // We want to turn on all possible measurement parameters
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    waitForWarmUp();

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report conductivity"));
    Wire.beginTransmission(_i2cAddressHex);  // Transmit to the sensor
    success &= Wire.write("O,EC,1");  // Enable conductivity
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

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report total dissolved solids"));
    Wire.beginTransmission(_i2cAddressHex);  // Transmit to the sensor
    success &= Wire.write("O,TDS,1");  // Enable total dissolved solids
    success &= !Wire.endTransmission();  // Finish

    delay(300);

    // Check that it was successful
    Wire.requestFrom(_i2cAddressHex, 3, 1);
    success &= (Wire.read() == 1);

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report salinity"));
    Wire.beginTransmission(_i2cAddressHex);  // Transmit to the sensor
    success &= Wire.write("O,S,1");  // Enable salinity
    success &= !Wire.endTransmission();  // Finish

    delay(300);

    // Check that it was successful
    Wire.requestFrom(_i2cAddressHex, 3, 1);
    success &= (Wire.read() == 1);

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report specific gravity"));
    Wire.beginTransmission(_i2cAddressHex);  // Transmit to the sensor
    success &= Wire.write("O,SG,1");  // Enable specific gravity
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
