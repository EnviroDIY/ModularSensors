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
void ModemOnOff::init(int vcc33Pin, int modemSleepRqPin, int modemStatusPin)
{
    MS_DBG(F("Initializing modem on/off..."));
    if (vcc33Pin >= 0) {
      _vcc33Pin = vcc33Pin;
      // First write the output value, and only then set the output mode.
      digitalWrite(_vcc33Pin, LOW);
      pinMode(_vcc33Pin, OUTPUT);
    }
    if (modemSleepRqPin >= 0) {
        _modemSleepRqPin = modemSleepRqPin;
        // First write the output value, and only then set the output mode.
        digitalWrite(_modemSleepRqPin, LOW);
        pinMode(_modemSleepRqPin, OUTPUT);
    }
    if (modemStatusPin >= 0) {
        _modemStatusPin = modemStatusPin;
        pinMode(_modemStatusPin, INPUT_PULLUP);
    }
    MS_DBG(F("   ... Success!\n"));
}

bool ModemOnOff::isOn(void)
{
    if (_modemStatusPin >= 0) {
        bool status = digitalRead(_modemStatusPin);
        MS_DBG(F("Is modem on? "), status, F("\n"));
        return status;
    }
    // No status pin. Let's assume it is on.
    return true;
}

void ModemOnOff::powerOn(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, HIGH);
        MS_DBG(F("Sending power to modem.\n"));
    }
}

void ModemOnOff::powerOff(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, LOW);
        MS_DBG(F("Cutting modem power.\n"));
    }
}



/* ===========================================================================
* Functions for pulsed method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

// Turns the modem on and off by pulsing the onoff/DTR/Key pin on for 2 seconds
    bool pulsedOnOff::on(void)
    {
        powerOn();
        MS_DBG(F("Pulsing modem to on with pin "));
        MS_DBG(_modemSleepRqPin, F("\n"));
        if (!isOn()) {pulse();}
        // Wait until is actually on
        for (unsigned long start = millis(); millis() - start < 5000; )
        {
            if (isOn())
            {
                MS_DBG(F("Modem now on.\n"));
                return true;
            }
          delay(5);
        }
        MS_DBG(F("Failed to turn modem on.\n"));
        return false;
    }

bool pulsedOnOff::off(void)
{
    if (isOn())
    {
        MS_DBG(F("Pulsing modem off with pin "));
        MS_DBG(_modemSleepRqPin, F("\n"));
        pulse();
    }
    else MS_DBG(F("Modem was not ever on.\n"));
    // Wait until is off
    for (unsigned long start = millis(); millis() - start < 5000; )
    {
        if (!isOn())
        {
            MS_DBG(F("Modem now off.\n"));
            powerOff();
            return true;
        }
        delay(5);
    }
    MS_DBG(F("Failed to turn modem off.\n"));
    powerOff();
    return false;
}

void pulsedOnOff::pulse(void)
{
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        delay(200);
        digitalWrite(_modemSleepRqPin, HIGH);
        delay(2500);
        digitalWrite(_modemSleepRqPin, LOW);
    }
}


/* ===========================================================================
* Functions for held method.
* This turns the modem on by setting the onoff/DTR/Key pin high and off by
* setting it low.
* This is used by the Sodaq GPRSBee v0.6.
* ========================================================================= */

// Turns the modem on by setting the onoff/DTR/Key high and off by setting it low
bool heldOnOff::on(void)
{
    powerOn();
    if (_modemSleepRqPin <= 0) {return true;}
    else
    {
        MS_DBG(F("Turning modem on by setting pin "));
        MS_DBG(_modemSleepRqPin);
        MS_DBG(F(" high\n"));
        digitalWrite(_modemSleepRqPin, HIGH);
        // Wait until is actually on
        for (unsigned long start = millis(); millis() - start < 5000; )
        {
            if (isOn())
            {
                MS_DBG(F("Modem now on.\n"));
                return true;
            }
            delay(5);
        }
        MS_DBG(F("Failed to turn modem on.\n"));
        return false;
    }
}

bool heldOnOff::off(void)
{
    if (_modemSleepRqPin <= 0) {return true;}
    else
    {
        if (!isOn()) MS_DBG(F("Modem was not ever on.\n"));
        digitalWrite(_modemSleepRqPin, LOW);
        // Wait until is off
        for (unsigned long start = millis(); millis() - start < 5000; )
        {
            if (!isOn())
            {
                MS_DBG(F("Modem now off.\n"));
                powerOff();
                return true;
            }
            delay(5);
        }
        MS_DBG(F("Failed to turn modem off.\n"));
        powerOff();
        return false;
    }
}


/* ===========================================================================
* Functions for reverse method.
* This turns the modem on by setting the onoff/DTR/Key pin LOW and off by
* setting it HIGH.
* This is used by the XBee's
* ========================================================================= */

// Turns the modem on by setting the onoff/DTR/Key LOW and off by setting it HIGH
bool reverseOnOff::isOn(void)
{
    if (_modemStatusPin >= 0) {
        bool status = digitalRead(_modemStatusPin);
        // MS_DBG(F("Is modem on? "), status, F("\n"));
        return !status;
    }
    // No status pin. Let's assume it is on.
    return true;
}

bool reverseOnOff::on(void)
{
    powerOn();
    MS_DBG(F("Turning modem on on by setting pin "));
    MS_DBG(_modemSleepRqPin);
    MS_DBG(F(" low\n"));
    if (_modemSleepRqPin >= 0) {
        digitalWrite(_modemSleepRqPin, LOW);
    }
    // Wait until is actually on
    for (unsigned long start = millis(); millis() - start < 5000; )
    {
        if (isOn())
        {
            MS_DBG(F("Modem now on.\n"));
            return true;
        }
        delay(5);
    }
    MS_DBG(F("Failed to turn modem on.\n"));
    return false;
}

bool reverseOnOff::off(void)
{
    if (!isOn()) MS_DBG(F("Modem was not ever on.\n"));
    if (_modemSleepRqPin >= 0) {
        digitalWrite(_modemSleepRqPin, HIGH);
    }
    // Wait until is off
    for (unsigned long start = millis(); millis() - start < 5000; )
    {
        if (!isOn())
        {
            MS_DBG(F("Modem now off.\n"));
            powerOff();
            return true;
        }
        delay(5);
    }
    MS_DBG(F("Failed to turn modem off.\n"));
    powerOff();
    return false;
}
