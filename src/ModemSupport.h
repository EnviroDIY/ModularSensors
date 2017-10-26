/*
 *ModemSupport.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file wraps the tinyGSM library:  https://github.com/vshymanskyy/TinyGSM
 *and adds in the power functions to turn the modem on and off.
*/

#ifndef ModemSupport_h
#define ModemSupport_h

#include <Arduino.h>
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
  #define TINY_GSM_YIELD() { delay(3);}
  #include <TinyGsmClient.h>
#else
  #include <NullModem.h>  // purely to help me debug compilation issues
  #define DBG(...)
#endif

// For the various communication devices"
typedef enum DTRSleepType
{
  held = 0,  // Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
  pulsed,  // Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
  reverse,  // Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
  always_on
} DTRSleepType;

/* ===========================================================================
* Functions for the modem class
* This is basically a wrapper for TinyGsm
* ========================================================================= */

class loggerModem : public Sensor
{
public:

    void setupModem(Stream *modemStream,
                    int vcc33Pin,
                    int status_CTS_pin,
                    int onoff_DTR_pin,
                    DTRSleepType sleepType,
                    const char *APN)
    {
        _APN = APN;
        init(modemStream, vcc33Pin, status_CTS_pin, onoff_DTR_pin, sleepType);
    }
    void setupModem(Stream &modemStream,
                    int vcc33Pin,
                    int status_CTS_pin,
                    int onoff_DTR_pin,
                    DTRSleepType sleepType,
                    const char *APN)
    {
        _APN = APN;
        init(&modemStream, vcc33Pin, status_CTS_pin, onoff_DTR_pin, sleepType);
    }

    void setupModem(Stream *modemStream,
                    int vcc33Pin,
                    int status_CTS_pin,
                    int onoff_DTR_pin,
                    DTRSleepType sleepType,
                    const char *ssid,
                    const char *pwd)
    {
        _ssid = ssid;
        _pwd = pwd;
        init(modemStream, vcc33Pin, status_CTS_pin, onoff_DTR_pin, sleepType);
    }
    void setupModem(Stream &modemStream,
                    int vcc33Pin,
                    int status_CTS_pin,
                    int onoff_DTR_pin,
                    DTRSleepType sleepType,
                    const char *ssid,
                    const char *pwd)
    {
        _ssid = ssid;
        _pwd = pwd;
        init(&modemStream, vcc33Pin, status_CTS_pin, onoff_DTR_pin, sleepType);
    }

    bool on(void)
    {
        // Check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) return modemOnOff->on();
        else return true;
    }

