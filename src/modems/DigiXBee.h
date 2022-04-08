/**
 * @file DigiXBee.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DigiXBee modem subclass of loggerModem, which itself is a
 * parent class for all other XBee's and XBee3's.
 */
/* clang-format off */
/**
 * @defgroup modem_digi Digi XBee's
 *
 * @ingroup the_modems
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section modem_digi_notes Introduction
 *
 * All of the various cellular XBee's and XBee3's are supported by this library.
 * The wifi S6B is also supported.
 *
 * @warning This library does _NOT_ directly support the ZigBee, 900mHZ, or any
 * other radio-based XBee's.
 * It is for the cellular and WiFi Bee's only.
 *
 * @note All GPIOs are user configurable and are configured to use the expected
 * input/output mode in the modem setup functions.
 *
 *
 * @section modem_digi_mayfly-and-digi-xbee-connections Mayfly and Digi XBee Connections
 *
 * @subsection modem_digi_raw_pins Pin Numbers for connecting Digi XBee's Directly to a Mayfly v0.3-v0.5c
 *
 * This applies to _all_ Digi XBees and XBee3's when attached directly to the Mayfly's bee slot.
 * @code{cpp}
 * const int8_t modemVccPin = -1;      // MCU pin controlling modem power
 * const bool useCTSforStatus = true;  // Flag to use the XBee `CTS` pin for status
 * const int8_t modemStatusPin = 19;   // MCU pin used to read modem status
 * const int8_t modemResetPin = -1;    // MCU pin connected to modem reset pin
 * const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request
 * const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem status
 * @endcode
 *
 * - The Digi XBee gets power from pin 1; on a Mayfly this pin is always powered - the XBee cannot be powered down.
 *     - NOTE:  If using a cellular XBee or XBee3 directly with the Mayfly your Mayfly must be at least v0.5b, you must use SJ13 to connect the Bee directly to the LiPo, and you must always have a battery connected to provide enough power for the XBee to make a cellular connection.
 *     - NOTE 2:  If you turn off the Mayfly via its switch but leave the XBee connected as above, it will drain your battery very quickly.
 * Disconnect the battery if you turn off the Mayfly.
 * - The Digi XBee reports ON/SLEEP_N on pin 13, but this is not connected to a Mayfly pin.
 * Instead, use the XBee's `CTS` pin (pin 12) which is connected to Mayfly pin 19.
 * - XBee pin 5 is `RESET_N` but this is not connected to any pin on the Mayfly.
 * - XBee pin 9 is SLEEP_RQ which is connected to Mayfly pin 23.
 * - I like to use the red LED to show the bee wake/sleep since the Digi XBee's have no LEDs of their own.
 *
 * @subsection modem_digi_adapter_pins Pin Numbers for connecting Digi LTE XBee3's to a Mayfly v0.3-v0.5c via the LTE adapter board
 *
 * @code{cpp}
 * const int8_t modemVccPin = -1;       // MCU pin controlling modem power
 * const bool useCTSforStatus = false;  // Flag to use the XBee `CTS` pin for status
 * const int8_t modemStatusPin = 19;    // MCU pin used to read modem status
 * const int8_t modemResetPin = 20;     // MCU pin connected to modem reset pin
 * const int8_t modemSleepRqPin = 23;   // MCU pin used for modem sleep/wake request
 * const int8_t modemLEDPin = redLED;   // MCU pin connected an LED to show modem status
 * @endcode
 *
 * - The power is still not controllable unless you use both SJ7 on the Mayfly and SJ1 on the adapter.
 * Doing so, you could use pin A5 for modem Vcc.
 * - The LTE adapter connects ON/SLEEP_N on bee pin 13 to Mayfly pin 19 (unless you change this with adapter SJ2).
 * - XBee pin 5 is `RESET_N` which the adapter connects to Mayfly pin 20.
 * - XBee pin 9 is SLEEP_RQ which is connected still to Mayfly pin 23.
 * - I still like to use the red LED to show the bee wake/sleep in addition to the lights on the adapter.
 */
