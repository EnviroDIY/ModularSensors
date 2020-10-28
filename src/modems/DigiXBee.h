/**
 * @file DigiXBee.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the DigiXBee modem subclass of loggerModem, which itself is a
 * parent class for all other XBee's and XBee3's.
 */

// Header Guards
#ifndef SRC_MODEMS_DIGIXBEE_H_
#define SRC_MODEMS_DIGIXBEE_H_

// Debugging Statement
// #define MS_DIGIXBEE_DEBUG

#ifdef MS_DIGIXBEE_DEBUG
#define MS_DEBUGGING_STD "DigiXBee"
#endif

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
 * @brief The parent class for all Digi XBee and XBee3 wifi and cellular
 * modules.
 *
 * All of the various cellular XBee's and XBee3's are supported.  The wifi S6B
 * is also supported.
 *
 * #### Pin and timing information for the XBees
 *
 * @copydetails #XBEE_STATUS_TIME_MS
 *
 * @copydetails #XBEE_RESET_LEVEL
 *
 * @copydetails #XBEE_WAKE_LEVEL
 *
 * @copydetails #XBEE_STATUS_TIME_MS
 *
 * @copydetails #XBEE_ATRESPONSE_TIME_MS
 *
 * @copydetails #XBEE_DISCONNECT_TIME_MS
 *
 * @note All GPIOs are user configurable and are configured to use the expected
 * input/output mode in the modem setup functions.
 *
 * @note The ZigBee, 900mHZ, and other radio-based XBee's are not
 * supported.
 *
 * @see @ref page_digi_xbees
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

#endif  // SRC_MODEMS_DIGIXBEE_H_
