/*
 *ModemOnOff.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for turning modems on and off to save power.
*/

#include "ModemOnOff.h"


/* ===========================================================================
* Classes for turning modems on and off
* IDEA FOR THIS TAKEN FROM SODAQ'S MODEM LIBRARIES
* ========================================================================= */

/* ===========================================================================
* Functions for the OnOff class
* ========================================================================= */


// Constructor
ModemOnOff::ModemOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin, int8_t modemStatusPin,
                       bool isHighWhenOn)
  : _vcc33Pin(vcc33Pin), _modemSleepRqPin(modemSleepRqPin), _modemStatusPin(modemStatusPin),
    _isHighWhenOn(isHighWhenOn)
{}


// Begins the instance - ie, sets pin modes
// This is the stuff that cannot happen in the constructor
void ModemOnOff::begin(void)
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
}

// Function to check if the modem is currently on
// That is, checks if the status pin is reading on
bool ModemOnOff::isOn(void)
{
    if (_modemStatusPin >= 0)
    {
        bool status = digitalRead(_modemStatusPin);
        if (!_isHighWhenOn) status = !status;
        // MS_DBG(F("Is modem on? "), status, F("\n"));
        return status;
    }
    // No status pin. Return true.
    return true;
}

// Function to supply power to the modem - sets power pin high
void ModemOnOff::powerOn(void)
{
    if (_vcc33Pin >= 0)
    {
        digitalWrite(_vcc33Pin, HIGH);
        MS_DBG(F("Sending power to modem.\n"));
    }
    else MS_DBG(F("No power control on modem.\n"));
}

// Function to cut power from the modem - sets power pin low
void ModemOnOff::powerOff(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, LOW);
        MS_DBG(F("Cutting modem power.\n"));
    }
    else MS_DBG(F("No power control on modem.\n"));
}



/* ===========================================================================
* Functions for the pulsed on-off method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

pulsedOnOff::pulsedOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin,
                         int8_t modemStatusPin, bool isHighWhenOn)
  : ModemOnOff(vcc33Pin, modemSleepRqPin, modemStatusPin, isHighWhenOn)
 {}

void pulsedOnOff::begin(void)
{
    ModemOnOff::begin();
    MS_DBG(F("Pin "), _modemSleepRqPin, F(" will be pulsed to "), _isHighWhenOn,
           F(" for 2.5 seconds to turn modem on.\n"));
}

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
bool pulsedOnOff::on(void)
{
    // Power up
    powerOn();

    // If no pin assigned to turn it on or off, assume it's on and return
    if (_modemSleepRqPin < 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is on/awake."));
        return true;
    }

    // Check if it's already on before sending pulse
    if (_modemStatusPin >= 0 && isOn())
    {
        MS_DBG(F("Modem was already on.\n"));
        return true;
    }
    else
    {
        MS_DBG(F("Turning modem on with a "), _isHighWhenOn, F(" pulse on pin "),
               _modemSleepRqPin, F(".\n"));
        pulse();

        // If we can't tell if we've succeeded, assume we have and exit
        if (_modemStatusPin < 0)
        {
            MS_DBG(F("Assume modem now on.\n"));
            return true;
        }

        // If there's a status pin available, wait until modem shows it's on
        for (uint32_t start = millis(); millis() - start < 10000L; )
        {
            if (isOn())
            {
                MS_DBG(F("Modem on after "), millis() - start, F("ms.\n"));
                return true;
            }
        }

        // If the modem doesn't show it's on within 10 seconds, return false
        MS_DBG(F("Failed to turn modem on!\n"));
        return false;
    }
}

bool pulsedOnOff::off(void)
{
    // If no pin assigned to turn it on or off, assume it's off and return
    if (_modemSleepRqPin < 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is off/asleep."));
        return true;
    }

    // Check if it's already off before sending pulse
    if (_modemStatusPin >= 0 && !isOn())
    {
        MS_DBG(F("Modem was not ever on.\n"));
        return true;
    }
    else
    {
        MS_DBG(F("Turning modem off with a "), !_isHighWhenOn, F(" pulse on pin "),
               _modemSleepRqPin, F(".\n"));
        pulse();

        // If we can't tell if we've succeeded, assume we have and exit
        if (_modemStatusPin < 0)
        {
            MS_DBG(F("Assume modem now on.\n"));
            return true;
        }

        // If there's a status pin available, wait until modem shows it's off
        for (uint32_t start = millis(); millis() - start < 10000L; )
        {
            if (!isOn())
            {
                MS_DBG(F("Modem off after "), millis() - start, F("ms.\n"));
                powerOff();
                return true;
            }
        }

        // If the modem doesn't show it's off within 10 seconds, cut the power
        // anyway and return true
        MS_DBG(F("Failed to turn modem off with on/sleep pin!\n"));
        // Power down anyway
        powerOff();
        return false;
    }
}

void pulsedOnOff::pulse(void)
{
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, !_isHighWhenOn);
        delay(200);
        digitalWrite(_modemSleepRqPin, _isHighWhenOn);
        delay(2500);
        digitalWrite(_modemSleepRqPin, !_isHighWhenOn);
    }
}


/* ===========================================================================
* Functions for the held on-off method.
* This turns the modem on by setting and holding the onoff/DTR/Key pin to
* either high or low.
* A "high" on is used by the Sodaq GPRSBee v0.6 and Sodaq 3GBee.
* A "low" on is used by the all Digi XBee's.
* ========================================================================= */

