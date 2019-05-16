/*
 *DigiXBeeCellularTransparent.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi Cellular XBee's
*/

// Included Dependencies
#include "DigiXBeeCellularTransparent.h"
#include "modems/LoggerModemMacros.h"

// Constructors
DigiXBeeCellularTransparent::DigiXBeeCellularTransparent(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *apn,
                           uint8_t measurementsToAverage)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin,
             measurementsToAverage),
    #ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}


MS_MODEM_DID_AT_RESPOND(DigiXBeeCellularTransparent);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeCellularTransparent);
MS_MODEM_IS_MEASUREMENT_COMPLETE(DigiXBeeCellularTransparent);
MS_MODEM_ADD_SINGLE_MEASUREMENT_RESULT(DigiXBeeCellularTransparent);
MS_MODEM_CONNECT_INTERNET(DigiXBeeCellularTransparent);
MS_MODEM_GET_NIST_TIME(DigiXBeeCellularTransparent);


bool DigiXBeeCellularTransparent::extraModemSetup(void)
{
    bool success = true;
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();  // initialize
    _modemName = gsmModem.getModemName();
    MS_DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode())
    {
        MS_DBG(F("Setting I/O Pins..."));
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(F("D8"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(F("D9"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        gsmModem.sendAT(F("D7"),1);
        success &= gsmModem.waitResponse() == 1;
        // Put the XBee in pin sleep mode
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(F("SM"),1);
        success &= gsmModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        gsmModem.sendAT(F("SO"),0);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Other Options..."));
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        gsmModem.sendAT(F("DO"),0);
        success &= gsmModem.waitResponse() == 1;
        // Make sure USB direct won't be pin enabled on XBee3 units
        gsmModem.sendAT(F("P0"),0);
        success &= gsmModem.waitResponse() == 1;
        // Make sure pins 7&8 are not set for USB direct on XBee3 units
        gsmModem.sendAT(F("P1"),0);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Cellular Carrier Options..."));
        // Cellular carrier profile - AT&T
        // Hologram says they can use any network, but I've never succeeded with anything but AT&T
        gsmModem.sendAT(F("CP"),2);
        success &= gsmModem.waitResponse() == 1;
        // Cellular network technology - LTE-M Only
        // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
        gsmModem.sendAT(F("N#"),2);
        success &= gsmModem.waitResponse() == 1;
        // Put the network connection parameters into flash
        success &= gsmModem.gprsConnect(_apn);
        // Write changes to flash and apply them
        gsmModem.writeChanges();
        // Exit command mode
        gsmModem.exitCommand();
    }
    else success = false;
    if (success) MS_DBG(F("... Setup successful!"));
    else MS_DBG(F("... failed!"));
    return success;
}


void DigiXBeeCellularTransparent::disconnectInternet(void)
{
    // XBee doesn't like to disconnect at all, so we're doing nothing
}
