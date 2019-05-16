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
// #define MS_DIGIXBEEWIFI_DEBUG_DEEP

#ifdef MS_DIGIXBEEWIFI_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// Included Dependencies
#include "ModSensorDebugger.h"
#include "DigiXBee.h"
#include "TinyGsmClient.h"

#ifdef defineMS_DIGIXBEEWIFI_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class DigiXBeeWifi : public DigiXBee
{

public:
    // Constructors
    DigiXBeeWifi(Stream* modemStream,
                 int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                 int8_t modemResetPin, int8_t modemSleepRqPin,
                 const char *ssid, const char *pwd,
                 uint8_t measurementsToAverage = 1);

    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm _tinyModem;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool extraModemSetup(void) override;

private:
    const char *_ssid;
    const char *_pwd;

};

#endif