/* clang-format on */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEE_H_
#define SRC_MODEMS_DIGIXBEE_H_

// Debugging Statement
// #define MS_DIGIXBEE_DEBUG

#ifdef MS_DIGIXBEE_DEBUG
#define MS_DEBUGGING_STD "DigiXBee"
#endif

/** @ingroup modem_digi */
/**@{*/

/**
 * @brief The loggerModem::_statusTime_ms.
 *
 * XBee status can either be measured directly with `ON/SLEEP_N/DIO9` or
 * indirectly with `CTS_N/DIO7`.  The status level will depend on which is
 * being used:
 *     - the `ON/SLEEP_N/DIO9` will be `HIGH` when the XBee is awake
 * (ie, yes, I am not sleeping)
 *     - but the `CTS_N/DIO7` will be `LOW` when the
 * board is away (ie, no it's not not clear to send).
 *
 * To use the `CTS_N/DIO7` as the status indicator, set useCTSStatus to true in
 * the constructor.
 *
 * The time from wake until the status pin is active is not documented.
 */
#define XBEE_STATUS_TIME_MS 15

/**
 * @brief The loggerModem::_resetLevel.
 *
 * All Digi XBee/XBee3's are reset via a `LOW` pulse on the `RESET_N` pin.
 */
#define XBEE_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #XBEE_RESET_LEVEL
 */
#define XBEE_RESET_PULSE_MS 5

/**
 * @brief The loggerModem::_wakeLevel.
 *
 * All XBee/XBee3 modules are woken by holding `DTR_N/SLEEP_RQ/DIO8` `LOW`
 * (not pulsed).
 */
#define XBEE_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #XBEE_WAKE_LEVEL
 */
#define XBEE_WAKE_PULSE_MS 0

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 */
#define XBEE_WAKE_DELAY_MS 100
/**
 * @brief The loggerModem::_max_atresponse_time_ms.
 *
 * The serial response time is not documented for the XBee so we allow a long
 * (15s) buffer.  It is probably much less than this, except possibly in bypass
 * mode.
 */
#define XBEE_ATRESPONSE_TIME_MS 15000L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Power down time for the XBee is dependent on the underlying cellular or wifi
 * component.  We allow 15 seconds in case it is not monitored.  The u-blox SARA
 * R4 on the LTE-M model takes nearly that long to shut down.
 */
#define XBEE_DISCONNECT_TIME_MS 15000L

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerModem.h"


/**
 * @brief The parent class for all [Digi XBee and XBee3](@ref modem_digi) wifi
 * and cellular modules.
 *
 * All of the various cellular XBee's and XBee3's are supported.  The wifi S6B
 * is also supported.
 *
 * @note The ZigBee, 900mHZ, and other radio-based XBee's are **not**
 * supported.
 */
class DigiXBee : public loggerModem {
 public:
    /**
     * @brief Construct a new Digi XBee object
     *
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * This can be either the pin named `ON/SLEEP_N/DIO9` or `CTS_N/DIO7` pin in
     * Digi's hardware reference.
     * @param useCTSStatus True to use the `CTS_N/DIO7` pin of the XBee as a
     * status indicator rather than the true status (`ON/SLEEP_N/DIO9`) pin.
     * This inverts the loggerModem::_statusLevel.
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * This shold be the pin called `RESET_N` in Digi's hardware reference.
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * This shold be the pin called `DTR_N/SLEEP_RQ/DIO8` in Digi's hardware
     * reference.
     *
     * @see loggerModem::loggerModem
     */
    DigiXBee(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
             int8_t modemResetPin, int8_t modemSleepRqPin);
    /**
     * @brief Destroy the Digi XBee object - no action taken
     */
    virtual ~DigiXBee();

 protected:
    bool modemSleepFxn(void) override;
    bool modemWakeFxn(void) override;
};
/**@}*/
#endif  // SRC_MODEMS_DIGIXBEE_H_
