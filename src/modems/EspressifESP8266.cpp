/**
 * @file EspressifESP8266.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the EspressifESP8266 class.
 */

// Included Dependencies
#include "EspressifESP8266.h"
#include "LoggerModemMacros.h"

// Constructors
EspressifESP8266::EspressifESP8266(Stream* modemStream, int8_t powerPin,
                                   int8_t modemResetPin, const char* ssid,
                                   const char* pwd)
    : Espressif(modemStream, powerPin, modemResetPin, ssid, pwd),
#if defined(MS_ESPRESSIFESP8266_DEBUG_DEEP)
      _modemATDebugger(*modemStream, MS_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger)
#else
      gsmModem(*modemStream)
#endif
{
}

// Destructor
EspressifESP8266::~EspressifESP8266() {}

MS_IS_MODEM_AWAKE(EspressifESP8266);
MS_MODEM_WAKE(EspressifESP8266);

MS_MODEM_CONNECT_INTERNET(EspressifESP8266, ESPRESSIF_RECONNECT_TIME_MS);
MS_MODEM_DISCONNECT_INTERNET(EspressifESP8266);
MS_MODEM_IS_INTERNET_AVAILABLE(EspressifESP8266);

MS_MODEM_CREATE_CLIENT(EspressifESP8266);
MS_MODEM_DELETE_CLIENT(EspressifESP8266);
MS_MODEM_CREATE_SECURE_CLIENT(EspressifESP8266);
MS_MODEM_DELETE_SECURE_CLIENT(EspressifESP8266);

MS_MODEM_GET_NIST_TIME(EspressifESP8266);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(EspressifESP8266);
MS_MODEM_GET_MODEM_BATTERY_DATA(EspressifESP8266);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(EspressifESP8266);

bool EspressifESP8266::modemSleepFxn(void) {
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake
    // from deep sleep.  We'll also put it in deep sleep before yanking power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for ESP8266"));
        bool retVal = gsmModem.poweroff();
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        _modemStream->flush();
        return retVal;
    } else {  // DON'T go to sleep if we can't wake up!
        MS_DEEP_DBG(F("No pins for sleeping the ESP8266. Hopefully it's in the "
                      "state you want."));
        _modemStream->flush();
        return true;
    }
}

// Set up the light-sleep status pin, if applicable
bool EspressifESP8266::extraModemSetup(void) {
    if (_modemSleepRqPin >= 0) { digitalWrite(_modemSleepRqPin, !_wakeLevel); }
    gsmModem.init();
    _modemName = gsmModem.getModemName();
    return true;
}
