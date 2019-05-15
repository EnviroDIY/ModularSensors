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
#include "modems/LoggerModemMacros.h"


// Constructor
QuectelBG96::QuectelBG96(Stream* modemStream,
                         int8_t powerPin, int8_t statusPin,
                         int8_t modemResetPin, int8_t modemSleepRqPin,
                         const char *apn,
                         uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin,
                BG96_STATUS_TIME_MS, BG96_DISCONNECT_TIME_MS,
                BG96_WARM_UP_TIME_MS, BG96_ATRESPONSE_TIME_MS,
                BG96_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_QUECTELBG96_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEBUGGING_SERIAL_OUTPUT),
    _tinyModem(_modemATDebugger)
    #else
    _tinyModem(*modemStream)
    #endif
{
    _apn = apn;
    TinyGsmClient *tinyClient = new TinyGsmClient(_tinyModem);
    _tinyClient = tinyClient;
    _modemStream = modemStream;
}


MS_MODEM_DID_AT_RESPOND(QuectelBG96);
MS_MODEM_IS_INTERNET_AVAILABLE(QuectelBG96);
MS_MODEM_IS_MEASUREMENT_COMPLETE(QuectelBG96);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(QuectelBG96);
MS_MODEM_CONNECT_INTERNET(QuectelBG96);
MS_MODEM_DISCONNECT_INTERNET(QuectelBG96);
MS_MODEM_GET_NIST_TIME(QuectelBG96);


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool QuectelBG96::modemWakeFxn(void)
{
    // Must power on and then pulse on
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        delay(160);  // ≥100ms
        digitalWrite(_modemSleepRqPin, LOW);
    }
    return true;
}


bool QuectelBG96::modemSleepFxn(void)
{
    if (_modemSleepRqPin >= 0) // BG96 must have access to PWRKEY pin to sleep
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        return _tinyModem.poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


bool QuectelBG96::extraModemSetup(void)
{
    _modemName = _tinyModem.getModemName();
    return true;
}
