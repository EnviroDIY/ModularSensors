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
ModemOnOff::ModemOnOff()
{
    _vcc33Pin = -1;
    _modemSleepRqPin = -1;
    _modemStatusPin = -1;
}

// Initializes the instance
void ModemOnOff::init(int vcc33Pin, int modemSleepRqPin, int modemStatusPin,
                      bool isHighWhenOn)
{
    MS_DBG(F("Initializing modem on/off..."));

    // Set whether using high or low to turn on
    _isHighWhenOn = isHighWhenOn;

    // Set pin modes
    _vcc33Pin = vcc33Pin;
    if (vcc33Pin >= 0)
    {
        pinMode(_vcc33Pin, OUTPUT);  // Set pin mode
        digitalWrite(_vcc33Pin, LOW);  // Set power off
    }
    _modemSleepRqPin = modemSleepRqPin;
    if (modemSleepRqPin >= 0)
    {
        pinMode(_modemSleepRqPin, OUTPUT);  // Set pin mode
        digitalWrite(_modemSleepRqPin, !isHighWhenOn);  // Set to off
    }
    _modemStatusPin = modemStatusPin;
    if (modemStatusPin >= 0)
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
        if (!_isHighWhenOn) status = !status;
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



/* ===========================================================================
* Functions for the pulsed on-off method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
bool pulsedOnOff::on(void)
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
            delay(5);
        }

        // If the modem doesn't show it's on within 5 seconds, return false
        MS_DBG(F("Failed to turn modem on!\n"));
        _isNowOn = false;
        return false;
    }
}

bool pulsedOnOff::off(void)
{
    // If no pin assigned to turn it on or off, assume it's pff and return
    if (_modemSleepRqPin <= 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is off/asleep."));
        _isNowOn = false;
        return true;
    }

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
            delay(5);
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

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
bool heldOnOff::on(void)
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
        delay(5);
    }

    // If the modem doesn't show it's on within 5 seconds, return false
    MS_DBG(F("Failed to turn modem on.\n"));
    _isNowOn = false;
    return false;
}

bool heldOnOff::off(void)
{
    // If no pin assigned to turn it on or off, assume it's off and return
    if (_modemSleepRqPin <= 0)
    {
        MS_DBG(F("No modem on/sleep pin assigned, assuming modem is off/asleep."));
        _isNowOn = false;
        return true;
    }

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
        delay(5);
    }

    // If the modem doesn't show it's off within 5 seconds, cut the power
    // anyway and return true
    MS_DBG(F("Failed to turn modem off.\n"));
    // Power down
    powerOff();
    _isNowOn = false;
    return false;
}
