/**
 * @file LoggerModem.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains type type-erased loggerModem class and the variable
 * subclasses Modem_RSSI, Modem_SignalPercent, Modem_BatteryState,
 * Modem_BatteryPercent, and Modem_BatteryVoltage - all of which are implemented
 * as "calculated" variables.
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
#include "VariableBase.h"

#include <Arduino.h>
#include <Client.h>

// Include the TinyGSM capabilities file to check what the modem can do
#include <TinyGsmCapabilities.h>
// Include TinyGsmSSL for SSLAuthMode and SSLVersion enums
#include <TinyGsmSSL.tpp>


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
 *
 * This class provides a common type-erased interface that allows LoggerBase and
 * other non-template code to hold pointers to modem objects regardless of their
 * specific template instantiation. All public-facing modem functionality
 * must be declared as virtual functions in this base class.  This allows
 * non-template code to interact with modem objects without knowing their
 * specific template parameters.
 */
class loggerModem {
 public:
    virtual ~loggerModem() = default;


    /**
     * @anchor modem_setup_functions
     * @name Functions related to the modem setup and identification
     *
     * These are similar to the like-named Sensor functions.
     */
    /**@{*/
    /**
     * @brief Set an LED to turn on (pin will be `HIGH`) when the modem is on.
     * @param modemLEDPin The digital pin number of the pin on the MCU connected
     * to the modem status LED or alert pin.  Set to a negative number if there
     * is no such pin.
     */
    virtual void setModemLED(int8_t modemLEDPin) = 0;
    /**
     * @brief Get the modem name.
     *
     * @note This returns the internally stored name of the modem without
     * re-querying the hardware.
     *
     * @return The modem name
     */
    virtual String getModemName() = 0;
    /**
     * @brief Get the modem device ID.
     *
     * @note This returns the internally stored device ID of the modem without
     * re-querying the hardware.
     *
     * @return The concatenated name, hardware version, firmware
     * version, and serial number of the modem.
     */
    virtual String getModemDevId() = 0;
    /**
     * @brief Set the UTC offset for the modem to use internally in NTP syncing
     *
     * This doesn't *have* to be the same as the RTC or logger timezone, but
     * you'd be stupid to make it different.
     *
     * @note This must be set for SSL connections to work! If the modem does not
     * have an accurate internal time when attempting an SSL connection, the
     * connection will fail because the certificates will not be within their
     * specified valid time ranges.
     *
     * @param timeZone UTC offset in hours that the modem will attempt to sync
     * itself to
     */
    virtual void setModemTimeZone(int8_t timeZone) = 0;
    /**
     * @brief Set up the modem before first use.
     *
     * This is used for operations that cannot happen in the modem constructor -
     * they must happen at run time, not at compile time.  By default, this
     * performs TinyGSM initialization and any modem-specific setup.
     *
     * @return True if setup was successful
     */
    virtual bool modemSetup() = 0;
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
    /**@}*/


    /**
     * @anchor modem_power_functions
     * @name Modem power management
     * Functions to power up or down the modem, wake it from sleep, or put it
     * to sleep.
     */
    /**@{*/
    /**
     * @brief Power up the modem.
     *
     * If there's a power pin, this sets that pin high to power the modem. If
     * there's a sleep request pin, it is set to the "not wake" level before
     * power up.
     */
    virtual void modemPowerUp() = 0;
    /**
     * @brief Cut power to the modem by setting the modem power pin low.
     *
     * @note modemPowerDown() simply kills power, while modemSleepPowerDown()
     * allows for graceful shut down.  You should use modemSleepPowerDown()
     * whenever possible.
     */
    virtual void modemPowerDown() = 0;
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
    virtual bool modemWake() = 0;
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
    virtual bool modemSleep() = 0;
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
    virtual bool modemSleepPowerDown() = 0;

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
    virtual bool modemHardReset() = 0;
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
    virtual bool isModemAwake() = 0;
    /**@}*/


    /**
     * @anchor modem_pin_functions
     * @name Pin setting functions
     * Functions to set or re-set the pin numbers for the connection between
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
    virtual void setModemStatusLevel(bool level) = 0;

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
    virtual void setModemWakeLevel(bool level) = 0;

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
    virtual void setModemResetLevel(bool level) = 0;
    /**@}*/


