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
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Status can be monitored on the STATUS(D0) pin
// Time after end pulse until status pin becomes active - 4.8s
#define BG96_STATUS_LEVEL HIGH
#define BG96_STATUS_TIME_MS 5000L

// Reset with a 150-460ms low pulse on the RESET_N pin
#define BG96_RESET_LEVEL LOW
#define BG96_RESET_PULSE_MS 300

// Module is switched on by a >100 millisecond LOW pulse on the PWRKEY pin
// Module is switched on by a >650 millisecond LOW pulse on the PWRKEY pin
// Using something between those times for wake and using AT commands for sleep,
// we should keep in the proper state.
#define BG96_WAKE_LEVEL LOW
#define BG96_WAKE_PULSE_MS 110
// Time after VBAT is stable before PWRKEY can be used is >30ms
#define BG96_WARM_UP_TIME_MS 100
// USB active at >4.2 sec, status at >4.8 sec, URAT at >4.9
#define BG96_ATRESPONSE_TIME_MS 10000L

// > 2 sec
#define BG96_DISCONNECT_TIME_MS 5000L

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
    QuectelBG96(Stream* modemStream,
                int8_t powerPin, int8_t statusPin,
                int8_t modemResetPin, int8_t modemSleepRqPin,
                const char *apn);
    ~QuectelBG96();

    bool modemWake(void) override;

    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    virtual void disconnectInternet(void) override;

    virtual uint32_t getNISTTime(void) override;

    virtual bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    virtual bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    virtual float getModemChipTemperature(void) override;

    bool modemHardReset(void) override;

#ifdef MS_QUECTELBG96_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    virtual bool isInternetAvailable(void) override;
    virtual bool modemSleepFxn(void) override;
    virtual bool modemWakeFxn(void) override;
    virtual bool extraModemSetup(void) override;

private:
    const char *_apn;

};

#endif  // Header Guard
