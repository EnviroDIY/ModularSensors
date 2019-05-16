/*
 *Sodaq2GBeeR6.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq 2GBee revisions 6 and higher - these are based on
 *the SIMCOM SIM800h.
*/

// Header Guards
#ifndef Sodaq2GBeeR6_h
#define Sodaq2GBeeR6_h

// Debugging Statement
// #define MS_SODAQ2GBEER6_DEBUG
// #define MS_SODAQ2GBEER6_DEBUG_DEEP

#ifdef MS_SODAQ2GBEER6_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_SIM800

// Time after end pulse until status pin becomes active (>3sec from start of 1s pulse)
#define SIM800_STATUS_TIME_MS 2000
// power down (gracefully) takes >3sec
// (Giving 15sec for shutdown in case it is not monitored.)
#define SIM800_DISCONNECT_TIME_MS 15000L

// Time after power on before "PWRKEY" can be used - >0.4sec
#define SIM800_WARM_UP_TIME_MS 450
// Time after end pulse until serial port becomes active (>3sec from start of 1s pulse)
#define SIM800_ATRESPONSE_TIME_MS 2000

// How long we're willing to wait to get signal quality
#define SIM800_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class Sodaq2GBeeR6 : public loggerModem
{

public:
    // Constructors
    // NOTE:  The Sodaq GPRSBee doesn't expose the SIM800's reset pin
    Sodaq2GBeeR6(Stream* modemStream,
                 int8_t powerPin, int8_t statusPin,
                 int8_t modemSleepRqPin,
                 const char *apn,
                 uint8_t measurementsToAverage = 1);

    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
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

#endif
