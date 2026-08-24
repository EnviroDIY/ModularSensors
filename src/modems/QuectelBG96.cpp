/**
 * @file QuectelBG96.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the QuectelBG96 class.
 */

// Included Dependencies
#include "QuectelBG96.h"

// Constructor
QuectelBG96::QuectelBG96(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                         int8_t modemResetPin, int8_t modemSleepRqPin,
                         const char* apn)
    : loggerModemImpl<TinyGsmBG96,                       // Modem Type
                      TinyGsmBG96::GsmClientBG96,        // TCP Client Type
                      TinyGsmBG96::GsmClientSecureBG96,  // SSL Client Type
                      false  // signal quality is RSSI
                      >(modemStream, powerPin, statusPin, BG96_STATUS_LEVEL,
                        modemResetPin, BG96_RESET_LEVEL, BG96_RESET_PULSE_MS,
                        modemSleepRqPin, BG96_WAKE_LEVEL, BG96_WAKE_PULSE_MS,
                        BG96_STATUS_TIME_MS, BG96_DISCONNECT_TIME_MS,
                        BG96_WAKE_DELAY_MS, BG96_AT_RESPONSE_TIME_MS),
      _apn(apn) {}


bool QuectelBG96::connectWithCredentials() {
    return gsmModem.gprsConnect(_apn, "", "");
}

bool QuectelBG96::modemSleepFxn() {
    // Don't go to sleep if we cannot wake up again.
    if (_modemSleepRqPin < 0 && _powerPin < 0) {
        MS_DBG(F("No way to wake the BG96; skipping power down"));
        return true;
    }
    /** Run the sleep AT command.
        Use AT+QPOWD=1 for normal power down. */
    return gsmModem.powerOff();
}

bool QuectelBG96::modemWakeFxn() {
    /** For most modules, the wake pin is the same pin as the power pin */
    if (_modemSleepRqPin >= 0) {
        MS_DBG(F("Wake pin"), _modemSleepRqPin, F("already at"),
               digitalRead(_modemSleepRqPin) ? F("HIGH") : F("LOW"));
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        MS_DBG(F("Setting pin to"), _wakeLevel ? F("HIGH") : F("LOW"), F("for"),
               _wakePulse_ms, F("ms"));
        delay(_wakePulse_ms);
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        return true;
    } else {
        return true;
    }
}

bool QuectelBG96::modemHardReset() {
    /** Check for a reset pin */
    if (_modemResetPin >= 0) {
        MS_DBG(F("Doing a hard reset on the modem by setting pin"),
               _modemResetPin, _resetLevel ? F("HIGH") : F("LOW"), F("for"),
               _resetPulse_ms, F("ms"));
        digitalWrite(_modemResetPin, _resetLevel);
        delay(_resetPulse_ms);
        digitalWrite(_modemResetPin, !_resetLevel);
        delay(_statusTime_ms);
        return true;
    } else {
        MS_DBG(F("No reset pin is available on pin"), _modemResetPin);
        return false;
    }
}

bool QuectelBG96::extraModemSetup() {
    // Perform any extra setup required for the BG96 modem
    return true;
}
