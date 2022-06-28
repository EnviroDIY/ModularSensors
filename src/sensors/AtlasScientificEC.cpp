/**
 * @file AtlasScientificEC.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the AtlasScientificEC class.
 */

// Included Dependencies
#include "AtlasScientificEC.h"

// Constructors
AtlasScientificEC::AtlasScientificEC(TwoWire* theI2C, int8_t powerPin,
                                     uint8_t i2cAddressHex,
                                     uint8_t measurementsToAverage)
    : AtlasParent(theI2C, powerPin, i2cAddressHex, measurementsToAverage,
                  "AtlasScientificEC", ATLAS_COND_NUM_VARIABLES,
                  ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS,
                  ATLAS_COND_MEASUREMENT_TIME_MS,
                  ATLAS_COND_INC_CALC_VARIABLES) {}
AtlasScientificEC::AtlasScientificEC(int8_t powerPin, uint8_t i2cAddressHex,
                                     uint8_t measurementsToAverage)
    : AtlasParent(powerPin, i2cAddressHex, measurementsToAverage,
                  "AtlasScientificEC", ATLAS_COND_NUM_VARIABLES,
                  ATLAS_COND_WARM_UP_TIME_MS, ATLAS_COND_STABILIZATION_TIME_MS,
                  ATLAS_COND_MEASUREMENT_TIME_MS,
                  ATLAS_COND_INC_CALC_VARIABLES) {}

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
    _i2c->beginTransmission(_i2cAddressHex);
    // Enable conductivity
    success &= static_cast<bool>(_i2c->write((const uint8_t*)"O,EC,1", 6));
    success &= !static_cast<bool>(_i2c->endTransmission());
    success &= waitForProcessing();

    MS_DBG(F("Asking"), getSensorNameAndLocation(),
           F("to report total dissolved solids"));
    _i2c->beginTransmission(_i2cAddressHex);
    // Enable total dissolved solids
    success &= static_cast<bool>(_i2c->write((const uint8_t*)"O,TDS,1", 7));
    success &= !static_cast<bool>(_i2c->endTransmission());
    success &= waitForProcessing();

    MS_DBG(F("Asking"), getSensorNameAndLocation(), F("to report salinity"));
    _i2c->beginTransmission(_i2cAddressHex);
    // Enable salinity
    success &= static_cast<bool>(_i2c->write((const uint8_t*)"O,S,1", 5));
    success &= !static_cast<bool>(_i2c->endTransmission());
    success &= waitForProcessing();

    MS_DBG(F("Asking"), getSensorNameAndLocation(),
           F("to report specific gravity"));
    _i2c->beginTransmission(_i2cAddressHex);
    // Enable specific gravity
    success &= static_cast<bool>(_i2c->write((const uint8_t*)"O,SG,1", 6));
    success &= !static_cast<bool>(_i2c->endTransmission());
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
