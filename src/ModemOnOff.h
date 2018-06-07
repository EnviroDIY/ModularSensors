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

template <class T>
class ModemOnOff
{
public:
    // Constructor
    ModemOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin, int8_t modemStatusPin,
                           bool isHighWhenOn)
      : _vcc33Pin(vcc33Pin), _modemSleepRqPin(modemSleepRqPin), _modemStatusPin(modemStatusPin),
        _isHighWhenOn(isHighWhenOn)
    {}


    // Checks if the status pin is reading on
    bool isOn(void)
    {
        if (_modemStatusPin >= 0)
        {
            bool status = digitalRead(_modemStatusPin);
            if (!_isHighWhenOn) status = !status;
            // MS_DBG(F("Is modem on? "), status, F("\n"));
            return status;
        }
        // No status pin. Return the "internal" status code.
        return _isNowOn;
    }

    // Begins the instance - ie, sets pin modes
    // This is the stuff that cannot happen in the constructor
    void begin(void)
    {
        MS_DBG(F("Initializing modem on/off with power on pin "), _vcc33Pin,
               F(" status on pin "), _modemStatusPin,
               F(" and on/off via pin "), _modemSleepRqPin, F(".\n"));

        // Set pin modes
        if (_vcc33Pin >= 0)
        {
            pinMode(_vcc33Pin, OUTPUT);  // Set pin mode
            digitalWrite(_vcc33Pin, LOW);  // Set power off
        }
        if (_modemSleepRqPin >= 0)
        {
            pinMode(_modemSleepRqPin, OUTPUT);  // Set pin mode
            digitalWrite(_modemSleepRqPin, !_isHighWhenOn);  // Set to off
        }
        if (_modemStatusPin >= 0)
        {
            pinMode(_modemStatusPin, INPUT_PULLUP);
        }

        // Initialize assuming modem is off
        _isNowOn = false;

        static_cast<T*>(this)->begin();  // CRTP = static polymorphism
    }

    bool on(void)
    {
      return static_cast<T*>(this)->on();  // CRTP = static polymorphism
    };
    bool off(void)
    {
      return static_cast<T*>(this)->on();  // CRTP = static polymorphism
    };

protected:
    int8_t _vcc33Pin;
    int8_t _modemSleepRqPin;
    int8_t _modemStatusPin;
    bool _isHighWhenOn;
    bool _isNowOn;

    // Function to supply power to the modem - sets power pin high
    void powerOn(void)
    {
        if (_vcc33Pin >= 0)
        {
            digitalWrite(_vcc33Pin, HIGH);
            MS_DBG(F("Sending power to modem.\n"));
        }
        else MS_DBG(F("No power control on modem.\n"));
    }


    // Function to cut power from the modem - sets power pin low
    void powerOff(void)
    {
        if (_vcc33Pin >= 0) {
            digitalWrite(_vcc33Pin, LOW);
            MS_DBG(F("Cutting modem power.\n"));
        }
        else MS_DBG(F("No power control on modem.\n"));
    }

};



/* ===========================================================================
* Functions for the pulsed on-off method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
// "On" can either be a high or low pulse
class pulsedOnOff : public ModemOnOff<pulsedOnOff>
{
public:
    pulsedOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin, int8_t modemStatusPin,
                bool isHighWhenOn);
    void begin(void);
    bool on(void);
    bool off(void);

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
class heldOnOff : public ModemOnOff<heldOnOff>
{
public:
    heldOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin, int8_t modemStatusPin, bool isHighWhenOn);
    void begin(void);
    bool on(void);
    bool off(void);
};

#endif /* ModemOnOff_h */
