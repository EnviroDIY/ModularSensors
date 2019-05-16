/*
 *DigiXBee.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for all Digi XBee's
*/

// Included Dependencies
#include "DigiXBee.h"


// Constructor
DigiXBee::DigiXBee(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                   int8_t modemResetPin, int8_t modemSleepRqPin,
                   uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, !useCTSStatus,
                modemResetPin, modemSleepRqPin,
                XBEE_STATUS_TIME_MS, XBEE_DISCONNECT_TIME_MS,
                XBEE_WARM_UP_TIME_MS, XBEE_ATRESPONSE_TIME_MS,
                XBEE_SIGNALQUALITY_TIME_MS,
                measurementsToAverage)
{}


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
// After enabling pin sleep, the sleep request pin is held LOW to keep the XBee on
// Enable pin sleep in the setup function or using XCTU prior to connecting the XBee
bool DigiXBee::modemWakeFxn(void)
{
    if (_modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        digitalWrite(_modemSleepRqPin, HIGH);
        return true;
    }
    else
    {
        return true;
    }
}


bool DigiXBee::modemSleepFxn(void)
{
    if (_powerPin >= 0)  // Turns on when power is applied
        return true;
    else if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        digitalWrite(_modemLEDPin, HIGH);  // Because the XBee doesn't have any lights
        return true;
    }
    else
    {
        return true;
    }
}