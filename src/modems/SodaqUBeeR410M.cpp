/*
 *SodaqUBeeR410M.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA R410M LTE-M Cellular Module
*/

// Included Dependencies
#include "SodaqUBeeR410M.h"
#include "modems/LoggerModemMacros.h"


// Constructor
#if F_CPU == 8000000L
SodaqUBeeR410M::SodaqUBeeR410M(HardwareSerial* modemStream,
                               int8_t powerPin, int8_t statusPin,
                               int8_t modemResetPin, int8_t modemSleepRqPin,
                               const char *apn,
                               uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin,
                R410M_STATUS_TIME_MS, R410M_DISCONNECT_TIME_MS,
                R410M_WARM_UP_TIME_MS, R410M_ATRESPONSE_TIME_MS,
                R410M_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SODAQUBEER410M_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEBUGGING_SERIAL_OUTPUT),
    _tinyModem(_modemATDebugger),
    _tinyClient(_tinyModem)
    #else
    _tinyModem(*modemStream),
    _tinyClient(_tinyModem)
    #endif
{
    _apn = apn;
}
#else
SodaqUBeeR410M::SodaqUBeeR410M(Stream* modemStream,
                               int8_t powerPin, int8_t statusPin,
                               int8_t modemResetPin, int8_t modemSleepRqPin,
                               const char *apn,
                               uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin,
                R410M_STATUS_TIME_MS, R410M_DISCONNECT_TIME_MS,
                R410M_WARM_UP_TIME_MS, R410M_ATRESPONSE_TIME_MS,
                R410M_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SODAQUBEER410M_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEBUGGING_SERIAL_OUTPUT),
    _tinyModem(_modemATDebugger),
    _tinyClient(_tinyModem)
    #else
    _tinyModem(*modemStream),
    _tinyClient(_tinyModem)
    #endif
{
    _apn = apn;

    _modemSerial = modemStream;
}
#endif


MS_MODEM_DID_AT_RESPOND(SodaqUBeeR410M);
MS_MODEM_IS_INTERNET_AVAILABLE(SodaqUBeeR410M);
MS_MODEM_IS_MEASUREMENT_COMPLETE(SodaqUBeeR410M);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(SodaqUBeeR410M);
MS_MODEM_CONNECT_INTERNET(SodaqUBeeR410M);
MS_MODEM_DISCONNECT_INTERNET(SodaqUBeeR410M);
MS_MODEM_GET_NIST_TIME(SodaqUBeeR410M);


// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool SodaqUBeeR410M::modemWakeFxn(void)
{
    // SARA R4/N4 series must power on and then pulse on
    if (_modemSleepRqPin >= 0)
    {
        digitalWrite(_modemSleepRqPin, LOW);
        delay(200);  // 0.15-3.2s pulse for wake on SARA R4/N4
        digitalWrite(_modemSleepRqPin, HIGH);
        // Need to slow down R4/N4's default 115200 baud rate for slow processors
        // The baud rate setting is NOT saved to non-volatile memory, so it must
        // be changed every time after loosing power.
        #if F_CPU == 8000000L
        if (_powerPin >= 0)
        {
            delay(4600);  // Must wait for UART port to become active
            _modemSerial->begin(115200);
            _tinyModem.setBaud(9600);
            _modemSerial->end();
            _modemSerial->begin(9600);
        }
        #endif
        return true;
    }
    else
    {
        return true;
    }
}


bool SodaqUBeeR410M::modemSleepFxn(void)
{
    if (_modemSleepRqPin >= 0) // R410 must have access to PWR_ON pin to sleep
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        return _tinyModem.poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


bool SodaqUBeeR410M::extraModemSetup(void)
{
    _tinyModem.init();
    _modemName = _tinyModem.getModemName();
    // Set to only use LTE-M, which should cause connection more quickly
    _tinyModem.sendAT(F("+URAT=7"));
    _tinyModem.waitResponse();
    return true;
}
