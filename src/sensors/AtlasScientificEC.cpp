/**
 * @file AtlasScientificEC.cpp
 * @brief Implements the AtlasScientificEC class.
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 */

// Included Dependencies
#include "AtlasScientificEC.h"

// Constructor
AtlasScientificEC::AtlasScientificEC(int8_t powerPin, uint8_t i2cAddressHex,
                                     uint8_t measurementsToAverage)
    : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                  "AtlasScientificEC", ATLAS_COND_NUM_VARIABLES,
                  ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS,
                  ATLAS_COND_MEASUREMENT_TIME_MS) {}
// Destructor
AtlasScientificEC::~AtlasScientificEC() {}


// Setup
bool AtlasScientificEC::setup() {
    bool success =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    // We want to turn on all possible measurement parameters
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    MS_DBG(F("Asking"), getSensorNameAndLocation(),
           F("to report conductivity"));
    Wire.beginTransmission(_i2cAddressHex);
    success &= Wire.write((const uint8_t*)"O,EC,1", 6);  // Enable conductivity
    success &= !Wire.endTransmission();
    success &= waitForProcessing();

    MS_DBG(F("Asking"), getSensorNameAndLocation(),
           F("to report total dissolved solids"));
    Wire.beginTransmission(_i2cAddressHex);
    success &= Wire.write((const uint8_t*)"O,TDS,1",
                          7);  // Enable total dissolved solids
    success &= !Wire.endTransmission();
    success &= waitForProcessing();

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report salinity"));
    Wire.beginTransmission(_i2cAddressHex);
    success &= Wire.write((const uint8_t*)"O,S,1", 5);  // Enable salinity
    success &= !Wire.endTransmission();
    success &= waitForProcessing();

    MS_DBG(F("Asking"), getSensorNameAndLocation(),
           F("to report specific gravity"));
    Wire.beginTransmission(_i2cAddressHex);
    success &= Wire.write((const uint8_t*)"O,SG,1",
                          6);  // Enable specific gravity
    success &= !Wire.endTransmission();
    success &= waitForProcessing();

    if (!success) {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return success;
}
