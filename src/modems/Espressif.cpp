/**
 * @file Espressif.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the Espressif class.
 */

// Included Dependencies
#include "Espressif.h"

// Constructors
Espressif::Espressif(Stream* modemStream, int8_t powerPin, int8_t modemResetPin,
                     const char* ssid, const char* pwd)
    : loggerModem(powerPin, -1, ESPRESSIF_STATUS_LEVEL, modemResetPin,
                  ESPRESSIF_RESET_LEVEL, ESPRESSIF_RESET_PULSE_MS, -1,
                  ESPRESSIF_WAKE_LEVEL, ESPRESSIF_WAKE_PULSE_MS,
                  ESPRESSIF_STATUS_TIME_MS, ESPRESSIF_DISCONNECT_TIME_MS,
                  ESPRESSIF_WAKE_DELAY_MS, ESPRESSIF_ATRESPONSE_TIME_MS),
      _modemStream(modemStream),
      _ssid(ssid),
      _pwd(pwd) {}

// Destructor
Espressif::~Espressif() {}

// A helper function to wait for the esp to boot and immediately change some
// settings We'll use this in the wake function
bool Espressif::ESPwaitForBoot(void) {
    // Wait for boot - finished when characters start coming
    // NOTE: After every "hard" reset (either power off or via RST-B), the ESP
    // sends out a boot log from the ROM on UART1 at 74880 baud.  We're not
    // going to worry about the odd baud rate since we're simply throwing the
    // characters away.
    MS_DBG(F("Waiting for boot-up message from Espressif module"));
    delay(200);  // It will take at least this long
    uint32_t start   = millis();
    bool     success = false;
    while (!_modemStream->available() && millis() - start < 1000) {
        // wait
    }
    if (_modemStream->available()) {
        success = true;
        // Read the boot log to empty it from the serial buffer
        while (_modemStream->available()) {
            _modemStream->read();
            delay(2);
        }
    }
    return success;
}

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool Espressif::modemWakeFxn(void) {
    bool success = true;
    if (_powerPin >= 0) {  // Turns on when power is applied
        MS_DEEP_DBG(
            F("Power pin"), _powerPin,
            F("takes priority over reset pin,  modem wakes on power on"));
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        success &= ESPwaitForBoot();
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, _wakeLevel);
        }
        return success;
    } else if (_modemResetPin >= 0) {
        MS_DBG(F("Sending a reset pulse to pin"), _modemResetPin,
               F("to wake Espressif module from deep sleep"));
        digitalWrite(_modemResetPin, LOW);
        delay(_resetPulse_ms);
        digitalWrite(_modemResetPin, HIGH);
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        success &= ESPwaitForBoot();
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, _wakeLevel);
        }
        return success;
    } else if (_modemSleepRqPin >= 0) {
        MS_DBG(F("Setting pin"), _modemSleepRqPin,
               _wakeLevel ? F("HIGH") : F("LOW"),
               F("to wake Espressif module from light sleep"));
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        return success;
    } else {
        MS_DEEP_DBG(
            F("No pins for waking the Espressif module. Hopefully it's in the "
              "state you want."));
        return true;
    }
}

bool Espressif::modemSleepFxn(void) {
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake
    // from deep sleep.  We'll also put it in deep sleep before yanking power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for Espressif module"));
        bool retVal = gsmModem.poweroff();
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        return retVal;
    } else {  // DON'T go to sleep if we can't wake up!
        MS_DEEP_DBG(F(
            "No pins for sleeping the Espressif module. Hopefully it's in the "
            "state you want."));
        return true;
    }
}

// Set up the light-sleep status pin, if applicable
bool Espressif::extraModemSetup(void) {
    if (_modemSleepRqPin >= 0) { digitalWrite(_modemSleepRqPin, !_wakeLevel); }
    gsmModem.init();
    _modemName = gsmModem.getModemName();
    return true;
}
