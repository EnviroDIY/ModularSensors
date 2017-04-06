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
    // Serial.println(_onoff_DTR_pin);  // For debugging
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
Stream *loggerModem::_modemStream;

void loggerModem::setupModem(modemType modType,
                               Stream *modemStream,
                               int vcc33Pin,
                               int status_CTS_pin,
                               int onoff_DTR_pin,
                               const char *APN)
{
    _modemType = modType;
    _APN = APN;


    // Initialize the modem
    switch(_modemType)
    {
        case GPRSBee4:
        case Fona:
        {
            // Serial.println(F("Setting up GPRSBee4 or Fona"));  // For debugging
            static pulsedOnOff pulsed;
            _modemOnOff = &pulsed;
            pulsed.init(vcc33Pin, onoff_DTR_pin, status_CTS_pin);
            break;
        }
        case GPRSBee6:
        {
            // Serial.println(F("Setting up GPRSBee6."));  // For debugging
            static heldOnOff held;
            _modemOnOff = &held;
            held.init(vcc33Pin, onoff_DTR_pin, status_CTS_pin);
            break;
        }
        default:
        {
            // Serial.println(F("Setting up modem."));  // For debugging
            _modemStream = modemStream;
            static heldOnOff held;
            _modemOnOff = &held;
            break;
        }
    }

    switch(_modemType)
    {
        case WiFiBee:
        {
            _modemStream = modemStream;
            break;
        }
        default:
        {
            // Serial.println(F("Initializing GSM modem instance"));  // For debugging
            static TinyGsm modem(*modemStream);
            _modem = &modem;
            static TinyGsmClient client(modem);
            _client = &client;
            _modemOnOff->on();
            _modem->begin();
            // _modem->restart();
            // _modem->factoryDefault();
            _modemOnOff->off();
            _modemStream = _client;
            break;
        }
    }
}


    // Turn on the modem and connect to the network
bool loggerModem::connectNetwork(void)
{
    bool retVal = false;
    switch(_modemType)
    {
        case WiFiBee:
        {
            retVal = true;
            break;
        }
        default:
        {
            // Serial.println(F("Attempting to turn on modem."));  // For debugging
            _modemOnOff->on();
            Serial.println(F("\nWaiting for network..."));  // For debugging
            if (!_modem->waitForNetwork()){
                Serial.println("... Connection failed");  // For debugging
            } else {
                _modem->gprsConnect(_APN, "", "");
                retVal = true;
            }
            break;
        }
    }
    return retVal;
}


// Disconnect and turn off the modem
void loggerModem::disconnectNetwork(void)
{
    switch(_modemType)
    {
        case WiFiBee:
            {break;}
        default:
        {
            _modem->gprsDisconnect();
            _modemOnOff->off();
            break;
        }
    }
}

int loggerModem::connect(const char *host, uint16_t port)
{
    int retVal = 0;
    switch(_modemType)
    {
        case WiFiBee:
            {break;}
        default:
        {
            retVal = _client->connect(host, port);
            break;
        }
    }
    return retVal;
}

int loggerModem::connect(IPAddress ip, uint16_t port)
{
    int retVal = 0;
    switch(_modemType)
    {
        case WiFiBee:
            {break;}
        default:
        {
            retVal = _client->connect(ip, port);
            break;
        }
    }
    return retVal;
}

void loggerModem::stop(void)
{
    switch(_modemType)
    {
        case WiFiBee:
            {break;}
        default:
        {
            _client->stop();
            break;
        }
    }
}



// Used to empty out the buffer after a post request.
// Removing this may cause communication issues. If you
// prefer to not see the std::out, remove the print statement
void loggerModem::dumpBuffer(Stream *stream, int timeDelay/* = 5*/, int timeout/* = 5000*/)
{
    while (timeout-- > 0 && stream->available() > 0)
    {
        while (stream->available() > 0)
        {
            // Serial.print(stream->readString());
            stream->read();
            delay(timeDelay);
        }
        delay(timeDelay);
    }
    stream->flush();
}


// Used only for debugging - can be removed
/*
void loggerModem::printHTTPResult(int HTTPcode)
{
    switch (HTTPcode)
    {
        case 200:
        case 201:
        case 202:
        {
            Serial.println(F("Data was sent successfully."));
            break;
        }

        case 301:
        case 302:
        {
            Serial.println(F("Request was redirected."));
            break;
        }

        case 400:
        {
            Serial.println(F("Bad request."));
            break;
        }

        case 404:
        {
            Serial.println(F("Requested resource not found."));
            break;
        }

        case 403:
        case 405:
        {
            Serial.print(F("Access forbidden.  "));
            Serial.println(F("Check your registration token and UUIDs."));
            break;
        }

        case 500:
        case 503:
        {
            Serial.println(F("Request caused an internal server error."));
            break;
        }

        case 504:
        {
            Serial.print(F("Request timed out.  "));
            Serial.println(F("No response from server or insufficient signal to send message."));
            break;
        }

        default:
        {
            Serial.print(F("\nAn unknown error has occured, and we're pretty confused\n"));
            break;
        }
    }
}*/
