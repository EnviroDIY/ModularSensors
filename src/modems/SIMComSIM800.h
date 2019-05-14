/*
 *SIMComSIM800.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Adafruit Fona 2G, the Sodaq GPRSBee R4 and almost any
 * other module based on the SIMCOM SIM800 or SIM900
 *the SIMCOM SIM800h.
*/

// Header Guards
#ifndef SIMComSIM800_h
#define SIMComSIM800_h

// Debugging Statement
// #define MS_SIMCOMSIM800_DEBUG

#ifdef MS_SIMCOMSIM800_DEBUG
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


class SIMComSIM800 : public loggerModem
{

public:
    // Constructors
    SIMComSIM800(Stream* modemStream,
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
