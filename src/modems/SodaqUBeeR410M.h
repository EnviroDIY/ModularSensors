/*
 *SodaqUBeeR410M.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA R410M LTE-M Cellular Module
*/

// Header Guards
#ifndef SodaqUBeeR410M_h
#define SodaqUBeeR410M_h

// Debugging Statement
// #define MS_SodaqUBeeR410M_DEBUG

#ifdef MS_SODAQUBEER410M_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_UBLOX
// V_INT becomes active mid-way through on-pulse
#define R410M_STATUS_TIME_MS 0
// Power down time "can largely vary depending
// on the application / network settings and the concurrent module
// activities."  Vint/status pin should be monitored and power not withdrawn
// until that pin reads low.  Giving 15sec here in case it is not monitored.
#define R410M_DISCONNECT_TIME_MS 15000L

// Time after power on before PWR_ON can be used ??? Unclear in documentation!
#define R410M_WARM_UP_TIME_MS 250
// Time until system and digital pins are operational (~4.5s)
#define R410M_ATRESPONSE_TIME_MS 4500L

// How long we're willing to wait to get signal quality
#define R410M_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"


class SodaqUBeeR410M : public loggerModem
{

public:
    // Constructors
    #if F_CPU == 8000000L
    // At this slow baud rate, we need to begin and end serial communication,
    // so we need a Serial instance rather than a stream
    SodaqUBeeR410M(HardwareSerial* modemStream,
                   int8_t powerPin, int8_t statusPin, int8_t modemSleepRqPin,
                   const char *apn,
                   uint8_t measurementsToAverage = 1);
    #else
    SodaqUBeeR410M(Stream* modemStream,
                   int8_t powerPin, int8_t statusPin, int8_t modemSleepRqPin,
                   const char *apn,
                   uint8_t measurementsToAverage = 1);
    #endif


    // The a measurement is "complete" when the modem is registered on the network.
    // For a cellular modem, this actually sets the GPRS bearer/APN!!
    bool startSingleMeasurement(void) override;
    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    TinyGsm _tinyModem;
    #if F_CPU == 8000000L
    HardwareSerial *_modemStream;
    #else
    Stream *_modemStream;
    #endif

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
