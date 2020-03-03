/*
 *EspressifESP8266.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *Copyright 2020 Stroud Water Research Center
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file SHOULD work for essentially any breakout of the Espressif ESP8266
 *wifi chip as long as the chip has been flashed with Espressif's AT command
 *firmware.
 */

// Header Guards
#ifndef SRC_MODEMS_ESPRESSIFESP8266_H_
#define SRC_MODEMS_ESPRESSIFESP8266_H_

// Debugging Statement
// #define MS_ESPRESSIFESP8266_DEBUG
// #define MS_ESPRESSIFESP8266_DEBUG_DEEP

#ifdef MS_ESPRESSIFESP8266_DEBUG
#define MS_DEBUGGING_STD "EspressifESP8266"
#endif

#define TINY_GSM_MODEM_ESP8266
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// It is not possible to get status from the ESP8266 in deep sleep mode.
// During deep sleep the pin state is undefined
// For cases where a pin is defined for light sleep mode, the Espressif
// documentation states:  since the system needs some time to wake up from
// light sleep, it is suggested that wait at least 5ms before sending next AT
// command.  They don't say anything about the pin state though.
// The status level during light sleep is user selectable, but we set it low
// for wake and high for sleep.  Of course, despite being able to configure
// light sleep mode for the module, it's not actually possible to purposefully
// enter light sleep via AT commands, so we are dependent on the module deciding
// it's been idle long enough and entering sleep on its own.  That is a terrible
// system.  Use a deep-sleep with reset if possible.
#define ESP8266_STATUS_LEVEL HIGH
#define ESP8266_STATUS_TIME_MS 350

// Reset is very fast
#define ESP8266_RESET_LEVEL LOW
#define ESP8266_RESET_PULSE_MS 1

// See notes above.. this is user configurable, but useless
#define ESP8266_WAKE_LEVEL LOW
#define ESP8266_WAKE_PULSE_MS 0
// Module turns on when power is applied regardless of pin states
#define ESP8266_WARM_UP_TIME_MS 0
// Time until system and digital pins are operational
#define ESP8266_ATRESPONSE_TIME_MS 350

// power down ???
#define ESP8266_DISCONNECT_TIME_MS 500

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class EspressifESP8266 : public loggerModem {
 public:
    // Constructor/Destructor
    EspressifESP8266(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                     int8_t modemResetPin, int8_t modemSleepRqPin,
                     const char* ssid, const char* pwd,
                     int8_t espSleepRqPin = -1, int8_t espStatusPin = -1);
    ~EspressifESP8266();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm       gsmModem;
    TinyGsmClient gsmClient;

    // Need the stream for tossing junk on boot
    Stream* _modemStream;

 protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;

 private:
    bool        ESPwaitForBoot(void);
    const char* _ssid;
    const char* _pwd;

    int8_t _espSleepRqPin;
    int8_t _espStatusPin;
};

#endif  // SRC_MODEMS_ESPRESSIFESP8266_H_
