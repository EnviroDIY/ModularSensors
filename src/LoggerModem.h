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
// #define MODEM_DEBUGGING_SERIAL_OUTPUT Serial
// #define TINY_GSM_DEBUG Serial

#include "ModemOnOff.h"
#include "SensorBase.h"
#include "VariableBase.h"

#if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM808) || \
    defined(TINY_GSM_MODEM_SIM868) || defined(TINY_GSM_MODEM_SIM900) || \
    defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
    defined(TINY_GSM_MODEM_UBLOX) || defined(TINY_GSM_MODEM_M590) || \
    defined(TINY_GSM_MODEM_M95) || defined(TINY_GSM_MODEM_BG96) || \
    defined(TINY_GSM_MODEM_MC60) || \
    defined(TINY_GSM_MODEM_ESP8266) || defined(TINY_GSM_MODEM_XBEE)
  #define TINY_GSM_YIELD() { delay(1);}
  #define TINY_GSM_RX_BUFFER 14
  #include <TinyGsmClient.h>
#else
  #include <NullModem.h>  // purely to help me debug compilation issues
#endif

// #if defined(USE_TINY_GSM)

// Give the modems names
#if defined(TINY_GSM_MODEM_SIM800)
    #define MODEM_NAME "SIMCom SIM800"
#elif defined(TINY_GSM_MODEM_SIM808)
    #define MODEM_NAME "SIMCom SIM808"
#elif defined(TINY_GSM_MODEM_SIM868)
    #define MODEM_NAME "SIMCom SIM868"
#elif defined(TINY_GSM_MODEM_SIM900)
    #define MODEM_NAME "SIMCom SIM900"
#elif defined(TINY_GSM_MODEM_UBLOX)
    #define MODEM_NAME "u-blox Cellular"
#elif defined(TINY_GSM_MODEM_M95)
    #define MODEM_NAME "Quectel M95"
#elif defined(TINY_GSM_MODEM_BG96)
    #define MODEM_NAME "Quectel BG96"
#elif defined(TINY_GSM_MODEM_A6)
    #define MODEM_NAME "AI-Thinker A6"
#elif defined(TINY_GSM_MODEM_A7)
    #define MODEM_NAME "AI-Thinker A7"
#elif defined(TINY_GSM_MODEM_M590)
    #define MODEM_NAME "Neoway M590"
#elif defined(TINY_GSM_MODEM_MC60)
    #define MODEM_NAME "Quectel MC60"
#elif defined(TINY_GSM_MODEM_MC60E)
    #define MODEM_NAME "Quectel MC60E"
#elif defined(TINY_GSM_MODEM_ESP8266)
    #define MODEM_NAME "ESP8266"
#elif defined(TINY_GSM_MODEM_XBEE)
    #define MODEM_NAME "Digi XBee"
#else
    #define MODEM_NAME "NO MODEM SELECTED"
#endif

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

// For the various ways of waking and sleeping the modem
typedef enum ModemSleepType
{
  modem_sleep_held = 0,  // Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
  modem_sleep_pulsed,  // Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
  modem_sleep_reverse,  // Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
  modem_always_on
} ModemSleepType;

