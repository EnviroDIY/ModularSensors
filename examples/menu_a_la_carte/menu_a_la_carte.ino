/*****************************************************************************
menu_a_la_carte.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.20.2

This shows most of the standard functions of the library at once.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts


// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The library version this example was written for
const char *libraryVersion = "0.20.2";
// The name of this file
const char *sketchName = "menu_a_la_carte.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
// NOTE:  Daylight savings time will not be applied!  Please use standard time!


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud = 115200;   // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;        // MCU pin for the green LED (-1 if not applicable)
const int8_t redLED = 9;          // MCU pin for the red LED (-1 if not applicable)
const int8_t buttonPin = 21;      // MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = A7;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22;  // MCU pin controlling main sensor power (-1 if not applicable)

// Create the main processor chip "sensor" - for general metadata
const char *mcuBoardVersion = "v0.5b";
ProcessorStats mcuBoard(mcuBoardVersion);

// Create the sample number, battery voltage, and free RAM variable objects for the processor
ProcessorStats_SampleNumber mcuBoardSampNo(&mcuBoard, "mcuBoard-Samp-1234-efgh-1234567890ab");
ProcessorStats_Batt mcuBoardBatt(&mcuBoard, "mcuBoard-Batt-1234-efgh-1234567890ab");
ProcessorStats_FreeRam mcuBoardAvailableRAM(&mcuBoard, "mcuBoard-Avai-1234-efgh-1234567890ab");


// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================

// The modem and a number of sensors communicate over UART/TTL - often called "serial".
// "Hardware" serial ports (automatically controlled by the MCU) are generally
// the most accurate and should be configured and used for as many peripherals
// as possible.  In some cases (ie, modbus communication) many sensors can share
// the same serial port.

#if not defined ARDUINO_ARCH_SAMD && not defined ATMEGA2560  // For AVR boards
// Unfortunately, most AVR boards have only one or two hardware serial ports,
// so we'll set up three types of extra software serial ports to use

// AltSoftSerial by Paul Stoffregen (https://github.com/PaulStoffregen/AltSoftSerial)
// is the most accurate software serial port for AVR boards.
// AltSoftSerial can only be used on one set of pins on each board so only one
// AltSoftSerial port can be used.
// Not all AVR boards are supported by AltSoftSerial.
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;

#if not defined ATMEGA32U4  // NeoSWSerial Doesn't support Leonardo
// NeoSWSerial (https://github.com/SRGDamia1/NeoSWSerial) is the best software
// serial that can be used on any pin supporting interrupts.
// You can use as many instances of NeoSWSerial as you want.
// Not all AVR boards are supported by NeoSWSerial.
#include <NeoSWSerial.h>  // for the stream communication
const int8_t neoSSerial1Rx = 11;     // data in pin
const int8_t neoSSerial1Tx = -1;     // data out pin
NeoSWSerial neoSSerial1(neoSSerial1Rx, neoSSerial1Tx);
// To use NeoSWSerial in this library, we define a function to receive data
// This is just a short-cut for later
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(neoSSerial1Rx)));
}
#endif  // NeoSWSerial

// The "standard" software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts.
// NOTE:  Only use if necessary.  This is not a very accurate serial port!
const int8_t softSerialRx = A3;     // data in pin
const int8_t softSerialTx = A4;     // data out pin

#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts softSerial1(softSerialRx, softSerialTx);
#endif  // End software serial for avr boards


// The SAMD21 has 6 "SERCOM" ports, any of which can be used for UART communication.
// The "core" code for most boards defines one or more UART (Serial) ports with
// the SERCOMs and uses others for I2C and SPI.  We can create new UART ports on
// any available SERCOM.  The table below shows definitions for select boards.

// Board =>   Arduino Zero       Adafruit Feather    Sodaq Boards
// -------    ---------------    ----------------    ----------------
// SERCOM0    Serial1 (D0/D1)    Serial1 (D0/D1)     Serial (D0/D1)
// SERCOM1    Available          Available           Serial3 (D12/D13)
// SERCOM2    Available          Available           I2C (A4/A5)
// SERCOM3    I2C (D20/D21)      I2C (D20/D21)       SPI (D11/12/13)
// SERCOM4    SPI (D21/22/23)    SPI (D21/22/23)     SPI1/Serial2
// SERCOM5    EDBG/Serial        Available           Serial1

// If using a Sodaq board, do not define the new sercoms, instead:
// #define ENABLE_SERIAL2
// #define ENABLE_SERIAL3


#if defined ARDUINO_ARCH_SAMD
#include <wiring_private.h> // Needed for SAMD pinPeripheral() function

#ifndef ENABLE_SERIAL2
// Set up a 'new' UART using SERCOM1
// The Rx will be on digital pin 11, which is SERCOM1's Pad #0
// The Tx will be on digital pin 10, which is SERCOM1's Pad #2
// NOTE:  SERCOM1 is undefinied on a "standard" Arduino Zero and many clones,
//        but not all!  Please check the variant.cpp file for you individual board!
//        Sodaq Autonomo's and Sodaq One's do NOT follow the 'standard' SERCOM definitions!
Uart Serial2(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
// Hand over the interrupts to the sercom port
void SERCOM1_Handler()
{
    Serial2.IrqHandler();
}
#endif

#ifndef ENABLE_SERIAL3
// Set up a 'new' UART using SERCOM2
// The Rx will be on digital pin 5, which is SERCOM2's Pad #3
// The Tx will be on digital pin 2, which is SERCOM2's Pad #2
// NOTE:  SERCOM2 is undefinied on a "standard" Arduino Zero and many clones,
//        but not all!  Please check the variant.cpp file for you individual board!
//        Sodaq Autonomo's and Sodaq One's do NOT follow the 'standard' SERCOM definitions!
Uart Serial3(&sercom2, 5, 2, SERCOM_RX_PAD_3, UART_TX_PAD_2);
// Hand over the interrupts to the sercom port
void SERCOM2_Handler()
{
    Serial3.IrqHandler();
}
#endif

#endif  // End hardware serial on SAMD21 boards


// ==========================================================================
//    Wifi/Cellular Modem Main Chip Selection
// ==========================================================================

// Select your modem chip - this determines the exact commands sent to it
// #define TINY_GSM_MODEM_SIM800  // Select for a SIMCOM SIM800, SIM900, or variant thereof
// #define SIM800_GPRSBEE_R6  // Select with SIM800 for - these use atypical sleep and wake fxns
// #define TINY_GSM_MODEM_SIM808  // Select for a SIMCOM SIM808 or SIM868, or variant thereof
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define USE_UBLOX_R410M  // Select with UBLOX for a non-XBee SARA R4 or N4 model
// #define USE_XBEE_BYPASS  // Select with UBLOX for a Digi 3G or LTE-M XBee in bypass mode
#define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
// #define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's
// #define USE_XBEE_WIFI  // Select with XBEE for an S6B wifi XBee
// #define TINY_GSM_MODEM_M590  // Select for a Neoway M590
// #define TINY_GSM_MODEM_A6  // Select for an AI-Thinker A6, A6C, A7, A20
// #define TINY_GSM_MODEM_M95  // Select for a Quectel M95
// #define TINY_GSM_MODEM_BG96  // Select for a Quectel BG96
// #define TINY_GSM_MODEM_MC60  // Select for a Quectel MC60 or MC60E


// ==========================================================================
//    Modem Pins
// ==========================================================================

const int8_t modemVccPin = -2;      // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const int8_t modemResetPin = A4;    // MCU pin connected to modem reset pin (-1 if unconnected)


// ==========================================================================
//    TinyGSM Client
// ==========================================================================

// #define TINY_GSM_DEBUG Serial  // If you want debugging on the main debug port

#define TINY_GSM_YIELD() { delay(2); }  // Can help with slow (9600) baud rates

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible
// AltSoftSerial &modemSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modemSerial = neoSSerial1;  // For software serial if needed

// Create a TinyGSM modem to run on that serial port
#if defined TINY_GSM_MODEM_XBEE
TinyGsm tinyModem(modemSerial, modemResetPin);
#else
TinyGsm tinyModem(modemSerial);
#endif

// Use this to create a modem if you want to spy on modem communication through
// a secondary Arduino stream.  Make sure you install the StreamDebugger library!
// https://github.com/vshymanskyy/StreamDebugger
// Also make sure you comment out the modem creation above to use this.
// #include <StreamDebugger.h>
// StreamDebugger modemDebugger(modemSerial, Serial);
// TinyGsm tinyModem(modemDebugger);

// Create a TCP client on that modem
TinyGsmClient tinyClient(tinyModem);
// The u-blox SARA R410 is very slow to open and close clients, so we can
// iterate through mutiple data senders much more quickly if we have multiple
// clients.  The u-blox SARA R410 is the only modem where there's any advantage
// to this.
#if defined USE_UBLOX_R410M
TinyGsmClient tinyClient2(tinyModem);
TinyGsmClient tinyClient3(tinyModem);
#endif


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

// This should apply to all Digi brand XBee modules.
#if defined TINY_GSM_MODEM_XBEE || defined USE_XBEE_BYPASS
// Describe the physical pin connection of your modem to your board
const long modemBaud = 9600;        // Communication speed of the modem, 9600 is default for XBee
const bool modemStatusLevel = LOW;  // The level of the status pin when the module is active (HIGH or LOW)

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
// After enabling pin sleep, the sleep request pin is held LOW to keep the XBee on
// Enable pin sleep in the setup function or using XCTU prior to connecting the XBee
bool modemSleepFxn(void)
{
    if (modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
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
        digitalWrite(redLED, HIGH);  // Because the XBee doesn't have any lights
        return true;
    }
    else
    {
        return true;
    }
}
// An extra function to set up pin sleep and other preferences on the XBee
// NOTE:  This will only succeed if the modem is turned on and awake!
#if defined TINY_GSM_MODEM_XBEE
void extraModemSetup(void)
{
    tinyModem.init();  // initialize
    if (tinyModem.commandMode())
    {
        tinyModem.sendAT(F("SM"),1);  // Pin sleep
        tinyModem.waitResponse();
        tinyModem.sendAT(F("DO"),0);  // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        tinyModem.waitResponse();
        #if defined USE_XBEE_WIFI
        tinyModem.sendAT(F("SO"),200);  // For WiFi - Disassociate from AP for Deep Sleep
        tinyModem.waitResponse();
        #else
        tinyModem.sendAT(F("SO"),0);  // For Cellular - disconnected sleep
        tinyModem.waitResponse();
        tinyModem.sendAT(F("N#"),2);  // Cellular network technology - LTE-M Only
        // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
        #endif
        tinyModem.waitResponse();
        tinyModem.writeChanges();
        tinyModem.exitCommand();
    }
}
#elif defined USE_XBEE_BYPASS
void extraModemSetup(void)
{
    delay(1000);  // Guard time for command mode
    tinyModem.streamWrite(GF("+++"));  // enter command mode
    tinyModem.waitResponse(2000, F("OK\r"));
    tinyModem.sendAT(F("SM"),1);  // Pin sleep
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.sendAT(F("DO"),0);  // Disable remote manager, USB Direct, and LTE PSM
    // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
    // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
    // so we'll use the Digi pin sleep instead.
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.sendAT(F("SO"),0);  // For Cellular - disconnected sleep
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.sendAT(F("N#"),2);  // Cellular network technology - LTE-M Only
    // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.sendAT(F("AP5"));  // Turn on bypass mode
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.sendAT(F("WR"));  // Write changes to flash
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.sendAT(F("AC"));  // Apply changes
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.sendAT(F("FR"));  // Force reset to enter bypass mode
    tinyModem.waitResponse(F("OK\r"));
    tinyModem.init();  // initialize
}
#endif


// This should work with most ESP8266 breakouts
#elif defined TINY_GSM_MODEM_ESP8266
// Describe the physical pin connection of your modem to your board
const long modemBaud = 115200;       // Communication speed of the modem, 115200 is default for ESP8266
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
const int8_t espSleepRqPin = 14;     // ESP8266 GPIO pin used for wake from light sleep (-1 if not applicable)
const int8_t espStatusPin = 13;      // ESP8266 GPIO pin used to give modem status (-1 if not applicable)

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
        tinyModem.sendAT(F("+GSLP="), sleepCommand);
        // Power down for 1 minute less than logging interval
        // Better:  Calculate length of loop and power down for logging interval - loop time
        return tinyModem.waitResponse() == 1;
    }*/
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake from deep sleep
    if (modemResetPin >= 0)
    {
        digitalWrite(redLED, LOW);
        return tinyModem.poweroff();
    }
    // Use this if you don't have access to the ESP8266's reset pin for deep sleep but you
    // do have access to another GPIO pin for light sleep.  This also sets up another
    // pin to view the sleep status.
    else if (modemSleepRqPin >= 0 && modemStatusPin >= 0)
    {
        tinyModem.sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0,"),
                          String(espStatusPin), ',', modemStatusLevel);
        bool success = tinyModem.waitResponse() == 1;
        tinyModem.sendAT(F("+SLEEP=1"));
        success &= tinyModem.waitResponse() == 1;
        digitalWrite(redLED, LOW);
        return success;
    }
    // Light sleep without the status pin
    else if (modemSleepRqPin >= 0 && modemStatusPin < 0)
    {
        tinyModem.sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0"));
        bool success = tinyModem.waitResponse() == 1;
        tinyModem.sendAT(F("+SLEEP=1"));
        success &= tinyModem.waitResponse() == 1;
        digitalWrite(redLED, LOW);
        return success;
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}
bool modemWakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
    {
        digitalWrite(redLED, HIGH);  // Because the ESP8266 doesn't have any lights
        // Wait for boot - finished when characters start coming
        delay(200);  // It will take at least this long
        uint32_t start = millis();
        bool success = false;
        while (!modemSerial.available() && millis() - start < 1000) {}
        if (modemSerial.available())
        {
            success = true;
            // Clear the junk the ESP sends out after boot-up
            while (modemSerial.available())
            {
                modemSerial.read();
                delay(2);
            }
            // Have to make sure echo is off or all AT commands will be confused
            tinyModem.sendAT(F("E0"));
            success &= tinyModem.waitResponse() == 1;
            // Slow down the baud rate for slow processors
            #if F_CPU == 8000000L
            if (modemBaud > 57600)
            {
                tinyModem.setBaud(9600);
                modemSerial.end();
                modemSerial.begin(9600);
            }
            #endif
            // re-run init to set mux and station mode
            success &= tinyModem.init();
        }
        if (!success)
        {
            digitalWrite(redLED, LOW);  // Turn off light if the boot failed
        }
        return success;
    }
    else if (modemResetPin >= 0)
    {
        digitalWrite(redLED, HIGH);
        digitalWrite(modemResetPin, LOW);
        delay(1);
        digitalWrite(modemResetPin, HIGH);

        // Wait for boot - finished when characters start coming
        delay(200);  // It will take at least this long
        uint32_t start = millis();
        bool success = false;
        while (!modemSerial.available() && millis() - start < 1000) {}
        if (modemSerial.available())
        {
            success = true;
            // Clear the junk the ESP sends out after boot-up
            while (modemSerial.available())
            {
                modemSerial.read();
                delay(2);
            }
            // Have to make sure echo is off or all AT commands will be confused
            tinyModem.sendAT(F("E0"));
            success &= tinyModem.waitResponse() == 1;
            // Slow down the baud rate for slow processors
            #if F_CPU == 8000000L
            if (modemBaud > 57600)
            {
                tinyModem.setBaud(9600);
                modemSerial.end();
                modemSerial.begin(9600);
            }
            #endif
            // re-run init to set mux and station mode
            success &= tinyModem.init();
        }
        if (!success)
        {
            digitalWrite(redLED, LOW);  // Turn off light if the boot failed
        }
        return success;
    }
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        delay(1);
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, HIGH);
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
    if (modemVccPin < 0 && modemResetPin < 0 && modemSleepRqPin >= 0 && modemStatusPin >= 0)
    {
        tinyModem.sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0,"),
                          String(espStatusPin), ',', modemStatusLevel);
        tinyModem.waitResponse();
    }
}


