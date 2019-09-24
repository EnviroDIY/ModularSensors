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
// #define MS_SODAQUBEEU201_DEBUG
// #define MS_SODAQUBEEU201_DEBUG_DEEP

#ifdef MS_SODAQUBEEU201_DEBUG
#define MS_DEBUGGING_STD "SodaqUBeeU201"
#endif

#define TINY_GSM_MODEM_UBLOX
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Time after end pulse until V_INT becomes active
// Unspecified in documentation! Taking value from Lisa U2
#define U201_STATUS_TIME_MS 100
// Power down time "can largely vary depending
// on the application / network settings and the concurrent module
// activities."  Vint/status pin should be monitored and power not withdrawn
// until that pin reads low.  Giving 15sec here in case it is not monitored.
#define U201_DISCONNECT_TIME_MS 15000L

// Module turns on when power is applied - level of PWR_ON then irrelevant
#define U201_WARM_UP_TIME_MS 0
// Time until system and digital pins are operational
// (6 sec typical for SARA U201)
#define U201_ATRESPONSE_TIME_MS 6000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_SODAQUBEEU201_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class SodaqUBeeU201 : public loggerModem
{

public:
    // Constructor/Destructor
    SodaqUBeeU201(Stream* modemStream,
                  int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char *apn);
    ~SodaqUBeeU201();

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    // Get values by other names
    bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    float getModemTemperature(void) override;

    uint32_t getNISTTime(void) override;

    bool modemHardReset(void) override;

    #ifdef MS_SODAQUBEEU201_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void)override;

private:
    const char *_apn;

};

#endif  // Header Guard
