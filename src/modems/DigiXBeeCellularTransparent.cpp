/**
 * @file DigiXBeeCellularTransparent.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * @author Greg Cutrell <gcutrell@limno.com>
 *
 * @brief Implements the DigiXBeeCellularTransparent class.
 */

// Included Dependencies
#include "DigiXBeeCellularTransparent.h"

// Constructor/Destructor
DigiXBeeCellularTransparent::DigiXBeeCellularTransparent(
    Stream* modemStream, int8_t powerPin, int8_t statusPin, bool useCTSStatus,
    int8_t modemResetPin, int8_t modemSleepRqPin, const char* apn,
    const char* user, const char* pwd)
    : DigiXBee<TinyGsmXBee,                      // Modem Type
               TinyGsmXBee::GsmClientXBee,       // TCP Client Type
               TinyGsmXBee::GsmClientSecureXBee  // SSL Client
                                                 // Type
               >(modemStream, powerPin, statusPin, useCTSStatus, modemResetPin,
                 modemSleepRqPin),
      _apn(apn),
      _user(user),
      _pwd(pwd) {}

bool DigiXBeeCellularTransparent::connectWithCredentials() {
    return gsmModem.gprsConnect(_apn, _user, _pwd);
}

// We turn off airplane mode in the wake.
bool DigiXBeeCellularTransparent::modemWakeFxn() {
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
bool DigiXBeeCellularTransparent::modemSleepFxn() {
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
        gsmModem.stream.flush();
        return true;
    } else {
        gsmModem.stream.flush();
        return true;
    }
}


bool DigiXBeeCellularTransparent::extraModemSetup() {
    bool success = true;
    // First run the TinyGSM init() function for the XBee.
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    _modemName = gsmModem.getModemName();
    // Then enter command mode to set pin outputs.
    MS_DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode()) {
        bool changesMade = false;
        MS_DBG(F("Setting I/O Pins..."));
        // Enable pin sleep functionality on `DIO9` if a pin is assigned.
        // NOTE: Only the `DTR_N/SLEEP_RQ/DIO8` pin (9 on the bee socket) can be
        // used for this pin sleep/wake.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("D8"),
                                                      _modemSleepRqPin >= 0);
        // Enable status indication on `DIO9` if a pin is assigned - it will be
        // HIGH when the XBee is awake.
        // NOTE: Only the `ON/SLEEP_N/DIO9` pin (13 on the bee socket) can be
        // used for direct status indication.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("D9"),
                                                      _statusPin >= 0);
        // Enable CTS on `DIO7` if a pin is assigned  - it will be `LOW` when it
        // is clear to send data to the XBee.  This can be used as proxy for
        // status indication if that pin is not readable.
        // NOTE: Only the `CTS_N/DIO7` pin (12 on the bee socket) can be used
        // for CTS.
        changesMade |= gsmModem.changeSettingIfNeeded(
            GF("D7"), _statusPin >= 0 && !_statusLevel);
        // Enable association indication on `DIO5` - this is should be directly
        // attached to an LED if possible.
        // - Solid light indicates no connection
        // - Single blink indicates connection
        // - double blink indicates connection but failed TCP link on last
        // attempt
        //
        // NOTE: Only the `Associate/DIO5` pin (15 on the bee socket) can be
        // used for this function.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("D5"), 1);
        // Enable RSSI PWM output on `DIO10` - this should be directly attached
        // to an LED if possible.  A higher PWM duty cycle (and thus brighter
        // LED) indicates better signal quality.
        // NOTE: Only the `DIO10/PWM0` pin (6 on the bee socket) can be used for
        // this function.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("P0"), 1);
        // Enable pin sleep on the XBee.
        MS_DBG(F("Setting Sleep Options..."));
        changesMade |= gsmModem.changeSettingIfNeeded(GF("SM"),
                                                      _modemSleepRqPin >= 0);
        MS_DBG(F("Setting Other Options..."));
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE: LTE-M's PSM (Power Save Mode) sounds good, but there's no easy
        // way on the LTE-M Bee to wake the cell chip itself from PSM, so we'll
        // use the Digi pin sleep instead.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("DO"), 0);

        // Ask data to be "packetized" and sent out with every new line (0x0A)
        // character.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("TD"), 10);
        // Make sure USB direct is NOT enabled on the XBee3 units.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("P1"), 0);
        // Set the socket timeout to 10s (this is default).
        changesMade |= gsmModem.changeSettingIfNeeded(GF("TM"), 64);

        MS_DBG(F("Setting the APN..."));
        // Save the network connection parameters.
        // This function does an internal check for changes made
        success &= gsmModem.gprsConnect(_apn, _user, _pwd);

        MS_DBG(F("Ensuring XBee is in transparent mode..."));
        // Make sure we're really in transparent mode.
        changesMade |= gsmModem.changeSettingIfNeeded(GF("AP"), 0);

        // Write all changes to flash and apply them.
        if (changesMade) {
            MS_DBG(F("Applying changes..."));
            success &= gsmModem.writeChanges();
        }
        // Finally, exit command mode.
        gsmModem.exitCommand();
        // Force restart the modem to make sure all settings take.
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

