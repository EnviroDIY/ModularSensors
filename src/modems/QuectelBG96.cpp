/**
 * @file QuectelBG96.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the QuectelBG96 class.
 */

// Included Dependencies
#include "QuectelBG96.h"
#include "LoggerModemMacros.h"

// Constructor
QuectelBG96::QuectelBG96(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                         int8_t modemResetPin, int8_t modemSleepRqPin,
                         const char* apn)
    : loggerModem(powerPin, statusPin, BG96_STATUS_LEVEL, modemResetPin,
                  BG96_RESET_LEVEL, BG96_RESET_PULSE_MS, modemSleepRqPin,
                  BG96_WAKE_LEVEL, BG96_WAKE_PULSE_MS, BG96_STATUS_TIME_MS,
                  BG96_DISCONNECT_TIME_MS, BG96_WAKE_DELAY_MS,
                  BG96_ATRESPONSE_TIME_MS),
#ifdef MS_QUECTELBG96_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger),
#else
      gsmModem(*modemStream),
#endif
      gsmClient(gsmModem),
      _apn(apn) {
}

// Destructor
QuectelBG96::~QuectelBG96() {}

MS_MODEM_EXTRA_SETUP(QuectelBG96);
MS_IS_MODEM_AWAKE(QuectelBG96);
MS_MODEM_WAKE(QuectelBG96);

MS_MODEM_CONNECT_INTERNET(QuectelBG96);
MS_MODEM_DISCONNECT_INTERNET(QuectelBG96);
MS_MODEM_IS_INTERNET_AVAILABLE(QuectelBG96);

MS_MODEM_GET_NIST_TIME(QuectelBG96);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(QuectelBG96);
MS_MODEM_GET_MODEM_BATTERY_DATA(QuectelBG96);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(QuectelBG96);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool QuectelBG96::modemWakeFxn(void) {
    // Must power on and then pulse on
    if (_modemSleepRqPin >= 0) {
        MS_DBG(F("Sending a"), _wakePulse_ms, F("ms"),
               _wakeLevel ? F("HIGH") : F("LOW"), F("wake-up pulse on pin"),
               _modemSleepRqPin, F("for"), _modemName);
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        delay(_wakePulse_ms);  // ≥100ms
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        return gsmModem.waitResponse(10000L, GF("RDY")) == 1;
    }
    return true;
}


bool QuectelBG96::modemSleepFxn(void) {
    if (_modemSleepRqPin >= 0) {
        // BG96 must have access to `PWRKEY` pin to sleep
        // Easiest to just go to sleep with the AT command rather than using
        // pins
        return gsmModem.poweroff();
    }
    return true;  // DON'T go to sleep if we can't wake up!
}

bool QuectelBG96::modemHardReset(void) {
    digitalWrite(_modemSleepRqPin, !_wakeLevel);  // set the wake pin high
    bool success = loggerModem::modemHardReset();
    if (success) { return gsmModem.waitResponse(10000L, GF("RDY")) == 1; }
    return false;
}
