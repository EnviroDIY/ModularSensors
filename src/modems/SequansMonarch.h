/**
 * @file SequansMonarch.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SequansMonarch subclass of loggerModem for Nimbelink or
 * other modules based on the Sequans Monarch VZM20Q.
 */
/* clang-format off */
/**
 * @defgroup modem_monarch Sequans Monarch VZM20Q
 *
 * @ingroup the_modems
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_monarch_notes Introduction
 *
 * The Sequans Monarch
 * [VZM20Q](https://www.sequans.com/products-solutions/streamlitelte/monarch-lte-platform/ezlinklte-vzm20q/)
 * is another LTE CatM1 chip.
 * It's available in "Skywire" (XBee-like) format from
 * [Nimbelink](https://nimbelink.com/products/4g-lte-m-verizon-sequans/).

 *
 * @section modem_monarch_mayfly Monarchs and Mayflys
 *
 * To my knowledge, there are not any Sequans modules available that can
 * directly connect to a Mayfly.
 * Although the Nimbelink board linked above would fit the physical XBee
 * footprint on the Mayfly, it will _not_ work without some additional
 * interface or other modifications.
 * The power supply on the Mayfly is insufficient, the voltage reference is not
 * implemented on the Mayfly, and the Nimbelink board requires 3 extra ground
 * pins that aren't available on the Mayfly.
 *
 * @section modem_monarch_docs Manufacturer Documentation
 * The module datasheet and AT commands are available here:
 * https://www.sequans.com/products-solutions/streamlitelte/monarch-lte-platform/ezlinklte-vzm20q/
 *
 * @section modem_monarch_ctor Modem Constructor
 * {{ @ref SequansMonarch::SequansMonarch }}
 *
 * ___
 * @section modem_monarch_examples Example Code
 * The monarch is used in the @menulink{sequans_monarch} example.
 *
 * @menusnip{sequans_monarch}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_SEQUANSMONARCH_H_
#define SRC_MODEMS_SEQUANSMONARCH_H_

// Debugging Statement
// #define MS_SEQUANSMONARCH_DEBUG
// #define MS_SEQUANSMONARCH_DEBUG_DEEP

#ifdef MS_SEQUANSMONARCH_DEBUG
#define MS_DEBUGGING_STD "SequansMonarch"
#endif

/** @ingroup modem_monarch */
/**@{*/

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_SEQUANS_MONARCH
#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif


/**
 * @brief The loggerModem::_statusLevel.
 *
 * Depending on firmware, you MIGHT be able to monitor the status on either
 * `GPIO2/POWER_MON` or `GPIO3/STATUS_LED`
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
 *
 * @copydetails #VZM20Q_STATUS_LEVEL
 */
#define VZM20Q_STATUS_TIME_MS 5000

/**
 * @brief The loggerModem::_resetLevel.
 *
 * Reset for VZM20Q with a minimum 1µs `LOW` pulse on `RESETN`.  The maximum
 * time is not documented.
 */
#define VZM20Q_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 *
 * @copydetails #VZM20Q_RESET_LEVEL
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
 * 3. Pull the `RTS` pin logic - level `HIGH` - device will enter PSM a minimum
 * of 100s later
 *
 * To exit PSM, you need to do the following :
 * 1. Pull the `RTS` pin logic - level `LOW`
 */
#define VZM20Q_WAKE_DELAY_MS 0
/**
 * @brief The loggerModem::_wakeLevel.
 *
 * The Sequans Monarch/VZM20Q wakes at `LOW` level.
 *
 * @note This wake functionality is unconfirmed.
 *
 */
#define VZM20Q_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 *
 * @copydetails #VZM20Q_WAKE_LEVEL
 */
#define VZM20Q_WAKE_PULSE_MS 0
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * Time to UART availability not documented for the VZM20Q; allowing a long 15s
 * buffer.
 */
#define VZM20Q_ATRESPONSE_TIME_MS 15000L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Shutdown time for VZM20Q is undocumented.  We allow 15sec in case it is not
 * monitored.
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
 * [Sequans Monarch VZM20Q](@ref modem_monarch).
 */
class SequansMonarch : public loggerModem {
 public:
    /**
     * @brief Construct a new Sequans Monarch object
     *
     * The constuctor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * This could be the pin called either the `GPIO3/STATUS_LED` or `POWER_MON`
     * in the Sequans Monarch integration guide.
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * This should be the pin called `RESETN` in the Sequans Monarch integration
     * guide.
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * This should be the pin called `RTS` or `RTS0` in the Sequans Monarch
     * integration guide.
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @see loggerModem::loggerModem
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

    /**
     * @brief Public reference to the TinyGSM modem.
     */
    TinyGsm gsmModem;
    /**
     * @brief Public reference to the TinyGSM Client.
     */
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
/**@}*/
#endif  // SRC_MODEMS_SEQUANSMONARCH_H_
