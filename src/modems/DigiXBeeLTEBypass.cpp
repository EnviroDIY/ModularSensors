/**
 * @file DigiXBeeLTEBypass.cpp
 * @copyright 2017-2022 Stroud Water Research Center
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
      gsmClient(gsmModem),
      _apn(apn) {
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
        /** First, wait the required guard time before entering command mode. */
        delay(1010);
        /** Now, enter command mode to set all pin I/O functionality. */
        gsmModem.streamWrite(GF("+++"));
        success = gsmModem.waitResponse(2000, GF("OK\r")) == 1;
        if (success) break;
    }
    if (success) {
        MS_DBG(F("Setting I/O Pins..."));
        /** Enable pin sleep functionality on `DIO9`.
         * NOTE: Only the `DTR_N/SLEEP_RQ/DIO8` pin (9 on the bee socket) can be
         * used for this pin sleep/wake. */
        gsmModem.sendAT(GF("D8"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable status indication on `DIO9` - it will be HIGH when the XBee
         * is awake.
         * NOTE: Only the `ON/SLEEP_N/DIO9` pin (13 on the bee socket) can be
         * used for direct status indication. */
        gsmModem.sendAT(GF("D9"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable CTS on `DIO7` - it will be `LOW` when it is clear to send
         * data to the XBee.  This can be used as proxy for status indication if
         * that pin is not readable.
         * NOTE: Only the `CTS_N/DIO7` pin (12 on the bee socket) can be used
         * for CTS. */
        gsmModem.sendAT(GF("D7"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable association indication on `DIO5` - this is should be directly
         * attached to an LED if possible.
         * - Solid light indicates no connection
         * - Single blink indicates connection
         * - double blink indicates connection but failed TCP link on last
         * attempt
         *
         * NOTE: Only the `Associate/DIO5` pin (15 on the bee socket) can be
         * used for this function. */
        gsmModem.sendAT(GF("D5"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable RSSI PWM output on `DIO10` - this should be directly attached
         * to an LED if possible.  A higher PWM duty cycle (and thus brighter
         * LED) indicates better signal quality.
         * NOTE: Only the `DIO10/PWM0` pin (6 on the bee socket) can be used for
         * this function. */
        gsmModem.sendAT(GF("P0"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable pin sleep on the XBee. */
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        MS_DBG(F("Setting Other Options..."));
        /** Disable remote manager, USB Direct, and LTE PSM.
         * NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no easy
         * way on the LTE-M Bee to wake the cell chip itself from PSM, so we'll
         * use the Digi pin sleep instead. */
        gsmModem.sendAT(GF("DO"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /* Make sure USB direct is NOT enabled on the XBee3 units. */
        gsmModem.sendAT(GF("P1"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Make sure airplane mode is off - bypass and airplane mode are
         * incompatible. */
        MS_DBG(F("Making sure airplane mode is off..."));
        gsmModem.sendAT(GF("AM"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        MS_DBG(F("Turning on Bypass Mode..."));
        /** Enable bypass mode. */
        gsmModem.sendAT(GF("AP5"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Write changes to flash. */
        gsmModem.sendAT(GF("WR"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Apply changes. */
        gsmModem.sendAT(GF("AC"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /* Finally, force a reset to actually enter bypass mode - this
         * effectively exits command mode. */
        MS_DBG(F("Resetting the module to reboot in bypass mode..."));
        gsmModem.sendAT(GF("FR"));
        success &= gsmModem.waitResponse(5000L, GF("OK\r")) == 1;
        /** Allow 5s for the unit to reset. */
        delay(500);
        /** Re-initialize the TinyGSM SARA R4 instance. */
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
