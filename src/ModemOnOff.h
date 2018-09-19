/*
 *ModemOnOff.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for turning modems on and off to save power.
*/

#ifndef ModemOnOff_h
#define ModemOnOff_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"


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
    ModemOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin, int8_t modemStatusPin,
               bool isHighWhenOn);

    // Begins the instance - ie, sets pin modes
    // This is the stuff that cannot happen in the constructor
    virtual void begin(void);

    // Check if the modem is currently on
    virtual bool isOn(void);

    // Turn the modem on and off
    virtual bool on(void) = 0;
    virtual bool off(void) = 0;

protected:
    int8_t _vcc33Pin;
    int8_t _modemSleepRqPin;
    int8_t _modemStatusPin;
    bool _isHighWhenOn;
    bool _isNowOn;

    // Turn the modem power on and off
    void powerOn(void);
    void powerOff(void);

};



/* ===========================================================================
* Functions for the pulsed on-off method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
// "On" can either be a high or low pulse
class pulsedOnOff : public ModemOnOff
{
public:
    pulsedOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin, int8_t modemStatusPin,
                bool isHighWhenOn);
    void begin(void) override;
    bool on(void) override;
    bool off(void) override;

private:
    void pulse(void);

};


/* ===========================================================================
* Functions for the held on-off method.
* This turns the modem on by setting and holding the onoff/DTR/Key pin to
* either high or low.
* A "high" on is used by the Sodaq GPRSBee v0.6 and Sodaq 3GBee.
* A "low" on is used by the all Digi XBee's.
* ========================================================================= */

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
class heldOnOff : public ModemOnOff
{
public:
    heldOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin, int8_t modemStatusPin, bool isHighWhenOn);
    void begin(void) override;
    bool on(void) override;
    bool off(void) override;
};

#endif /* ModemOnOff_h */
