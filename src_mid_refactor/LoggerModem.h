/**
 * @file LoggerModem.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * The original header emits member-function definitions with macros. This
 * copy puts those implementations in a class template so they are ordinary
 * C++ functions selected by TinyGsmCapabilities at compile time.
 */

// Header Guards
#ifndef SRC_LOGGERMODEM_H_
#define SRC_LOGGERMODEM_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEM_DEBUG
#define MS_DEBUGGING_STD "LoggerModem"
#endif
#ifdef MS_LOGGERMODEM_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModem"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "ClockSupport.h"
#include "VariableBase.h"

#include <Arduino.h>
#include <Client.h>
// Include the TinyGSM capabilities file to check what the modem can do
#include <TinyGsmCapabilities.h>


/**
 * @defgroup modem_measured_variables Modem Variables
 *
 * Variable objects to be tied to a loggerModem.  These are measured by a modem,
 * but are implemented as calculated variables.
 *
 * @note  The modem is NOT set up as a sensor.  ALL of these variables for the
 * modem object are actually being called as calculated variables where the
 * calculation function is to ask the modem object for the values from the last
 * time it connected to the internet.
 *
 * @ingroup the_modems
 */

/** @ingroup modem_measured_variables */
/**@{*/
/**
 * @anchor modem_rssi
 * @name Modem RSSI
 * The RSSI (received signal strength indication) variable from a modem-like
 * device.
 *
 * {{ @ref Modem_RSSI::Modem_RSSI }}
 */
/**@{*/
/**
 * @brief Decimal places in string representation; RSSI should have 0.
 *
 * RSSI is a rough calculation, so it has 0 decimal place resolution
 */
#define MODEM_RSSI_RESOLUTION 0
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable RSSI
/// polling.
#define MODEM_RSSI_ENABLE_BITMASK 0b00000001
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "RSSI"
#define MODEM_RSSI_VAR_NAME "RSSI"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "RSSI"
#define MODEM_RSSI_UNIT_NAME "RSSI"
/// @brief Default variable short code; "decibelMiliWatt"
#define MODEM_RSSI_DEFAULT_CODE "decibelMiliWatt"
/**@}*/

/**
 * @anchor modem_signal_pct
 * @name Modem Percent Full Signal
 * The percent full signal variable from a modem-like device.
 *
 * {{ @ref Modem_SignalPercent::Modem_SignalPercent }}
 */
/**@{*/
/**
 * @brief Decimal places in string representation; percent signal should have
 * 0.
 *
 * Percent signal is a rough calculation, so it has 0 decimal place resolution
 */
#define MODEM_PERCENT_SIGNAL_RESOLUTION 0
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable percent
/// signal polling.
#define MODEM_PERCENT_SIGNAL_ENABLE_BITMASK 0b00000010
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "signalPercent"
#define MODEM_PERCENT_SIGNAL_VAR_NAME "signalPercent"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent"
#define MODEM_PERCENT_SIGNAL_UNIT_NAME "percent"
/// @brief Default variable short code; "signalPercent"
#define MODEM_PERCENT_SIGNAL_DEFAULT_CODE "signalPercent"
/**@}*/

/**
 * @anchor modem_battery_state
 * @name Modem Battery Charge State
 * The battery charge state variable from a modem-like device.  This is used to
 * indicate whether or not the modem battery is currently being charged.
 *
 * @warning Whether this value is valid depends on both the type of modem you
 * are using and the individual break out of it.  **In many cases, this value
 * is not valid and should be ignored.**
 *
 * {{ @ref Modem_BatteryState::Modem_BatteryState }}
 */
/**@{*/
/**
 * @brief Decimal places in string representation; battery state should have 0.
 *
 * Battery state is a code value; it has 0 decimal place resolution
 */
#define MODEM_BATTERY_STATE_RESOLUTION 0
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable modem
/// battery charging state polling.
#define MODEM_BATTERY_STATE_ENABLE_BITMASK 0b00000100
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "batteryChargeState"
#define MODEM_BATTERY_STATE_VAR_NAME "batteryChargeState"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "number"
/// (dimensionless)
#define MODEM_BATTERY_STATE_UNIT_NAME "number"
/// @brief Default variable short code; "modemBatteryCS"
#define MODEM_BATTERY_STATE_DEFAULT_CODE "modemBatteryCS"
/**@}*/

/**
 * @anchor modem_battery_percent
 * @name Modem Battery Charge Percent
 * The percent battery charge from a modem-like device.
 *
 * @warning Whether this value is valid depends on both the type of modem you
 * are using and the individual break out of it.  **In many cases, this value
 * is not valid and should be ignored.**
 *
 * {{ @ref Modem_BatteryPercent::Modem_BatteryPercent }}
 */
/**@{*/
/// @brief Decimal places in string representation; battery charge percent
/// should have 0.
#define MODEM_BATTERY_PERCENT_RESOLUTION 0
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable modem
/// battery percent polling.
#define MODEM_BATTERY_PERCENT_ENABLE_BITMASK 0b00001000
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "batteryVoltage"
#define MODEM_BATTERY_PERCENT_VAR_NAME "batteryVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent"
#define MODEM_BATTERY_PERCENT_UNIT_NAME "percent"
/// @brief Default variable short code; "modemBatteryPct"
#define MODEM_BATTERY_PERCENT_DEFAULT_CODE "modemBatteryPct"
/**@}*/

/**
 * @anchor modem_battery_voltage
 * @name Modem Battery Voltage
 * The battery voltage from a modem-like device.
 *
 * @warning Whether this value is valid depends on both the type of modem you
 * are using and the individual break out of it.  **In many cases, this value
 * is not valid and should be ignored.**
 *
 * {{ @ref Modem_BatteryVoltage::Modem_BatteryVoltage }}
 */
/**@{*/
/**
 * @brief Decimal places in string representation; battery voltage should have
 * 0.
 *
 * No supported module has higher than 1mV resolution in battery reading.
 */
#define MODEM_BATTERY_VOLTAGE_RESOLUTION 0
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable modem
/// battery voltage polling.
#define MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK 0b00010000
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "batteryVoltage"
#define MODEM_BATTERY_VOLTAGE_VAR_NAME "batteryVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "millivolt"
#define MODEM_BATTERY_VOLTAGE_UNIT_NAME "millivolt"
/// @brief Default variable short code; "modemBatterymV"
#define MODEM_BATTERY_VOLTAGE_DEFAULT_CODE "modemBatterymV"
/**@}*/

/**
 * @anchor modem_temperature
 * @name Modem Chip Temperature
 * The chip temperature from a modem-like device.
 *
 * @warning This is *NOT* representative of environmental temperature and should
 * only be used to verify that the module is not overheating.
 *
 * {{ @ref Modem_Temp::Modem_Temp }}
 */
/**@{*/
/**
 * @brief Decimal places in string representation; temperature should
 * have 1.
 *
 * Most modules that can measure temperature measure to 0.1°C
 */
#define MODEM_TEMPERATURE_RESOLUTION 1
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable modem
/// temperature polling.
#define MODEM_TEMPERATURE_ENABLE_BITMASK 0b00100000
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define MODEM_TEMPERATURE_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define MODEM_TEMPERATURE_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "modemTemp"
#define MODEM_TEMPERATURE_DEFAULT_CODE "modemTemp"
/**@}*/

#ifdef MS_CHECK_MODEM_TIMING
/**
 * @anchor modem_activation
 * @name Modem Active Time
 * The active time from a modem-like device.
 *
 * @note This is only a testing/development diagnostic.
 *
 * {{ @ref Modem_ActivationDuration::Modem_ActivationDuration }}
 */
/**@{*/
/// @brief Decimal places in string representation; total active time should
/// have 3.
#define MODEM_ACTIVATION_RESOLUTION 3
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable modem
/// activation time polling.
#define MODEM_ACTIVATION_ENABLE_BITMASK 0b01000000
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "timeElapsed"
#define MODEM_ACTIVATION_VAR_NAME "timeElapsed"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "second"
#define MODEM_ACTIVATION_UNIT_NAME "second"
/// @brief Default variable short code; "modemActiveSec"
#define MODEM_ACTIVATION_DEFAULT_CODE "modemActiveSec"
/**@}*/

/**
 * @anchor modem_power
 * @name Modem Power Time
 * The total powered time from a modem-like device.
 *
 * @note This is only a testing/development diagnostic.
 *
 * {{ @ref Modem_PoweredDuration::Modem_PoweredDuration }}
 */
/**@{*/
/// @brief Decimal places in string representation; total powered time should
/// have 3.
#define MODEM_POWERED_RESOLUTION 3
/// @brief The bit mask for loggerModem::_pollModemMetaData to enable modem
/// power time polling
#define MODEM_POWERED_ENABLE_BITMASK 0b10000000
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "timeElapsed"
#define MODEM_POWERED_VAR_NAME "timeElapsed"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "second"
#define MODEM_POWERED_UNIT_NAME "second"
/// @brief Default variable short code; "modemPoweredSec"
#define MODEM_POWERED_DEFAULT_CODE "modemPoweredSec"
/**@}*/
#endif
/**@}*/


/**
 * @brief The loggerModem class provides an internet connection for the
 * logger and supplies an Arduino Client instance to use to publish data.
 *
 * A modem is a device that can be controlled by a logger to send out data
 * directly to the world wide web.
 *
 * The loggerModem class wraps the TinyGSM library and adds in the power
 * functions to turn the modem on and off and some error checking.
 *
 * TinyGSM is available here:  https://github.com/vshymanskyy/TinyGSM
 *
 * @ingroup base_classes
 * @tparam GsmModemType Concrete TinyGSM modem type.
 * @tparam ClientType Concrete unencrypted TinyGSM client type.
 * @tparam SecureClientType Concrete secure client type, or void without SSL.
 * @tparam signalQualityIsRSSI True when getSignalQuality() returns RSSI.
 */
template <typename GsmModemType, typename ClientType,
          typename SecureClientType = void, bool signalQualityIsRSSI = false>
class loggerModem {
 protected:
#ifdef MS_LOGGERMODEM_DEBUG_DEEP
    StreamDebugger _modemATDebugger;
#endif
    GsmModemType gsmModem;

