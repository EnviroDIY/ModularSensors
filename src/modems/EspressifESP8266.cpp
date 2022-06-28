/**
 * @file EspressifESP8266.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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
    : loggerModem(powerPin, -1, ESP8266_STATUS_LEVEL, modemResetPin,
                  ESP8266_RESET_LEVEL, ESP8266_RESET_PULSE_MS, -1,
                  ESP8266_WAKE_LEVEL, ESP8266_WAKE_PULSE_MS,
                  ESP8266_STATUS_TIME_MS, ESP8266_DISCONNECT_TIME_MS,
                  ESP8266_WAKE_DELAY_MS, ESP8266_ATRESPONSE_TIME_MS),
#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger),
#else
      gsmModem(*modemStream),
#endif
      gsmClient(gsmModem),
      _modemStream(modemStream),
      _ssid(ssid),
      _pwd(pwd) {
}

// Destructor
EspressifESP8266::~EspressifESP8266() {}

MS_IS_MODEM_AWAKE(EspressifESP8266);
MS_MODEM_WAKE(EspressifESP8266);

MS_MODEM_CONNECT_INTERNET(EspressifESP8266);
MS_MODEM_DISCONNECT_INTERNET(EspressifESP8266);
MS_MODEM_IS_INTERNET_AVAILABLE(EspressifESP8266);

MS_MODEM_GET_NIST_TIME(EspressifESP8266);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(EspressifESP8266);
MS_MODEM_GET_MODEM_BATTERY_DATA(EspressifESP8266);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(EspressifESP8266);

// A helper function to wait for the esp to boot and immediately change some
// settings We'll use this in the wake function
bool EspressifESP8266::ESPwaitForBoot(void) {
    // Wait for boot - finished when characters start coming
    // NOTE: After every "hard" reset (either power off or via RST-B), the ESP
    // sends out a boot log from the ROM on UART1 at 74880 baud.  We're not
    // going to worry about the odd baud rate since we're simply throwing the
    // characters away.
    MS_DBG(F("Waiting for boot-up message from ESP8266"));
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
bool EspressifESP8266::modemWakeFxn(void) {
    bool success = true;
    if (_powerPin >= 0) {  // Turns on when power is applied
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        success &= ESPwaitForBoot();
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, _wakeLevel);
        }
        return success;
    } else if (_modemResetPin >= 0) {
        MS_DBG(F("Sending a reset pulse to pin"), _modemResetPin,
               F("to wake ESP8266 from deep sleep"));
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
               F("to wake ESP8266 from light sleep"));
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        return success;
    } else {
        return true;
    }
}

bool EspressifESP8266::modemSleepFxn(void) {
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake
    // from deep sleep.  We'll also put it in deep sleep before yanking power.
    if (_modemResetPin >= 0 || _powerPin >= 0) {
        MS_DBG(F("Requesting deep sleep for ESP8266"));
        bool retVal = gsmModem.poweroff();
        if (_modemSleepRqPin >= 0) {
            digitalWrite(_modemSleepRqPin, !_wakeLevel);
        }
        return retVal;
    } else {  // DON'T go to sleep if we can't wake up!
        return true;
    }
}

// Set up the light-sleep status pin, if applicable
bool EspressifESP8266::extraModemSetup(void) {
    if (_modemSleepRqPin >= 0) { digitalWrite(_modemSleepRqPin, !_wakeLevel); }
    gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    return true;
}
