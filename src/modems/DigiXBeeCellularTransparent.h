/*
 *DigiXBeeCellular.h
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
// #define MS_DIGIXBEECELLULAR_DEBUG

#ifdef MS_DIGIXBEECELLULAR_DEBUG
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
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's


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
extern Stream  &modemSerial;


// ==========================================================================
//    TinyGSM Client
// ==========================================================================

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
TinyGsm *tinyModem = new TinyGsm(modemSerial, modemResetPin);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================
// Describe the physical pin connection of your modem to your board
const bool modemStatusLevel = LOW;  // The level of the status pin when the module is active (HIGH or LOW)

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
// After enabling pin sleep, the sleep request pin is held LOW to keep the XBee on
// Enable pin sleep in the setup function or using XCTU prior to connecting the XBee
bool modemSleepFxn(void)
{
    if (modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(modemLEDPin, LOW);
        return true;
    }
    else
    {
        return true;
    }
}


bool modemWakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
        return true;
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(modemLEDPin, HIGH);  // Because the XBee doesn't have any lights
        return true;
    }
    else
    {
        return true;
    }
}


void extraModemSetup(void)
{
    tinyModem->init();  // initialize
    if (tinyModem->commandMode())
    {
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        tinyModem->sendAT(F("D8"),1);
        tinyModem->waitResponse();
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        tinyModem->sendAT(F("D9"),1);
        tinyModem->waitResponse();
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        tinyModem->sendAT(F("D7"),1);
        tinyModem->waitResponse();
        // Put the XBee in pin sleep mode
        tinyModem->sendAT(F("SM"),1);
        tinyModem->waitResponse();
        // Disassociate from network for lowest power deep sleep
        tinyModem->sendAT(F("SO"),0);
        tinyModem->waitResponse();
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        tinyModem->sendAT(F("DO"),0);
        tinyModem->waitResponse();
        // Make sure USB direct won't be pin enabled on XBee3 units
        tinyModem->sendAT(F("P0"),0);
        tinyModem->waitResponse();
        // Make sure pins 7&8 are not set for USB direct on XBee3 units
        tinyModem->sendAT(F("P1"),0);
        tinyModem->waitResponse();
        // Cellular carrier profile - AT&T
        // Hologram says they can use any network, but I've never succeeded with anything but AT&T
        tinyModem->sendAT(F("CP"),2);
        tinyModem->waitResponse();
        // Cellular network technology - LTE-M Only
        // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
        tinyModem->sendAT(F("N#"),2);
        tinyModem->waitResponse();
        // Write changes to flash and apply them
        tinyModem->writeChanges();
        // Exit command mode
        tinyModem->exitCommand();
    }
}



#endif