    bool off(void)
    {
        bool retVal = true;
         // Wait for any sending to complete
        stream->flush();
        // Check if the modem is on; turn it off if so
        if(modemOnOff->isOn()) retVal = modemOnOff->off();
        else retVal =  true;
        // Empty anything out of the receive buffer
        dumpBuffer(stream);
        return retVal;
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
    static int getPctFromRSSI(int rssi)
    {
        int pct = 1.6163*rssi + 182.61;
        if (rssi == 0) pct = 0;
        if (rssi == (255-93)) pct = 0;  // This is a no-data-yet value from XBee
        return pct;
    }

    #define MODEM_NUM_MEASUREMENTS 2
    #define CSQ_VAR_NUM 0
    #define PERCENT_STAT_VAR_NUM 1

    bool connectNetwork(void)
    {
        bool retVal = false;

        // Check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) modemOnOff->on();
        // Check again if the modem is on.  If it still isn't on, give up
        if(!modemOnOff->isOn()) return false;

        // Check that the modem is responding to AT commands.  If not, give up.
        if (!_modem->testAT(5000L)) return false;

        // WiFi modules immediately re-connect to the last access point so we
        // can save just a tiny bit of time (and thus power) by not resending
        // the credentials every time.
        #if defined(TINY_GSM_MODEM_HAS_WIFI)
        if (_ssid)
        {
            DBG(F("\nConnecting to WiFi network..."));
            if (!_modem->waitForNetwork(2000L)){
                DBG("... Connection failed.  Resending credentials...");
                _modem->networkConnect(_ssid, _pwd);
                if (!_modem->waitForNetwork(30000L)){
                    DBG("... Connection failed");
                } else {
                    retVal = true;
                    DBG("... Success!");
                }
            } else {
                DBG("... Success!");
                retVal = true;
            }
        }
        else
        {
        #endif
        #if defined(TINY_GSM_MODEM_HAS_GPRS)
            DBG(F("\nWaiting for cellular network..."));
            if (!_modem->waitForNetwork(45000L)){
                DBG("... Connection failed.");
            } else {
                _modem->gprsConnect(_APN, "", "");
                DBG("... Success!");
                retVal = true;
            }

        #endif
        #if defined(TINY_GSM_MODEM_HAS_WIFI)
        }
        #endif

        // #if defined(USE_TINY_GSM)
        // Now we are essentially running the "update" function to update
        // the variables assigned to the modem "sensor".  We are doing this
        // here because we want the values to be assigned with the actual
        // connection used when the data is sent out.

        // Clear values before starting loop
        clearValues();

        // Get signal quality
        int signalQual = _modem->getSignalQuality();

        // Convert signal quality to RSSI, if necessary
        #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
            int rssi = signalQual;
        #else
            int rssi = getRSSIFromCSQ(signalQual);
        #endif

        // Convert signal quality to a percent
        #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
            int signalPercent = getPctFromRSSI(signalQual);
        #else
            int signalPercent = getPctFromCSQ(signalQual);
        #endif

        sensorValues[CSQ_VAR_NUM] = rssi;
        sensorValues[PERCENT_STAT_VAR_NUM] = signalPercent;

        // Update the registered variables with the new values
        notifyVariables();
        // #endif

        return retVal;
    }

    void disconnectNetwork(void)
    {
        DBG(F("Disconnecting from network"));
    #if defined(TINY_GSM_MODEM_HAS_GPRS)
        _modem->gprsDisconnect();
    #elif defined(TINY_GSM_MODEM_HAS_WIFI)
        _modem->networkDisconnect();
    #endif
    }

    int connect(const char *host, uint16_t port)
    {
    DBG("Connecting to ", host, "...");
    // #if defined(USE_TINY_GSM)
        int ret_val = _client->connect(host, port);
        if (ret_val) DBG("... Success!");
        else DBG("... Connection failed.");
        return ret_val;
    // #else
    //     return 0;
    // #endif
    }

    void stop(void)
    {
    DBG(F("Disconnecting from TCP/IP..."));
    // #if defined(USE_TINY_GSM)
        _client->stop();
    // #endif
    }

    // Used to empty out the buffer after a post request.
    // Removing this may cause communication issues.
    void dumpBuffer(Stream *stream, int timeDelay = 5, int timeout = 5000)
    {
        delay(timeDelay);
        while (timeout-- > 0 && stream->available() > 0)
        {
            stream->read();
            delay(timeDelay);
        }
    }

    // Get the time from NIST via TIME protocol (rfc868)
    // This would be much more efficient if done over UDP, but I'm doing it
    // over TCP because I don't have a UDP library for all the modems.
    uint32_t getNISTTime(void)
    {

        // Make TCP connection
        #if defined(TINY_GSM_MODEM_XBEE)
        connect("time-c.nist.gov", 37);  // XBee cannot resolve time.nist.gov
        #else
        connect("time.nist.gov", 37);
        #endif

        // XBee needs to send something before the connection is actually made
        #if defined(TINY_GSM_MODEM_XBEE)
        stream->write("Hi!");
        delay(75); // Need this delay!  Can get away with 50, but 100 is safer.
        #endif

        // Wait up to 5 seconds for a response
        long start = millis();
        while (stream->available() < 4 && millis() - start < 5000){}

        // Response is returned as 32-bit number as soon as connection is made
        // Connection is then immediately closed, so there is no need to close it
        uint32_t secFrom1900 = 0;
        byte response[4] = {0};
        for (uint8_t i = 0; i < 4; i++)
        {
            response[i] = stream->read();
            // DBG("\n",response[i]);
            secFrom1900 += 0x000000FF & response[i];
            // DBG("\n*****",String(secFrom1900, BIN),"*****");
            if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
        }
        // DBG("\n*****",secFrom1900,"*****");

        // Return the timestamp
        uint32_t unixTimeStamp = secFrom1900 - 2208988800;
        DBG(F("Timesamp returned by NIST (UTC): "), unixTimeStamp);
        // If before Jan 1, 2017 or after Jan 1, 2030, most likely an error
        if (unixTimeStamp < 1483228800) return 0;
        else if (unixTimeStamp > 1893456000) return 0;
        else return unixTimeStamp;
    }

    Stream *stream;
    ModemOnOff *modemOnOff;

    // More functions for using the modem as a "sensor"
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
        #define MODEM_NAME "Unknown"
    #endif

    // Constructors
    loggerModem() : Sensor(-1, -1, F(MODEM_NAME), MODEM_NUM_MEASUREMENTS, 0) {}

    String getSensorLocation(void) override { return F("Modem Serial Port"); }
    // Actually doing NOTHING on any of the rest of the functions.  The modem
    // must be set-up and turned on and off separately!!  The update then
    // happens when connecting to the network
    virtual SENSOR_STATUS setup(void) override {return SENSOR_READY;}
    virtual bool sleep(void) override {return true;}
    virtual bool wake(void) override {return true;}
    bool update(void) override { return true; }

// #if defined(USE_TINY_GSM)
    TinyGsm *_modem;
    TinyGsmClient *_client;
// #endif


private:
    void init(Stream *modemStream, int vcc33Pin, int status_CTS_pin, int onoff_DTR_pin,
              DTRSleepType sleepType)
    {
        // Set up the method for putting the modem to sleep
        switch(sleepType)
        {
            case pulsed:
            {
                static pulsedOnOff pulsed;
                modemOnOff = &pulsed;
                pulsed.init(vcc33Pin, onoff_DTR_pin, status_CTS_pin);
                break;
            }
            case held:
            {
                static heldOnOff held;
                modemOnOff = &held;
                held.init(vcc33Pin, onoff_DTR_pin, status_CTS_pin);
                break;
            }
            case reverse:
            {
                static reverseOnOff reverse;
                modemOnOff = &reverse;
                reverse.init(vcc33Pin, onoff_DTR_pin, status_CTS_pin);
                break;
            }
            default:
            {
                static heldOnOff held;
                modemOnOff = &held;
                held.init(-1, -1, -1);
                break;
            }
        }

        // #if defined(USE_TINY_GSM)

            // Initialize the modem
            DBG(F("Initializing GSM modem instance..."));
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
            stream = _client;
            DBG(F("   ... Complete!"));

        // #else
            // stream = modemStream;
        // #endif
    }

    const char *_APN;
    const char *_ssid;
    const char *_pwd;
};



// Classes for the modem variables
// Defines the received signal strength indication
class Modem_RSSI : public Variable
{
public:
    Modem_RSSI(Sensor *parentSense, String customVarCode = "")
     : Variable(parentSense, CSQ_VAR_NUM,
                F("RSSI"), F("decibelMiliWatt"),
                0,
                F("RSSI"), customVarCode)
    {}
};


// Defines the Signal Percentage
class Modem_SignalPercent : public Variable
{
public:
    Modem_SignalPercent(Sensor *parentSense, String customVarCode = "")
     : Variable(parentSense, PERCENT_STAT_VAR_NUM,
                F("signalPercent"), F("percent"),
                0,
                F("signalPercent"), customVarCode)
    {}
};

#endif /* ModemSupport_h */
