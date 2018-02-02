/*
 *LoggerModem.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file wraps the tinyGSM library:  https://github.com/vshymanskyy/TinyGSM
 *and adds in the power functions to turn the modem on and off.
*/

#ifndef LoggerModem_h
#define LoggerModem_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "ModemOnOff.h"
#include "SensorBase.h"
#include "VariableBase.h"

#if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM808) || \
    defined(TINY_GSM_MODEM_SIM868) || defined(TINY_GSM_MODEM_SIM900) || \
    defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
    defined(TINY_GSM_MODEM_M590) || defined(TINY_GSM_MODEM_U201) || \
    defined(TINY_GSM_MODEM_ESP8266) || defined(TINY_GSM_MODEM_XBEE)
  #define USE_TINY_GSM
  // #define TINY_GSM_DEBUG Serial
  #define TINY_GSM_YIELD() { delay(1);}
  #define TINY_GSM_RX_BUFFER 14  // So we never get much data
  #include <TinyGsmClient.h>
// #else
//   #include <NullModem.h>  // purely to help me debug compilation issues
//   #define USE_TINY_GSM
#endif

#if defined(USE_TINY_GSM)

// Give the modems names
#if defined(TINY_GSM_MODEM_SIM800)
    #define MODEM_NAME "SIMCom SIM800"
#elif defined(TINY_GSM_MODEM_SIM808)
    #define MODEM_NAME "SIMCom SIM808"
#elif defined(TINY_GSM_MODEM_SIM868)
    #define MODEM_NAME "SIMCom SIM868"
#elif defined(TINY_GSM_MODEM_SIM900)
    #define MODEM_NAME "SIMCom SIM900"
#elif defined(TINY_GSM_MODEM_A6)
    #define MODEM_NAME "AI-Thinker A6"
#elif defined(TINY_GSM_MODEM_A7)
    #define MODEM_NAME "AI-Thinker A7"
#elif defined(TINY_GSM_MODEM_M590)
    #define MODEM_NAME "Neoway SIM590"
#elif defined(TINY_GSM_MODEM_U201)
    #define MODEM_NAME "U-blox SARA U201"
#elif defined(TINY_GSM_MODEM_ESP8266)
    #define MODEM_NAME "ESP8266"
#elif defined(TINY_GSM_MODEM_XBEE)
    #define MODEM_NAME "Digi XBee"
#else
    #define MODEM_NAME "NO MODEM SELECTED"
#endif

#define MODEM_NUM_VARIABLES 2
#define MODEM_WARM_UP 0
#define MODEM_STABILITY 0
#define MODEM_RESAMPLE 0
#define CSQ_VAR_NUM 0
#define PERCENT_STAT_VAR_NUM 1

// For the various communication devices"
typedef enum ModemSleepType
{
  held = 0,  // Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
  pulsed,  // Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
  reverse,  // Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
  always_on
} ModemSleepType;

/* ===========================================================================
* Functions for the modem class
* This is basically a wrapper for TinyGsm
* ========================================================================= */

class loggerModem : public Sensor
{

public:
    // Constructors
    loggerModem()
        : Sensor(F(MODEM_NAME), MODEM_NUM_VARIABLES, MODEM_WARM_UP, 0, 0, -1, -1, 1)
    {}

    String getSensorLocation(void) override { return F("Modem Serial Port"); }

    // The modem must be setup separately!
    virtual SENSOR_STATUS setup(void) override {return SENSOR_READY;}

    void powerUp(void) override
    {
        // Check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) modemOnOff->on();
    }

    virtual bool wake(void) override
    {
        // Check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) return modemOnOff->on();
        else return true;
    }

    // Do NOT power down the modem with the regular sleep function.
    // This is because when it is run in an array with other sensors, we will
    // generally want the modem to remain on after all the other sensors have
    // gone to sleep so the modem can send out data
    virtual bool sleep(void) override { return true; }

    virtual bool off(void)
    {
        bool retVal = true;
         // Wait for any sending to complete
        _client->flush();
        // Check if the modem is on; turn it off if so
        if(modemOnOff->isOn()) retVal = modemOnOff->off();
        else retVal =  true;
        return retVal;
    }

    void powerDown(void) override {off();}

