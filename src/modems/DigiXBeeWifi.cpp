/**
 * @file DigiXBeeWifi.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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
                           const char* ssid, const char* pwd)
    : DigiXBee(powerPin, statusPin, useCTSStatus, modemResetPin,
               modemSleepRqPin),
#ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger, modemResetPin),
#else
      gsmModem(*modemStream, modemResetPin),
#endif
      gsmClient(gsmModem),
      _ssid(ssid),
      _pwd(pwd) {
}

// Destructor
DigiXBeeWifi::~DigiXBeeWifi() {}

MS_IS_MODEM_AWAKE(DigiXBeeWifi);
MS_MODEM_WAKE(DigiXBeeWifi);

MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);

MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeWifi);

bool DigiXBeeWifi::extraModemSetup(void) {
    bool success = true;
    /** First run the TinyGSM init() function for the XBee. */
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    /** Then enter command mode to set pin outputs. */
    MS_DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode()) {
        MS_DBG(F("Setting I/O Pins..."));
        /** Enable pin sleep functionality on `DIO9`.
         * NOTE: Only the `DTR_N/SLEEP_RQ/DIO8` pin (9 on the bee socket) can be
         * used for this pin sleep/wake. */
        gsmModem.sendAT(GF("D8"), 1);
        success &= gsmModem.waitResponse() == 1;
        /** Enable status indication on `DIO9` - it will be HIGH when the XBee
         * is awake.
         * NOTE: Only the `ON/SLEEP_N/DIO9` pin (13 on the bee socket) can be
         * used for direct status indication. */
        gsmModem.sendAT(GF("D9"), 1);
        success &= gsmModem.waitResponse() == 1;
        /** Enable CTS on `DIO7` - it will be `LOW` when it is clear to send
         * data to the XBee.  This can be used as proxy for status indication if
         * that pin is not readable.
         * NOTE: Only the `CTS_N/DIO7` pin (12 on the bee socket) can be used
         * for CTS. */
        gsmModem.sendAT(GF("D7"), 1);
        success &= gsmModem.waitResponse() == 1;
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
        success &= gsmModem.waitResponse() == 1;
        /** Enable RSSI PWM output on `DIO10` - this should be directly attached
         * to an LED if possible.  A higher PWM duty cycle (and thus brighter
         * LED) indicates better signal quality.
         * NOTE: Only the `DIO10/PWM0` pin (6 on the bee socket) can be used for
         * this function. */
        gsmModem.sendAT(GF("P0"), 1);
        success &= gsmModem.waitResponse() == 1;
        /** Enable pin sleep on the XBee. */
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"), 1);
        success &= gsmModem.waitResponse() == 1;
        /** Disassociate from the network for the lowest power deep sleep. */
        gsmModem.sendAT(GF("SO"), 200);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Wifi Network Options..."));
        /** Set the socket timeout to 10s (this is default). */
        gsmModem.sendAT(GF("TM"), 64);
        success &= gsmModem.waitResponse() == 1;
        /** Save the network connection parameters. */
        success &= gsmModem.networkConnect(_ssid, _pwd);
        MS_DBG(F("Ensuring XBee is in transparent mode..."));
        /* Make sure we're really in transparent mode. */
        gsmModem.sendAT(GF("AP0"));
        success &= gsmModem.waitResponse() == 1;
        /** Write all changes to flash and apply them. */
        MS_DBG(F("Applying changes..."));
        gsmModem.writeChanges();
        /** Finally, exit command mode. */
        gsmModem.exitCommand();
        /** Force restart the modem to make sure all settings take. */
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


void DigiXBeeWifi::disconnectInternet(void) {
    // Wifi XBee doesn't like to disconnect AT ALL, so we're doing nothing
    // If you do disconnect, you must power cycle before you can reconnect
    // to the same access point.
}


// Get the time from NIST via TIME protocol (rfc868)
uint32_t DigiXBeeWifi::getNISTTime(void) {
    // bail if not connected to the internet
    if (!isInternetAvailable()) {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    gsmClient.stop();

    // Try up to 12 times to get a timestamp from NIST
    for (uint8_t i = 0; i < 12; i++) {
        // Must ensure that we do not ping the daylight more than once every 4
        // seconds.  NIST clearly specifies here that this is a requirement for
        // all software that accesses its servers:
        // https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() < _lastNISTrequest + 4000) {
            // wait
        }

        // Make TCP connection
        MS_DBG(F("\nConnecting to NIST daytime Server"));
        bool connectionMade = false;

        // This is the IP address of time-e-wwv.nist.gov
        // XBee's address lookup falters on time.nist.gov
        // NOTE:  This "connect" only sets up the connection parameters, the TCP
        // socket isn't actually opened until we first send data (the '!' below)
        IPAddress ip(132, 163, 97, 6);
        connectionMade = gsmClient.connect(ip, 37);
        // Need to send something before connection is made
        gsmClient.println('!');

        // Wait up to 5 seconds for a response
        if (connectionMade) {
            uint32_t start = millis();
            while (gsmClient && gsmClient.available() < 4 &&
                   millis() - start < 5000L) {
                // wait
            }

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


bool DigiXBeeWifi::getModemSignalQuality(int16_t& rssi, int16_t& percent) {
    bool success = true;

    // Initialize float variable
    int16_t signalQual = -9999;
    percent            = -9999;
    rssi               = -9999;

    // NOTE:  using Google doesn't work because there's no reply
    MS_DBG(F("Opening connection to NIST to check connection strength..."));
    // This is the IP address of time-c-g.nist.gov
    // XBee's address lookup falters on time.nist.gov
    // NOTE:  This "connect" only sets up the connection parameters, the TCP
    // socket isn't actually opened until we first send data (the '!' below)
    IPAddress ip(132, 163, 97, 6);
    gsmClient.connect(ip, 37);
    // Wait so NIST doesn't refuse us!
    while (millis() < _lastNISTrequest + 4000) {
        // wait
    }
    // Need to send something before connection is made
    gsmClient.println('!');
    uint32_t start = millis();
    delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
    while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L) {
        // wait
    }

    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality:"));
    signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    if (gsmClient.connected()) { gsmClient.stop(); }

    // Convert signal quality to RSSI
    rssi    = signalQual;
    percent = getPctFromRSSI(signalQual);

    MS_DBG(F("RSSI:"), rssi);
    MS_DBG(F("Percent signal strength:"), percent);

    return success;
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

    // Initialize variable
    int16_t  rssi    = -9999;
    int16_t  percent = -9999;
    uint16_t volt    = 9999;

    // Try up to 5 times to get a signal quality - that is, ping NIST 5 times
    // and see if the value updates
    int8_t num_pings_remaining = 5;
    do {
        getModemSignalQuality(rssi, percent);
        MS_DBG(F("Raw signal quality:"), rssi);
        if (percent != 0 && percent != -9999) break;
        num_pings_remaining--;
    } while ((percent == 0 || percent == -9999) && num_pings_remaining);

    // Convert signal quality to RSSI
    loggerModem::_priorRSSI          = rssi;
    loggerModem::_priorSignalPercent = percent;


    // Enter command mode only once for temp and battery
    MS_DBG(F("Entering Command Mode:"));
    success &= gsmModem.commandMode();

    MS_DBG(F("Getting input voltage:"));
    volt = gsmModem.getBattVoltage();
    MS_DBG(F("CURRENT Modem input battery voltage:"), volt);
    if (volt != 9999)
        loggerModem::_priorBatteryVoltage = static_cast<float>(volt);
    else
        loggerModem::_priorBatteryVoltage = static_cast<float>(-9999);

    MS_DBG(F("Getting chip temperature:"));
    loggerModem::_priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem temperature:"), loggerModem::_priorModemTemp);

    // Exit command modem
    MS_DBG(F("Leaving Command Mode:"));
    gsmModem.exitCommand();

    return success;
}