    /**
     * @anchor modem_connection_functions
     * @name Functions for internet connection and disconnection
     * Functions for managing internet connection and determining connectivity
     * status.
     */
    /**@{*/
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
    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) = 0;
    /**
     * @brief Disconnect from the internet
     */
    virtual void disconnectInternet() = 0;
    /**
     * @brief Check whether there is an active internet connection available.
     * @return True if there is an active data connection to the internet; false
     * otherwise
     */
    virtual bool isInternetAvailable() = 0;
    /**@}*/


    /**
     * @anchor modem_client_functions
     * @name Functions for creating standard TCP clients
     */
    /**@{*/
    /**
     * @brief Create a new client object
     * @warning Be sure to delete this object when you're done with it!
     * @param mux Multiplexing channel to use, defaults to 0
     * @return A new client object
     *
     * @note ALL modems MUST support client creation. Every TinyGSM modem does.
     * We do not need two flavors of this function dependent on capabilities.
     */
    virtual Client* createClient(uint8_t mux = 0) = 0;
    /**
     * @brief Attempts to delete a created TinyGsmClient object. We need to do
     * this to close memory leaks from the create client because we can't
     * delete the created client from a pointer to the parent because the
     * Arduino core's client class doesn't have a virtual destructor.
     *
     * @warning CRITICAL: This function MUST only be called with Client*
     * pointers that were created by the corresponding createClient() function.
     * Passing a Client* created by createSecureClient() will cause undefined
     * behavior.
     * Always match create/delete pairs:
     * - createClient() -> deleteClient()
     * - createSecureClient() -> deleteSecureClient()
     *
     * @param client The client to delete
     */
    virtual void deleteClient(Client* client) = 0;

    /**@}*/


    /**
     * @anchor modem_ssl_client_functions
     * @name Functions for creating secured (TLS/SSL) clients
     */
    /**@{*/
    /**
     * @brief Create a new secure client object
     * @warning Be sure to delete this object when you're done with it!
     * @param mux Multiplexing channel to use, defaults to 0
     * @return A new secure client object
     */
    virtual Client* createSecureClient(uint8_t mux = 0) = 0;

    /**
     * @brief Create a new secure client object with certificate specification
     * and a multiplexing channel.
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param mux Multiplexing channel to use
     * @param sslAuthMode The SSL authentication mode to use
     * @param sslVersion The SSL version to use
     * @param CAcertName The name of the CA certificate to use
     * @param clientCertName The name of the client certificate to use
     * @param clientKeyName The name of the client key to use
     *
     * @return A new secure client object
     */
    virtual Client* createSecureClient(
        uint8_t mux, SSLAuthMode sslAuthMode,
        SSLVersion  sslVersion = SSLVersion::TLS1_2,
        const char* CAcertName = nullptr, const char* clientCertName = nullptr,
        const char* clientKeyName = nullptr) = 0;
    /**
     * @brief Create a new secure client object with certificate specification
     * and default multiplexing channel.
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
    virtual Client* createSecureClient(
        SSLAuthMode sslAuthMode, SSLVersion sslVersion = SSLVersion::TLS1_2,
        const char* CAcertName = nullptr, const char* clientCertName = nullptr,
        const char* clientKeyName = nullptr) = 0;

    /**
     * @brief Create a new secure client object with PSK credentials and a
     * multiplexing channel.
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param mux Multiplexing channel to use
     * @param pskIdent The pre-shared key identity
     * @param psKey The pre-shared key
     * @param sslVersion The SSL version to use
     *
     * @return A new secure client object
     */
    virtual Client* createSecureClient(
        uint8_t mux, const char* pskIdent, const char* psKey,
        SSLVersion sslVersion = SSLVersion::TLS1_2) = 0;
    /**
     * @brief Create a new secure client object with PSK credentials and default
     * multiplexing channel.
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param pskIdent The pre-shared key identity
     * @param psKey The pre-shared key
     * @param sslVersion The SSL version to use
     *
     * @return A new secure client object
     */
    virtual Client* createSecureClient(
        const char* pskIdent, const char* psKey,
        SSLVersion sslVersion = SSLVersion::TLS1_2) = 0;

    /**
     * @brief Create a new secure client object with PSK table name and a
     * multiplexing channel.
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param mux The multiplexing channel to use
     * @param pskTableName The pre-shared key table name - for modems that
     * require PSK's in a "table" format
     * @param sslVersion The SSL version to use
     *
     * @return A new secure client object
     */
    virtual Client* createSecureClient(
        uint8_t mux, const char* pskTableName,
        SSLVersion sslVersion = SSLVersion::TLS1_2) = 0;
    /**
     * @brief Create a new secure client object with PSK table name and default
     * multiplexing channel.
     * @warning Be sure to delete this object when you're done with it!
     *
     * @param pskTableName The pre-shared key table name - for modems that
     * require PSK's in a "table" format
     * @param sslVersion The SSL version to use
     *
     * @return A new secure client object
     */
    virtual Client* createSecureClient(
        const char* pskTableName,
        SSLVersion  sslVersion = SSLVersion::TLS1_2) = 0;

    /**
     * @brief Attempts to delete a created TinyGsmSecureClient object. We need
     * to do this to close memory leaks from the create client because we can't
     * delete the created client from a pointer to the parent because the
     * Arduino core's client class doesn't have a virtual destructor.
     *
     * @warning CRITICAL: This function MUST only be called with Client*
     * pointers that were created by the corresponding createSecureClient()
     * function. Passing a Client* created by createClient() will cause
     * undefined behavior.
     * Always match create/delete pairs:
     * - createClient() -> deleteClient()
     * - createSecureClient() -> deleteSecureClient()
     *
     * @param client The client to delete
     */
    virtual void deleteSecureClient(Client* client) = 0;
    /**@}*/


    /**
     * @anchor modem_time_sync_functions
     * @name Functions for NTP and NIST time synchronization
     * Functions for synchronizing the modem's internal time via NTP or NIST
     * time servers.
     */
    /**@{*/
    /**
     * @brief Get the time from NIST via NTP on those modems that support it or
     * using the NIST time protocol over TCP (RFC-868) for those that don't.
     *
     * @return Unix timestamp (seconds since Jan 1, 1970 UTC)
     */
    virtual uint32_t getNISTTime() = 0;
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
     * @brief Get the signal quality from the modem
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
    virtual bool getModemSignalQuality(int16_t& rssi, int16_t& percent) = 0;

    /**
     * @brief Get the modem's battery information  - this may or may not be a
     * valid values depending on the module and breakout.
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
                                      int16_t& milliVolts) = 0;
    /**
     * @brief Get the current temperature provided by the modem module.
     *
     * @remark This is **not** a measurement of the ambient temperature, it only
     * reflects the temperature of the modem chip itself.  This temperature is
     * expected to be above ambient temperature.
     *
     * @return The modem temperature in degrees Celsius
     */
    virtual float getModemChipTemperature() = 0;
    /**@}*/


    /**
     * @anchor modem_polling_functions
     * @name Modem polling functions
     * Functions to enable or disable polling for modem metadata - using the
     * modem like a sensor.
     */
    /**@{*/
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
    void enableMetadataPolling(uint8_t pollingBitmask);

    /**
     * @brief Disables metadata polling for one or more modem measured
     * variables.  Setting this to 0b11111111 will disable polling for all
     * modem measured variables.
     *
     * @param pollingBitmask The bitmask indicating which parameters to disable.
     *
     * @see loggerModem::_pollModemMetaData
     *
     * @note This will **not** enable polling for any unset bits in the provided
     * bitmask.  It will only disable polling for those bits that are set.
     */
    void disableMetadataPolling(uint8_t pollingBitmask);

    /**
     * @brief Sets the complete bitmask for modem metadata polling.
     *
     * This will enable polling for 1 bits and disable polling for 0 bits.
     * Setting this to 0 (0b00000000) will disable polling for all metadata
     * parameters.  Setting it to 255 (0b11111111) will enable polling for all
     * parameters.
     *
     * @param pollingBitmask The new polling bitmask.
     *
     * @see loggerModem::_pollModemMetaData
     */
    void setMetadataPolling(uint8_t pollingBitmask);
    /**
     * @brief Update all stored modem metadata.
     *
     * This polls the modem for signal quality, battery stats, and chip
     * temperature based on the configured polling bitmask. The results are
     * stored in static member variables for later retrieval by the static
     * getter functions.
     *
     * @return True if all requested metadata was successfully polled; false
     * otherwise
     */
    virtual bool updateModemMetadata();
 protected:
    /**
     * @brief Polling mask for modem metadata
     *
     * An 8-bit code for the enabled modem polling variables
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
    /**@}*/

    /**
     * @anchor modem_static_functions
     * @name Functions to return the current value of static member variables
     *
     * These functions do **NOT** query the modem for new values, they return
     * the stored value from the last poll.
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
    static float getModemRSSI();

    /**
     * @brief Get the stored modem signal strength as a percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored signal strength as a percent
     */
    static float getModemSignalPercent();

    /**
     * @brief Get the stored modem battery charge state.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery charge state
     */
    static float getModemBatteryChargeState();

    /**
     * @brief Get the stored modem battery charge percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery charge percent
     */
    static float getModemBatteryChargePercent();

    /**
     * @brief Get the stored modem battery voltage.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored battery voltage in mV
     */
    static float getModemBatteryVoltage();

    /**
     * @brief Get the stored modem temperature.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return The stored temperature in degrees Celsius
     */
    static float getModemTemperature();
    /**@}*/

    // NOTE:  These must be static so that the modem variables can call the
    // member functions that return them.  (Non-static member functions cannot
    // be called without an object.)
    /**
     * @anchor modem_static_variables
     * @name Static member variables used to hold modem metadata
     */
    /**@{*/
 protected:
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

// cSpell:ignore modemBatterymV