    bool startSingleMeasurement(void) override
    {return true;}
    bool addSingleMeasurementResult(void) override
    {return true;}

    bool update(void) override
    {
        // Clear values before starting loop
        clearValues();

        bool retVal = true;

        // Connect to the network before asking for quality
        if (!_modem->isNetworkConnected()) retVal &= connectInternet();
        if (retVal == false) return false;


        // The XBee needs to make a connection before it knows the signal quality
        // Connecting to the daytime server because it works and will immediately
        // close on its own so I don't need a separate stop.
        #if defined(TINY_GSM_MODEM_XBEE)
            IPAddress ip(129, 6, 15, 30);
            openTCP(ip, 37);  // XBee is faster with an ip address
            _client->print(F("Hi!"));  // Need to send something before connection is made
            delay(100); // Need this delay!  Can get away with 50, but 100 is safer.
        #endif

        // Get signal quality
        sensorValues[CSQ_VAR_NUM] = getSignalRSSI();
        sensorValues[PERCENT_STAT_VAR_NUM] = getSignalPercent();

        // Update the registered variables with the new values
        notifyVariables();

        // Do NOT disconnect from the network here!
        // Chances are high the modem should be left on for sending data after
        // the update has been run!

        return retVal;
    }

public:
    void setupModem(Stream *modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *APN)
    {
        _APN = APN;
        init(modemStream, vcc33Pin, modemStatusPin, modemSleepRqPin, sleepType);
    }
    void setupModem(Stream &modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *APN)
    {
        _APN = APN;
        init(&modemStream, vcc33Pin, modemStatusPin, modemSleepRqPin, sleepType);
    }

    void setupModem(Stream *modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *ssid,
                    const char *pwd)
    {
        _ssid = ssid;
        _pwd = pwd;
        init(modemStream, vcc33Pin, modemStatusPin, modemSleepRqPin, sleepType);
    }
    void setupModem(Stream &modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *ssid,
                    const char *pwd)
    {
        _ssid = ssid;
        _pwd = pwd;
        init(&modemStream, vcc33Pin, modemStatusPin, modemSleepRqPin, sleepType);
    }

    int getSignalRSSI(void)
    {
        int signalQual = _modem->getSignalQuality();

        // Convert signal quality to RSSI, if necessary
        #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
            int rssi = signalQual;
        #else
            int rssi = getRSSIFromCSQ(signalQual);
        #endif

        return rssi;
    }

    int getSignalPercent(void)
    {
        int signalQual = _modem->getSignalQuality();

        // Convert signal quality to RSSI, if necessary
        #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
            int percent = getPctFromRSSI(signalQual);
        #else
            int percent = getPctFromCSQ(signalQual);
        #endif

        return percent;
    }

    bool connectInternet(void)
    {
        bool retVal = false;

        // Check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) modemOnOff->on();
        // Check again if the modem is on.  If it still isn't on, give up
        if(!modemOnOff->isOn())
        {
            MS_DBG(F("\nModem failed to turn on!\n"));
            return false;
        }

        // Check that the modem is responding to AT commands.  If not, give up.
        if (!_modem->testAT(5000L))
        {
            MS_DBG(F("\nModem does not respond to AT commands!\n"));
            return false;
        }

        // WiFi modules immediately re-connect to the last access point so we
        // can save just a tiny bit of time (and thus power) by not resending
        // the credentials every time.
        #if defined(TINY_GSM_MODEM_HAS_WIFI)
        if (_ssid)
        {
            MS_DBG(F("Connecting to WiFi network..."));
            if (!_modem->waitForNetwork(5000L)){
                MS_DBG("   ... Connection failed.  Resending credentials...");
                _modem->networkConnect(_ssid, _pwd);
                if (!_modem->waitForNetwork(30000L)){
                    MS_DBG("   ... Connection failed\n");
                } else {
                    retVal = true;
                    MS_DBG("   ... Success!\n");
                }
            } else {
                #if defined(TINY_GSM_MODEM_ESP8266)
                    // make sure mux is right
                    _modem->sendAT(GF("+CIPMUX=1"));
                    _modem->waitResponse();
                #endif  //  TINY_GSM_MODEM_ESP8266
                MS_DBG("   ... Success!\n");
                 retVal = true;
            }
        }
        else
        {
        #endif  // TINY_GSM_MODEM_HAS_WIFI
        #if defined(TINY_GSM_MODEM_HAS_GPRS)
            MS_DBG(F("\nWaiting for cellular network..."));
            if (!_modem->waitForNetwork(45000L)){
                MS_DBG("   ...Connection failed.");
            } else {
                _modem->gprsConnect(_APN, "", "");
                MS_DBG("   ...Success!\n");
                retVal = true;
            }
        #endif  // TINY_GSM_MODEM_HAS_GPRS
        #if defined(TINY_GSM_MODEM_HAS_WIFI)
        }
        #endif  // TINY_GSM_MODEM_HAS_WIFI

        return retVal;
    }

