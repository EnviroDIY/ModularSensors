/*
 *IteadWee.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is the Sodaq UBee based on the u-blox SARA R410M LTE-M Cellular Module
*/

// Header Guards
#ifndef ModularSensorsSpecificModem_h
#define ModularSensorsSpecificModem_h

// Debugging Statement
// #define MS_ITEADWEE_DEBUG

#ifdef MS_ITEADWEE_DEBUG
#define MS_DEBUGGING_STD
#define TINY_GSM_DEBUG DEBUGGING_SERIAL_OUTPUT
#endif

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates

// Included Dependencies
#include "ModSensorDebugger.h"
#include "LoggerModem.h"

// ==========================================================================
//    Wifi/Cellular Modem Main Chip Selection
// ==========================================================================

// Select your modem chip - this determines the exact commands sent to it
#define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE


// ==========================================================================
//    Modem Pins
// ==========================================================================

// Declare the existance of modem pin Variables
// Use the "extern" keyword to denote that these will actually be defined in
// the main program rather than in this file.

extern const int8_t modemVccPin;
extern const int8_t modemSleepRqPin;
extern const int8_t modemStatusPin;
extern const int8_t modemResetPin;
extern const int8_t modemLEDPin;
extern Stream  &modemSerial;

extern const int8_t espSleepRqPin;
extern const int8_t espStatusPin;


// ==========================================================================
//    TinyGSM Client
// ==========================================================================

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
TinyGsm *tinyModem = new TinyGsm(modemSerial);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
// const long modemBaud = 115200;       // Communication speed of the modem, 115200 is default for ESP8266

// A helper function to wait for the esp to boot and immediately change some settings
// We'll use this in the wake function
bool ESPwaitForBoot(void)
{
    // Wait for boot - finished when characters start coming
    // NOTE: After every "hard" reset (either power off or via RST-B), the ESP
    // sends out a boot log from the ROM on UART1 at 74880 baud.  We're not
    // going to worry about the odd baud rate since we're simply throwing the
    // characters away.
    delay(200);  // It will take at least this long
    uint32_t start = millis();
    bool success = false;
    while (!modemSerial.available() && millis() - start < 1000) {}
    if (modemSerial.available())
    {
        success = true;
        // Read the boot log to empty it from the serial buffer
        while (modemSerial.available())
        {
            modemSerial.read();
            delay(2);
        }
        // Have to make sure echo is off or all AT commands will be confused
        tinyModem->sendAT(F("E0"));
        success &= tinyModem->waitResponse() == 1;
        // re-run init to set mux and station mode
        success &= tinyModem->init();
    }
    return success;
}

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool modemSleepFxn(void)
{
    // Use this if you have GPIO16 connected to the reset pin to wake from deep sleep
    // but no other MCU pin connected to the reset pin.
    // NOTE:  This will NOT work nicely with "testingMode"
    /*if (loggingInterval > 1)
    {
        uint32_t sleepSeconds = (((uint32_t)loggingInterval) * 60 * 1000) - 75000L;
        String sleepCommand = String(sleepSeconds);
        tinyModem->sendAT(F("+GSLP="), sleepCommand);
        // Power down for 1 minute less than logging interval
        // Better:  Calculate length of loop and power down for logging interval - loop time
        return tinyModem->waitResponse() == 1;
    }*/
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake from deep sleep
    if (modemResetPin >= 0)
    {
        digitalWrite(modemLEDPin, LOW);
        return tinyModem->poweroff();
    }
    // Use this if you don't have access to the ESP8266's reset pin for deep sleep but you
    // do have access to another GPIO pin for light sleep.  This also sets up another
    // pin to view the sleep status.
    else if (modemSleepRqPin >= 0 && modemStatusPin >= 0)
    {
        tinyModem->sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0,"),
                          String(espStatusPin), ',', modemStatusLevel);
        bool success = tinyModem->waitResponse() == 1;
        tinyModem->sendAT(F("+SLEEP=1"));
        success &= tinyModem->waitResponse() == 1;
        digitalWrite(modemLEDPin, LOW);
        return success;
    }
    // Light sleep without the status pin
    else if (modemSleepRqPin >= 0 && modemStatusPin < 0)
    {
        tinyModem->sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0"));
        bool success = tinyModem->waitResponse() == 1;
        tinyModem->sendAT(F("+SLEEP=1"));
        success &= tinyModem->waitResponse() == 1;
        digitalWrite(modemLEDPin, LOW);
        return success;
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}
bool modemWakeFxn(void)
{
    bool success = true;
    if (modemVccPin >= 0)  // Turns on when power is applied
    {
        digitalWrite(modemLEDPin, HIGH);  // Because the ESP8266 doesn't have any lights
        success &= ESPwaitForBoot();
        if (!success)
        {
            digitalWrite(modemLEDPin, LOW);  // Turn off light if the boot failed
        }
        return success;
    }
    else if (modemResetPin >= 0)
    {
        digitalWrite(modemLEDPin, HIGH);
        digitalWrite(modemResetPin, LOW);
        delay(1);
        digitalWrite(modemResetPin, HIGH);
        success &= ESPwaitForBoot();
        if (!success)
        {
            digitalWrite(modemLEDPin, LOW);  // Turn off light if the boot failed
        }
        return success;
    }
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        delay(1);
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(modemLEDPin, HIGH);
        // Don't have to wait for a boot if using light sleep
        return true;
    }
    else
    {
        return true;
    }
}
// Set up the light-sleep status pin, if applicable
void extraModemSetup(void)
{
    // Slow down the baud rate for slow processors - and save the change to
    // the ESP's non-volatile memory so we don't have to do it every time
    // #if F_CPU == 8000000L
    // if (modemBaud > 57600)
    // {
    //     modemSerial.begin(modemBaud);
    //     tinyModem->sendAT(F("+UART_DEF=9600,8,1,0,0"));
    //     tinyModem->waitResponse();
    //     modemSerial.end();
    //     modemSerial.begin(9600);
    // }
    // #endif
    if (modemVccPin < 0 && modemResetPin < 0 && modemSleepRqPin >= 0 && modemStatusPin >= 0)
    {
        tinyModem->sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0,"),
                          String(espStatusPin), ',', modemStatusLevel);
        tinyModem->waitResponse();
    }
}



#endif
