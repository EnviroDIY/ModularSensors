/*
 *Modem_OnOff.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for turning modems on and off to save power
*/


#include "Modem_OnOff.h"

/* ===========================================================================
* Functions for the main OnOff class
* ========================================================================= */

OnOff::OnOff()
{
    _vcc33Pin = -1;
    _onoff_DTR_pin = -1;
    _status_CTS_pin = -1;
}

// Initializes the instance
void OnOff::init(int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin)
{
    Serial.println(F("Initializing modem on/off."));  // For debugging
    if (vcc33Pin >= 0) {
      _vcc33Pin = vcc33Pin;
      // First write the output value, and only then set the output mode.
      digitalWrite(_vcc33Pin, LOW);
      pinMode(_vcc33Pin, OUTPUT);
    }
    if (onoff_DTR_pin >= 0) {
        _onoff_DTR_pin = onoff_DTR_pin;
        // First write the output value, and only then set the output mode.
        digitalWrite(_onoff_DTR_pin, LOW);
        pinMode(_onoff_DTR_pin, OUTPUT);
    }
    if (status_CTS_pin >= 0) {
        _status_CTS_pin = status_CTS_pin;
        pinMode(_status_CTS_pin, INPUT);
    }
}

bool OnOff::isOn(void)
{
    if (_status_CTS_pin >= 0) {
        bool status = digitalRead(_status_CTS_pin);
        return status;
    }
    // No status pin. Let's assume it is on.
    return true;
}

void OnOff::powerOn(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, HIGH);
        Serial.println(F("Sending power to modem."));  // For debugging
    }
}

void OnOff::powerOff(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, LOW);
        Serial.println(F("Cutting modem power."));  // For debugging
    }
}


/* ===========================================================================
* Functions for pulsed method.
* This turns the modem on and off by turning the onoff/DTR/Key pin on for two
* seconds and then back off.
* This is used by the Sodaq GPRSBee v0.4 and the Adafruit Fona.
* ========================================================================= */

void pulsedOnOff::pulse(void)
{
    if (_onoff_DTR_pin >= 0)
    {
        digitalWrite(_onoff_DTR_pin, LOW);
        delay(200);
        digitalWrite(_onoff_DTR_pin, HIGH);
        delay(2500);
        digitalWrite(_onoff_DTR_pin, LOW);
    }
}

void pulsedOnOff::on()
{
    powerOn();
    Serial.println(F("Pulsing modem to on."));  // For debugging
    if (!isOn()) {pulse();}
}

void pulsedOnOff::off()
{
    if (isOn()) {pulse();}
    Serial.println(F("Modem pulsed to off."));  // For debugging
    powerOff();
}

/* ===========================================================================
* Functions for held method.
* This turns the modem on by setting the onoff/DTR/Key pin high and off by
* setting it low.
* This is used by the Sodaq GPRSBee v0.6.
* ========================================================================= */

void heldOnOff::on()
{
    powerOn();
    Serial.println(F("Setting modem to on."));  // For debugging
    if (_onoff_DTR_pin >= 0) {
        digitalWrite(_onoff_DTR_pin, HIGH);
    }
}

void heldOnOff::off()
{
    if (_onoff_DTR_pin >= 0) {
        digitalWrite(_onoff_DTR_pin, LOW);
        // Should be instant
        // Let's wait a little, but not too long
        delay(50);
    }
    Serial.println(F("Modem set to off."));  // For debugging
    powerOff();
}
