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

/*
// Constructor
ModemOnOff::ModemOnOff(int8_t modemSleepRqPin, bool sleepRqLevel,
                       int8_t modemStatusPin, bool statusLevel)
  : _modemSleepRqPin(modemSleepRqPin), _sleepRqLevel(sleepRqLevel),
    _modemStatusPin(modemStatusPin), _statusLevel(statusLevel)
{}


// Begins the instance - ie, sets pin modes
// This is the stuff that cannot happen in the constructor
void ModemOnOff::begin(void)
{
    if (_modemSleepRqPin >= 0)
    {
        pinMode(_modemSleepRqPin, OUTPUT);  // Set pin mode
        digitalWrite(_modemSleepRqPin, !_sleepRqLevel);  // Set to off
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
        return digitalRead(_modemStatusPin) == _statusLevel;
    }
    // No status pin. Return true.
    return true;
}
*/




// Initialize the static values
int8_t pulsedOnOff::_modemSleepRqPin = -1;
bool pulsedOnOff::_sleepRqLevel = 0;
int8_t pulsedOnOff::_modemStatusPin = -1;
bool pulsedOnOff::_statusLevel = 0;
uint16_t pulsedOnOff::_pulseTime_ms = 0;
/* ===========================================================================
* Functions for the pulsed on-off method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

pulsedOnOff::pulsedOnOff(int8_t modemSleepRqPin, bool sleepRqLevel, uint16_t pulseTime_ms,
            int8_t modemStatusPin, bool statusLevel)
  // : ModemOnOff(modemSleepRqPin, sleepRqLevel, modemStatusPin, statusLevel)
{
    _modemSleepRqPin = modemSleepRqPin;
    _sleepRqLevel = sleepRqLevel;
    _pulseTime_ms = pulseTime_ms;
    _modemStatusPin = modemStatusPin;
    _statusLevel = statusLevel;
}

void pulsedOnOff::begin(void)
{
    if (_modemSleepRqPin >= 0)
    {
        pinMode(_modemSleepRqPin, OUTPUT);  // Set pin mode
        digitalWrite(_modemSleepRqPin, !_sleepRqLevel);  // Set to off
    }
    if (_modemStatusPin >= 0)
    {
        pinMode(_modemStatusPin, INPUT_PULLUP);
    }
    MS_DBG(F("Pin "), _modemSleepRqPin, F(" will be pulsed to "), _sleepRqLevel,
           F(" for "), _pulseTime_ms, F("ms to turn modem on.\n"));
    MS_DBG(F("On will be considered successful if "), _modemStatusPin, F(" is "), _statusLevel,
          F(".\n"));
}

// Function to check if the modem is currently on
// That is, checks if the status pin is reading on
bool pulsedOnOff::isOn(void)
{
    if (_modemStatusPin >= 0)
    {
        return digitalRead(_modemStatusPin) == _statusLevel;
    }
    // No status pin. Return true.
    return true;
}

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
bool pulsedOnOff::on(void)
{
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
        MS_DBG(F("Turning modem on with a "), _sleepRqLevel, F(" pulse on pin "),
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
        MS_DBG(F("Turning modem off with a "), !_sleepRqLevel, F(" pulse on pin "),
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
                return true;
            }
        }

        // If the modem doesn't show it's off within 10 seconds, return false
        MS_DBG(F("Failed to turn modem off with on/sleep pin!\n"));
        return false;
    }
}

void pulsedOnOff::pulse(void)
{
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, !_sleepRqLevel);
        delay(_pulseTime_ms);
        digitalWrite(_modemSleepRqPin, _sleepRqLevel);
        delay(_pulseTime_ms);
        digitalWrite(_modemSleepRqPin, !_sleepRqLevel);
    }
}


// Initialize the static values
int8_t heldOnOff::_modemSleepRqPin = -1;
bool heldOnOff::_sleepRqLevel = 0;
int8_t heldOnOff::_modemStatusPin = -1;
bool heldOnOff::_statusLevel = 0;
/* ===========================================================================
* Functions for the held on-off method.
* This turns the modem on by setting and holding the onoff/DTR/Key pin to
* either high or low.
* A "high" on is used by the Sodaq GPRSBee v0.6 and Sodaq 3GBee.
* A "low" on is used by the all Digi XBee's.
* ========================================================================= */

heldOnOff::heldOnOff(int8_t modemSleepRqPin, bool sleepRqLevel,
                     int8_t modemStatusPin, bool statusLevel)
  // : ModemOnOff(modemSleepRqPin, sleepRqLevel, modemStatusPin, statusLevel)
{
    _modemSleepRqPin = modemSleepRqPin;
    _sleepRqLevel = sleepRqLevel;
    _modemStatusPin = modemStatusPin;
    _statusLevel = statusLevel;
}

void heldOnOff::begin(void)
{
    if (_modemSleepRqPin >= 0)
    {
        pinMode(_modemSleepRqPin, OUTPUT);  // Set pin mode
        digitalWrite(_modemSleepRqPin, !_sleepRqLevel);  // Set to off
    }
    if (_modemStatusPin >= 0)
    {
        pinMode(_modemStatusPin, INPUT_PULLUP);
    }
    MS_DBG(F("Pin "), _modemSleepRqPin, F(" will be held "), _sleepRqLevel,
           F(" to turn modem on.\n"));
    MS_DBG(F("On will be considered successful if "), _modemStatusPin, F(" is "), _statusLevel,
          F(".\n"));
}

// Function to check if the modem is currently on
// That is, checks if the status pin is reading on
bool heldOnOff::isOn(void)
{
    if (_modemStatusPin >= 0)
    {
        return digitalRead(_modemStatusPin) == _statusLevel;
    }
    // No status pin. Return true.
    return true;
}

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
bool heldOnOff::on(void)
{
    // If no pin assigned to turn it on or off, assume it's on and return
    if (_modemSleepRqPin < 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is on/awake."));
        return true;
    }

    // Do not check if on or off; just set the pin to whatever it should be held
    // at to turn the modem on
    MS_DBG(F("Turning modem on by setting pin "), _modemSleepRqPin, F(" to "),
           _sleepRqLevel, F(".\n"));
    digitalWrite(_modemSleepRqPin, _sleepRqLevel);

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
           !_sleepRqLevel, F(".\n"));
    digitalWrite(_modemSleepRqPin, !_sleepRqLevel);

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
            return true;
        }
    }

    // If the modem doesn't show it's off within 10 seconds, return false
    MS_DBG(F("Failed to turn modem off.\n"));
    return false;
}
