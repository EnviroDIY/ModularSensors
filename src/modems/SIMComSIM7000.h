/**
 * @file SIMComSIM7000.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SIMComSIM7000 subclass of loggerModem for
 * Botletics other modules based on the SIMCOM SIM7000.
 */

// Header Guards
#ifndef SRC_MODEMS_SIMCOMSIM7000_H_
#define SRC_MODEMS_SIMCOMSIM7000_H_

// Debugging Statement
// #define MS_SIMCOMSIM7000_DEBUG
// #define MS_SIMCOMSIM7000_DEBUG_DEEP

#ifdef MS_SIMCOMSIM7000_DEBUG
#define MS_DEBUGGING_STD "SIMComSIM7000"
#endif

#define TINY_GSM_MODEM_SIM7000
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

// Status should be monitored on the STATUS pin, which is at a high level
// when the module has powered on and the firmware goes ready
// Time after end pulse until status pin becomes active (>4.5sec)
#define SIM7000_STATUS_LEVEL HIGH
#define SIM7000_STATUS_TIME_MS 5000L

// R4 series are reset with a >10 SECOND low pulse on the RESET_N pin
#define SIM7000_RESET_LEVEL LOW
#define SIM7000_RESET_PULSE_MS 10000L

// Module is switched on by a >1 second LOW pulse on the PWRKEY pin
// NOTE: Module is switched OFF by a >1.2 second LOW pulse on the PWRKEY pin,
// so by using a pulse of >1 but <1.2 s to wake the SIM7000 and using AT
// commands to put it to sleep, we should always be in the correct state
#define SIM7000_WAKE_LEVEL LOW
#define SIM7000_WAKE_PULSE_MS 1100
// Time after power on before "PWRKEY" can be used (guess - diagram isn't clear)
#define SIM7000_WARM_UP_TIME_MS 1000L
// Time after end pulse until serial port becomes active (>4.5sec)
#define SIM7000_ATRESPONSE_TIME_MS 4500

// power down (gracefully) takes 1.8-6.9 sec
#define SIM7000_DISCONNECT_TIME_MS 7000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


class SIMComSIM7000 : public loggerModem {
 public:
    // Constructor/Destructor
    SIMComSIM7000(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char* apn);
    ~SIMComSIM7000();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SIMCOMSIM7000_DEBUG_DEEP
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

#endif  // SRC_MODEMS_SIMCOMSIM7000_H_
