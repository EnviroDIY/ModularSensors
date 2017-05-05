/*
 *ModemSupport.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for turning modems on and off to save power.  It is more-or-less
 *a wrapper for tinyGSM library:  https://github.com/vshymanskyy/TinyGSM
*/

#ifndef modem_onoff_h
#define modem_onoff_h

#include <Arduino.h>

#if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900) || \
    defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
    defined(TINY_GSM_MODEM_M590) || defined(TINY_GSM_MODEM_ESP8266) || \
    defined(TINY_GSM_MODEM_XBEE)
  #define USE_TINY_GSM
  // #define TINY_GSM_DEBUG Serial
  #if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900)
    #define TINY_GSM_YIELD() { delay(3);}
  #endif
  #include <TinyGsmClient.h>
#else
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
* Classes for turning modems on and off
* IDEA FOR THIS TAKEN FROM SODAQ'S MODEM LIBRARIES
* ========================================================================= */


/* ===========================================================================
* Functions for the OnOff class
* ========================================================================= */

class ModemOnOff
{
public:
    // Constructor
    ModemOnOff()
    {
        _vcc33Pin = -1;
        _onoff_DTR_pin = -1;
        _status_CTS_pin = -1;
    }

    // Initializes the instance
    virtual void init(int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin)
    {
        DBG(F("Initializing modem on/off..."));
        if (vcc33Pin >= 0) {
          _vcc33Pin = vcc33Pin;
          // First write the output value, and only then set the output mode.
          digitalWrite(_vcc33Pin, LOW);
          pinMode(_vcc33Pin, OUTPUT);
        }
        if (onoff_DTR_pin >= 0) {
            _onoff_DTR_pin = onoff_DTR_pin;
            // First write the output value, and only then set the output mode.
            digitalWrite(_onoff_DTR_pin, LOW);
            pinMode(_onoff_DTR_pin, OUTPUT);
        }
        if (status_CTS_pin >= 0) {
            _status_CTS_pin = status_CTS_pin;
            pinMode(_status_CTS_pin, INPUT);
        }
        DBG(F("   ... Success!\n"));
    }

    virtual bool isOn(void)
    {
        if (_status_CTS_pin >= 0) {
            bool status = digitalRead(_status_CTS_pin);
            // DBG(F("Is modem on? "), status, F("\n"));
            return status;
        }
        // No status pin. Let's assume it is on.
        return true;
    }

    virtual bool on(void) = 0;

    virtual bool off(void) = 0;

protected:
    int8_t _vcc33Pin;
    int8_t _onoff_DTR_pin;
    int8_t _status_CTS_pin;

    void powerOn(void)
    {
        if (_vcc33Pin >= 0) {
            digitalWrite(_vcc33Pin, HIGH);
            DBG(F("Sending power to modem.\n"));
        }
    }

    void powerOff(void)
    {
        if (_vcc33Pin >= 0) {
            digitalWrite(_vcc33Pin, LOW);
            DBG(F("Cutting modem power.\n"));
        }
    }

};



/* ===========================================================================
* Functions for pulsed method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
class pulsedOnOff : public ModemOnOff
{
public:
    bool on(void) override
    {
        powerOn();
        DBG(F("Pulsing modem to on with pin "));
        DBG(_onoff_DTR_pin, F("\n"));
        if (!isOn()) {pulse();}
        // Wait until is actually on
        for (unsigned long start = millis(); millis() - start < 10000; )
        {
            if (isOn())
            {
                DBG(F("Modem now on.\n"));
                return true;
            }
          delay(5);
        }
        DBG(F("Failed to turn modem on.\n"));
        return false;
    }

    bool off(void) override
    {
        if (isOn()) {pulse();}
        // else DBG(F("Modem was not ever on.\n"));
        // Wait until is off
        for (unsigned long start = millis(); millis() - start < 10000; )
        {
            if (!isOn())
            {
                DBG(F("Modem now off.\n"));
                powerOff();
                return true;
            }
            delay(5);
        }
        DBG(F("Failed to turn modem off.\n"));
        powerOff();
        return false;
    }

private:
    void pulse(void)
    {
        if (_onoff_DTR_pin >= 0)
        {
            digitalWrite(_onoff_DTR_pin, LOW);
            delay(200);
            digitalWrite(_onoff_DTR_pin, HIGH);
            delay(2500);
            digitalWrite(_onoff_DTR_pin, LOW);
        }
    }
};


/* ===========================================================================
* Functions for held method.
* This turns the modem on by setting the onoff/DTR/Key pin high and off by
* setting it low.
* This is used by the Sodaq GPRSBee v0.6.
* ========================================================================= */

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
class heldOnOff : public ModemOnOff
{
public:
    bool on(void) override
    {
        powerOn();
        if (_onoff_DTR_pin <= 0) {return true;}
        else
        {
            DBG(F("Turning modem on by setting pin "));
            DBG(_onoff_DTR_pin);
            DBG(F(" high\n"));
            digitalWrite(_onoff_DTR_pin, HIGH);
            // Wait until is actually on
            for (unsigned long start = millis(); millis() - start < 10000; )
            {
                if (isOn())
                {
                    DBG(F("Modem now on.\n"));
                    return true;
                }
                delay(5);
            }
            DBG(F("Failed to turn modem on.\n"));
            return false;
        }
    }

