/**
 * @file LoggerModem.h
 * @brief Contains the loggerModem class which wraps the TinyGSM library and
 * adds in the power functions to turn the modem on and off.
 *
 * TinyGSM is available here:  https://github.com/vshymanskyy/TinyGSM
 *
 * This also contains the variable subclasses Modem_RSSI, Modem_SignalPercent,
 * Modem_BatteryState, Modem_BatteryPercent, and Modem_BatteryVoltage - all of
 * which are implentend as "calculated" variables.
 *
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @copyright 2020 Stroud Water Research Center
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
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

#define MODEM_RSSI_RESOLUTION 0
#define MODEM_PERCENT_SIGNAL_RESOLUTION 0
#define MODEM_BATTERY_STATE_RESOLUTION 0
#define MODEM_BATTERY_PERCENT_RESOLUTION 0
#define MODEM_BATTERY_VOLT_RESOLUTION 0
#define MODEM_TEMPERATURE_RESOLUTION 1
// #define MODEM_ACTIVATION_RESOLUTION 3
// #define MODEM_POWERED_RESOLUTION 3

/* ===========================================================================
 * Functions for the modem class
 * This is basically a wrapper for TinyGsm with power control added
 * ========================================================================= */

// template <class Derived, typename modemType, typename modemClientType>
/**
 * @brief The "loggerModem" class provides an internet connection for the logger
 * and supplies and Arduino "Client" instance to use to publish data.
 */
class loggerModem {
 public:
    /**
     * @brief Construct a new logger Modem object.
     *
     * @param powerPin The digital pin number of the pin suppling power to the
     * modem (active HIGH)
     * @param statusPin The digital pin number of a pin indicating modem status
     * @param statusLevel The level of the status pin (0/LOW or 1/HIGH) when the
     * modem is active
     * @param modemResetPin The digital pin number of a pin for hard or panic
     * resets
     * @param resetLevel The level on the modemResetPin which will reset the
     * modem
     * @param resetPulse_ms The length of time in ms required to reset the modem
     * using a panic reset
     * @param modemSleepRqPin The digital pin number of a pin used to request
     * the modem enter its lowest possible power state
     * @param wakeLevel The level the modemSleepRqPin should be set to to *wake*
     * the modem
     * @param wakePulse_ms The length of time the modemSleepRqPin should be held
     * at the wakeLevel in order to wake the modem.  Set to 0 if the pin must be
     * continuously held to the level to keep the modem at fully functional
     * state.
     * @param max_status_time_ms The maximum length of time in milliseconds
     * between when power is supplied to the modem and when the statusPin should
     * reach the statusLevel.
     * @param max_disconnetTime_ms The maximum length of time in milliseconds
     * between when the modem is requested to enter lowest power state and when
     * it should have completed necessary steps to shut down.
     * @param wakeDelayTime_ms The time in milliseconds between when power is
     * supplied to the modem and when the wake function can be used to request
     * the modem enter fully power operation.
     * @param max_atresponse_time_ms The maximum length of time in milliseconds
     * between when the modem has been requested to wake and when it begins
     * responding to commands over the main serial connection.
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
     * @brief Set an LED to turn on when the modem is on.
     *
     * @param modemLEDPin the digital PIN number for the LED
     */
    void setModemLED(int8_t modemLEDPin);

    /**
     * @brief Get the modem name.
     *
     * @return String The modem name
     */
    String getModemName(void);

    /**
     * @brief Set up the modem before first use.
     *
     * This is used for operations that cannot happen in the modem constructor -
     * they must happen at run time, not at compile time.
     *
     * @return true Setup was successful
     * @return false Setup was not successful
     */
    virtual bool modemSetup(void);
    /**
     * @brief Retained for backwards compatibility.
     *
     * @deprecated use modemSetup()
     *
     * @return true Setup was successful
     * @return false Setup was not successful
     */
    bool setup(void) {
        return modemSetup();
    }

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
     * @return true The modem is responsive and ready for action.
     * @return false The modem is not responding to commands as expected.
     */
    virtual bool modemWake(void) = 0;
    /**
     * @brief Retained for backwards compatibility.
     *
     * @deprecated use modemWake()
     *
     * @return true Wake was sucessful, modem should be ready
     * @return false Wake failed, modem is probably not able to communicate
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
     * NOTE:  modemPowerDown() simply kills power, while modemSleepPowerDown()
     * allows for graceful shut down.  You should use modemSleepPowerDown()
     * whenever possible.
     */
    virtual void modemPowerDown(void);
    /**
     * @brief Request that the modem enter its lowest possible power state.
     *
     * @return true The modem has sucessfully entered low power state
     * @return false The modem didn't enter low power state successfully
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
     * @return true The modem has sucessfully entered low power state and then
     * powered off
     * @return false The modem didn't enter low power state successfully
     */
    virtual bool modemSleepPowerDown(void);

