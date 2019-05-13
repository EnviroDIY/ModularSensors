/*
 *SodaqUBeeR410M.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA R410M LTE-M Cellular Module
*/

// Header Guards
#ifndef ModularSensorsSpecificModem_h
#define ModularSensorsSpecificModem_h

// Debugging Statement
// #define MS_SODAQUBEER410M_DEBUG

#ifdef MS_SODAQUBEER410M_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"

// ==========================================================================
//    Wifi/Cellular Modem Main Chip Selection
// ==========================================================================

// Select your modem chip - this determines the exact commands sent to it
#define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems


// ==========================================================================
//    Modem Pins
// ==========================================================================

// Declare the existance of modem pin Variables
// Use the "extern" keyword to denote that these will actually be defined in
// the main program rather than in this file.

extern const int8_t modemVccPin;
extern const int8_t modemSleepRqPin;
extern const int8_t modemStatusPin;
extern const int8_t modemResetPin;
extern const int8_t modemLEDPin;


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool modemSleepFxn(void)
{
    if (modemSleepRqPin >= 0)  // R410 must have access to PWR_ON pin to sleep
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        return tinyModem->poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}
bool modemWakeFxn(void)
{
    // SARA R4/N4 series must power on and then pulse on
    if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(modemLEDPin, HIGH);
        delay(200);  // 0.15-3.2s pulse for wake on SARA R4/N4
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(modemLEDPin, LOW);
        // Need to slow down R4/N4's default 115200 baud rate for slow processors
        #if F_CPU == 8000000L && defined USE_UBLOX_R410M
        delay(4600);  // Must wait for UART port to become active
        modemSerial.begin(115200);
        tinyModem->setBaud(9600);
        modemSerial.end();
        modemSerial.begin(9600);
        #endif
        return true;
    }
    else
    {
        return true;
    }
}


void extraModemSetup(void)
{
    tinyModem->sendAT(F("+URAT=7"));
    tinyModem->waitResponse();
}


#endif
