/*
 *QuectelBG96.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Botletics and other modules based on the SIMCOM BG96.
*/

// Included Dependencies
#include "QuectelBG96.h"


// Constructor
QuectelBG96::QuectelBG96(Stream* modemStream,
                         int8_t powerPin, int8_t statusPin,
                         int8_t modemResetPin, int8_t modemSleepRqPin,
                         const char *apn)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin, false,
                BG96_STATUS_TIME_MS, BG96_DISCONNECT_TIME_MS,
                BG96_WARM_UP_TIME_MS, BG96_ATRESPONSE_TIME_MS),
    #ifdef MS_QUECTELBG96_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}


// Destructor
QuectelBG96::~QuectelBG96(){}

MS_MODEM_HARD_RESET(QuectelBG96);
MS_MODEM_IS_INTERNET_AVAILABLE(QuectelBG96);
MS_MODEM_GET_MODEM_SIGNAL_QUALITY(QuectelBG96);
MS_MODEM_CONNECT_INTERNET(QuectelBG96);

// NOTE:  Could actually get temperature from the Digi chip by entering command mode
float QuectelBG96::getModemChipTemperature(void)
{
    MS_DBG(F("This modem doesn't return temperature!"));
    return (float)-9999;
}

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool QuectelBG96::modemWakeFxn(void)
{
    // Must power on and then pulse on
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        delay(160);  // ≥100ms
        digitalWrite(_modemSleepRqPin, HIGH);
    }
    return true;
}


bool QuectelBG96::modemSleepFxn(void)
{
    if (_modemSleepRqPin >= 0) // BG96 must have access to PWRKEY pin to sleep
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        return gsmModem.poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


bool QuectelBG96::extraModemSetup(void){}


void QuectelBG96::modemPowerUp(void)
{
    if (_powerPin >= 0)
    {
        if (_modemSleepRqPin >= 0)
        {
            // The PWR_ON pin MUST be high at power up.
            digitalWrite(_modemSleepRqPin, HIGH);
        }
        MS_DBG(F("Powering"), getModemName(), F("with pin"), _powerPin);
        digitalWrite(_powerPin, HIGH);
        // Mark the time that the sensor was powered
        _millisPowerOn = millis();
    }
    else
    {
        MS_DBG(F("Power to"), getModemName(), F("is not controlled by this library."));
        // Mark the power-on time, just in case it had not been marked
        if (_millisPowerOn == 0) _millisPowerOn = millis();
    }
}
