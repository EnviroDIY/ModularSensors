/*****************************************************************************
altMayfly.cpp   (more control than .ino)
Written By:  Neil Hancock from great example/menu_a_la_carte by Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.19.6

This sketch is an example of logging data to an SD card and sending the data to
the EnviroDIY data portal.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <Time.h>
#include <errno.h>
#include "ms_cfg.h" //must be before modular_sensors_common.h
#include "ms_common.h"
#define DEBUGGING_SERIAL_OUTPUT Serial
#define KCONFIG_SHOW_NETWORK_INFO 1
// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The library version this example was written for
const char *libraryVersion = "0.19.6";
// The name of this file
const char *sketchName = "atlMayfly.cpp";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID_def = LOGGERID_DEF_STR;
const char *configIniID = configIniID_DEF_STR;  
// How frequently (in minutes) to log data
//const uint8_t loggingInterval = 5;
// The logger's timezone default.
int8_t timeZone =  CONFIG_TIME_ZONE_DEF;
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

const char build_date[] = __DATE__ " " __TIME__;
const char file_name[] = __FILE__;
//const char git_version[] = GIT_BRANCH;
//const char build_epochTime = __BUILD_TIMESTAMP__;
//const char build_epochTime = __TIME_UNIX__;

// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud = 115200;   // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;        // MCU pin for the green LED (-1 if not applicable)
const int8_t redLED = 9;          // MCU pin for the red LED (-1 if not applicable)
const int8_t buttonPin = -1;      // 21 Not used -MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = A7;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin = 22; // MCU pin controlling main sensor power (-1 if not applicable)

// Create and return the main processor chip "sensor" - for general metadata
const char *mcuBoardVersion = HwVersion_DEF;
#if defined(ProcessorStats_ACT)
ProcessorStats mcuBoard(mcuBoardVersion);
#endif //ProcessorStats_ACT

//#define CHECK_SLEEP_POWER

// Create the battery voltage and free RAM variable objects for the processor and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *mcuBoardBatt = new ProcessorStats_Batt(&mcuBoard, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *mcuBoardAvailableRAM = new ProcessorStats_FreeRam(&mcuBoard, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *mcuBoardSampNo = new ProcessorStats_SampleNumber(&mcuBoard, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================

// The modem and a number of sensors communicate over UART/TTL - often called "serial".
// "Hardware" serial ports (automatically controlled by the MCU) are generally
// the most accurate and should be configured and used for as many peripherals
// as possible.  In some cases (ie, modbus communication) many sensors can share
// the same serial port.

#if not defined(ARDUINO_ARCH_SAMD) && not defined(ATMEGA2560)  // For AVR boards
// Unfortunately, most AVR boards have only one or two hardware serial ports,
// so we'll set up three types of extra software serial ports to use

// AltSoftSerial by Paul Stoffregen (https://github.com/PaulStoffregen/AltSoftSerial)
// is the most accurate software serial port for AVR boards.
// AltSoftSerial can only be used on one set of pins on each board so only one
// AltSoftSerial port can be used.
// Not all AVR boards are supported by AltSoftSerial.
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;

#if not defined(ATMEGA32U4)  // NeoSWSerial Doesn't support Leonardo
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


#if defined(ARDUINO_ARCH_SAMD)
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

// Select the modem chip, in ms_cfg.h 

// Select your modem chip - this determines the exact commands sent to it
// #define TINY_GSM_MODEM_SIM800  // Select for a SIMCOM SIM800, SIM900, or variant thereof
// define SIM800_GPRSBEE_R6  // These use atypical sleep and wake fxns
// #define TINY_GSM_MODEM_SIM808  // Select for a SIMCOM SIM808 or SIM868, or variant thereof
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define USE_XBEE_BYPASS  // If you're using a Digi 3G or LTE-M XBee in bypass mode as a u-blox
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's
#define USE_XBEE_WIFI  // If you're using a S6B wifi XBee
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

#if defined(TINY_GSM_MODEM_XBEE) || defined(USE_XBEE_BYPASS)
  #define TINY_GSM_YIELD() { delay(2); }  // Use to counter slow (9600) baud rate
#endif

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible
// AltSoftSerial &modemSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modemSerial = neoSSerial1;  // For software serial if needed
//#define RS485PHY_TX 5  // AltSoftSerial Tx pin 
//#define RS485PHY_RX 6  // AltSoftSerial Rx pin
const int8_t RS485PHY_TX_PIN = CONFIG_HW_RS485PHY_TX_PIN;
const int8_t RS485PHY_RX_PIN = CONFIG_HW_RS485PHY_RX_PIN;

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
//#define STREAMDEBUGGER_DBG
#if !defined(STREAMDEBUGGER_DBG)
#if defined(TINY_GSM_MODEM_XBEE)
TinyGsm *tinyModem = new TinyGsm(modemSerial, modemResetPin);
#else
TinyGsm *tinyModem = new TinyGsm(modemSerial);
#endif
#endif //STREAMDEBUGGER_DBG

// Use this to create a modem if you want to spy on modem communication through
// a secondary Arduino stream.  Make sure you install the StreamDebugger library!
// https://github.com/vshymanskyy/StreamDebugger
#ifdef STREAMDEBUGGER_DBG
 #include <StreamDebugger.h>
 StreamDebugger modemDebugger(modemSerial, Serial);
 TinyGsm *tinyModem = new TinyGsm(modemDebugger);
#endif //STREAMDEBUGGER_DBG
// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);
// The ublox is very slow to open and close clients, so we can iterate through
// mutiple data senders much more quickly if we have multiple clients
#if defined(TINY_GSM_MODEM_UBLOX)
TinyGsmClient *tinyClient2 = new TinyGsmClient(*tinyModem);
TinyGsmClient *tinyClient3 = new TinyGsmClient(*tinyModem);
#endif


// ==========================================================================
//    Specific Modem On-Off Methods
// ==========================================================================

bool modemSetup=false;

// This should apply to all Digi brand XBee modules.
#if defined(TINY_GSM_MODEM_XBEE) || defined(USE_XBEE_BYPASS)
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 9600;        // Communication speed of the modem, 9600 is default for XBee
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
        Serial.println(F("modemSleepFxnH"));
        return true;
    }
    Serial.println(F("modemSleepFxn!"));
    return true;
}
bool modemWakeFxn(void)
{
    if (modemVccPin >= 0){  // Turns on when power is applied
        Serial.print(F("modemWakeFxnV!="));
        Serial.println(modemVccPin);
        return true;
    }else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);  // Because the XBee doesn't have any lights
        Serial.println(F("modemWakeFxnL"));
        return true;
    }
    Serial.print(F("modemWakeFxn!"));
    return true;
}

// An extra function to set up pin sleep and other preferences on the XBee
// NOTE:  This will only succeed if the modem is turned on and awake!
#if defined(TINY_GSM_MODEM_XBEE)
void setupXBee(void)
{
    tinyModem->init();  // initialize
    if (tinyModem->commandMode())
    {
        tinyModem->sendAT(F("SM"),1);  // Pin sleep
        tinyModem->waitResponse();
        tinyModem->sendAT(F("DO"),0);  // Disable remote manager, USB Direct, and LTE PSM
        // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
        // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
        // so we'll use the Digi pin sleep instead.
        tinyModem->waitResponse();
        #ifdef KCONFIG_SHOW_NETWORK_INFO
        PRINTOUT(F("Get IP number"));
        String xbeeRsp;
        for (int mdm_lp=1;mdm_lp<7;mdm_lp++) {
            delay(mdm_lp*500);
            tinyModem->sendAT(F("MY"));  // Request IP #
            tinyModem->waitResponse(1000,xbeeRsp);
            PRINTOUT("mdmIP:"+xbeeRsp);
            if (0!=xbeeRsp.compareTo("0.0.0.0")) {
                break;
            }
            xbeeRsp="";
        }
        #endif
        #if defined(USE_XBEE_WIFI)
        tinyModem->sendAT(F("SO"),100);  // For WiFi - Disassociate from AP for Deep Sleep
        //tinyModem->waitResponse();
        #else
        tinyModem->sendAT(F("SO"),0);  // For Cellular - disconnected sleep
        tinyModem->waitResponse();
        tinyModem->sendAT(F("N#"),2);  // Cellular network technology - LTE-M Only
        // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
        #endif
        tinyModem->waitResponse();
        tinyModem->writeChanges();
        tinyModem->exitCommand();
    }
}
#elif defined(USE_XBEE_BYPASS)
void setupXBee(void)
{
    delay(1000);  // Guard time for command mode
    tinyModem->streamWrite(GF("+++"));  // enter command mode
    tinyModem->waitResponse(2000, F("OK\r"));
    tinyModem->sendAT(F("SM"),1);  // Pin sleep
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->sendAT(F("DO"),0);  // Disable remote manager, USB Direct, and LTE PSM
    // NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no
    // easy way on the LTE-M Bee to wake the cell chip itself from PSM,
    // so we'll use the Digi pin sleep instead.
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->sendAT(F("SO"),0);  // For Cellular - disconnected sleep
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->sendAT(F("N#"),2);  // Cellular network technology - LTE-M Only
    // LTE-M XBee connects much faster on AT&T/Hologram when set to LTE-M only (instead of LTE-M/NB IoT)
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->sendAT(F("AP5"));  // Turn on bypass mode
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->sendAT(F("WR"));  // Write changes to flash
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->sendAT(F("AC"));  // Apply changes
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->sendAT(F("FR"));  // Force reset to enter bypass mode
    tinyModem->waitResponse(F("OK\r"));
    tinyModem->init();  // initialize
}
#endif

#elif defined(TINY_GSM_MODEM_ESP8266)
#elif defined(TINY_GSM_MODEM_UBLOX)
#elif defined(TINY_GSM_MODEM_SIM800) && defined(SIM800_GPRSBEE_R6)
#else
#endif


// ==========================================================================
//    Network Information and LoggerModem Object
// ==========================================================================
#include <LoggerModem.h>

// Network connection information
const char *apn_def = APN_CDEF;  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId_def = WIFIID_CDEF;  // The WiFi access point, unnecessary for gprs
const char *wifiPwd_def = WIFIPWD_CDEF;  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a Client, and functions for wake and sleep
#if defined(TINY_GSM_MODEM_ESP8266) || defined(USE_XBEE_WIFI)
loggerModem modemPhy(modemVccPin, modemStatusPin, modemStatusLevel, modemWakeFxn, modemSleepFxn, tinyModem, tinyClient, wifiId_def, wifiPwd_def);
// ^^ Use this for WiFi
#else
loggerModem modemPhy(modemVccPin, modemStatusPin, modemStatusLevel, modemWakeFxn, modemSleepFxn, tinyModem, tinyClient, apn);
// ^^ Use this for cellular
#endif

// loggerModem modemPhy(modemVccPin, modemStatusPin, modemStatusLevel, modemSleepRqPin, tinyModem, tinyClient, apn);
// ^^ Use this for a chip where you want to use the "default" functions

// Create the RSSI and signal strength variable objects for the modem and return
// variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *modemRSSI = new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *modemSignalPct = new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);

// Create the temperature variable object for the DS3231 and return a variable-type pointer to it
// Use this to create a variable pointer with a name to use in multiple arrays or any calculated variables.
// Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab");


#ifdef SENSOR_CONFIG_GENERAL
// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>

const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)

// Create and return the AOSong AM2315 sensor object
AOSongAM2315 am2315(I2CPower);

// Create the humidity and temperature variable objects for the AM2315 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *am2315Humid = new AOSongAM2315_Humidity(&am2315, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *am2315Temp = new AOSongAM2315_Temp(&am2315, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    AOSong DHT 11/21 (AM2301)/22 (AM2302) Digital Humidity and Temperature
// ==========================================================================
#include <sensors/AOSongDHT.h>

const int8_t DHTPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t DHTPin = 10;  // DHT data pin
DHTtype dhtType = DHT11;  // DHT type, either DHT11, DHT21, or DHT22

// Create and return the AOSong DHT sensor object
AOSongDHT dht(DHTPower, DHTPin, dhtType);

// Create the humidity, temperature and heat index variable objects for the DHT
// and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *dhtHumid = new AOSongDHT_Humidity(&dht, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *dhtTemp = new AOSongDHT_Temp(&dht, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *dhtHI = new AOSongDHT_HI(&dht, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Apogee SQ-212 Photosynthetically Active Radiation (PAR) Sensor
// ==========================================================================
#include <sensors/ApogeeSQ212.h>

const int8_t SQ212Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SQ212ADSChannel = 3;  // The ADS channel for the SQ212
const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC

// Create and return the Apogee SQ212 sensor object
ApogeeSQ212 SQ212(SQ212Power, SQ212ADSChannel);

// Create the PAR variable object for the SQ212 and return a variable-type pointer to it
// Use this to create a variable pointer with a name to use in multiple arrays or any calculated variables.
// Variable *sq212PAR = new ApogeeSQ212_PAR(&SQ212, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <sensors/BoschBME280.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t BMEi2c_addr = 0x76;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address

// Create and return the Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);

// Create the four variable objects for the BME280 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *bme280Humid = new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *bme280Temp = new BoschBME280_Temp(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *bme280Press = new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *bme280Alt = new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-efgh-1234567890ab");


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
// Create and return the Campbell OBS3+ LOW RANGE sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowADSChannel, OBSLow_A, OBSLow_B, OBSLow_C, ADSi2c_addr, OBS3numberReadings);

// Create the turbidity and voltage variable objects for the low range OBS3 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *obs3TurbLow = new CampbellOBS3_Turbidity(&osb3low, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *obs3VoltLow = new CampbellOBS3_Voltage(&osb3low, "12345678-abcd-1234-efgh-1234567890ab");


// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighADSChannel = 1;  // The ADS channel for the high range output
const float OBSHigh_A = 0.000E+00;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 1.000E+00;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = 0.000E+00;  // The "C" value from the high range calibration
// Create and return the Campbell OBS3+ HIGH RANGE sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighADSChannel, OBSHigh_A, OBSHigh_B, OBSHigh_C, ADSi2c_addr, OBS3numberReadings);

// Create the turbidity and voltage variable objects for the high range OBS3 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *obs3TurbHigh = new CampbellOBS3_Turbidity(&osb3high, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *obs3VoltHigh = new CampbellOBS3_Voltage(&osb3high, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Decagon 5TM Soil Moisture Sensor
// ==========================================================================
#include <sensors/Decagon5TM.h>

const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create and return the Decagon 5TM sensor object
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);

// Create the matric potential, volumetric water content, and temperature
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// variable objects for the 5TM and return variable-type pointers to them
// Variable *fivetmEa = new Decagon5TM_Ea(&fivetm, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *fivetmVWC = new Decagon5TM_VWC(&fivetm, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *fivetmTemp = new Decagon5TM_Temp(&fivetm, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>

const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDnumberReadings = 6;  // The number of readings to average
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create and return the Decagon CTD sensor object
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, CTDnumberReadings);

// Create the conductivity, temperature, and depth variable objects for the CTD
// and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *ctdCond = new DecagonCTD_Cond(&ctd, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *ctdTemp = new DecagonCTD_Temp(&ctd, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *ctdDepth = new DecagonCTD_Depth(&ctd, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Decagon ES2 Conductivity and Temperature Sensor
// ==========================================================================
#include <sensors/DecagonES2.h>

const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin
const uint8_t ES2NumberReadings = 3;

// Create and return the Decagon ES2 sensor object
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data, ES2NumberReadings);

// Create the conductivity and temperature variable objects for the ES2 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *es2Cond = new DecagonES2_Cond(&es2, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *es2Temp = new DecagonES2_Temp(&es2, "12345678-abcd-1234-efgh-1234567890ab");

#endif //SENSOR_CONFIG_GENERAL
#ifdef ExternalVoltage_ACT
// ==========================================================================
//    External Voltage via TI ADS1115
// ==========================================================================
#include <sensors/ExternalVoltage.h>

const int8_t ADSPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t ADSChannel0 = 0;  // The ADS channel of interest
const int8_t ADSChannel1 = 1;  // The ADS channel of interest
const int8_t ADSChannel2 = 2;  // The ADS channel of interest
const int8_t ADSChannel3 = 3;  // The ADS channel of interest
const float dividerGain = 2; //  Default 1/gain for grove voltage divider is 10x
const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC
const uint8_t VoltReadsToAvg = 1; // Only read one sample

// Create and return the External Voltage sensor object
ExternalVoltage extvolt0(ADSPower, ADSChannel0, dividerGain, ADSi2c_addr, VoltReadsToAvg);
ExternalVoltage extvolt1(ADSPower, ADSChannel1, dividerGain, ADSi2c_addr, VoltReadsToAvg);

// Create the voltage variable object and return a variable-type pointer to it
// Variable *extvoltV = new ExternalVoltage_Volt(&extvolt, "12345678-abcd-1234-efgh-1234567890ab");
#endif //ExternalVoltage_ACT
#ifdef SENSOR_CONFIG_GENERAL
// ==========================================================================
//    Freescale Semiconductor MPL115A2 Barometer
// ==========================================================================
#include <sensors/FreescaleMPL115A2.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MPL115A2ReadingsToAvg = 1;

// Create and return the MPL115A2 barometer sensor object
MPL115A2 mpl115a2(I2CPower, MPL115A2ReadingsToAvg);

// Create the pressure and temperature variable objects for the MPL and return variable-type pointer to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *mplPress = new MPL115A2_Pressure(&mpl115a2, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *mplTemp = new MPL115A2_Temp(&mpl115a2, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxbotix HRXL Ultrasonic Range Finder
// ==========================================================================
#include <sensors/MaxBotixSonar.h>

// Create a reference to the serial port for the sonar
// A Maxbotix sonar with the trigger pin disconnect CANNOT share the serial port
// A Maxbotix sonar using the trigger may be able to share but YMMV
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
#if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
HardwareSerial &sonarSerial = Serial3;  // Use hardware serial if possible
#else
// AltSoftSerial &sonarSerial = altSoftSerial;  // For software serial if needed
NeoSWSerial &sonarSerial = neoSSerial1;  // For software serial if needed
// SoftwareSerial_ExtInts &sonarSerial = softSerial1;  // For software serial if needed
#endif

const int8_t SonarPower = sensorPowerPin;  // Excite (power) pin (-1 if unconnected)
const int8_t Sonar1Trigger = A1;  // Trigger pin (a unique negative number if unconnected) (D25 = A1)

// Create and return the MaxBotix Sonar sensor object
MaxBotixSonar sonar1(sonarSerial, SonarPower, Sonar1Trigger) ;

// Create the voltage variable object and return a variable-type pointer to it
// Variable *sonar1Range = new MaxBotixSonar_Range(&sonar1, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <sensors/MaximDS18.h>

// OneWire Address [array of 8 hex characters]
DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
DeviceAddress OneWireAddress2 = {0x28, 0xFF, 0x57, 0x90, 0x82, 0x16, 0x04, 0x67};
DeviceAddress OneWireAddress3 = {0x28, 0xFF, 0x74, 0x2B, 0x82, 0x16, 0x03, 0x57};
DeviceAddress OneWireAddress4 = {0x28, 0xFF, 0xB6, 0x6E, 0x84, 0x16, 0x05, 0x9B};
DeviceAddress OneWireAddress5 = {0x28, 0xFF, 0x3B, 0x07, 0x82, 0x16, 0x03, 0xB3};
const int8_t OneWirePower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t OneWireBus = A0;  // Pin attached to the OneWire Bus (-1 if unconnected) (D24 = A0)

// Create and return the Maxim DS18 sensor objects (use this form for a known address)
MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
MaximDS18 ds18_2(OneWireAddress2, OneWirePower, OneWireBus);
MaximDS18 ds18_3(OneWireAddress3, OneWirePower, OneWireBus);
MaximDS18 ds18_4(OneWireAddress4, OneWirePower, OneWireBus);
MaximDS18 ds18_5(OneWireAddress5, OneWirePower, OneWireBus);

// Create and return the Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
// MaximDS18 ds18_u(OneWirePower, OneWireBus);

// Create the temperature variable object for the DS18 and return a variable-type pointer to it
// Use this to create a variable pointer with a name to use in multiple arrays or any calculated variables.
// Variable *ds18Temp = new MaximDS18_Temp(&ds18_u, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <sensors/MeaSpecMS5803.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 (default) or 0x77
const int16_t MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;

// Create and return the MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);

// Create the conductivity and temperature variable objects for the ES2 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *ms5803Press = new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *ms5803Temp = new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    External I2C Rain Tipping Bucket Counter
// ==========================================================================
#include <sensors/RainCounterI2C.h>

const uint8_t RainCounterI2CAddress = 0x08;  // I2C Address for external tip counter
const float depthPerTipEvent = 0.2;  // rain depth in mm per tip event

// Create and return the Rain Counter sensor object
RainCounterI2C tbi2c(RainCounterI2CAddress, depthPerTipEvent);

// Create the conductivity and temperature variable objects for the ES2 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *tbi2cTips = new RainCounterI2C_Tips(&tbi2c, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *tbi2cDepth = new RainCounterI2C_Depth(&tbi2c, "12345678-abcd-1234-efgh-1234567890ab");
#endif //SENSOR_CONFIG_GENERAL
#if defined(INA219ORIG_PHY_ACT)


// ==========================================================================
//    TI INA219 High Side Current/Voltage Sensor (Current mA, Voltage, Power)
// ==========================================================================
#include <sensors/TIINA219.h>

uint8_t INA219i2c_addr = 0x40; // 1000000 (Board A0+A1=GND)
// The INA219 can be addressed either as 0x40 (Adafruit default) or 0x41 44 45
// Either can be physically mofidied for the other address
// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t INA219ReadingsToAvg = 1;

// Create and return the INA219 sensor object
TIINA219 ina219(I2CPower, INA219i2c_addr, INA219ReadingsToAvg);

// Create the current, voltage, and power variable objects for the Nanolevel and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *inaCurrent = new TIINA219_Current(&ina219, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *inaVolt = new TIINA219_Volt(&ina219, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *inaPower = new TIINA219_Power(&ina219, "12345678-abcd-1234-efgh-1234567890ab");


#endif //INA219ORIG_PHY_ACT
#if defined(INA219M_PHY_ACT)
//    TI INA219 High Side Current/Voltage Sensor (Current mA, Voltage, Power)
#include <sensors/TIINA219M.h>

uint8_t INA219i2c_addr = 0x40; // 1000000 (Board A0+A1=GND)
// The INA219 can be addressed either as 0x40 (Adafruit default) or 0x41 44 45
// Either can be physically mofidied for the other address
const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t INA219ReadingsToAvg = 1;

// Create and return the INA219 sensor object
TIINA219M ina219m_phy(I2CPower, INA219i2c_addr, INA219ReadingsToAvg);
//was TIINA219M ina219_phy(I2CPower);

// Create the current, voltage, and power variable objects for the Nanolevel and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *inaCurrent = new TIINA219_Current(&ina219, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *inaVolt = new TIINA219_Volt(&ina219, "12345678-abcd-1234-efgh-1234567890ab");
// NO Power
#endif //INA219M_PHY_ACT

#if defined(KellerAcculevel_ACT) || defined(KellerNanolevel_ACT)
// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
#if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
HardwareSerial &modbusSerial = Serial2;  // Use hardware serial if possible
#else
AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
#endif
const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
#endif //defined(KellerAcculevel_ACT) | defined(KellerNanolevel_ACT)
// ==========================================================================
//    Keller Acculevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#ifdef KellerAcculevel_ACT 
#include <sensors/KellerAcculevel.h>

byte acculevelModbusAddress = 0x01;  // The modbus address of KellerAcculevel
const uint8_t acculevelNumberReadings = 5;  // The manufacturer recommends taking and averaging a few readings

// Create and return the Keller Acculevel sensor object
KellerAcculevel acculevel(acculevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, acculevelNumberReadings);

// Create the pressure, temperature, and height variable objects for the Acculevel and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *acculevPress = new KellerAcculevel_Pressure(&acculevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *acculevTemp = new KellerAcculevel_Temp(&acculevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *acculevHeight = new KellerAcculevel_Height(&acculevel, "12345678-abcd-1234-efgh-1234567890ab");
#endif //KellerAcculevel_ACT 


// ==========================================================================
//    Keller Nanolevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#ifdef KellerNanolevel_ACT
#include <sensors/KellerNanolevel.h>

byte nanolevelModbusAddress = 0x01;  // The modbus address of KellerNanolevel
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t nanolevelNumberReadings = 3;  // The manufacturer recommends taking and averaging a few readings

// Create and return the Keller Nanolevel sensor object
KellerNanolevel nanolevelfn(nanolevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, nanolevelNumberReadings);

// Create the pressure, temperature, and height variable objects for the Nanolevel and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *acculevPress = new KellerNanolevel_Pressure(&nanolevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *acculevTemp = new KellerNanolevel_Temp(&nanolevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *acculevHeight = new KellerNanolevel_Height(&nanolevel, "12345678-abcd-1234-efgh-1234567890ab");

#endif //KellerNanolevel_ACT
#ifdef SENSOR_CONFIG_GENERAL

// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/YosemitechY504.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Yosemitech Y504 dissolved oxygen sensor object
YosemitechY504 y504(y504ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y504NumberReadings);

// Create the dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable objects for the Y504 and return variable-type
// pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y504DOpct = new YosemitechY504_DOpct(&y504, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y504DOmgL = new YosemitechY504_DOmgL(&y504, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y504Temp = new YosemitechY504_Temp(&y504, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y510 Turbidity Sensor
// ==========================================================================
#include <sensors/YosemitechY510.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Y510-B Turbidity sensor object
YosemitechY510 y510(y510ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y510NumberReadings);

// Create the turbidity and temperature variable objects for the Y510 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y510Turb = new YosemitechY510_Turbidity(&y510, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y510Temp = new YosemitechY510_Temp(&y510, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y511 Turbidity Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY511.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Y511-A Turbidity sensor object
YosemitechY511 y511(y511ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y511NumberReadings);

// Create the turbidity and temperature variable objects for the Y511 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y511Turb = new YosemitechY511_Turbidity(&y511, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y511Temp = new YosemitechY511_Temp(&y511, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y514 Chlorophyll Sensor
// ==========================================================================
#include <sensors/YosemitechY514.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Y514 chlorophyll sensor object
YosemitechY514 y514(y514ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y514NumberReadings);

// Create the chlorophyll concentration and temperature variable objects for the Y514 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y514Chloro = new YosemitechY514_Chlorophyll(&y514, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y514Temp = new YosemitechY514_Temp(&y514, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y520 Conductivity Sensor
// ==========================================================================
#include <sensors/YosemitechY520.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Y520 conductivity sensor object
YosemitechY520 y520(y520ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y520NumberReadings);

// Create the specific conductance and temperature variable objects for the Y520 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y520Cond = new YosemitechY520_Cond(&y520, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y520Temp = new YosemitechY520_Temp(&y520, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y532 pH
// ==========================================================================
#include <sensors/YosemitechY532.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Yosemitech Y532 pH sensor object
YosemitechY532 y532(y532ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y532NumberReadings);

// Create the pH, electrical potential, and temperature variable objects for the Y532 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y532Voltage = new YosemitechY532_Voltage(&y532, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y532pH = new YosemitechY532_pH(&y532, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y532Temp = new YosemitechY532_Temp(&y532, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y550 COD Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY550.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Y550 conductivity sensor object
YosemitechY550 y550(y550ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y550NumberReadings);

// Create the COD, turbidity, and temperature variable objects for the Y550 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y550COD = new YosemitechY550_COD(&y550, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y550Turbid = new YosemitechY550_Turbidity(&y550, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y550Temp = new YosemitechY550_Temp(&y550, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y4000 Multiparameter Sonde (DOmgL, Turbidity, Cond, pH, Temp, ORP, Chlorophyll, BGA)
// ==========================================================================
#include <sensors/YosemitechY4000.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
// #if defined(ARDUINO_ARCH_SAMD) || defined(ATMEGA2560)
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

// Create and return the Yosemitech Y4000 multi-parameter sensor object
YosemitechY4000 y4000(y4000ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y4000NumberReadings);

// Create all of the variable objects for the Y4000 and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *y4000DO = new YosemitechY4000_DOmgL(&y4000, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y4000Turb = new YosemitechY4000_Turbidity(&y4000, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y4000Cond = new YosemitechY4000_Cond(&y4000, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y4000pH = new YosemitechY4000_pH(&y4000, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y4000Temp = new YosemitechY4000_Temp(&y4000, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y4000ORP = new YosemitechY4000_ORP(&y4000, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y4000Chloro = new YosemitechY4000_Chlorophyll(&y4000, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *y4000BGA = new YosemitechY4000_BGA(&y4000, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Zebra Tech D-Opto Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/ZebraTechDOpto.h>

const char *DOptoDI12address = "5";  // The SDI-12 Address of the Zebra Tech D-Opto
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create and return the Zebra Tech DOpto dissolved oxygen sensor object
ZebraTechDOpto dopto(*DOptoDI12address, SDI12Power, SDI12Data);

// Create the dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable objects for the Zebra Tech and return variable-type
// pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *dOptoDOpct = new ZebraTechDOpto_DOpct(&dopto, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *dOptoDOmgL = new ZebraTechDOpto_DOmgL(&dopto, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *dOptoTemp = new ZebraTechDOpto_Temp(&dopto, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Calculated Variables
// ==========================================================================

// Create any calculated variables you want here

// Create the function to give your calculated result.
// The function should take no input (void) and return a float.
// You can use any named variable pointers to access values by way of variable->getValue()

/*
float calculateVariableValue(void)
{
    float calculatedResult = -9999;  // Always safest to start with a bad value
    float inputVar1 = variable1->getValue();
    float inputVar2 = variable2->getValue();
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
const char *calculatedVarCode = "CorrectedPressure";  // An (optional) short code for the variable

// Finally, create the calculated variable object and return a variable pointer to it
Variable *calculatedVar = new Variable(calculateVariableValue, calculatedVarName,
                                       calculatedVarUnit, calculatedVarResolution,
                                       calculatedVarUUID, calculatedVarCode);
*/
#endif //SENSOR_CONFIG_GENERAL

// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================
#include <VariableArray.h>

// FORM1: Create pointers for all of the variables from the sensors,
// at the same time putting them into an array
// NOTE:  Forms one and two can be mixed
Variable *variableList[] = {
#if defined(ProcessorStats_SampleNumber_UUID)
    //Always have this first so can see on debug screen
    new ProcessorStats_SampleNumber(&mcuBoard,ProcessorStats_SampleNumber_UUID),
#endif
#if defined(ProcessorStats_Batt_UUID)
    new ProcessorStats_Batt(&mcuBoard,   ProcessorStats_Batt_UUID),//was mayflyPhy
#endif
#if defined(ExternalVoltage_Volt0_UUID)
    new ExternalVoltage_Volt(&extvolt0, ExternalVoltage_Volt0_UUID),
#endif
#if defined(ExternalVoltage_Volt1_UUID)
    new ExternalVoltage_Volt(&extvolt1, ExternalVoltage_Volt1_UUID),
#endif
#if defined(INA219M_MA_UUID)
    new TIINA219M_Current(&ina219m_phy, INA219M_MA_UUID),
#endif
#if defined(INA219M_VOLT_UUID)
    new TIINA219M_Volt(&ina219m_phy, INA219M_VOLT_UUID),
#endif
#ifdef SENSOR_CONFIG_GENERAL
    new ApogeeSQ212_PAR(&SQ212, "12345678-abcd-1234-efgh-1234567890ab"),
    new AOSongAM2315_Humidity(&am2315, "12345678-abcd-1234-efgh-1234567890ab"),
    new AOSongAM2315_Temp(&am2315, "12345678-abcd-1234-efgh-1234567890ab"),
    new AOSongDHT_Humidity(&dht, "12345678-abcd-1234-efgh-1234567890ab"),
    new AOSongDHT_Temp(&dht, "12345678-abcd-1234-efgh-1234567890ab"),
    new AOSongDHT_HI(&dht, "12345678-abcd-1234-efgh-1234567890ab"),
    new BoschBME280_Temp(&bme280, "12345678-abcd-1234-efgh-1234567890ab"),
    new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-efgh-1234567890ab"),
    new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-efgh-1234567890ab"),
    new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-efgh-1234567890ab"),
    new CampbellOBS3_Turbidity(&osb3low, "12345678-abcd-1234-efgh-1234567890ab", "TurbLow"),
    new CampbellOBS3_Voltage(&osb3low, "12345678-abcd-1234-efgh-1234567890ab", "TurbLowV"),
    new CampbellOBS3_Turbidity(&osb3high, "12345678-abcd-1234-efgh-1234567890ab", "TurbHigh"),
    new CampbellOBS3_Voltage(&osb3high, "12345678-abcd-1234-efgh-1234567890ab", "TurbHighV"),
    new Decagon5TM_Ea(&fivetm, "12345678-abcd-1234-efgh-1234567890ab"),
    new Decagon5TM_Temp(&fivetm, "12345678-abcd-1234-efgh-1234567890ab"),
    new Decagon5TM_VWC(&fivetm, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Cond(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Temp(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Depth(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonES2_Cond(&es2, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonES2_Temp(&es2, "12345678-abcd-1234-efgh-1234567890ab"),
    new ExternalVoltage_Volt(&extvolt, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaxBotixSonar_Range(&sonar1, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaxBotixSonar_Range(&sonar2, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS18_Temp(&ds18_1, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS18_Temp(&ds18_2, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS18_Temp(&ds18_3, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS18_Temp(&ds18_4, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS18_Temp(&ds18_5, "12345678-abcd-1234-efgh-1234567890ab"),
    // new MaximDS18_Temp(&ds18_u, "12345678-abcd-1234-efgh-1234567890ab"),
    new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-efgh-1234567890ab"),
    new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-efgh-1234567890ab"),
    new MPL115A2_Temp(&mpl115a2, "12345678-abcd-1234-efgh-1234567890ab"),
    new MPL115A2_Pressure(&mpl115a2, "12345678-abcd-1234-efgh-1234567890ab"),
    new RainCounterI2C_Tips(&tbi2c, "12345678-abcd-1234-efgh-1234567890ab"),
    new RainCounterI2C_Depth(&tbi2c, "12345678-abcd-1234-efgh-1234567890ab"),
#endif //SENSOR_CONFIG_GENERAL
#ifdef KellerAcculevel_ACT
    new KellerAcculevel_Pressure(&acculevel, "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerAcculevel_Temp(&acculevel, "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerAcculevel_Height(&acculevel, "12345678-abcd-1234-efgh-1234567890ab"),
#endif // KellerAcculevel_ACT
#ifdef KellerNanolevel_ACT
//   new KellerNanolevel_Pressure(&nanolevelfn, "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerNanolevel_Temp(&nanolevelfn,   KellerNanolevel_Temp_UUID),
    new KellerNanolevel_Height(&nanolevelfn, KellerNanolevel_Height_UUID),
#endif //SENSOR_CONFIG_KELLER_NANOLEVEL
#ifdef SENSOR_CONFIG_GENERAL
    new YosemitechY504_DOpct(&y504, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY504_Temp(&y504, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY504_DOmgL(&y504, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY510_Temp(&y510, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY510_Turbidity(&y510, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY511_Temp(&y511, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY511_Turbidity(&y511, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY514_Temp(&y514, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY514_Chlorophyll(&y514, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY520_Temp(&y520, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY520_Cond(&y520, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY532_Temp(&y532, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY532_Voltage(&y532, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY532_pH(&y532, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_DOmgL(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_Turbidity(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_Cond(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_pH(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_Temp(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_ORP(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_Chlorophyll(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new YosemitechY4000_BGA(&y4000, "12345678-abcd-1234-efgh-1234567890ab"),
    new ZebraTechDOpto_Temp(&dopto, "12345678-abcd-1234-efgh-1234567890ab"),
    new ZebraTechDOpto_DOpct(&dopto, "12345678-abcd-1234-efgh-1234567890ab"),
    new ZebraTechDOpto_DOmgL(&dopto, "12345678-abcd-1234-efgh-1234567890ab"),
    new ProcessorStats_FreeRam(&mcuBoard, "12345678-abcd-1234-efgh-1234567890ab"),
#endif // SENSOR_CONFIG_GENERAL
#if defined(MaximDS3231_Temp_UUID)
    new MaximDS3231_Temp(&ds3231,      MaximDS3231_Temp_UUID),
#endif //MaximDS3231_Temp_UUID
    //new Modem_RSSI(&modemPhy, "12345678-abcd-1234-efgh-1234567890ab"),
#if defined(Modem_SignalPercent_UUID)
    //new Modem_SignalPercent(&modemPhy, Modem_SignalPercent_UUID),
#endif
    // new YOUR_variableName_HERE(&)
};

/*
// FORM2: Fill array with already created and named variable pointers
// NOTE:  Forms one and two can be mixed
Variable *variableList[] = {
    mcuBoardBatt,
    mcuBoardAvailableRAM,
    mcuBoardSampNo,
    modemRSSI,
    modemSignalPct,
    // etc, etc, etc,
    calculatedVar
}
*/


// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);


// ==========================================================================
//     Local storage - evolving
// ==========================================================================
#ifdef USE_SD_MAYFLY_INI
 persistent_store_t ps;
#endif //#define USE_SD_MAYFLY_INI
// ==========================================================================
//     The Logger Object[s]
// ==========================================================================
#include <LoggerBase.h>

// Create a new logger instance
Logger dataLogger(LoggerID_def, loggingInterval_def, sdCardPin, wakePin, &varArray);


// ==========================================================================
//    A Publisher to WikiWatershed
// ==========================================================================
// Device registration and sampling feature information can be obtained after
// registration at http://data.WikiWatershed.org
#if defined(registrationToken_UUID) && defined(samplingFeature_UUID)
const char *registrationToken_def = registrationToken_UUID;   // Device registration token
const char *samplingFeature_def = samplingFeature_UUID;     // Sampling feature UUID

// Create a data publisher for the EnviroDIY/WikiWatershed POST endpoint
#include <publishers/EnviroDIYPublisher.h>
EnviroDIYPublisher EnviroDIYPOST(dataLogger, registrationToken_def, samplingFeature_def);
#endif //registrationToken_UUID

// ==========================================================================
//    ThingSpeak Data Publisher
// ==========================================================================
// Create a channel with fields on ThingSpeak in advance
// The fields will be sent in exactly the order they are in the variable array.
// Any custom name or identifier given to the field on ThingSpeak is irrelevant.
// No more than 8 fields of data can go to any one channel.  Any fields beyond the
// eighth in the array will be ignored.
#if defined(thingSpeakMQTTKey)
const char *thingSpeakMQTTKey = "XXXXXXXXXXXXXXXX";  // Your MQTT API Key from Account > MyProfile.
const char *thingSpeakChannelID = "######";  // The numeric channel id for your channel
const char *thingSpeakChannelKey = "XXXXXXXXXXXXXXXX";  // The Write API Key for your channel

// Create a data publisher for ThingSpeak
#include <publishers/ThingSpeakPublisher.h>
#if defined(TINY_GSM_MODEM_UBLOX)
ThingSpeakPublisher TsMqtt(dataLogger, tinyClient3, thingSpeakMQTTKey, thingSpeakChannelID, thingSpeakChannelKey);
#else
ThingSpeakPublisher TsMqtt(dataLogger, thingSpeakMQTTKey, thingSpeakChannelID, thingSpeakChannelKey);
#endif
#endif //thingSpeakMQTTKey


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
// inihUnhandled 
// For any Unhandled sections this is called
// ==========================================================================
#ifdef USE_SD_MAYFLY_INI
//expect to be in near space
  //#define EDIY_PROGMEM PROGMEM
#define mCONST_UNI(p1) const char p1##_pm[] PROGMEM = #p1
const char BOOT_pm[] EDIY_PROGMEM = "BOOT";
const char VER_pm[] EDIY_PROGMEM = "VER";
const char MAYFLY_SN_pm[] EDIY_PROGMEM = "MAYFLY_SN"; 
const char MAYFLY_REV_pm[] EDIY_PROGMEM = "MAYFLY_REV";
const char MAYFLY_INIT_ID_pm[] EDIY_PROGMEM = "MAYFLY_INIT_ID";

const char COMMON_pm[] EDIY_PROGMEM = "COMMON";
const char LOGGER_ID_pm[] EDIY_PROGMEM = "LOGGER_ID";
//mCONST_UNI(LOGGER_ID);// = "nh07k" ;
const char LOGGING_INTERVAL_MIN_pm[] EDIY_PROGMEM = "LOGGING_INTERVAL_MIN";
const char LIION_TYPE_pm[] EDIY_PROGMEM = "LIION_TYPE";
const char TIME_ZONE_pm[] EDIY_PROGMEM = "TIME_ZONE";
//FUT const char GEOGRAPHICAL_ID_pm[] EDIY_PROGMEM = "GEOGRAPHICAL_ID";

const char NETWORK_pm[] EDIY_PROGMEM = "NETWORK";
const char apn_pm[] EDIY_PROGMEM = "apn";
const char WiFiId_pm[] EDIY_PROGMEM = "WiFiId";
const char WiFiPwd_pm[] EDIY_PROGMEM = "WiFiPwd";

const char PROVIDER_pm[] EDIY_PROGMEM = "PROVIDER";
const char CLOUD_ID_pm[] EDIY_PROGMEM = "CLOUD_ID";
const char REGISTRATION_TOKEN_pm[] EDIY_PROGMEM = "REGISTRATION_TOKEN";
const char SAMPLING_FEATURE_pm[] EDIY_PROGMEM = "SAMPLING_FEATURE";

const char UUIDs_pm[] EDIY_PROGMEM = "UUIDs";
const char index_pm[] EDIY_PROGMEM = "index";
static uint8_t uuid_index =0;

#define RAM_AVAILABLE   ramAvailable();
#define RAM_REPORT_LEVEL 1
void ramAvailable(){
    extern int16_t __heap_start, *__brkval;
    uint16_t top_stack = (int) &top_stack  - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    Serial.print(F(" Ram available:"));
    Serial.println(top_stack );// Stack and heap ??    
}
static int inihUnhandledFn( const char* section, const char* name, const char* value)
{
    #if RAM_REPORT_LEVEL > 1
    bool ram_track = true;
    #endif
    if (strcmp_P(section,PROVIDER_pm)== 0)
    {
        if        (strcmp_P(name,REGISTRATION_TOKEN_pm)== 0) {
            //TODO: njh move storage to class EnviroDIYPublisher
            strcpy(ps.provider.s.registration_token, value);
            Serial.print(F("PROVIDER Setting registration token: "));
            Serial.println(ps.provider.s.registration_token );
            EnviroDIYPOST.setToken(ps.provider.s.registration_token);
        } else if (strcmp_P(name,CLOUD_ID_pm)== 0) {
            //TODO: njh move storage to class EnviroDIYPublisher
            strcpy(ps.provider.s.cloudId, value);
            Serial.print(F("PROVIDER Setting cloudId: "));
            Serial.println(ps.provider.s.cloudId );
        } else if (strcmp_P(name,SAMPLING_FEATURE_pm)== 0) {
            //TODO: njh move storage to class EnviroDIYPublisher
            strcpy(ps.provider.s.sampling_feature, value);
            Serial.print(F("PROVIDER Setting SamplingFeature: "));
            Serial.println(ps.provider.s.sampling_feature );
            dataLogger.setSamplingFeatureUUID(ps.provider.s.sampling_feature);
        } else {
            Serial.print(F("PROVIDER not supported:"));
            Serial.print(name);
            Serial.print("=");
            Serial.println(value);
        }
    } else if (strcmp_P(section,UUIDs_pm)== 0)
    {
        /* UUIDs are applied to internal sensor Array as follows: 
        1) "UUID_label"="UUID"
        eg ASQ212_PAR="UUID"
           search variableList for UUID_label and if found replace with "UUID"
        2) index="UUID"
           if the word "index" is there with a UUID, then the UUID is applied in sequence. 
           Any UUID_label's found also increment the counted 'index'
        */

        uint8_t uuid_search_i=0;
    
        Serial.print(F(""));
        Serial.print(uuid_index);
        Serial.print(":");
        Serial.print(name);
        Serial.print(F("={"));
        Serial.print(value);        
        do {
            if (strcmp((const char *)variableList[uuid_search_i]->getVarUUID().c_str(),name)==0) 
            {//Found a match
                variableList[uuid_search_i]->setVarUUID((char *)value,true);
                uuid_search_i=variableCount;
            }
            uuid_search_i++;
        } while (uuid_search_i < variableCount );
        
        if (uuid_search_i > variableCount) {
            Serial.println(F("} match  & added."));
        } else 
        if (strcmp_P(name,index_pm)== 0) { //Check if index and then simple reference
            if (uuid_index < variableCount) 
            {
                Serial.print(F("} replacing {"));
                Serial.print(variableList[uuid_index]->getVarUUID() );
                Serial.println(F("}"));
                variableList[uuid_index]->setVarUUID((char *)value,true);           
            } else {
                Serial.println(F("} out of range. Notused"));
            }
        } else 
        {
            //Serial.println();
            Serial.println(F(" UUID not supported"));
            //Serial.print(name);
            //Serial.print("=");
            //Serial.println(value);
        } 
        uuid_index++;
    } else if (strcmp_P(section,COMMON_pm)== 0) {// [COMMON] processing
        if (strcmp_P(name,LOGGER_ID_pm)== 0) {
            Serial.print(F("COMMON LoggerId Set: "));
            Serial.println(value);
            dataLogger.setLoggerId(value,true);
        } else if (strcmp_P(name,LOGGING_INTERVAL_MIN_pm)== 0){
            //convert str to num with error checking
            long intervalMin;
            char *endptr;
            errno=0;
            intervalMin = strtoul(value,&endptr,10);
            #define INTERVAL_MINUTES_MAX 480
            if ((intervalMin <= INTERVAL_MINUTES_MAX) && (intervalMin>0) &&(errno!=ERANGE) ) {
                dataLogger.setLoggingInterval(intervalMin);
                Serial.print(F("COMMON Logging Interval(min): "));
                Serial.println(intervalMin);
            } else {
                Serial.print(F(" Set interval error(0-480) with:"));
                Serial.println(intervalMin);
            }
        } else if (strcmp_P(name,LIION_TYPE_pm)== 0){
            //convert  str to num with error checking
            long batLiionType;
            char *endptr;
            errno=0;
            batLiionType = strtoul(value,&endptr,10);
            if ((batLiionType < PSLR_NUM) && (batLiionType>0) &&(errno!=ERANGE) ) {
                mcuBoard.setBatteryType((ps_liion_rating_t )batLiionType);
                //mayflyPhy.setBatteryType((ps_liion_rating_t )batLiionType);
                Serial.print(F("COMMON LiIon Type: "));
                Serial.println(batLiionType);
            } else {
                Serial.print(F(" Set LiIon Type error; (range 0-2) read:"));
                Serial.println(batLiionType);
            }
        } else if (strcmp_P(name,TIME_ZONE_pm)== 0){
            //convert  str to num with error checking
            long time_zone_local;
            char *endptr;
            errno=0;
            time_zone_local = strtoul(value,&endptr,10);    
            if ((time_zone_local < 13) && (time_zone_local> -13) &&(errno!=ERANGE) ) {
                Serial.print(F("COMMON Set TimeZone ; "));
                timeZone=time_zone_local;
            } else {
                Serial.print(F("COMMON Set TimeZone error; (range -12 : +12) read:"));     
            }
            Serial.println(time_zone_local);           
        } else {
            Serial.print(F("COMMON tbd "));
            Serial.print(name);
            Serial.print(F(" to "));  
            Serial.println(value);  
        }
    } else if (strcmp_P(section,NETWORK_pm)== 0) {
        if (strcmp_P(name,apn_pm)== 0) {
            Serial.print(F("NETWORK APN: was '"));
            Serial.print(modemPhy.getApn());
            modemPhy.setWiFiId(value,true);
            Serial.print(F("' now '"));
            Serial.print(modemPhy.getApn());
            Serial.println("'");            
        } else if (strcmp_P(name,WiFiId_pm)== 0)  {
            Serial.print(F("NETWORK WiFiId: was '"));
            Serial.print(modemPhy.getWiFiId());
            modemPhy.setWiFiId(value,true);
            Serial.print(F("' now '"));
            Serial.print(modemPhy.getWiFiId());
            Serial.println("'");
        } else if (strcmp_P(name,WiFiPwd_pm)== 0) {
            Serial.print(F("NETWORK WiFiPwd: was '"));
            Serial.print(modemPhy.getWiFiPwd());
            modemPhy.setWiFiPwd(value,true);
            Serial.print(F("' now '"));
            Serial.print(modemPhy.getWiFiPwd());
            Serial.println("'");
        } else {
            Serial.print(F("NETWORK tbd "));
            Serial.print(name);
            Serial.print(F(" to "));  
            Serial.println(value);  
        }
    } else if (strcmp_P(section,BOOT_pm)== 0) 
    {
        #if 0
        //FUT: needs to go into EEPROM
        if (strcmp_P(name,VER_pm)== 0) {
            strcpy(ps.provider.s.registration_token, value);
        } else
        const char VER_pm[] EDIY_PROGMEM = "VER";
const char MAYFLY_SN_pm[] EDIY_PROGMEM = "MAYFLY_SN"; 
const char MAYFLY_REV_pm[] EDIY_PROGMEM = "MAYFLY_REV";
const char MAYFLY_INIT_ID_pm[] EDIY_PROGMEM = "MAYFLY_INIT_ID";
        #endif  
        if (strcmp_P(name,MAYFLY_SN_pm)== 0) {
            //FUT: needs to go into EEPROM
            //strcpy(ps.hw_boot.s.serial_num, value);
            //MFsn_def
            //FUT needs to be checked for sz
            Serial.print(F("Mayfly SerialNum :"));
            Serial.println(value);
#if 0
//Need to use to update EEPROM. Can cause problems if wrong. 
        } else if (strcmp_P(name,MAYFLY_REV_pm)== 0) {
            //FUT: needs to go into EEPROM
            //strcpy(ps.hw_boot.s.rev, value);
            //FUT needs to be checked for sz
            strcpy(MFVersion, value); //won't work with mcuBoardVersion
            Serial.print(F("Mayfly Rev:"));
            Serial.println(mcuBoardVersion);
#endif //
        } else
        {
            Serial.print(F("BOOT tbd "));
            Serial.print(name);
            Serial.print(F(" to "));  
            Serial.println(value);
        }  
    } else
    {
        Serial.print(F("Not supported ["));
        Serial.print(section);
        Serial.println(F("] "));
        Serial.print(name);
        Serial.print(F("="));  
        Serial.println(value);  
    }
    #if RAM_REPORT_LEVEL > 1
    if (ram_track) RAM_AVAILABLE;
    #endif //RAM_REPORT_LEVEL
    return 1;
}
#endif //USE_SD_MAYFLY_INI
// ==========================================================================

//#define mfSLEEP_TEST
#ifdef mfSLEEP_TEST
void sensorsSleep()
{
  //Add any code which your sensors require before sleep
}
void mfSystemSleep()
{
  //This method handles any sensor specific sleep setup
  sensorsSleep();
  
  //Wait until the serial ports have finished transmitting
  Serial.flush();
  //Serial1.flush();
#if defined ARDUINO_ARCH_SAMD
#elif defined ARDUINO_ARCH_AVR
  //The next timed interrupt will not be sent until this is cleared
  rtc.clearINTStatus();

  //Disable ADC
  ADCSRA &= ~_BV(ADEN);
  #endif 
  //Sleep time
  noInterrupts();
  sleep_enable();
  interrupts();
  sleep_cpu();
  sleep_disable();

#if defined ARDUINO_ARCH_SAMD
#elif defined ARDUINO_ARCH_AVR
  //Enbale ADC
  ADCSRA |= _BV(ADEN);
#endif  
  //This method handles any sensor specific wake setup
 // sensorsWake();
}

#endif //mfSLEEP_TEST

// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    bool LiBattPower_Unseable;
    uint16_t lp_wait=1;
    //ADCSRA |= _BV(ADEN);
    //uint8_t mcu_status = MCUSR; is already cleared by Arduino startup???
    //MCUSR = 0; //reset for unique read
    // Start the primary serial connection
    Serial.begin(serialBaud);
    Serial.print(F("---Boot. Build date:")); 
    Serial.print(build_date);
    //Serial.write('/');
    //Serial.print(build_epochTime,HEX);
    //Serial.print(__TIMESTAMP__); //still a ASC string Tue Dec 04 19:47:20 2018

    //MCUSR Serial.println(mcu_status,HEX);
    Serial.println(file_name); //Dir and filename
    Serial.print(F("Mayfly "));
    Serial.print(mcuBoardVersion);
    ramAvailable();

    // A vital check on power availability
    do {
        LiBattPower_Unseable = ((PS_LBATT_UNUSEABLE_STATUS == mcuBoard.isBatteryStatusAbove(true,PS_PWR_LOW_REQ))?true:false);
        if (LiBattPower_Unseable)
        {
            /* Sleep 
            * If can't collect data wait for more power to accumulate.
            * This sleep appears to taking 5mA, where as later sleep takes 3.7mA
            * Under no other load conditions the mega1284 takes about 35mA
            * Another issue is that onstartup currently requires turning on comms device to set it up.
            * On an XbeeS6 WiFi this can take 20seconds for some reason.
            */
            #if 1//defined(CHECK_SLEEP_POWER)
            Serial.print(lp_wait++);
            Serial.print(F(": BatteryLow-Sleep60sec, BatV="));
            Serial.println(mcuBoard.getBatteryVm1(false));
            #endif //(CHECK_SLEEP_POWER)
            //delay(59000); //60Seconds
            //if(_mcuWakePin >= 0){systemSleep();}
            dataLogger.systemSleep(); 
            Serial.println(F("----Wakeup"));
        }
    } while (LiBattPower_Unseable); 
    MS_DBG(F("Good BatV="),mcuBoard.getBatteryVm1(false));
    /////// Measured LiIon voltage is good enough to start up

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
    modemSetup=false;
    modemSerial.begin(ModemBaud);

#if !defined(CONFIG_SENSOR_RS485_PHY)
    // Start the stream for the modbus sensors; all currently supported modbus sensors use 9600 baud
    modbusSerial.begin(9600);
#else
    //Move to LoggerBase setup??
    digitalWrite(RS485PHY_TX_PIN, LOW);   // Reset AltSoftSerial Tx pin to LOW
    digitalWrite(RS485PHY_RX_PIN, LOW);   // Reset AltSoftSerial Rx pin to LOW
#endif

    // Start the SoftwareSerial stream for the sonar; it will always be at 9600 baud
    //sonarSerial.begin(9600);

    // Assign pins SERCOM functionality for SAMD boards
    // NOTE:  This must happen *after* the begin
    #if defined(ARDUINO_ARCH_SAMD)
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

#ifdef USE_SD_MAYFLY_INI
    PRINTOUT(F("---parseIni "));
    dataLogger.parseIniSd(configIniID,inihUnhandledFn);
#endif //USE_SD_MAYFLY_INI

#if 0
    Serial.print(F(" .ini-Logger:"));
    Serial.println(ps.msc.s.logger_id[0]);
    Serial.println(F(" List of UUIDs"));
    uint8_t i_lp;
    for (i_lp=0;i_lp<variableCount;i_lp++)
    {
        Serial.print(F("["));
        Serial.print(i_lp);
        Serial.print(F("] "));
        Serial.println(variableList[i_lp]->getVarUUID() );
    }
    //Serial.print(F("sF "))
    Serial.print(samplingFeature);
    Serial.print(F("/"));
    Serial.println(ps.provider.s.sampling_feature);
#endif //1
    //List PowerManagementSystem LiIon Bat thresholds

    mcuBoard.printBatteryThresholds();

#if 0
    uint8_t psilp,psolp;
    for (psolp=0; psolp<PSLR_NUM;psolp++) {
        Serial.print(psolp);
        Serial.print(F(": "));
        for (psilp=0; psilp<PS_LPBATT_TBL_NUM;psilp++) {
            Serial.print(mayflyPhy.PS_LBATT_TBL[psolp][psilp]);
            Serial.print(F(", "));
        }
        Serial.println();
    }
#endif //0

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
    #if defined(TINY_GSM_MODEM_SIM800) && defined(SIM800_GPRSBEE_R6)  // ONLY FOR GPRSBee R6!!!!
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

    Serial.print(F("Current Time: "));
    Serial.println(Logger::formatDateTime_ISO8601(dataLogger.getNowEpoch()+(timeZone*60)) );
#if 0
 //FUT: njh now part of Move to loggerModem???
    // Set up the sleep/wake pin for the modem and put its inital value as "off"
    Serial.println(F("Setting up sleep mode on the XBee."));
    pinMode(modemSleepRqPin, OUTPUT);
    digitalWrite(modemSleepRqPin, LOW);  // Turn it on to talk, just in case
    tinyModem->init();  // initialize
    if (tinyModem->commandMode())
    {
        tinyModem->sendAT(F("SM"),1);  // Pin sleep
        tinyModem->waitResponse();
        tinyModem->sendAT(F("DO"),0);  // Disable remote manager
        tinyModem->waitResponse();
        tinyModem->sendAT(F("SO"),0);  // For Cellular - disconnected sleep
        tinyModem->waitResponse();
        tinyModem->sendAT(F("SO"),200);  // For WiFi - Disassociate from AP for Deep Sleep
        tinyModem->waitResponse();
        tinyModem->writeChanges();
        tinyModem->exitCommand();
    }
    digitalWrite(modemSleepRqPin, HIGH);  // back to sleep
#endif
    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Attach the modem and information pins to the logger
    //ramAvailable();
    dataLogger.attachModem(modemPhy);
    dataLogger.setAlertPin(greenLED);
    dataLogger.setTestingModePin(buttonPin);

    // Begin the logger
    //modemPhy.modemPowerUp();
    dataLogger.begin(true);

    // Set up XBee later on first access
    #if 0 //defined(TINY_GSM_MODEM_XBEE)
    Serial.println(F("Setting up sleep mode on the XBee."));
    modemPhy.modemPowerUp();
    modemPhy.wake();  // Turn it on to talk
    setupXBee();
    #endif
#if 0
    Serial.print(F("WiFiId: '"));
    Serial.print(modemPhy.getWiFiId());
    Serial.print(F("' WiFiPwd: '"));
    Serial.print(modemPhy.getWiFiPwd());
    Serial.print("' ");
#endif //

    // Call the processor sleep
    dataLogger.systemSleep();
}


// **************************************************************************
// processSensors function
// **************************************************************************

void processSensors()
{

    // Set sensors and file up if it hasn't happened already
    // NOTE:  Unless it completed in less than one second, the sensor set-up
    // will take the place of logging for this interval!
    dataLogger.setupSensorsAndFile();

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (dataLogger.checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        //Logger::isLoggingNow = true;

        if (PS_LBATT_UNUSEABLE_STATUS==mcuBoard.isBatteryStatusAbove(true,PS_PWR_USEABLE_REQ)) {
            MS_DBG(F("---NewReading CANCELLED--Lbatt_V="));
            MS_DBG(mcuBoard.getBatteryVm1(false));
            MS_DBG("\n");
            return;
        }
        // Print a line to show new reading
        PRINTOUT(F("---NewReading-----------------------------"));
        MS_DBG(F("Lbatt_V="),mcuBoard.getBatteryVm1(false));
        //PRINTOUT(F("----------------------------\n"));
        #if !defined(CHECK_SLEEP_POWER)
        // Turn on the LED to show we're taking a reading
        //digitalWrite(greenLED, HIGH);
        // Turn on the LED to show we're taking a reading
        dataLogger.alertOn();

        // Start the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.begin(9600);

        // Do a complete sensor update
        //MS_DBG(F("    Running a complete sensor update...\n"));
        //_internalArray->completeUpdate();
        varArray.completeUpdate();

        // End the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.end();
        // Reset AltSoftSerial pins to LOW, to reduce power bleed on sleep, 
        // because Modbus Stop bit leaves these pins HIGH
        digitalWrite( RS485PHY_TX_PIN, LOW);   // Reset AltSoftSerial Tx pin to LOW
        digitalWrite( RS485PHY_RX_PIN, LOW);   // Reset AltSoftSerial Rx pin to LOW

        // Create a csv data record and save it to the log file
        dataLogger.logToSD();
         // Turn on the modem to let it start searching for the network

        //if Modem  is Cellular then PS_PWR_HEAVY_REQ
        if (PS_LBATT_UNUSEABLE_STATUS==mcuBoard.isBatteryStatusAbove(false,PS_PWR_MEDIUM_REQ)) 
        {          
            MS_DBG(F("---NewCloud Update CANCELLED---\n"));
        } else 
        {
            //if (dataLogger._logModem != NULL)
            {
                modemPhy.modemPowerUp();
                if (!modemSetup) {
                    modemSetup = true;
                    MS_DBG(F("  Modem setup up 1st pass\n"));
                    // The first time thru, setup modem. Can't do it in regular setup due to potential power drain.
                    modemPhy.wake();  // Turn it on to talk
                    setupXBee();
                    if (dataLogger.getNowEpoch() < 1545091200) {  /*Before 12/18/2018*/
                        PRINTOUT(F("  timeSync on startup "));
                        //dataLogger.setRTClock(dataLogger._logModem->getNISTTime());
                        dataLogger.syncRTC();
                    }
                }
                // Connect to the network
                MS_DBG(F("  Connecting to the Internet...\n"));
                if (modemPhy.connectInternet())
                {
                    MS_DBG(F("  sending..\n"));
                    // Post the data to the WebSDL
                    dataLogger.sendDataToRemotes();

                    // Sync the clock at midnight
                    if (Logger::markedEpochTime != 0 && Logger::markedEpochTime % 86400 == 0)
                    {
                        MS_DBG(F("  Running a daily clock sync..."));
                        //dataLogger.setRTClock(dataLogger. _logModem->getNISTTime());
                        dataLogger.syncRTC();
                    }

                    // Disconnect from the network
                    MS_DBG(F("  Disconnecting from the Internet...\n"));
                    modemPhy.disconnectInternet();
                } else {MS_DBG(F("  No internet connection...\n"));}
                // Turn the modem off
                modemPhy.modemSleepPowerDown();
            } //else MS_DBG(F("  No Modem configured.\n"));
            PRINTOUT(F("---Complete-------------------------------\n"));
        }
        // Turn off the LED
        //digitalWrite(greenLED, LOW);
        dataLogger.alertOff();
        // Print a line to show reading ended

        #endif //(CHECK_SLEEP_POWER)
        // Unset flag
        //Logger::isLoggingNow = false;
    }
}

// ==========================================================================
void loop()
{
    processSensors();
    // Check if it was instead the testing interrupt that woke us up
    // not implemented yet: if (EnviroDIYLogger.startTesting) EnviroDIYLogger.testingMode();

    // Sleep
    //if(_mcuWakePin >= 0){systemSleep();}
    dataLogger.systemSleep();
#if defined(CHECK_SLEEP_POWER)
    PRINTOUT(F("A"));
#endif //(CHECK_SLEEP_POWER)
}
