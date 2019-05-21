/*
 *DigiXBee.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Digi XBee's
*/

// Header Guards
#ifndef DigiXBee_h
#define DigiXBee_h

// Debugging Statement
// #define MS_DIGIXBEE_DEBUG

#ifdef MS_DIGIXBEE_DEBUG
#define MS_DEBUGGING_STD "DigiXBee"
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

// Time after turn on until the status pin is active - total WAG
#define XBEE_STATUS_TIME_MS 15
// Power down time "can largely vary depending
// on the application / network settings and the concurrent module
// activities."  Vint/status pin should be monitored and power not withdrawn
// until that pin reads low.  Giving 15sec here in case it is not monitored.
#define XBEE_DISCONNECT_TIME_MS 15000L

// Time after power-up before we can wake the model
#define XBEE_WARM_UP_TIME_MS 0
// Time we'll get back an AT response.  Probably much less than this, except in bypass
#define XBEE_ATRESPONSE_TIME_MS 5000L

// How long we're willing to wait to get signal quality
// This can be super slow for cellular modules
#define XBEE_SIGNALQUALITY_TIME_MS 45000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"


class DigiXBee : public loggerModem
{

public:
    // Constructor/Destructor
    DigiXBee(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
             int8_t modemResetPin, int8_t modemSleepRqPin,
             uint8_t measurementsToAverage = 1);
    virtual ~DigiXBee();

protected:
    virtual bool modemSleepFxn(void) override;
    virtual bool modemWakeFxn(void) override;
};

#endif  // Header Guard
