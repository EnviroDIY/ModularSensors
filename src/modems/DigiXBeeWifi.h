/*
 *DigiXBeeWifi.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi S6B Wifi XBee's
*/

// Header Guards
#ifndef DigiXBeeWifi_h
#define DigiXBeeWifi_h

// Debugging Statement
// #define MS_DIGIXBEEWIFI_DEBUG

#ifdef MS_DIGIXBEEWIFI_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// Included Dependencies
#include "DigiXBee.h"
#include "TinyGsmClient.h"


class DigiXBeeWifi : public DigiXBee
{

public:
    // Constructors
    DigiXBeeWifi(Stream* modemStream,
                 int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                 int8_t modemResetPin, int8_t modemSleepRqPin,
                 const char *ssid, const char *pwd,
                 uint8_t measurementsToAverage = 1);


    // The a measurement is "complete" when the modem is registered on the network.
    // For a cellular modem, this actually sets the GPRS bearer/APN!!
    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    TinyGsm _tinyModem;
    Stream *_modemStream;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool extraModemSetup(void) override;

 private:
    const char *_ssid;
    const char *_pwd;

};

#endif
