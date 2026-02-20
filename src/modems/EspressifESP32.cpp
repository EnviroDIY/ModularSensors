/**
 * @file EspressifESP32.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the EspressifESP32 class.
 */

// Included Dependencies
#include "EspressifESP32.h"
#include "LoggerModemMacros.h"

// Constructors
EspressifESP32::EspressifESP32(Stream* modemStream, int8_t powerPin,
                               int8_t modemResetPin, const char* ssid,
                               const char* pwd)
    : Espressif(modemStream, powerPin, modemResetPin, ssid, pwd),
#if defined(MS_ESPRESSIFESP32_DEBUG_DEEP)
      _modemATDebugger(*modemStream, MS_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger)
#else
      gsmModem(*modemStream)
#endif
{
}

// Destructor
EspressifESP32::~EspressifESP32() {}

MS_IS_MODEM_AWAKE(EspressifESP32);
MS_MODEM_WAKE(EspressifESP32);

MS_MODEM_CONNECT_INTERNET(EspressifESP32, ESPRESSIF_RECONNECT_TIME_MS);
MS_MODEM_DISCONNECT_INTERNET(EspressifESP32);
MS_MODEM_IS_INTERNET_AVAILABLE(EspressifESP32);

MS_MODEM_CREATE_CLIENT(EspressifESP32, ESP32);
MS_MODEM_DELETE_CLIENT(EspressifESP32, ESP32);
MS_MODEM_CREATE_SECURE_CLIENT(EspressifESP32, ESP32);
MS_MODEM_DELETE_SECURE_CLIENT(EspressifESP32, ESP32);

MS_MODEM_GET_NIST_TIME(EspressifESP32, ESP32);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(EspressifESP32);
MS_MODEM_GET_MODEM_BATTERY_DATA(EspressifESP32);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(EspressifESP32);

bool EspressifESP32::modemSleepFxn(void) {
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake
    // from deep sleep.  We'll also put it in deep sleep before yanking power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for ESP32"));
        bool retVal = gsmModem.poweroff();
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        _modemStream->flush();
        return retVal;
    } else {  // DON'T go to sleep if we can't wake up!
        MS_DEEP_DBG(F("No pins for sleeping the ESP32. Hopefully it's in the "
                      "state you want."));
        _modemStream->flush();
        return true;
    }
}

// Set up the light-sleep status pin, if applicable
bool EspressifESP32::extraModemSetup(void) {
    if (_modemSleepRqPin >= 0) { digitalWrite(_modemSleepRqPin, !_wakeLevel); }
    gsmModem.init();
    _modemName = gsmModem.getModemName();
    // AT+CWCOUNTRY=<country_policy>,<country_code>,<start_channel>,<total_channel_count>
    // <country_policy>:
    //     0: will change the county code to be the same as the AP that the
    //     ESP32 is connected to.
    //     1: the country code will not change, always be the one set by
    //     command.
    // <country_code>: country code. Maximum length: 3 characters. Refer to ISO
    //     3166-1 alpha-2 for country codes.
    // <start_channel>: the channel number to start. Range: [1,14].
    // <total_channel_count>: total number of channels.
    gsmModem.sendAT(
        GF("+CWCOUNTRY=0,\"US\",1,13"));  // Set country code to default to US,
                                          // but allow to change if the AP is
    gsmModem.waitResponse();
    return true;
}

// cSpell:ignore CWCOUNTRY
