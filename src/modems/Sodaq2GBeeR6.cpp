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
Sodaq2GBeeR6::Sodaq2GBeeR6(Stream *modemStream,
                           int8_t powerPin, int8_t statusPin,
                           const char *apn)
    : SIMComSIM800(modemStream,
                   powerPin, statusPin, -1, -1,
                   apn)
{
    setVRefPin(-1);
}


// Constructor
Sodaq2GBeeR6::Sodaq2GBeeR6(Stream *modemStream,
                           int8_t vRefPin, int8_t statusPin, int8_t powerPin,
                           const char *apn)
    : SIMComSIM800(modemStream,
                   powerPin, statusPin, -1, -1,
                   apn)
{
    setVRefPin(vRefPin);
}

// Destructor
Sodaq2GBeeR6::~Sodaq2GBeeR6(){}

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool Sodaq2GBeeR6::modemWakeFxn(void)
{
    if (_vRefPin >= 0)
    {
        MS_DBG(F("Enabling voltage reference for GPRSBeeR6 on pin"), _vRefPin);
        digitalWrite(_vRefPin, HIGH);
    }
    return true;
}


bool Sodaq2GBeeR6::modemSleepFxn(void)
{
    // Ask the SIM800 to shut down nicely
    MS_DBG(F("Asking SIM800 on GPRSBeeR6 to power down"));
    bool success =  gsmModem.poweroff();
    if (_vRefPin >= 0)
    {
        MS_DBG(F("Disabling voltage reference for GPRSBeeR6 on pin"), _vRefPin);
        digitalWrite(_vRefPin, LOW);
    }
    return success;
}
