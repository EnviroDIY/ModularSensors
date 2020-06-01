/**
 * @file SodaqUBeeR410M.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SodaqUBeeR410M subclass of loggerModem for the Sodaq UBee
 * based on the u-blox SARA R410M LTE-M cellular module.
 */

// Header Guards
#ifndef SRC_MODEMS_SODAQUBEER410M_H_
#define SRC_MODEMS_SODAQUBEER410M_H_

// Debugging Statement
// #define MS_SODAQUBEER410M_DEBUG
// #define MS_SODAQUBEER410M_DEBUG_DEEP

#ifdef MS_SODAQUBEER410M_DEBUG
#define MS_DEBUGGING_STD "SodaqUBeeR410M"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SARAR4
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * V_INT on the SARA R4 becomes active mid-way through on-pulse so it should be
 * instantly visible
 *
 * Status should be monitored on the V_INT pin
 */
#define R410M_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails R410M_STATUS_LEVEL
 */
#define R410M_STATUS_TIME_MS 0

/**
 * @brief The loggerModem::_resetLevel.
 *
 * R4 series are reset with a >10 SECOND low pulse on the RESET_N pin
 */
#define R410M_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails R410M_RESET_LEVEL
 */
#define R410M_RESET_PULSE_MS 10000L

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The SARA R410M is switched on by a 0.15-3.2 second LOW pulse on the PWR_ON
 * pin
 */
#define R410M_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #R410M_WAKE_LEVEL
 */
#define R410M_WAKE_PULSE_MS 200
/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Time after power on before PWR_ON on SARA R4 can be used is nclear in
 * documentation!  Using 250ms.
 */
#define R410M_WAKE_DELAY_MS 250
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time until system and digital pins on SARA R4 are operational is ~4.5s.
 */
#define R410M_ATRESPONSE_TIME_MS 4500L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Power down time fpr u-blox modueles "can largely vary depending on the
 * application / network settings and the concurrent module activities."
 * Vint/status pin should be monitored and power not withdrawn until that pin
 * reads low.  Giving 15sec here in case it is not monitored.
 */
#define R410M_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SODAQUBEER410M_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for the Sodaq UBee based on the u-blox SARA
 * R410M LTE-M cellular module.
 *
 * @copydetails #R410M_STATUS_LEVEL
 * @copydetails #R410M_RESET_LEVEL
 * @copydetails #R410M_WAKE_LEVEL
 * @copydetails #R410M_WAKE_DELAY_MS
 * @copydetails #R410M_ATRESPONSE_TIME_MS
 * @copydetails #R410M_DISCONNECT_TIME_MS
 */
class SodaqUBeeR410M : public loggerModem {
 public:
    // Constructor/Destructor

#if F_CPU == 8000000L
    // At this slow baud rate, we need to begin and end serial communication,
    // so we need a Serial instance rather than a stream
    /**
     * @brief Construct a new Sodaq UBee R410M object
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * @param apn The Access Point Name (APN) for the SIM card.
     */
    SodaqUBeeR410M(HardwareSerial* modemStream, int8_t powerPin,
                   int8_t statusPin, int8_t modemResetPin,
                   int8_t modemSleepRqPin, const char* apn);
#else
    /**
     * @brief Construct a new Sodaq UBee R410M object
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * @param apn The Access Point Name (APN) for the SIM card.
     */
    SodaqUBeeR410M(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                   int8_t modemResetPin, int8_t modemSleepRqPin,
                   const char* apn);
#endif
    /**
     * @brief Destroy the Sodaq UBee R410M object - no action needed
     */
    ~SodaqUBeeR410M();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

    bool modemHardReset(void) override;

#ifdef MS_SODAQUBEER410M_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif

    TinyGsm       gsmModem;
    TinyGsmClient gsmClient;

#if F_CPU == 8000000L
    HardwareSerial* _modemSerial;
#endif

 protected:
    bool isInternetAvailable(void) override;
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
    bool extraModemSetup(void) override;
    bool isModemAwake(void) override;

 private:
    const char* _apn;
};

#endif  // SRC_MODEMS_SODAQUBEER410M_H_
