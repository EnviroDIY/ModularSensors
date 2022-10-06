/**
 * @file LoggerModem.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the loggerModem class and the variable subclasses
 * Modem_RSSI, Modem_SignalPercent, Modem_BatteryState, Modem_BatteryPercent,
 * and Modem_BatteryVoltage - all of which are implemented as "calculated"
 * variables.
 */
/**
 * @defgroup the_modems Supported Modems and Communication Modules
 * All implemented loggerModem classes
 *
 * @copydetails loggerModem
 *
 * @see @ref page_modem_notes
 */

// Header Guards
#ifndef SRC_LOGGERMODEM_H_
#define SRC_LOGGERMODEM_H_

// FOR DEBUGGING
// #define MS_LOGGERMODEM_DEBUG
// #define MS_LOGGERMODEM_DEBUG_DEEP

#ifdef MS_LOGGERMODEM_DEBUG
#define MS_DEBUGGING_STD "LoggerModem"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include <Arduino.h>


/**
 * @defgroup modem_measured_variables Modem Variables
 *
 * Variable objects to be tied to a loggerModem.  These are measured by a modem,
 * but are implemented as calculated variables.
 *
 * @note  The modem is NOT set up as a sensor.  ALl of these variables for the
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
 * @brief Decimals places in string representation; RSSI should have 0.
 *
 * RSSI is a rough calculation, so it has 0 decimal place resolution
 */
#define MODEM_RSSI_RESOLUTION 0
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
 * @brief Decimals places in string representation; percent signal should have
 * 0.
 *
 * Percent signal is a rough calculation, so it has 0 decimal place resolution
 */
#define MODEM_PERCENT_SIGNAL_RESOLUTION 0
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
 * @brief Decimals places in string representation; battery state should have 0.
 *
 * Battery state is a code value; it has 0 decimal place resolution
 */
#define MODEM_BATTERY_STATE_RESOLUTION 0
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
/// @brief Decimals places in string representation; battery charge percent
/// should have 0.
#define MODEM_BATTERY_PERCENT_RESOLUTION 0
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
 * @brief Decimals places in string representation; battery voltage should have
 * 0.
 *
 * No supported module has higher than 1mV resolution in battery reading.
 */
#define MODEM_BATTERY_VOLTAGE_RESOLUTION 0
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
 * @brief Decimals places in string representation; temperature should
 * have 1.
 *
 * Most modules that can measure temperature measure to 0.1°C
 */
#define MODEM_TEMPERATURE_RESOLUTION 1
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
/// @brief Decimals places in string representation; total active time should
/// have 3.
#define MODEM_ACTIVATION_RESOLUTION 3
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
/// @brief Decimals places in string representation; total powered time should
/// have 3.
#define MODEM_POWERED_RESOLUTION 3
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


/* ===========================================================================
 * Functions for the modem class
 * This is basically a wrapper for TinyGsm with power control added
 * ========================================================================= */

// template <class Derived, typename modemType, typename modemClientType>
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
 */
