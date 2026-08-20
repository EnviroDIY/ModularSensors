/**
 * @file QuectelBG96_Template_Example.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Example of QuectelBG96 using the new template-based architecture
 *
 * This file demonstrates how to migrate from the macro-based approach to the
 * template-based approach. This is NOT yet integrated into the build - it's
 * a reference implementation showing the migration path.
 */

// Header Guards
#ifndef SRC_MODEMS_QUECTELBG96_H_
#define SRC_MODEMS_QUECTELBG96_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_QUECTELBG96_DEBUG
#define MS_DEBUGGING_STD "QuectelBG96"
#endif
#ifdef MS_QUECTELBG96_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "QuectelBG96"
#endif

/**
 * @brief The modem type for the underlying TinyGSM library.
 */
#define TINY_GSM_MODEM_BG96

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "TinyGsmClientBG96.h"
#include "LoggerModemImpl.h"

#ifdef MS_QUECTELBG96_DEBUG_DEEP
#include <StreamDebugger.h>
#endif

/** @ingroup modem_bg96 */
/**@{*/

/**
 * @anchor modem_bg96_pins_timing
 * @name Modem Pin Settings and Timing
 * The timing and pin level settings for a Quectel BG96
 */
/**@{*/
/**
 * @brief The loggerModem::_statusLevel.
 *
 * Status of the BG96 can be monitored on the `STATUS(D0)` pin.  Time after end
 * pulse until status pin becomes active is 4.8s.
 */
#define BG96_STATUS_LEVEL HIGH
/**
 * @brief The loggerModem::_statusTime_ms.
 * @copydetails #BG96_STATUS_LEVEL
 */
#define BG96_STATUS_TIME_MS 5000L

/**
 * @brief The loggerModem::_resetLevel.
 *
 * BG96 is reset with a 150-460ms low pulse on the `RESET_N` pin
 */
#define BG96_RESET_LEVEL LOW
/**
 * @brief The loggerModem::_resetPulse_ms.
 * @copydetails #BG96_RESET_LEVEL
 */
#define BG96_RESET_PULSE_MS 300


/**
 * @brief The loggerModem::_wakeLevel.
 *
 * Module is switched on by a >100 millisecond `LOW` pulse on the `PWRKEY` pin.
 * Module is switched on by a >650 millisecond `LOW` pulse on the `PWRKEY` pin.
 * Using something between those times for wake and using AT commands for sleep,
 * we should keep in the proper state.
 *
 * @note If at all possible, the BG96 status pin should be monitored to confirm
 * active status.
 */
#define BG96_WAKE_LEVEL LOW
/**
 * @brief The loggerModem::_wakePulse_ms.
 * @copydetails #BG96_WAKE_LEVEL
 */
#define BG96_WAKE_PULSE_MS 110

/**
 * @brief The loggerModem::_wakeDelayTime_ms.
 *
 * Time after `VBAT` is stable before `PWRKEY` can be used on BG96 is >30ms
 */
#define BG96_WAKE_DELAY_MS 100
/**
 * @brief The loggerModem::_max_at_response_time_ms.
 *
 * The BG96 has USB active at >4.2 sec, status at >4.8 sec, URAT at >4.9
 */
#define BG96_AT_RESPONSE_TIME_MS 10000L

/**
 * @brief The loggerModem::_disconnectTime_ms.
 *
 * Documentation for the BG96 says to allow >2s for clean shutdown.
 */
#define BG96_DISCONNECT_TIME_MS 5000L
/**@}*/

/**
 * @brief The loggerModem subclass for Dragino, Nimbelink, or any other module
 * based on the [Quectel BG96](@ref modem_bg96).
 */
class QuectelBG96
    : public loggerModemImpl<TinyGsmBG96,                 // Modem Type
                             TinyGsmBG96::GsmClientBG96,  // TCP Client Type
                             TinyGsmBG96::GsmClientSecureBG96  // SSL Client
                                                               // Type
                             > {
 public:
    /**
     * @brief Construct a new Quectel BG96 object
     *
     * The constructor initializes all of the provided member variables,
     * constructs a loggerModem parent class with the appropriate timing for the
     * module, calls the constructor for a TinyGSM modem on the provided
     * modemStream, and creates a TinyGSM Client linked to the modem.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin @copydoc loggerModem::_powerPin
     * @param statusPin @copydoc loggerModem::_statusPin
     * This is the pin labeled `STATUS` in Quectel's integration guide.
     * @param modemResetPin @copydoc loggerModem::_modemResetPin
     * This is the pin labeled `RESET_N` in Quectel's integration guide.
     * @param modemSleepRqPin @copydoc loggerModem::_modemSleepRqPin
     * This is the pin labeled `PWRKEY` in Quectel's integration guide.
     * @param apn The Access Point Name (APN) for the SIM card.
     *
     * @see loggerModem::loggerModem
     */
    QuectelBG96(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                int8_t modemResetPin, int8_t modemSleepRqPin, const char* apn);
    /**
     * @brief Destroy the Quectel BG96 object - no action taken
     */
    ~QuectelBG96() override = default;

    // Hard reset has device-specific implementation
    bool modemHardReset() override;

 protected:
    // Implementation of the pure virtual functions from loggerModemImpl
    bool modemWakeFxn() override;
    bool modemSleepFxn() override;

    // Only override connectWithCredentials to provide APN
    bool connectWithCredentials() override;

 private:
    const char* _apn;  ///< Internal reference to the cellular APN
};
/**@}*/
#endif  // SRC_MODEMS_QUECTELBG96_H_

// cSpell:ignore VBAT URAT
