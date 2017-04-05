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
#define TINY_GSM_MODEM_SIM800  // for debugging - should go in main sketch
#include <TinyGsmClient.h>

// For the various communication devices"
typedef enum modemType
{
    GPRSBee4 = 0,  // Sodaq GPRSBee v4 - 2G (GPRS) communication
    GPRSBee6,  // Sodaq GPRSBee v6 - 2G (GPRS) communication
    WIFIBee,  // Digi XBee S6B - WiFi communication
    Fona,  // Adafruit Fona - for our purposes, it operates like a GPRSBee v4
    // RADIO,
    // THREEG
} modemType;

// The versions of GPRSBees available
  typedef enum GPRSVersion {
    V04 = 0,
    V05,
    V06
} GPRSVersion;


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
    virtual void on(void) = 0;
    virtual void off(void) = 0;
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
    void on(void) override;
    void off(void) override;
private:
    void pulse(void);
};

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
class heldOnOff : public ModemOnOff
{
public:
    void on(void) override;
    void off(void) override;
};


/* ===========================================================================
* The modem class
* ========================================================================= */

class loggerModem
{
public:
    void setupModem(modemType modType,
                    Stream *modemStream,
                    int vcc33Pin,
                    int status_CTS_pin,
                    int onoff_DTR_pin,
                    const char *APN);
    bool connectNetwork(void);
    void disconnectNetwork(void);
    int connect(const char *host, uint16_t port);
    int connect(IPAddress ip, uint16_t port);
    void stop(void);
    void dumpBuffer(Stream *stream = _modemStream, int timeDelay = 5, int timeout = 5000);
    static void printHTTPResult(int HTTPcode);
    static Stream *_modemStream;
private:
    ModemOnOff *_modemOnOff;
    TinyGsm *_modem;
    TinyGsmClient *_client;
    modemType _modemType;
    const char *_APN;
};



#endif /* modem_onoff_h */
