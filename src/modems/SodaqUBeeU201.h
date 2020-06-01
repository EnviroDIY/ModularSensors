/**
 * @file SodaqUBeeU201.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SodaqUBeeU201 subclass of loggerModem for the Sodaq UBee
 * based on the u-blox SARA U201 3G cellular module.
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

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_UBLOX
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif

/**
 * @brief The loggerModem::_statusLevel.
 *
 * Status of the U201 should be monitored on the V_INT pin.  The time after end
 * of wake pulse until V_INT becomes active is unspecified in documentation;
 * Taking value from Lisa U2, which is 100ms.
 */
#define U201_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails U201_STATUS_LEVEL
 */
#define U201_STATUS_TIME_MS 100

/**
 * @brief The loggerModem::_resetLevel.
 *
 * U201 is reset with a >50ms low pulse on the RESET_N pin
 */
#define U201_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails U201_RESET_LEVEL
 */
#define U201_RESET_PULSE_MS 75

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The SARA U201 is switched on by a 50-80 MICRO second LOW pulse on the PWR_ON
 * pin.
 */
#define U201_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #U201_WAKE_LEVEL
 */
#define U201_WAKE_PULSE_MS 1

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * The SARA U201 turns on when power is applied - the level of PWR_ON then is
 * irrelevant.  No separate pulse or other wake is needed.
 */
#define U201_WAKE_DELAY_MS 0
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time until system and digital pins are operational is 6 sec typical for SARA
 * U201.
 */
#define U201_ATRESPONSE_TIME_MS 6000L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Power down time fpr u-blox modueles "can largely vary depending on the
 * application / network settings and the concurrent module activities."
 * Vint/status pin should be monitored and power not withdrawn until that pin
 * reads low.  Giving 15sec here in case it is not monitored.
 */
#define U201_DISCONNECT_TIME_MS 15000L


// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SODAQUBEEU201_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for the Sodaq UBee based on the u-blox SARA
 * U201 3G cellular module.
 *
 * @copydetails #U201_STATUS_LEVEL
 * @copydetails #U201_RESET_LEVEL
 * @copydetails #U201_WAKE_LEVEL
 * @copydetails #U201_WAKE_DELAY_MS
 * @copydetails #U201_ATRESPONSE_TIME_MS
 * @copydetails #U201_DISCONNECT_TIME_MS
 */
class SodaqUBeeU201 : public loggerModem {
 public:
    /**
     * @brief Construct a new Sodaq UBee U201 object
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * @param apn The Access Point Name (APN) for the SIM card.
     */
    SodaqUBeeU201(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                  int8_t modemResetPin, int8_t modemSleepRqPin,
                  const char* apn);
    /**
     * @brief Destroy the Sodaq UBee U201 object - no action taken
     */
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
