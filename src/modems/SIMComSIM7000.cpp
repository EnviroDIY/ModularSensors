/**
 * @file SIMComSIM7000.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SIMComSIM7000 class.
 */

// Included Dependencies
#include "SIMComSIM7000.h"

// Constructor
SIMComSIM7000::SIMComSIM7000(Stream* modemStream, int8_t powerPin,
                             int8_t statusPin, int8_t modemResetPin,
                             int8_t modemSleepRqPin, const char* apn)
    : loggerModemImpl<
          TinyGsmSim7000SSL,                             // Modem Type
          TinyGsmSim7000SSL::GsmClientSim7000SSL,        // TCP Client Type
          TinyGsmSim7000SSL::GsmClientSecureSim7000SSL,  // SSL Client Type
          false  // signal quality is RSSI
          >(modemStream, powerPin, statusPin, SIM7000_STATUS_LEVEL,
            modemResetPin, SIM7000_RESET_LEVEL, SIM7000_RESET_PULSE_MS,
            modemSleepRqPin, SIM7000_WAKE_LEVEL, SIM7000_WAKE_PULSE_MS,
            SIM7000_STATUS_TIME_MS, SIM7000_DISCONNECT_TIME_MS,
            SIM7000_WAKE_DELAY_MS, SIM7000_AT_RESPONSE_TIME_MS),
      _apn(apn) {}


bool SIMComSIM7000::connectWithCredentials() {
    return gsmModem.gprsConnect(_apn, "", "");
}


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SIMComSIM7000::modemWakeFxn() {
    // Must power on and then pulse on
    if (_modemSleepRqPin >= 0) {
        MS_DBG(F("Sending a"), _wakePulse_ms, F("ms"),
               _wakeLevel ? F("HIGH") : F("LOW"), F("wake-up pulse on pin"),
               _modemSleepRqPin, F("for"), _modemName);
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        delay(_wakePulse_ms);  // >1s
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
    }
    return true;
}


bool SIMComSIM7000::modemSleepFxn() {
    if (_modemSleepRqPin >= 0) {
        // Must have access to `PWRKEY` pin to wake up; don't go to sleep if we
        // can't wake up!
        // Easiest to just go to sleep with the AT command rather than using
        // pins
        MS_DBG(F("Asking SIM7000 to power down"));
        bool res = gsmModem.powerOff();
        gsmModem.stream.flush();
        return res;
    } else {  // DON'T go to sleep if we can't wake up!
        gsmModem.stream.flush();
        return true;
    }
}
