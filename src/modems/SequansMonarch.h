/*
 *SequansMonarch.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Dragino BG96, Nimbelink Skywire 4G LTE-M Global, and
 *other modules based on the Quectel BG96.
*/

// Header Guards
#ifndef SequansMonarch_h
#define SequansMonarch_h

// Debugging Statement
// #define MS_SEQUANSMONARCH_DEBUG
// #define MS_SEQUANSMONARCH_DEBUG_DEEP

#ifdef MS_SEQUANSMONARCH_DEBUG
#define MS_DEBUGGING_STD "SequansMonarch"
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_SEQUANS_MONARCH

// ?? Undocumented
#define MONARCH_STATUS_TIME_MS 50
// ?? Undocumented (Giving 15sec here in case it is not monitored.)
#define MONARCH_DISCONNECT_TIME_MS 15000L

// Module automatically boots when power is applied
#define MONARCH_WARM_UP_TIME_MS 0
// ?? Time to UART availability not documented
#define MONARCH_ATRESPONSE_TIME_MS 5000L

// How long we're willing to wait to get signal quality
#define MONARCH_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class SequansMonarch : public loggerModem
{

public:
    // Constructors
    SequansMonarch(Stream* modemStream,
                   int8_t powerPin, int8_t statusPin,
                   int8_t modemResetPin, int8_t modemSleepRqPin,
                   const char *apn,
                   uint8_t measurementsToAverage = 1);
    virtual ~SequansMonarch();              

    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void)override;

private:
    const char *_apn;

};

#endif  // Header Guard
