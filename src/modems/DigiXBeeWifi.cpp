/**
 * @file DigiXBeeWifi.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the DigiXBeeWifi class.
 */

// Included Dependencies
#include "DigiXBeeWifi.h"
#include "LoggerModemMacros.h"

// Constructor/Destructor
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream, int8_t powerPin,
                           int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char* ssid, const char* pwd,
                           bool maintainAssociation)
    : DigiXBee(powerPin, statusPin, useCTSStatus, modemResetPin,
               modemSleepRqPin),
#ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
      _modemATDebugger(*modemStream, MS_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger, modemResetPin),
#else
      gsmModem(*modemStream, modemResetPin),
#endif

      _ssid(ssid),
      _pwd(pwd),
      _maintainAssociation(maintainAssociation) {
}

// Destructor
DigiXBeeWifi::~DigiXBeeWifi() {}

MS_IS_MODEM_AWAKE(DigiXBeeWifi);
MS_MODEM_WAKE(DigiXBeeWifi);


// This is different MS_MODEM_CONNECT_INTERNET in that it doesn't attempt to
// resend credentials
bool DigiXBeeWifi::connectInternet(uint32_t maxConnectionTime) {
    bool success = true;

    /** Power up, if necessary */
    bool wasPowered = true;
    if (_millisPowerOn == 0) {
        modemPowerUp();
        wasPowered = false;
    }

    /** Check if the modem was awake, wake it if not */
    bool wasAwake = isModemAwake();
    if (!wasAwake) {
        MS_DBG(F("Waiting for modem to boot after power on ..."));
        while (millis() - _millisPowerOn < _wakeDelayTime_ms) { /** wait */
        }
        MS_DBG(F("Waking up the modem to connect to the internet ..."));
        success &= modemWake();
    } else {
        MS_DBG(F("Modem was already awake and should be ready."));
    }

    if (success) {
        MS_START_DEBUG_TIMER
        MS_DBG(F("\nAttempting to connect to WiFi without sending new "
                 "credentials..."));
        if (!(gsmModem.isNetworkConnected())) {
            if (!gsmModem.waitForNetwork(maxConnectionTime)) {
                PRINTOUT(F("... WiFi connection failed"));
                success = false;
            }
        }
        MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER,
               F("milliseconds!"));
    }
    if (!wasPowered) {
        MS_DBG(F("Modem was powered to connect to the internet!  "
                 "Remember to turn it off when you're done."));
    } else if (!wasAwake) {
        MS_DBG(F("Modem was woken up to connect to the internet!   "
                 "Remember to put it to sleep when you're done."));
    }
    return success;
}
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);

MS_MODEM_CREATE_CLIENT(DigiXBeeWifi, XBee);
MS_MODEM_DELETE_CLIENT(DigiXBeeWifi, XBee);
MS_MODEM_CREATE_SECURE_CLIENT(DigiXBeeWifi, XBee);
MS_MODEM_DELETE_SECURE_CLIENT(DigiXBeeWifi, XBee);

MS_MODEM_GET_MODEM_SIGNAL_QUALITY(DigiXBeeWifi);
MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeWifi);

