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
    #define MS_DEBUGGING_STD "EspressifESP8266"
#endif

#define TINY_GSM_MODEM_ESP8266
#ifndef TINY_GSM_RX_BUFFER
    #define TINY_GSM_RX_BUFFER 64
#endif

// It is not possible to get status from the ESP8266 in deep sleep mode.
// During deep sleep the pin state is undefined
// For cases where a pin is defined for light sleep mode, the Espressif
// documentation states:  ince the system needs some time to wake up from
// light sleep, it is suggested that wait at least 5ms before sending next AT
// command.  They don't say anything about the pin state though.
#define ESP8266_STATUS_TIME_MS 350
// power down ???
#define ESP8266_DISCONNECT_TIME_MS 500

// Module turns on when power is applied regardless of pin states
#define ESP8266_WARM_UP_TIME_MS 350
// Time until system and digital pins are operational
#define ESP8266_ATRESPONSE_TIME_MS 350

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
    #include <StreamDebugger.h>
#endif


class EspressifESP8266 : public loggerModem
{

public:
    // Constructor/Destructor
    EspressifESP8266(Stream *modemStream,
                     int8_t powerPin, int8_t statusPin,
                     int8_t modemResetPin, int8_t modemSleepRqPin,
                     const char *ssid, const char *pwd,
                     int8_t espSleepRqPin = -1, int8_t espStatusPin = -1);
    ~EspressifESP8266();

    virtual bool modemSetup(void) override;
    virtual bool modemWake(void) override;

    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    virtual void disconnectInternet(void) override;

    virtual uint32_t getNISTTime(void) override;

    virtual bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    virtual bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    virtual float getModemChipTemperature(void) override;

#ifdef MS_ESPRESSIFESP8266_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

    // Need the stream for tossing junk on boot
    Stream *_modemStream;

protected:
    virtual bool isInternetAvailable(void) override;
    virtual bool modemSleepFxn(void) override;
    virtual bool modemWakeFxn(void) override;
    virtual bool extraModemSetup(void) override;

private:
    bool ESPwaitForBoot(void);
    const char *_ssid;
    const char *_pwd;

    int8_t _espSleepRqPin;
    int8_t _espStatusPin;
};

#endif  // Header Guard
