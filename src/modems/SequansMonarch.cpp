/*
 *SequansMonarch.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Botletics and other modules based on the SIMCOM BG96.
*/

// Included Dependencies
#include "SequansMonarch.h"
#include "modems/LoggerModemMacros.h"


// Constructor
SequansMonarch::SequansMonarch(Stream* modemStream,
                               int8_t powerPin, int8_t statusPin,
                               int8_t modemResetPin, int8_t modemSleepRqPin,
                               const char *apn,
                               uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin,
                MONARCH_STATUS_TIME_MS, MONARCH_DISCONNECT_TIME_MS,
                MONARCH_WARM_UP_TIME_MS, MONARCH_ATRESPONSE_TIME_MS,
                MONARCH_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SEQUANSMONARCH_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}


MS_MODEM_DID_AT_RESPOND(SequansMonarch);
MS_MODEM_IS_INTERNET_AVAILABLE(SequansMonarch);
MS_MODEM_IS_MEASUREMENT_COMPLETE(SequansMonarch);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(SequansMonarch);
MS_MODEM_CONNECT_INTERNET(SequansMonarch);
MS_MODEM_DISCONNECT_INTERNET(SequansMonarch);
MS_MODEM_GET_NIST_TIME(SequansMonarch);

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SequansMonarch::modemWakeFxn(void)
{
    // Module turns on when power is applied
    // No pulsing required in this case
    if (_powerPin >= 0)
        return true;
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        delayMicroseconds(50);  // ?? Time isn't documented
        digitalWrite(_modemSleepRqPin, HIGH);
        return true;
    }
    else
    {
        return true;
    }
}


bool SequansMonarch::modemSleepFxn(void)
{
    if (_powerPin >= 0 || _modemSleepRqPin >= 0)  // will go on with power on
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        return gsmModem.poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


bool SequansMonarch::extraModemSetup(void)
{
    gsmModem.init();
    _modemName = gsmModem.getModemName();
    return true;
}