    /**
     * @brief Use the modem reset pin specified in the constructor to perform a
     * "hard" or "panic" reset.
     *
     * This should only be used if the modem is clearly non-responsive.
     *
     * @return true The reset succeeded and the modem should now be responsive
     * @return false The modem remains non-responsive either because the reset
     * failed to fix the communication issue or because a reset is not possible
     * with the current pin/modem configuration.
     */
    virtual bool modemHardReset(void);


    /**
     * @brief Set the pin level to be expected when the on the modem status pin
     * when the modem is active.
     *
     * If this function is not called, the modem status pin is assumed to
     * exactly follow the hardware specifications for that modems raw cellular
     * component.
     *
     * @param level The active level of the pin (0/LOW or 1/HIGH)
     */
    void setModemStatusLevel(bool level);

    /**
     * @brief Set the pin level to be used to wake the modem.
     *
     * If this function is not called, the modem status pin is assumed to
     * exactly follow the hardware specifications for that modems raw cellular
     * component.
     *
     * @param level The pin level (0/LOW or 1/HIGH) of the pin while waking the
     * modem.
     */
    void setModemWakeLevel(bool level);

    /**
     * @brief Set the pin level to be used to reset the modem.
     *
     * If this function is not called, the modem status pin is assumed to
     * exactly follow the hardware specifications for that modems raw cellular
     * component - nearly always low.
     *
     * @param level The pin level (0/LOW or 1/HIGH) of the pin while resetting
     * the modem.
     */
    void setModemResetLevel(bool level);

    /**
     * @brief Wait for the modem to successfully register on the cellular
     * network and then request that it establish either EPS or GPRS data
     * connection.
     *
     * @param maxConnectionTime The maximum length of time in milliseconds to
     * wait for network registration and data sconnection.  Defaults to 50,000ms
     * (50s).
     * @return true EPS or GPRS data connection has been established
     * @return false The modem was either unable to communicated, unable to
     * register with the cellular network, or unable to establish a EPS or GPRS
     * connection.
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
     * @return uint32_t The number of seconds since Jan 1, 1970 IN UTC
     */
    virtual uint32_t getNISTTime(void) = 0;