//  For the various chips
typedef enum ModemChipType
{
  sim_chip_SIM800 = 0,
  sim_chip_SIM808,
  sim_chip_SIM868,
  sim_chip_SIM900,
  sim_chip_A6,
  sim_chip_A7,
  sim_chip_M590,
  sim_chip_U201,
  sim_chip_ESP8266,
  sim_chip_XBeeWifi,
  sim_chip_XBeeCell,
  sim_chip_Undefined
} ModemChipType;

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
    loggerModem(Stream *modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *APN)
        : Sensor(MODEM_NAME, MODEM_NUM_VARIABLES, MODEM_WARM_UP_TIME_MS, 0, 0, -1, -1, 1)
    {
        _stream = modemStream;
        _vcc33Pin = vcc33Pin;
        _modemStatusPin = modemStatusPin;
        _modemSleepRqPin = modemSleepRqPin;
        _sleepType = sleepType;
        _lastNISTrequest = 0;
        _APN = APN;
    }
    loggerModem(Stream &modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *APN)
        : Sensor(MODEM_NAME, MODEM_NUM_VARIABLES, MODEM_WARM_UP_TIME_MS, 0, 0, -1, -1, 1)
    {
        _stream = &modemStream;
        _vcc33Pin = vcc33Pin;
        _modemStatusPin = modemStatusPin;
        _modemSleepRqPin = modemSleepRqPin;
        _sleepType = sleepType;
        _APN = APN;
        _lastNISTrequest = 0;
    }

    loggerModem(Stream *modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *ssid,
                    const char *pwd)
        : Sensor(MODEM_NAME, MODEM_NUM_VARIABLES, MODEM_WARM_UP_TIME_MS, 0, 0, -1, -1, 1)
    {
        _stream = modemStream;
        _vcc33Pin = vcc33Pin;
        _modemStatusPin = modemStatusPin;
        _modemSleepRqPin = modemSleepRqPin;
        _sleepType = sleepType;
        _ssid = ssid;
        _pwd = pwd;
        _lastNISTrequest = 0;
    }
    loggerModem(Stream &modemStream,
                    int vcc33Pin,
                    int modemStatusPin,
                    int modemSleepRqPin,
                    ModemSleepType sleepType,
                    const char *ssid,
                    const char *pwd)
        : Sensor(MODEM_NAME, MODEM_NUM_VARIABLES, MODEM_WARM_UP_TIME_MS, 0, 0, -1, -1, 1)
    {
        _stream = &modemStream;
        _vcc33Pin = vcc33Pin;
        _modemStatusPin = modemStatusPin;
        _modemSleepRqPin = modemSleepRqPin;
        _sleepType = sleepType;
        _ssid = ssid;
        _pwd = pwd;
        _lastNISTrequest = 0;
    }

    String getSensorLocation(void) override { return F("modemSerial"); }

    // The modem must be setup separately!
    virtual bool setup(void) override
    {
        init(_stream, _vcc33Pin, _modemStatusPin, _modemSleepRqPin, _sleepType);
        return true;
    }

    // Do NOT turn the modem on and off with the regular power up and down or
    // wake and sleep functions.
    // This is because when it is run in an array with other sensors, we will
    // generally want the modem to remain on after all the other sensors have
    // gone to sleep and powered down so the modem can send out data
    void powerUp(void) override
    {
        MS_MOD_DBG(F("Skipping modem in sensor power up!\n"));
    }
    void powerDown(void) override
    {
        MS_MOD_DBG(F("Skipping modem in sensor power down!\n"));
    }


    bool addSingleMeasurementResult(void) override
    {
        int signalQual = 0;
        int percent = 0;
        int rssi = -9999;

        // Check that the modem is responding to AT commands.  If not, give up.
        MS_MOD_DBG(F("\nWaiting up to 5 seconds for modem to respond to AT commands...\n"));
        if (_modem->testAT(5000))
        {
            // The XBee needs to make an actual TCP connection and get some sort
            // of response on that connection before it knows the signal quality.
            // Connecting to the NIST daytime server, which immediately returns a
            // 4 byte response and then closes the connection
            if (loggerModemChip == sim_chip_XBeeWifi || loggerModemChip == sim_chip_XBeeCell)
            {
                // Connect to the network
                // Only waiting for up to 5 seconds here for the internet!
                if (!(_modem->isNetworkConnected()))
                {
                    MS_MOD_DBG(F("No prior internet connection, attempting to make a connection."));
                    connectInternet(5000L);
                }
                // Must ensure that we do not ping the daylight more than once every 4 seconds
                // NIST clearly specifies here that this is a requirement for all software
                /// that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
                while (millis() < _lastNISTrequest + 4000) {}
                MS_MOD_DBG("Connecting to NIST daytime server to check connection strength...\n");
                IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
                openTCP(ip, 37);
                _client->print(F("Hi!"));  // Need to send something before connection is made
                delay(100); // Need this delay!  Can get away with 50, but 100 is safer.
                while (_client->available()) _client->read();  // Delete anything returned
                _lastNISTrequest = millis();
            }

            // Get signal quality
            // Non XBee's do not need to be registered or "connnected" to the
            // network to get quality
            // if (_modem->isNetworkConnected())
            // {

                MS_MOD_DBG("Getting signal quality:\n");
                signalQual = _modem->getSignalQuality();

                // Convert signal quality to RSSI, if necessary
                if (loggerModemChip == sim_chip_XBeeWifi ||
                    loggerModemChip == sim_chip_XBeeCell ||
                    loggerModemChip == sim_chip_ESP8266)
                {
                    rssi = signalQual;
                    percent = getPctFromRSSI(signalQual);
                }
                else
                {
                    rssi = getRSSIFromCSQ(signalQual);
                    percent = getPctFromCSQ(signalQual);
                }
            // }
            // else MS_MOD_DBG("Insufficient signal to connect to the internet!\n");
        }
        else MS_MOD_DBG(F("\nModem does not respond to AT commands!\n"));

        MS_MOD_DBG(F("RSSI: "), rssi, F("\n"));
        MS_MOD_DBG(F("Percent signal strength: "), percent, F("\n"));

        verifyAndAddMeasurementResult(RSSI_VAR_NUM, rssi);
        verifyAndAddMeasurementResult(PERCENT_SIGNAL_VAR_NUM, percent);

        // Unset the time stamp for the beginning of this measurement
        _millisMeasurementRequested = 0;
        // Unset the status bit for a measurement having been requested (bit 5)
        _sensorStatus &= 0b11011111;
        // Set the status bit for measurement completion (bit 6)
        _sensorStatus |= 0b01000000;

        return true;
    }


