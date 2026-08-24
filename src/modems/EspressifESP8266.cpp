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
                                   const char* pwd, int8_t modemSleepRqPin,
                                   int8_t espSleepRqPin)
    : Espressif<TinyGsmESP8266,                         // Modem Type
                TinyGsmESP8266::GsmClientESP8266,       // TCP Client Type
                TinyGsmESP8266::GsmClientSecureESP8266  // SSL Client
                                                        // Type
                >(modemStream, powerPin, modemResetPin, ssid, pwd,
                  modemSleepRqPin, espSleepRqPin) {}


bool EspressifESP8266::modemSleepFxn() {
    // Use this if you have an MCU pin connected to the ESP's reset pin to
    // wake from deep sleep.  We'll also put it in deep sleep before yanking
    // power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for ESP8266"));
        // On the ESP8266, we can request indefinite deep sleep by setting the
        // deep sleep timer time to 0.
        gsmModem.sendAT(GF("+GSLP=0"));
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