// This should work for many u-blox breakouts, but check the timing in wake/sleep functions
#elif defined TINY_GSM_MODEM_UBLOX
// Describe the physical pin connection of your modem to your board
#if defined USE_UBLOX_R410M
const long modemBaud = 115200;       // Communication speed of the modem, R4/N4 use 115200
#else
const long modemBaud = 9600;         // Communication speed of the modem, most u-blox use 9600
#endif
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool modemSleepFxn(void)
{
    #if defined USE_UBLOX_R410M
    if (modemSleepRqPin >= 0)  // R410 must have access to PWR_ON pin to sleep
    #else
    if (modemVccPin >= 0 || modemSleepRqPin >= 0)  // others will go on with power on
    #endif
    {
        // Easiest to just go to sleep with the AT command rather than using pins
        return tinyModem.poweroff();
    }
    else  // DON'T go to sleep if we can't wake up!
    {
        return true;
    }
}
bool modemWakeFxn(void)
{
    // SARA/LISA U2/G2 and SARA G3 series turn on when power is applied
    // SARA R4/N4 series must power on and then pulse on
    #ifndef USE_UBLOX_R410M
    if (modemVccPin >= 0)
        return true;
    #endif
    if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);
        #if defined USE_UBLOX_R410M
        delay(200);  // 0.15-3.2s pulse for wake on SARA R4/N4
        #else
        // delay(6);  // >5ms pulse for wake on SARA G3
        delayMicroseconds(65);  // 50-80µs pulse for wake on SARA/LISA U2/G2
        #endif
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
        // Need to slow down R4/N4's default 115200 baud rate for slow processors
        #if F_CPU == 8000000L && defined USE_UBLOX_R410M
        delay(4600);  // Must wait for UART port to become active
        modemSerial.begin(115200);
        tinyModem.setBaud(9600);
        modemSerial.end();
        modemSerial.begin(9600);
        #endif
        return true;
    }
    else
    {
        return true;
    }
}
#if defined USE_UBLOX_R410M
// Set band to only use LTE-M (not NB IoT) - this can speed up connection
void extraModemSetup(void)
{
    tinyModem.sendAT(F("+URAT=7"));
    tinyModem.waitResponse();
}
#else
void extraModemSetup(void){}
#endif

