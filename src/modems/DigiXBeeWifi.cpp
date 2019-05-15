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


// Constructors
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *ssid, const char *pwd,
                           uint8_t measurementsToAverage)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin,
             measurementsToAverage),
    #ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
    _modemATDebugger(*modemStream, DEBUGGING_SERIAL_OUTPUT),
    _tinyModem(_modemATDebugger)
    #else
    _tinyModem(*modemStream)
    #endif
{
    _ssid = ssid;
    _pwd = pwd;
    TinyGsmClient *tinyClient = new TinyGsmClient(_tinyModem);
    _tinyClient = tinyClient;
    _modemStream = modemStream;
}


MS_MODEM_DID_AT_RESPOND(DigiXBeeWifi);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);
MS_MODEM_IS_MEASUREMENT_COMPLETE(DigiXBeeWifi);
MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi);
MS_MODEM_GET_NIST_TIME(DigiXBeeWifi);


bool DigiXBeeWifi::extraModemSetup(void)
{
    bool success = true;
    success &= _tinyModem.init();  // initialize
    _modemName = _tinyModem.getModemName();
    if (_tinyModem.commandMode())
    {
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        _tinyModem.sendAT(F("D8"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        _tinyModem.sendAT(F("D9"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        _tinyModem.sendAT(F("D7"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Put the XBee in pin sleep mode
        _tinyModem.sendAT(F("SM"),1);
        success &= _tinyModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        _tinyModem.sendAT(F("SO"),200);
        success &= _tinyModem.waitResponse() == 1;
        // Put the network connection parameters into flash
        success &= _tinyModem.networkConnect(_ssid, _pwd);
        // Write changes to flash and apply them
        _tinyModem.writeChanges();
        // Exit command mode
        _tinyModem.exitCommand();
    }
    else success = false;
    return success;
}


bool DigiXBeeWifi::addSingleMeasurementResult(void)
{
    bool success = true;

    // Initialize float variable
    int16_t signalQual = -9999;
    int16_t percent = -9999;
    int16_t rssi = -9999;

    // Check a measurement was *successfully* started (status bit 6 set)
    // Only go on to get a result if it was
    if (bitRead(_sensorStatus, 6))
    {
        // The WiFi XBee needs to make an actual TCP connection and get some sort
        // of response on that connection before it knows the signal quality.
        // Connecting to the Google DNS servers for now
        MS_DBG(F("Opening connection to check connection strength..."));
        bool usedGoogle = false;
        if (!_tinyModem.gotIPforSavedHost())
        {
            usedGoogle = true;
            IPAddress ip(8, 8, 8, 8);  // This is the IP address of time-c-g.nist.gov
            success &= _tinyClient->connect(ip, 80);
        }
        _tinyClient->print('!');  // Need to send something before connection is made
        delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
        if (usedGoogle)
        {
            _tinyClient->stop();
        }

        // Get signal quality
        // NOTE:  We can't actually distinguish between a bad modem response, no
        // modem response, and a real response from the modem of no service/signal.
        // The TinyGSM getSignalQuality function returns the same "no signal"
        // value (99 CSQ or 0 RSSI) in all 3 cases.
        MS_DBG(F("Getting signal quality:"));
        signalQual = _tinyModem.getSignalQuality();
        MS_DBG(F("Raw signal quality:"), signalQual);

        // Convert signal quality to RSSI
        rssi = signalQual;
        percent = getPctFromRSSI(signalQual);

        MS_DBG(F("RSSI:"), rssi);
        MS_DBG(F("Percent signal strength:"), percent);
    }
    else MS_DBG(getSensorName(), F("is not connected to the network; unable to get signal quality!"));

    verifyAndAddMeasurementResult(RSSI_VAR_NUM, rssi);
    verifyAndAddMeasurementResult(PERCENT_SIGNAL_VAR_NUM, percent);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    _sensorStatus &= 0b10011111;

    return success;
}


void DigiXBeeWifi::disconnectInternet(void)
{
    // XBee doesn't like to disconnect at all, so we're doing nothing
}
