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
                               const char* pwd, int8_t modemSleepRqPin,
                               int8_t espSleepRqPin)
    : Espressif<TinyGsmESP32,                       // Modem Type
                TinyGsmESP32::GsmClientESP32,       // TCP Client Type
                TinyGsmESP32::GsmClientSecureESP32  // SSL Client
                                                    // Type
                >(modemStream, powerPin, modemResetPin, ssid, pwd,
                  modemSleepRqPin, espSleepRqPin) {}


bool EspressifESP32::modemSleepFxn() {
    // Use this if you have an MCU pin connected to the ESP's reset pin to
    // wake from deep sleep.  We'll also put it in deep sleep before yanking
    // power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for ESP32"));
        // On the ESP32, we cannot request indefinite deep sleep.  We can only
        // request a deep sleep for a specific time, but that time can be as
        // long as about 28.8 days (2^31 - 1 milliseconds).  Theoretically, a
        // user could set their logging interval to be 32767 minutes (~22.7
        // days).  So here I'll tell the ESP32 to sleep for 1 second longer than
        // that knowing we can use the reset pin or power cycle to wake it up if
        // we're ready sooner.
        gsmModem.sendAT(GF("+GSLP=196021000"));
        bool success = gsmModem.waitResponse() == 1;
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        _modemStream->flush();
        return success;
    } else if (_modemSleepRqPin >= 0) {
        // Light sleep without the status pin
        MS_DBG(F("Configuring light sleep for the Espressif module"));
        // AT+SLEEPWKCFG=<wakeup source>,<param1>[,<param2>]
        // <wakeup source>:
        //     0: reserved, not supported now.
        //     1: reserved, not supported now.
        //     2: wakeup by GPIO.
        // <param1>:
        //     - If the wakeup source is a timer, it means the time before
        //     wakeup. Unit: millisecond.
        //     - If the wakeup source is GPIO, it means the GPIO number.
        // <param2>:
        //     If the wakeup source is GPIO, it means the wakeup level:
        //         0: low level.
        //         1: high level.
        gsmModem.sendAT(GF("+SLEEPWKCFG=2,"), String(_espSleepRqPin), ',',
                        ESPRESSIF_WAKE_LEVEL ? 1 : 0);
        bool success = gsmModem.waitResponse() == 1;
        // AT+SLEEP=<sleep mode>
        // <sleep mode>:
        //     0: Disable the sleep mode.
        //     1: Modem-sleep DTIM mode. RF will be periodically closed
        //     according to AP DTIM.
        //     2: Light-sleep mode. CPU will automatically sleep and RF will
        //     be periodically closed according to listen interval set by
        //     AT+CWJAP.
        //     3: Modem-sleep listen interval mode. RF will be periodically
        //     closed according to listen interval set by AT+CWJAP.
        gsmModem.sendAT(GF("+SLEEP=2"));
        success &= gsmModem.waitResponse() == 1;
        delay(5);
        MS_DBG(F("Setting pin"), _modemSleepRqPin,
               !_wakeLevel ? F("HIGH") : F("LOW"),
               F("to allow the Espressif module to enter light sleep"));
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        MS_DBG(F("Module MIGHT enter light sleep mode if it has been idle for "
                 "sufficient time."));
        return success;
    } else {  // DON'T go to sleep if we can't wake up!
        MS_DEEP_DBG(F("No pins for sleeping the Espressif module. "
                      "Hopefully it's in the "
                      "state you want."));
        _modemStream->flush();
        return true;
    }
}

// cSpell:ignore CWCOUNTRY
