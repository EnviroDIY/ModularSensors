/*
 *ublox2and3G.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all u-blox 3G/2G modules.  This is an extra level
 *on top of the DigiXBee3GBypass and the Sodaq UBee U201 needed so that
 *TinyGSMublox.h is only included once.  Because that h file contains the
 *definitions of functions instead of mearly their declarations, including
 *it more than once causes a violation of the one-definition rule and
 *possible undefined behavior (ie, bad things).
*/

// Header Guards
#ifndef ublox2and3G_h
#define ublox2and3G_h

// Debugging Statement
// #define MS_UBLOX2AND3G_DEBUG
// #define MS_UBLOX2AND3G_DEBUG_DEEP

#ifdef MS_UBLOX2AND3G_DEBUG
#define MS_DEBUGGING_STD "ublox2and3G"
#endif

#define TINY_GSM_MODEM_UBLOX

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"
#include "TinyGsmClient.h"

#ifdef MS_UBLOX2AND3G_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class ublox2and3G : public loggerModem
{

public:
    // Constructor/Destructor
    ublox2and3G(Stream *modemStream,
                int8_t powerPin, int8_t statusPin, bool statusLevel,
                int8_t modemResetPin, int8_t modemSleepRqPin, bool alwaysRunWake,
                uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                uint32_t max_warmUpTime_ms, uint32_t max_atresponse_time_ms,
                uint32_t max_signalQuality_time_ms,
                const char *apn,
                uint8_t measurementsToAverage = 1);
    ~ublox2and3G();

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    // Get values by other names
    bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    float getModemTemperature(void) override;

    uint32_t getNISTTime(void) override;

#ifdef MS_UBLOX2AND3G_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    bool didATRespond(void) override;
    bool isInternetAvailable(void) override;
    bool verifyMeasurementComplete(bool debug = false) override;

    const char *_apn;
};

#endif // Header Guard