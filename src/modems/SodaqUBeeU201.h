/*
 *SodaqUBeeU201.h
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
// #define MS_SODAQUBEEU201_DEBUG

#ifdef MS_SODAQUBEEU201_DEBUG
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
//    TinyGSM Client
// ==========================================================================

// #define TINY_GSM_DEBUG Serial  // If you want debugging on the main debug port

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible
// AltSoftSerial &modemSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modemSerial = neoSSerial1;  // For software serial if needed

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
TinyGsm *tinyModem = new TinyGsm(modemSerial);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool modemSleepFxn(void)
{
    if (modemVccPin >= 0 || modemSleepRqPin >= 0)  // others will go on with power on
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
    // SARA/LISA U2/G2 and SARA G3 series turn on when power is applied
    if (modemVccPin >= 0)
        return true;
    if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(modemLEDPin, HIGH);
        // delay(6);  // >5ms pulse for wake on SARA G3
        delayMicroseconds(65);  // 50-80µs pulse for wake on SARA/LISA U2/G2
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(modemLEDPin, LOW);
        return true;
    }
    else
    {
        return true;
    }
}


void extraModemSetup(void){}


#endif