class loggerModem {
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
     * @param max_disconnetTime_ms @copybrief loggerModem::_disconnetTime_ms
     * @param wakeDelayTime_ms @copybrief loggerModem::_wakeDelayTime_ms
     * @param max_atresponse_time_ms @copybrief #_max_atresponse_time_ms
     *
     * @see @ref modem_ctor_variables
     */
    loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                int8_t modemResetPin, bool resetLevel, uint32_t resetPulse_ms,
                int8_t modemSleepRqPin, bool wakeLevel, uint32_t wakePulse_ms,
                uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                uint32_t wakeDelayTime_ms, uint32_t max_atresponse_time_ms);

    /**
     * @brief Destroy the logger Modem object - no action taken.
     */
    virtual ~loggerModem();

    /**
     * @brief Set an LED to turn on (pin will be `HIGH`) when the modem is on.
     *
     * @param modemLEDPin The digital pin number for the LED
     */
    void setModemLED(int8_t modemLEDPin);

    /**
     * @brief Get the modem name.
     *
     * @return **String** The modem name
     */
    String getModemName(void);

    /**
     * @brief Set up the modem before first use.
     *
     * This is used for operations that cannot happen in the modem constructor -
     * they must happen at run time, not at compile time.
     *
     * @return **bool** True if setup was successful
     */
    virtual bool modemSetup(void);
    /**
     * @brief Retained for backwards compatibility; use modemSetup() in new
     * code.
     *
     * @m_deprecated_since{0,24,1}
     *
     * @return **bool** True if setup was successful
     */
    bool setup(void) {
        return modemSetup();
    }

    /**
     * @anchor modem_power_functions
     * @name Functions related to the modem power and activity state
     *
     * These are similar to the like-named Sensor functions.
     */
    /**@{*/
    /**
     * @brief Wake up the modem.
     *
     * This sets pin modes, powers up the modem if necessary, sets time stamps,
     * runs the specific modem's wake function, tests for responsiveness to AT
     * commands, and then re-runs the TinyGSM init() if necessary.  If the modem
     * fails to respond, this attempts a "hard" pin reset if possible.
     *
     * For most modules, this function is created by the #MS_MODEM_WAKE macro.
     *
     * @return **bool** True if the modem is responsive and ready for action.
     */
    virtual bool modemWake(void) = 0;
    /**
     * @brief Retained for backwards compatibility; use modemWake() in new code.
     *
     * @m_deprecated_since{0,24,1}
     *
     * @return **bool** True if wake was sucessful, modem should be ready to
     * communicate
     */
    bool wake(void) {
        return modemWake();
    }

    /**
     * @brief Power the modem by setting the modem power pin high.
     */
    virtual void modemPowerUp(void);
    /**
     * @brief Cut power to the modem by setting the modem power pin low.
     *
     * @note modemPowerDown() simply kills power, while modemSleepPowerDown()
     * allows for graceful shut down.  You should use modemSleepPowerDown()
     * whenever possible.
     */
    virtual void modemPowerDown(void);
    /**
     * @brief Request that the modem enter its lowest possible power state.
     *
     * @return **bool** True if the modem has sucessfully entered low power
     * state
     */
    virtual bool modemSleep(void);
    /**
     * @brief Request that the modem enter its lowest possible power state and
     * then set the power pin low after the modem has indicated it has
     * successfully gone to low power.
     *
     * This allows the modem to shut down all connections cleanly and do any
     * necessary internal housekeeping before stopping power.
     *
     * @return **bool** True if the modem has sucessfully entered low power
     * state _and_ then powered off
     */
    virtual bool modemSleepPowerDown(void);
    /**@}*/

    /**
     * @brief Use the modem reset pin specified in the constructor to perform a
     * "hard" or "panic" reset.
     *
     * This should only be used if the modem is clearly non-responsive.
     *
     * @return **bool** True if the reset succeeded and the modem should now be
     * responsive.  False if the modem remains non-responsive either because the
     * reset failed to fix the communication issue or because a reset is not
     * possible with the current pin/modem configuration.
     */
    virtual bool modemHardReset(void);


    /**
     * @anchor modem_pin_functions
     * @name Pin setting functions
     * Functions to set or re-set the the pin numbers for the connection between
     * the modem module and the logger MCU.
     */
    /**@{*/
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
    void setModemStatusLevel(bool level);

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
    void setModemWakeLevel(bool level);

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
    void setModemResetLevel(bool level);
    /**@}*/

    /**
     * @anchor modem_internet_functions
     * @name Functions for internet connectivity
     */
    /**@{*/
    /**
     * @brief Wait for the modem to successfully register on the cellular
     * network and then request that it establish either EPS or GPRS data
     * connection.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data sconnection.  Defaults to 50,000ms
     * (50s).
     * @return **bool** True if EPS or GPRS data connection has been
     * established.  False if the modem wasunresponsive, unable to register with
     * the cellular network, or unable to establish a EPS or GPRS connection.
     */
    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) = 0;
    /**
     * @brief Detatch from EPS or GPRS data connection and then deregister from
     * the cellular network.
     */
    virtual void disconnectInternet(void) = 0;


    /**
     * @brief Get the time from NIST via TIME protocol (rfc868).
     *
     * This would be much more efficient if done over UDP, but I'm doing it over
     * TCP because I don't have a UDP library for all the modems.
     *
     * @note The return is the number of seconds since Jan 1, 1970 IN UTC
     *
     * @return **uint32_t** The number of seconds since Jan 1, 1970 IN UTC
     */
    virtual uint32_t getNISTTime(void) = 0;
    /**@}*/


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
    /**
     * @brief Query the modem for the current signal quality and write the
     * results to the supplied non-constant references.
     *
     * @param rssi A reference to an int16_t which will be set with the received
     * signal strength indicator
     * @param percent A reference to an int16_t which will be set with the
     * "percent" signal strength
     * @return **bool** True indicates that the communication with the modem was
     * successful and the values referenced by the pointers should be valid.
     */
    virtual bool getModemSignalQuality(int16_t& rssi, int16_t& percent) = 0;

    /**
     * @brief Query the modem for battery information and write the values to
     * the supplied non-constant references.
     *
     * @param chargeState A reference to an uint8_t which will be set with the
     * current charge state (significance of value varies)
     * @param percent A reference to an int8_t which will be set with the
     * current charge percent - this may or may not be a valid value depending
     * on the module and breakout.
     * @param milliVolts A reference to an uint16_t which will be set with the
     * current battery voltage in mV - this may or may not be a valid value
     * depending on the module and breakout.
     * @return **bool** True indicates that the communication with the modem was
     * successful and the values referenced by the pointers should be valid.
     */
    virtual bool getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                                      uint16_t& milliVolts) = 0;
    /**
     * @brief Get the current temperature provided by the modem module.
     *
     * @return **float** The temperature in degrees Celsius
     */
    virtual float getModemChipTemperature(void) = 0;

    /**
     * @brief Query the modem for signal quality, battery, and temperature
     * information and store the values to the static internal variables.
     *
     * @return **bool** True indicates that the communication with the modem was
     * successful and the values of the internal static variables should be
     * valid.
     */
    virtual bool updateModemMetadata(void);
    /**@}*/

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
    /**
     * @brief Get the stored Modem RSSI value.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return **float** The stored RSSI
     */
    static float getModemRSSI();

    /**
     * @brief Get the stored modem signal strength as a percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return **float** The stored signal strength
     */
    static float getModemSignalPercent();

    /**
     * @brief Get the stored modem battery charge state.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return **float** The stored signal percent
     */
    static float getModemBatteryChargeState();

    /**
     * @brief Get the stored modem battery charge percent object.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return **float** The stored battery charge percent
     */
    static float getModemBatteryChargePercent();

    /**
     * @brief Get the stored modem battery voltage.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return **float** The stored battery voltage in mV
     */
    static float getModemBatteryVoltage();

    /**
     * @brief Get the stored modem temperature.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return **float** The stored temperature in degrees Celsius
     */
    static float getModemTemperature();
    /**@}*/

 protected:
    /**
     * @anchor modem_signal_functions
     * @name Functions to convert between signal strength measurement types
     */
    /**@{*/
    /**
     * @brief Get approximate RSSI from CSQ (assuming no noise).
     *
     * The RSSI is estimated from a look-up assuming no noise.
     *
     * @param csq A "CSQ" (0-31) signal qualilty
     * @return **int16_t** An RSSI in dBm, making assumptions about the
     * conversion
     */
    static int16_t getRSSIFromCSQ(int16_t csq);
    /**
     * @brief Get signal percent from CSQ.
     *
     * The percent is grabbed from a look-up.
     *
     * @param csq A "CSQ" (0-31) signal qualilty
     * @return **int16_t** The percent of maximum signal strength.
     */
    static int16_t getPctFromCSQ(int16_t csq);
    /**
     * @brief Get signal percent from CSQ.
     *
     * @param rssi The RSSI in dBm.
     * @return **int16_t** The estimated percent of maximum signal strength.
     */
    static int16_t getPctFromRSSI(int16_t rssi);
    /**@}*/

    /**
     * @anchor modem_helper_functions
     * @name Helper functions
     */
    /**@{*/
    /**
     * @brief Turn on the modem LED/alert pin - sets it `HIGH`
     */
    void modemLEDOn(void);
    /**
     * @brief Turn off the modem LED/alert pin - sets it `LOW`
     */
    void modemLEDOff(void);
    /**
     * @brief Set the processor pin modes (input vs output, with and without
     * pull-up) for all pins connected between the modem module and the mcu.
     */
    virtual void setModemPinModes(void);
    /**@}*/

    /**
     * @anchor modem_virtual_functions
     * @name Pure virtual functions for each modem to implement
     */
    /**@{*/
    /**
     * @brief Check whether there is an active internet connection available.
     *
     * @return **bool** True if there is an active data connection to the
     * internet.
     */
    virtual bool isInternetAvailable(void) = 0;
    /**
     * @brief Perform the parts of the modem sleep process that are unique to a
     * specific module, as opposed to the parts of setup that are common to all
     * modem modules.
     *
     * @return **bool** True if the unique part of the sleep function ran
     * sucessfully.
     */
    virtual bool modemSleepFxn(void) = 0;
    /**
     * @brief Perform the parts of the modem wake up process that are unique to
     * a specific module, as opposed to the parts of setup that are common to
     * all modem modules.
     *
     * @return **bool** True if the unique part of the wake function ran
     * sucessfully - does _NOT_ indicate that the modem is now responsive.
     */
    virtual bool modemWakeFxn(void) = 0;
    /**
     * @brief Perform the parts of the modem set up process that are unique to a
     * specific module, as opposed to the parts of setup that are common to all
     * modem modules.
     *
     * For most modules, this function is created by the #MS_MODEM_EXTRA_SETUP
     * macro which runs the TinyGSM modem init() and client init() functions.
     *
     * @return **bool** True if the extra setup succeeded.
     */
    virtual bool extraModemSetup(void) = 0;
    /**
     * @brief Check if the modem was awake using all possible means.
     *
     * If possible, we always want to check if the modem was awake before
     * attempting to wake it up.  Most cellular modules are woken and put to
     * sleep by identical pulses on a sleep or "power" pin.  We don't want to
     * accidently pulse an already on modem to off.
     *
     * For most modules, this function is created by the #MS_IS_MODEM_AWAKE
     * macro.
     *
     * @note It's possible that the status pin is on, but the modem is actually
     * mid-shutdown.  In that case, we'll mistakenly skip re-waking it.  This
     * only applies to modules with a pulse wake (ie, non-zero wake time).  For
     * all modules that do pulse on, where possible I've selected a pulse time
     * that is sufficient to wake but not quite long enough to put it to sleep
     * and am using AT commands to sleep.  This *should* keep everything lined
     * up.
     *
     * @return **bool** True if the modem is already awake.
     */
    virtual bool isModemAwake(void) = 0;
    /**@}*/

    /**
     * @brief Convert the 4 bytes returned on the NIST daytime protocol to the
     * number of seconds since January 1, 1970 in UTC.
     *
     * NIST supplies a 4 byte response to any TCP connection made on port 37.
     * This is the 32-bit number of seconds since January 1, 1970 00:00:00 UTC.
     * The server closes the TCP connection immediately after sending the data,
     * so there is no need to close it
     *
     * @param nistBytes 4 bytes from NIST
     * @return **uint32_t** the number of seconds since January 1, 1970 00:00:00
     * UTC
     */
    static uint32_t parseNISTBytes(byte nistBytes[4]);

    /**
     * @anchor modem_ctor_variables
     * @name Member variables set in the constructor
     * These are all related to expected modem response times and the pin
     * connections between the modem module and the logger MCU.
     */
    /**@{*/
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
     * @brief The digital pin number of the pin on the mcu attached the the hard
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
    uint32_t _disconnetTime_ms;
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
    uint32_t _max_atresponse_time_ms;
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
    /**
     * @brief Flag.  True indicates that the pins on the mcu attached to the
     * modem are set to the correct mode (ie, input vs output).
     */
    bool _pinModesSet = false;
    /**@}*/

    // NOTE:  These must be static so that the modem variables can call the
    // member functions that return them.  (Non-static member functions cannot
    // be called without an object.)
    /**
     * @anchor modem_static_variables
     * @name Static member variables used to hold modem metadata
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

    /**
     * @brief The modem name
     *
     * Set in the init() portion of the #modemSetup().
     * Returned by #getModemName().
     */
    String _modemName = "unspecified modem";

    // modemType gsmModem;
    // modemClientType gsmClient;
};

