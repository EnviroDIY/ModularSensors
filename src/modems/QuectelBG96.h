/*
 *QuectelBG96.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Dragino BG96, Nimbelink Skywire 4G LTE-M Global, and
 *other modules based on the Quectel BG96.
*/

// Header Guards
#ifndef QuectelBG96_h
#define QuectelBG96_h

// Debugging Statement
// #define MS_QUECTELBG96_DEBUG
// #define MS_QUECTELBG96_DEBUG_DEEP

#ifdef MS_QUECTELBG96_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_BG96

// Time after end pulse until status pin becomes active
#define BG96_STATUS_TIME_MS 4800L
// > 2 sec
#define BG96_DISCONNECT_TIME_MS 5000L

// Time after VBAT is stable before PWRKEY can be used
#define BG96_WARM_UP_TIME_MS 30
// USB active at >4.2 sec, status at >4.8 sec, URAT at >4.9
#define BG96_ATRESPONSE_TIME_MS 4200L

// How long we're willing to wait to get signal quality
#define BG96_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_QUECTELBG96_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class QuectelBG96 : public loggerModem
{

public:
    // Constructors
    QuectelBG96(Stream* modemStream,
                int8_t powerPin, int8_t statusPin,
                int8_t modemResetPin, int8_t modemSleepRqPin,
                const char *apn,
                uint8_t measurementsToAverage = 1);

    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_QUECTELBG96_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm _tinyModem;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void)override;

private:
    const char *_apn;

};

#endif
