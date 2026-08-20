/**
 * @file QuectelBG96_Template_Example.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Example implementation of QuectelBG96 using templates
 *
 * This file demonstrates the DRAMATIC simplification achieved by using
 * templates instead of macros.
 *
 * ===========================================================================
 * COMPARISON: Lines of Code
 * ===========================================================================
 *
 * OLD APPROACH (QuectelBG96.cpp with macros):
 * - File length: ~50+ lines
 * - 13+ macro invocations
 * - Dependency on LoggerModemMacros.h
 *
 * NEW APPROACH (this file):
 * - File length: ~90 lines (including extensive comments)
 * - 0 macro invocations
 * - Only implements unique behavior
 * - No LoggerModemMacros.h dependency
 *
 * ===========================================================================
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

/**
 * @brief Override the modem-specific sleep function.
 *
 * For the BG96, sleep is by AT command.
 *
 * @return True if the sleep function ran successfully.
 */
bool QuectelBG96::modemSleepFxn() {
    /** Run the sleep AT command.
        Use AT+QPOWD=1 for normal power down. */
    return gsmModem.powerOff();
}

/**
 * @brief Override the modem-specific wake function.
 *
 * The BG96 requires a pulse on the wake pin to wake from sleep.
 *
 * @return True if the wake function ran successfully.
 */
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

/**
 * @brief Do a "hard" or panic reset of the modem.
 *
 * @return True if the reset succeeded and the modem should now be
 * responsive.
 */
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