// typedef float (loggerModem::_*loggerGetValueFxn)(void);

// Classes for the modem variables

/**
 * @brief The Variable sub-class used for the RSSI measured by a modem.
 *
 * The value is in decibelMiliWatts and has resolution of 1 dBm.
 *
 * @ingroup modem_measured_variables
 */
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
    explicit Modem_RSSI(loggerModem* parentModem, const char* uuid = "",
                        const char* varCode = MODEM_RSSI_DEFAULT_CODE)
        : Variable(&parentModem->getModemRSSI, (uint8_t)MODEM_RSSI_RESOLUTION,
                   &*MODEM_RSSI_VAR_NAME, &*MODEM_RSSI_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Destroy the Modem_RSSI object - no action needed.
     */
    ~Modem_RSSI() {}
};


/**
 * @brief The Variable sub-class used for the percent of maximum signal measured
 * by a modem.
 *
 * The value is in units of percent and has resolution of 1 percent.
 *
 * @ingroup modem_measured_variables
 */
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
        loggerModem* parentModem, const char* uuid = "",
        const char* varCode = MODEM_PERCENT_SIGNAL_DEFAULT_CODE)
        : Variable(&parentModem->getModemSignalPercent,
                   (uint8_t)MODEM_PERCENT_SIGNAL_RESOLUTION,
                   &*MODEM_PERCENT_SIGNAL_VAR_NAME,
                   &*MODEM_PERCENT_SIGNAL_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Destroy the Modem_SignalPercent object - no action needed.
     */
    ~Modem_SignalPercent() {}
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
        loggerModem* parentModem, const char* uuid = "",
        const char* varCode = MODEM_BATTERY_STATE_DEFAULT_CODE)
        : Variable(&parentModem->getModemBatteryChargeState,
                   (uint8_t)MODEM_BATTERY_STATE_RESOLUTION,
                   &*MODEM_BATTERY_STATE_VAR_NAME,
                   &*MODEM_BATTERY_STATE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Destroy the Modem_BatteryState object - no action needed.
     */
    ~Modem_BatteryState() {}
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
        loggerModem* parentModem, const char* uuid = "",
        const char* varCode = MODEM_BATTERY_PERCENT_DEFAULT_CODE)
        : Variable(&parentModem->getModemBatteryChargePercent,
                   (uint8_t)MODEM_BATTERY_PERCENT_RESOLUTION,
                   &*MODEM_BATTERY_PERCENT_VAR_NAME,
                   &*MODEM_BATTERY_PERCENT_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Destroy the Modem_BatteryPercent object - no action needed.
     */
    ~Modem_BatteryPercent() {}
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
        loggerModem* parentModem, const char* uuid = "",
        const char* varCode = MODEM_BATTERY_VOLTAGE_DEFAULT_CODE)
        : Variable(&parentModem->getModemBatteryVoltage,
                   (uint8_t)MODEM_BATTERY_VOLTAGE_RESOLUTION,
                   &*MODEM_BATTERY_VOLTAGE_VAR_NAME,
                   &*MODEM_BATTERY_VOLTAGE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Destroy the Modem_BatteryVoltage object - no action needed.
     */
    ~Modem_BatteryVoltage() {}
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
    explicit Modem_Temp(loggerModem* parentModem, const char* uuid = "",
                        const char* varCode = MODEM_TEMPERATURE_DEFAULT_CODE)
        : Variable(&parentModem->getModemTemperature,
                   (uint8_t)MODEM_TEMPERATURE_RESOLUTION,
                   &*MODEM_TEMPERATURE_VAR_NAME, &*MODEM_TEMPERATURE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Destroy the Modem_Temp object - no action needed.
     */
    ~Modem_Temp() {}
};

// #include <LoggerModem.tpp>
#endif  // SRC_LOGGERMODEM_H_