// THIS ONLY APPLIES TO A SODAQ GPRSBEE R6!!!
#elif defined TINY_GSM_MODEM_SIM800 && defined SIM800_GPRSBEE_R6
// Describe the physical pin connection of your modem to your board
const long modemBaud = 9600;         // Communication speed of the modem
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool modemWakeFxn(void)
{
    digitalWrite(modemSleepRqPin, HIGH);
    digitalWrite(redLED, HIGH);  // A light just for show
    return true;
}
bool modemSleepFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    digitalWrite(redLED, LOW);
    return true;
}
void extraModemSetup(void){}

// Most cellular chips/breakouts respond to a low pulse of some length to power up and down
#else
// Describe the physical pin connection of your modem to your board
const long modemBaud = 9600;         // Communication speed of the modem
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)

// Create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool modemWakeFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    // delay(1100);  // >1s for SIM800, SIM900, Quectel M95, Quectel MC60
    // delay(510);  // >300ms (>500ms recommended) for Neoway M590
    delay(110);  // >100ms for Quectel BG96
    digitalWrite(modemSleepRqPin, HIGH);
    return true;
}
bool modemSleepFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    // delay(1100);  // 1sec > t > 33sec for SIM800 and clones
    // delay(600);  // 0.5sec > pull down > 1sec for SIM900
    // delay(510);  // >300ms (>500ms recommended) for Neoway M590
    delay(700);  // >650ms for Quectel BG96, 0.6sec > pull down > 1sec for  Quectel M95, Quectel MC60
    digitalWrite(modemSleepRqPin, HIGH);
    return true;
}
void extraModemSetup(void){}
#endif


// ==========================================================================
//    Network Information and LoggerModem Object
// ==========================================================================
#include <LoggerModem.h>

// Network connection information
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a Client, and functions for wake and sleep
#if defined TINY_GSM_MODEM_ESP8266 || defined USE_XBEE_WIFI
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, modemWakeFxn, modemSleepFxn, &tinyModem, &tinyClient, wifiId, wifiPwd);
// ^^ Use this for WiFi
#else
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, modemWakeFxn, modemSleepFxn, &tinyModem, &tinyClient, apn);
// ^^ Use this for cellular
#endif

