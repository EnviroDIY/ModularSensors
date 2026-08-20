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

// Constructors
EspressifESP8266::EspressifESP8266(Stream* modemStream, int8_t powerPin,
                                   int8_t modemResetPin, const char* ssid,
                                   const char* pwd)
    : Espressif<TinyGsmESP8266,                         // Modem Type
                TinyGsmESP8266::GsmClientESP8266,       // TCP Client Type
                TinyGsmESP8266::GsmClientSecureESP8266  // SSL Client
                                                        // Type
                >(modemStream, powerPin, modemResetPin, ssid, pwd) {}


bool EspressifESP8266::modemSleepFxn() {
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake
    // from deep sleep.  We'll also put it in deep sleep before yanking power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for ESP8266"));
        bool retVal = gsmModem.powerOff();
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
bool EspressifESP8266::extraModemSetup() {
    if (_modemSleepRqPin >= 0) { digitalWrite(_modemSleepRqPin, !_wakeLevel); }
    bool success = gsmModem.init();
    // Attempt to get the modem name even without a successful init
    // The full make and model won't be returned, but it will at least be
    // something that identifies the modem as an ESP8266, which is helpful for
    // debugging.
    _modemName = gsmModem.getModemName();
    return success;
}
