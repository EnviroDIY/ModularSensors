/*
 *DigiXBeeLTEBypass.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi Cellular XBee's BASED ON UBLOX CHIPS in bypass mode
*/

// Header Guards
#ifndef DigiXBeeLTEBypass_h
#define DigiXBeeLTEBypass_h

// Debugging Statement
// #define MS_DIGIXBEELTEBYPASS_DEBUG
// #define MS_DIGIXBEELTEBYPASS_DEBUG_DEEP

#ifdef MS_DIGIXBEELTEBYPASS_DEBUG
#define MS_DEBUGGING_STD "DigiXBeeLTEBypass"
#endif

#define TINY_GSM_MODEM_SARAR4
#define MS_MODEM_HAS_BATTERY_DATA
#define MS_MODEM_HAS_TEMPERATURE_DATA
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "DigiXBee.h"

#ifdef MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class DigiXBeeLTEBypass : public DigiXBee
{

public:
    // Constructor/Destructor
    DigiXBeeLTEBypass(Stream* modemStream,
                      int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                      int8_t modemResetPin, int8_t modemSleepRqPin,
                      const char* apn);
    ~DigiXBeeLTEBypass();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool getModemBatteryStats(uint8_t& chargeState, int8_t& percent, uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    bool isInternetAvailable(void) override;
    bool extraModemSetup(void) override;

private:
    const char* _apn;
};

#endif  // Header Guard
