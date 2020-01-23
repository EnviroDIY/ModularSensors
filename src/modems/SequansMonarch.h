/*
 *SequansMonarch.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Dragino BG96, Nimbelink Skywire 4G LTE-M Global, and
 *other modules based on the Quectel BG96.
*/

// Header Guards
#ifndef SEQUANSMONARCH_h
#define SEQUANSMONARCH_h

// Debugging Statement
// #define MS_SEQUANSMONARCH_DEBUG
// #define MS_SEQUANSMONARCH_DEBUG_DEEP

#ifdef MS_SEQUANSMONARCH_DEBUG
#define MS_DEBUGGING_STD "SequansMonarch"
#endif

#define TINY_GSM_MODEM_SEQUANS_MONARCH
#define MS_MODEM_HAS_TEMPERATURE_DATA
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Depending on firmware, you MIGHT be able to monitor the status on either GPIO2/POWER_MON or GPIO3/STATUS_LED
// The module integration guide says:
// GPIO3: Optional STATUS_LED.  Note that the LED function is currently not available.
// GPIO2:  GPIO or Power monitor (Output) in option.  POWER_MON is high right after POWER_ON,
// then remains high until shutdown procedure is completed.  Module can be safely electrically power off
// as soon as POWER_MON goes low.  Note that this feature is currently not available.
// Very useful, right?
// The Nimbelink manual for their breakout lists a status pin, but doesn't disclose which of these it is and
// the time for reporting isn't mentioned either.
#define VZM20Q_STATUS_LEVEL HIGH
#define VZM20Q_STATUS_TIME_MS 5000

// Minimum 1µs LOW pulse on RESETN for reset - fast.  Max time not documented.
#define VZM20Q_RESET_LEVEL LOW
#define VZM20Q_RESET_PULSE_MS 1

// Module automatically boots when power is applied
// Nimbelink's documentation says of the wake pin: "Default configuration forwakeup is a low to high transition
// on this line."  There is no further documentation on timing or anything else.
#define VZM20Q_WARM_UP_TIME_MS 0
#define VZM20Q_WAKE_LEVEL HIGH
#define VZM20Q_WAKE_PULSE_MS 200
// ?? Time to UART availability not documented
#define VZM20Q_ATRESPONSE_TIME_MS 15000L

// ?? Undocumented (Giving 15sec here in case it is not monitored.)
#define VZM20Q_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class SequansMonarch : public loggerModem
{

public:
    // Constructor/Destructor
    SequansMonarch(Stream* modemStream,
                   int8_t powerPin, int8_t statusPin,
                   int8_t modemResetPin, int8_t modemSleepRqPin,
                   const char* apn);
    ~SequansMonarch();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool getModemBatteryStats(uint8_t& chargeState, int8_t& percent, uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm gsmModem;
    TinyGsmClient gsmClient;

protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;

private:
    const char* _apn;
};

#endif  // Header Guard
