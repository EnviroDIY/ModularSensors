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
#include "LoggerModemMacros.h"

// Constructor/Destructor
DigiXBeeCellularTransparent::DigiXBeeCellularTransparent(
    Stream* modemStream, int8_t powerPin, int8_t statusPin, bool useCTSStatus,
    int8_t modemResetPin, int8_t modemSleepRqPin, const char* apn)
    : DigiXBee(powerPin, statusPin, useCTSStatus, modemResetPin,
               modemSleepRqPin),
#ifdef MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger, modemResetPin),
#else
      gsmModem(*modemStream, modemResetPin),
#endif
      gsmClient(gsmModem) {
    _apn = apn;
}

// Destructor
DigiXBeeCellularTransparent::~DigiXBeeCellularTransparent() {}

MS_MODEM_WAKE(DigiXBeeCellularTransparent);

MS_MODEM_CONNECT_INTERNET(DigiXBeeCellularTransparent);
MS_MODEM_DISCONNECT_INTERNET(DigiXBeeCellularTransparent);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeCellularTransparent);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(DigiXBeeCellularTransparent);
MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeCellularTransparent);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeCellularTransparent);

// We turn off airplane mode in the wake.
bool DigiXBeeCellularTransparent::modemWakeFxn(void) {
    if (_modemSleepRqPin >= 0) {
        // Don't go to sleep if there's not a wake pin!
        MS_DBG(F("Setting pin"), _modemSleepRqPin,
               _wakeLevel ? F("HIGH") : F("LOW"), F("to wake"), _modemName);
        digitalWrite(_modemSleepRqPin, _wakeLevel);
        MS_DBG(F("Turning off airplane mode..."));
        if (gsmModem.commandMode()) {
            gsmModem.sendAT(GF("AM"), 0);
            gsmModem.waitResponse();
            // Write changes to flash and apply them
            gsmModem.writeChanges();
            // Exit command mode
            gsmModem.exitCommand();
        }
        return true;
    } else {
        return true;
    }
}


// We turn on airplane mode in before sleep
bool DigiXBeeCellularTransparent::modemSleepFxn(void) {
    if (_modemSleepRqPin >= 0) {
        MS_DBG(F("Turning on airplane mode..."));
        if (gsmModem.commandMode()) {
            gsmModem.sendAT(GF("AM"), 0);
            gsmModem.waitResponse();
            // Write changes to flash and apply them
            gsmModem.writeChanges();
            // Exit command mode
            gsmModem.exitCommand();
        }
        MS_DBG(F("Setting pin"), _modemSleepRqPin,
               !_wakeLevel ? F("HIGH") : F("LOW"), F("to put"), _modemName,
               F("to sleep"));
        digitalWrite(_modemSleepRqPin, !_wakeLevel);
        return true;
    } else {
        return true;
    }
}