 public:
    /**
     * @brief Construct a new loggerModem object.
     *
     * @param powerPin @copybrief loggerModem::_powerPin
     * @param statusPin @copybrief loggerModem::_statusPin
     * @param statusLevel @copybrief loggerModem::_statusLevel
     * @param modemResetPin @copybrief loggerModem::_modemResetPin
     * @param resetLevel @copybrief loggerModem::_resetLevel
     * @param resetPulse_ms @copybrief loggerModem::_resetPulse_ms
     * @param modemSleepRqPin @copybrief loggerModem::_modemSleepRqPin
     * @param wakeLevel @copybrief loggerModem::_wakeLevel
     * @param wakePulse_ms @copybrief loggerModem::_wakePulse_ms
     * @param max_status_time_ms @copybrief loggerModem::_statusTime_ms
     * @param max_disconnectTime_ms @copybrief loggerModem::_disconnectTime_ms
     * @param wakeDelayTime_ms @copybrief loggerModem::_wakeDelayTime_ms
     * @param max_at_response_time_ms @copybrief #_max_at_response_time_ms
     *
     * @see @ref modem_ctor_variables
     */
    loggerModem(Stream* modemStream, int8_t powerPin, int8_t statusPin,
                bool statusLevel, int8_t modemResetPin, bool resetLevel,
                uint32_t resetPulse_ms, int8_t modemSleepRqPin, bool wakeLevel,
                uint32_t wakePulse_ms, uint32_t max_status_time_ms,
                uint32_t max_disconnectTime_ms, uint32_t wakeDelayTime_ms,
                uint32_t max_at_response_time_ms)
        :
#ifdef MS_LOGGERMODEM_DEBUG_DEEP
          _modemATDebugger(*modemStream, MS_SERIAL_OUTPUT),
          gsmModem(_modemATDebugger)
#else
          gsmModem(*modemStream),
#endif
              _powerPin(powerPin),
          _statusPin(statusPin),
          _statusLevel(statusLevel),
          _modemResetPin(modemResetPin),
          _resetLevel(resetLevel),
          _resetPulse_ms(resetPulse_ms),
          _modemSleepRqPin(modemSleepRqPin),
          _wakeLevel(wakeLevel),
          _wakePulse_ms(wakePulse_ms),
          _statusTime_ms(max_status_time_ms),
          _disconnectTime_ms(max_disconnectTime_ms),
          _wakeDelayTime_ms(wakeDelayTime_ms),
          _max_at_response_time_ms(max_at_response_time_ms),
          _modemName("unspecified modem") {
    }

    /**
     * @brief Destroy the logger modem object - no action taken.
     */
    virtual ~loggerModem() = default;

 public:
    /**
     * @brief Set an LED to turn on (pin will be `HIGH`) when the modem is on.
     *
     * @param modemLEDPin The digital pin number for the LED
     */
    void setModemLED(int8_t modemLEDPin) {
        _modemLEDPin = modemLEDPin;
        if (_modemLEDPin >= 0) {
            pinMode(_modemLEDPin, OUTPUT);
            digitalWrite(_modemLEDPin, LOW);
        }
    }
    /* ===================================================================== */
    /* Modem Identification Functions                                        */
    /* ===================================================================== */
    /**
     * @brief Get the internally stored modem name.
     *
     * This is set during the extraModemSetup() function and is not polled from
     * the modem.
     *
     * @return The modem name
     */
    String getModemName() {
        return _modemName;
    }

    /**
     * @brief Get a detailed printable description of the modem from the stored
     * modem information.
     *
     * @note These values are polled for and cached in memory till needed
     *
     * @return The concatenated name, hardware version, firmware
     * version, and serial number of the modem.
     *
     * @todo Implement this for modems other than the XBee WiFi
     */
    String getModemDevId() {
        return _modemName + F(" Sn ") + _modemSerialNumber + F(" HwVer ") +
            _modemHwVersion + F(" FwVer ") + _modemFwVersion;
    }

    /* ===================================================================== */
    /* Modem setup                                                           */
    /* ===================================================================== */
 public:
    /**
     * @brief Set up the modem before first use.
     *
     * This is used for operations that cannot happen in the modem constructor -
     * they must happen at run time, not at compile time.
     *
     * @return True if setup was successful
     */
    virtual bool modemSetup() {
        // NOTE:  Set flag FIRST to stop infinite loop between modemSetup() and
        // modemWake()
        bool success  = true;
        _hasBeenSetup = true;

        MS_DBG(F("Setting up the modem ..."));

        // Power up
        bool wasPowered = true;
        if (_millisPowerOn == 0) {
            modemPowerUp();
            wasPowered = false;
        }

        // Check if the modem was awake, wake it if not
        bool wasAwake = isModemAwake();
        if (!wasAwake) {
            while (millis() - _millisPowerOn < _wakeDelayTime_ms) {
                // wait
            }
            MS_DBG(F("Waking up the modem for setup ..."));
            success &= modemWake();
        } else {
            MS_DBG(F("Modem was already awake and should be ready for setup."));
        }

        if (success) {
            MS_DBG(F("Running modem's extra setup function ..."));
            success &= extraModemSetup();
            if (success) {
                MS_DBG(F("... setup complete!  It's a"), getModemName());
            } else {
                MS_DBG(F("... setup failed!  It's a"), getModemName());
                _hasBeenSetup = false;
            }
        } else {
            MS_DBG(F("... "), getModemName(),
                   F("did not wake up and cannot be set up!"));
        }

        MS_DBG(_modemName, F("warms up in"), _wakeDelayTime_ms,
               F("ms, indicates status in"), _statusTime_ms,
               F("ms, is responsive to AT commands in less than"),
               _max_at_response_time_ms, F("ms, and takes up to"),
               _disconnectTime_ms, F("ms to close connections and shut down."));

        // Put the modem back to sleep if it was woken up just for setup
        // Only go to sleep if it had been asleep and is now awake
        if (!wasPowered) {  // Run the sleep and power down functions
            MS_DBG(
                F("Because the modem was not powered prior to setup, putting it"
                  "back to sleep and powering it down now."));
            success &= modemSleepPowerDown();
        } else if (!wasAwake) {  // Run only the sleep function
            MS_DBG(F(
                "Because the modem was asleep prior to setup, putting it back "
                "to sleep now."));
            success &= modemSleep();
        } else {
            MS_DBG(F("Leaving modem on after setup ..."));
        }

        return success;
    }
    /**
     * @brief Retained for backwards compatibility; use modemSetup() in new
     * code.
     *
     * @m_deprecated_since{0,24,1}
     *
     * @return True if setup was successful
     */
    bool setup() {
        return modemSetup();
    }

    /* ===================================================================== */
    /* Power Functions                                                       */
    /* ===================================================================== */
    /**
     * @anchor modem_power_functions
     * @name Functions related to the modem power and activity state
     *
     * These are similar to the like-named Sensor functions.
     */
    /**@{*/
 public:
    /**
     * @brief Power the modem by setting the modem power pin high.
     */
    virtual void modemPowerUp() {
        if (_powerPin >= 0) {
            if (_modemSleepRqPin >= 0) {
                // For most modules, the sleep pin should be held high during
                // power up. After some warm-up time, that pin is usually pulsed
                // low to wake the module.
                MS_DBG(F("Setting sleep pin"), _modemSleepRqPin, F("to"),
                       !_wakeLevel ? F("HIGH") : F("LOW"),
                       F("while powering on"), getModemName());
                digitalWrite(_modemSleepRqPin, !_wakeLevel);
            }
            MS_DBG(F("Powering"), getModemName(), F("with pin"), _powerPin);
            pinMode(_powerPin, OUTPUT);
            digitalWrite(_powerPin, HIGH);
            // Mark the time that the sensor was powered
            _millisPowerOn = millis();
        } else {
            MS_DBG(F("Power to"), getModemName(),
                   F("is not controlled by this library."));
            // Mark the power-on time, just in case it had not been marked
            if (_millisPowerOn == 0) _millisPowerOn = millis();
        }
    }
    /**
     * @brief Cut power to the modem by setting the modem power pin low.
     *
     * @note modemPowerDown() simply kills power, while modemSleepPowerDown()
     * allows for graceful shut down.  You should use modemSleepPowerDown()
     * whenever possible.
     */
    virtual void modemPowerDown() {
        if (_powerPin >= 0) {
            MS_DBG(F("Turning off power to"), getModemName(), F("with pin"),
                   _powerPin);
            digitalWrite(_powerPin, LOW);
            // Unset the power-on time
            _millisPowerOn = 0;
        } else {
            MS_DBG(F("Power to"), getModemName(),
                   F("is not controlled by this library."));
        }
    }

    /**
     * @brief Wake up the modem.
     *
     * This sets pin modes, powers up the modem if necessary, sets time stamps,
     * runs the specific modem's wake function, tests for responsiveness to AT
     * commands, and then re-runs the TinyGSM init() if necessary.  If the modem
     * fails to respond, this attempts a "hard" pin reset if possible.
     *
     * @return True if the modem is responsive and ready for action.
     */
    virtual bool modemWake() {
        // Set-up pin modes.
        // Because the modem calls wake BEFORE the first setup, we must set the
        // pin modes in the wake function.
        setModemPinModes();

        // Power up
        if (_millisPowerOn == 0) modemPowerUp();
        // wait for warmup
        if (millis() - _millisPowerOn < _wakeDelayTime_ms) {
            MS_DBG(F("Wait"), _wakeDelayTime_ms - (millis() - _millisPowerOn),
                   F("ms longer for warm-up"));
            while (millis() - _millisPowerOn < _wakeDelayTime_ms) {
                yield();  // wait
            }
        }

        if (isModemAwake()) {
            MS_DBG(getModemName(),
                   F("was already on! Will not run wake function."));
        } else {  // Run the specific modemWakeFxn()
            MS_DBG(F("Running wake function for"), getModemName());
            if (!modemWakeFxn()) {
                MS_DBG(F("Wake function for"), getModemName(),
                       F("did not run as expected!"));
            }
        }

        uint8_t resets  = 0;
        bool    success = false;
        while (!success && resets < 2) {
            // Check that the modem is responding to AT commands.
            MS_START_DEBUG_TIMER;
            MS_DBG(F("\nWaiting up to"), _max_at_response_time_ms, F("ms for"),
                   getModemName(), F("to respond to AT commands..."));
            success = gsmModem.testAT(_max_at_response_time_ms + 500);
            if (success) {
                MS_DBG(F("... AT OK after"), MS_PRINT_DEBUG_TIMER,
                       F("milliseconds!"));
            } else {  // Hard reset is there's no AT response.
                MS_DBG(F("No response to AT commands!"));
                MS_DBG(F("Attempting a hard reset on the modem! "), resets + 1);
                if (!modemHardReset()) {  // Exit if we can't hard reset.
                    break;
                } else {
                    resets++;
                }
            }
        }

        // Clean any junk out of the modem buffer.
        gsmModem.streamClear();

        // Re-run the modem init, or setup if necessary.
        // This will turn off echo, which often turns itself back on after a
        // reset/power loss.
        // This also checks the SIM card state.
        if (!_hasBeenSetup) {
            // If we run setup, take success value entirely from that.
            success = modemSetup();
        } else {
            success &= gsmModem.init();
            syncNTP(
                typename TinyGsmCapabilities::has_ntp<GsmModemType>::type());
        }

        if (success) {
            modemLEDOn();
            MS_DBG(getModemName(), F("should be awake and ready to go."));
        } else {
            MS_DBG(getModemName(), F("failed to wake!"));
        }

        return success;
    }
    /**
     * @brief Retained for backwards compatibility; use modemWake() in new code.
     *
     * @m_deprecated_since{0,24,1}
     *
     * @return True if wake was successful, modem should be ready to
     * communicate
     */
    bool wake() {
        return modemWake();
    }

