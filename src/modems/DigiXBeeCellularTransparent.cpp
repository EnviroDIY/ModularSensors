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

// Constructors
DigiXBeeCellularTransparent::DigiXBeeCellularTransparent(Stream* modemStream,
                           int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char *apn,
                           uint8_t measurementsToAverage)
  : DigiXBee(powerPin, statusPin, useCTSStatus,
             modemResetPin, modemSleepRqPin,
             measurementsToAverage),
    _tinyModem(*modemStream, modemResetPin)
{
    _apn = apn;
    TinyGsmClient *tinyClient = new TinyGsmClient(_tinyModem);
    _tinyClient = tinyClient;
    _modemStream = modemStream;
}


MS_MODEM_DID_AT_RESPOND(DigiXBeeCellularTransparent);
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeCellularTransparent);
MS_MODEM_IS_MEASUREMENT_COMPLETE(DigiXBeeCellularTransparent);
MS_MODEM_CONNECT_INTERNET(DigiXBeeCellularTransparent);
MS_MODEM_GET_NIST_TIME(DigiXBeeCellularTransparent);


bool DigiXBeeCellularTransparent::extraModemSetup(void)
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
        _tinyModem.sendAT(F("SO"),0);
        success &= _tinyModem.waitResponse() == 1;
        // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        _tinyModem.sendAT(F("DO"),0);
        success &= _tinyModem.waitResponse() == 1;
        // Make sure USB direct won't be pin enabled on XBee3 units
        _tinyModem.sendAT(F("P0"),0);
        success &= _tinyModem.waitResponse() == 1;
        // Make sure pins 7&8 are not set for USB direct on XBee3 units
        _tinyModem.sendAT(F("P1"),0);
        success &= _tinyModem.waitResponse() == 1;
        // Cellular carrier profile - AT&T
        // Hologram says they can use any network, but I've never succeeded with anything but AT&T
        _tinyModem.sendAT(F("CP"),2);
        success &= _tinyModem.waitResponse() == 1;
        // Cellular network technology - LTE-M Only
        // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
        _tinyModem.sendAT(F("N#"),2);
        success &= _tinyModem.waitResponse() == 1;
        // Put the network connection parameters into flash
        success &= _tinyModem.gprsConnect(_apn);
        // Write changes to flash and apply them
        _tinyModem.writeChanges();
        // Exit command mode
        _tinyModem.exitCommand();
    }
    else success = false;
    return success;
}


bool DigiXBeeCellularTransparent::addSingleMeasurementResult(void)
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


void DigiXBeeCellularTransparent::disconnectInternet(void)
{
    // XBee doesn't like to disconnect at all, so we're doing nothing
}
