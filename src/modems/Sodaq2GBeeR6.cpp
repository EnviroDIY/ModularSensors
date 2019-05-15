/*
 *Sodaq2GBeeR6.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq 2GBee revisions 6 and higher - these are based on
 *the SIMCOM SIM800h.
*/

// Included Dependencies
#include "Sodaq2GBeeR6.h"
#include "modems/LoggerModemMacros.h"


// Constructor
Sodaq2GBeeR6::Sodaq2GBeeR6(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin,
                           int8_t modemSleepRqPin,
                           const char *apn,
                           uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                -1, modemSleepRqPin,
                SIM800_STATUS_TIME_MS, SIM800_DISCONNECT_TIME_MS,
                SIM800_WARM_UP_TIME_MS, SIM800_ATRESPONSE_TIME_MS,
                SIM800_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
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


MS_MODEM_DID_AT_RESPOND(Sodaq2GBeeR6);
MS_MODEM_IS_INTERNET_AVAILABLE(Sodaq2GBeeR6);
MS_MODEM_IS_MEASUREMENT_COMPLETE(Sodaq2GBeeR6);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(Sodaq2GBeeR6);
MS_MODEM_CONNECT_INTERNET(Sodaq2GBeeR6);
MS_MODEM_DISCONNECT_INTERNET(Sodaq2GBeeR6);
MS_MODEM_GET_NIST_TIME(Sodaq2GBeeR6);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool Sodaq2GBeeR6::modemWakeFxn(void)
{
    digitalWrite(_modemSleepRqPin, HIGH);
    return true;
}


bool Sodaq2GBeeR6::modemSleepFxn(void)
{
    digitalWrite(_modemSleepRqPin, LOW);
    return true;
}


bool Sodaq2GBeeR6::extraModemSetup(void)
{
    _modemName = _tinyModem.getModemName();
    return true;
}
