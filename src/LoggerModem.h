/*
 *LoggerModem.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file wraps the tinyGSM library:  https://github.com/vshymanskyy/TinyGSM
 *and adds in the power functions to turn the modem on and off.
 */

// Header Guards
#ifndef LoggerModem_h
#define LoggerModem_h

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
    // Constructor/Destructor
    loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                int8_t modemResetPin, bool resetLevel, uint32_t resetPulse_ms,
                int8_t modemSleepRqPin, bool wakeLevel, uint32_t wakePulse_ms,
                uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                uint32_t wakeDelayTime_ms, uint32_t max_atresponse_time_ms);
    virtual ~loggerModem();

    // Sets an LED to turn on when the modem is on
    void setModemLED(int8_t modemLEDPin);

    // Merely returns the modem name
    String getModemName(void);

    // Sets up the modem before first use
    virtual bool modemSetup(void);
    bool         setup(void) {
        return modemSetup();
    }
    virtual bool modemWake(void) = 0;
    bool         wake(void) {
        return modemWake();
    }

    // Note:  modemPowerDown() simply kills power, while modemSleepPowerDown()
    // allows for graceful shut down.  You should use modemSleepPowerDown()
    // whenever possible.
    virtual void modemPowerUp(void);
    virtual void modemPowerDown(void);
    virtual bool modemSleep(void);
    virtual bool modemSleepPowerDown(void);
    virtual bool modemHardReset(void);
    // Options to change pin levels from the "standard" for the modem
    void setModemStatusLevel(bool level);
    void setModemWakeLevel(bool level);
    void setModemResetLevel(bool level);

    // Access the internet
    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) = 0;
    virtual void disconnectInternet(void)                             = 0;

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
    Modem_RSSI(loggerModem* parentModem, const char* uuid = "",
               const char* varCode = "RSSI")
        : Variable(&parentModem->getModemRSSI, (uint8_t)MODEM_RSSI_RESOLUTION,
                   &*"RSSI", &*"decibelMiliWatt", varCode, uuid) {}
    ~Modem_RSSI() {}
};


// Defines the Signal Percentage
class Modem_SignalPercent : public Variable {
 public:
    Modem_SignalPercent(loggerModem* parentModem, const char* uuid = "",
                        const char* varCode = "signalPercent")
        : Variable(&parentModem->getModemSignalPercent,
                   (uint8_t)MODEM_PERCENT_SIGNAL_RESOLUTION, &*"signalPercent",
                   &*"percent", varCode, uuid) {}
    ~Modem_SignalPercent() {}
};


// Defines the Battery Charge State
class Modem_BatteryState : public Variable {
 public:
    Modem_BatteryState(loggerModem* parentModem, const char* uuid = "",
                       const char* varCode = "modemBatteryCS")
        : Variable(&parentModem->getModemBatteryChargeState,
                   (uint8_t)MODEM_BATTERY_STATE_RESOLUTION,
                   &*"batteryChargeState", &*"number", varCode, uuid) {}
    ~Modem_BatteryState() {}
};


// Defines the Battery Charge Percent
class Modem_BatteryPercent : public Variable {
 public:
    Modem_BatteryPercent(loggerModem* parentModem, const char* uuid = "",
                         const char* varCode = "modemBatteryPct")
        : Variable(&parentModem->getModemBatteryChargePercent,
                   (uint8_t)MODEM_BATTERY_PERCENT_RESOLUTION,
                   &*"batteryVoltage", &*"percent", varCode, uuid) {}
    ~Modem_BatteryPercent() {}
};


// Defines the Battery Voltage
class Modem_BatteryVoltage : public Variable {
 public:
    Modem_BatteryVoltage(loggerModem* parentModem, const char* uuid = "",
                         const char* varCode = "modemBatterymV")
        : Variable(&parentModem->getModemBatteryVoltage,
                   (uint8_t)MODEM_BATTERY_VOLT_RESOLUTION, &*"batteryVoltage",
                   &*"millivolt", varCode, uuid) {}
    ~Modem_BatteryVoltage() {}
};


// Defines the Temperature Sensor on the modem (not all modems have one)
class Modem_Temp : public Variable {
 public:
    Modem_Temp(loggerModem* parentModem, const char* uuid = "",
               const char* varCode = "modemTemp")
        : Variable(&parentModem->getModemTemperature,
                   (uint8_t)MODEM_TEMPERATURE_RESOLUTION, &*"temperature",
                   &*"degreeCelsius", varCode, uuid) {}
    ~Modem_Temp() {}
};


// // Defines a diagnostic variable for how long the modem was last active
// class Modem_ActivationDuration : public Variable
// {
// public:
//     Modem_ActivationDuration(loggerModem *parentModem,
//                              const char *uuid = "",
//                              const char *varCode = "modemActiveSec")
//         : Variable(&parentModem->getModemActivationDuration,
//                    (uint8_t)MODEM_ACTIVATION_RESOLUTION,
//                    &*"timeElapsed", &*"second",
//                    varCode, uuid)
//     {}
//     ~Modem_ActivationDuration(){}
// };


// // Defines a diagnostic variable for how long the modem was last active
// class Modem_PoweredDuration : public Variable
// {
// public:
//     Modem_PoweredDuration(loggerModem *parentModem,
//                           const char *uuid = "",
//                           const char *varCode = "modemPoweredSec")
//         : Variable(&parentModem->getModemPoweredDuration,
//                    (uint8_t)MODEM_POWERED_RESOLUTION,
//                    &*"timeElapsed", &*"second",
//                    varCode, uuid)
//     {}
//     ~Modem_PoweredDuration(){}
// };

// #include <LoggerModem.tpp>
#endif  // Header Guard
