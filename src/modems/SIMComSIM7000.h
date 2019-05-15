/*
 *SIMComSIM7000.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Botletics and other modules based on the SIMCOM SIM7000.
*/

// Header Guards
#ifndef SIMComSIM7000_h
#define SIMComSIM7000_h

// Debugging Statement
// #define MS_SIMCOMSIM7000_DEBUG
// #define MS_SIMCOMSIM7000_DEBUG_DEEP

#ifdef MS_SIMCOMSIM7000_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_SIM7000

// Time after end pulse until status pin becomes active (>4.5sec)
#define SIM7000_STATUS_TIME_MS 4500L
// power down (gracefully) takes 1.8-6.9 sec
#define SIM7000_DISCONNECT_TIME_MS 7000L

// Time after power on before "PWRKEY" can be used (guess - diagram isn't clear)
#define SIM7000_WARM_UP_TIME_MS 1000L
// Time after end pulse until serial port becomes active (>4.5sec)
#define SIM7000_ATRESPONSE_TIME_MS 4500

// How long we're willing to wait to get signal quality
#define SIM7000_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class SIMComSIM7000 : public loggerModem
{

public:
    // Constructors
    SIMComSIM7000(Stream* modemStream,
                  int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char *apn,
                  uint8_t measurementsToAverage = 1);

    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm _tinyModem;
    Stream *_modemStream;

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
