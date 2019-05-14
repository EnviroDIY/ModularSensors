/*
 *SodaqUBeeU201.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA U201 3G Cellular Module
*/

// Header Guards
#ifndef SodaqUBeeU201_h
#define SodaqUBeeU201_h

// Debugging Statement
// #define MS_SodaqUBeeU201_DEBUG

#ifdef MS_SODAQUBEEU201_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_UBLOX
// Time after end pulse until V_INT becomes active
// Unspecified in documentation! Taking value from Lisa U2
#define U201_STATUS_TIME_MS 35
// Power down time "can largely vary depending
// on the application / network settings and the concurrent module
// activities."  Vint/status pin should be monitored and power not withdrawn
// until that pin reads low.  Giving 15sec here in case it is not monitored.
#define U201_DISCONNECT_TIME_MS 15000L

// Module turns on when power is applied - level of PWR_ON then irrelevant
#define U201_WARM_UP_TIME_MS 0
// Time until system and digital pins are operational
// (6 sec typical for SARA U201)
// Time for an AT response may be even longer when using a 3G XBee in bypass mode!
#define U201_ATRESPONSE_TIME_MS 6000L

// How long we're willing to wait to get signal quality
#define U201_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"


class SodaqUBeeU201 : public loggerModem
{

public:
    // Constructors
    SodaqUBeeU201(Stream* modemStream,
                  int8_t powerPin, int8_t statusPin, int8_t modemSleepRqPin,
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
