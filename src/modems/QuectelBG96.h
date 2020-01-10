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
    #define MS_DEBUGGING_STD "QuectelBG96"
#endif

#define TINY_GSM_MODEM_BG96
#define MS_MODEM_HAS_BATTERY_DATA
#ifndef TINY_GSM_RX_BUFFER
    #define TINY_GSM_RX_BUFFER 64
#endif

// Time after end pulse until status pin becomes active - 4.8s
#define BG96_STATUS_TIME_MS 5000L
// > 2 sec
#define BG96_DISCONNECT_TIME_MS 5000L

// Time after VBAT is stable before PWRKEY can be used
#define BG96_WARM_UP_TIME_MS 30
// USB active at >4.2 sec, status at >4.8 sec, URAT at >4.9
#define BG96_ATRESPONSE_TIME_MS 4200L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_QUECTELBG96_DEBUG_DEEP
    #include <StreamDebugger.h>
#endif


class QuectelBG96 : public loggerModem
{

public:
    // Constructor/Destructor
    QuectelBG96(Stream *modemStream,
                int8_t powerPin, int8_t statusPin,
                int8_t modemResetPin, int8_t modemSleepRqPin,
                const char *apn);
    ~QuectelBG96();

     bool modemSetup(void) override;
     bool modemWake(void) override;

     bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
     void disconnectInternet(void) override;

     uint32_t getNISTTime(void) override;

     bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
     bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
     float getModemChipTemperature(void) override;

#ifdef MS_QUECTELBG96_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
     bool isInternetAvailable(void) override;
     bool modemSleepFxn(void) override;
     bool modemWakeFxn(void) override;
     bool extraModemSetup(void) override;

private:
    const char *_apn;
};

#endif  // Header Guard