bool DigiXBeeCellularTransparent::extraModemSetup(void) {
    bool success = true;
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    MS_DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode()) {
        MS_DBG(F("Setting I/O Pins..."));
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(GF("D8"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive
        // commands This can be used as proxy for status indication if the true
        // status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        gsmModem.sendAT(GF("D7"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on the associate LED (if you're using a board with one)
        // NOTE:  Only pin 15/DIO5 can be used for this function
        gsmModem.sendAT(GF("D5"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on the RSSI indicator LED (if you're using a board with one)
        // NOTE:  Only pin 6/DIO10/PWM0 can be used for this function
        gsmModem.sendAT(GF("P0"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Put the XBee in pin sleep mode
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        gsmModem.sendAT(GF("SO"), 0);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Other Options..."));
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        gsmModem.sendAT(GF("DO"), 0);
        success &= gsmModem.waitResponse() == 1;
        // Ask data to be "packetized" and sent out with every new line (0x0A
        gsmModem.sendAT(GF("TD0A"));
        success &= gsmModem.waitResponse() == 1;
        // Make sure pins 7&8 are not set for USB direct on XBee3 units
        gsmModem.sendAT(GF("P1"), 0);
        success &= gsmModem.waitResponse() == 1;
        // Set the socket timeout to 10s
        gsmModem.sendAT(GF("TM"), 64);
        success &= gsmModem.waitResponse() == 1;
        // MS_DBG(F("Setting Cellular Carrier Options..."));
        // // Carrier Profile - 1 = No profile/SIM ICCID selected
        // gsmModem.sendAT(GF("CP"),0);
        // gsmModem.waitResponse();  // Don't check for success - only works on
        // LTE
        // // Cellular network technology - LTE-M/NB IoT
        // gsmModem.sendAT(GF("N#"),0);
        // gsmModem.waitResponse();  // Don't check for success - only works on
        // LTE
        // Put the network connection parameters into flash
        success &= gsmModem.gprsConnect(_apn);
        MS_DBG(F("Ensuring XBee is in transparent mode..."));
        // Make sure we're really in transparent mode
        gsmModem.sendAT(GF("AP0"));
        success &= gsmModem.waitResponse() == 1;
        // Write changes to flash and apply them
        MS_DBG(F("Applying changes..."));
        gsmModem.writeChanges();
        // Exit command mode
        gsmModem.exitCommand();
        // Force restart the modem to make sure all settings take
        MS_DBG(F("Restarting XBee..."));
        success &= gsmModem.restart();
    } else {
        success = false;
    }

    if (success) {
        MS_DBG(F("... setup successful!"));
    } else {
        MS_DBG(F("... setup failed!"));
    }
    return success;
}


// Get the time from NIST via TIME protocol (rfc868)
// This would be much more efficient if done over UDP, but I'm doing it
// over TCP because I don't have a UDP library for all the modems.
/*uint32_t DigiXBeeCellularTransparent::getNISTTime(void)
{
    // bail if not connected to the internet
    gsmModem.commandMode();
    if (!gsmModem.isNetworkConnected())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        gsmModem.exitCommand();
        return 0;
    }

    // We can get the NIST timestamp directly from the XBee
    gsmModem.sendAT(GF("DT0"));
    String res = gsmModem.readResponseString();
    gsmModem.exitCommand();
    MS_DBG(F("Raw hex response from XBee:"), res);
    char buf[9] = {0,};
    res.toCharArray(buf, 9);
    uint32_t secFrom2000 = strtol(buf, 0, 16);
    MS_DBG(F("Seconds from Jan 1, 2000 from XBee (UTC):"), secFrom2000);

    // Convert from seconds since Jan 1, 2000 to 1970
    uint32_t unixTimeStamp = secFrom2000 + 946684800 ;
    MS_DBG(F("Unix Timestamp returned by NIST (UTC):"), unixTimeStamp);

    // If before Jan 1, 2019 or after Jan 1, 2030, most likely an error
    if (unixTimeStamp < 1546300800)
    {
        return 0;
    }
    else if (unixTimeStamp > 1893456000)
    {
        return 0;
    }
    else
    {
        return unixTimeStamp;
    }
}*/
uint32_t DigiXBeeCellularTransparent::getNISTTime(void) {
    /* bail if not connected to the internet */
    if (!isInternetAvailable()) {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    /* Try up to 12 times to get a timestamp from NIST */
    for (uint8_t i = 0; i < 12; i++) {
        // Must ensure that we do not ping the daylight more than once every 4
        // seconds.  NIST clearly specifies here that this is a requirement for
        // all software that accesses its servers:
        // https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() < _lastNISTrequest + 4000) {}

        /* Make TCP connection */
        MS_DBG(F("\nConnecting to NIST daytime Server"));
        bool connectionMade = false;

        /* This is the IP address of time-e-wwv.nist.gov  */
        /* XBee's address lookup falters on time.nist.gov */
        IPAddress ip(132, 163, 97, 6);
        connectionMade = gsmClient.connect(ip, 37, 15);
        /* Wait again so NIST doesn't refuse us! */
        delay(4000L);
        /* Try sending something to ensure connection */
        gsmClient.println('!');

        /* Wait up to 5 seconds for a response */
        if (connectionMade) {
            uint32_t start = millis();
            while (gsmClient && gsmClient.available() < 4 &&
                   millis() - start < 5000L) {}

            if (gsmClient.available() >= 4) {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte response[4] = {0};
                gsmClient.read(response, 4);
                gsmClient.stop();
                return parseNISTBytes(response);
            } else {
                MS_DBG(F("NIST Time server did not respond!"));
                gsmClient.stop();
            }
        } else {
            MS_DBG(F("Unable to open TCP to NIST!"));
        }
    }
    return 0;
}


bool DigiXBeeCellularTransparent::updateModemMetadata(void) {
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI           = -9999;
    loggerModem::_priorSignalPercent  = -9999;
    loggerModem::_priorBatteryState   = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorModemTemp      = -9999;

    // Initialize variable
    int16_t signalQual = -9999;

    // Enter command mode only once
    MS_DBG(F("Entering Command Mode:"));
    gsmModem.commandMode();

    // Try for up to 15 seconds to get a valid signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    uint32_t startMillis = millis();
    do {
        MS_DBG(F("Getting signal quality:"));
        signalQual = gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);
        if (signalQual != 0 && signalQual != -9999) break;
        delay(250);
    } while ((signalQual == 0 || signalQual == -9999) &&
             millis() - startMillis < 15000L && success);

    // Convert signal quality to RSSI
    loggerModem::_priorRSSI = signalQual;
    MS_DBG(F("CURRENT RSSI:"), signalQual);
    loggerModem::_priorSignalPercent = getPctFromRSSI(signalQual);
    MS_DBG(F("CURRENT Percent signal strength:"), getPctFromRSSI(signalQual));

    MS_DBG(F("Getting chip temperature:"));
    loggerModem::_priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem temperature:"), loggerModem::_priorModemTemp);

    // Exit command modem
    MS_DBG(F("Leaving Command Mode:"));
    gsmModem.exitCommand();

    return success;
}