    void disconnectInternet(void)
    {
    #if defined(TINY_GSM_MODEM_HAS_GPRS)
        _modem->gprsDisconnect();
        MS_DBG(F("Disconnected from cellular network.\n"));
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        // _modem->networkDisconnect();  // Eh.. why bother?
        // MS_DBG(F("Disconnected from WiFi network.\n"));
    #endif
    }

    int openTCP(const char *host, uint16_t port)
    {
        MS_DBG("Connecting to ", host, "...");
        int ret_val = _client->connect(host, port);
        if (ret_val) MS_DBG("   ...Success!\n");
        else MS_DBG("   ...Connection failed.\n");
        return ret_val;
    }

    int openTCP(IPAddress ip, uint16_t port)
    {
        MS_DBG("Connecting to ", ip, "...");
        int ret_val = _client->connect(ip, port);
        if (ret_val) MS_DBG("   ...Success!\n");
        else MS_DBG("   ...Connection failed.\n");
        return ret_val;
    }

    void closeTCP(void)
    {
        _client->stop();
        MS_DBG(F("Closed TCP/IP.\n"));
    }

    // Get the time from NIST via TIME protocol (rfc868)
    // This would be much more efficient if done over UDP, but I'm doing it
    // over TCP because I don't have a UDP library for all the modems.
    uint32_t getNISTTime(void)
    {

        // Make TCP connection
        #if defined(TINY_GSM_MODEM_XBEE)
            IPAddress ip(129, 6, 15, 30);
            openTCP(ip, 37);  // XBee is faster with an ip address
        #else
        openTCP("time.nist.gov", 37);
        #endif

        // XBee needs to send something before the connection is actually made
        #if defined(TINY_GSM_MODEM_XBEE)
        _client->print(F("Hi!"));
        delay(100); // Need this delay!  Can get away with 50, but 100 is safer.
        #endif

        // Wait up to 5 seconds for a response
        long start = millis();
        while (_client->available() < 4 && millis() - start < 5000){}

        // Response is returned as 32-bit number as soon as connection is made
        // Connection is then immediately closed, so there is no need to close it
        uint32_t secFrom1900 = 0;
        byte response[4] = {0};
        for (uint8_t i = 0; i < 4; i++)
        {
            response[i] = _client->read();
            // MS_DBG("\n",response[i]);
            secFrom1900 += 0x000000FF & response[i];
            // MS_DBG("\n*****",String(secFrom1900, BIN),"*****");
            if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
        }
        // MS_DBG("\n*****",secFrom1900,"*****");

        // Return the timestamp
        uint32_t unixTimeStamp = secFrom1900 - 2208988800;
        MS_DBG(F("Timestamp returned by NIST (UTC): "), unixTimeStamp, '\n');
        // If before Jan 1, 2017 or after Jan 1, 2030, most likely an error
        if (unixTimeStamp < 1483228800) return 0;
        else if (unixTimeStamp > 1893456000) return 0;
        else return unixTimeStamp;
    }

public:
    ModemOnOff *modemOnOff;

