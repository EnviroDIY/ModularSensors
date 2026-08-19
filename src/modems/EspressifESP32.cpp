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

// Constructors
EspressifESP32::EspressifESP32(Stream* modemStream, int8_t powerPin,
                               int8_t modemResetPin, const char* ssid,
                               const char* pwd)
    : Espressif<TinyGsmESP32,                       // Modem Type
                TinyGsmESP32::GsmClientESP32,       // TCP Client Type
                TinyGsmESP32::GsmClientSecureESP32  // SSL Client
                                                    // Type
                >(modemStream, powerPin, modemResetPin, ssid, pwd) {}


bool EspressifESP32::modemSleepFxn() {
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake
    // from deep sleep.  We'll also put it in deep sleep before yanking power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for ESP32"));
        bool retVal = gsmModem.powerOff();
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
bool EspressifESP32::extraModemSetup() {
    if (_modemSleepRqPin >= 0) { digitalWrite(_modemSleepRqPin, !_wakeLevel); }
    bool success = gsmModem.init();
    // Attempt to get the modem name even without a successful init
    // The full make and model won't be returned, but it will at least be
    // something that identifies the modem as an ESP32, which is helpful for
    // debugging.
    _modemName = gsmModem.getModemName();
    if (success) {
        // AT+CWCOUNTRY=<country_policy>,<country_code>,<start_channel>,<total_channel_count>
        // <country_policy>:
        //     0: will change the county code to be the same as the AP that the
        //     ESP32 is connected to.
        //     1: the country code will not change, always be the one set by
        //     command.
        // <country_code>: country code. Maximum length: 3 characters. Refer to
        //     ISO 3166-1 alpha-2 for country codes.
        // <start_channel>: the channel number to start. Range: [1,14].
        // <total_channel_count>: total number of channels.
        // We set the country code to default to US, but allow it to change if
        // the AP is in a different country.
        gsmModem.sendAT(GF("+CWCOUNTRY=0,\"US\",1,13"));
        success &= (gsmModem.waitResponse() == 1);
    }
    return success;
}

// cSpell:ignore CWCOUNTRY
