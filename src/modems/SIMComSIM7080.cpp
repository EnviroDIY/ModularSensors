/**
 * @file SIMComSIM7080.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SIMComSIM7080 class.
 */

// Included Dependencies
#include "SIMComSIM7080.h"
#include "LoggerModemMacros.h"

// Constructor
SIMComSIM7080::SIMComSIM7080(Stream* modemStream, int8_t powerPin,
                             int8_t statusPin, int8_t modemSleepRqPin,
                             const char* apn)
    : loggerModem(powerPin, statusPin, SIM7080_STATUS_LEVEL, modemSleepRqPin,
                  SIM7080_RESET_LEVEL, SIM7080_RESET_PULSE_MS, modemSleepRqPin,
                  SIM7080_WAKE_LEVEL, SIM7080_WAKE_PULSE_MS,
                  SIM7080_STATUS_TIME_MS, SIM7080_DISCONNECT_TIME_MS,
                  SIM7080_WAKE_DELAY_MS, SIM7080_ATRESPONSE_TIME_MS),
#ifdef MS_SIMCOMSIM7080_DEBUG_DEEP
      _modemATDebugger(*modemStream, MS_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger),
#else
      gsmModem(*modemStream),
#endif
      _apn(apn) {
}

// Destructor
SIMComSIM7080::~SIMComSIM7080() {}

bool SIMComSIM7080::extraModemSetup(void) {
    bool success = gsmModem.init();
    _modemName   = gsmModem.getModemName();

    // The modem is liable to crash if the send buffer overflows and TinyGSM
    // offers no way to know when that might happen. Reduce the chance of
    // problems by maxing out the send buffer size. This size should accommodate
    // a completely full 8K LogBuffer and a crappy connection.
    gsmModem.sendAT(F("+CACFG=\"SNDBUF\",29200"));
    gsmModem.waitResponse();

    // Enable the netlight indicator
    gsmModem.sendAT(F("+CNETLIGHT=1"));
    gsmModem.waitResponse();
    // Enable netlight indication of GPRS status
    // Enable, the netlight will be forced to enter into 64ms on/300ms off
    // blinking state in GPRS data transmission service.Otherwise,  the netlight
    // state is not restricted.
    gsmModem.sendAT(F("+CNETLIGHT=1"));
    gsmModem.waitResponse();

    // Enable the battery check functionality
    gsmModem.sendAT(F("+CBATCHK=1"));
    gsmModem.waitResponse();

    return success;
}

MS_IS_MODEM_AWAKE(SIMComSIM7080);
MS_MODEM_WAKE(SIMComSIM7080);

MS_MODEM_CONNECT_INTERNET(SIMComSIM7080);
MS_MODEM_DISCONNECT_INTERNET(SIMComSIM7080);
MS_MODEM_IS_INTERNET_AVAILABLE(SIMComSIM7080);

MS_MODEM_CREATE_CLIENT(SIMComSIM7080);
MS_MODEM_CREATE_SECURE_CLIENT(SIMComSIM7080);

MS_MODEM_GET_NIST_TIME(SIMComSIM7080);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(SIMComSIM7080);
MS_MODEM_GET_MODEM_BATTERY_DATA(SIMComSIM7080);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(SIMComSIM7080);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SIMComSIM7080::modemWakeFxn(void) {
    // Must power on and then pulse on
    if (_modemSleepRqPin >= 0) {
        MS_DBG(F("Sending a"), _wakePulse_ms, F("ms"),
               _wakeLevel ? F("HIGH") : F("LOW"), F("wake-up pulse on pin"),
               _modemSleepRqPin, F("for"), _modemName);
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        delay(_wakePulse_ms);  // >1s
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        int ready_response = gsmModem.waitResponse(30000L, GF("SMS Ready"),
                                                   GF("+CPIN: NOT INSERTED"));
        if (ready_response == 1) {
            MS_DBG(F("Got SMS Ready indicating modem is awake and ready"));
        } else if (ready_response == 2) {
            MS_DBG(F("Got +CPIN: NOT INSERTED indicating modem is awake and "
                     "ready but has no SIM card"));
        } else {
            MS_DBG(F("Didn't get expected finish URC for modem wake!"));
        }

        return ready_response == 1 || ready_response == 2;
    }
    return true;
}


bool SIMComSIM7080::modemSleepFxn(void) {
    if (_modemSleepRqPin >= 0) {
        // Must have access to `PWRKEY` pin to sleep
        // Easiest to just go to sleep with the AT command rather than using
        // pins
        MS_DBG(F("Asking SIM7080 to power down"));
        return gsmModem.poweroff();
    } else {  // DON'T go to sleep if we can't wake up!
        return true;
    }
}
