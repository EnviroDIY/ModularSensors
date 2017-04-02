/*
 *Modem_OnOff.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for turning modems on and off to save power
*/

#ifndef modem_onoff_h
#define modem_onoff_h

#include <Arduino.h>

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


/*!  TAKEN FROM SODAQ'S MODEM LIBRARIES
 * \brief This class is used to switch on or off a (SODAQ) Bee device.
 *
 * It's a pure virtual class, so you'll have to implement a specialized
 * class.
 */
class OnOff
{
public:
    OnOff();
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
class pulsedOnOff : public OnOff
{
public:
    void on(void) override;
    void off(void) override;
private:
    void pulse(void);
};

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
class heldOnOff : public OnOff
{
public:
    void on(void) override;
    void off(void) override;
};




#endif /* modem_onoff_h */