    TinyGsm *_modem;
    TinyGsmClient *_client;

private:
    const char *_APN;
    const char *_ssid;
    const char *_pwd;

private:
    void init(Stream *modemStream, int vcc33Pin, int modemStatusPin, int modemSleepRqPin,
              ModemSleepType sleepType)
    {
        // Set up the method for putting the modem to sleep

        switch(sleepType)
        {
            case pulsed:
            {
                MS_DBG(F("Setting "), F(MODEM_NAME),
                    F(" power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and on/off via 2.5 second pulse on pin "), modemSleepRqPin, F(".\n"));
                static pulsedOnOff pulsed;
                modemOnOff = &pulsed;
                pulsed.init(vcc33Pin, modemSleepRqPin, modemStatusPin);
                break;
            }
            case held:
            {
                MS_DBG(F("Setting "), F(MODEM_NAME),
                    F(" power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and on/off by holding pin "), modemSleepRqPin, F("high.\n"));
                static heldOnOff held;
                modemOnOff = &held;
                held.init(vcc33Pin, modemSleepRqPin, modemStatusPin);
                break;
            }
            case reverse:
            {
                MS_DBG(F("Setting "), F(MODEM_NAME),
                    F(" power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and on/off by holding pin "), modemSleepRqPin, F("low.\n"));
                static reverseOnOff reverse;
                modemOnOff = &reverse;
                reverse.init(vcc33Pin, modemSleepRqPin, modemStatusPin);
                break;
            }
            default:
            {
                MS_DBG(F("Setting "), F(MODEM_NAME),
                    F(" power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and no on/off pin.\n"));
                static heldOnOff held;
                modemOnOff = &held;
                held.init(vcc33Pin, -1, modemStatusPin);
                break;
            }
        }

        // Initialize the modem
        MS_DBG(F("Initializing "), F(MODEM_NAME), F("..."));
        static TinyGsm modem(*modemStream);
        _modem = &modem;
        static TinyGsmClient client(modem);
        _client = &client;

        // Check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) modemOnOff->on();
        // Check again if the modem is on.  Only "begin" if it responded.
        if(modemOnOff->isOn())
        {
            _modem->begin();
            #if defined(TINY_GSM_MODEM_XBEE)
                _modem->setupPinSleep();
            #endif
            modemOnOff->off();
        }
        else MS_DBG(F("   ... Modem failed to turn on!\n"));
        MS_DBG(F("   ... Complete!\n"));
    }

    // Helper to get approximate RSSI from CSQ (assuming no noise)
    static int getRSSIFromCSQ(int csq)
    {
        int CSQs[33]  = {  0,   1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 99};
        int RSSIs[33] = {113, 111, 109, 107, 105, 103, 101, 99, 97, 95, 93, 91, 89, 87, 85, 83, 81, 79, 77, 75, 73, 71, 69, 67, 65, 63, 61, 59, 57, 55, 53, 51, 0};
        for (int i = 0; i < 33; i++)
        {
            if (CSQs[i] == csq) return RSSIs[i];
        }
        return 0;
    }

    // Helper to get signal percent from CSQ
    static int getPctFromCSQ(int csq)
    {
        int CSQs[33] = {0, 1, 2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 99};
        int PCTs[33] = {0, 3, 6, 10, 13, 16, 19, 23, 26, 29, 32, 36, 39, 42, 45, 48, 52, 55, 58, 61, 65, 68, 71, 74, 78, 81, 84, 87, 90, 94, 97, 100, 0};
        for (int i = 0; i < 33; i++)
        {
            if (CSQs[i] == csq) return PCTs[i];
        }
        return 0;
    }

    // Helper to get signal percent from CSQ
    static int getPctFromRSSI(int rssi)
    {
        int pct = 1.6163*rssi + 182.61;
        if (rssi == 0) pct = 0;
        if (rssi == (255-93)) pct = 0;  // This is a no-data-yet value from XBee
        return pct;
    }

};



// Classes for the modem variables
// Defines the received signal strength indication
class Modem_RSSI : public Variable
{
public:
    Modem_RSSI(Sensor *parentSense, String UUID = "", String customVarCode = "")
     : Variable(parentSense, CSQ_VAR_NUM,
                F("RSSI"), F("decibelMiliWatt"),
                0,
                F("RSSI"), UUID, customVarCode)
    {}
};


// Defines the Signal Percentage
class Modem_SignalPercent : public Variable
{
public:
    Modem_SignalPercent(Sensor *parentSense, String UUID = "", String customVarCode = "")
     : Variable(parentSense, PERCENT_STAT_VAR_NUM,
                F("signalPercent"), F("percent"),
                0,
                F("signalPercent"), UUID, customVarCode)
    {}
};

#endif /* USE_TINY_GSM */

#endif /* LoggerModem_h */
