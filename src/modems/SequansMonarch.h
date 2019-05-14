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

#ifdef MS_SEQUANSMONARCH_DEBUG
#define MS_DEBUGGING_STD
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
#include "LoggerModem.h"
#include "TinyGsmClient.h"


class SequansMonarch : public loggerModem
{

public:
    // Constructors
    SequansMonarch(Stream* modemStream,
                   int8_t powerPin, int8_t statusPin,
                   int8_t modemResetPin, int8_t modemSleepRqPin,
                   const char *apn,
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
    const char *_apn;
};

#endif
