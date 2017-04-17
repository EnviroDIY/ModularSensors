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

#define TINY_GSM_DEBUG Serial
#define TINY_GSM_YIELD() { delay(3);}

#if defined(MODEM_CHIP_SIM800)
  #define GSM_INCLUDE <TinyGsmClientSIM800.h>
#elif defined(MODEM_CHIP_A6)
  #define GSM_INCLUDE <TinyGsmClientA6.h>
#elif defined(MODEM_CHIP_M590)
  #define GSM_INCLUDE <TinyGsmClientM590.h>
#elif defined(MODEM_CHIP_ESP8266)
  #define GSM_INCLUDE <TinyGsmClientESP8266.h>
#elif defined(MODEM_CHIP_XBEE)
  #define GSM_INCLUDE <TinyGsmClientXBee.h>
// #else
//   #error "Please define GSM modem model"
#endif

#include GSM_INCLUDE

// For the various communication devices"
typedef enum DTRSleepType
{
  held = 0,  // Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
  pulsed,  // Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
  reverse,  // Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
  always_on
} DTRSleepType;


/* ===========================================================================
* Classes fot turning modems on and off
* TAKEN FROM SODAQ'S MODEM LIBRARIES
* ========================================================================= */

class ModemOnOff
{
public:
    ModemOnOff();
    virtual void init(int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin);
    virtual bool isOn(void);
    virtual bool on(void) = 0;
    virtual bool off(void) = 0;
protected:
    int8_t _vcc33Pin;
    int8_t _onoff_DTR_pin;
    int8_t _status_CTS_pin;

    void powerOn(void);
    void powerOff(void);
};

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
class pulsedOnOff : public ModemOnOff
{
public:
    bool on(void) override;
    bool off(void) override;
private:
    void pulse(void);
};

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
class heldOnOff : public ModemOnOff
{
public:
    bool on(void) override;
    bool off(void) override;
};

// Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
class reverseOnOff : public ModemOnOff
{
public:
    bool isOn(void) override;
    bool on(void) override;
    bool off(void) override;
};


/* ===========================================================================
* The modem class
* ========================================================================= */

class loggerModem
{
public:
    void setupModem(Stream *modemStream,
                    int vcc33Pin,
                    int status_CTS_pin,
                    int onoff_DTR_pin,
                    DTRSleepType sleepType,
                    const char *APN);
    void setupModem(Stream *modemStream,
                    int vcc33Pin,
                    int status_CTS_pin,
                    int onoff_DTR_pin,
                    DTRSleepType sleepType,
                    const char *ssid,
                    const char *pwd);
    bool connectNetwork(void);
    void disconnectNetwork(void);
    int connect(const char *host, uint16_t port);
    void stop(void);
    void dumpBuffer(Stream *stream = _modemStream, int timeDelay = 5, int timeout = 5000);
    // static void printHTTPResult(int HTTPcode);
    static Stream *_modemStream;
    ModemOnOff *modemOnOff;
private:
    void init(Stream *modemStream, int vcc33Pin, int status_CTS_pin, int onoff_DTR_pin,
              DTRSleepType sleepType);
    const char *_APN;
    const char *_ssid;
    const char *_pwd;

    TinyGsm *_modem;
    TinyGsmClient *_client;
};



#endif /* modem_onoff_h */