bool DigiXBeeWifi::extraModemSetup(void) {
    bool success = true;
    /** First run the TinyGSM init() function for the XBee. */
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    if (!success) { MS_DBG(F("Failed TinyGSM init")); }
    _modemName = gsmModem.getModemName();
    /** Then enter command mode to set pin outputs. */
    MS_DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode()) {
        MS_DBG(F("Getting Detailed Modem Version..."));

        gsmModem.getSeries();
        _modemName       = gsmModem.getModemName();
        String xbeeSnLow = gsmModem.sendATGetString(
            GFP("SL"));  // Request Module MAC/Serial Number Low
        String xbeeSnHigh = gsmModem.sendATGetString(
            GFP("SH"));  // Request Module MAC/Serial Number High
        _modemSerialNumber = xbeeSnHigh + xbeeSnLow;
        _modemHwVersion =
            gsmModem.sendATGetString(GFP("HV"));  // Request Module Hw Version
        _modemFwVersion =
            gsmModem.sendATGetString(GFP("VR"));  // Firmware Version
        PRINTOUT(F("Digi XBee"), _modemName, F("Mac/SN"), xbeeSnHigh, xbeeSnLow,
                 F("HwVer"), _modemHwVersion, F("FwVer"), _modemFwVersion);

        bool changesMade = false;
        MS_DBG(F("Enabling XBee Pin Pullups..."));
        // Leave all unused pins disconnected. Use the PR command to pull
        // all of the inputs on the device high using 40 k internal pull-up
        // resistors. You do not need a specific treatment for unused
        // outputs.
        //   Mask Bit Description
        // 1 0001  0 TH11 DIO4
        // 1 0002  1 TH17 DIO3
        // 1 0004  2 TH18 DIO2
        // 1 0008  3 TH19 DIO1
        // 1 0010  4 TH20 DIO0
        // 1 0020  5 TH16 DIO6/RTS
        // 0 0040  6 TH09 DIO8/DTR/Sleep Request
        // 0 0080  7 TH03 DIN
        // 1 0100  8 TH15 DIO5/Associate
        // 0 0200  9 TH13 DIO9/- OnSLEEP
        // 1 0400 10 TH04 DIO12
        // 1 0800 11 TH06 DIO10/PWM RSSI
        // 1 1000 12 TH07 DIO11/PWM1
        // 1 2000 13 TH12 DIO7/-CTR
        // 0 4000 14 TH02 DIO13/DOUT
        //   3D3F
        bool changedRP = gsmModem.changeSettingIfNeeded(GF("PR"), "3D3F");
        changesMade |= changedRP;
        if (changedRP) {
            MS_DBG(F("Pin pullup bits changed to"), F("3D3F"));
        } else {
            MS_DEEP_DBG(F("Pin pullup bits not changed"));
        }


        MS_DBG(F("Setting I/O Pins..."));
        // To use sleep pins they physically need to be enabled.
        /** Enable pin sleep functionality on `DIO8` if a pin is assigned.
         * NOTE: Only the `DTR_N/SLEEP_RQ/DIO8` pin (9 on the bee socket) can be
         * used for this pin sleep/wake. */
        bool changedD8 = gsmModem.changeSettingIfNeeded(GF("D8"),
                                                        _modemSleepRqPin >= 0);
        changesMade |= changedD8;
        if (changedD8) {
            MS_DBG(F("DTR_N/SLEEP_RQ/DIO8 changed to"), _modemSleepRqPin >= 0);
        } else {
            MS_DEEP_DBG(F("DTR_N/SLEEP_RQ/DIO8 not changed"));
        }

        /** Enable status indication on `DIO9` if a pin is assigned - it will be
         * HIGH when the XBee is awake.
         * NOTE: Only the `ON/SLEEP_N/DIO9` pin (13 on the bee socket) can be
         * used for direct status indication. */
        bool changedD9 = gsmModem.changeSettingIfNeeded(GF("D9"),
                                                        _statusPin >= 0);
        changesMade |= changedD9;
        if (changedD9) {
            MS_DBG(F("ON/SLEEP_N/DIO9 changed to"), _statusPin >= 0);
        } else {
            MS_DEEP_DBG(F("ON/SLEEP_N/DIO9 not changed"));
        }

        /** Enable CTS on `DIO7` if a pin is assigned - it will be `LOW` when
         * it is clear to send data to the XBee.  This can be used as proxy for
         * status indication if that pin is not readable.
         * NOTE: Only the `CTS_N/DIO7` pin (12 on the bee socket) can be used
         * for CTS. */
        bool changedD7 = gsmModem.changeSettingIfNeeded(
            GF("D7"), _statusPin >= 0 && !_statusLevel);
        changesMade |= changedD7;
        if (changedD7) {
            MS_DBG(F("CTS_N/DIO7 changed to"),
                   _statusPin >= 0 && !_statusLevel);
        } else {
            MS_DEEP_DBG(F("CTS_N/DIO7 not changed"));
        }

        /** Enable association indication on `DIO5` - this is should be
         * directly attached to an LED if possible.
         * - Solid light indicates no connection
         * - Single blink indicates connection
         * - double blink indicates connection but failed TCP link on last
         * attempt
         *
         * NOTE: Only the `Associate/DIO5` pin (15 on the bee socket) can be
         * used for this function. */
        bool changedD5 = gsmModem.changeSettingIfNeeded(GF("D5"), 1);
        changesMade |= changedD5;
        if (changedD5) {
            MS_DBG(F("Associate/DIO5 changed to"), 1);
        } else {
            MS_DEEP_DBG(F("Associate/DIO5 not changed"));
        }

        /** Enable RSSI PWM output on `DIO10` - this should be directly
         * attached to an LED if possible.  A higher PWM duty cycle (and
         * thus brighter LED) indicates better signal quality. NOTE: Only
         * the `DIO10/PWM0` pin (6 on the bee socket) can be used for this
         * function. */
        bool changedP0 = gsmModem.changeSettingIfNeeded(GF("D5"), 1);
        changesMade |= changedP0;
        if (changedP0) {
            MS_DBG(F("DIO10/PWM0 changed to"), 1);
        } else {
            MS_DEEP_DBG(F("DIO10/PWM0 not changed"));
        }

        /** Put the XBee in pin sleep mode in conjuction with D8=1 */
        // From the S6B User Guide:
        // 0  - Normal. In this mode the device never sleeps.
        // 1  - Pin Sleep. In this mode the device honors the SLEEP_RQ pin.
        //      Set D8 (DIO8 Configuration) to the sleep request function: 1.
        // 4  - Cyclic Sleep. In this mode the device repeatedly sleeps for the
        // value specified by SP and spends ST time awake.
        // 5  - Cyclic Sleep with Pin Wake. In this mode the device acts as in
        // Cyclic Sleep but does not sleep if the SLEEP_RQ pin is inactive,
        // allowing the device to be kept awake or woken by the connected
        // system.
        bool changedSM = gsmModem.changeSettingIfNeeded(GF("SM"),
                                                        _modemSleepRqPin >= 0);
        changesMade |= changedSM;
        if (changedSM) {
            MS_DBG(F("Sleep mode changed to"), _modemSleepRqPin >= 0);
        } else {
            MS_DEEP_DBG(F("Sleep mode not changed"));
        }
        // Disassociate from the network for the lowest power deep sleep.
        // From S6B User Guide:
        // 0x40 - Stay associated with AP during sleep. Draw more current
        // (+10mA?) during sleep with this option enabled, but also avoid data
        // loss. [0x40 = 64]
        // 0x100 - For cyclic sleep, ST specifies the time before returning
        // to sleep. With this bit set, new receptions from either the serial or
        // the RF port do not restart the ST timer.  Current implementation does
        // not support this bit being turned off. [0x100 = 256]
        bool changedSO = gsmModem.changeSettingIfNeeded(
            GF("SO"), _maintainAssociation ? "40" : "100");
        changesMade |= changedSO;
        if (changedSO) {
            MS_DBG(F("Sleep options changed to"),
                   _maintainAssociation ? "0x40" : "0x100");
        } else {
            MS_DEEP_DBG(F("Sleep options not changed"));
        }

        /** Write pin and sleep options to flash and apply them, if needed. */
        if (changesMade) {
            MS_DBG(F("Applying changes to pin and sleep options..."));
            gsmModem.writeChanges();
        } else {
            MS_DBG(F("No pin or sleep option changes to apply"));
        }

        MS_DBG(F("Setting Wifi Network Options..."));
        // Put the network connection parameters into flash
        // NOTE: This will write to the flash every time if there is a password
        // set!
        success &= gsmModem.networkConnect(_ssid, _pwd);
        if (!success) {
            MS_DBG(F("Fail Connect "), success);
            success = true;
        }

        // Set to TCP mode
        changesMade        = false;
        bool changedIPMode = gsmModem.changeSettingIfNeeded(GF("IP"), 1);
        changesMade |= changedIPMode;
        if (changedIPMode) {
            MS_DBG(F("IP mode changed to"), 1);
        } else {
            MS_DEEP_DBG(F("IP mode not changed"));
        }


        /** Set the socket timeout to 10s (this is default).*/
        bool changedTM = gsmModem.changeSettingIfNeeded(GF("TM"), "64");
        changesMade |= changedTM;
        if (changedTM) {
            MS_DBG(F("Socket timeout changed to"), F("0x64"));
        } else {
            MS_DEEP_DBG(F("Socket timeout not changed"));
        }

        /** Set the destination IP to 0 (this is default). */
        bool changedDL = gsmModem.changeSettingIfNeeded(GF("DL"),
                                                        GF("0.0.0.0"));
        changesMade |= changedDL;
        if (changedDL) {
            MS_DBG(F("Destination IP changed to"), F("0.0.0.0"));
        } else {
            MS_DEEP_DBG(F("Destination IP not changed"));
        }

        /** Write all changes to flash and apply them. */
        if (changesMade) {
            MS_DBG(F("Applying changes to socket times..."));
            success &= gsmModem.writeChanges();
        }

        if (success) {
            MS_DBG(F("Successfully setup Wifi Network"), _ssid);
        } else {
            MS_DBG(F("Failed Setting WiFi"), _ssid);
        }

        // Since this is the only time we're going to send the credentials,
        // confirm that we can connect to the network and get both an IP and DNS
        // address.
        if (!(gsmModem.isNetworkConnected())) {
            if (!gsmModem.waitForNetwork()) {
                PRINTOUT(
                    F("... Initial WiFi connection failed - resetting module"));
                loggerModem::modemHardReset();
                delay(50);
                success = false;
            } else {
                PRINTOUT(F("... Initial WiFi connection succeeded!"));
                success = true;
            }
        } else {
            PRINTOUT(F("... Initial WiFi connection succeeded!"));
            success = true;
        }
        gsmModem.exitCommand();
    }

    return success;
}


