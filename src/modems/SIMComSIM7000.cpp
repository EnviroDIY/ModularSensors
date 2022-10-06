/**
 * @file SIMComSIM7000.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SIMComSIM7000 class.
 */

// Included Dependencies
#include "SIMComSIM7000.h"
#include "LoggerModemMacros.h"

// Constructor
SIMComSIM7000::SIMComSIM7000(Stream* modemStream, int8_t powerPin,
                             int8_t statusPin, int8_t modemResetPin,
                             int8_t modemSleepRqPin, const char* apn)
    : loggerModem(powerPin, statusPin, SIM7000_STATUS_LEVEL, modemResetPin,
                  SIM7000_RESET_LEVEL, SIM7000_RESET_PULSE_MS, modemSleepRqPin,
                  SIM7000_WAKE_LEVEL, SIM7000_WAKE_PULSE_MS,
                  SIM7000_STATUS_TIME_MS, SIM7000_DISCONNECT_TIME_MS,
                  SIM7000_WAKE_DELAY_MS, SIM7000_ATRESPONSE_TIME_MS),
#ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger),
#else
      gsmModem(*modemStream),
#endif
      gsmClient(gsmModem),
      _apn(apn) {
}

// Destructor
SIMComSIM7000::~SIMComSIM7000() {}

MS_MODEM_EXTRA_SETUP(SIMComSIM7000);
MS_IS_MODEM_AWAKE(SIMComSIM7000);
MS_MODEM_WAKE(SIMComSIM7000);

MS_MODEM_CONNECT_INTERNET(SIMComSIM7000);
MS_MODEM_DISCONNECT_INTERNET(SIMComSIM7000);
MS_MODEM_IS_INTERNET_AVAILABLE(SIMComSIM7000);

MS_MODEM_GET_NIST_TIME(SIMComSIM7000);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(SIMComSIM7000);
MS_MODEM_GET_MODEM_BATTERY_DATA(SIMComSIM7000);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(SIMComSIM7000);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SIMComSIM7000::modemWakeFxn(void) {
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


bool SIMComSIM7000::modemSleepFxn(void) {
    if (_modemSleepRqPin >= 0) {
        // Must have access to `PWRKEY` pin to sleep
        // Easiest to just go to sleep with the AT command rather than using
        // pins
        MS_DBG(F("Asking SIM7000 to power down"));
        return gsmModem.poweroff();
    } else {  // DON'T go to sleep if we can't wake up!
        return true;
    }
}
