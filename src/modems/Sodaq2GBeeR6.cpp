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


// Constructor
Sodaq2GBeeR6::Sodaq2GBeeR6(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin,
                           int8_t modemSleepRqPin,
                           const char *apn,
                           uint8_t measurementsToAverage)
  : SIMComSIM800(modemStream,
                 powerPin, statusPin,
                 -1, modemSleepRqPin,
                 apn,
                 measurementsToAverage)
{
    // Override two of the standard SIM800 setting
    // Need these because the newer GPRSBee's (>v6) have the PWR_KEY
    // tied to the power pin
    _alwaysRunWake = true;
    _warmUpTime_ms = S2GBR6_WARM_UP_TIME_MS;
}


// Destructor
Sodaq2GBeeR6::~Sodaq2GBeeR6(){}

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



void Sodaq2GBeeR6::modemPowerUp(void)
{
    loggerModem::modemPowerUp();
}
