/*
 *EspressifESP8266.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file SHOULD work for essentially any breakout of the Espressif ESP8266
 *wifi chip as long as the chip has been flashed with Espressif's AT command
 *firmware.
*/

// Header Guards
#ifndef EspressifESP8266_h
#define EspressifESP8266_h

// Debugging Statement
// #define MS_ESPRESSIFESP8266_DEBUG
// #define MS_ESPRESSIFESP8266_DEBUG_DEEP

#ifdef MS_ESPRESSIFESP8266_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_ESP8266

// No possible status pin on the ESP8266 in deep sleep mode
// For cases where a pin is defined for light sleep mode, we'll call this
// the boot time because I have no idea what it would be
#define ESP8266_STATUS_TIME_MS 350
// power down ???
#define ESP8266_DISCONNECT_TIME_MS 500

// Module turns on when power is applied regardless of pin states
#define ESP8266_WARM_UP_TIME_MS 0
// Time until system and digital pins are operational
#define ESP8266_ATRESPONSE_TIME_MS 350

// How long we're willing to wait to get signal quality
#define ESP8266_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class EspressifESP8266 : public loggerModem
{

public:
    // Constructors
    EspressifESP8266(Stream* modemStream,
                     int8_t powerPin, int8_t statusPin,
                     int8_t modemResetPin, int8_t modemSleepRqPin,
                     const char *ssid, const char *pwd,
                     uint8_t measurementsToAverage = 1,
                     int8_t espSleepRqPin = -1, int8_t espStatusPin = -1);

    bool startSingleMeasurement(void) override;
    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

    // Need the stream for tossing junk on boot
    Stream *_modemStream;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void)override;

private:
    bool ESPwaitForBoot(void);
    const char *_ssid;
    const char *_pwd;

    int8_t _espSleepRqPin;
    int8_t _espStatusPin;

};

#endif