heldOnOff::heldOnOff(int8_t vcc33Pin, int8_t modemSleepRqPin,
                     int8_t modemStatusPin, bool isHighWhenOn)
  : ModemOnOff(vcc33Pin, modemSleepRqPin, modemStatusPin, isHighWhenOn)
{}

void heldOnOff::begin(void)
{
    ModemOnOff::begin();
    MS_DBG(F("Pin "), _modemSleepRqPin, F(" will be held "), _isHighWhenOn,
           F(" to turn modem on.\n"));
}

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
bool heldOnOff::on(void)
{
    // Power up
    powerOn();

    // If no pin assigned to turn it on or off, assume it's on and return
    if (_modemSleepRqPin < 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is on/awake."));
        return true;
    }

    // Do not check if on or off; just set the pin to whatever it should be held
    // at to turn the modem on
    MS_DBG(F("Turning modem on by setting pin "), _modemSleepRqPin, F(" to "),
           _isHighWhenOn, F(".\n"));
    digitalWrite(_modemSleepRqPin, _isHighWhenOn);

    // If we can't tell if we've succeeded, assume we have and exit
    if (_modemStatusPin < 0)
    {
        MS_DBG(F("Assume modem now on.\n"));
        return true;
    }

    // If there's a status pin available, wait until modem shows it's on
    for (uint32_t start = millis(); millis() - start < 10000L; )
    {
        if (isOn())
        {
            MS_DBG(F("Modem on after "), millis() - start, F("ms.\n"));
            return true;
        }
    }

    // If the modem doesn't show it's on within 5 seconds, return false
    MS_DBG(F("Failed to turn modem on.\n"));
    return false;
}

bool heldOnOff::off(void)
{
    // If no pin assigned to turn it on or off, assume it's off and return
    if (_modemSleepRqPin < 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is off/asleep."));
        return true;
    }

    // Do not check if on or off; just set the pin to whatever it should be held
    // at to turn the modem off
    MS_DBG(F("Turning modem off by setting pin "), _modemSleepRqPin, F(" to "),
           !_isHighWhenOn, F(".\n"));
    digitalWrite(_modemSleepRqPin, !_isHighWhenOn);

    // If we can't tell if we've succeeded, assume we have and exit
    if (_modemStatusPin < 0)
    {
        MS_DBG(F("Assume modem now off.\n"));
        return true;
    }

    // If there's a status pin available, wait until modem shows it's off
    for (uint32_t start = millis(); millis() - start < 10000L; )
    {
        if (!isOn())
        {
            MS_DBG(F("Modem off after "), millis() - start, F("ms.\n"));
            powerOff();
            return true;
        }
    }

    // If the modem doesn't show it's off within 10 seconds, cut the power anyway
    MS_DBG(F("Failed to turn modem off.\n"));
    // Power down
    powerOff();
    return false;
}
