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
#include "SensorBase.h"
#include <Arduino.h>


#define MODEM_NUM_VARIABLES 6

#define MODEM_RSSI_VAR_NUM 0
#define MODEM_RSSI_RESOLUTION 0

#define MODEM_PERCENT_SIGNAL_VAR_NUM 1
#define MODEM_PERCENT_SIGNAL_RESOLUTION 0

#define MODEM_BATTERY_STATE_VAR_NUM 2
#define MODEM_BATTERY_STATE_RESOLUTION 0

#define MODEM_BATTERY_PERCENT_VAR_NUM 3
#define MODEM_BATTERY_PERCENT_RESOLUTION 0

#define MODEM_BATTERY_VOLT_VAR_NUM 4
#define MODEM_BATTERY_VOLT_RESOLUTION 0

#define MODEM_TEMPERATURE_VAR_NUM 5
#define MODEM_TEMPERATURE_RESOLUTION 1

/* ===========================================================================
* Functions for the modem class
* This is basically a wrapper for TinyGsm
* ========================================================================= */

class loggerModem : public Sensor
{
// ==========================================================================//
//          These are the functions that set the modem up as a sensor
// ==========================================================================//
public:
    // Constructor/Destructor
    loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                int8_t modemResetPin, int8_t modemSleepRqPin, bool alwaysRunWake,
                uint32_t max_status_time_ms, uint32_t max_disconnetTime_ms,
                uint32_t max_warmUpTime_ms, uint32_t max_atresponse_time_ms,
                uint32_t max_signalQuality_time_ms,
                uint8_t measurementsToAverage = 1);
    virtual ~loggerModem();

    String getSensorName(void) override;

    virtual bool setup(void) override;
    virtual bool wake(void) override;
    virtual bool addSingleMeasurementResult(void);

    // Do NOT turn the modem on and off with the regular power up and down!
    // This is because when it is run in an array with other sensors, we will
    // generally want the modem to remain on after all the other sensors have
    // gone to sleep and powered down so the modem can send out data
    void powerUp(void) override;
    void powerDown(void) override;

protected:
    // We override these because the modem can tell us if it's ready or not

    // The modem is "stable" when it responds to AT commands.
    // For a WiFi modem, this actually sets the network connection parameters!!
    virtual bool isStable(bool debug=false) override;

    // This checks to see if enough time has passed for measurement completion
    // In the case of the modem, we consider a measurement to be "complete" when
    // the modem has registered on the network *and* returns good signal strength.
    // In theory, both of these things happen at the same time - as soon as the
    // module detects a network with sufficient signal strength, it connects and
    // will respond corretly to requests for its connection status and the signal
    // strength.  In reality sometimes the modem might respond with successful
    // network connection before it responds with a valid signal strength or it
    // might be able to return a real measurement of cellular signal strength but
    // not be able to register to the network.  We'd prefer to wait until it both
    // responses are good so we're getting an actual signal strength and it's as
    // close as possible to what the antenna is will see when the data publishers
    // push data.
    virtual bool isMeasurementComplete(bool debug = false) override;


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//
public:

    // Sets an LED to turn on when the modem is on
    void setModemLED(int8_t modemLEDPin);

    // Access the internet
    virtual bool connectInternet(uint32_t maxConnectionTime = 50000L) = 0;
    virtual void disconnectInternet(void) = 0;

    // Get values by other names
    virtual bool getModemSignalQuality(int16_t &rssi, int16_t &percent) = 0;
    virtual bool getModemBatteryStats(uint8_t &chargeState, int8_t &percent, uint16_t &milliVolts) = 0;
    virtual float getModemTemperature(void) = 0;

    // This has the same functionality as Client->connect with debugging text
    // int16_t openTCP(const char *host, uint16_t port);
    // This has the same functionality as Client->connect with debugging text
    // int16_t openTCP(IPAddress ip, uint16_t port);
    // This has the same functionality as Client->close with debugging text
    // void closeTCP(void);
    // Special sleep and power function for the modem
    void modemPowerUp(void);
    bool modemSleepPowerDown(void);

    // Get the time from NIST via TIME protocol (rfc868)
    // This would be much more efficient if done over UDP, but I'm doing it
    // over TCP because I don't have a UDP library for all the modems.
    virtual uint32_t getNISTTime(void) = 0;

protected:
    // Helper to get approximate RSSI from CSQ (assuming no noise)
    static int16_t getRSSIFromCSQ(int16_t csq);
    // Helper to get signal percent from CSQ
    static int16_t getPctFromCSQ(int16_t csq);
    // Helper to get signal percent from CSQ
    static int16_t getPctFromRSSI(int16_t rssi);

    // Other helper functions
    void modemLEDOn(void);
    void modemLEDOff(void);
    virtual bool didATRespond(void) = 0;
    virtual bool isInternetAvailable(void) = 0;
    virtual bool verifyMeasurementComplete(bool debug = false) = 0;
    virtual bool modemSleepFxn(void) = 0;
    virtual bool modemWakeFxn(void) = 0;
    virtual bool extraModemSetup(void) = 0;

    // Helper to set the timing for specific cellular chipsets based on their documentation
    // void setModemTiming(void);