    /**
     * @brief Request that the modem enter its lowest possible power state.
     *
     * @return True if the modem has successfully entered low power
     * state
     */
    virtual bool modemSleep() {
        bool success = true;
        MS_DBG(F("Putting"), getModemName(), F("to sleep."));

        // If there's a status pin available, check before running the sleep
        // function NOTE:  It's possible that the modem could still be in the
        // process of turning on and thus status pin isn't valid yet.  In that
        // case, we wouldn't yet know it's coming on and so we'd mistakenly
        // assume it's already off and not turn it back off. This only applies
        // to modules with a pulse wake (i.e., non-zero wake time). For all
        // modules that do pulse on, where possible I've selected a pulse time
        // that is sufficient to wake but not quite long enough to put it to
        // sleep and am using AT commands to sleep.  This *should* keep
        // everything lined up.
        if (!isModemAwake()) {
            MS_DBG(getModemName(),
                   F("is already off!  Will not run sleep function."));
        } else {
            // Run the sleep function
            MS_DBG(F("Running given sleep function for"), getModemName());
            success &= modemSleepFxn();
            modemLEDOff();
        }
        return success;
    }
    /**
     * @brief Request that the modem enter its lowest possible power state and
     * then set the power pin low after the modem has indicated it has
     * successfully gone to low power.
     *
     * This allows the modem to shut down all connections cleanly and do any
     * necessary internal housekeeping before stopping power.
     *
     * @return True if the modem has successfully entered low power
     * state _and_ then powered off
     */
    virtual bool modemSleepPowerDown() {
        bool     success = true;
        uint32_t start   = millis();
        MS_DBG(F("Turning"), getModemName(), F("off."));

        modemSleep();

        // Now power down
        if (_powerPin >= 0) {
            // If there's a status pin available, wait until modem shows it's
            // ready to be powered off This allows the modem to shut down
            // gracefully.
            if (_statusPin >= 0) {
                MS_DBG(
                    F("Waiting up to"), _disconnectTime_ms,
                    F("milliseconds for graceful shutdown as indicated by pin"),
                    _statusPin, F("going"),
                    !_statusLevel ? F("HIGH") : F("LOW"), F("..."));
                while (millis() - start < _disconnectTime_ms &&
                       digitalRead(_statusPin) ==
                           static_cast<int>(_statusLevel)) {  // wait
                }
                if (digitalRead(_statusPin) == static_cast<int>(_statusLevel)) {
                    MS_DBG(F("... "), getModemName(),
                           F("did not successfully shut down!"));
                } else {
                    MS_DBG(F("... shutdown complete after"), millis() - start,
                           F("ms."));
                }
            } else if (_disconnectTime_ms > 0) {
                MS_DBG(F("Waiting"), _disconnectTime_ms,
                       F("ms for graceful shutdown."));
                while (millis() - start < _disconnectTime_ms) {
                    // wait
                }
            }

            MS_DBG(F("Turning off power to"), getModemName(), F("with pin"),
                   _powerPin);
            digitalWrite(_powerPin, LOW);
            // Unset the power-on time
            _millisPowerOn = 0;
        } else {
            // If we're not going to power the modem down, there's no reason to
            // hold up the main processor while waiting for the modem to shut
            // down. It can just do its thing unwatched while the main processor
            // sleeps.
            MS_DBG(F("Power to"), getModemName(),
                   F("is not controlled by this library - not waiting for "
                     "shut-down to complete."));
        }

        return success;
    }
    /**@}*/


    /* ===================================================================== */
    /* Pin Functions                                                         */
    /* ===================================================================== */
    /**
     * @anchor modem_pin_functions
     * @name Pin setting functions
     * Functions to set or re-set the pin numbers for the connection between
     * the modem module and the logger MCU.
     */
    /**@{*/
 public:
    /**
     * @brief Set the pin level to be expected when the on the modem status pin
     * when the modem is active.
     *
     * If this function is not called, the modem status pin is assumed to
     * exactly follow the hardware specifications for that modems raw cellular
     * component.
     *
     * @param level The active level of the pin (`LOW` or `HIGH`)
     */
    void setModemStatusLevel(bool level) {
        _statusLevel = level;
    }

    /**
     * @brief Set the pin level to be used to wake the modem.
     *
     * If this function is not called, the modem status pin is assumed to
     * exactly follow the hardware specifications for that modems raw cellular
     * component.
     *
     * @param level The pin level (`LOW` or `HIGH`) of the pin while waking
     * the modem.
     */
    void setModemWakeLevel(bool level) {
        _wakeLevel = level;
    }

    /**
     * @brief Set the pin level to be used to reset the modem.
     *
     * If this function is not called, the modem status pin is assumed to
     * exactly follow the hardware specifications for that modems raw cellular
     * component - nearly always low.
     *
     * @param level The pin level (`LOW` or `HIGH`) of the pin while
     * resetting the modem.
     */
    void setModemResetLevel(bool level) {
        _resetLevel = level;
    }

    /**
     * @brief Use the modem reset pin specified in the constructor to perform a
     * "hard" or "panic" reset.
     *
     * This should only be used if the modem is clearly non-responsive.
     *
     * @return True if the reset succeeded and the modem should now be
     * responsive.  False if the modem remains non-responsive either because the
     * reset failed to fix the communication issue or because a reset is not
     * possible with the current pin/modem configuration.
     */
    virtual bool modemHardReset() {
        if (_modemResetPin >= 0) {
            MS_DBG(F("Doing a hard reset on the modem by setting pin"),
                   _modemResetPin, _resetLevel ? F("HIGH") : F("LOW"), F("for"),
                   _resetPulse_ms, F("ms"));
            digitalWrite(_modemResetPin, _resetLevel);
            delay(_resetPulse_ms);
            digitalWrite(_modemResetPin, !_resetLevel);
            return true;
        } else {
            MS_DBG(F("No pin has been provided to reset the modem!"));
            return false;
        }
    }
    /**@}*/