// ==========================================================================//
// These are the unique functions for the modem as an internet connected device
// ==========================================================================//
public:

    int getSignalRSSI(void) {return sensorValues[RSSI_VAR_NUM];}
    int getSignalPercent(void) {return sensorValues[PERCENT_SIGNAL_VAR_NUM];}

    bool connectInternet(uint32_t waitTime_ms = 50000L)
    {
        bool retVal = false;

        // Check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) modemOnOff->on();
        // Check again if the modem is on.  If it still isn't on, give up
        if(!modemOnOff->isOn())
        {
            MS_MOD_DBG(F("\nModem failed to turn on!\n"));
            return false;
        }

        // Check that the modem is responding to AT commands.  If not, give up.
        MS_MOD_DBG(F("\nWaiting up to 5 seconds for modem to respond to AT commands...\n"));
        if (!_modem->testAT(5000))
        {
            MS_MOD_DBG(F("\nModem does not respond to AT commands!\n"));
            return false;
        }

        // WiFi modules immediately re-connect to the last access point so we
        // can save just a tiny bit of time (and thus power) by not resending
        // the credentials every time.  (True for both ESP8266 and Wifi XBee)
        if (_ssid)
        {
            MS_MOD_DBG(F("\nWaiting up to "), waitTime_ms/1000,
                       F(" seconds for WiFi network...\n"));
            if (!(_modem->isNetworkConnected()))
            {
                MS_MOD_DBG("   Sending credentials...\n");
                #if defined(TINY_GSM_MODEM_HAS_WIFI)
                // make multiple attempts to send credentials
                while (!_modem->networkConnect(_ssid, _pwd)) {};
                #endif
                if (_modem->waitForNetwork(waitTime_ms))
                {
                    retVal = true;
                    MS_MOD_DBG("   ... Connected!\n");
                }
                else MS_MOD_DBG("   ... Connection failed\n");
            }
            else
            {
                MS_MOD_DBG("   ... Connected with saved WiFi settings!\n");
                retVal = true;
            }

        }
        else
        {
            MS_MOD_DBG(F("\nWaiting up to "), waitTime_ms/1000,
                       F(" seconds for cellular network...\n"));
            if (_modem->waitForNetwork(waitTime_ms))
            {
                #if defined(TINY_GSM_MODEM_HAS_GPRS)
                _modem->gprsConnect(_APN, "", "");
                #endif
                MS_MOD_DBG("   ...Connected!\n");
                retVal = true;
            }
            else MS_MOD_DBG("   ...Connection failed.\n");
        }
        _millisSensorActivated = millis();
        return retVal;
    }

    void disconnectInternet(void)
    {
        if (loggerModemChip != sim_chip_XBeeWifi &&
            loggerModemChip != sim_chip_ESP8266)
        {
            #if defined(TINY_GSM_MODEM_HAS_GPRS)
            _modem->gprsDisconnect();
            #endif
            MS_MOD_DBG(F("Disconnected from cellular network.\n"));
        }
        else{}
            // _modem->networkDisconnect();  // Eh.. why bother?
            // MS_MOD_DBG(F("Disconnected from WiFi network.\n"));
        _millisSensorActivated = 0;
    }

    int openTCP(const char *host, uint16_t port)
    {
        MS_MOD_DBG("Connecting to ", host, "...");
        int ret_val = _client->connect(host, port);
        if (ret_val) MS_MOD_DBG("   ...Success!\n");
        else MS_MOD_DBG("   ...Connection failed.\n");
        return ret_val;
    }

    int openTCP(IPAddress ip, uint16_t port)
    {
        MS_MOD_DBG("Connecting to ", ip, "...");
        int ret_val = _client->connect(ip, port);
        if (ret_val) MS_MOD_DBG("   ...Success!\n");
        else MS_MOD_DBG("   ...Connection failed.\n");
        return ret_val;
    }

    void closeTCP(void)
    {
        _client->stop();
        MS_MOD_DBG(F("Closed TCP/IP.\n"));
    }

    bool modemPowerUp(void)
    {
        MS_MOD_DBG(F("Turning modem on.\n"));
        // Turn the modem on .. whether it was on or not
        // Need to turn on no matter what because some modems don't have an
        // effective way of telling us whether they're on or not
        modemOnOff->on();
        // Double check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) modemOnOff->on();
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
        // Set the status bit for sensor power (bit 0)
        _sensorStatus |= 0b00000001;
        return modemOnOff->isOn();
    }

    bool modemPowerDown(void)
    {
        MS_MOD_DBG(F("Turning modem off.\n"));
         // Wait for any sending to complete
        _client->flush();
        // Turn the modem off .. whether it was on or not
        // Need to turn off no matter what because some modems don't have an
        // effective way of telling us whether they're on or not
        modemOnOff->off();
        // Unset the status bits for sensor power (bit 0), warm-up (bit 2),
        // activation (bit 3), stability (bit 4), measurement request (bit 5), and
        // measurement completion (bit 6)
        _sensorStatus &= 0b10000010;
        return true;
    }

    // Get the time from NIST via TIME protocol (rfc868)
    // This would be much more efficient if done over UDP, but I'm doing it
    // over TCP because I don't have a UDP library for all the modems.
    uint32_t getNISTTime(void)
    {
        bool connectionMade = false;
        // bail if not connected to the internet
        // TODO:  Figure out why _modem->isNetworkConnected() isn't working here
        // if (!_modem->isNetworkConnected())
        if (!(connectInternet(1000)))
        {
            MS_MOD_DBG(F("No internet connection, cannot connect to NIST.\n"));
            return 0;
        }

        // Must ensure that we do not ping the daylight more than once every 4 seconds
        // NIST clearly specifies here that this is a requirement for all software
        /// that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() < _lastNISTrequest + 4000) {}

        // Make TCP connection
        MS_MOD_DBG(F("Connecting to NIST daytime Server\n"));
        if (loggerModemChip == sim_chip_XBeeWifi || loggerModemChip == sim_chip_XBeeCell)
        {
            IPAddress ip(129, 6, 15, 30);  // This is the IP address of time-c-g.nist.gov
            connectionMade = openTCP(ip, 37);  // XBee's address lookup falters on time.nist.gov
            _client->print(F("Hi!"));
            delay(100); // Need this delay!  Can get away with 50, but 100 is safer.
        }
        else connectionMade = openTCP("time.nist.gov", 37);

        // Wait up to 5 seconds for a response
        if (connectionMade)
        {
            long start = millis();
            while (_client->available() < 4 && millis() - start < 5000){}

            // Response is returned as 32-bit number as soon as connection is made
            // Connection is then immediately closed, so there is no need to close it
            uint32_t secFrom1900 = 0;
            byte response[4] = {0};
            for (uint8_t i = 0; i < 4; i++)
            {
                response[i] = _client->read();
                // MS_MOD_DBG("\n",response[i]);
                secFrom1900 += 0x000000FF & response[i];
                // MS_MOD_DBG("\n*****",String(secFrom1900, BIN),"*****");
                if (i+1 < 4) {secFrom1900 = secFrom1900 << 8;}
            }
            // MS_MOD_DBG("\n*****",secFrom1900,"*****");

            // Return the timestamp
            uint32_t unixTimeStamp = secFrom1900 - 2208988800;
            MS_MOD_DBG(F("Timestamp returned by NIST (UTC): "), unixTimeStamp, '\n');
            // If before Jan 1, 2017 or after Jan 1, 2030, most likely an error
            if (unixTimeStamp < 1483228800) return 0;
            else if (unixTimeStamp > 1893456000) return 0;
            else return unixTimeStamp;
        }
        else MS_MOD_DBG(F("Unable to open TCP to NIST\n"));
        return 0;
    }

