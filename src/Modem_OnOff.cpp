/*
 *Modem_OnOff.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for turning modems on and off to save power
*/


#include "Modem_OnOff.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////    GPRSbeeOnOff       /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

GPRSbeeOnOff::GPRSbeeOnOff()
{
    _vcc33Pin = -1;
    _onoff_DTR_pin = -1;
    _status_CTS_pin = -1;
    _version = V06;
}

// Initializes the instance
void GPRSbeeOnOff::init(int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin, GPRSVersion version /* = V06*/)
{
    _version = version;

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

void GPRSbeeOnOff::on()
{
    // First VCC 3.3 HIGH
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, HIGH);
    }

    switch (_version) {
        case V06:
        case V05:
        {
            // Wait a little
            // TODO Figure out if this is really needed
            delay(2);
            if (_onoff_DTR_pin >= 0) {
                digitalWrite(_onoff_DTR_pin, HIGH);
            }
        }
        case V04:
        {
            if (!isOn()) {
            #if 1
              // To be on the safe side, make sure we start from LOW
              // TODO Decide if this is useful.
              digitalWrite(_onoff_DTR_pin, LOW);
              delay(200);
            #endif
              digitalWrite(_onoff_DTR_pin, HIGH);
              delay(2500);
              digitalWrite(_onoff_DTR_pin, LOW);
            }
        }
    }
}

void GPRSbeeOnOff::off()
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, LOW);
    }

    switch (_version) {
        case V06:
        case V05:
        {
            // The GPRSbee is switched off immediately
            if (_onoff_DTR_pin >= 0) {
                digitalWrite(_onoff_DTR_pin, LOW);
            // Should be instant
            // Let's wait a little, but not too long
            delay(50);
        }
        }
        case V04:
        {
            if (isOn()) {
                digitalWrite(_onoff_DTR_pin, LOW);
                delay(200);
                digitalWrite(_onoff_DTR_pin, HIGH);
                delay(2500);
                digitalWrite(_onoff_DTR_pin, LOW);
                // Not bothering to wait and do a graceful shutdown.
            }
        }
    }
}

bool GPRSbeeOnOff::isOn()
{
    if (_status_CTS_pin >= 0) {
        bool status = digitalRead(_status_CTS_pin);
        return status;
    }
    // No status pin. Let's assume it is on.
    return true;
}
