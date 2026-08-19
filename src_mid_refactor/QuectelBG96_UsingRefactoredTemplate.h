/**
 * @file QuectelBG96_UsingRefactoredTemplate.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Example of QuectelBG96 using the refactored template-based
 * architecture.
 *
 * This file demonstrates how child modem classes would be implemented with
 * the refactored loggerModem that uses CRTP mixins.
 *
 * KEY POINTS:
 * - The child class only needs to inherit from the ONE template: loggerModem
 * - All mixin functionality is automatically inherited
 * - Child classes only override modem-specific virtual functions
 * - All power, communication, and sensing functions are available automatically
 * - No changes needed to existing usage code (LoggerBase.cpp, tests, user
 * sketches)
 *
 * @note This is an EXAMPLE. Do NOT attempt to compile this as-is.
 */

// Header Guards
#ifndef SRC_MODEMS_QUECTELBG96_REFACTORED_H_
#define SRC_MODEMS_QUECTELBG96_REFACTORED_H_

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

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "TinyGsmClientBG96.h"
#include "LoggerModemTemplated.h"  // The refactored template with CRTP mixins

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
#define BG96_STATUS_LEVEL HIGH
#define BG96_STATUS_TIME_MS 5000L
#define BG96_RESET_LEVEL LOW
#define BG96_RESET_PULSE_MS 300
#define BG96_WAKE_LEVEL LOW
#define BG96_WAKE_PULSE_MS 110
#define BG96_WAKE_DELAY_MS 100
#define BG96_AT_RESPONSE_TIME_MS 10000L
#define BG96_DISCONNECT_TIME_MS 5000L
/**@}*/

/**
 * @brief The logger modem subclass for Quectel BG96 modules.
 *
 * This class demonstrates how the refactored template-based approach works:
 *
 * ADVANTAGES OVER MACRO-BASED APPROACH:
 * - No need to include LoggerModemMacros.h
 * - No MS_MODEM_* macro calls needed
 * - All functionality inherited from template via CRTP mixins
 * - Only override what's unique to this modem
 * - Clean, understandable code
 *
 * ADVANTAGES OVER NON-TEMPLATE APPROACH:
 * - Smaller binary size (no duplicate implementations)
 * - Better type safety
 * - Compile-time modem capability selection
 *
 * The template parameters specify:
 * - GsmModemType: TinyGsmBG96 (the low-level driver)
 * - ClientType: TinyGsmBG96::GsmClientBG96 (standard TCP client)
 * - SecureClientType: TinyGsmBG96::GsmClientSecureBG96 (SSL/TLS client)
 * - signalQualityIsRSSI: false (BG96 returns CSQ, not RSSI)
 */
class QuectelBG96
    : public loggerModem<TinyGsmBG96,                       // Modem Type
                         TinyGsmBG96::GsmClientBG96,        // TCP Client Type
                         TinyGsmBG96::GsmClientSecureBG96,  // SSL Client
                                                            // Type
                         false>  // Signal quality is CSQ, not RSSI
{
 public:
    /**
     * @brief Construct a new Quectel BG96 object
     *
     * The constructor initializes all of the provided member variables,
     * constructs a parent loggerModem template with the appropriate timing,
     * and creates a TinyGSM modem on the provided modemStream.
     *
     * @param modemStream The Arduino stream instance for serial communication.
     * @param powerPin MCU pin controlling modem power (active HIGH)
     * @param statusPin MCU pin connected to modem status output (STATUS pin)
     * @param modemResetPin MCU pin connected to modem reset pin (RESET_N pin)
     * @param modemSleepRqPin MCU pin for modem sleep/wake request (PWRKEY pin)
     * @param apn The Access Point Name (APN) for the cellular network
     */
    QuectelBG96(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                int8_t modemResetPin, int8_t modemSleepRqPin, const char* apn)
        : loggerModem(modemStream,          // Stream for modem communication
                      powerPin,             // _powerPin
                      statusPin,            // _statusPin
                      BG96_STATUS_LEVEL,    // _statusLevel
                      modemResetPin,        // _modemResetPin
                      BG96_RESET_LEVEL,     // _resetLevel
                      BG96_RESET_PULSE_MS,  // _resetPulse_ms
                      modemSleepRqPin,      // _modemSleepRqPin
                      BG96_WAKE_LEVEL,      // _wakeLevel
                      BG96_WAKE_PULSE_MS,   // _wakePulse_ms
                      BG96_STATUS_TIME_MS,  // _statusTime_ms
                      BG96_DISCONNECT_TIME_MS,    // _disconnectTime_ms
                      BG96_WAKE_DELAY_MS,         // _wakeDelayTime_ms
                      BG96_AT_RESPONSE_TIME_MS),  // _max_at_response_time_ms
          _apn(apn) {
        _modemName = "Quectel BG96";
    }

    /**
     * @brief Destroy the Quectel BG96 object
     */
    ~QuectelBG96() override = default;

    /**
     * @brief Perform a hard reset of the modem using the BG96-specific
     * procedure.
     *
     * This overrides the default modemHardReset() from loggerModemPowerMixin
     * if the BG96 requires special handling (though in this case it doesn't).
     *
     * @return True if reset succeeded
     */
    bool modemHardReset() override {
        MS_DBG(F("Performing BG96-specific hard reset"));
        // In this case, the default implementation is fine, so just call
        // parent:
        return loggerModem::modemHardReset();
    }

 protected:
    /**
     * @brief Perform modem-specific setup after wake.
     *
     * This is called by modemSetup() after the modem wakes up and is
     * ready to receive AT commands. Use this to:
     * - Query modem identity (model, firmware version, etc.)
     * - Configure modem-specific settings
     * - Test basic communication
     *
     * @return True if extra setup completed successfully
     */
    bool extraModemSetup() override {
        MS_DBG(F("Running BG96 extra setup"));

        bool success = true;

        // Initialize TinyGSM modem
        if (!gsmModem.init()) {
            MS_DBG(F("TinyGSM init failed"));
            return false;
        }

        // Get modem model and version information
        _modemName = gsmModem.getModemModel();
        _modemHwVersion =
            gsmModem
                .getModemInfo();  // Simplified; real implementation queries GSN
        _modemFwVersion    = gsmModem.getModemInfo();  // Simplified
        _modemSerialNumber = gsmModem.getModemSerialNumber();

        MS_DBG(F("BG96 setup complete:"), getModemName());

        return success;
    }

    /**
     * @brief Check if the modem can reach the internet.
     *
     * For cellular modems, this checks if GPRS/LTE is connected.
     *
     * @return True if internet connection is available
     */
    bool isInternetAvailable() override {
        // BG96-specific: check if GPRS is connected
        return gsmModem.isGprsConnected();
    }

    /**
     * @brief Connect to the cellular network with the provided APN.
     *
     * This is called by connectInternet() from loggerModemCommMixin
     * after the modem wakes up and registers to the network.
     *
     * This MUST be overridden to provide network-specific credentials (APN).
     */
    bool connectWithCredentials() override {
        MS_DBG(F("Connecting BG96 to network with APN: "), _apn);

        // BG96-specific: connect to GPRS with APN
        bool success = gsmModem.gprsConnect(_apn);
        return success;
    }

    /**
     * @brief Perform the BG96-specific wake sequence.
     *
     * This is called by modemWake() from loggerModemPowerMixin to perform
     * any modem-specific wake-up procedures after the power has been
     * established and pin signals have been sent.
     *
     * @return True if wake succeeded
     */
    bool modemWakeFxn() override {
        MS_DBG(F("Running BG96-specific wake function"));

        // Perform TinyGSM wake if needed (e.g., re-init after sleep)
        return gsmModem.init();
    }

    /**
     * @brief Perform the BG96-specific sleep sequence.
     *
     * This is called by modemSleep() from loggerModemPowerMixin to perform
     * any modem-specific sleep procedures (e.g., graceful shutdown, disable
     * RF, etc.) before the PWRKEY pin is toggled.
     *
     * @return True if sleep succeeded
     */
    bool modemSleepFxn() override {
        MS_DBG(F("Running BG96-specific sleep function"));

        // Tell the modem to enter deep sleep mode
        // This is AT+QSCLK=1 for BG96

        return true;
    }

 private:
    /**
     * @brief The Access Point Name (APN) for the cellular network.
     *
     * This is stored here and used in connectWithCredentials().
     * Examples: "emnify", "hologram", "verizon.com", etc.
     */
    const char* _apn;
};
/**@}*/

