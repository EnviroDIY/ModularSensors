/*
 *SIMComSIM7000.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Botletics and other modules based on the SIMCOM SIM7000.
*/

// Header Guards
#ifndef SIMComSIM7000_h
#define SIMComSIM7000_h

// Debugging Statement
// #define MS_SIMCOMSIM7000_DEBUG
// #define MS_SIMCOMSIM7000_DEBUG_DEEP

#ifdef MS_SIMCOMSIM7000_DEBUG
#define MS_DEBUGGING_STD "SIMComSIM7000"
#endif

#define TINY_GSM_MODEM_SIM7000
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Time after end pulse until status pin becomes active (>4.5sec)
#define SIM7000_STATUS_TIME_MS 5000L
// power down (gracefully) takes 1.8-6.9 sec
#define SIM7000_DISCONNECT_TIME_MS 7000L

// Time after power on before "PWRKEY" can be used (guess - diagram isn't clear)
#define SIM7000_WARM_UP_TIME_MS 1000L
// Time after end pulse until serial port becomes active (>4.5sec)
#define SIM7000_ATRESPONSE_TIME_MS 4500

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class SIMComSIM7000 : public loggerModem
{

public:
    // Constructor/Destructor
    SIMComSIM7000(Stream* modemStream,
                  int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char *apn);
    ~SIMComSIM7000();

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;

    // Get values by other names
    bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    float getModemChipTemperature(void) override;

    bool modemHardReset(void) override;

    void modemPowerUp(void) override;

    #ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
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
