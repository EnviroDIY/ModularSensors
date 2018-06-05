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

// For the various ways of waking and sleeping the modem
typedef enum ModemSleepType
{
  modem_sleep_held = 0,   // Turns the modem on by setting the onoff/DTR/Key HIGH and off by setting it LOW
  modem_sleep_reverse,    // Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
  modem_sleep_pulsed,     // Turns the modem on and off by pulsing the onoff/DTR/Key pin HIGH for 2 seconds
  modem_sleep_rev_pulse,  // Turns the modem on and off by pulsing the onoff/DTR/Key pin LOW 2 seconds
  modem_always_on
} ModemSleepType;


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
    ModemOnOff(int vcc33Pin, int modemSleepRqPin, int modemStatusPin,
               ModemSleepType sleepType);

    // Initializes the instance
    virtual void begin(void);

    virtual bool isOn(void);
    virtual bool on(void);
    virtual bool off(void);
    ModemSleepType _sleepType;

private:
    int8_t _vcc33Pin;
    int8_t _modemSleepRqPin;
    int8_t _modemStatusPin;
    bool _isNowOn;

    void powerOn(void);
    void powerOff(void);
    void pulse(void);

};

#endif /* ModemOnOff_h */