    // Get modem metadata values
    // NOTE:  In order to use the modem variables called below, these functions
    // must be called when the modem is connected to the internet.  Normally
    // this is done by the "logDataAndPublish()" function of the logger, but
    // if "handwriting" a logging function, remember to call this.
    // These three functions will query the modem to get new values
    /**
     * @brief Query the modem for the current signal quality and write the
     * results to the supplied non-constant references.
     *
     * @param rssi A reference to an int16_t which will be set with the received
     * signal strength indicator
     * @param percent A reference to an int16_t which will be set with the
     * "percent" signal strength
     * @return true indicates that the communication with the modem was
     * successful and the values referenced by the pointers should be valid
     * @return false indicates that communication with the modem failed
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
     * @return true indicates that the communication with the modem was
     * successful and the values referenced by the pointers should be valid
     * @return false indicates that communication with the modem failed
     */
    virtual bool getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                                      uint16_t& milliVolts) = 0;
    /**
     * @brief Get the current temperature provided by the modem module.
     *
     * @return float The temperature in degrees Celsius
     */
    virtual float getModemChipTemperature(void) = 0;

    /**
     * @brief Query the modem for signal quality, battery, and temperature
     * information and store the values to the static internal variables.
     *
     * @return true indicates that the communication with the modem was
     * successful and the values of the internal static variables should be
     * valid.
     * @return false indicates that communication with the modem failed
     */
    virtual bool updateModemMetadata(void);

    // These functions simply return the stored values
    // NOTE:  These must be static so that the modem variables can call them.
    // (Non-static member functions cannot be called without an object.)
    /**
     * @brief Get the stored Modem RSSI value.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return float The stored RSSI
     */
    static float getModemRSSI();

    /**
     * @brief Get the stored modem signal strength as a percent.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return float The stored signal strength
     */
    static float getModemSignalPercent();

    /**
     * @brief Get the stored modem battery charge state.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return float The stored signal percent
     */
    static float getModemBatteryChargeState();

    /**
     * @brief Get the stored modem battery charge percent object.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return float The stored battery charge percent
     */
    static float getModemBatteryChargePercent();

    /**
     * @brief Get the stored modem battery voltage.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return float The stored battery voltage in mV
     */
    static float getModemBatteryVoltage();

    /**
     * @brief Get the stored modem temperature.
     *
     * @note Does NOT query the modem for a new value.
     *
     * @return float The stored temperature in degrees Celsius
     */
    static float getModemTemperature();

    // static float getModemActivationDuration();
    // static float getModemPoweredDuration();

 protected:
    /**
     * @brief Get approximate RSSI from CSQ (assuming no noise).
     *
     * The RSSI is estimated from a look-up assuming no noise.
     *
     * @param csq A "CSQ" (0-31) signal qualilty
     * @return int16_t An RSSI in dBm, making assumptions about the conversion
     */
    static int16_t getRSSIFromCSQ(int16_t csq);
    /**
     * @brief Get signal percent from CSQ.
     *
     * The percent is grabbed from a look-up.
     *
     * @param csq A "CSQ" (0-31) signal qualilty
     * @return int16_t The percent of maximum signal strength.
     */
    static int16_t getPctFromCSQ(int16_t csq);
    /**
     * @brief Get signal percent from CSQ.
     *
     * @param rssi The RSSI in dBm.
     * @return int16_t The estimated percent of maximum signal strength.
     */
    static int16_t getPctFromRSSI(int16_t rssi);

    // Other helper functions
    /**
     * @brief Turn on the modem LED/alert pin - sets it HIGH
     */
    void modemLEDOn(void);
    /**
     * @brief Turn off the modem LED/alert pin - sets it LOW
     */
    void modemLEDOff(void);
    /**
     * @brief Set the processor pin modes (input vs output, with and without
     * pullup) for all pins connected between the modem module and the mcu.
     */
    virtual void setModemPinModes(void);

    /**
     * @brief Check whether there is an active internet connection available.
     *
     * @return true
     * @return false
     */
    virtual bool isInternetAvailable(void) = 0;
    /**
     * @brief Perform the parts of the modem sleep process that are unique to a
     * specific module, as opposed to the parts of setup that are common to all
     * modem modules.
     *
     * @return true The unique part of the sleep function ran sucessfully.
     * @return false The extra sleep failed.
     */
    virtual bool modemSleepFxn(void) = 0;
    /**
     * @brief Perform the parts of the modem wake up process that are unique to
     * a specific module, as opposed to the parts of setup that are common to
     * all modem modules.
     *
     * @return true The unique part of the wake function ran sucessfully - does
     * NOT indicated that the modem is now responsive.
     * @return false The extra wake failed.
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
     * @return true The extra setup succeeded.
     * @return false The extra setup failed.
     */
    virtual bool extraModemSetup(void) = 0;
    /**
     * @brief Check if the modem was awake using all possible means.
     *
     * If possible, we always want to check if the modem was awake before
     * attempting to wake it up. Most cellular modules are woken and put to
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
     * @return true The modem is already awake.
     * @return false The modem appears to be asleep or non-responsive.
     */
    virtual bool isModemAwake(void) = 0;

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
     * @return uint32_t the number of seconds since January 1, 1970 00:00:00 UTC
     */
    static uint32_t parseNISTBytes(byte nistBytes[4]);

    // Helper to set the timing for specific cellular chipsets based on their
    // documentation void setModemTiming(void);

    int8_t   _powerPin;
    int8_t   _statusPin;
    bool     _statusLevel;
    int8_t   _modemResetPin;
    bool     _resetLevel;
    uint32_t _resetPulse_ms;
    int8_t   _modemSleepRqPin;
    bool     _wakeLevel;
    uint32_t _wakePulse_ms;
    int8_t   _modemLEDPin;

    uint32_t _statusTime_ms;
    uint32_t _disconnetTime_ms;
    uint32_t _wakeDelayTime_ms;
    uint32_t _max_atresponse_time_ms;

    uint32_t _millisPowerOn;
    uint32_t _lastNISTrequest;
    bool     _hasBeenSetup;
    bool     _pinModesSet;

    // NOTE:  These must be static so that the modem variables can call the
    // member functions that return them.  (Non-static member functions cannot
    // be called without an object.)
    static int16_t _priorRSSI;
    static int16_t _priorSignalPercent;
    static float   _priorModemTemp;
    static float   _priorBatteryState;
    static float   _priorBatteryPercent;
    static float   _priorBatteryVoltage;
    // static float _priorActivationDuration;
    // static float _priorPoweredDuration;

    String _modemName;

    // modemType gsmModem;
    // modemClientType gsmClient;
};