// Create the RSSI and signal strength variable objects for the modem
Modem_RSSI modemRSSI(&modem, "modemRSS-Ixxx-1234-efgh-1234567890ab");
Modem_SignalPercent modemSignalPct(&modem, "modemSig-nalP-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create the DS3231 sensor object
MaximDS3231 ds3231(1);

// Create the temperature variable object for the DS3231
MaximDS3231_Temp ds3231Temp(&ds3231, "ds3231Te-mpxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-CO2 Embedded NDIR Carbon Dioxide Sensor
// ==========================================================================
#include <sensors/AtlasScientificCO2.h>

const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasCO2i2c_addr = 0x69;  // Default for CO2-EZO is 0x69 (105)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create the Atlas Scientific CO2 sensor object
// AtlasScientificCO2 atlasCO2(I2CPower, AtlasCO2i2c_addr);
AtlasScientificCO2 atlasCO2(I2CPower);

// Create the concentration and temperature variable objects for the EZO-CO2
AtlasScientificCO2_CO2 atlasCO2CO2(&atlasCO2, "atlasCO2-CO2x-1234-efgh-1234567890ab");
AtlasScientificCO2_Temp atlasCO2Temp(&atlasCO2, "atlasCO2-Temp-1234-efgh-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-DO Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/AtlasScientificDO.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasDOi2c_addr = 0x61;  // Default for DO is 0x61 (97)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create the Atlas Scientific DO sensor object
// AtlasScientificDO atlasDO(I2CPower, AtlasDOi2c_addr);
AtlasScientificDO atlasDO(I2CPower);

// Create the concentration and percent saturation variable objects for the EZO-DO
AtlasScientificDO_DOmgL atlasDOconc(&atlasDO, "atlasDOc-oncx-1234-efgh-1234567890ab");
AtlasScientificDO_DOpct atlasDOpct(&atlasDO, "atlasDOp-ctxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-EC Conductivity Sensor
// ==========================================================================
#include <sensors/AtlasScientificEC.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasECi2c_addr = 0x64;  // Default for EC is 0x64 (100)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create the Atlas Scientific Conductivity sensor object
// AtlasScientificEC atlasEC(I2CPower, AtlasECi2c_addr);
AtlasScientificEC atlasEC(I2CPower);

// Create the four variable objects for the EZO-ES
AtlasScientificEC_Cond atlasCond(&atlasEC, "atlasCon-dxxx-1234-efgh-1234567890ab");
AtlasScientificEC_TDS atlasTDS(&atlasEC, "atlasTDS-xxxx-1234-efgh-1234567890ab");
AtlasScientificEC_Salinity atlasSal(&atlasEC, "atlasSal-xxxx-1234-efgh-1234567890ab");
AtlasScientificEC_SpecificGravity atlasGrav(&atlasEC, "atlasGra-vxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-ORP Oxidation/Reduction Potential Sensor
// ==========================================================================
#include <sensors/AtlasScientificORP.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasORPi2c_addr = 0x62;  // Default for ORP is 0x62 (98)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create the Atlas Scientific ORP sensor object
// AtlasScientificORP atlasORP(I2CPower, AtlasORPi2c_addr);
AtlasScientificORP atlasORP(I2CPower);

// Create the potential variable object for the ORP
AtlasScientificORP_Potential atlasORPot(&atlasORP, "atlasORP-otxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-pH Sensor
// ==========================================================================
#include <sensors/AtlasScientificpH.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlaspHi2c_addr = 0x63;  // Default for RTD is 0x63 (99)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create the Atlas Scientific pH sensor object
// AtlasScientificpH atlaspH(I2CPower, AtlaspHi2c_addr);
AtlasScientificpH atlaspH(I2CPower);

// Create the pH variable object for the pH sensor
AtlasScientificpH_pH atlaspHpH(&atlaspH, "atlaspHp-Hxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-RTD Temperature Sensor
// ==========================================================================
#include <sensors/AtlasScientificRTD.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasRTDi2c_addr = 0x66;  // Default for RTD is 0x66 (102)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create the Atlas Scientific RTD sensor object
// AtlasScientificRTD atlasRTD(I2CPower, AtlasRTDi2c_addr);
AtlasScientificRTD atlasRTD(I2CPower);

// Create the temperature variable object for the RTD
AtlasScientificRTD_Temp atlasTemp(&atlasRTD, "atlasTem-pxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)

// Create the AOSong AM2315 sensor object
AOSongAM2315 am2315(I2CPower);

// Create the humidity and temperature variable objects for the AM2315
AOSongAM2315_Humidity am2315Humid(&am2315, "am2315Hu-midx-1234-efgh-1234567890ab");
AOSongAM2315_Temp am2315Temp(&am2315, "am2315Te-mpxx-1234-efgh-1234567890ab");


// ==========================================================================
//    AOSong DHT 11/21 (AM2301)/22 (AM2302) Digital Humidity and Temperature
// ==========================================================================
#include <sensors/AOSongDHT.h>

const int8_t DHTPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t DHTPin = 10;  // DHT data pin
DHTtype dhtType = DHT11;  // DHT type, either DHT11, DHT21, or DHT22

// Create the AOSong DHT sensor object
AOSongDHT dht(DHTPower, DHTPin, dhtType);

// Create the humidity, temperature and heat index variable objects for the DHT
AOSongDHT_Humidity dhtHumid(&dht, "dhtHumid-xxxx-1234-efgh-1234567890ab");
AOSongDHT_Temp dhtTemp(&dht, "dhtTempx-xxxx-1234-efgh-1234567890ab");
AOSongDHT_HI dhtHI(&dht, "dhtHIxxx-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Apogee SQ-212 Photosynthetically Active Radiation (PAR) Sensor
// ==========================================================================
#include <sensors/ApogeeSQ212.h>

const int8_t SQ212Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SQ212ADSChannel = 3;  // The ADS channel for the SQ212
const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC

// Create the Apogee SQ212 sensor object
ApogeeSQ212 SQ212(SQ212Power, SQ212ADSChannel);

// Create the PAR variable object for the SQ212
ApogeeSQ212_PAR sq212PAR(&SQ212, "sq212PAR-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <sensors/BoschBME280.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t BMEi2c_addr = 0x76;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address

// Create the Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);

// Create the four variable objects for the BME280
BoschBME280_Humidity bme280Humid(&bme280, "bme280Hu-midx-1234-efgh-1234567890ab");
BoschBME280_Temp bme280Temp(&bme280, "bme280Te-mpxx-1234-efgh-1234567890ab");
BoschBME280_Pressure bme280Press(&bme280, "bme280Pr-essx-1234-efgh-1234567890ab");
BoschBME280_Altitude bme280Alt(&bme280, "bme280Al-txxx-1234-efgh-1234567890ab");


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>

const int8_t OBS3Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3numberReadings = 10;
// const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowADSChannel = 0;  // The ADS channel for the low range output
const float OBSLow_A = 0.000E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 1.000E+00;  // The "B" value (X) from the low range calibration
const float OBSLow_C = 0.000E+00;  // The "C" value from the low range calibration

// Create the Campbell OBS3+ LOW RANGE sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowADSChannel, OBSLow_A, OBSLow_B, OBSLow_C, ADSi2c_addr, OBS3numberReadings);

// Create the turbidity and voltage variable objects for the low range of the OBS3
CampbellOBS3_Turbidity obs3TurbLow(&osb3low, "obs3Turb-Lowx-1234-efgh-1234567890ab", "TurbLow");
CampbellOBS3_Voltage obs3VoltLow(&osb3low, "obs3Volt-Lowx-1234-efgh-1234567890ab", "TurbLowV");


// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighADSChannel = 1;  // The ADS channel for the high range output
const float OBSHigh_A = 0.000E+00;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 1.000E+00;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = 0.000E+00;  // The "C" value from the high range calibration

// Create the Campbell OBS3+ HIGH RANGE sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighADSChannel, OBSHigh_A, OBSHigh_B, OBSHigh_C, ADSi2c_addr, OBS3numberReadings);

// Create the turbidity and voltage variable objects for the high range of the OBS3
CampbellOBS3_Turbidity obs3TurbHigh(&osb3high, "obs3Turb-High-1234-efgh-1234567890ab", "TurbHigh");
CampbellOBS3_Voltage obs3VoltHigh(&osb3high, "obs3Volt-High-1234-efgh-1234567890ab", "TurbHighV");


// ==========================================================================
//    Decagon 5TM Soil Moisture Sensor
// ==========================================================================
#include <sensors/Decagon5TM.h>

const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create the Decagon 5TM sensor object
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);

// Create the matric potential, volumetric water content, and temperature
// variable objects for the 5TM
Decagon5TM_Ea fivetmEa(&fivetm, "fivetmEa-xxxx-1234-efgh-1234567890ab");
Decagon5TM_VWC fivetmVWC(&fivetm, "fivetmVW-Cxxx-1234-efgh-1234567890ab");
Decagon5TM_Temp fivetmTemp(&fivetm, "fivetmTe-mpxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>

const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDnumberReadings = 6;  // The number of readings to average
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create the Decagon CTD sensor object
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, CTDnumberReadings);

// Create the conductivity, temperature, and depth variable objects for the CTD
DecagonCTD_Cond ctdCond(&ctd, "ctdCondx-xxxx-1234-efgh-1234567890ab");
DecagonCTD_Temp ctdTemp(&ctd, "ctdTempx-xxxx-1234-efgh-1234567890ab");
DecagonCTD_Depth ctdDepth(&ctd, "ctdDepth-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Decagon ES2 Conductivity and Temperature Sensor
// ==========================================================================
#include <sensors/DecagonES2.h>

const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin
const uint8_t ES2NumberReadings = 3;

// Create the Decagon ES2 sensor object
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data, ES2NumberReadings);

// Create the conductivity and temperature variable objects for the ES2
DecagonES2_Cond es2Cond(&es2, "es2Condx-xxxx-1234-efgh-1234567890ab");
DecagonES2_Temp es2Temp(&es2, "es2Tempx-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    External Voltage via TI ADS1115
// ==========================================================================
#include <sensors/ExternalVoltage.h>

const int8_t ADSPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t ADSChannel = 2;  // The ADS channel of interest
const float dividerGain = 10;  //  Default 1/gain for grove voltage divider is 10x
// const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC
const uint8_t VoltReadsToAvg = 1;  // Only read one sample

// Create the External Voltage sensor object
ExternalVoltage extvolt(ADSPower, ADSChannel, dividerGain, ADSi2c_addr, VoltReadsToAvg);

// Create the voltage variable object
ExternalVoltage_Volt extvoltV(&extvolt, "extvoltV-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Freescale Semiconductor MPL115A2 Barometer
// ==========================================================================
#include <sensors/FreescaleMPL115A2.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MPL115A2ReadingsToAvg = 1;

// Create the MPL115A2 barometer sensor object
MPL115A2 mpl115a2(I2CPower, MPL115A2ReadingsToAvg);

// Create the pressure and temperature variable objects for the MPL
MPL115A2_Pressure mplPress(&mpl115a2, "mplPress-xxxx-1234-efgh-1234567890ab");
MPL115A2_Temp mplTemp(&mpl115a2, "mplTempx-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxbotix HRXL Ultrasonic Range Finder
// ==========================================================================
#include <sensors/MaxBotixSonar.h>

// Create a reference to the serial port for the sonar
// A Maxbotix sonar with the trigger pin disconnect CANNOT share the serial port
// A Maxbotix sonar using the trigger may be able to share but YMMV
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
#if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
HardwareSerial &sonarSerial = Serial3;  // Use hardware serial if possible
#else
// AltSoftSerial &sonarSerial = altSoftSerial;  // For software serial if needed
NeoSWSerial &sonarSerial = neoSSerial1;  // For software serial if needed
// SoftwareSerial_ExtInts &sonarSerial = softSerial1;  // For software serial if needed
#endif

const int8_t SonarPower = sensorPowerPin;  // Excite (power) pin (-1 if unconnected)
const int8_t Sonar1Trigger = A1;  // Trigger pin (a unique negative number if unconnected) (D25 = A1)

// Create the MaxBotix Sonar sensor object
MaxBotixSonar sonar1(sonarSerial, SonarPower, Sonar1Trigger) ;

// Create the ultrasonic range variable object
MaxBotixSonar_Range sonar1Range(&sonar1, "sonar1Ra-ngex-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <sensors/MaximDS18.h>

// OneWire Address [array of 8 hex characters]
// If only using a single sensor on the OneWire bus, you may omit the address
DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
const int8_t OneWirePower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t OneWireBus = A0;  // Pin attached to the OneWire Bus (-1 if unconnected) (D24 = A0)

// Create the Maxim DS18 sensor objects (use this form for a known address)
MaximDS18 ds18(OneWireAddress1, OneWirePower, OneWireBus);

// Create the Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
// MaximDS18 ds18(OneWirePower, OneWireBus);

// Create the temperature variable object for the DS18
MaximDS18_Temp ds18Temp(&ds18, "ds18Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <sensors/MeaSpecMS5803.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 (default) or 0x77
const int16_t MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;

// Create the MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);

// Create the conductivity and temperature variable objects for the MS5803
MeaSpecMS5803_Pressure ms5803Press(&ms5803, "ms5803Pr-essx-1234-efgh-1234567890ab");
MeaSpecMS5803_Temp ms5803Temp(&ms5803, "ms5803Te-mpxx-1234-efgh-1234567890ab");


// ==========================================================================
//    External I2C Rain Tipping Bucket Counter
// ==========================================================================
#include <sensors/RainCounterI2C.h>

const uint8_t RainCounterI2CAddress = 0x08;  // I2C Address for external tip counter
const float depthPerTipEvent = 0.2;  // rain depth in mm per tip event

// Create the Rain Counter sensor object
RainCounterI2C tbi2c(RainCounterI2CAddress, depthPerTipEvent);

// Create the number of tips and rain depth variable objects for the tipping bucket
RainCounterI2C_Tips tbi2cTips(&tbi2c, "tbi2cTip-sxxx-1234-efgh-1234567890ab");
RainCounterI2C_Depth tbi2cDepth(&tbi2c, "tbi2cDep-thxx-1234-efgh-1234567890ab");


// ==========================================================================
//    TI INA219 High Side Current/Voltage Sensor (Current mA, Voltage, Power)
// ==========================================================================
#include <sensors/TIINA219.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t INA219i2c_addr = 0x40;  // 1000000 (Board A0+A1=GND)
// The INA219 can have one of 16 addresses, depending on the connections of A0 and A1
const uint8_t INA219ReadingsToAvg = 1;

// Create the INA219 sensor object
TIINA219 ina219(I2CPower, INA219i2c_addr, INA219ReadingsToAvg);

// Create the current, voltage, and power variable objects for the INA219
TIINA219_Current inaCurrent(&ina219, "inaCurre-ntxx-1234-efgh-1234567890ab");
TIINA219_Volt inaVolt(&ina219, "inaVoltx-xxxx-1234-efgh-1234567890ab");
TIINA219_Power inaPower(&ina219, "inaPower-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Keller Acculevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#include <sensors/KellerAcculevel.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
#if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
#else
AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
#endif

byte acculevelModbusAddress = 0x01;  // The modbus address of KellerAcculevel
const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t acculevelNumberReadings = 5;  // The manufacturer recommends taking and averaging a few readings

// Create the Keller Acculevel sensor object
KellerAcculevel acculevel(acculevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, acculevelNumberReadings);

// Create the pressure, temperature, and height variable objects for the Acculevel
KellerAcculevel_Pressure acculevPress(&acculevel, "acculevP-ress-1234-efgh-1234567890ab");
KellerAcculevel_Temp acculevTemp(&acculevel, "acculevT-empx-1234-efgh-1234567890ab");
KellerAcculevel_Height acculevHeight(&acculevel, "acculevH-eigh-1234-efgh-1234567890ab");


// ==========================================================================
//    Keller Nanolevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#include <sensors/KellerNanolevel.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte nanolevelModbusAddress = 0x01;  // The modbus address of KellerNanolevel
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t nanolevelNumberReadings = 3;  // The manufacturer recommends taking and averaging a few readings

// Create the Keller Nanolevel sensor object
KellerNanolevel nanolevel(nanolevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, nanolevelNumberReadings);

// Create the pressure, temperature, and height variable objects for the Nanolevel
KellerNanolevel_Pressure nanolevPress(&nanolevel, "nanolevP-ress-1234-efgh-1234567890ab");
KellerNanolevel_Temp nanolevTemp(&nanolevel, "nanolevT-empx-1234-efgh-1234567890ab");
KellerNanolevel_Height nanolevHeight(&nanolevel, "nanolevH-eigh-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/YosemitechY504.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y504ModbusAddress = 0x04;  // The modbus address of the Y504
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y504NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create the Yosemitech Y504 dissolved oxygen sensor object
YosemitechY504 y504(y504ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y504NumberReadings);

// Create the dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable objects for the Y504
YosemitechY504_DOpct y504DOpct(&y504, "y504DOpc-txxx-1234-efgh-1234567890ab");
YosemitechY504_DOmgL y504DOmgL(&y504, "y504DOmg-Lxxx-1234-efgh-1234567890ab");
YosemitechY504_Temp y504Temp(&y504, "y504Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y510 Turbidity Sensor
// ==========================================================================
#include <sensors/YosemitechY510.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y510ModbusAddress = 0x0B;  // The modbus address of the Y510
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y510NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create the Y510-B Turbidity sensor object
YosemitechY510 y510(y510ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y510NumberReadings);

// Create the turbidity and temperature variable objects for the Y510
YosemitechY510_Turbidity y510Turb(&y510, "y510Turb-xxxx-1234-efgh-1234567890ab");
YosemitechY510_Temp y510Temp(&y510, "y510Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y511 Turbidity Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY511.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y511ModbusAddress = 0x1A;  // The modbus address of the Y511
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y511NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create the Y511-A Turbidity sensor object
YosemitechY511 y511(y511ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y511NumberReadings);

// Create the turbidity and temperature variable objects for the Y511
YosemitechY511_Turbidity y511Turb(&y511, "y511Turb-xxxx-1234-efgh-1234567890ab");
YosemitechY511_Temp y511Temp(&y511, "y511Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y514 Chlorophyll Sensor
// ==========================================================================
#include <sensors/YosemitechY514.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y514ModbusAddress = 0x14;  // The modbus address of the Y514
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y514NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create the Y514 chlorophyll sensor object
YosemitechY514 y514(y514ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y514NumberReadings);

// Create the chlorophyll concentration and temperature variable objects for the Y514
YosemitechY514_Chlorophyll y514Chloro(&y514, "y514Chlo-roxx-1234-efgh-1234567890ab");
YosemitechY514_Temp y514Temp(&y514, "y514Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y520 Conductivity Sensor
// ==========================================================================
#include <sensors/YosemitechY520.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y520ModbusAddress = 0x20;  // The modbus address of the Y520
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y520NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create the Y520 conductivity sensor object
YosemitechY520 y520(y520ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y520NumberReadings);

// Create the specific conductance and temperature variable objects for the Y520
YosemitechY520_Cond y520Cond(&y520, "y520Cond-xxxx-1234-efgh-1234567890ab");
YosemitechY520_Temp y520Temp(&y520, "y520Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y532 pH
// ==========================================================================
#include <sensors/YosemitechY532.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y532ModbusAddress = 0x32;  // The modbus address of the Y532
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y532NumberReadings = 1;  // The manufacturer actually doesn't mention averaging for this one

// Create the Yosemitech Y532 pH sensor object
YosemitechY532 y532(y532ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y532NumberReadings);

// Create the pH, electrical potential, and temperature variable objects for the Y532
YosemitechY532_Voltage y532Voltage(&y532, "y532Volt-agex-1234-efgh-1234567890ab");
YosemitechY532_pH y532pH(&y532, "y532pHxx-xxxx-1234-efgh-1234567890ab");
YosemitechY532_Temp y532Temp(&y532, "y532Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y550 COD Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY550.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y550ModbusAddress = 0x50;  // The modbus address of the Y550
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y550NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create the Y550 conductivity sensor object
YosemitechY550 y550(y550ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y550NumberReadings);

// Create the COD, turbidity, and temperature variable objects for the Y550
YosemitechY550_COD y550COD(&y550, "y550CODx-xxxx-1234-efgh-1234567890ab");
YosemitechY550_Turbidity y550Turbid(&y550, "y550Turb-idxx-1234-efgh-1234567890ab");
YosemitechY550_Temp y550Temp(&y550, "y550Temp-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y4000 Multiparameter Sonde (DOmgL, Turbidity, Cond, pH, Temp, ORP, Chlorophyll, BGA)
// ==========================================================================
#include <sensors/YosemitechY4000.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined ARDUINO_ARCH_SAMD || defined ATMEGA2560
// HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
// #else
// AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// // NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
// #endif

byte y4000ModbusAddress = 0x05;  // The modbus address of the Y4000
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y4000NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption

// Create the Yosemitech Y4000 multi-parameter sensor object
YosemitechY4000 y4000(y4000ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y4000NumberReadings);

// Create all of the variable objects for the Y4000
YosemitechY4000_DOmgL y4000DO(&y4000, "y4000DOx-xxxx-1234-efgh-1234567890ab");
YosemitechY4000_Turbidity y4000Turb(&y4000, "y4000Tur-bxxx-1234-efgh-1234567890ab");
YosemitechY4000_Cond y4000Cond(&y4000, "y4000Con-dxxx-1234-efgh-1234567890ab");
YosemitechY4000_pH y4000pH(&y4000, "y4000pHx-xxxx-1234-efgh-1234567890ab");
YosemitechY4000_Temp y4000Temp(&y4000, "y4000Tem-pxxx-1234-efgh-1234567890ab");
YosemitechY4000_ORP y4000ORP(&y4000, "y4000ORP-xxxx-1234-efgh-1234567890ab");
YosemitechY4000_Chlorophyll y4000Chloro(&y4000, "y4000Chl-orox-1234-efgh-1234567890ab");
YosemitechY4000_BGA y4000BGA(&y4000, "y4000BGA-xxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Zebra Tech D-Opto Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/ZebraTechDOpto.h>

const char *DOptoDI12address = "5";  // The SDI-12 Address of the Zebra Tech D-Opto
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create the Zebra Tech DOpto dissolved oxygen sensor object
ZebraTechDOpto dopto(*DOptoDI12address, SDI12Power, SDI12Data);

// Create the dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable objects for the Zebra Tech
ZebraTechDOpto_DOpct dOptoDOpct(&dopto, "dOptoDOp-ctxx-1234-efgh-1234567890ab");
ZebraTechDOpto_DOmgL dOptoDOmgL(&dopto, "dOptoDOm-gLxx-1234-efgh-1234567890ab");
ZebraTechDOpto_Temp dOptoTemp(&dopto, "dOptoTem-pxxx-1234-efgh-1234567890ab");


// ==========================================================================
//    Calculated Variables
// ==========================================================================

// Create any calculated variables you want here

// Create the function to give your calculated result.
// The function should take no input (void) and return a float.
// You can use any named variables to access values by way of variable.getValue()

/*
float calculateVariableValue(void)
{
    float calculatedResult = -9999;  // Always safest to start with a bad value
    float inputVar1 = variable1.getValue();
    float inputVar2 = variable2.getValue();
    if (inputVar1 != -9999 && inputVar2 != -9999)  // make sure both inputs are good
    {
        calculatedResult = inputVar1 + inputVar2;
    }
    return calculatedResult;
}

// Properties of the calculated variable
const char *calculatedVarName = "varName";  // This must be a value from http://vocabulary.odm2.org/variablename/
const char *calculatedVarUnit = "varUnit";  // This must be a value from http://vocabulary.odm2.org/units/
int calculatedVarResolution = 3;  // The number of digits after the decimal place
const char *calculatedVarUUID = "12345678-abcd-1234-efgh-1234567890ab";  // The (optional) universallly unique identifier
const char *calculatedVarCode = "calcVar";  // An (optional) short code for the variable

// Finally, create the calculated variable object
Variable calculatedVar(calculateVariableValue, calculatedVarName,
                       calculatedVarUnit, calculatedVarResolution,
                       calculatedVarUUID, calculatedVarCode);
*/


// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
#include <VariableArray.h>

Variable *variableList[] = {
  &mcuBoardSampNo,
  &mcuBoardBatt,
  &mcuBoardAvailableRAM,
  &modemRSSI,
  &modemSignalPct,
  &ds3231Temp,
  &atlasCO2CO2,
  &atlasCO2Temp,
  &atlasDOconc,
  &atlasDOpct,
  &atlasCond,
  &atlasTDS,
  &atlasSal,
  &atlasGrav,
  &atlasORPot,
  &atlaspHpH,
  &atlasTemp,
  &am2315Humid,
  &am2315Temp,
  &dhtHumid,
  &dhtTemp,
  &dhtHI,
  &sq212PAR,
  &bme280Humid,
  &bme280Temp,
  &bme280Press,
  &bme280Alt,
  &obs3TurbLow,
  &obs3VoltLow,
  &obs3TurbHigh,
  &obs3VoltHigh,
  &fivetmEa,
  &fivetmVWC,
  &fivetmTemp,
  &ctdCond,
  &ctdTemp,
  &ctdDepth,
  &es2Cond,
  &es2Temp,
  &extvoltV,
  &mplPress,
  &mplTemp,
  &sonar1Range,
  &ds18Temp,
  &ms5803Press,
  &ms5803Temp,
  &tbi2cTips,
  &tbi2cDepth,
  &inaCurrent,
  &inaVolt,
  &inaPower,
  &acculevPress,
  &acculevTemp,
  &acculevHeight,
  &nanolevPress,
  &nanolevTemp,
  &nanolevHeight,
  &y504DOpct,
  &y504DOmgL,
  &y504Temp,
  &y510Turb,
  &y510Temp,
  &y511Turb,
  &y511Temp,
  &y514Chloro,
  &y514Temp,
  &y520Cond,
  &y520Temp,
  &y532Voltage,
  &y532pH,
  &y532Temp,
  &y550COD,
  &y550Turbid,
  &y550Temp,
  &y4000DO,
  &y4000Turb,
  &y4000Cond,
  &y4000pH,
  &y4000Temp,
  &y4000ORP,
  &y4000Chloro,
  &y4000BGA,
  &dOptoDOpct,
  &dOptoDOmgL,
  &dOptoTemp,
};


// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);


// ==========================================================================
//     The Logger Object[s]
// ==========================================================================
#include <LoggerBase.h>

// Create a logger instance
Logger dataLogger;


// ==========================================================================
//    A Publisher to WikiWatershed
// ==========================================================================
// Device registration and sampling feature information can be obtained after
// registration at http://data.WikiWatershed.org
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID

// Create a data publisher for the EnviroDIY/WikiWatershed POST endpoint
#include <publishers/EnviroDIYPublisher.h>
EnviroDIYPublisher EnviroDIYPOST(dataLogger, registrationToken, samplingFeature);


// ==========================================================================
//    A Publisher to DreamHost
// ==========================================================================
// NOTE:  This is an outdated data collection tool used by the Stroud Center.
// It us unlikely that you will use this.

const char * DreamHostPortalRX = "xxxx";

// Create a data publisher to DreamHost
#include <publishers/DreamHostPublisher.h>
#if defined USE_UBLOX_R410M
DreamHostPublisher DreamHostGET(dataLogger, &tinyClient2, DreamHostPortalRX);
#else
DreamHostPublisher DreamHostGET(dataLogger, DreamHostPortalRX);
#endif


// ==========================================================================
//    ThingSpeak Data Publisher
// ==========================================================================
// Create a channel with fields on ThingSpeak in advance
// The fields will be sent in exactly the order they are in the variable array.
// Any custom name or identifier given to the field on ThingSpeak is irrelevant.
// No more than 8 fields of data can go to any one channel.  Any fields beyond the
// eighth in the array will be ignored.
const char *thingSpeakMQTTKey = "XXXXXXXXXXXXXXXX";  // Your MQTT API Key from Account > MyProfile.
const char *thingSpeakChannelID = "######";  // The numeric channel id for your channel
const char *thingSpeakChannelKey = "XXXXXXXXXXXXXXXX";  // The Write API Key for your channel

// Create a data publisher for ThingSpeak
#include <publishers/ThingSpeakPublisher.h>
#if defined USE_UBLOX_R410M
ThingSpeakPublisher TsMqtt(dataLogger, &tinyClient3, thingSpeakMQTTKey, thingSpeakChannelID, thingSpeakChannelKey);
#else
ThingSpeakPublisher TsMqtt(dataLogger, thingSpeakMQTTKey, thingSpeakChannelID, thingSpeakChannelKey);
#endif


// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75)
{
    for (uint8_t i = 0; i < numFlash; i++) {
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, LOW);
        delay(rate);
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        delay(rate);
    }
    digitalWrite(redLED, LOW);
}


// Read's the battery voltage
// NOTE: This will actually return the battery level from the previous update!
float getBatteryVoltage()
{
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
}


// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    // Wait for USB connection to be established by PC
    // NOTE:  Only use this when debugging - if not connected to a PC, this
    // could prevent the script from starting
    #if defined SERIAL_PORT_USBVIRTUAL
      while (!SERIAL_PORT_USBVIRTUAL && (millis() < 10000)){}
    #endif

    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);
    Serial.println();

    Serial.print(F("Using ModularSensors Library version "));
    Serial.println(MODULAR_SENSORS_VERSION);

    if (String(MODULAR_SENSORS_VERSION) !=  String(libraryVersion))
        Serial.println(F(
            "WARNING: THIS EXAMPLE WAS WRITTEN FOR A DIFFERENT VERSION OF MODULAR SENSORS!!"));

    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
        enableInterrupt(softSerialRx, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif
    #if defined NeoSWSerial_h
        enableInterrupt(neoSSerial1Rx, neoSSerial1ISR, CHANGE);
    #endif

    // Start the serial connection with the modem
    modemSerial.begin(modemBaud);

    // Start the stream for the modbus sensors; all currently supported modbus sensors use 9600 baud
    modbusSerial.begin(9600);

    // Start the SoftwareSerial stream for the sonar; it will always be at 9600 baud
    sonarSerial.begin(9600);

    // Assign pins SERCOM functionality for SAMD boards
    // NOTE:  This must happen *after* the begin
    #if defined ARDUINO_ARCH_SAMD
    #ifndef ENABLE_SERIAL2
    pinPeripheral(10, PIO_SERCOM);  // Serial2 Tx/Dout = SERCOM1 Pad #2
    pinPeripheral(11, PIO_SERCOM);  // Serial2 Rx/Din = SERCOM1 Pad #0
    #endif
    #ifndef ENABLE_SERIAL3
    pinPeripheral(2, PIO_SERCOM);  // Serial3 Tx/Dout = SERCOM2 Pad #2
    pinPeripheral(5, PIO_SERCOM);  // Serial3 Rx/Din = SERCOM2 Pad #3
    #endif
    #endif

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set up some of the power pins so the board boots up with them off
    // NOTE:  This isn't necessary at all.  The logger begin() function
    // should leave all power pins off when it finishes.
    if (modemVccPin >= 0)
    {
        pinMode(modemVccPin, OUTPUT);
        digitalWrite(modemVccPin, LOW);
    }
    if (sensorPowerPin >= 0)
    {
        pinMode(sensorPowerPin, OUTPUT);
        digitalWrite(sensorPowerPin, LOW);
    }

    // Set up the sleep/wake pin for the modem and put its inital value as "off"
    #if defined TINY_GSM_MODEM_SIM800 && defined SIM800_GPRSBEE_R6  // ONLY FOR GPRSBee R6!!!!
        if (modemSleepRqPin >= 0)
        {
            pinMode(modemSleepRqPin, OUTPUT);
            digitalWrite(modemSleepRqPin, LOW);
        }
    #else
        if (modemSleepRqPin >= 0)
        {
            pinMode(modemSleepRqPin, OUTPUT);
            digitalWrite(modemSleepRqPin, HIGH);
        }
        if (modemResetPin >= 0)
        {
            pinMode(modemResetPin, OUTPUT);
            digitalWrite(modemResetPin, HIGH);
        }
    #endif

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modem);
    dataLogger.setLoggerPins(sdCardPin, wakePin, greenLED, buttonPin);

    // Begin the logger
    dataLogger.begin(LoggerID, loggingInterval, &varArray);

    // Note:  Please change these battery voltages to match your battery
    // Check that the battery is OK before powering the modem
    if (getBatteryVoltage() > 3.7)
    {
        modem.modemPowerUp();
        modem.wake();

        // Run any extra pre-set-up for the modem
        Serial.println(F("Running extra modem pre-setup"));
        extraModemSetup();

        // At very good battery voltage, or with suspicious time stamp, sync the clock
        // Note:  Please change these battery voltages to match your battery
        if (getBatteryVoltage() > 3.8 ||
            dataLogger.getNowEpoch() < 1546300800 ||  /*Before 01/01/2019*/
            dataLogger.getNowEpoch() > 1735689600)  /*Before 1/1/2025*/
        {
            // Synchronize the RTC with NIST
            Serial.println(F("Attempting to synchronize RTC with NIST"));
            if (modem.connectInternet(120000L))
            {
                dataLogger.setRTClock(modem.getNISTTime());
            }
        }
    }

    // Set up the sensors, except at lowest battery level
    if (getBatteryVoltage() > 3.4)
    {
        Serial.println(F("Setting up sensors..."));
        varArray.setupSensors();
    }

    // Power down the modem
    modem.modemSleepPowerDown();

    // Create the log file, adding the default header to it
    // Do this last so we have the best chance of getting the time correct and
    // all sensor names correct
    // Writing to the SD card can be power intensive, so if we're skipping
    // the sensor setup we'll skip this too.
    if (getBatteryVoltage() > 3.4)
    {
        dataLogger.createLogFile(true);
    }

    // Call the processor sleep
    Serial.println(F("Putting processor to sleep"));
    dataLogger.systemSleep();
}


// ==========================================================================
// Main loop function
// ==========================================================================

// Use this short loop for simple data logging and sending
// /*
void loop()
{
    // Note:  Please change these battery voltages to match your battery
    // At very low battery, just go back to sleep
    if (getBatteryVoltage() < 3.4)
    {
        dataLogger.systemSleep();
    }
    // At moderate voltage, log data but don't send it over the modem
    else if (getBatteryVoltage() < 3.7)
    {
        dataLogger.logData();
    }
    // If the battery is good, send the data to the world
    else
    {
        dataLogger.logDataAndSend();
    }
}
// */


// Use this long loop when you want to do something special
// Because of the way alarms work on the RTC, it will wake the processor and
// start the loop every minute exactly on the minute.
// The processor may also be woken up by another interrupt or level change on a
// pin - from a button or some other input.
// The "if" statements in the loop determine what will happen - whether the
// sensors update, testing mode starts, or it goes back to sleep.
/*
void loop()
{
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    // We're only doing anything at all if the battery is above 3.4V
    if (dataLogger.checkInterval() && getBatteryVoltage() > 3.4)
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;

        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        dataLogger.alertOn();

        // Turn on the modem to let it start searching for the network
        // Only turn the modem on if the battery at the last interval was high enough
        // NOTE:  if the modemPowerUp function is not run before the completeUpdate
        // function is run, the modem will not be powered and will not return
        // a signal strength readign.
        if (getBatteryVoltage() > 3.7)
            modem.modemPowerUp();

        // Do a complete update on the variable array.
        // This this includes powering all of the sensors, getting updated
        // values, and turing them back off.
        // NOTE:  The wake function for each sensor should force sensor setup
        // to run if the sensor was not previously set up.
        varArray.completeUpdate();

        // Create a csv data record and save it to the log file
        dataLogger.logToSD();

        // Connect to the network
        // Again, we're only doing this if the battery is doing well
        if (getBatteryVoltage() > 3.7)
        {
            if (modem.connectInternet())
            {
                // Post the data to the WebSDL
                dataLogger.sendDataToRemotes();

                // Sync the clock at midnight
                if (Logger::markedEpochTime != 0 && Logger::markedEpochTime % 86400 == 0)
                {
                    Serial.println(F("Running a daily clock sync..."));
                    dataLogger.setRTClock(modem.getNISTTime());
                }

                // Disconnect from the network
                modem.disconnectInternet();
            }
            // Turn the modem off
            modem.modemSleepPowerDown();
        }

        // Turn off the LED
        dataLogger.alertOff();
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) dataLogger.testingMode();

    // Call the processor sleep
    dataLogger.systemSleep();
}
*/
