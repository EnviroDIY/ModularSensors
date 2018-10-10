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
// #define TINY_GSM_DEBUG Serial
// #define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_YIELD() { delay(1); }

// Included Dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include <Arduino.h>
#include <TinyGsmCommon.h>

// Maximum time to wait for a reply to an AT command from the modem
#define MODEM_MAX_REPLY_TIME 5000L
// Maximum time the modem is allowed to search for the network
#define MODEM_MAX_SEARCH_TIME 15000L

#define MODEM_NUM_VARIABLES 2
#define MODEM_WARM_UP_TIME_MS 0
#define MODEM_STABILIZATION_TIME_MS 0
#define MODEM_MEASUREMENT_TIME_MS 0

#define RSSI_VAR_NUM 0
#define RSSI_RESOLUTION 0

#define PERCENT_SIGNAL_VAR_NUM 1
#define PERCENT_SIGNAL_RESOLUTION 0


#ifdef MODEM_DEBUGGING_SERIAL_OUTPUT
    namespace {
        template<typename T>
        static void MS_MOD_DBG(T last) {
            MODEM_DEBUGGING_SERIAL_OUTPUT.print(last);
        }

        template<typename T, typename... Args>
        static void MS_MOD_DBG(T head, Args... tail) {
            MODEM_DEBUGGING_SERIAL_OUTPUT.print(head);
            MS_MOD_DBG(tail...);
        }
    }
#else
    #define MS_MOD_DBG(...)
#endif  // MODEM_DEBUGGING_SERIAL_OUTPUT

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
    loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                bool (*wakeFxn)(), bool (*sleepFxn)(),
                TinyGsmModem *inModem, Client *inClient, const char *APN);

    loggerModem(int8_t powerPin, int8_t statusPin, bool statusLevel,
                bool (*wakeFxn)(), bool (*sleepFxn)(),
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
    int getSignalRSSI(void) {return sensorValues[RSSI_VAR_NUM];}
    int getSignalPercent(void) {return sensorValues[PERCENT_SIGNAL_VAR_NUM];}

    bool connectInternet(uint32_t waitTime_ms = 50000L);
    void disconnectInternet(void);

    // This has the same functionality as Client->connect with debugging text
    // int openTCP(const char *host, uint16_t port);
    // This has the same functionality as Client->connect with debugging text
    // int openTCP(IPAddress ip, uint16_t port);
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
    static int getRSSIFromCSQ(int csq);
    // Helper to get signal percent from CSQ
    static int getPctFromCSQ(int csq);
    // Helper to get signal percent from CSQ
    static int getPctFromRSSI(int rssi);

private:
    bool _statusLevel;
    uint32_t _indicatorTime_ms;
    uint32_t _disconnetTime_ms;
    bool (*_wakeFxn)(void);
    bool (*_sleepFxn)(void);
    const char *_apn;
    const char *_ssid;
    const char *_pwd;
    uint32_t _lastNISTrequest;
    String _modemName;

};



// Classes for the modem variables
// Defines the received signal strength indication
class Modem_RSSI : public Variable
{
public:
    Modem_RSSI(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, RSSI_VAR_NUM,
                "RSSI", "decibelMiliWatt",
                RSSI_RESOLUTION,
                "RSSI", UUID, customVarCode)
    {}
    ~Modem_RSSI(){}
};


// Defines the Signal Percentage
class Modem_SignalPercent : public Variable
{
public:
    Modem_SignalPercent(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, PERCENT_SIGNAL_VAR_NUM,
                "signalPercent", "percent",
                PERCENT_SIGNAL_RESOLUTION,
                "signalPercent", UUID, customVarCode)
    {}
    ~Modem_SignalPercent(){}
};

#endif  // Header Guard