// typedef float (loggerModem::*loggerGetValueFxn)(void);

// Classes for the modem variables
// NOTE:  The modem is NOT set up as a sensor.  ALl of these variables for
// the modem object are actually being called as calculated variables where the
// calculation function is to ask the modem object for the values from the
// last time it connected to the internet.

// Defines the received signal strength indication
class Modem_RSSI : public Variable {
 public:
    explicit Modem_RSSI(loggerModem* parentModem, const char* uuid = "",
                        const char* varCode = "RSSI")
        : Variable(&parentModem->getModemRSSI, (uint8_t)MODEM_RSSI_RESOLUTION,
                   &*"RSSI", &*"decibelMiliWatt", varCode, uuid) {}
    ~Modem_RSSI() {}
};


// Defines the Signal Percentage
class Modem_SignalPercent : public Variable {
 public:
    explicit Modem_SignalPercent(loggerModem* parentModem,
                                 const char*  uuid    = "",
                                 const char*  varCode = "signalPercent")
        : Variable(&parentModem->getModemSignalPercent,
                   (uint8_t)MODEM_PERCENT_SIGNAL_RESOLUTION, &*"signalPercent",
                   &*"percent", varCode, uuid) {}
    ~Modem_SignalPercent() {}
};


// Defines the Battery Charge State
class Modem_BatteryState : public Variable {
 public:
    explicit Modem_BatteryState(loggerModem* parentModem, const char* uuid = "",
                                const char* varCode = "modemBatteryCS")
        : Variable(&parentModem->getModemBatteryChargeState,
                   (uint8_t)MODEM_BATTERY_STATE_RESOLUTION,
                   &*"batteryChargeState", &*"number", varCode, uuid) {}
    ~Modem_BatteryState() {}
};


// Defines the Battery Charge Percent
class Modem_BatteryPercent : public Variable {
 public:
    explicit Modem_BatteryPercent(loggerModem* parentModem,
                                  const char*  uuid    = "",
                                  const char*  varCode = "modemBatteryPct")
        : Variable(&parentModem->getModemBatteryChargePercent,
                   (uint8_t)MODEM_BATTERY_PERCENT_RESOLUTION,
                   &*"batteryVoltage", &*"percent", varCode, uuid) {}
    ~Modem_BatteryPercent() {}
};


// Defines the Battery Voltage
class Modem_BatteryVoltage : public Variable {
 public:
    explicit Modem_BatteryVoltage(loggerModem* parentModem,
                                  const char*  uuid    = "",
                                  const char*  varCode = "modemBatterymV")
        : Variable(&parentModem->getModemBatteryVoltage,
                   (uint8_t)MODEM_BATTERY_VOLT_RESOLUTION, &*"batteryVoltage",
                   &*"millivolt", varCode, uuid) {}
    ~Modem_BatteryVoltage() {}
};


// Defines the Temperature Sensor on the modem (not all modems have one)
class Modem_Temp : public Variable {
 public:
    explicit Modem_Temp(loggerModem* parentModem, const char* uuid = "",
                        const char* varCode = "modemTemp")
        : Variable(&parentModem->getModemTemperature,
                   (uint8_t)MODEM_TEMPERATURE_RESOLUTION, &*"temperature",
                   &*"degreeCelsius", varCode, uuid) {}
    ~Modem_Temp() {}
};


// // Defines a diagnostic variable for how long the modem was last active
// class Modem_ActivationDuration : public Variable {
//  public:
//     explicit Modem_ActivationDuration(loggerModem* parentModem,
//                                       const char*  uuid    = "",
//                                       const char*  varCode =
//                                       "modemActiveSec")
//         : Variable(&parentModem->getModemActivationDuration,
//                    (uint8_t)MODEM_ACTIVATION_RESOLUTION, &*"timeElapsed",
//                    &*"second", varCode, uuid) {}
//     ~Modem_ActivationDuration() {}
// };


// // Defines a diagnostic variable for how long the modem was last active
// class Modem_PoweredDuration : public Variable {
//  public:
//     explicit Modem_PoweredDuration(loggerModem* parentModem,
//                                    const char*  uuid    = "",
//                                    const char*  varCode = "modemPoweredSec")
//         : Variable(&parentModem->getModemPoweredDuration,
//                    (uint8_t)MODEM_POWERED_RESOLUTION, &*"timeElapsed",
//                    &*"second", varCode, uuid) {}
//     ~Modem_PoweredDuration() {}
// };

// #include <LoggerModem.tpp>
#endif  // SRC_LOGGERMODEM_H_
