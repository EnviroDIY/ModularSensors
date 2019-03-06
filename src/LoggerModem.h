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
// #define MODEM_DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Arduino.h>
#include <TinyGsmCommon.h>


#define MODEM_NUM_VARIABLES 2

// Set some default timing variables for the modems.
// Some of these times are reset for specific modems in the modem set-up function
// based on the values in the modem chip's datasheet.

// Length of time after power is applied to module before the enable pin can be
// called to turn on the module or other wake fxn can be used.  If the module
// boots up as soon as power is applied, this value is 0.
// This is used as the sensor variable _warmUpTime_ms.
#define MODEM_WARM_UP_TIME_MS 50
// Length of time from the completion of wake up function until UART port becomes
// available for AT commands.  This is the MAXIMUM amount of time we will wait
// for a response from the modem.
// This is used as the sensor variable _stabilizationTime_ms.
#define MODEM_ATRESPONSE_TIME_MS 5000L
// The maximum amount of time we are willing to wait for a network connection
// before accepting the modem signal strength.  Most modems do not give
// real signal strength until they've connected to the network, which takes
// more time in areas of weaker signal.
// This is used as the sensor variable _measurementTime_ms.
#define MODEM_MAX_SEARCH_TIME 15000L
// Length of time from the completion of wake up  request until the modem status
// pin begins to show an "on" status.
// This is the modem-only variable _statusTime_ms.
#define MODEM_STATUS_TIME_MS 5000
// Approximate length of time for unit to gracefully close sockets and disconnect
// from the network.  Most manufactures strongly recommend allowing a graceful
// shut-down rather than a sudden power-off.
// This is the modem-only variable _disconnetTime_ms.
#define MODEM_DISCONNECT_TIME_MS 5000

#define RSSI_VAR_NUM 0
#define RSSI_RESOLUTION 0

#define PERCENT_SIGNAL_VAR_NUM 1
#define PERCENT_SIGNAL_RESOLUTION 0

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
    // Constructors
    // Note:  The client for the TinyGSM Modem is needed as an input because
    // the tiny GSM client class is a subclass of the unique modem class, not
    // of the generalized modem class.  That means the client cannot be create
    // until the specific modem is defined.  So the user must first create the
    // specific modem in their sketch, create that modem's client, and then
    // feed that client back in here.  The TinyGSM library has a bunch of
    // typedef's in the TinyGsmClient.h that make this somewhat invisible to
    // the user.
    loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                bool (*modemWakeFxn)(), bool (*modemSleepFxn)(),
                TinyGsmModem *inModem, Client *inClient, const char *APN);

    loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                bool (*modemWakeFxn)(), bool (*modemSleepFxn)(),
                TinyGsmModem *inModem, Client *inClient, const char *ssid, const char *pwd);

    ~loggerModem();

    String getSensorName(void) override;

    virtual bool setup(void) override;
    virtual bool wake(void) override;

    // Do NOT turn the modem on and off with the regular power up and down!
    // This is because when it is run in an array with other sensors, we will
    // generally want the modem to remain on after all the other sensors have
    // gone to sleep and powered down so the modem can send out data
    void powerUp(void) override;
    void powerDown(void) override;

    // Turns modem signal strength into a measurement
    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;

protected:
    // We override these because the modem can tell us if it's ready or not

    // The modem is "stable" when it responds to AT commands.
    // For a WiFi modem, this actually sets the network connection parameters!!
    bool isStable(bool debug=false) override;

    // The a measurement is "complete" when the modem is registered on the network.
    // For a cellular modem, this actually sets the GPRS bearer/APN!!
    bool isMeasurementComplete(bool debug=false) override;


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//
public:
    int16_t getSignalRSSI(void) {return sensorValues[RSSI_VAR_NUM];}
    int16_t getSignalPercent(void) {return sensorValues[PERCENT_SIGNAL_VAR_NUM];}

    Client * getClient(void){return _tinyClient;}

    bool connectInternet(uint32_t waitTime_ms = 50000L);
    void disconnectInternet(void);

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
    uint32_t getNISTTime(void);

public:
    // All of these must be pointers - these are all abstract classes!
    TinyGsmModem *_tinyModem;
    Client *_tinyClient;

private:
    // Helper to get approximate RSSI from CSQ (assuming no noise)
    static int16_t getRSSIFromCSQ(int16_t csq);
    // Helper to get signal percent from CSQ
    static int16_t getPctFromCSQ(int16_t csq);
    // Helper to get signal percent from CSQ
    static int16_t getPctFromRSSI(int16_t rssi);
    // Helper to set the timing for specific cellular chipsets based on their documentation
    void setModemTiming(void);

private:
    bool (*_modemWakeFxn)(void);
    bool (*_modemSleepFxn)(void);

    bool _statusLevel;
    int8_t _modemSleepRqPin;
    uint16_t _statusTime_ms;
    uint16_t _disconnetTime_ms;
    // uint16_t _on_pull_down_ms;
    // uint16_t _off_pull_down_ms;
    const char *_apn;
    const char *_ssid;
    const char *_pwd;
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
                 (const uint8_t)RSSI_VAR_NUM,
                 (uint8_t)RSSI_RESOLUTION,
                 "RSSI", "decibelMiliWatt",
                 varCode, uuid)
    {}
    Modem_RSSI()
      : Variable((const uint8_t)RSSI_VAR_NUM,
                 (uint8_t)RSSI_RESOLUTION,
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
                 (const uint8_t)PERCENT_SIGNAL_VAR_NUM,
                 (uint8_t)PERCENT_SIGNAL_RESOLUTION,
                 "signalPercent", "percent",
                 varCode, uuid)
    {}
    Modem_SignalPercent()
      : Variable((const uint8_t)PERCENT_SIGNAL_VAR_NUM,
                 (uint8_t)PERCENT_SIGNAL_RESOLUTION,
                 "signalPercent", "percent", "signalPercent")
    {}
    ~Modem_SignalPercent(){}
};

#endif  // Header Guard
