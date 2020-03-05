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
#endif

// NOTE:  all GPIOs are user configurable and are configured to follow these settings in the
// modem setup functions.

// Status can either be measured directly with ON/SLP_N/DIO9 or indirectly with CTS_N/DIO9
// The status level will depend on which is being used - the ON/SLP_N will be HIGH when the XBee
// is awake (ie, yes, I am not sleeping) but the CTS_N will be LOW when the board is away
// (ie, no it's not not clear to send). [double negatives much?]
// Time after turn on until the status pin is active - total WAG
#define XBEE_STATUS_TIME_MS 15

// Reset is via a LOW pulse on the RESET_N pin
#define XBEE_RESET_LEVEL LOW
#define XBEE_RESET_PULSE_MS 5

// Module is woken by holding DTR_N/SLP_RQ/DIO8 LOW (not pulsed)
#define XBEE_WAKE_LEVEL LOW
#define XBEE_WAKE_PULSE_MS 0
// Time after power-up before we can wake the model
#define XBEE_WARM_UP_TIME_MS 100
// Time we'll get back an AT response.  Probably much less than this, except in bypass
#define XBEE_ATRESPONSE_TIME_MS 15000L

// Power down time "can largely vary depending
// on the application / network settings and the concurrent module
// activities."  Vint/status pin should be monitored and power not withdrawn
// until that pin reads low.  Giving 15sec here in case it is not monitored.
#define XBEE_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"


class DigiXBee : public loggerModem
{

public:
    // Constructor/Destructor
    DigiXBee(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
             int8_t modemResetPin, int8_t modemSleepRqPin);
    virtual ~DigiXBee();

protected:
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
};

#endif  // Header Guard