    /* ===================================================================== */
    /* Internet Functions                                                    */
    /* ===================================================================== */
    /**
     * @anchor modem_internet_functions
     * @name Functions for internet connectivity
     */
    /**@{*/
 public:
    /**
     * @brief Wait for the modem to successfully connect to the internet -
     * either WiFi or cellular data.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data connection.  Defaults to 50,000ms
     * (50s).
     * @return True if data connection has been established.  False if the modem
     * was unresponsive, unable to register with the network, or unable to
     * establish an internet connection.
     */
    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) {
        return connectInternet(
            maxConnectionTime,
            typename TinyGsmCapabilities::has_gprs<GsmModemType>::type());
    }
 protected:
    /**
     * @brief Common preparation for connecting to the internet
     * @return True if the modem successfully woke and is ready to connect to
     * the internet; false otherwise
     */
    virtual bool prepareForInternet() {
        bool success = true;

        // Power up, if necessary
        bool wasPowered = true;
        if (_millisPowerOn == 0) {
            modemPowerUp();
            wasPowered = false;
        }

        // Check if the modem was awake, wake it if not
        bool wasAwake = isModemAwake();
        if (!wasAwake) {
            MS_DBG(F("Waiting for modem to boot after power on ..."));
            while (millis() - _millisPowerOn < _wakeDelayTime_ms) {  // wait
            }
            MS_DBG(F("Waking up the modem to connect to the internet ..."));
            success &= modemWake();
        } else {
            MS_DBG(F("Modem was already awake and should be ready."));
        }
        if (!wasPowered) {
            MS_DBG(F("Modem was powered to connect to the internet!  "
                     "Remember to turn it off when you're done."));
        } else if (!wasAwake) {
            MS_DBG(F("Modem was woken up to connect to the internet!   "
                     "Remember to put it to sleep when you're done."));
        }
        return success;
    }
    /**
     * @brief The connect to the internet function for modems that have cellular
     * (GPRS, EPS, etc) connectivity.
     *
     * Wait for the modem to successfully register on the cellular
     * network and then request that it establish either EPS or GPRS data
     * connection.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data connection.  Defaults to 50,000ms
     * (50s).
     * @return True if EPS or GPRS data connection has been
     * established.  False if the modem was unresponsive, unable to register
     * with the cellular network, or unable to establish a EPS or GPRS
     * connection.
     */
    bool connectInternet(uint32_t maxConnectionTime,
                         TinyGsmCapabilities::true_type) {
        bool success = prepareForInternet();
        if (!success) return false;

        MS_START_DEBUG_TIMER
        MS_DBG(F("\nWaiting up to"), maxConnectionTime / 1000,
               F("seconds for cellular network registration..."));
        if (gsmModem.waitForNetwork(maxConnectionTime)) {
            // for all cellular modems **except the XBee** we need to actively
            // connect to the APN using the gprsConnect function after we've
            // connected to the base cellular network.  The XBee stores the APN
            // in NVM and automatically inputs in in the network connection
            // process.  We *avoid* setting the APN unnecessarily on the XBee so
            // as not to wear out the limited write flash.
            if (strcmp(GsmModemType::ModemConfig::MODEM_MANUFACTURER, "XBee") ==
                0) {
                MS_DBG(F("... Registered after"), MS_PRINT_DEBUG_TIMER,
                       F("milliseconds.  Connecting to GPRS..."));
                connectWithCredentials();
            }
            MS_DBG(F("... Connected after"), MS_PRINT_DEBUG_TIMER,
                   F("milliseconds."));
            return true;
        } else {
            MS_DBG(F("...GPRS connection failed."));
            return false;
        }
    }
    /**
     * @brief The connect to the internet function for modems that have WiFi
     * connectivity and do not have cellular connectivity
     *
     * Wait for the modem see if the modem will reestablish a connection on its
     * own with previously saved connections and then send new credentials and
     * wait for a new connection if that fails.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data connection.  Defaults to 50,000ms
     * (50s).
     * @return True if WiFi data connection has been established.  False if the
     * modem was unresponsive or unable to connect to the network.
     */
    bool connectInternet(uint32_t maxConnectionTime,
                         TinyGsmCapabilities::false_type) {
        bool success = prepareForInternet();
        if (!success) return false;

        const uint32_t reconnectTime = autoReconnectTime();
        MS_START_DEBUG_TIMER
        MS_DBG(F("\nWaiting"), reconnectTime,
               F("ms to see if WiFi connects without sending new "
                 "credentials..."));
        if (!(gsmModem.isNetworkConnected())) {
            // If still not connected, send new credentials
            if (!(gsmModem.waitForNetwork(reconnectTime))) {
                MS_DBG(F("Sending credentials..."));
                for (uint8_t i = 0; i < 5; i++) {
                    if (connectWithCredentials()) { break; }
                }
                MS_DBG(F("Waiting up to"), maxConnectionTime / 1000,
                       F("seconds for connection"));
                if (!gsmModem.waitForNetwork(maxConnectionTime)) {
                    MS_DBG(F("... WiFi connection failed"));
                    return false;
                }
            }
        }
        MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds!"));
        return success;
    }

    /**
     * @brief Helper function to connect to the network after registration.
     *
     * For most cellular modems, this calls gprsConnect with the APN (ie,
     * `gprsConnect(_apn)`).
     * For WiFi modems, this calls networkConnect with the SSID and password
     * (ie, `networkConnect(_ssid, _password)`). For XBee modems, this does
     * nothing (as the credentials are set in flash during modem setup).
     *
     * Override this in derived classes that need custom network connection
     * logic.
     */
    virtual bool connectWithCredentials() {
        // Default: do nothing
        // Derived classes with _apn member should override to call:
        // gsmModem.gprsConnect(_apn, "", "");
        // Derived classes with _ssid and _password members should override to
        // call: return gsmModem.networkConnect(_ssid, _password);
        return true;
    }
    virtual uint32_t autoReconnectTime() const {
        return 10000L;
    }

 public:
    /**
     * @brief Disconnect from the internet
     */
    virtual void disconnectInternet() {
        MS_START_DEBUG_TIMER;
        disconnectInternet(
            typename TinyGsmCapabilities::has_gprs<GsmModemType>::type());
    }
 protected:
    /**
     * @brief The disconnect function for modems with cellular connectivity.
     *
     * Detach from EPS or GPRS data connection and then deregister from the
     * cellular network.
     */
    void disconnectInternet(TinyGsmCapabilities::true_type) {
        gsmModem.gprsDisconnect();
        MS_DBG(F("Disconnected from cellular network after"),
               MS_PRINT_DEBUG_TIMER, F("milliseconds."));
    }
    /**
     * @brief The disconnect function for modems without cellular (ie, with
     * WiFi) connectivity.
     */
    void disconnectInternet(TinyGsmCapabilities::false_type) {
        gsmModem.networkDisconnect();
        MS_DBG(F("Disconnected from WiFi network after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds."));
    }


 public:
    /**
     * @brief Check whether there is an active internet connection available.
     * @return True if there is an active data connection to the internet; false
     * otherwise
     */
    virtual bool isInternetAvailable() {
        return isInternetAvailable(
            typename TinyGsmCapabilities::has_gprs<GsmModemType>::type());
    }
 protected:
    /**
     * @brief The internet verification function for modems with cellular
     * connectivity.
     * @return True if there is an active data connection to the internet; false
     * otherwise
     */
    bool isInternetAvailable(TinyGsmCapabilities::true_type) {
        return gsmModem.isGprsConnected();
    }
    /**
     * @brief The internet verification function for modems without cellular
     * connectivity (ie, WiFi modems)
     * @return True if there is an active data connection to the internet; false
     * otherwise
     */
    bool isInternetAvailable(TinyGsmCapabilities::false_type) {
        return gsmModem.isNetworkConnected();
    }
    /**@}*/


    /* ===================================================================== */
    /* TCP Clients                                                           */
    /* ===================================================================== */
    /**
     * @anchor modem_client_functions
     * @name Functions for creating standard TCP clients
     */
    /**@{*/
 public:
    /**
     * @brief Create a new client object
     * @warning Be sure to delete this object when you're done with it!
     * @param mux Multiplexing channel to use, defaults to 0
     * @return A new client object
     */
    virtual ClientType* createClient(uint8_t mux = 0) {
        // Use the new keyword to create a new client on the **heap**
        return new ClientType(gsmModem, mux);
    }
    /**
     * @brief Attempts to delete a created TinyGsmClient object. We need to do
     * this to close memory leaks from the create client because we can't
     * delete the created client from a pointer to the parent because the
     * Arduino core's client class doesn't have a virtual destructor.
     *
     * Always match create/delete pairs:
     * - createClient() -> deleteClient()
     * - createSecureClient() -> deleteSecureClient()
     *
     * @param client The client to delete
     */
    virtual void deleteClient(ClientType* client) {
        delete static_cast<ClientType*>(client);
    }
    /**
     * @brief Delete a client from the modem's array of client pointers based on
     * the multiplex socket number.
     * @note This will work for both secure and unsecure clients.
     * @param mux Multiplexing channel to use.
     */
    void deleteClient(uint8_t mux = 0) {
        if (mux < GsmModemType::TcpConfig::kMuxCount &&
            gsmModem.sockets[mux] == nullptr) {
            delete (gsmModem.sockets[mux]);
            gsmModem.sockets[mux] = nullptr;
        }
    }
    /**@}*/


    /* ===================================================================== */
    /* Secure Clients                                                        */
    /* ===================================================================== */
    /**
     * @anchor modem_ssl_client_functions
     * @name Functions for creating secured (TLS/SSL) clients
     */
    /**@{*/
 public:
    /**
     * @brief Create a new secure client object
     * @warning Be sure to delete this object when you're done with it!
     * @param mux Multiplexing channel to use, defaults to 0
     * @return A new secure client object
     */
    virtual SecureClientType* createSecureClient(uint8_t mux = 0) {
        return createSecureClient(
            mux, typename TinyGsmCapabilities::has_ssl<GsmModemType>::type());
    }
 protected:
    /**
     * @brief The create secure client function for modems that support SSL
     * @param mux Multiplexing channel to use, defaults to 0
     * @return A new secure client object
     */
    SecureClientType* createSecureClient(uint8_t mux,
                                         TinyGsmCapabilities::true_type) {
        return new SecureClientType(gsmModem, mux);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL
     * @return A null pointer
     */
    SecureClientType* createSecureClient(uint8_t,
                                         TinyGsmCapabilities::false_type) {
        return nullptr;
    }

 public:
    /**
     * @brief Create a new secure client object with certificate specification
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param sslAuthMode The SSL authentication mode to use
     * @param sslVersion The SSL version to use
     * @param CAcertName The name of the CA certificate to use
     * @param clientCertName The name of the client certificate to use
     * @param clientKeyName The name of the client key to use
     *
     * @return A new secure client object
     */
    virtual SecureClientType* createSecureClient(
        SSLAuthMode sslAuthMode, SSLVersion sslVersion = SSLVersion::TLS1_2,
        const char* CAcertName = nullptr, const char* clientCertName = nullptr,
        const char* clientKeyName = nullptr) {
        return createSecureClient(
            sslAuthMode, sslVersion, CAcertName, clientCertName, clientKeyName,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
 protected:
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that can specify certificates
     * @copydetails loggerModem::createSecureClient(SSLAuthMode,
     * SSLVersion, const char*, const char*, const char*)
     */
    SecureClientType* createSecureClient(SSLAuthMode sslAuthMode,
                                         SSLVersion  sslVersion,
                                         const char* CAcertName,
                                         const char* clientCertName,
                                         const char* clientKeyName,
                                         TinyGsmCapabilities::true_type) {
        return new SecureClientType(gsmModem, sslAuthMode, sslVersion,
                                    CAcertName, clientCertName, clientKeyName);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL with certificate specification
     * @return A null pointer
     */
    SecureClientType* createSecureClient(SSLAuthMode, SSLVersion, const char*,
                                         const char*, const char*,
                                         TinyGsmCapabilities::false_type) {
        return nullptr;
    }

 public:
    /**
     * @brief Create a new secure client object with PSK credentials
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param pskIdent The pre-shared key identity
     * @param psKey The pre-shared key
     * @param sslVersion The SSL version to use
     *
     * @return A new secure client object
     */
    virtual SecureClientType* createSecureClient(
        const char* pskIdent, const char* psKey,
        SSLVersion sslVersion = SSLVersion::TLS1_2) {
        return createSecureClient(
            pskIdent, psKey, sslVersion,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
 protected:
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that can specify certificates
     * @copydetails loggerModem::createSecureClient(const char*,
     * const char*, SSLVersion)
     */
    SecureClientType* createSecureClient(const char* pskIdent,
                                         const char* psKey,
                                         SSLVersion  sslVersion,
                                         TinyGsmCapabilities::true_type) {
        return new SecureClientType(gsmModem, pskIdent, psKey, sslVersion);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL
     * @return A null pointer
     */
    SecureClientType* createSecureClient(const char*, const char*, SSLVersion,
                                         TinyGsmCapabilities::false_type) {
        return nullptr;
    }

 public:
    /**
     * @brief Create a new secure client object with PSK table name
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param pskTableName The pre-shared key table name - for modems that
     * require PSK's in a "table" format
     * @param sslVersion The SSL version to use
     *
     * @return A new secure client object
     */
    virtual SecureClientType* createSecureClient(
        const char* pskTableName, SSLVersion sslVersion = SSLVersion::TLS1_2) {
        return createSecureClient(
            pskTableName, sslVersion,
            typename TinyGsmCapabilities::can_specify_certs<
                GsmModemType>::type());
    }
 protected:
    /**
     * @brief The create secure client function for modems with SSL capabilities
     * that can specify certificates
     * @copydetails loggerModem::createSecureClient(const char*,
     * SSLVersion)
     */
    SecureClientType* createSecureClient(const char* pskTableName,
                                         SSLVersion  sslVersion,
                                         TinyGsmCapabilities::true_type) {
        return new SecureClientType(gsmModem, pskTableName, sslVersion);
    }
    /**
     * @brief The create secure client function for modems that do not support
     * SSL with certificate specification
     * @return A null pointer
     */
    SecureClientType* createSecureClient(const char*, SSLVersion,
                                         TinyGsmCapabilities::false_type) {
        return nullptr;
    }

 public:
    /**
     * @brief Attempts to delete a created TinyGsmSecureClient object. We need
     * to do this to close memory leaks from the create client because we can't
     * delete the created client from a pointer to the parent because the
     * Arduino core's client class doesn't have a virtual destructor.
     *
     * Always match create/delete pairs:
     * - createClient() -> deleteClient()
     * - createSecureClient() -> deleteSecureClient()
     *
     * @param client The client to delete
     */
    virtual void deleteSecureClient(SecureClientType* client) {
        deleteSecureClient(
            client,
            typename TinyGsmCapabilities::has_ssl<GsmModemType>::type());
    }
 protected:
    /**
     * @brief The delete secure client function for modems with SSL capabilities
     */
    void deleteSecureClient(SecureClientType* client,
                            TinyGsmCapabilities::true_type) {
        delete static_cast<SecureClientType*>(client);
    }
    /**
     * @brief The delete secure client function for modems that do not support
     * SSL
     */
    void deleteSecureClient(SecureClientType*,
                            TinyGsmCapabilities::false_type) {}
    /**@}*/


    /* ===================================================================== */
    /* NIST and Network Time Protocol (NTP) synchronization                  */
    /* ===================================================================== */
    /**
     * @anchor modem_nist_ntp_functions
     * @name Functions for NTP and NIST time synchronization
     */
    /**@{*/
 protected:
    /**
     * @brief A helper typedef to combine the two qualities needed to get NTP
     * time directly from the modem rather than from a NIST server.
     *
     * To get the time directly from the modem, the modem must support both NTP
     * __and__ Time functions.
     */
    typedef TinyGsmCapabilities::integral_constant<
        bool,
        TinyGsmCapabilities::has_ntp<GsmModemType>::value &&
            TinyGsmCapabilities::has_time<GsmModemType>::value>
        HasNTPAndTime;
    /**
     * @brief The syncNTP function for modems that have NTP capabilities.
     *
     * This needs to be called at wake because many modules forget their time
     * when they are powered down.  Without a proper time set, it is generally
     * not possible to make a SSL connection.
     */
    bool syncNTP(TinyGsmCapabilities::true_type) {
        return gsmModem.NTPServerSync("pool.ntp.org", _modemUTCOffset);
    }
    /**
     * @brief The syncNTP function for modems that do not have NTP capabilities.
     */
    bool syncNTP(TinyGsmCapabilities::false_type) {
        return false;
    }
 public:
    /**
     * @brief Set the timezone that the modem will attempt to sync itself to.
     *
     * This doesn't *have* to be the same as the RTC or logger timezone, but
     * you'd be stupid to make it different.
     *
     * @note This must be set for SSL connections to work! If the modem does not
     * have an accurate internal time when attempting an SSL connection, the
     * connection will fail because the certificates will not be within their
     * specified valid time ranges.
     *
     * @param timeZone The timezone that the modem will attempt to sync itself
     * to.
     */
    void setModemTimeZone(int8_t timeZone) {
        _modemUTCOffset = timeZone;
    }

 public:
    /**
     * @brief Get the time from NIST via NTP on those modems that support it or
     * using the NIST time protocol over TCP (RFC-868) for those that don't.
     *
     * @return Unix timestamp (seconds since Jan 1, 1970 UTC)
     */
    virtual uint32_t getNISTTime() {
        // Check for and bail if not connected to the internet.
        if (!isInternetAvailable()) {
            MS_DBG(F("No internet connection, cannot get network time."));
            return 0;
        }
        return getNISTTime(typename HasNTPAndTime::type());
    }
 protected:
    /**
     * @brief The get NIST time function for modems that have NTP and Time
     * capabilities.
     *
     * @return Unix timestamp (seconds since Jan 1, 1970 UTC)
     */
    uint32_t getNISTTime(TinyGsmCapabilities::true_type) {
        gsmModem.NTPServerSync("pool.ntp.org", 0);
        gsmModem.waitForTimeSync();

        int seconds = 0, minutes = 0, hours = 0;
        int day = 0, month = 0, year = 0;
        if (!gsmModem.getNetworkTime(&year, &month, &day, &hours, &minutes,
                                     &seconds, 0)) {
            return 0;
        }

        tm timeParts       = {};
        timeParts.tm_sec   = seconds;
        timeParts.tm_min   = minutes;
        timeParts.tm_hour  = hours;
        timeParts.tm_mday  = day;
        timeParts.tm_mon   = month - 1;
        timeParts.tm_year  = year - 1900;
        timeParts.tm_isdst = 0;
        return static_cast<uint32_t>(mktime(&timeParts)) -
            TimeUtils::getCoreTimeZone();
    }

    /**
     * @brief A struct for the constants for logger modems
     */
    struct LoggerModemNISTConstants {
        static constexpr uint16_t kTimeProtocolPort  = 37;
        static constexpr uint16_t kNistTimeout       = 5000;
        static constexpr size_t   kNistResponseBytes = 4;
        static constexpr uint8_t  kNistServerRetries = 12;
    };
    /**
     * @brief The get NIST time function for modems that do not have both NTP
     * and Time capabilities.
     *
     * This uses the NIST time protocol over TCP (RFC-868) to get the time.
     *
     * @note We must ensure that we do not ping the daylight server more than
     * once every 4 seconds.  NIST clearly specifies here that this is a
     * requirement for all software that accesses its servers:
     * https://tf.nist.gov/tf-cgi/servers.cgi
     *
     * @return Unix timestamp (seconds since Jan 1, 1970 UTC)
     */
    uint32_t getNISTTime(TinyGsmCapabilities::false_type) {
        // create a client
        ClientType nistClient(gsmModem);
        // Try up to 12 times to get a timestamp from NIST.
        for (uint8_t i = 0; i < LoggerModemNISTConstants::kNistServerRetries;
             i++) {
            while (millis() - _lastNISTrequest < 4000) yield();

            // Make TCP connection
            MS_DBG(F("\nConnecting to NIST time server"));
            if (!nistClient.connect("time.nist.gov",
                                    LoggerModemNISTConstants::kTimeProtocolPort,
                                    15)) {
                // If the connection failed, go to the next loop
                MS_DBG(F("Unable to open TCP connection to NIST!"));
                continue;
            }
            // Wait up to 5 seconds for a response
            _lastNISTrequest = millis();
            while (nistClient &&
                   nistClient.available() <
                       LoggerModemNISTConstants::kNistResponseBytes &&
                   millis() - _lastNISTrequest <
                       LoggerModemNISTConstants::kNistTimeout) {
                yield();
            }
            if (nistClient.available() >=
                LoggerModemNISTConstants::kNistResponseBytes) {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte nistBytes[LoggerModemNISTConstants::kNistResponseBytes] = {
                    0};
                nistClient.read(nistBytes,
                                LoggerModemNISTConstants::kNistResponseBytes);
                if (nistClient.connected()) nistClient.stop();
                uint32_t nistParsed = parseNISTBytes(nistBytes);
                if (nistParsed != 0) {
                    MS_DBG(F("Got non-zero NIST timestamp"));
                    return nistParsed;
                } else {
                    MS_DBG(F("Invalid/Zero NIST timestamp"));
                }
            } else {
                MS_DBG(F("NIST time server did not respond!"));
                if (nistClient.connected()) { nistClient.stop(); }
            }
        }
        return 0;
    }

    /**
     * @brief Convert the 4 bytes returned by the NIST time protocol to the
     * number of seconds since January 1, 1970 in UTC.
     *
     * NIST supplies a 4 byte response to any TCP connection made on port 37.
     * This is the 32-bit number of seconds since January 1, 1970 00:00:00 UTC.
     * The server closes the TCP connection immediately after sending the data,
     * so there is no need to close it
     *
     * @param nistBytes 4 bytes from NIST
     * @return the number of seconds since January 1, 1970 00:00:00
     * UTC
     */
    static uint32_t parseNISTBytes(byte nistBytes[4]) {
        // Response is returned as 32-bit number as soon as connection is made
        // Connection is then immediately closed, so there is no need to close
        // it
        uint32_t secFrom1900 = 0;
        for (uint8_t i = 0; i < 4; i++) {
            MS_DBG(F("Response Byte"), i, ':', static_cast<char>(nistBytes[i]),
                   '=', static_cast<uint8_t>(nistBytes[i]), '=',
                   String(nistBytes[i], BIN));
            secFrom1900 += 0x000000FF & nistBytes[i];
            if (i + 1 < 4) { secFrom1900 = secFrom1900 << 8; }
        }
        MS_DBG(F("Seconds from Jan 1, 1900 returned by NIST (UTC):"),
               secFrom1900, '=', String(secFrom1900, BIN));

        // Convert from NIST epoch (seconds since Jan 1, 1900) to Unix epoch
        // (seconds since Jan 1, 1970)
        uint32_t unixTimeStamp = secFrom1900 - EPOCH_NIST_TO_UNIX;
        MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);
        // Sanity check the result
        if (unixTimeStamp < EARLIEST_SANE_UNIX_TIMESTAMP) {
            return 0;
        } else if (unixTimeStamp > LATEST_SANE_UNIX_TIMESTAMP) {
            return 0;
        } else {
            return unixTimeStamp;
        }
    }

    /* ===================================================================== */
    /* Modem Metadata                                                        */
    /* ===================================================================== */
    /**
     * @anchor modem_metadata_functions
     * @name Modem metadata functions
     * Functions to get metadata about modem functionality - using the modem
     * like a sensor.
     *
     * These functions will query the modem to get new values.
     *
     * @note In order to use the modem metadata functions, they must be
     * called after the modem is connected to the internet.
     */
    /**@{*/


    /* ===================================================================== */
    /* Signal Quality                                                        */
    /* ===================================================================== */
 protected:
    /**
     * @brief A helper typedef to determine whether the modem returns RSSI or
     * CSQ for signal quality.
     *
     * Most modules return CSQ between 0 and 31.  The RSSI in dBm is calculated
     * using getRSSIFromCSQ(int16_t csq) and a the percent strength is
     * calculated using getPctFromCSQ(int16_t csq).
     *
     * XBee and Espressif modules return RSSI in dBm and the percent strength is
     * calculated using getPctFromRSSI(int16_t rssi).
     */
    typedef TinyGsmCapabilities::integral_constant<bool, signalQualityIsRSSI>
        SignalQualityIsRSSI;
 public:
    /**
     * @brief Get the signal quality from the modem and write the values to the
     * supplied non-constant references.
     *
     * @remark We can't distinguish between a bad modem response, no modem
     * response, and a real response from the modem of no service/signal.
     * The TinyGSM getSignalQuality function returns the same "no signal" value
     * (99 CSQ or 0 RSSI) in all 3 cases.
     *
     * @param rssi The reference to an int16_t which will be set with the
     * received signal strength indicator.
     * @param percent The reference to an int16_t which will be set with the
     * "percent" signal strength.
     * @return True if the communication with the modem was successful and the
     * values referenced by the pointers should be valid; false otherwise.
     */
    virtual bool getModemSignalQuality(int16_t& rssi, int16_t& percent) {
        MS_DBG(F("Getting signal quality:"));
        int16_t signalQual = gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);
        convertSignalQuality(signalQual, rssi, percent,
                             typename SignalQualityIsRSSI::type());
        return true;
    }
 protected:
    /**
     * @anchor modem_signal_functions
     * @name Functions to convert between signal strength measurement types
     */
    /**@{*/
    /**
     * @brief The convert signal quality function for modems that return RSSI
     * directly - those with "signalQualityIsRSSI" set to a true type.
     *
     * @param signalQual The raw signal quality value from the modem.
     * @param rssi The reference to an int16_t which will be set with the
     * received signal strength indicator.
     * @param percent The reference to an int16_t which will be set with the
     * "percent" signal strength.
     */
    void convertSignalQuality(int16_t signalQual, int16_t& rssi,
                              int16_t& percent,
                              TinyGsmCapabilities::true_type) {
        rssi = signalQual;
        MS_DBG(F("Raw signal is already in units of RSSI:"), rssi);
        percent = getPctFromRSSI(signalQual);
        MS_DBG(F("Signal percent calculated from RSSI:"), percent);
    }
    /**
     * @brief The convert signal quality function for modems that return RSSI
     * directly. - those with "signalQualityIsRSSI" set to a false type.
     *
     * @param signalQual The raw signal quality value from the modem.
     * @param rssi The reference to an int16_t which will be set with the
     * received signal strength indicator.
     * @param percent The reference to an int16_t which will be set with the
     * "percent" signal strength.
     */
    void convertSignalQuality(int16_t signalQual, int16_t& rssi,
                              int16_t& percent,
                              TinyGsmCapabilities::false_type) {
        rssi = getRSSIFromCSQ(signalQual);
        MS_DBG(F("RSSI Estimated from CSQ:"), rssi);
        percent = getPctFromCSQ(signalQual);
        MS_DBG(F("Signal percent calculated from CSQ:"), percent);
    }

    /**
     * @brief Get approximate RSSI from CSQ (assuming no noise).
     *
     * The RSSI is estimated from a look-up assuming no noise.
     *
     * @param csq A "CSQ" (0-31) signal quality
     * @return An RSSI in dBm, making assumptions about the
     * conversion
     */
    static int16_t getRSSIFromCSQ(int16_t csq) {
        if ((csq < 0) || (csq > 31)) return 0;
        // equation matches previous table. not sure the original motivation.
        return ((csq * 2) - 113);
    }

    /**
     * @brief Get signal percent from CSQ.
     *
     * @param csq A "CSQ" (0-31) signal quality
     * @return The percent of maximum signal strength.
     */
    static int16_t getPctFromCSQ(int16_t csq) {
        if ((csq < 0) || (csq > 31)) return 0;
        // equation matches previous table. not sure the original motivation.
        return (csq * 827 + 127) >> 8;
    }

    /**
     * @brief Get signal percent from RSSI.
     *
     * @param rssi The RSSI in dBm.
     * @return The estimated percent of maximum signal strength.
     */
    static int16_t getPctFromRSSI(int16_t rssi) {
        auto pct =
            static_cast<int16_t>(1.6163 * static_cast<float>(rssi) + 182.61);
        if (rssi == 0) pct = 0;
        if (rssi == (255 - 93))
            pct = 0;  // This is a no-data-yet value from XBee
        return pct;
    }
    /**@}*/


    /* ===================================================================== */
    /* Battery Stats - battery voltage, charging state, and charge percent    */
    /* ===================================================================== */
 public:
    /**
     * @brief Get the modem's battery stats
     *
     * This populates the entered references with invalid values for modems
     * where such data is not available.
     *
     * @warning This function does **not** use #MS_INVALID_VALUE for the invalid
     * values! This is because of the size of the int variables and the
     * standards within TinyGSM.
     *
     * @remark The battery values for the modem will only be valid if the modem
     * is directly connected to a battery.  This is __not__ the way most modems
     * are wired.  If the modem is not directly wired to a battery, this will
     * return invalid data.
     *
     * @param chargeState The reference to an int8_t which will be set with the
     * battery charge state - exactly the meaning of this varies by module, but
     * it generally indicates whether the battery is currently charging or
     * discharging.
     * @param percent The reference to an int8_t which will be set with the
     * percent battery charge
     * @param milliVolts The reference to an int16_t which will be set with the
     * battery voltage in millivolts
     * @return True if the communication with the modem was successful and the
     * values referenced by the pointers should be valid; false otherwise.
     */
    virtual bool getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                                      int16_t& milliVolts) {
        return getModemBatteryStats(
            chargeState, percent, milliVolts,
            typename TinyGsmCapabilities::has_battery<GsmModemType>::type());
    }
 protected:
    /**
     * @brief The get battery stats function for modems that are capable of
     * returning battery information.
     * @copydetail getModemBatteryStats(int8_t&, int8_t&, int16_t&)
     */
    bool getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                              int16_t& milliVolts,
                              TinyGsmCapabilities::true_type) {
        MS_DBG(F("Getting modem battery data:"));
        return gsmModem.getBattStats(chargeState, percent, milliVolts);
    }
    /**
     * @brief The get battery stats function for modems that cannot return
     * battery information.
     * @copydetail getModemBatteryStats(int8_t&, int8_t&, int16_t&)
     */
    bool getModemBatteryStats(int8_t& chargeState, int8_t& percent,
                              int16_t& milliVolts,
                              TinyGsmCapabilities::false_type) {
        MS_DBG(F("This modem doesn't return battery information!"));
        chargeState = 99;
        percent     = -99;
        milliVolts  = 9999;
        return false;
    }


    /* ===================================================================== */
    /* Modem internal chip temperature                                       */
    /* ===================================================================== */
 public:
    /**
     * @brief Get the current temperature provided by the modem module.
     *
     * @return The modem temperature in degrees Celsius
     */
    virtual float getModemChipTemperature() {
        return getModemChipTemperature(
            typename TinyGsmCapabilities::has_temperature<
                GsmModemType>::type());
    }
 protected:
    /**
     * @brief The modem temperature function for modems that are able to return
     * temperature
     * @return The modem temperature in degrees Celsius
     */
    float getModemChipTemperature(TinyGsmCapabilities::true_type) {
        MS_DBG(F("Getting temperature:"));
        float temp = gsmModem.getTemperature();
        MS_DBG(F("Temperature:"), temp);

        return temp;
    }
    /**
     * @brief The modem temperature function for modems __cannot__ return
     * temperature
     * @return An invalid value
     */
    float getModemChipTemperature(TinyGsmCapabilities::false_type) {
        MS_DBG(F("This modem doesn't return temperature!"));
        return static_cast<float>(MS_INVALID_VALUE);
    }

 public:
    /**
     * @brief Enables metadata polling for one or more modem measured
     * variables. Setting this to 0b11111111 will enable polling for all modem
     * measured variables.
     *
     * @param pollingBitmask The bitmask indicating which parameters to poll.
     *
     * @see loggerModem::_pollModemMetaData
     *
     * @note This will **not** disable polling for any unset bits in the
     * provided bitmask.  It will only enable those bits that are set.
     */
    void enableMetadataPolling(uint8_t pollingBitmask) {
        _pollModemMetaData |= pollingBitmask;
    }

    /**
     * @brief Disables metadata polling for one or more modem measured
     * variables.  Setting this to 0b11111111 will disable polling for all modem
     * measured variables.
     *
     * @param pollingBitmask The bitmask indicating which parameters to poll.
     *
     * @see loggerModem::_pollModemMetaData
     *
     * @note This will **not** enable polling for any unset bits in the provided
     * bitmask.  It will only disable polling for those bits that are set.
     */
    void disableMetadataPolling(uint8_t pollingBitmask) {
        _pollModemMetaData |= ~pollingBitmask;
    }

    /**
     * @brief Sets the bitmask for modem metadata polling.
     *
     * This will enable polling for 1 bits and disable polling for 0 bits.
     * Setting this to 0 (0b00000000) will disable polling for all metadata
     * parameters.  Setting it to 255 (0b11111111) will enable polling for all
     * parameters.
     *
     * @param pollingBitmask The bitmask indicating which parameters to poll.
     *
     * @see loggerModem::_pollModemMetaData
     */
    void setMetadataPolling(uint8_t pollingBitmask) {
        _pollModemMetaData = pollingBitmask;
    }
    /**
     * @brief Query the modem for signal quality, battery, and temperature
     * information and store the values to the static internal variables.
     *
     * @return True indicates that the communication with the modem
     * was successful and the values of the internal static variables should
     * be valid.
     */
    virtual bool updateModemMetadata() {
        bool success = true;

        // Unset whatever we had previously
        loggerModem::_priorRSSI           = MS_INVALID_VALUE;
        loggerModem::_priorSignalPercent  = MS_INVALID_VALUE;
        loggerModem::_priorBatteryState   = MS_INVALID_VALUE;
        loggerModem::_priorBatteryPercent = MS_INVALID_VALUE;
        loggerModem::_priorBatteryVoltage = MS_INVALID_VALUE;
        loggerModem::_priorModemTemp      = MS_INVALID_VALUE;

        // Initialize variable
        int16_t rssi     = MS_INVALID_VALUE;
        int16_t percent  = MS_INVALID_VALUE;
        int8_t  state    = 99;
        int8_t  bpercent = -99;
        int16_t volt     = 9999;

        MS_DBG(F("Modem polling settings:"), String(_pollModemMetaData, BIN));

        if ((_pollModemMetaData & MODEM_RSSI_ENABLE_BITMASK) ==
                MODEM_RSSI_ENABLE_BITMASK ||
            (_pollModemMetaData & MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) ==
                MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) {
            // Try for up to 15 seconds to get a valid signal quality
            uint32_t startMillis = millis();
            do {
                success &= getModemSignalQuality(rssi, percent);
                loggerModem::_priorRSSI          = rssi;
                loggerModem::_priorSignalPercent = percent;
                if (rssi != 0 && rssi != MS_INVALID_VALUE) break;
                delay(250);
            } while ((rssi == 0 || rssi == MS_INVALID_VALUE) &&
                     millis() - startMillis < 15000L && success);
            MS_DBG(F("CURRENT RSSI:"), rssi);
            MS_DBG(F("CURRENT Percent signal strength:"), percent);
        } else {
            MS_DBG(F("Polling for both RSSI and signal strength is disabled"));
        }

        if ((_pollModemMetaData & MODEM_BATTERY_STATE_ENABLE_BITMASK) ==
                MODEM_BATTERY_STATE_ENABLE_BITMASK ||
            (_pollModemMetaData & MODEM_BATTERY_PERCENT_ENABLE_BITMASK) ==
                MODEM_BATTERY_PERCENT_ENABLE_BITMASK ||
            (_pollModemMetaData & MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) ==
                MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) {
            success &= getModemBatteryStats(state, bpercent, volt);
            MS_DBG(F("CURRENT Modem Battery Charge State:"), state);
            MS_DBG(F("CURRENT Modem Battery Charge Percentage:"), bpercent);
            MS_DBG(F("CURRENT Modem Battery Voltage:"), volt);
            if (state != 99)
                loggerModem::_priorBatteryState = static_cast<float>(state);
            else
                loggerModem::_priorBatteryState =
                    static_cast<float>(MS_INVALID_VALUE);

            if (bpercent != -99)
                loggerModem::_priorBatteryPercent =
                    static_cast<float>(bpercent);
            else
                loggerModem::_priorBatteryPercent =
                    static_cast<float>(MS_INVALID_VALUE);

            if (volt != 9999)
                loggerModem::_priorBatteryVoltage = static_cast<float>(volt);
            else
                loggerModem::_priorBatteryVoltage =
                    static_cast<float>(MS_INVALID_VALUE);
        } else {
            MS_DBG(F("Polling for all modem battery parameters is disabled"));
        }

        if ((_pollModemMetaData & MODEM_TEMPERATURE_ENABLE_BITMASK) ==
            MODEM_TEMPERATURE_ENABLE_BITMASK) {
            loggerModem::_priorModemTemp = getModemChipTemperature();
            MS_DBG(F("CURRENT Modem Chip Temperature:"),
                   loggerModem::_priorModemTemp);
        } else {
            MS_DBG(F("Polling for modem chip temperature is disabled"));
        }

        return success;
    }
    /**@}*/

    /* ===================================================================== */
    /* Static Functions                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_static_functions
     * @name Functions to return the current value of static member variables
     *
     * These functions do **NOT** query the modem for new values, they return
     * the stored value.
     *
     * @note These must be static so that the modem variables can call
     * them.  (Non-static member functions cannot be called without an object.)
     */
    /**@{*/
 public:
    /**
     * @brief Get the stored Modem RSSI value.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored RSSI
     */
    static float getModemRSSI() {
        float retVal = loggerModem::_priorRSSI;
        MS_DEEP_DBG(F("PRIOR RSSI:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem signal strength as a percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored signal strength percent
     */
    static float getModemSignalPercent() {
        float retVal = loggerModem::_priorSignalPercent;
        MS_DEEP_DBG(F("PRIOR Percent signal strength:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem battery charge state.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery charge state
     */
    static float getModemBatteryChargeState() {
        float retVal = loggerModem::_priorBatteryState;
        MS_DEEP_DBG(F("PRIOR Modem Battery Charge State:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem battery charge percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery charge percent
     */
    static float getModemBatteryChargePercent() {
        float retVal = loggerModem::_priorBatteryPercent;
        MS_DEEP_DBG(F("PRIOR Modem Battery Charge Percentage:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem battery voltage.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery voltage in mV
     */
    static float getModemBatteryVoltage() {
        float retVal = loggerModem::_priorBatteryVoltage;
        MS_DEEP_DBG(F("PRIOR Modem Battery Voltage:"), retVal);
        return retVal;
    }

    /**
     * @brief Get the stored modem temperature.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored temperature in degrees Celsius
     */
    static float getModemTemperature() {
        float retVal = loggerModem::_priorModemTemp;
        MS_DEEP_DBG(F("PRIOR Modem Chip Temperature:"), retVal);
        return retVal;
    }
    /**@}*/

    /* ===================================================================== */
    /* Helper Functions                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_helper_functions
     * @name Helper functions
     */
    /**@{*/
 public:
    /**
     * @brief Turn on the modem LED/alert pin - sets it `HIGH`
     */
    void modemLEDOn();
    /**
     * @brief Turn off the modem LED/alert pin - sets it `LOW`
     */
    void modemLEDOff();
    /**
     * @brief Set the processor pin modes (input vs output, with and without
     * pull-up) for all pins connected between the modem module and the mcu.
     */
    virtual void setModemPinModes();
    /**@}*/

    /* ===================================================================== */
    /* Pure Virtual Functions                                                */
    /* ===================================================================== */
    /**
     * @anchor modem_virtual_functions
     * @name Pure virtual functions for each modem to implement
     */
    /**@{*/
 protected:
    /**
     * @brief Perform the parts of the modem sleep process that are unique to a
     * specific module, as opposed to the parts of setup that are common to all
     * modem modules.
     *
     * @return True if the unique part of the sleep function ran
     * successfully.
     */
    virtual bool modemSleepFxn() = 0;
    /**
     * @brief Perform the parts of the modem wake up process that are unique to
     * a specific module, as opposed to the parts of setup that are common to
     * all modem modules.
     *
     * @return True if the unique part of the wake function ran
     * successfully - does _NOT_ indicate that the modem is now responsive.
     */
    virtual bool modemWakeFxn() = 0;
    /**
     * @brief Perform the parts of the modem set up process that are unique to a
     * specific module, as opposed to the parts of setup that are common to all
     * modem modules.
     *
     * Be default, this runs the TinyGSM modem init() function, sets the modem
     * name in internal memory, and synchronizes the modem's time with NTP if
     * the modem is capable of doing so.
     *
     * @return True if extra setup succeeded; false otherwise.
     */
    virtual bool extraModemSetup() {
        bool success = gsmModem.init();
        _modemName   = gsmModem.getModemName();
        syncNTP(typename TinyGsmCapabilities::has_ntp<GsmModemType>::type());
        return success;
    }

    /**
     * @brief Check if the modem was awake using all possible means.
     *
     * If possible, we always want to check if the modem was awake before
     * attempting to wake it up.  Most cellular modules are woken and put to
     * sleep by identical pulses on a sleep or "power" pin.  We don't want to
     * accidentally pulse an already on modem to off.
     *
     * @note It's possible that the status pin is on, but the modem is actually
     * mid-shutdown.  In that case, we'll mistakenly skip re-waking it.  This
     * only applies to modules with a pulse wake (i.e., non-zero wake time). For
     * all modules that do pulse on, where possible I've selected a pulse time
     * that is sufficient to wake but not quite long enough to put it to sleep
     * and am using AT commands to sleep.  This *should* keep everything lined
     * up.
     *
     * @return True if the modem is already awake; false otherwise.
     */
    virtual bool isModemAwake() {
        if (_wakePulse_ms == 0 && _modemSleepRqPin >= 0) {
            // If the wake up is one where a pin is held (0 wake time) and that
            // pin is defined, then we're going to check the level of the held
            // pin as the indication of whether attempts were made to wake the
            // modem before entering the setup function.
            int8_t sleepRqBitNumber =
                log(digitalPinToBitMask(_modemSleepRqPin)) / log(2);
            bool currentRqPinState =
                bitRead(*portInputRegister(digitalPinToPort(_modemSleepRqPin)),
                        sleepRqBitNumber);
            MS_DBG(F("Current state of sleep request pin"), _modemSleepRqPin,
                   '=', currentRqPinState ? F("HIGH") : F("LOW"), F("meaning"),
                   getModemName(), F("should be"),
                   currentRqPinState == _wakeLevel ? F("on") : F("off"));
            return currentRqPinState == _wakeLevel;
        } else if (_statusPin >= 0) {
            // If there's a status pin, use that to determine if the modem is
            // awake
            bool levelNow = digitalRead(_statusPin);
            MS_DBG(getModemName(), F("status pin"), _statusPin, F("level = "),
                   levelNow ? F("HIGH") : F("LOW"), F("meaning"),
                   getModemName(), F("should be"),
                   levelNow == _statusLevel ? F("on") : F("off"));
            return levelNow == _statusLevel;
        } else {
            // If we can't determine status by pin level, try checking if the
            // modem responds to AT commands.
            MS_DEEP_DBG(F("No modem status pins, checking if modem is awake by "
                          "testing AT response"));
            int8_t i   = 5;
            bool   res = false;
            while (i && !res) {
                gsmModem.sendAT(GF(""));
                res = gsmModem.waitResponse(100) == 1;
                if (res) break;
                delay(50);
                i--;
            }
            MS_DBG(F("Tested AT command and got"),
                   res ? F("OK") : F("no response"), F("meaning"),
                   getModemName(),
                   res ? F("must be awake") : F("is probably asleep"));
            return res;
        }
    }
    /**@}*/

    /* ===================================================================== */
    /* Internal Members                                                      */
    /* ===================================================================== */
    /**
     * @anchor modem_ctor_variables
     * @name Member variables set in the constructor
     * These are all related to expected modem response times and the pin
     * connections between the modem module and the logger MCU.
     */
    /**@{*/
 protected:
    /**
     * @brief The digital pin number of the mcu pin controlling power to the
     * modem (active `HIGH`).
     *
     * Should be set to a negative number if the modem should be continuously
     * powered or the power cannot be controlled by the MCU.
     */
    int8_t _powerPin;
    /**
     * @brief The digital pin number of the mcu pin connected to the modem
     * status output pin.
     *
     * Should be set to a negative number if the modem status pin cannot be
     * read.
     */
    int8_t _statusPin;
    /**
     * @brief The level (`LOW` or `HIGH`) of the #_statusPin when the modem
     * is active.
     */
    bool _statusLevel;
    /**
     * @brief The digital pin number of the pin on the mcu attached to the hard
     * or panic reset pin of the modem.
     *
     * Should be set to a negative number if the modem reset pin is not
     * connected to the MCU.
     */
    int8_t _modemResetPin;
    /**
     * @brief The level (`LOW` or `HIGH`) of the #_modemResetPin which will
     * cause the modem to reset.
     */
    bool _resetLevel;
    /**
     * @brief The length of time in milliseconds at #_resetLevel needed on
     * #_modemResetPin to reset the modem.
     */
    uint32_t _resetPulse_ms;
    /**
     * @brief The digital pin number of a pin on the mcu used to request the
     * modem enter its lowest possible power state.
     *
     * Should be set to a negative number if there is no pin usable for deep
     * sleep modes or it is not accessible to the MCU.
     */
    int8_t _modemSleepRqPin;
    /**
     * @brief The level (`LOW` or `HIGH`) on the #_modemSleepRqPin used to
     * **wake** the modem.
     */
    bool _wakeLevel;
    /**
     * @brief The length of pulse in milliseconds at #_wakeLevel needed on the
     * #_modemSleepRqPin to wake the modem.
     *
     * Set to 0 if the pin must be continuously held at #_wakeLevel to keep the
     * modem active.
     */
    uint32_t _wakePulse_ms;
    /**
     * @brief The time in milliseconds between when #modemWake() is run and when
     * the #_statusPin is expected to be at #_statusLevel.
     */
    uint32_t _statusTime_ms;
    /**
     * @brief The maximum length of time in milliseconds between when the modem
     * is requested to enter lowest power state (#modemSleep()) and when it
     * should have completed necessary steps to shut down.
     */
    uint32_t _disconnectTime_ms;
    /**
     * @brief The time in milliseconds between when the modem is powered and
     * when it is able to receive a wake command.
     *
     * That is, the time that should be allowed between #modemPowerUp() and
     * #modemWake().
     */
    uint32_t _wakeDelayTime_ms;
    /**
     * @brief The time in milliseconds between when the modem is awake and when
     * its serial ports reach full functionality and are ready to accept AT
     * commands.
     *
     * That is, the time that should be allowed between #modemWake() and
     * init().  If the modem does not respond within this time frame (plus a
     * 500ms buffer) a #modemHardReset() will be attempted.
     */
    uint32_t _max_at_response_time_ms;
    /**@}*/

    /**
     * @anchor modem_flag_variables
     * @name Flags and other member variables only used internally
     */
    /**@{*/
    /**
     * @brief The digital pin number of a pin on the mcu used to indicate the
     * modem's current activity state.
     *
     * Intended to be a pin attached to a LED.
     *
     * Should be set to a negative number if no LED is available.
     */
    int8_t _modemLEDPin;

    /**
     * @brief The processor elapsed time when the power was turned on for the
     * modem.
     *
     * The #_millisPowerOn value is set in the modemPowerUp()
     * function.  It is un-set in the modemSleepPowerDown() function.
     */
    uint32_t _millisPowerOn = 0;

    /**
     * @brief The processor elapsed time when the a connection to the NIST time
     * server was last attempted.
     *
     * NIST documentation is very clear that it must not be contacted more than
     * once every 4 seconds.
     */
    uint32_t _lastNISTrequest = 0;
    /**
     * @brief Flag.  True indicates that the modem has already successfully
     * completed setup.
     */
    bool _hasBeenSetup = false;
    /**@}*/

    /**
     * @brief The modem name
     *
     * Set in the init() portion of the #modemSetup().
     * Returned by #getModemName().
     */
    String _modemName = "unspecified modem";

    // modemType gsmModem;
    // modemClientType gsmClient;

    // @todo: Implement these for all modems; most support it.

    /**
     * @brief The modem hardware version.
     *
     * Set in #modemSetup().
     * Returned as a portion of the #getModemDevId().
     *
     * @todo Implement this for modems other than the XBee WiFi
     */
    String _modemHwVersion;
    /**
     * @brief The modem firmware version.
     *
     * Set in #modemSetup().
     * Returned as a portion of the #getModemDevId().
     *
     * @todo Implement this for modems other than the XBee WiFi
     */
    String _modemFwVersion;
    /**
     * @brief The modem serial number.
     *
     * Set in #modemSetup().
     * Returned as a portion of the #getModemDevId().
     *
     * @todo Implement this for modems other than the XBee WiFi
     */
    String _modemSerialNumber;

    /**
     * @brief The timezone that the modem will attempt to sync itself to.
     *
     * This doesn't *have* to be the same as the RTC or logger timezone, but
     * you'd be stupid to make it different.
     *
     * @note This must be set for SSL connections to work! If the modem does not
     * have an accurate internal time when attempting an SSL connection, the
     * connection will fail because the certificates will not be within their
     * specified valid time ranges.
     */
    int8_t _modemUTCOffset = 0;

    /**
     * @brief An 8-bit code for the enabled modem polling variables
     *
     * Setting a bit to 0 will disable polling, to 1 will enable it.  By default
     * no polling is enabled to save time and power by not requesting
     * unnecessary information from the modem.  When modem measured variables
     * are attached to a modem, polling for those results is automatically
     * enabled.
     *
     * Bit | Variable Class | Relevent Define
     * ----|----------------|----------------
     *  0  | #Modem_RSSI | #MODEM_RSSI_ENABLE_BITMASK
     *  1  | #Modem_SignalPercent | #MODEM_PERCENT_SIGNAL_ENABLE_BITMASK
     *  2  | #Modem_BatteryState | #MODEM_BATTERY_STATE_ENABLE_BITMASK
     *  3  | #Modem_BatteryPercent | #MODEM_BATTERY_PERCENT_ENABLE_BITMASK
     *  4  | #Modem_BatteryVoltage | #MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK
     *  5  | #Modem_Temp | #MODEM_TEMPERATURE_ENABLE_BITMASK
     */
    uint8_t _pollModemMetaData = 0;


    /**
     * @anchor modem_static_variables
     * @name Static member variables used to hold modem metadata
     *
     * @note These must be static so that the modem variables can call the
     * member functions that return them.  (Non-static member functions cannot
     * be called without an object.)
     */
    /**@{*/
    /**
     * @brief The last stored RSSI value
     *
     * Set by #getModemSignalQuality() or updateModemMetadata().
     * Returned by #getModemRSSI().
     */
    static int16_t _priorRSSI;
    /**
     * @brief The last stored signal strength percent value
     *
     * Set by #getModemSignalQuality() or updateModemMetadata().
     * Returned by #getModemSignalPercent().
     */
    static int16_t _priorSignalPercent;
    /**
     * @brief The last stored modem chip temperature value
     *
     * Set by #getModemChipTemperature() or updateModemMetadata().
     * Returned by #getModemTemperature().
     */
    static float _priorModemTemp;
    /**
     * @brief The last stored modem battery state value
     *
     * Set by #getModemBatteryStats() or updateModemMetadata().
     * Returned by #getModemBatteryChargeState().
     */
    static float _priorBatteryState;
    /**
     * @brief The last stored modem battery percent value
     *
     * Set by #getModemBatteryStats() or updateModemMetadata().
     * Returned by #getModemBatteryChargePercent().
     */
    static float _priorBatteryPercent;
    /**
     * @brief The last stored modem battery voltage value
     *
     * Set by #getModemBatteryStats() or updateModemMetadata().
     * Returned by #getModemBatteryVoltage().
     */
    static float _priorBatteryVoltage;
    // static float _priorActivationDuration;
    // static float _priorPoweredDuration;
    /**@}*/
};


// Classes for the modem variables

/**
 * @brief The Variable sub-class used for the RSSI measured by a modem.
 *
 * The value is in decibelMiliWatts and has resolution of 1 dBm.
 *
 * @ingroup modem_measured_variables
 */
template <typename LoggerModemTypeArg>
class Modem_RSSI : public Variable {
 public:
    /**
     * @brief Construct a new Modem_RSSI object.
     *
     * @param parentModem The parent modem providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RSSI".
     */
    explicit Modem_RSSI(LoggerModemTypeArg* parentModem, const char* uuid = "",
                        const char* varCode = MODEM_RSSI_DEFAULT_CODE)
        : Variable(&parentModem->getModemRSSI, MODEM_RSSI_RESOLUTION,
                   &*MODEM_RSSI_VAR_NAME, &*MODEM_RSSI_UNIT_NAME, varCode,
                   uuid) {
        parentModem->enableMetadataPolling(MODEM_RSSI_ENABLE_BITMASK);
    }
    /**
     * @brief Destroy the Modem_RSSI object - no action needed.
     */
    ~Modem_RSSI() override = default;
};


/**
 * @brief The Variable sub-class used for the percent of maximum signal measured
 * by a modem.
 *
 * The value is in units of percent and has resolution of 1 percent.
 *
 * @ingroup modem_measured_variables
 */
template <typename LoggerModemTypeArg>
class Modem_SignalPercent : public Variable {
 public:
    /**
     * @brief Construct a new Modem_SignalPercent object.
     *
     * @param parentModem The parent modem providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "signalPercent".
     */
    explicit Modem_SignalPercent(
        LoggerModemTypeArg* parentModem, const char* uuid = "",
        const char* varCode = MODEM_PERCENT_SIGNAL_DEFAULT_CODE)
        : Variable(&parentModem->getModemSignalPercent,
                   MODEM_PERCENT_SIGNAL_RESOLUTION,
                   &*MODEM_PERCENT_SIGNAL_VAR_NAME,
                   &*MODEM_PERCENT_SIGNAL_UNIT_NAME, varCode, uuid) {
        parentModem->enableMetadataPolling(MODEM_PERCENT_SIGNAL_ENABLE_BITMASK);
    }
    /**
     * @brief Destroy the Modem_SignalPercent object - no action needed.
     */
    ~Modem_SignalPercent() override = default;
};


/**
 * @brief The Variable sub-class used for the battery charge state measured by a
 * modem.
 *
 * Whether or not this value is meaningful depends on the specific modem
 * subclass and the wiring of the logger setup.
 *
 * The value is dimensionless and has a resolution of 1.
 *
 * @ingroup modem_measured_variables
 */
template <typename LoggerModemTypeArg>
class Modem_BatteryState : public Variable {
 public:
    /**
     * @brief Construct a new Modem_BatteryState object.
     *
     * @param parentModem The parent modem providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "modemBatteryCS".
     */
    explicit Modem_BatteryState(
        LoggerModemTypeArg* parentModem, const char* uuid = "",
        const char* varCode = MODEM_BATTERY_STATE_DEFAULT_CODE)
        : Variable(&parentModem->getModemBatteryChargeState,
                   MODEM_BATTERY_STATE_RESOLUTION,
                   &*MODEM_BATTERY_STATE_VAR_NAME,
                   &*MODEM_BATTERY_STATE_UNIT_NAME, varCode, uuid) {
        parentModem->enableMetadataPolling(MODEM_BATTERY_STATE_ENABLE_BITMASK);
    }
    /**
     * @brief Destroy the Modem_BatteryState object - no action needed.
     */
    ~Modem_BatteryState() override = default;
};


/**
 * @brief The Variable sub-class used for the battery charge percent measured by
 * a modem.
 *
 * Whether or not this value is meaningful depends on the specific modem
 * subclass and the wiring of the logger setup.
 *
 * The value is has units of percent and has a resolution of 1%.
 *
 * @ingroup modem_measured_variables
 */
template <typename LoggerModemTypeArg>
class Modem_BatteryPercent : public Variable {
 public:
    /**
     * @brief Construct a new Modem_BatteryPercent object.
     *
     * @param parentModem The parent modem providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "modemBatteryPct".
     */
    explicit Modem_BatteryPercent(
        LoggerModemTypeArg* parentModem, const char* uuid = "",
        const char* varCode = MODEM_BATTERY_PERCENT_DEFAULT_CODE)
        : Variable(&parentModem->getModemBatteryChargePercent,
                   MODEM_BATTERY_PERCENT_RESOLUTION,
                   &*MODEM_BATTERY_PERCENT_VAR_NAME,
                   &*MODEM_BATTERY_PERCENT_UNIT_NAME, varCode, uuid) {
        parentModem->enableMetadataPolling(
            MODEM_BATTERY_PERCENT_ENABLE_BITMASK);
    }
    /**
     * @brief Destroy the Modem_BatteryPercent object - no action needed.
     */
    ~Modem_BatteryPercent() override = default;
};


/**
 * @brief The Variable sub-class used for the battery voltage measured by a
 * modem.
 *
 * Whether or not this value is meaningful depends on the specific modem
 * subclass and the wiring of the logger setup.
 *
 * The value has units of mV and has a resolution of 1mV.
 *
 * @ingroup modem_measured_variables
 */
template <typename LoggerModemTypeArg>
class Modem_BatteryVoltage : public Variable {
 public:
    /**
     * @brief Construct a new Modem_BatteryVoltage object.
     *
     * @param parentModem The parent modem providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "modemBatterymV".
     */
    explicit Modem_BatteryVoltage(
        LoggerModemTypeArg* parentModem, const char* uuid = "",
        const char* varCode = MODEM_BATTERY_VOLTAGE_DEFAULT_CODE)
        : Variable(&parentModem->getModemBatteryVoltage,
                   MODEM_BATTERY_VOLTAGE_RESOLUTION,
                   &*MODEM_BATTERY_VOLTAGE_VAR_NAME,
                   &*MODEM_BATTERY_VOLTAGE_UNIT_NAME, varCode, uuid) {
        parentModem->enableMetadataPolling(
            MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK);
    }
    /**
     * @brief Destroy the Modem_BatteryVoltage object - no action needed.
     */
    ~Modem_BatteryVoltage() override = default;
};


/**
 * @brief The Variable sub-class used for the chip temperature as measured by
 * the modem.
 *
 * Whether or not this value is meaningful depends on the specific modem
 * subclass.
 *
 * The value has units of degrees Celsius and has a resolution of 0.1°C.
 *
 * @ingroup modem_measured_variables
 */
template <typename LoggerModemTypeArg>
class Modem_Temp : public Variable {
 public:
    /**
     * @brief Construct a new Modem_Temp object.
     *
     * @param parentModem The parent modem providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "modemTemp".
     */
    explicit Modem_Temp(LoggerModemTypeArg* parentModem, const char* uuid = "",
                        const char* varCode = MODEM_TEMPERATURE_DEFAULT_CODE)
        : Variable(&parentModem->getModemTemperature,
                   MODEM_TEMPERATURE_RESOLUTION, &*MODEM_TEMPERATURE_VAR_NAME,
                   &*MODEM_TEMPERATURE_UNIT_NAME, varCode, uuid) {
        parentModem->enableMetadataPolling(MODEM_TEMPERATURE_ENABLE_BITMASK);
    }
    /**
     * @brief Destroy the Modem_Temp object - no action needed.
     */
    ~Modem_Temp() override = default;
};


#endif  // SRC_LOGGERMODEM_H_

// cSpell:ignore bpercent modemBatterymV
