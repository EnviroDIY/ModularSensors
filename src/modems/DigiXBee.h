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

    // Access the internet
    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) = 0;
    virtual void disconnectInternet(void) = 0;

    // Get values by other names
    virtual bool getModemSignalQuality(int16_t &rssi, int16_t &percent) = 0;
    virtual bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) = 0;
    virtual float getModemTemperature(void) = 0;

    // Get the time from NIST via TIME protocol (rfc868)
    // This would be much more efficient if done over UDP, but I'm doing it
    // over TCP because I don't have a UDP library for all the modems.
    virtual uint32_t getNISTTime(void) = 0;

protected:
    virtual bool didATRespond(void) = 0;
    virtual bool isInternetAvailable(void) = 0;
    virtual bool verifyMeasurementComplete(bool debug = false) = 0;
    virtual bool modemSleepFxn(void) override;
    virtual bool modemWakeFxn(void) override;
    virtual bool extraModemSetup(void) = 0;
};

#endif  // Header Guard
