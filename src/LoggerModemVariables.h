/**
 * @file LoggerModemVariables.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 */

// Header Guards
#ifndef SRC_LOGGERMODEMVARIABLES_H_
#define SRC_LOGGERMODEMVARIABLES_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_LOGGERMODEMVARIABLES_DEBUG
#define MS_DEBUGGING_STD "LoggerModemVariables"
#endif
#ifdef MS_LOGGERMODEMVARIABLES_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "LoggerModemVariables"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "VariableBase.h"

#include <Arduino.h>


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


#endif  // SRC_LOGGERMODEMVARIABLES_H_

// cSpell:ignore bpercent modemBatterymV