// Get the time from NIST via TIME protocol (RFC-868)
uint32_t DigiXBeeCellularTransparent::getNISTTime() {
    // bail if not connected to the internet
    if (!isInternetAvailable()) {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    for (uint8_t i = 0; i < NIST_TIME_PROTOCOL_RETRIES; i++) {
        // Must ensure that we do not ping the daylight servers more than once
        // every 4 seconds.  NIST clearly specifies here that this is a
        // requirement for all software that accesses its servers:
        // https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() - _lastNISTrequest < NIST_TIME_PROTOCOL_SPACING) {
            yield();
        }

        // Make TCP connection
        MS_DBG(F("\nConnecting to NIST time server"));
        bool connectionMade = false;

        // These are is the IP address of time-[a,b,c,d]-wwv.nist.gov
        // XBee's address lookup falters on time.nist.gov
        IPAddress nistIPs[] = {
            IPAddress(132, 163, 97, 1), IPAddress(132, 163, 97, 2),
            IPAddress(132, 163, 97, 3), IPAddress(132, 163, 97, 4),
            IPAddress(132, 163, 97, 6), IPAddress(132, 163, 97, 8)};
        const uint8_t ipIndex = i % (sizeof(nistIPs) / sizeof(nistIPs[0]));
        MS_DBG(F("\nConnecting to NIST time server at ip"), nistIPs[ipIndex],
               F("attempt"), i, F("of"), NIST_TIME_PROTOCOL_RETRIES);

        // NOTE:  This "connect" only sets up the connection parameters, the TCP
        // socket isn't actually opened until we first send data (the '!' below)
        TinyGsmXBee::GsmClientXBee gsmClient(gsmModem);
        connectionMade   = gsmClient.connect(nistIPs[ipIndex],
                                             NIST_TIME_PROTOCOL_PORT);
        _lastNISTrequest = millis();
        // Need to send something before connection is made
        gsmClient.println('!');

        // Wait up to 5 seconds for a response
        if (connectionMade) {
            uint32_t start = millis();
            while (gsmClient &&
                   gsmClient.available() < NIST_TIME_PROTOCOL_BYTES &&
                   millis() - start < NIST_TIME_PROTOCOL_TIMEOUT) {
                yield();
            }

            if (gsmClient.available() >= NIST_TIME_PROTOCOL_BYTES) {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte response[NIST_TIME_PROTOCOL_BYTES] = {0};
                gsmClient.read(response, NIST_TIME_PROTOCOL_BYTES);
                gsmClient.stop();
                uint32_t nistParsed = parseNISTBytes(response);
                if (nistParsed != 0) {
                    return nistParsed;
                } else {
                    MS_DBG(F("NIST response was invalid!"));
                }
            } else {
                MS_DBG(F("NIST time server did not respond!"));
                gsmClient.stop();
            }
        } else {
            MS_DBG(F("Unable to open TCP to NIST!"));
        }
    }
    return 0;
}


