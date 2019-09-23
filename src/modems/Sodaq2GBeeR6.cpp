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
                S2GBR6_STATUS_TIME_MS, S2GBR6_DISCONNECT_TIME_MS,
                S2GBR6_WARM_UP_TIME_MS, S2GBR6_ATRESPONSE_TIME_MS,
                S2GBR6_SIGNALQUALITY_TIME_MS,
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


// Destructor
Sodaq2GBeeR6::~Sodaq2GBeeR6(){}


MS_MODEM_DID_AT_RESPOND(Sodaq2GBeeR6);
MS_MODEM_IS_INTERNET_AVAILABLE(Sodaq2GBeeR6);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(Sodaq2GBeeR6);
MS_MODEM_GET_MODEM_SIGNAL_QUALITY(Sodaq2GBeeR6);
MS_MODEM_GET_MODEM_BATTERY_AVAILABLE(Sodaq2GBeeR6);
MS_MODEM_GET_MODEM_TEMPERATURE_NA(Sodaq2GBeeR6);
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
    MS_DBG(F("Setting Pin"), _modemSleepRqPin, F("low to stop GPRSBeeR6"));
    digitalWrite(_modemSleepRqPin, LOW);
    return true;
}


bool Sodaq2GBeeR6::extraModemSetup(void)
{
    gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    return true;
}
