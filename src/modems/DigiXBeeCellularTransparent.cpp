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

// Constructor/Destructor
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


// Destructor
DigiXBeeCellularTransparent::~DigiXBeeCellularTransparent(){}


MS_MODEM_DID_AT_RESPOND(DigiXBeeCellularTransparent);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeCellularTransparent);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(DigiXBeeCellularTransparent);
MS_MODEM_GET_MODEM_SIGNAL_QUALITY(DigiXBeeCellularTransparent);
MS_MODEM_GET_MODEM_BATTERY_NA(DigiXBeeCellularTransparent);
MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(DigiXBeeCellularTransparent);
MS_MODEM_CONNECT_INTERNET(DigiXBeeCellularTransparent);


bool DigiXBeeCellularTransparent::extraModemSetup(void)
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
        gsmModem.sendAT(GF("D8"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        gsmModem.sendAT(GF("D9"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO5/CTS can be used for this function
        gsmModem.sendAT(GF("D7"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on the associate LED (if you're using a board with one)
        // NOTE:  Only pin 15/DIO5 can be used for this function
        gsmModem.sendAT(GF("D5"),1);
        success &= gsmModem.waitResponse() == 1;
        // Turn on the RSSI indicator LED (if you're using a board with one)
        // NOTE:  Only pin 6/DIO10/PWM0 can be used for this function
        gsmModem.sendAT(GF("P0"),1);
        success &= gsmModem.waitResponse() == 1;
        // Put the XBee in pin sleep mode
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"),1);
        success &= gsmModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        gsmModem.sendAT(GF("SO"),0);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Other Options..."));
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        gsmModem.sendAT(GF("DO"),0);
        success &= gsmModem.waitResponse() == 1;
        // Ask data to be "packetized" and sent out with every new line (0x0A
        gsmModem.sendAT(GF("TD0A"));
        success &= gsmModem.waitResponse() == 1;
        // Make sure pins 7&8 are not set for USB direct on XBee3 units
        gsmModem.sendAT(GF("P1"),0);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Cellular Carrier Options..."));
        // Cellular carrier profile - AT&T
        // Hologram says they can use any network, but I've never succeeded with anything but AT&T
        gsmModem.sendAT(GF("CP"),2);
        gsmModem.waitResponse();  // Don't check for success - only works on LTE
        // Cellular network technology - LTE-M Only
        // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
        gsmModem.sendAT(GF("N#"),2);
        gsmModem.waitResponse();  // Don't check for success - only works on LTE
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
        // Force restart the modem, just for good measure
        MS_DBG(F("Restarting XBee..."));
        success &= gsmModem.restart();
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
uint32_t DigiXBeeCellularTransparent::getNISTTime(void)
{
    /* bail if not connected to the internet */
    if (!isInternetAvailable())
    {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    /* Must ensure that we do not ping the daylight more than once every 4 seconds */
    /* NIST clearly specifies here that this is a requirement for all software */
    /* that accesses its servers:  https://tf.nist.gov/tf-cgi/servers.cgi */
    while (millis() < _lastNISTrequest + 4000) {}

    /* Make TCP connection */
    MS_DBG(F("\nConnecting to NIST daytime Server"));
    bool connectionMade = false;

    /* This is the IP address of time-c-g.nist.gov */
    /* XBee's address lookup falters on time.nist.gov */
    IPAddress ip(129, 6, 15, 30);
    connectionMade = gsmClient.connect(ip, 37, 15);

    /* Wait up to 5 seconds for a response */
    if (connectionMade)
    {
        uint32_t start = millis();
        while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L){}


        if (gsmClient.available() >= 4)
        {
            MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
            byte response[4] = {0};
            gsmClient.read(response, 4);
            return parseNISTBytes(response);
        }
        else
        {
            MS_DBG(F("NIST Time server did not respond!"));
            return 0;
        }
    }
    else
    {
        MS_DBG(F("Unable to open TCP to NIST!"));
    }
    return 0;
}


bool DigiXBeeCellularTransparent::addSingleMeasurementResult(void)
{
    bool success = true;

    /* Initialize float variable */
    int16_t signalQual = -9999;
    int16_t percent = -9999;
    int16_t rssi = -9999;
    float temp = -9999;

    /* Check a measurement was *successfully* started (status bit 6 set) */
    /* Only go on to get a result if it was */
    if (bitRead(_sensorStatus, 6))
    {
        // Enter command mode only once
        MS_DBG(F("Entering Command Mode:"));
        gsmModem.commandMode();

        // Get signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no service/signal.
        // The TinyGSM getSignalQuality function returns the same "no signal"
        // value (99 CSQ or 0 RSSI) in all 3 cases.
        MS_DBG(F("Getting signal quality:"));
        signalQual = gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);

        // Convert signal quality to RSSI
        rssi = signalQual;
        percent = getPctFromRSSI(signalQual);

        MS_DBG(F("RSSI:"), rssi);
        MS_DBG(F("Percent signal strength:"), percent);

        MS_DBG(F("Getting chip temperature:"));
        temp = getModemTemperature();
        MS_DBG(F("Modem temperature:"), temp);

        // Exit command modem
        MS_DBG(F("Leaving Command Mode:"));
        gsmModem.exitCommand();
    }
    else
    {
        MS_DBG(getSensorName(), F("is not connected to the network; unable to get signal quality!"));
    }

    verifyAndAddMeasurementResult(MODEM_RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(MODEM_PERCENT_SIGNAL_VAR_NUM, percent);
    verifyAndAddMeasurementResult(MODEM_BATTERY_STATE_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_BATTERY_PERCENT_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_BATTERY_VOLT_VAR_NUM, (float)-9999);
    verifyAndAddMeasurementResult(MODEM_TEMPERATURE_VAR_NUM, temp);

    /* Unset the time stamp for the beginning of this measurement */
    _millisMeasurementRequested = 0;
    /* Unset the status bits for a measurement request (bits 5 & 6) */
    _sensorStatus &= 0b10011111;

    return success;
}
