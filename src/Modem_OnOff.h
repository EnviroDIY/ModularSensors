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

// For the "Bee" devices"
typedef enum modems
{
    GPRSBeev4 = 0,  // Sodaq GPRSBee v4 - 2G (GPRS) communication
    GPRSBeev6,  // Sodaq GPRSBee v6 - 2G (GPRS) communication
    WIFIBee,  // Digi XBee S6B - WiFi communication
    Fona,  // Adafruit Fona - for our purposes, it operates like a GPRSBee v4
    // RADIO,
    // THREEG
} modems;

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
    virtual ~OnOff() {}
    virtual void on() = 0;
    virtual void off() = 0;
    virtual bool isOn() = 0;
};

// A specialized class to switch on/off the GPRSbee module
// The VCC3.3 pin is switched by the Autonomo BEE_VCC pin
// The DTR pin is the actual ON/OFF pin, it is A13 on Autonomo, D20 on Tatu
class GPRSbeeOnOff : public OnOff
{
public:
    GPRSbeeOnOff();
    void init(int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin, GPRSVersion version = V06);
    void on();
    void off();
    bool isOn();
private:
    int8_t _vcc33Pin;
    int8_t _onoff_DTR_pin;
    int8_t _status_CTS_pin;
    GPRSVersion _version;
};




#endif /* modem_onoff_h */
