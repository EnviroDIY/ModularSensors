/**
 * @file TallyCounterI2C.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the TallyCounterI2C sensor subclass.
 */

#include "TallyCounterI2C.h"


// The constructor
TallyCounterI2C::TallyCounterI2C(int8_t powerPin, uint8_t i2cAddressHex)
    : Sensor("TallyCounterI2C", TALLY_NUM_VARIABLES, TALLY_WARM_UP_TIME_MS,
             TALLY_STABILIZATION_TIME_MS, TALLY_MEASUREMENT_TIME_MS, powerPin,
             -1, 1, TALLY_INC_CALC_VARIABLES),
      _i2cAddressHex(i2cAddressHex) {}
// Destructor
TallyCounterI2C::~TallyCounterI2C() {}


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
    uint8_t Stat    = false;  // Used to test for connectivity to Tally device
    while (!success && ntries < 5) {
        Stat = counter_internal.begin();
        counter_internal.Sleep();  // Engage auto-sleep mode between event
                                   // counts
        counter_internal.Clear();  // Clear count to ensure valid first reading
        if (Stat == 0) success = true;
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

        // Clear count value
        counter_internal.Clear();

        if (events < 0)
            events = -9999;  // If negetive value results, return failure

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
