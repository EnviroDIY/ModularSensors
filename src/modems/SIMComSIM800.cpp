/**
 * @file SIMComSIM800.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SIMComSIM800 class.
 */

// Included Dependencies
#include "SIMComSIM800.h"

// Constructor
SIMComSIM800::SIMComSIM800(Stream* modemStream, int8_t powerPin,
                           int8_t statusPin, int8_t modemResetPin,
                           int8_t modemSleepRqPin, const char* apn)
    : loggerModemImpl<TinyGsmSim800,                         // Modem Type
                      TinyGsmSim800::GsmClientSim800,        // TCP Client Type
                      TinyGsmSim800::GsmClientSecureSim800,  // SSL Client Type
                      false  // signal quality is RSSI
                      >(modemStream, powerPin, statusPin, SIM800_STATUS_LEVEL,
                        modemResetPin, SIM800_RESET_LEVEL,
                        SIM800_RESET_PULSE_MS, modemSleepRqPin,
                        SIM800_WAKE_LEVEL, SIM800_WAKE_PULSE_MS,
                        SIM800_STATUS_TIME_MS, SIM800_DISCONNECT_TIME_MS,
                        SIM800_WAKE_DELAY_MS, SIM800_AT_RESPONSE_TIME_MS),

      _apn(apn) {}


bool SIMComSIM800::connectWithCredentials() {
    return gsmModem.gprsConnect(_apn, "", "");
}


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SIMComSIM800::modemWakeFxn() {
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


bool SIMComSIM800::modemSleepFxn() {
    if (_modemSleepRqPin >= 0) {
        // Must have access to `PWRKEY` (_modemSleepRqPin) pin to wake up; don't
        // sleep without it.
        // Easiest to just go to sleep with the AT command rather than using
        // pins
        MS_DBG(F("Asking SIM800 to power down"));
        bool res = gsmModem.powerOff();
        gsmModem.stream.flush();
        return res;
    } else {  // DON'T go to sleep if we can't wake up!
        gsmModem.stream.flush();
        return true;  // nothing's wrong with sleeping, we just won't do it!
    }
}

bool SIMComSIM800::extraModemSetup() {
    // Perform any extra setup required for the SIM800 modem
    return true;
}
