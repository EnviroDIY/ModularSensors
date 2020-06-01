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

// NOTE:  all GPIOs are user configurable and are configured to follow these
// settings in the modem setup functions.

/**
 * @brief The loggerModem::_statusTime_ms.
 *
 * XBee status can either be measured directly with ON/SLP_N/DIO9 or indirectly
 * with CTS_N/DIO9 The status level will depend on which is being used - the
 * ON/SLP_N will be HIGH when the XBee is awake (ie, yes, I am not sleeping) but
 * the CTS_N will be LOW when the board is away (ie, no it's not not clear to
 * send). (double negatives much?) To use the CTS as the status indicator, set
 * useCTSStatus to true in the constructor.
 *
 * The time from wake until the status pin is active is not documented
 */
#define XBEE_STATUS_TIME_MS 15

/**
 * @brief The loggerModem::_resetLevel.
 *
 * All Digi XBee/XBee3's are reset via a LOW pulse on the RESET_N pin
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
 * All XBee/XBee3 modules are woken by holding DTR_N/SLP_RQ/DIO8 LOW (not
 * pulsed)
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
 * The serial response time is not documented for the XBee; allowing a long 15s
 * buffer. It is probably much less than this, except possibly in bypass mode.
 */
#define XBEE_ATRESPONSE_TIME_MS 15000L

/**
 * @brief The loggerModem::_disconnetTime_ms.
 *
 * Power down time for the XBee is dependent on the underlying cellular or wifi
 * component.  Giving 15sec here in case it is not monitored.  The u-blox SARA
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
 * @copydetails #XBEE_STATUS_TIME_MS
 * @copydetails #XBEE_RESET_LEVEL
 * @copydetails #XBEE_WAKE_LEVEL
 * @copydetails #XBEE_STATUS_TIME_MS
 * @copydetails #XBEE_ATRESPONSE_TIME_MS
 * @copydetails #XBEE_DISCONNECT_TIME_MS
 */
class DigiXBee : public loggerModem {
 public:
    /**
     * @brief Construct a new Digi XBee object
     *
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * @param useCTSStatus True to use the CTS pin of the XBee as a status
     * indicator rather than the true status pin.  This inverts the
     * loggerModem::_statusLevel.
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
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
