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
// #define MS_SIMCOMSIM800_DEBUG_DEEP

#ifdef MS_SIMCOMSIM800_DEBUG
#define MS_DEBUGGING_STD "SIMComSIM800"
#endif

#define TINY_GSM_MODEM_SIM800
#define MS_MODEM_HAS_BATTERY_DATA
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Status can be monitored on the STATUS pin which is active HIGH
// Time after end pulse until status pin becomes active
// SIM800 - >3sec from start of 1s pulse
// SIM900 - >2.2sec from end of pulse
#define SIM800_STATUS_LEVEL HIGH
#define SIM800_STATUS_TIME_MS 3000

// Reset with a >105ms low pulse on the RESET_N pin
#define SIM800_RESET_LEVEL LOW
#define SIM800_RESET_PULSE_MS 105

// Module is switched on by a > 1 second LOW pulse on the PWR_ON pin
// Module is switched on by a 1-3 second LOW pulse on the PWR_ON pin
// Please monitor the status so on and off are correct!
#define SIM800_WAKE_LEVEL LOW
#define SIM800_WAKE_PULSE_MS 1100
// Time after power on before "PWRKEY" can be used - >0.4sec
#define SIM800_WARM_UP_TIME_MS 450
// Time after end pulse until serial port becomes active (>3sec from start of 1s pulse)
#define SIM800_ATRESPONSE_TIME_MS 3000

// power down (gracefully) takes >3sec
// (Giving 15sec for shutdown in case it is not monitored.)
#define SIM800_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SIMCOMSIM800_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class SIMComSIM800 : public loggerModem
{

public:
    // Constructor/Destructor
    SIMComSIM800(Stream* modemStream,
                 int8_t powerPin, int8_t statusPin,
                 int8_t modemResetPin, int8_t modemSleepRqPin,
                 const char *apn);
    ~SIMComSIM800();

    bool modemWake(void) override;

    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    virtual void disconnectInternet(void) override;

    virtual uint32_t getNISTTime(void) override;

    virtual bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    virtual bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    virtual float getModemChipTemperature(void) override;

#ifdef MS_SIMCOMSIM800_DEBUG_DEEP
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
