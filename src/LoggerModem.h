/*
 *LoggerModem.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *Copyright 2020 Stroud Water Research Center
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file wraps the tinyGSM library:  https://github.com/vshymanskyy/TinyGSM
 *and adds in the power functions to turn the modem on and off.
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
class loggerModem {
 public:
    /**
     * @brief Construct a new logger Modem object
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
     * @brief Destroy the logger Modem object - no action taken
     */
    virtual ~loggerModem();

    /**
     * @brief Sets an LED to turn on when the modem is on
     *
     * @param modemLEDPin the digital PIN number for the LED
     */
    void setModemLED(int8_t modemLEDPin);

    /**
     * @brief Merely returns the modem name
     *
     * @return String The modem name
     */
    String getModemName(void);

    /**
     * @brief Sets up the modem before first use
     *
     * @return true Setup was successful
     * @return false Setup was not successful
     */
    virtual bool modemSetup(void);
    /**
     * @brief Sets up the modem before first use
     * This is an overload only for backwards compatibility
     *
     * @return true Setup was successful
     * @return false Setup was not successful
     */
    bool setup(void) {
        return modemSetup();
    }

    /**
     * @brief Does whatever is needed to prepare the modem to connect to the
     * internet.  Includes powering up the modem if necessary.
     *
     * @return true Wake was sucessful, modem should be ready
     * @return false Wake failed, modem is probably not able to communicate
     */
    virtual bool modemWake(void) = 0;
    /**
     * @brief Does whatever is needed to prepare the modem to connect to the
     * internet.  This is an overload for backwards compatibility.
     *
     * @return true Wake was sucessful, modem should be ready
     * @return false Wake failed, modem is probably not able to communicate
     */
    bool wake(void) {
        return modemWake();
    }

    /**
     * @brief sets whatever pin is defined for modem power in the constructor
     * high
     */
    virtual void modemPowerUp(void);
    /**
     * @brief sets whatever pin is defined for modem power in the constructor
     * low.
     *
     * NOTE:  modemPowerDown() simply kills power, while modemSleepPowerDown()
     * allows for graceful shut down.  You should use modemSleepPowerDown()
     * whenever possible.
     */
    virtual void modemPowerDown(void);
    /**
     * @brief requests that the modem enter its lowest possible power state
     *
     * @return true The modem has sucessfully entered low power state
     * @return false The modem didn't enter low power state successfully
     */
    virtual bool modemSleep(void);
    /**
     * @brief requests that the modem enter its lowest possible power state and
     * then sets the power pin low after the modem has indicated successfully
     * going to low power.
     *
     * @return true The modem has sucessfully entered low power state and then
     * powered off
     * @return false The modem didn't enter low power state successfully
     */
    virtual bool modemSleepPowerDown(void);

    /**
     * @brief Uses the modem reset pin specified in the constructor to signal
     * the modem for a "hard" or "panic" reset.
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
     * @brief Sets what pin level is to be expected when the on the modem status
     * pin when the modem is active.
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
     * @brief Waits for the modem to successfully register on the cellular
     * network and then requests that it establish either EPS or GPRS data
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
     * @brief Detatches from EPS or GPRS data connection and then deregisters
     * from the cellular network.
     */
    virtual void disconnectInternet(void) = 0;

    // Get the time from NIST via TIME protocol (rfc868)
    // This would be much more efficient if done over UDP, but I'm doing it
    // over TCP because I don't have a UDP library for all the modems.
    // NOTE:  The return is the number of seconds since Jan 1, 1970 IN UTC
    virtual uint32_t getNISTTime(void) = 0;

    // Get modem metadata values
    // NOTE:  In order to use the modem variables called below, these functions
    // must be called when the modem is connected to the internet.  Normally
    // this is done by the "logDataAndPublish()" function of the logger, but
    // if "handwriting" a logging function, remember to call this.
    // These three functions will query the modem to get new values
    virtual bool  getModemSignalQuality(int16_t& rssi, int16_t& percent) = 0;
    virtual bool  getModemBatteryStats(uint8_t& chargeState, int8_t& percent,
                                       uint16_t& milliVolts)             = 0;
    virtual float getModemChipTemperature(void)                          = 0;
    // This gets all of the above at once
    virtual bool updateModemMetadata(void);

    // These functions simply return the stored values
    // NOTE:  These must be static so that the modem variables can call them.
    // (Non-static member functions cannot be called without an object.)
    static float getModemRSSI();
    static float getModemSignalPercent();
    static float getModemBatteryChargeState();
    static float getModemBatteryChargePercent();
    static float getModemBatteryVoltage();
    static float getModemTemperature();
    // static float getModemActivationDuration();
    // static float getModemPoweredDuration();

 protected:
    // Helper to get approximate RSSI from CSQ (assuming no noise)
    static int16_t getRSSIFromCSQ(int16_t csq);
    // Helper to get signal percent from CSQ
    static int16_t getPctFromCSQ(int16_t csq);
    // Helper to get signal percent from CSQ
    static int16_t getPctFromRSSI(int16_t rssi);

    // Other helper functions
    void         modemLEDOn(void);
    void         modemLEDOff(void);
    virtual void setModemPinModes(void);

    virtual bool isInternetAvailable(void) = 0;
    virtual bool modemSleepFxn(void)       = 0;
    virtual bool modemWakeFxn(void)        = 0;
    virtual bool extraModemSetup(void)     = 0;
    virtual bool isModemAwake(void)        = 0;

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
