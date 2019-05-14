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
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define XBEE_STATUS_TIME_MS 15
#define XBEE_DISCONNECT_TIME_MS 5000L

#define XBEE_WARM_UP_TIME_MS 50
#define XBEE_ATRESPONSE_TIME_MS 5000L

#define XBEE_SIGNALQUALITY_TIME_MS 45000L

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"


class DigiXBee : public loggerModem
{

public:
    // Constructors
    DigiXBee(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
             int8_t modemResetPin, int8_t modemSleepRqPin,
             uint8_t measurementsToAverage = 1);

protected:
    virtual bool modemSleepFxn(void) override;
    virtual bool modemWakeFxn(void) override;
};



#endif
