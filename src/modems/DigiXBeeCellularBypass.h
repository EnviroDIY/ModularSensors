/*
 *DigiXBeeCellularBypass.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi Cellular XBee's BASED ON UBLOX CHIPS in bypass mode
*/

// Header Guards
#ifndef DigiXBeeCellularBypass_h
#define DigiXBeeCellularBypass_h

// Debugging Statement
// #define MS_DIGIXBEECELLULARBYPASS_DEBUG
// #define MS_DIGIXBEECELLULARBYPASS_DEBUG_DEEP

#ifdef MS_DIGIXBEECELLULARBYPASS_DEBUG
#define MS_DEBUGGING_STD "DigiXBeeCellularBypass"
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_MODEM_UBLOX

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "DigiXBee.h"
#include "TinyGsmClient.h"

#ifdef MS_DIGIXBEECELLULARBYPASS_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class DigiXBeeCellularBypass : public DigiXBee
{

public:
    // Constructors
    DigiXBeeCellularBypass(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *apn,
                           uint8_t measurementsToAverage = 1);
    virtual ~DigiXBeeCellularBypass();

    bool isMeasurementComplete(bool debug=false) override;
    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    #ifdef MS_DIGIXBEECELLULARBYPASS_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
    #endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool extraModemSetup(void) override;

private:
    const char *_apn;

};

#endif  // Header Guard
