/*
 *DigiXBee.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA R410M LTE-M Cellular Module
*/

// Header Guards
#ifndef DigiXBee_h
#define DigiXBee_h

// Debugging Statement
// #define MS_DIGIXBEEWIFI_DEBUG

#ifdef MS_DIGIXBEEWIFI_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#define TINY_GSM_XBEE_XBEE  // Select for Digi brand WiFi or Cellular XBee's

#define XBEE_STATUS_TIME_MS 15
#define XBEE_DISCONNECT_TIME_MS 5000L

#define XBEE_WARM_UP_TIME_MS 50
#define XBEE_ATRESPONSE_TIME_MS 5000L


class DigiXBee : public loggerModem
{

public:
    // Constructors
    DigiXBee(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                int8_t modemResetPin, int8_t modemSleepRqPin,
                TinyGsmModem *inModem, Client *inClient, const char *ssid, const char *pwd);

    virtual bool setup(void) override;
    virtual bool wake(void) override;
    virtual bool modemSleepPowerDown(void);

protected:
    virtual bool XBeeWake(void);
    virtual bool XBeeSleep(void);
    virtual bool extraXBeeSetup(void) = 0;
};



#endif