    bool off(void) override
    {
        if (_onoff_DTR_pin <= 0) {return true;}
        else
        {
            if (!isOn()) DBG(F("Modem was not ever on.\n"));
            digitalWrite(_onoff_DTR_pin, LOW);
            // Wait until is off
            for (unsigned long start = millis(); millis() - start < 10000; )
            {
                if (!isOn())
                {
                    DBG(F("Modem now off.\n"));
                    powerOff();
                    return true;
                }
                delay(5);
            }
            DBG(F("Failed to turn modem off.\n"));
            powerOff();
            return false;
        }
    }
};


/* ===========================================================================
* Functions for reverse method.
* This turns the modem on by setting the onoff/DTR/Key pin LOW and off by
* setting it HIGH.
* This is used by the XBee's
* ========================================================================= */

// Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
class reverseOnOff : public ModemOnOff
{
public:
    bool isOn(void) override
    {
        if (_status_CTS_pin >= 0) {
            bool status = digitalRead(_status_CTS_pin);
            // DBG(F("Is modem on? "), status, F("\n"));
            return !status;
        }
        // No status pin. Let's assume it is on.
        return true;
    }

    bool on(void) override
    {
        powerOn();
        DBG(F("Turning modem on on by setting pin "));
        DBG(_onoff_DTR_pin);
        DBG(F(" low\n"));
        if (_onoff_DTR_pin >= 0) {
            digitalWrite(_onoff_DTR_pin, LOW);
        }
        // Wait until is actually on
        for (unsigned long start = millis(); millis() - start < 10000; )
        {
            if (isOn())
            {
                DBG(F("Modem now on.\n"));
                return true;
            }
            delay(5);
        }
        DBG(F("Failed to turn modem on.\n"));
        return false;
    }

    bool off(void) override
    {
        // if (!isOn()) DBG(F("Modem was not ever on.\n"));
        if (_onoff_DTR_pin >= 0) {
            digitalWrite(_onoff_DTR_pin, HIGH);
        }
        // Wait until is off
        for (unsigned long start = millis(); millis() - start < 10000; )
        {
            if (!isOn())
            {
                DBG(F("Modem now off.\n"));
                powerOff();
                return true;
            }
            delay(5);
        }
        DBG(F("Failed to turn modem off.\n"));
        powerOff();
        return false;
    }
};


/* ===========================================================================
* Functions for the modem class
* This is basically a wrapper for TinyGsm
* ========================================================================= */

class loggerModem
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

    bool connectNetwork(void)
    {
        bool retVal = false;

        #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
        if (_ssid)
        {
            if(!modemOnOff->isOn())modemOnOff->on();
            DBG(F("\nConnecting to WiFi network...\n"));
            if (!_modem->waitForNetwork(15000L)){
                DBG("... Connection failed.  Resending credentials...", F("\n"));
                _modem->networkConnect(_ssid, _pwd);
                if (!_modem->waitForNetwork(45000L)){
                    DBG("... Connection failed", F("\n"));
                } else {
                    retVal = true;
                    DBG("... Success!", F("\n"));
                }
            } else {
                DBG("... Success!", F("\n"));
                retVal = true;
            }
        }
        else
        {
        #endif
        #if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900) || \
            defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
            defined(TINY_GSM_MODEM_M590) || defined(TINY_GSM_MODEM_XBEE)
            if(!modemOnOff->isOn())modemOnOff->on();
            DBG(F("\nWaiting for cellular network...\n"));
            if (!_modem->waitForNetwork(60000L)){
                DBG("... Connection failed.", F("\n"));
            } else {
                _modem->gprsConnect(_APN, "", "");
                DBG("... Success!", F("\n"));
                retVal = true;
            }
        #endif
        #if defined(TINY_GSM_MODEM_XBEE) || defined(TINY_GSM_MODEM_ESP8266)
        }
        #endif

        return retVal;
    }

    void disconnectNetwork(void)
    {
    #if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900) || \
        defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
        defined(TINY_GSM_MODEM_M590) || defined(TINY_GSM_MODEM_XBEE)
        _modem->gprsDisconnect();
    #endif
    }

    int connect(const char *host, uint16_t port)
    {
    #if defined(USE_TINY_GSM)
        return  _client->connect(host, port);
    #else
        return 0;
    #endif
    }

    void stop(void)
    {
    #if defined(USE_TINY_GSM)
        return _client->stop();
    #endif
    }

    // Used to empty out the buffer after a post request.
    // Removing this may cause communication issues. If you
    // prefer to not see the std::out, remove the print statement
    void dumpBuffer(Stream *stream, int timeDelay = 5, int timeout = 5000)
    {
        while (stream->available() > 0)
        {
            char c[2] = {0};
            stream->readBytes(c, 1);  // readBytes includes a timeout
            if(c[0]) DBG(c[0]);
            delay(timeDelay);
        }
        DBG(F("\n"));
        stream->flush();
    }

    Stream *_modemStream;
    ModemOnOff *modemOnOff;

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

        #if defined(USE_TINY_GSM)
            // Initialize the modem
            DBG(F("Initializing GSM modem instance..."));
            modemStream->setTimeout(200);
            static TinyGsm modem(*modemStream);
            _modem = &modem;
            static TinyGsmClient client(modem);
            _client = &client;
            modemOnOff->on();
            _modem->begin();
            // _client->stop();  // Close any open sockets, just in case
            #if defined(TINY_GSM_MODEM_XBEE)
                _modem->setupPinSleep();
            #endif
            modemOnOff->off();
            _modemStream = _client;
            DBG(F("   ... Complete!\n"));
        #else
            _modemStream = modemStream;
        #endif
    }

    const char *_APN;
    const char *_ssid;
    const char *_pwd;

#if defined(USE_TINY_GSM)
    TinyGsm *_modem;
    TinyGsmClient *_client;
#endif
};

#endif /* modem_onoff_h */
