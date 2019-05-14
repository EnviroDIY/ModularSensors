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

#ifdef MS_ESPRESSIFESP8266_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_ESP8266

#define ESP8266_STATUS_TIME_MS 350  // N/A? - No status pin - use boot time if using a GPIO pin
#define ESP8266_DISCONNECT_TIME_MS 500 // power down ???

#define ESP8266_WARM_UP_TIME_MS 0  // Module turns on when power is applied
#define ESP8266_ATRESPONSE_TIME_MS 5000L

#define ESP8266_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"


class EspressifESP8266 : public loggerModem
{

public:
    // Constructors
    EspressifESP8266(Stream* modemStream,
                     int8_t powerPin, int8_t statusPin,
                     int8_t modemResetPin, int8_t modemSleepRqPin,
                     const char *ssid, const char *pwd,
                     uint8_t measurementsToAverage = 1);


    // The a measurement is "complete" when the modem is registered on the network.
    // For a cellular modem, this actually sets the GPRS bearer/APN!!
    bool startSingleMeasurement(void) override;
    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    TinyGsm _tinyModem;
    Stream *_modemStream;

protected:
    virtual bool didATRespond(void) override;
    virtual bool isInternetAvailable(void) override;
    virtual bool modemSleepFxn(void) override;
    virtual bool modemWakeFxn(void) override;
    virtual bool extraModemSetup(void)override;

private:
    bool ESPwaitForBoot(void);
    const char *_ssid;
    const char *_pwd;

    int8_t espSleepRqPin;
    int8_t espStatusPin;
};

#endif