void DigiXBeeWifi::disconnectInternet(void) {
    // Ensure Wifi XBee IP socket torn down by forcing connection to
    // localhost IP For A XBee S6B bug, then force restart.
    TinyGsmXBee::GsmClientXBee gsmClient(
        gsmModem);  // need to create again to force close
    String        oldRemoteIp = gsmClient.remoteIP();
    IPAddress     newHostIp   = IPAddress(127, 0, 0, 1);  // localhost
    gsmClient.connect(newHostIp, 80);
    MS_DBG(gsmModem.getBeeName(), oldRemoteIp, F("disconnectInternet set to"),
           gsmClient.remoteIP());

    gsmModem.restart();
}


// Get the time from NIST via TIME protocol (rfc868)
uint32_t DigiXBeeWifi::getNISTTime(void) {
    // bail if not connected to the internet
    if (!isInternetAvailable()) {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    for (uint8_t i = 0; i < NIST_SERVER_RETRYS; i++) {
        // Must ensure that we do not ping the daylight servers more than once
        // every 4 seconds.  NIST clearly specifies here that this is a
        // requirement for all software that accesses its servers:
        // https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() < _lastNISTrequest + 4000) {
            // wait
        }

        // Make TCP connection
        // Uses "TIME" protocol on port 37 NIST: This protocol is expensive,
        // since it uses the complete tcp machinery to transmit only 32 bits
        // of data. FUTURE Users are *strongly* encouraged to upgrade to the
        // network time protocol (NTP), which is both more accurate and more
        // robust.
        MS_DBG(F("\nConnecting to NIST daytime Server"));
        bool connectionMade = false;

        // These are is the IP address of time-[a,b,c,d]-wwv.nist.gov
        // XBee's address lookup falters on time.nist.gov

        IPAddress nistIPs[] = {
            IPAddress(132, 163, 97, 1), IPAddress(132, 163, 97, 2),
            IPAddress(132, 163, 97, 3), IPAddress(132, 163, 97, 4),
            IPAddress(132, 163, 97, 6), IPAddress(132, 163, 97, 8)};
        MS_DBG(F("\nConnecting to NIST daytime Server at ip"), nistIPs[i],
               F("attempt"), i, F("of"), NIST_SERVER_RETRYS);

        // NOTE:  This "connect" only sets up the connection parameters, the TCP
        // socket isn't actually opened until we first send data (the '!' below)
        TinyGsmXBee::GsmClientXBee gsmClient(gsmModem);
        connectionMade = gsmClient.connect(nistIPs[i], TIME_PROTOCOL_PORT);
        // Need to send something before connection is made
        gsmClient.println('!');

        // Wait up to 5 seconds for a response
        if (connectionMade) {
            uint32_t start = millis();
            while (gsmClient && gsmClient.available() < NIST_RESPONSE_BYTES &&
                   millis() - start < 5000L) {
                // wait
            }

            if (gsmClient.available() >= 4) {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte response[NIST_RESPONSE_BYTES] = {0};
                gsmClient.read(response, NIST_RESPONSE_BYTES);
                gsmClient.stop();
                uint32_t nistParsed = parseNISTBytes(response);
                if (nistParsed != 0) {
                    return nistParsed;
                } else {
                    MS_DBG(F("NIST response was invalid!"));
                }
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


bool DigiXBeeWifi::updateModemMetadata(void) {
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI           = -9999;
    loggerModem::_priorSignalPercent  = -9999;
    loggerModem::_priorBatteryState   = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorModemTemp      = -9999;

    MS_DBG(F("Modem polling settings:"), String(_pollModemMetaData, BIN));

    // if not enabled don't collect data
    if (_pollModemMetaData == 0) {
        MS_DBG(F("No modem metadata to update"));
        return false;
    }

    // Enter command mode only once
    MS_DBG(F("Entering Command Mode to update modem metadata:"));
    success &= gsmModem.commandMode();

    if ((_pollModemMetaData & MODEM_RSSI_ENABLE_BITMASK) ==
            MODEM_RSSI_ENABLE_BITMASK ||
        (_pollModemMetaData & MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) ==
            MODEM_PERCENT_SIGNAL_ENABLE_BITMASK) {
        // Assume a signal has already been established.
        // Try to get a valid signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no
        // service/signal. The TinyGSM getSignalQuality function returns the
        // same "no signal" value (99 CSQ or 0 RSSI) in all 3 cases.

        // Try up to 5 times to get a signal quality
        int8_t  num_trys_remaining = 5;
        int16_t rssi               = -9999;
        do {
            rssi = gsmModem.getSignalQuality();
            MS_DBG(F("Raw signal quality ("), num_trys_remaining, F("):"),
                   rssi);
            if (rssi != 0 && rssi != -9999) break;
            num_trys_remaining--;
        } while ((rssi == 0 || rssi == -9999) && num_trys_remaining);


        // Convert signal quality to a percent
        loggerModem::_priorSignalPercent = getPctFromRSSI(rssi);
        MS_DBG(F("CURRENT Percent signal strength:"),
               loggerModem::_priorSignalPercent);

        loggerModem::_priorRSSI = rssi;
        MS_DBG(F("CURRENT RSSI:"), rssi);

        success &= ((rssi != -9999) && (rssi != 0));
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
                static_cast<float>(volt_mV / 1000);
        } else {
            loggerModem::_priorBatteryVoltage = static_cast<float>(-9999);
        }

        success &= ((volt_mV != 9999) && (volt_mV != 0));
    } else {
        MS_DBG(F("Polling for modem battery voltage is disabled"));
    }

    if ((_pollModemMetaData & MODEM_TEMPERATURE_ENABLE_BITMASK) ==
        MODEM_TEMPERATURE_ENABLE_BITMASK) {
        MS_DBG(F("Getting chip temperature:"));
        float chip_temp = -9999;
        chip_temp       = getModemChipTemperature();
        MS_DBG(F("CURRENT Modem temperature(C):"),
               loggerModem::_priorModemTemp);
        if (chip_temp != -9999.f) {
            loggerModem::_priorModemTemp = chip_temp;
        } else {
            loggerModem::_priorModemTemp = static_cast<float>(-9999);
        }

        success &= ((chip_temp != -9999.f) && (chip_temp != 0));
    } else {
        MS_DBG(F("Polling for modem chip temperature is disabled"));
    }

    // Exit command mode
    MS_DBG(F("Leaving Command Mode after updating modem metadata:"));
    gsmModem.exitCommand();


    // bump up the failure count if we didn't successfully update any of the
    // metadata parameters
    if (!success) { metadata_failure_count++; }

    // If the metadata update has failed more than the XBEE_RESET_THRESHOLD
    // number of times, restart the modem for next time. This is likely to take
    // over 2 seconds.
    if (metadata_failure_count >= XBEE_RESET_THRESHOLD) {
        metadata_failure_count = 0;  // reset the count
        PRINTOUT(F("updateModemMetadata forcing restart xbee..."));
        gsmModem.restart();
    }

    return success;
}
