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
#define MS_DEBUGGING_STD "DigiXBeeLTEBypassS"
#endif
#ifdef MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "DigiXBeeLTEBypassD"
#endif

#define TINY_GSM_MODEM_SARAR4

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

#include "DigiXBee.h"
#include "TinyGsmClient.h"

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
                           const char *apn,
                           uint8_t measurementsToAverage = 1);
    ~DigiXBeeLTEBypass();

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;

    // Get values by other names
    bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    float getModemTemperature(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

    //Az Extensions
    void setApn(const char *APN, bool copyId=false);
    String getApn(void);

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool verifyMeasurementComplete(bool debug=false) override;
    bool extraModemSetup(void) override;

private:
    const char *_apn;

    //Az extension
    char *_apn_buf = NULL; //Pointer to updated buffer
};

#endif  // Header Guard
