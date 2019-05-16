/*
 *SodaqUBeeU201.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA U201 3G Cellular Module
*/

// Included Dependencies
#include "SodaqUBeeU201.h"
#include "modems/LoggerModemMacros.h"


// Constructor
SodaqUBeeU201::SodaqUBeeU201(Stream* modemStream,
                             int8_t powerPin, int8_t statusPin,
                             int8_t modemResetPin, int8_t modemSleepRqPin,
                             const char *apn,
                             uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin, true,
                U201_STATUS_TIME_MS, U201_DISCONNECT_TIME_MS,
                U201_WARM_UP_TIME_MS, U201_ATRESPONSE_TIME_MS,
                U201_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SODAQUBEEU201_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}


MS_MODEM_DID_AT_RESPOND(SodaqUBeeU201);
MS_MODEM_IS_INTERNET_AVAILABLE(SodaqUBeeU201);
MS_MODEM_IS_MEASUREMENT_COMPLETE(SodaqUBeeU201);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(SodaqUBeeU201);
MS_MODEM_CONNECT_INTERNET(SodaqUBeeU201);
MS_MODEM_DISCONNECT_INTERNET(SodaqUBeeU201);
MS_MODEM_GET_NIST_TIME(SodaqUBeeU201);


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SodaqUBeeU201::modemWakeFxn(void)
{
    // SARA/LISA U2/G2 and SARA G3 series turn on when power is applied
    // No pulsing required in this case
    if (_powerPin >= 0)
        return true;
    if (_modemSleepRqPin >= 0)
    {
        MS_DBG(F("Sending a wake-up pulse on pin"), _modemSleepRqPin, F("for Sodaq UBee U201"));
        digitalWrite(_modemSleepRqPin, LOW);
        delayMicroseconds(65);  // 50-80µs pulse for wake on SARA/LISA U2/G2
        digitalWrite(_modemSleepRqPin, HIGH);
        return true;
    }
    else
    {
        return true;
    }
}


bool SodaqUBeeU201::modemSleepFxn(void)
{
    if (_powerPin >= 0 || _modemSleepRqPin >= 0)  // will go on with power on
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        MS_DBG(F("Asking u-blox SARA U201 to power down"));
        return gsmModem.poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


bool SodaqUBeeU201::extraModemSetup(void)
{
    gsmModem.init();
    _modemName = gsmModem.getModemName();
    return true;
}
