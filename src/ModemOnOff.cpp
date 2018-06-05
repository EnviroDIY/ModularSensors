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
ModemOnOff::ModemOnOff(int vcc33Pin, int modemSleepRqPin, int modemStatusPin,
                       ModemSleepType sleepType)
{
    _vcc33Pin = vcc33Pin;
    _modemSleepRqPin = modemSleepRqPin;
    _modemStatusPin = modemStatusPin;
    _sleepType = sleepType;
    // Assume modem is off at constructor
    _isNowOn = false;
}


// Begins the instance - ie, sets pin modes
// This is the stuff that cannot happen in the constructor
void ModemOnOff::begin(void)
{
    MS_DBG(F("Initializing modem on/off with power on pin "), _vcc33Pin,
           F(" status on pin "), _modemStatusPin,
           F(" and on/off via pin "), modemSleepRqPin, F("..."));

    // Set pin modes
    if (_vcc33Pin >= 0)
    {
        pinMode(_vcc33Pin, OUTPUT);  // Set pin mode
        digitalWrite(_vcc33Pin, LOW);  // Set power off
    }
    if (_modemSleepRqPin >= 0)
    {
        pinMode(_modemSleepRqPin, OUTPUT);  // Set pin mode
        switch (_sleepType)
        {
            case modem_sleep_reverse:
            case modem_sleep_rev_pulse:
            {
                digitalWrite(_modemSleepRqPin, HIGH);  // Set to off
                break;
            }
            default:
            {
                digitalWrite(_modemSleepRqPin, LOW);  // Set to off
                break;
            }
        }
    }
    if (_modemStatusPin >= 0)
    {
        pinMode(_modemStatusPin, INPUT_PULLUP);
    }

    // Initialize assuming modem is off
    _isNowOn = false;

    MS_DBG(F("   ... Success!\n"));
}


// Function to check if the modem is currently on
bool ModemOnOff::isOn(void)
{
    if (_modemStatusPin >= 0)
    {
        bool status = digitalRead(_modemStatusPin);
        switch (_sleepType)
        {
            case modem_sleep_reverse:
            case modem_sleep_rev_pulse:
            {
                status = !status;
                break;
            }
            default: break;
        }
        // MS_DBG(F("Is modem on? "), status, F("\n"));
        return status;
    }
    // No status pin. Return the "internal" status code.
    return _isNowOn;
}


// Function to supply power to the modem - sets power pin high
void ModemOnOff::powerOn(void)
{
    if (_vcc33Pin >= 0)
    {
        digitalWrite(_vcc33Pin, HIGH);
        MS_DBG(F("Sending power to modem.\n"));
    }
}


// Function to cut power from the modem - sets power pin low
void ModemOnOff::powerOff(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, LOW);
        MS_DBG(F("Cutting modem power.\n"));
    }
}


// Function to turn the modem on
bool ModemOnOff::on(void)
{
    // Power up
    powerOn();

    // If no pin assigned to turn it on or off, assume it's on and return
    if (_modemSleepRqPin <= 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is on/awake."));
        _isNowOn = true;
        return true;
    }

    switch(_sleepType)
    {
        case modem_sleep_held:
        case modem_sleep_reverse:
        {
            // Do not check if on or off; just set the pin to whatever it should be held
            // at to turn the modem on
            MS_DBG(F("Turning modem on by setting pin "), _modemSleepRqPin, F(" to "),
                   _isHighWhenOn, F(".\n"));
            digitalWrite(_modemSleepRqPin, HIGH);

            // Wait until is actually on
            for (uint32_t start = millis(); millis() - start < 5000; )
            {
                if (isOn())
                {
                    MS_DBG(F("Modem now on.\n"));
                    _isNowOn = true;
                    return true;
                }
            }
            break;
        }
        case modem_sleep_pulsed:
        case modem_sleep_rev_pulse:
        {
            // Check if it's already on before sending pulse
            if (isOn())
            {
                MS_DBG(F("Modem was already on.\n"));
                _isNowOn = true;
                return true;
            }
            else
            {
                MS_DBG(F("Turning modem on with a "), _isHighWhenOn, F(" pulse on pin "),
                       _modemSleepRqPin, F(".\n"));
                pulse();

                // Wait until is actually on
                for (uint32_t start = millis(); millis() - start < 5000; )
                {
                    if (isOn())
                    {
                        MS_DBG(F("Modem now on.\n"));
                        _isNowOn = true;
                        return true;
                    }
                }
                break;
            }
        }
        case modem_always_on:
        {
            MS_DBG(F("Modem always on.\n"));
            _isNowOn = true;
            return true;
        }
    }

    // If the modem doesn't show it's on within 5 seconds, return false
    MS_DBG(F("Failed to turn modem on!\n"));
    _isNowOn = false;
    return false;
}


// Function to turn the modem off
bool ModemOnOff::off(void)
{
    // If no pin assigned to turn it on or off, assume it's off and return
    if (_modemSleepRqPin <= 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is off/asleep."));
        _isNowOn = false;
        return true;
    }

    switch(_sleepType)
    {
        case modem_sleep_held:
        case modem_sleep_reverse:
        {
            // Do not check if on or off; just set the pin to whatever it should be held
            // at to turn the modem off
            MS_DBG(F("Turning modem off by setting pin "), _modemSleepRqPin, F(" to "),
                   !_isHighWhenOn, F(".\n"));
            digitalWrite(_modemSleepRqPin, LOW);

            // Wait until is off
            for (uint32_t start = millis(); millis() - start < 5000; )
            {
                if (!isOn())
                {
                    MS_DBG(F("Modem now off.\n"));
                    powerOff();
                    return true;
                }
            }
            break;
        }
        case modem_sleep_pulsed:
        case modem_sleep_rev_pulse:
        {
            // Check if it's already off before sending pulse
            if (!isOn())
            {
                MS_DBG(F("Modem was not ever on.\n"));
                _isNowOn = false;
                return true;
            }
            else
            {
                MS_DBG(F("Turning modem off with a "), !_isHighWhenOn, F(" pulse on pin "),
                       _modemSleepRqPin, F(".\n"));
                pulse();

                // Wait until is off
                for (uint32_t start = millis(); millis() - start < 1000; )
                {
                    if (!isOn())
                    {
                        MS_DBG(F("Modem now off.\n"));
                        _isNowOn = false;
                        powerOff();
                        return true;
                    }
                }
            }
            break;
        }
        case modem_always_on:
        {
            MS_DBG(F("Modem always on.\n"));
            _isNowOn = true;
            return false;
        }

        // If the modem doesn't show it's off within 5 seconds, cut the power
        // anyway and return true
        MS_DBG(F("Failed to turn modem off with on/sleep pin!\n"));
        // Power down anyway
        powerOff();
        _isNowOn = false;
        return true;
    }
}


void ModemOnOff::pulse(void)
{
    if (_modemSleepRqPin >= 0)
    {
        switch (_sleepType)
        {
            case modem_sleep_reverse:
            case modem_sleep_rev_pulse:
            {
                digitalWrite(_modemSleepRqPin, LOW);
                delay(200);
                digitalWrite(_modemSleepRqPin, HIGH);
                delay(2500);
                digitalWrite(_modemSleepRqPin, LOW);
                break;
            }
            default:
            {
                digitalWrite(_modemSleepRqPin, HIGH);
                delay(200);
                digitalWrite(_modemSleepRqPin, LOW);
                delay(2500);
                digitalWrite(_modemSleepRqPin, HIGH);
                break;
            }
        }
    }
}
