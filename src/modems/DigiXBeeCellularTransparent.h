/*
 *DigiXBeeCellularTransparent.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi Cellular XBee's
*/

// Header Guards
#ifndef DigiXBeeCellularTransparent_h
#define DigiXBeeCellularTransparent_h

// Debugging Statement
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP

#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
#define MS_DEBUGGING_STD "DigiXBeeCellularTransparent"
#endif
#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "DigiXBeeCellularTransparentD"
#endif
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

#include "DigiXBee.h"
#include "TinyGsmClient.h"

#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class DigiXBeeCellularTransparent : public DigiXBee
{

public:
    // Constructor/Destructor
    DigiXBeeCellularTransparent(Stream* modemStream,
                                int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                                int8_t modemResetPin, int8_t modemSleepRqPin,
                                const char *apn,
                                uint8_t measurementsToAverage = 1);
    ~DigiXBeeCellularTransparent();

    bool addSingleMeasurementResult(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    // Get values by other names
    bool getModemSignalQuality(int16_t &rssi, int16_t &percent) override;
    bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) override;
    float getModemTemperature(void) override;

    uint32_t getNISTTime(void) override;
    uint32_t getTimeNIST(void);
    uint32_t getTimeNTP(void);
    uint32_t getTimeCellTower(void);


    #ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
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
