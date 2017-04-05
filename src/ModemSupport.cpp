/*
 *Modem_OnOff.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for turning modems on and off to save power
*/


#include "ModemSupport.h"

/* ===========================================================================
* Functions for the main OnOff class
* ========================================================================= */

ModemOnOff::ModemOnOff()
{
    _vcc33Pin = -1;
    _onoff_DTR_pin = -1;
    _status_CTS_pin = -1;
}

// Initializes the instance
void ModemOnOff::init(int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin)
{
    // Serial.println(F("Initializing modem on/off."));  // For debugging
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

bool ModemOnOff::isOn(void)
{
    if (_status_CTS_pin >= 0) {
        bool status = digitalRead(_status_CTS_pin);
        // Serial.print(F("Is modem on? "));  // For debugging
        // Serial.println(status);  // For debugging
        return status;
    }
    // No status pin. Let's assume it is on.
    return true;
}

void ModemOnOff::powerOn(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, HIGH);
        // Serial.println(F("Sending power to modem."));  // For debugging
    }
}

void ModemOnOff::powerOff(void)
{
    if (_vcc33Pin >= 0) {
        digitalWrite(_vcc33Pin, LOW);
        // Serial.println(F("Cutting modem power."));  // For debugging
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
    // Serial.print(F("Pulsing modem to on with pin "));  // For debugging
    Serial.println(_onoff_DTR_pin);  // For debugging
    if (!isOn()) {pulse();}
    while (!isOn()){delay(1);}  // Wait until is actually on
}

void pulsedOnOff::off()
{
    if (isOn()) {pulse();}
    while (isOn()){delay(1);}  // Wait until is off
    // Serial.println(F("Modem pulsed to off."));  // For debugging
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
    // Serial.print(F("Setting modem to on with pin "));  // For debugging
    // Serial.println(_onoff_DTR_pin);  // For debugging
    if (_onoff_DTR_pin >= 0) {
        digitalWrite(_onoff_DTR_pin, HIGH);
    }
    while (!isOn()){delay(1);}  // Wait until is actually on
}

void heldOnOff::off()
{
    if (_onoff_DTR_pin >= 0) {
        digitalWrite(_onoff_DTR_pin, LOW);
    }
    while (isOn()){delay(1);}  // Wait until is off
    // Serial.println(F("Modem set to off."));  // For debugging
    powerOff();
}



/* ===========================================================================
* Functions for the modems
* This is basically a wrapper for TinyGsm
* ========================================================================= */
void loggerModem::setupModem(modemType modType,
                               Stream *modemStream,
                               int vcc33Pin,
                               int status_CTS_pin,
                               int onoff_DTR_pin,
                               const char *APN)
{
    _modemType = modType;
    _APN = APN;

    Serial.println(F("Setting up modem."));  // For debugging

    // Initialize the modem
    switch(modType)
    {
        case GPRSBee4:
        case Fona:
        {
            static pulsedOnOff pulsed;
            _modemOnOff = &pulsed;
            pulsed.init(vcc33Pin, onoff_DTR_pin, status_CTS_pin);
            break;
        }
        case GPRSBee6:
        {
            static heldOnOff held;
            _modemOnOff = &held;
            held.init(vcc33Pin, onoff_DTR_pin, status_CTS_pin);
            break;
        }
        case WIFIBee:
        {
            _modemStream = modemStream;
            static heldOnOff held;
            _modemOnOff = &held;
            break;
        }
    }

    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            Serial.println(F("Initializing GSM modem instance"));  // For debugging
            static TinyGsm modem(*modemStream);
            _modem = &modem;
            static TinyGsmClient client(modem);
            _client = &client;
            _modemOnOff->on();
            _modem->init();
            _modemOnOff->off();
            _modemStream = _client;
            break;
        }
        case WIFIBee:
        {
            _modemStream = modemStream;
            break;
        }
    }
}


void loggerModem::connectNetwork(void)
{
    // Turn on the modem and connect to the network
    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            // Serial.println(F("Attempting to turn on modem."));  // For debugging
            _modemOnOff->on();
            Serial.println(F("Waiting for network..."));  // For debugging
            if (!_modem->waitForNetwork()) {
                Serial.println("... Connection failed");  // For debugging
            } else {
                _modem->gprsConnect(_APN, "", "");
            }
            break;
        }
        case WIFIBee:
            {break;}
    }
}


void loggerModem::disconnectNetwork(void)
// Disconnect and turn off the modem
{
    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            _modem->gprsDisconnect();
            _modemOnOff->off();
            break;
        }
        case WIFIBee:
            {break;}
    }
}

int loggerModem::connect(const char *host, uint16_t port)
{
    return _client->connect(host, port);
}

int loggerModem::connect(IPAddress ip, uint16_t port)
{
    return _client->connect(ip, port);
}

void loggerModem::stop(void)
{
    _client->stop();
}
