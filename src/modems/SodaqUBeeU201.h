/**
 * @file SodaqUBeeU201.h
 * @brief Contains the SodaqUBeeU201 subclass of loggerModem for the Sodaq UBee
 * based on the u-blox SARA U201 3G cellular module.
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_MODEMS_SODAQUBEEU201_H_
#define SRC_MODEMS_SODAQUBEEU201_H_

// Debugging Statement
// #define MS_SODAQUBEEU201_DEBUG
// #define MS_SODAQUBEEU201_DEBUG_DEEP

#ifdef MS_SODAQUBEEU201_DEBUG
#define MS_DEBUGGING_STD "SodaqUBeeU201"
#endif

#define TINY_GSM_MODEM_UBLOX
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Status should be monitored on the V_INT pin
// The time after end of wake pulse until V_INT becomes active is
// unspecified in documentation; Taking value from Lisa U2
#define U201_STATUS_LEVEL HIGH
#define U201_STATUS_TIME_MS 100

// U201 is reset with a >50ms low pulse on the RESET_N pin
#define U201_RESET_LEVEL LOW
#define U201_RESET_PULSE_MS 75

// Module is switched on by a 50-80 MICRO second LOW pulse on the PWR_ON pin
#define U201_WAKE_LEVEL LOW
#define U201_WAKE_PULSE_MS 1
#define U201_WAKE_TIME_US 65
// Module turns on when power is applied - level of PWR_ON then irrelevant
#define U201_WARM_UP_TIME_MS 0
// Time until system and digital pins are operational
// (6 sec typical for SARA U201)
#define U201_ATRESPONSE_TIME_MS 6000L

// Power down time "can largely vary depending
// on the application / network settings and the concurrent module
// activities."  Vint/status pin should be monitored and power not withdrawn
// until that pin reads low.  Giving 15sec here in case it is not monitored.
#define U201_DISCONNECT_TIME_MS 15000L


// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SODAQUBEEU201_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

class SodaqUBeeU201 : public loggerModem {
 public:
    // Constructor/Destructor
    SodaqUBeeU201(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char* apn);
    ~SodaqUBeeU201();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SODAQUBEEU201_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm       gsmModem;
    TinyGsmClient gsmClient;

 protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;
};

#endif  // SRC_MODEMS_SODAQUBEEU201_H_
