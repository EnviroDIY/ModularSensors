/*
 *DigiXBeeWifi.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for Digi S6B Wifi XBee's
*/

// Included DependenciesV
#include "DigiXBeeWifi.h"
#include "LoggerModemMacros.h"

// Constructor/Destructor
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char* ssid, const char* pwd)
    : DigiXBee(powerPin, statusPin, useCTSStatus,
               modemResetPin, modemSleepRqPin),
#ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger, modemResetPin),
#else
      gsmModem(*modemStream, modemResetPin),
#endif
      gsmClient(gsmModem)
{
    _ssid = ssid;
    _pwd = pwd;
}

// Destructor
DigiXBeeWifi::~DigiXBeeWifi() {}

MS_MODEM_SETUP(DigiXBeeWifi);
MS_MODEM_WAKE(DigiXBeeWifi);

MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);

MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeWifi);

bool DigiXBeeWifi::extraModemSetup(void)
{
    bool success = true;
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    MS_DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode())
    {
        MS_DBG(F("Setting I/O Pins..."));
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(GF("D8"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"), 1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
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
        gsmModem.sendAT(GF("SO"), 200);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Wifi Network Options..."));
        // Put the network connection parameters into flash
        success &= gsmModem.networkConnect(_ssid, _pwd);
        // Set the socket timeout to 10s (this is default)
        gsmModem.sendAT(GF("TM"), 64);
        success &= gsmModem.waitResponse() == 1;
        // Write changes to flash and apply them
        gsmModem.writeChanges();
        // Exit command mode
        gsmModem.exitCommand();
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


void DigiXBeeWifi::disconnectInternet(void)
{
    // Wifi XBee doesn't like to disconnect AT ALL, so we're doing nothing
    // If you do disconnect, you must power cycle before you can reconnect
    // to the same access point.
}


// Get the time from NIST via TIME protocol (rfc868)
uint32_t DigiXBeeWifi::getNISTTime(void)
{
    /* bail if not connected to the internet */
    if (!isInternetAvailable())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    gsmClient.stop();

    /* Try up to 12 times to get a timestamp from NIST */
    for (uint8_t i = 0; i < 12; i++)
    {

        /* Must ensure that we do not ping the daylight more than once every 4 seconds */
        /* NIST clearly specifies here that this is a requirement for all software */
        /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
        while (millis() < _lastNISTrequest + 4000)
        {
        }

        /* Make TCP connection */
        MS_DBG(F("\nConnecting to NIST daytime Server"));
        bool connectionMade = false;

        /* This is the IP address of time-e-wwv.nist.gov  */
        /* XBee's address lookup falters on time.nist.gov */
        IPAddress ip(132, 163, 97, 6);
        connectionMade = gsmClient.connect(ip, 37);
        /* Wait again so NIST doesn't refuse us! */
        delay(4000L);
        /* Need to send something before connection is made */
        gsmClient.println('!');
        // delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

        /* Wait up to 5 seconds for a response */
        if (connectionMade)
        {
            uint32_t start = millis();
            while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L)
            {
            }

            if (gsmClient.available() >= 4)
            {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte response[4] = {0};
                gsmClient.read(response, 4);
                gsmClient.stop();
                return parseNISTBytes(response);
            }
            else
            {
                MS_DBG(F("NIST Time server did not respond!"));
                gsmClient.stop();
            }
        }
        else
        {
            MS_DBG(F("Unable to open TCP to NIST!"));
        }
    }
    return 0;
}


bool DigiXBeeWifi::getModemSignalQuality(int16_t& rssi, int16_t& percent)
{
    bool success = true;

    // Initialize float variable
    int16_t signalQual = -9999;
    percent = -9999;
    rssi = -9999;

    // The WiFi XBee needs to make an actual TCP connection and get some sort
    // of response on that connection before it knows the signal quality.
    // Connecting to the Google DNS servers - this doesn't really work
    // MS_DBG(F("Opening connection to check connection strength..."));
    // bool usedGoogle = false;
    // if (!gsmModem.gotIPforSavedHost())
    // {
    //     usedGoogle = true;
    //     IPAddress ip(8, 8, 8, 8);  // This is one of Google's IP's
    //     gsmClient.stop();
    //     success &= gsmClient.connect(ip, 80);
    // }
    // gsmClient.print('!');  // Need to send something before connection is made
    // delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

    MS_DBG(F("Opening connection to NIST to check connection strength..."));
    // This is the IP address of time-c-g.nist.gov
    // XBee's address lookup falters on time.nist.gov
    IPAddress ip(132, 163, 97, 6);
    gsmClient.connect(ip, 37);
    // Wait again so NIST doesn't refuse us!
    delay(4000L);
    // Need to send something before connection is made
    gsmClient.println('!');
    delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.

    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality:"));
    signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    if (gsmClient.connected())
    {
        gsmClient.stop();
    }

    // Convert signal quality to RSSI
    rssi = signalQual;
    percent = getPctFromRSSI(signalQual);

    MS_DBG(F("RSSI:"), rssi);
    MS_DBG(F("Percent signal strength:"), percent);

    return success;
}


bool DigiXBeeWifi::updateModemMetadata(void)
{
    bool success = true;

    // Initialize variables
    int16_t signalQual = -9999;
    uint16_t volt = 9999;

    // Enter command mode only once
    MS_DBG(F("Entering Command Mode:"));
    success &= gsmModem.commandMode();

    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality:"));
    signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    // Convert signal quality to RSSI
    _priorRSSI = signalQual;
    _priorSignalPercent = getPctFromRSSI(signalQual);

    MS_DBG(F("CURRENT RSSI:"), _priorRSSI);
    MS_DBG(F("CURRENT Percent signal strength:"), _priorSignalPercent);

    MS_DBG(F("Getting input voltage:"));
    volt = gsmModem.getBattVoltage();
    MS_DBG(F("CURRENT Modem input battery voltage:"), volt);
    if (volt != 9999)
    {
        _priorBatteryVoltage = (float)volt;
    }
    else
    {
        _priorBatteryVoltage = (float)-9999;
    }

    MS_DBG(F("Getting chip temperature:"));
    _priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem temperature:"), _priorModemTemp);

    // Exit command modem
    MS_DBG(F("Leaving Command Mode:"));
    gsmModem.exitCommand();

    return success;
}
