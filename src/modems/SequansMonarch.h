/**
 * @file SequansMonarch.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SequansMonarch subclass of loggerModem for Nimbelink or
 * other modules based on the Sequans Monarch VZM20Q.
 */

// Header Guards
#ifndef SRC_MODEMS_SEQUANSMONARCH_H_
#define SRC_MODEMS_SEQUANSMONARCH_H_

// Debugging Statement
// #define MS_SEQUANSMONARCH_DEBUG
// #define MS_SEQUANSMONARCH_DEBUG_DEEP

#ifdef MS_SEQUANSMONARCH_DEBUG
#define MS_DEBUGGING_STD "SequansMonarch"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SEQUANS_MONARCH
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * Depending on firmware, you MIGHT be able to monitor the status on either
 * GPIO2/POWER_MON or GPIO3/STATUS_LED
 *
 * The module integration guide says:
 * > GPIO3: Optional STATUS_LED.  _Note that the LED function is currently not
 * > available._
 * >
 * > GPIO2:  GPIO or Power monitor (Output) in option.  POWER_MON is high right
 * > after POWER_ON, then remains high until shutdown procedure is completed.
 * > Module can be safely electrically power off as soon as POWER_MON goes
 * > low. _Note that this feature is currently not available._
 *
 * Very useful, right?
 *
 * The Nimbelink manual for their breakout lists a status pin, but doesn't
 * disclose which of these it is and the time for reporting isn't mentioned
 * either.
 */
#define VZM20Q_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails VZM20Q_STATUS_LEVEL
 */
#define VZM20Q_STATUS_TIME_MS 5000

/**
 * @brief The loggerModem::_resetLevel.
 *
 * Reset for VZM20Q with a minimum 1µs LOW pulse on RESETN.  Max time not
 * documented.
 */
#define VZM20Q_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails VZM20Q_RESET_LEVEL
 */
#define VZM20Q_RESET_PULSE_MS 1

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Module automatically boots when power is applied, no further command is
 * needed.
 *
 * To enter PSM (power save mode), you need to do the following :
 * 1. Request timers from the network
 * 2. Register on the network
 * 3. Pull the RTS pin logic - level HIGH - device will enter PSM a minimum of
 * 100s later
 *
 * To exit PSM, you need to do the following :
 * 1. Pull the RTS pin logic - level LOW
 */
#define VZM20Q_WAKE_DELAY_MS 0
/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The Sequans Monarch/VZM20Q wakes at LOW level.
 *
 * @note This wake functionality is unconfirmed.
 */
#define VZM20Q_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #VZM20Q_WAKE_LEVEL
 */
#define VZM20Q_WAKE_PULSE_MS 0
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 * Time to UART availability not documented for the VZM20Q; allowing a long 15s
 * buffer.
 */
#define VZM20Q_ATRESPONSE_TIME_MS 15000L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Shutdown time for VZM20Q is undocumented.  Giving 15sec here in case it is
 * not monitored.
 */
#define VZM20Q_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "TinyGsmClient.h"
#include "LoggerModem.h"

#ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
#include <StreamDebugger.h>
#endif


/**
 * @brief The loggerModem subclass for Nimbelink or other modules based on the
 * Sequans Monarch VZM20Q.
 *
 * @copydetails #VZM20Q_STATUS_LEVEL
 * @copydetails #VZM20Q_RESET_LEVEL
 * @copydetails #VZM20Q_WAKE_LEVEL
 * @copydetails #VZM20Q_WAKE_DELAY_MS
 * @copydetails #VZM20Q_ATRESPONSE_TIME_MS
 * @copydetails #VZM20Q_DISCONNECT_TIME_MS
 */
class SequansMonarch : public loggerModem {
 public:
    /**
     * @brief Construct a new Sequans Monarch object
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * *
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * @param statusLevel @copydoc loggerModem::_statusLevel
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * @param apn The Access Point Name (APN) for the SIM card.
     */
    SequansMonarch(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                   int8_t modemResetPin, int8_t modemSleepRqPin,
                   const char* apn);
    /**
     * @brief Destroy the Sequans Monarch object - no action taken
     */
    ~SequansMonarch();

    bool modemWake(void) override;

    bool connectInternet(uint32_t maxConnectionTime = 50000L) override;
    void disconnectInternet(void) override;

    uint32_t getNISTTime(void) override;

    bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) override;
    bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                               uint16_t& milliVolts) override;
    float getModemChipTemperature(void) override;

#ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
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

#endif  // SRC_MODEMS_SEQUANSMONARCH_H_