public:
    ModemOnOff *modemOnOff;
    ModemChipType loggerModemChip;

    TinyGsm *_modem;
    TinyGsmClient *_client;

private:
    Stream* _stream;
    int8_t _vcc33Pin;
    int8_t _modemStatusPin;
    int8_t _modemSleepRqPin;
    ModemSleepType _sleepType;
    const char *_APN;
    const char *_ssid;
    const char *_pwd;
    uint32_t _lastNISTrequest;

private:
    void init(Stream *modemStream, int vcc33Pin, int modemStatusPin, int modemSleepRqPin,
              ModemSleepType sleepType)
    {
        #if defined(TINY_GSM_MODEM_XBEE)  // ALL XBee's use modem_sleep_reverse!
            if (sleepType != modem_sleep_reverse)
            {
                MS_MOD_DBG(F("Correcting sleep type for XBee to modem_sleep_reverse!"));
                sleepType = modem_sleep_reverse;
            }
        #endif

        // Set up the method for putting the modem to sleep
        switch(sleepType)
        {
            case modem_sleep_pulsed:
            {
                MS_MOD_DBG(F("Creating a new on/off method for the "), F(MODEM_NAME),
                    F(" with power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and on/off via 2.5 second pulse on pin "), modemSleepRqPin, F(".\n"));
                static pulsedOnOff modem_sleep_pulsed;
                modemOnOff = &modem_sleep_pulsed;
                modem_sleep_pulsed.init(vcc33Pin, modemSleepRqPin, modemStatusPin, true);
                break;
            }
            case modem_sleep_held:
            {
                MS_MOD_DBG(F("Creating a new on/off method for the "), F(MODEM_NAME),
                    F(" with power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and on/off by holding pin "), modemSleepRqPin, F(" high.\n"));
                static heldOnOff modem_sleep_held;
                modemOnOff = &modem_sleep_held;
                modem_sleep_held.init(vcc33Pin, modemSleepRqPin, modemStatusPin, true);
                break;
            }
            case modem_sleep_reverse:
            {
                MS_MOD_DBG(F("Creating a new on/off method for the "), F(MODEM_NAME),
                    F(" with power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and on/off by holding pin "), modemSleepRqPin, F(" low.\n"));
                static heldOnOff modem_sleep_held;
                modemOnOff = &modem_sleep_held;
                modem_sleep_held.init(vcc33Pin, modemSleepRqPin, modemStatusPin, false);
                break;
            }
            default:
            {
                MS_MOD_DBG(F("Creating a new on/off method for the "), F(MODEM_NAME),
                    F(" with power on pin "), vcc33Pin,
                    F(" status on pin "), modemStatusPin,
                    F(" and no on/off pin.\n"));
                static heldOnOff modem_sleep_held;
                modemOnOff = &modem_sleep_held;
                modem_sleep_held.init(vcc33Pin, -1, modemStatusPin);
                break;
            }
        }

        // Initialize the modem
        MS_MOD_DBG(F("Creating a new TinyGSM modem and client for the "),
               F(MODEM_NAME), F("...\n"));
        static TinyGsm modem(*modemStream);
        _modem = &modem;
        static TinyGsmClient client(modem);
        _client = &client;

        MS_MOD_DBG(F("Initializing "), F(MODEM_NAME), F("...\n"));
        String XBeeChip;
        // Turn the modem on .. whether it was on or not
        // Need to turn on no matter what because some modems don't have an
        // effective way of telling us whether they're on or not
        modemOnOff->on();
        // Double check if the modem is on; turn it on if not
        if(!modemOnOff->isOn()) modemOnOff->on();
        // Check again if the modem is on.  Only "begin" if it responded.
        if(modemOnOff->isOn())
        {
            _modem->begin();
            #if defined(TINY_GSM_MODEM_XBEE)
                if (sleepType != modem_always_on) _modem->setupPinSleep();
                XBeeChip = _modem->getBeeType();
            #endif

            modemOnOff->off();
            MS_MOD_DBG(F("   ... Complete!\n"));
        }
        else MS_MOD_DBG(F("   ... Modem failed to turn on!\n"));

        // Assign a chip type
        #if defined(TINY_GSM_MODEM_SIM800)
            loggerModemChip = sim_chip_SIM800;
        #elif defined(TINY_GSM_MODEM_SIM808)
            loggerModemChip = sim_chip_SIM808;
        #elif defined(TINY_GSM_MODEM_SIM868)
            loggerModemChip = sim_chip_SIM868;
        #elif defined(TINY_GSM_MODEM_SIM900)
            loggerModemChip = sim_chip_SIM900;
        #elif defined(TINY_GSM_MODEM_A6)
            loggerModemChip = sim_chip_A6;
        #elif defined(TINY_GSM_MODEM_A7)
            loggerModemChip = sim_chip_A7;
        #elif defined(TINY_GSM_MODEM_M590)
            loggerModemChip = sim_chip_M590;
        #elif defined(TINY_GSM_MODEM_U201)
            loggerModemChip = sim_chip_U201;
        #elif defined(TINY_GSM_MODEM_ESP8266)
            loggerModemChip = sim_chip_ESP8266;
        #elif defined(TINY_GSM_MODEM_XBEE)
            if (XBeeChip = "S6B Wifi") loggerModemChip = sim_chip_XBeeWifi;
            else loggerModemChip = sim_chip_XBeeCell;
        #else
            loggerModemChip = sim_chip_SIM808;
        #endif

        // MS_MOD_DBG(F("Modem chip: "), loggerModemChip, '\n');

        // Set the status bit marking that the modem has been set up (bit 1)
        _sensorStatus |= 0b00000010;
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
    Modem_RSSI(Sensor *parentSense, const char *UUID = "", const char *customVarCode = "")
     : Variable(parentSense, RSSI_VAR_NUM,
                "RSSI", "decibelMiliWatt",
                RSSI_RESOLUTION,
                "RSSI", UUID, customVarCode)
    {}
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
};

// #endif /* USE_TINY_GSM */

#endif /* LoggerModem_h */