protected:

    int8_t _modemSleepRqPin;
    int8_t _modemResetPin;
    int8_t _modemLEDPin;

    // THis denotes whether we should run the wake/sleep functions regardless
    // of the state of the status pins or if we should check the status pin first.
    bool _alwaysRunWake;

    bool _statusLevel;
    uint32_t _statusTime_ms;
    uint32_t _disconnetTime_ms;

    uint32_t _lastNISTrequest;
    uint32_t _lastATCheck;
    uint32_t _lastConnectionCheck;
    String _modemName;

};



// Classes for the modem variables
// Defines the received signal strength indication
class Modem_RSSI : public Variable
{
public:
    Modem_RSSI(Sensor *parentSense,
               const char *uuid = "",
               const char *varCode = "RSSI")
      : Variable(parentSense,
                 (const uint8_t)MODEM_RSSI_VAR_NUM,
                 (uint8_t)MODEM_RSSI_RESOLUTION,
                 "RSSI", "decibelMiliWatt",
                 varCode, uuid)
    {}
    Modem_RSSI()
      : Variable((const uint8_t)MODEM_RSSI_VAR_NUM,
                 (uint8_t)MODEM_RSSI_RESOLUTION,
                 "RSSI", "decibelMiliWatt", "RSSI")
    {}
    ~Modem_RSSI(){}
};


// Defines the Signal Percentage
class Modem_SignalPercent : public Variable
{
public:
    Modem_SignalPercent(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "signalPercent")
      : Variable(parentSense,
                 (const uint8_t)MODEM_PERCENT_SIGNAL_VAR_NUM,
                 (uint8_t)MODEM_PERCENT_SIGNAL_RESOLUTION,
                 "signalPercent", "percent",
                 varCode, uuid)
    {}
    Modem_SignalPercent()
      : Variable((const uint8_t)MODEM_PERCENT_SIGNAL_VAR_NUM,
                 (uint8_t)MODEM_PERCENT_SIGNAL_RESOLUTION,
                 "signalPercent", "percent", "signalPercent")
    {}
    ~Modem_SignalPercent(){}
};


// Defines the Battery Charge State
class Modem_BatteryState : public Variable
{
public:
    Modem_BatteryState(Sensor *parentSense,
                       const char *uuid = "",
                       const char *varCode = "modemBatteryCS")
      : Variable(parentSense,
                 (const uint8_t)MODEM_BATTERY_STATE_VAR_NUM,
                 (uint8_t)MODEM_BATTERY_STATE_RESOLUTION,
                 "batteryChargeState", "number",
                 varCode, uuid)
    {}
    Modem_BatteryState()
      : Variable((const uint8_t)MODEM_BATTERY_STATE_VAR_NUM,
                 (uint8_t)MODEM_BATTERY_STATE_RESOLUTION,
                 "batteryChargeState", "number", "modemBatteryCS")
    {}
    ~Modem_BatteryState(){}
};


// Defines the Battery Charge Percent
class Modem_BatteryPercent : public Variable
{
public:
    Modem_BatteryPercent(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "modemBatteryPct")
      : Variable(parentSense,
                 (const uint8_t)MODEM_BATTERY_PERCENT_VAR_NUM,
                 (uint8_t)MODEM_BATTERY_PERCENT_RESOLUTION,
                 "batteryVoltage", "percent",
                 varCode, uuid)
    {}
    Modem_BatteryPercent()
      : Variable((const uint8_t)MODEM_BATTERY_PERCENT_VAR_NUM,
                 (uint8_t)MODEM_BATTERY_PERCENT_RESOLUTION,
                 "batteryVoltage", "percent", "modemBatteryPct")
    {}
    ~Modem_BatteryPercent(){}
};


// Defines the Battery Voltage
class Modem_BatteryVoltage : public Variable
{
public:
    Modem_BatteryVoltage(Sensor *parentSense,
                         const char *uuid = "",
                         const char *varCode = "modemBatterymVolt")
      : Variable(parentSense,
                 (const uint8_t)MODEM_BATTERY_VOLT_VAR_NUM,
                 (uint8_t)MODEM_BATTERY_VOLT_RESOLUTION,
                 "batteryVoltage", "milliVolt",
                 varCode, uuid)
    {}
    Modem_BatteryVoltage()
      : Variable((const uint8_t)MODEM_BATTERY_VOLT_VAR_NUM,
                 (uint8_t)MODEM_BATTERY_VOLT_RESOLUTION,
                 "batteryVoltage", "milliVolt", "modemBatterymVolt")
    {}
    ~Modem_BatteryVoltage(){}
};


// Defines the Temperature Sensor on the modem (not all modems have one)
class Modem_Temp : public Variable
{
public:
    Modem_Temp(Sensor *parentSense,
                        const char *uuid = "",
                        const char *varCode = "modemTemp")
      : Variable(parentSense,
                 (const uint8_t)MODEM_TEMPERATURE_VAR_NUM,
                 (uint8_t)MODEM_TEMPERATURE_RESOLUTION,
                 "temperature", "degreeCelsius",
                 varCode, uuid)
    {}
    Modem_Temp()
      : Variable((const uint8_t)MODEM_TEMPERATURE_VAR_NUM,
                 (uint8_t)MODEM_TEMPERATURE_RESOLUTION,
                 "temperature", "degreeCelsius", "modemTemp")
    {}
    ~Modem_Temp(){}
};

#endif  // Header Guard
