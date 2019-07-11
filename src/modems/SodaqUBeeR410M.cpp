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
                modemResetPin, modemSleepRqPin, false,
                R410M_STATUS_TIME_MS, R410M_DISCONNECT_TIME_MS,
                R410M_WARM_UP_TIME_MS, R410M_ATRESPONSE_TIME_MS,
                R410M_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SODAQUBEER410M_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;

    _modemSerial = modemStream;
}
#else
SodaqUBeeR410M::SodaqUBeeR410M(Stream* modemStream,
                               int8_t powerPin, int8_t statusPin,
                               int8_t modemResetPin, int8_t modemSleepRqPin,
                               const char *apn,
                               uint8_t measurementsToAverage)
  : loggerModem(powerPin, statusPin, HIGH,
                modemResetPin, modemSleepRqPin, false,
                R410M_STATUS_TIME_MS, R410M_DISCONNECT_TIME_MS,
                R410M_WARM_UP_TIME_MS, R410M_ATRESPONSE_TIME_MS,
                R410M_SIGNALQUALITY_TIME_MS,
                measurementsToAverage),
    #ifdef MS_SODAQUBEER410M_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}
#endif


// Destructor
SodaqUBeeR410M::~SodaqUBeeR410M(){}


MS_MODEM_DID_AT_RESPOND(SodaqUBeeR410M);
MS_MODEM_IS_INTERNET_AVAILABLE(SodaqUBeeR410M);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(SodaqUBeeR410M);
MS_MODEM_GET_MODEM_SIGNAL_QUALITY(SodaqUBeeR410M);
MS_MODEM_GET_MODEM_BATTERY_AVAILABLE(SodaqUBeeR410M);
MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(SodaqUBeeR410M);
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
        MS_DBG(F("Sending a wake-up pulse on pin"), _modemSleepRqPin, F("for Sodaq UBee R410M"));
        digitalWrite(_modemSleepRqPin, LOW);

        // If possible, monitor the v_int pin waiting for it to become high before ending pulse
        if (_dataPin >= 0)
        {
            uint32_t startTimer = millis();
            // 0.15-3.2s pulse for wake on SARA R4/N4 (ie, max is 3.2s)
            // Wait no more than 3.2s
            while (digitalRead(_dataPin) != _statusLevel && millis() - startTimer < 3200L) {}
            MS_DBG(F("Status pin came on after"), millis() - startTimer, F("ms"));
            // But at least 0.15s
            while (millis() - startTimer < 150) {}
            MS_DBG(F("Pulsed for"), millis() - startTimer, F("ms"));
        }
        else delay(200);  // 0.15-3.2s pulse for wake on SARA R4/N4

        digitalWrite(_modemSleepRqPin, HIGH);
        // Need to slow down R4/N4's default 115200 baud rate for slow processors
        // The baud rate setting is NOT saved to non-volatile memory, so it must
        // be changed every time after loosing power.
        #if F_CPU == 8000000L
        if (_powerPin >= 0)
        {
            MS_DBG(F("Waiting for UART to become active and requesting a slower baud rate."));
            delay(R410M_ATRESPONSE_TIME_MS + 250);  // Must wait for UART port to become active
            _modemSerial->begin(115200);
            gsmModem.setBaud(9600);
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
        MS_DBG(F("Asking u-blox R410M to power down"));
        return gsmModem.poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}


bool SodaqUBeeR410M::extraModemSetup(void)
{
    gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    // Set to only use LTE-M, which should cause connection more quickly
    gsmModem.sendAT(GF("+URAT=7"));
    gsmModem.waitResponse();
    return true;
}


void SodaqUBeeR410M::modemHardReset(void)
{
    if (_modemResetPin >= 0)
    {
        MS_DBG(F("Doing a hard reset.  This takes 10s, be patient."));
        digitalWrite(_modemResetPin, LOW);
        delay(10000L);
        digitalWrite(_modemResetPin, HIGH);
        // Re-set _millisSensorActivated  - the hard reset is a new activation
        _millisSensorActivated = millis();
        // Unset the flag for prior communication failure
        previousCommunicationFailed = false;
    }
}
