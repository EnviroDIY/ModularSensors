/**
 * @file TIINA219.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Neil Hancock
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the TIINA219 class.
 */

#include "TIINA219.h"

// The constructors
TIINA219::TIINA219(TwoWire* theI2C, int8_t powerPin, uint8_t i2cAddressHex,
                   uint8_t measurementsToAverage)
    : Sensor("TIINA219", INA219_NUM_VARIABLES, INA219_WARM_UP_TIME_MS,
             INA219_STABILIZATION_TIME_MS, INA219_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage),
      _i2cAddressHex(i2cAddressHex),
      _i2c(theI2C) {}
TIINA219::TIINA219(int8_t powerPin, uint8_t i2cAddressHex,
                   uint8_t measurementsToAverage)
    : Sensor("TIINA219", INA219_NUM_VARIABLES, INA219_WARM_UP_TIME_MS,
             INA219_STABILIZATION_TIME_MS, INA219_MEASUREMENT_TIME_MS, powerPin,
             -1, measurementsToAverage, INA219_INC_CALC_VARIABLES),
      _i2cAddressHex(i2cAddressHex),
      _i2c(&Wire) {}
// Destructor
TIINA219::~TIINA219() {}


String TIINA219::getSensorLocation(void) {
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool TIINA219::setup(void) {
    bool wasOn;
    Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    wasOn = checkPowerOn();
    if (!wasOn) {
        powerUp();
        waitForWarmUp();
    }

    ina219_phy.begin(_i2c);

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return true;
}


bool TIINA219::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    // Begin/Init needs to be rerun after every power-up to set the calibration
    // coefficient for the INA219 (see p21 of datasheet)
    ina219_phy.begin(_i2c);

    return true;
}


bool TIINA219::addSingleMeasurementResult(void) {
    bool success = false;

    // Initialize float variables
    float current_mA = -9999;
    float busV_V     = -9999;
    float power_mW   = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // Read values
        current_mA = ina219_phy.getCurrent_mA();
        if (isnan(current_mA)) current_mA = -9999;
        busV_V = ina219_phy.getBusVoltage_V();
        if (isnan(busV_V)) busV_V = -9999;
        power_mW = ina219_phy.getPower_mW();
        if (isnan(power_mW)) power_mW = -9999;

        success = true;

        MS_DBG(F("  Current [mA]:"), current_mA);
        MS_DBG(F("  Bus Voltage [V]:"), busV_V);
        MS_DBG(F("  Power [mW]:"), power_mW);
    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(INA219_CURRENT_MA_VAR_NUM, current_mA);
    verifyAndAddMeasurementResult(INA219_BUS_VOLTAGE_VAR_NUM, busV_V);
    verifyAndAddMeasurementResult(INA219_POWER_MW_VAR_NUM, power_mW);


    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
