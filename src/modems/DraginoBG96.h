/*
 *DraginoBG96.h
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
// #define MS_DRAGINOBG96_DEBUG

#ifdef MS_DRAGINOBG96_DEBUG
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
#define TINY_GSM_MODEM_BG96  // Select for a Quectel BG96


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
#if defined TINY_GSM_MODEM_XBEE
TinyGsm *tinyModem = new TinyGsm(modemSerial, modemResetPin);
#else
TinyGsm *tinyModem = new TinyGsm(modemSerial);
#endif

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

bool modemWakeFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    delay(110);  // >100ms for Quectel BG96
    digitalWrite(modemSleepRqPin, HIGH);
    return true;
}


bool modemSleepFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    delay(700);  // >650ms for Quectel BG96, 0.6sec > pull down > 1sec for  Quectel M95, Quectel MC60
    digitalWrite(modemSleepRqPin, HIGH);
    return true;
}


void extraModemSetup(void){}



#endif