#endif  // SRC_MODEMS_QUECTELBG96_REFACTORED_H_


// ============================================================================
// USAGE EXAMPLES (showing that existing code doesn't change)
// ============================================================================
//
// Example 1: Internal use in LoggerBase.cpp (UNCHANGED)
// ---------
// void Logger::attachModem(loggerModem& modem) {
//     _logModem = &modem;
//     if (_logModem->modemWake()) {  // Calls power mixin's modemWake()
//         if (_logModem->connectInternet()) {  // Calls comm mixin's
//         connectInternet()
//             // ... rest of code
//         }
//     }
// }
//
// NOTES:
// - loggerModem is still the base type (defined in LoggerModem.h as a typedef)
// - The actual modem type (QuectelBG96) is unknown at compile time
// - Virtual dispatch works through loggerModem's virtual functions
// - All mixin methods are accessible through the virtual interface
//
//
// Example 2: User-facing concrete instance (UNCHANGED)
// ---------
// void setup() {
//     Serial.begin(9600);
//
//     // Create a BG96 modem instance
//     QuectelBG96 modem(&modemSerial, 18, -1, A5, -1, "emnify");
//
//     // Set up the modem (calls modemSetup() from template)
//     modem.modemSetup();
//
//     // Wake it (calls modemWake() from power mixin)
//     if (modem.modemWake()) {
//         // Connect (calls connectInternet() from comm mixin)
//         if (modem.connectInternet(120000)) {
//             // Get signal quality (calls getModemRSSI() from sensing mixin)
//             float rssi = QuectelBG96::getModemRSSI();
//             Serial.print("RSSI: ");
//             Serial.println(rssi);
//
//             // Disconnect
//             modem.disconnectInternet();
//         }
//     }
// }
//
// NOTES:
// - All mixin methods are available on concrete instances
// - Virtual functions are overridden by this class
// - Power mixin methods: modemWake(), modemPowerUp(), modemHardReset(), etc.
// - Comm mixin methods: connectInternet(), disconnectInternet(),
// createClient(), etc.
// - Sensing mixin methods: getModemRSSI(), getModemBatteryState(), etc.
//
//
// Example 3: Developer creating a new modem (slightly different from before)
// ---------
// class MyNewModem : public loggerModem<TinyGsmNewType,
// TinyGsmNewType::GsmClient> {
//  public:
//     MyNewModem(Stream* stream, ...);
//  protected:
//     bool modemWakeFxn() override { /* ... */ }
//     bool modemSleepFxn() override { /* ... */ }
//     void connectWithCredentials() override { /* ... */ }
//     bool isInternetAvailable() override { /* ... */ }
// };
//
// CHANGES FROM BEFORE:
// - No need to include LoggerModemMacros.h
// - No MS_MODEM_* macro calls
// - Fewer files to understand/modify
// - All virtual functions still work the same way
// - All inherited methods from mixins available automatically
//
// ============================================================================
