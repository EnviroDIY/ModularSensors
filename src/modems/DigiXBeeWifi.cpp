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
#include "modems/LoggerModemMacros.h"


// Constructor/Destructor
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *ssid, const char *pwd,
                           uint8_t measurementsToAverage)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin,
             measurementsToAverage),
    #ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
    gsmModem(_modemATDebugger),
    #else
    gsmModem(*modemStream),
    #endif
    gsmClient(gsmModem)
{
    _ssid = ssid;
    _pwd = pwd;
}


// Destructor
DigiXBeeWifi::~DigiXBeeWifi(){}


MS_MODEM_DID_AT_RESPOND(DigiXBeeWifi);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);
MS_MODEM_VERIFY_MEASUREMENT_COMPLETE(DigiXBeeWifi);
MS_MODEM_GET_MODEM_BATTERY_NA(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_AVAILABLE(DigiXBeeWifi);
MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);
MS_MODEM_GET_NIST_TIME(DigiXBeeWifi);


bool DigiXBeeWifi::extraModemSetup(void)
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
        gsmModem.sendAT(F("SO"),200);
        success &= gsmModem.waitResponse() == 1;
        MS_DBG(F("Setting Wifi Network Options..."));
        // Put the network connection parameters into flash
        success &= gsmModem.networkConnect(_ssid, _pwd);
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


bool DigiXBeeWifi::getModemSignalQuality(int16_t &rssi, int16_t &percent)
{
    // Initialize float variable
    bool success = true;
    int16_t signalQual = -9999;
    percent = -9999;
    rssi = -9999;

    // The WiFi XBee needs to make an actual TCP connection and get some sort
    // of response on that connection before it knows the signal quality.
    // Connecting to the Google DNS servers for now
    MS_DBG(F("Opening connection to check connection strength..."));
    bool usedGoogle = false;
    if (!gsmModem.gotIPforSavedHost())
    {
        usedGoogle = true;
        IPAddress ip(8, 8, 8, 8);  // This is the IP address of time-c-g.nist.gov
        success &= gsmClient.connect(ip, 80);
    }
    gsmClient.print('!');  // Need to send something before connection is made
    delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
    if (usedGoogle)
    {
        gsmClient.stop();
    }

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

    return success;
}


bool DigiXBeeWifi::addSingleMeasurementResult(void)
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
        // The WiFi XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // Connecting to the Google DNS servers for now
        MS_DBG(F("Opening connection to check connection strength..."));
        bool usedGoogle = false;
        if (!gsmModem.gotIPforSavedHost())
        {
            usedGoogle = true;
            IPAddress ip(8, 8, 8, 8);  // This is the IP address of time-c-g.nist.gov
            success &= gsmClient.connect(ip, 80);
        }
        gsmClient.print('!');  // Need to send something before connection is made
        delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
        if (usedGoogle)
        {
            gsmClient.stop();
        }

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
    else MS_DBG(getSensorName(), F("is not connected to the network; unable to get signal quality!"));

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


void DigiXBeeWifi::disconnectInternet(void)
{
    // XBee doesn't like to disconnect at all, so we're doing nothing
}
