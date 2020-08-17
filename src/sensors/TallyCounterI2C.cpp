/*
 *TallyCounterI2C.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *Copyright 2020 Stroud Water Research Center (per LICENSE.md)
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for NorthernWidget's Tally external event counter
 *It is used to measure windspeed or rainfall, and dependent on Tally_Library
 *
 *Documentation for the sensor can be found at:
 * https://github.com/NorthernWidget-Skunkworks/Project-Tally​
 * https://github.com/NorthernWidget-Skunkworks/Tally_Library/tree/Dev_I2C
 *
 * For Wind Speed:
 *  Accuracy and resolution are dependent on the sensor used:
 *  Inspeed WS2R Version II Reed Switch Anemometer
 *    https://www.store.inspeed.com/Inspeed-Version-II-Reed-Switch-Anemometer-Sensor-Only-WS2R.htm
 *
 * Assume sensor is immediately stable
*/

#include "TallyCounterI2C.h"


// The constructor - because this is I2C, only need the power pin and address
// measurementsToAverage = 1, b/c Tally counts all events over interval
TallyCounterI2C::TallyCounterI2C(int8_t powerPin, uint8_t i2cAddressHex)
     : Sensor("TallyCounterI2C", TALLY_NUM_VARIABLES, TALLY_WARM_UP_TIME_MS,
              TALLY_STABILIZATION_TIME_MS, TALLY_MEASUREMENT_TIME_MS, powerPin,
              -1, 1) {
    _i2cAddressHex  = i2cAddressHex;
}
// Destructor
TallyCounterI2C::~TallyCounterI2C(){}


String TallyCounterI2C::getSensorLocation(void) {
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


bool TallyCounterI2C::setup(void) {
    bool retVal =
        Sensor::setup();  // this will set pin modes and the setup status bit

    // This sensor needs power for setup!
    bool wasOn = checkPowerOn();
    if (!wasOn) { powerUp(); }
    waitForWarmUp();

    // Run begin fxn because it returns true or false for success in contact
    // Make 5 attempts
    uint8_t ntries  = 0;
    bool    success = false;
    uint8_t Stat    = false; //Used to test for connectivity to Tally device
    while (!success && ntries < 5) {
        Stat = counter_internal.begin();
        counter_internal.Sleep();  //Engage auto-sleep mode between event counts
        counter_internal.Clear();  //Clear count to ensure valid first reading
        if(Stat == 0) success = true;
        ntries++;
    }
    if (!success) {
        // Set the status error bit (bit 7)
        _sensorStatus |= 0b10000000;
        // UN-set the set-up bit (bit 0) since setup failed!
        _sensorStatus &= 0b11111110;
    }
    retVal &= success;

    // Turn the power back off it it had been turned on
    if (!wasOn) { powerDown(); }

    return retVal;
}


bool TallyCounterI2C::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;


    return true;
}


bool TallyCounterI2C::addSingleMeasurementResult(void) {
    bool success = false;

    // Initialize variables
    int16_t events = -9999;  // Number of events

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6)) {
        MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

        // Read values
        // Read data from counter before clear

        events = counter_internal.Peek();
        if (isnan(events)) events = -9999;

        // Assume that if negative a failed response
        // May also return a very negative temp when receiving a bad response
        if (events < 0) {
            MS_DBG(F("All values 0 or bad, assuming sensor non-response!"));
            events = -9999;
        } else {
            success = true;
        }

        //Clear count value
        counter_internal.Clear();

        if (events < 0) events = -9999;  // If negetive value results, return failure

        MS_DBG(F("  Events:"), events);

    } else {
        MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
    }

    verifyAndAddMeasurementResult(TALLY_EVENTS_VAR_NUM, events);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}
