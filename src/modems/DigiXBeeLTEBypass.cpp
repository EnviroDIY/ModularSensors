/**
 * @file DigiXBeeLTEBypass.cpp
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the DigiXBeeLTEBypass class.
 */

// Included Dependencies
#include "DigiXBeeLTEBypass.h"
#include "LoggerModemMacros.h"

// Constructor/Destructor
DigiXBeeLTEBypass::DigiXBeeLTEBypass(Stream* modemStream, int8_t powerPin,
                                     int8_t statusPin, bool useCTSStatus,
                                     int8_t modemResetPin,
                                     int8_t modemSleepRqPin, const char* apn)
    : DigiXBee(powerPin, statusPin, useCTSStatus, modemResetPin,
               modemSleepRqPin),
#ifdef MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger),
#else
      gsmModem(*modemStream),
#endif
      gsmClient(gsmModem) {
    _apn = apn;
}

// Destructor
DigiXBeeLTEBypass::~DigiXBeeLTEBypass() {}

MS_IS_MODEM_AWAKE(DigiXBeeLTEBypass);
MS_MODEM_WAKE(DigiXBeeLTEBypass);

MS_MODEM_CONNECT_INTERNET(DigiXBeeLTEBypass);
MS_MODEM_DISCONNECT_INTERNET(DigiXBeeLTEBypass);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeLTEBypass);

MS_MODEM_GET_NIST_TIME(DigiXBeeLTEBypass);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(DigiXBeeLTEBypass);
MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeLTEBypass);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeLTEBypass);

bool DigiXBeeLTEBypass::extraModemSetup(void) {
    bool success = false;
    MS_DBG(F("Putting XBee into command mode..."));
    for (uint8_t i = 0; i < 5; i++) {
        // Wait the required guard time before entering command mode
        delay(1010);
        gsmModem.streamWrite(GF("+++"));  // enter command mode
        success = gsmModem.waitResponse(2000, GF("OK\r")) == 1;
        if (success) break;
    }
    if (success) {
        MS_DBG(F("Setting I/O Pins..."));
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(GF("D8"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on status indication pin - will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive
        // commands This can be used as proxy for status indication if the true
        // status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        gsmModem.sendAT(GF("D7"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on the associate LED (if you're using a board with one)
        // NOTE:  Only pin 15/DIO5 can be used for this function
        gsmModem.sendAT(GF("D5"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Turn on the RSSI indicator LED (if you're using a board with one)
        // NOTE:  Only pin 6/DIO10/PWM0 can be used for this function
        gsmModem.sendAT(GF("P0"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Put the XBee in pin sleep mode
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        MS_DBG(F("Setting Other Options..."));
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        gsmModem.sendAT(GF("DO"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Make sure pins 7&8 are not set for USB direct on XBee3 units
        gsmModem.sendAT(GF("P1"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // MS_DBG(F("Setting Cellular Carrier Options..."));
        // // Carrier Profile - 1 = No profile/SIM ICCID selected
        // gsmModem.sendAT(GF("CP"),1);
        // success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // // Cellular network technology - LTE-M/NB IoT
        // gsmModem.sendAT(GF("N#"),0);
        // success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Make sure airplane mode is off - bypass and airplane mode are
        // incompatible
        MS_DBG(F("Making sure airplane mode is off..."));
        gsmModem.sendAT(GF("AM"), 0);
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
        // Force reset to actually enter bypass mode - this effectively exits
        // command mode
        MS_DBG(F("Resetting the module to reboot in bypass mode..."));
        gsmModem.sendAT(GF("FR"));
        success &= gsmModem.waitResponse(5000L, GF("OK\r")) == 1;
        delay(500);  // Allow the unit to reset
        // re-initialize
        MS_DBG(F("Attempting to reconnect to the u-blox SARA R410M module..."));
        success &= gsmModem.init();
        gsmClient.init(&gsmModem);
        _modemName = gsmModem.getModemName();
    } else {
        success = false;
    }

    if (success) {
        MS_DBG(F("... Setup successful!"));
    } else {
        MS_DBG(F("... setup failed!"));
    }
    return success;
}

bool DigiXBeeLTEBypass::modemHardReset(void) {
    bool success = false;
    // If the u-blox cellular component isn't responding but the Digi processor
    // is, use the Digi API to reset the cellular component
    MS_DBG(F("Returning XBee to command mode..."));
    for (uint8_t i = 0; i < 5; i++) {
        // Wait the required guard time before entering command mode
        delay(1010);
        gsmModem.streamWrite(GF("+++"));  // enter command mode
        success = gsmModem.waitResponse(2000, GF("OK\r")) == 1;
        if (success) break;
    }
    if (success) {
        MS_DBG(F("... and forcing a reset of the cellular component."));
        // Force a reset of the undelying cellular component
        gsmModem.sendAT(GF("!R"));
        success &= gsmModem.waitResponse(30000L, GF("OK\r")) == 1;
        // Exit command mode
        gsmModem.sendAT(GF("CN"));
        success &= gsmModem.waitResponse(5000L, GF("OK\r")) == 1;
    } else {
        MS_DBG(F("... failed!  Using a pin reset on the XBee."));
        success = loggerModem::modemHardReset();
    }
    return success;
}
