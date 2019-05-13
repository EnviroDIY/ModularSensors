/*
 *DigiXBeeWifi.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA R410M LTE-M Cellular Module
*/

// Header Guards
#ifndef DigiXBeeWifi_h
#define DigiXBeeWifi_h

// Debugging Statement
// #define MS_DIGIXBEEWIFI_DEBUG

#ifdef MS_DIGIXBEEWIFI_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"

#define TINY_GSM_XBEE_XBEE  // Select for Digi brand WiFi or Cellular XBee's

#define XBEE_STATUS_TIME_MS 15
#define XBEE_DISCONNECT_TIME_MS 5000L

#define XBEE_WARM_UP_TIME_MS 50
#define XBEE_ATRESPONSE_TIME_MS 5000L


class DigiXBeeWifi : public loggerModem
{

public:
    // Constructors
    DigiXBeeWifi(int8_t powerPin, int8_t statusPin, bool useCTSStatus,
                int8_t modemResetPin, int8_t modemSleepRqPin,
                TinyGsmModem *inModem, Client *inClient, const char *ssid, const char *pwd);

    virtual bool setup(void) override;
    virtual bool wake(void) override;
    virtual bool modemSleepPowerDown(void);
}


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

if (modemCTSPin >= 0 && modemStatusPin < 0)
{
    const bool modemStatusLevel = LOW;  // The level of the status pin when the module is active (HIGH or LOW)
}

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
// After enabling pin sleep, the sleep request pin is held LOW to keep the XBee on
// Enable pin sleep in the setup function or using XCTU prior to connecting the XBee
bool modemSleepFxn(void)
{
    if (modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(modemLEDPin, LOW);
        return true;
    }
    else
    {
        return true;
    }
}


bool modemWakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
        return true;
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(modemLEDPin, HIGH);  // Because the XBee doesn't have any lights
        return true;
    }
    else
    {
        return true;
    }
}


void extraModemSetup(void)
{
    tinyModem->init();  // initialize
    if (tinyModem->commandMode())
    {
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        tinyModem->sendAT(F("D8"),1);
        tinyModem->waitResponse();
        // Turn on status indication pin - it will be HIGH when the XBee is awake
        // NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this function
        tinyModem->sendAT(F("D9"),1);
        tinyModem->waitResponse();
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive commands
        // This can be used as proxy for status indication if the true status pin is not accessible
        // NOTE:  Only pin 12/DIO7/CTS can be used for this function
        tinyModem->sendAT(F("D7"),1);
        tinyModem->waitResponse();
        // Put the XBee in pin sleep mode
        tinyModem->sendAT(F("SM"),1);
        tinyModem->waitResponse();
        // Disassociate from network for lowest power deep sleep
        tinyModem->sendAT(F("SO"),200);
        tinyModem->waitResponse();
        // Write changes to flash and apply them
        tinyModem->writeChanges();
        // Exit command mode
        tinyModem->exitCommand();
    }
}



#endif
