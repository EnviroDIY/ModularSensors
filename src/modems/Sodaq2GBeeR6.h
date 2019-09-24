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
#define MS_DEBUGGING_STD "Sodaq2GBeeR6"
#endif

#define TINY_GSM_MODEM_SIM800
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Time after end pulse until status pin becomes active (>3sec from start of 1s pulse)
#define S2GBR6_STATUS_TIME_MS 3000
// power down (gracefully) takes >3sec
// (Giving 15sec for shutdown in case it is not monitored.)
#define S2GBR6_DISCONNECT_TIME_MS 15000L

// NOTE:  The GPRSBee R6+ has the PWR_KEY tied to the input voltage, so there is
// no warm-up time needed
#define S2GBR6_WARM_UP_TIME_MS 0
// Time after end pulse until serial port becomes active (>3sec from start of 1s pulse)
#define S2GBR6_ATRESPONSE_TIME_MS 3000

// How long we're willing to wait to get signal quality
#define S2GBR6_SIGNALQUALITY_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class Sodaq2GBeeR6 : public loggerModem
{

public:
    // Constructor/Destructor
    // NOTE:  The Sodaq GPRSBee doesn't expose the SIM800's reset pin
    // NOTE:  The power pin of the SIM800 is wired to the XBee's DTR
    // pin, the actualy PWR_KEY is not exposed, there is no way to
    // request sleep.  The normal Vin pin of the Bee socket (pin 1)
    // is used for voltage reference only.
    // The order of the pins in the constructor is different from
    // other modems for backwards compatibility and because the
    // Sodaq documentation is confusing
    Sodaq2GBeeR6(Stream *modemStream,
                 int8_t powerPin, int8_t statusPin,
                 const char *apn,
                 uint8_t measurementsToAverage = 1);
    ~Sodaq2GBeeR6();

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    // Get values by other names
    bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    float getModemTemperature(void) override;

    uint32_t getNISTTime(void) override;

    void setVRefPin(int8_t vRefPin);

    #ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool verifyMeasurementComplete(bool debug=false) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void)override;

private:
    const char *_apn;
    int8_t _vRefPin;

};

#endif  // Header Guard
