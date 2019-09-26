/*
 *DigiXBee3GBypass.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi Cellular XBee's BASED ON UBLOX CHIPS in bypass mode
*/

// Included Dependencies
#include "DigiXBee3GBypass.h"

// Constructor/Destructor
DigiXBee3GBypass::DigiXBee3GBypass(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *apn)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin),
    #ifdef MS_DIGIXBEE3GBYPASS_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _apn = apn;
}


// Destructor
DigiXBee3GBypass::~DigiXBee3GBypass(){}

MS_MODEM_HARD_RESET(DigiXBee3GBypass);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBee3GBypass);
MS_MODEM_GET_MODEM_SIGNAL_QUALITY(DigiXBee3GBypass);
MS_MODEM_CONNECT_INTERNET(DigiXBee3GBypass);

// NOTE:  Could actually get temperature from the Digi chip by entering command mode
float DigiXBee3GBypass::getModemChipTemperature(void)
{
    MS_DBG(F("This modem doesn't return temperature!"));
    return (float)-9999;
}

bool DigiXBee3GBypass::extraModemSetup(void)
{
    bool success = true;
    delay(1010);  // Wait the required guard time before entering command mode
    MS_DBG(F("Putting XBee into command mode..."));
    gsmModem.streamWrite(GF("+++"));  // enter command mode
    if (success &= gsmModem.waitResponse(2000, GF("OK\r")) == 1)
    {
        MS_DBG(F("Setting I/O Pins..."));
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(GF("D8"),1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"),1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        gsmModem.sendAT(GF("D7"),1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on the associate LED (if you're using a board with one)
        // NOTE:  Only pin 15/DIO5 can be used for this function
        gsmModem.sendAT(GF("D5"),1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on the RSSI indicator LED (if you're using a board with one)
        // NOTE:  Only pin 6/DIO10/PWM0 can be used for this function
        gsmModem.sendAT(GF("P0"),1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Put the XBee in pin sleep mode
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"),1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Disassociate from network for lowest power deep sleep
        MS_DBG(F("Setting Other Options..."));
        // Disable remote manager, enable 2G fallback
        gsmModem.sendAT(GF("DO"),02);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Make sure airplane mode is off - bypass and airplane mode are incompatible
        MS_DBG(F("Making sure airplane mode is off..."));
        gsmModem.sendAT(GF("AM"),0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        MS_DBG(F("Turning on Bypass Mode..."));
        // Turn on bypass mode
        gsmModem.sendAT(GF("AP5"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Write changes to flash
        gsmModem.sendAT(GF("WR"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Apply changes
        gsmModem.sendAT(GF("AC"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Force reset to actually enter bypass mode - this effectively exits command mode
        MS_DBG(F("Resetting the module to reboot in bypass mode..."));
        gsmModem.sendAT(GF("FR"));
        success &= gsmModem.waitResponse(5000L, GF("OK\r")) == 1;
        delay(500);  // Allow the unit to reset
        // re-initialize
        MS_DBG(F("Attempting to reconnect to the u-blox module..."));
        success &= gsmModem.init();
        gsmClient.init(&gsmModem);
        _modemName = gsmModem.getModemName();
    }
    else
    {
        success = false;
    }

    if (success)
    {
        MS_DBG(F("... Setup successful!"));
    }
    else
    {
        MS_DBG(F("... failed!"));
    }
    return success;
}
