/**
 * @file SodaqUBeeU201.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the SodaqUBeeU201 class.
 */

// Included Dependencies
#include "SodaqUBeeU201.h"
#include "LoggerModemMacros.h"

// Constructor
SodaqUBeeU201::SodaqUBeeU201(Stream* modemStream, int8_t powerPin,
                             int8_t statusPin, int8_t modemResetPin,
                             int8_t modemSleepRqPin, const char* apn)
    : loggerModem(powerPin, statusPin, U201_STATUS_LEVEL, modemResetPin,
                  U201_RESET_LEVEL, U201_RESET_PULSE_MS, modemSleepRqPin,
                  U201_WAKE_LEVEL, U201_WAKE_PULSE_MS, U201_STATUS_TIME_MS,
                  U201_DISCONNECT_TIME_MS, U201_WAKE_DELAY_MS,
                  U201_ATRESPONSE_TIME_MS),
#ifdef MS_SODAQUBEEU201_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger),
#else
      gsmModem(*modemStream),
#endif
      gsmClient(gsmModem),
      _apn(apn) {
}

// Destructor
SodaqUBeeU201::~SodaqUBeeU201() {}

MS_IS_MODEM_AWAKE(SodaqUBeeU201);
MS_MODEM_WAKE(SodaqUBeeU201);

MS_MODEM_CONNECT_INTERNET(SodaqUBeeU201);
MS_MODEM_DISCONNECT_INTERNET(SodaqUBeeU201);
MS_MODEM_IS_INTERNET_AVAILABLE(SodaqUBeeU201);

MS_MODEM_GET_NIST_TIME(SodaqUBeeU201);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(SodaqUBeeU201);
MS_MODEM_GET_MODEM_BATTERY_DATA(SodaqUBeeU201);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(SodaqUBeeU201);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SodaqUBeeU201::modemWakeFxn(void) {
    // SARA/LISA U2/G2 and SARA G3 series turn on when power is applied
    // No pulsing required in this case
    if (_powerPin >= 0) { return true; }
    if (_modemSleepRqPin >= 0) {
        MS_DBG(F("Sending a"), _wakePulse_ms, F("ms"),
               _wakeLevel ? F("HIGH") : F("LOW"), F("wake-up pulse on pin"),
               _modemSleepRqPin, F("for Sodaq UBee U201"));
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        // 50-80µs pulse for wake on SARA/LISA U2/G2
        delayMicroseconds(_wakePulse_ms);
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        return true;
    } else {
        return true;
    }
}


bool SodaqUBeeU201::modemSleepFxn(void) {
    if (_powerPin >= 0 || _modemSleepRqPin >= 0) {
        // will go on with power on
        // Easiest to just go to sleep with the AT command rather than using
        // pins
        MS_DBG(F("Asking u-blox SARA U201 to power down"));
        return gsmModem.poweroff();
    } else {  // DON'T go to sleep if we can't wake up!
        return true;
    }
}

bool SodaqUBeeU201::extraModemSetup(void) {
    bool success = gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    // Turn on network indicator light
    // Pin 16 = GPIO1, function 2 = network status indication
    gsmModem.sendAT(GF("+UGPIOC=16,2"));
    gsmModem.waitResponse();
    return success;
}
