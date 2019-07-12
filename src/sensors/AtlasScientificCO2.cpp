/*
 * AtlasScientificCO2.cpp
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano
 *
 * The output from the Atlas Scientifc CO2 is the temperature in degrees C.
 *     Accuracy is ± 3% or ± 30 ppm
 *     Range is 0 − 10000 ppm
 *     Resolution is 1 ppm
 */

// Included Dependencies
#include "AtlasScientificCO2.h"

// Constructors
#if defined MS_ATLAS_SOFTWAREWIRE
AtlasScientificCO2::AtlasScientificCO2(SoftwareWire *theI2C, int8_t powerPin,
                                       uint8_t i2cAddressHex,
                                       uint8_t measurementsToAverage)
  : AtlasParent(theI2C, powerPin,
                i2cAddressHex, measurementsToAverage,
                "AtlasScientificCO2", ATLAS_CO2_NUM_VARIABLES,
                ATLAS_CO2_WARM_UP_TIME_MS, ATLAS_CO2_STABILIZATION_TIME_MS,
                ATLAS_CO2_MEASUREMENT_TIME_MS)
{}
AtlasScientificCO2::AtlasScientificCO2(int8_t powerPin, int8_t dataPin, int8_t clockPin,
                                       uint8_t i2cAddressHex,
                                       uint8_t measurementsToAverage)
  : AtlasParent(powerPin, dataPin, clockPin,
                i2cAddressHex, measurementsToAverage,
                "AtlasScientificCO2", ATLAS_CO2_NUM_VARIABLES,
                ATLAS_CO2_WARM_UP_TIME_MS, ATLAS_CO2_STABILIZATION_TIME_MS,
                ATLAS_CO2_MEASUREMENT_TIME_MS)
{}
#else
AtlasScientificCO2::AtlasScientificCO2(TwoWire *theI2C, int8_t powerPin,
                                       uint8_t i2cAddressHex,
                                       uint8_t measurementsToAverage)
  : AtlasParent(theI2C, powerPin,
                i2cAddressHex, measurementsToAverage,
                "AtlasScientificCO2", ATLAS_CO2_NUM_VARIABLES,
                ATLAS_CO2_WARM_UP_TIME_MS, ATLAS_CO2_STABILIZATION_TIME_MS,
                ATLAS_CO2_MEASUREMENT_TIME_MS)
{}
AtlasScientificCO2::AtlasScientificCO2(int8_t powerPin,
                                       uint8_t i2cAddressHex,
                                       uint8_t measurementsToAverage)
  : AtlasParent(powerPin,
               i2cAddressHex, measurementsToAverage,
                "AtlasScientificCO2", ATLAS_CO2_NUM_VARIABLES,
                ATLAS_CO2_WARM_UP_TIME_MS, ATLAS_CO2_STABILIZATION_TIME_MS,
                ATLAS_CO2_MEASUREMENT_TIME_MS)
{}
#endif
// Destructor
AtlasScientificCO2::~AtlasScientificCO2(){}


// Setup
bool AtlasScientificCO2::setup()
{
    bool success = Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // We want to turn on all possible measurement parameters
    bool wasOn = checkPowerOn();
    if (!wasOn) {powerUp();}
    waitForWarmUp();

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report temperature with CO2"));
    _i2c->beginTransmission(_i2cAddressHex);
    success &= _i2c->write((const uint8_t *)"O,t,1", 5);  // Enable temperature
    success &= !_i2c->endTransmission();
    // NOTE: The return of 0 from endTransmission indicates success
    success &= waitForProcessing();

    if (!success)
    {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }

    // Turn the power back off it it had been turned on
    if (!wasOn) {powerDown();}

    return success;
}
