/**
 * @file DigiXBee3GBypass.h
 * @brief
 * @brief This file is the DigiXBee3GBypass subclass of the DigiXBee class for
 * Digi Cellular XBee's based on u-blox SARA U201 chips and operated in bypass
 * mode.
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEE3GBYPASS_H_
#define SRC_MODEMS_DIGIXBEE3GBYPASS_H_

// Debugging Statement
// #define MS_DIGIXBEE3GBYPASS_DEBUG
// #define MS_DIGIXBEE3GBYPASS_DEBUG_DEEP

#ifdef MS_DIGIXBEE3GBYPASS_DEBUG
#define MS_DEBUGGING_STD "DigiXBee3GBypass"
#endif

#define TINY_GSM_MODEM_UBLOX
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "DigiXBee.h"

#ifdef MS_DIGIXBEE3GBYPASS_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class DigiXBee3GBypass : public DigiXBee {
 public:
    // Constructor/Destructor
    DigiXBee3GBypass(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                     bool useCTSStatus, int8_t modemResetPin,
                     int8_t modemSleepRqPin, const char* apn);
    ~DigiXBee3GBypass();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool modemHardReset(void) override;

#ifdef MS_DIGIXBEE3GBYPASS_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm       gsmModem;
    TinyGsmClient gsmClient;

 protected:
    bool isInternetAvailable(void) override;
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;
};

#endif  // SRC_MODEMS_DIGIXBEE3GBYPASS_H_
