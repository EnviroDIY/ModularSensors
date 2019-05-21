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
                -1, modemSleepRqPin, true,
                SIM800_STATUS_TIME_MS, SIM800_DISCONNECT_TIME_MS,
                SIM800_WARM_UP_TIME_MS, SIM800_ATRESPONSE_TIME_MS,
                SIM800_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SODAQ2GBEER6_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}


MS_MODEM_DID_AT_RESPOND(Sodaq2GBeeR6);
MS_MODEM_IS_INTERNET_AVAILABLE(Sodaq2GBeeR6);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(Sodaq2GBeeR6);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(Sodaq2GBeeR6);
MS_MODEM_CONNECT_INTERNET(Sodaq2GBeeR6);
MS_MODEM_DISCONNECT_INTERNET(Sodaq2GBeeR6);
MS_MODEM_GET_NIST_TIME(Sodaq2GBeeR6);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool Sodaq2GBeeR6::modemWakeFxn(void)
{
    MS_DBG(F("Setting pin"), _modemSleepRqPin, F("high to wake GPRSBeeR6"));
    digitalWrite(_modemSleepRqPin, HIGH);
    return true;
}


bool Sodaq2GBeeR6::modemSleepFxn(void)
{
    MS_DBG(F("Sending pin"), _modemSleepRqPin, F("low to stop GPRSBeeR6"));
    digitalWrite(_modemSleepRqPin, LOW);
    return true;
}


bool Sodaq2GBeeR6::extraModemSetup(void)
{
    gsmModem.init();
    _modemName = gsmModem.getModemName();
    return true;
}