bool DigiXBeeCellularTransparent::updateModemMetadata() {
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI           = MS_INVALID_VALUE;
    loggerModem::_priorSignalPercent  = MS_INVALID_VALUE;
    loggerModem::_priorBatteryState   = MS_INVALID_VALUE;
    loggerModem::_priorBatteryPercent = MS_INVALID_VALUE;
    loggerModem::_priorBatteryVoltage = MS_INVALID_VALUE;
    loggerModem::_priorModemTemp      = MS_INVALID_VALUE;
    // Initialize variable
    int16_t rssi = MS_INVALID_VALUE;

    MS_DBG(F("Modem polling settings:"), String(_pollModemMetaData, BIN));

    // if not enabled don't collect data
    if (_pollModemMetaData == 0) {
        MS_DBG(F("No modem metadata to update"));
        return false;
    }

    // Enter command mode only once
    MS_DBG(F("Entering Command Mode to update modem metadata:"));
    success &= gsmModem.commandMode();
    // Enter command mode only once
    MS_DBG(F("Entering Command Mode to update modem metadata:"));
    if (!gsmModem.commandMode()) {
        MS_DBG(F("Failed to enter command mode - cannot update metadata"));
        return false;
    }

    if ((_pollModemMetaData & MODEM_RSSI_ENABLE_BITMASK) ==
            MODEM_RSSI_ENABLE_BITMASK ||
        (_pollModemMetaData & MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) ==
            MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) {
        // Try for up to 15 seconds to get a valid signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no
        // service/signal. The TinyGSM getSignalQuality function returns the
        // same "no signal" value (99 CSQ or 0 RSSI) in all 3 cases.
        uint32_t startMillis = millis();
        do {
            MS_DBG(F("Getting signal quality:"));
            rssi = gsmModem.getSignalQuality();
            MS_DBG(F("Raw signal quality:"), rssi);
            if (rssi != 0 && rssi != MS_INVALID_VALUE) break;
            delay(250);
        } while ((rssi == 0 || rssi == MS_INVALID_VALUE) &&
                 millis() - startMillis < 15000L);

        // Convert signal quality to RSSI
        loggerModem::_priorRSSI = rssi;
        MS_DBG(F("CURRENT RSSI:"), rssi);

        // Convert signal quality to a percent
        loggerModem::_priorSignalPercent = getPctFromRSSI(rssi);
        MS_DBG(F("CURRENT Percent signal strength:"),
               loggerModem::_priorSignalPercent);

        success &= ((rssi != MS_INVALID_VALUE) && (rssi != 0));
    } else {
        MS_DBG(F("Polling for both RSSI and signal strength is disabled"));
    }


    if ((_pollModemMetaData & MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) ==
        MODEM_BATTERY_VOLTAGE_ENABLE_BITMASK) {
        MS_DBG(F("Getting input voltage:"));
        uint16_t volt_mV = 9999;
        volt_mV          = gsmModem.getBattVoltage();
        MS_DBG(F("CURRENT Modem battery (mV):"), volt_mV);
        if (volt_mV != 9999) {
            loggerModem::_priorBatteryVoltage =
                static_cast<float>(volt_mV / 1000.0f);
        } else {
            loggerModem::_priorBatteryVoltage =
                static_cast<float>(MS_INVALID_VALUE);
        }

        success &= ((volt_mV != 9999) && (volt_mV != 0));
    } else {
        MS_DBG(F("Polling for modem battery voltage is disabled"));
    }

    if ((_pollModemMetaData & MODEM_TEMPERATURE_ENABLE_BITMASK) ==
        MODEM_TEMPERATURE_ENABLE_BITMASK) {
        MS_DBG(F("Getting chip temperature:"));
        float chip_temp              = getModemChipTemperature();
        loggerModem::_priorModemTemp = chip_temp;
        MS_DBG(F("CURRENT Modem temperature(C):"),
               loggerModem::_priorModemTemp);

        // TinyGSM returns -9999 when it fails to get a temperature reading, so
        // check for that as well as the invalid value
        success &= ((chip_temp != MS_INVALID_VALUE) && (chip_temp != -9999));
    } else {
        MS_DBG(F("Polling for modem chip temperature is disabled"));
    }

    // Exit command mode
    MS_DBG(F("Leaving Command Mode after updating modem metadata:"));
    gsmModem.exitCommand();

    return success;
}
