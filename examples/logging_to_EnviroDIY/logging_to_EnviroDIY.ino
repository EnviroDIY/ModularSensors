/*****************************************************************************
logging_to_EnviroDIY.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.17.0

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
#include "mayfly_routing.h"
#define DEBUGGING_SERIAL_OUTPUT Serial
// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "logging_to_EnviroDIY.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "nh07k";
const char *MayflyIniID = "mayfly.ini";
// How frequently (in minutes) to log data
//const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -8;
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

const char compile_date[] = __DATE__ " " __TIME__;
const char file_name[] = __FILE__;

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
const int8_t sensorPowerPin = 22; // MCU pin controlling main sensor power (-1 if not applicable)

// Create and return the processor "sensor"
#if !defined(BOARD_NAME)
const char *MFVersion = "v0.5b";
#endif
ProcessorStats mayflyPhy(MFVersion);
//#define CHECK_SLEEP_POWER

// ==========================================================================
//    Modem/Internet connection options
// ==========================================================================

// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#if !defined(CHECK_SLEEP_POWER)
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's
#endif //CHECK_SLEEP_POWER
// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

 // Set the serial port for the modem - software serial can also be used.
HardwareSerial &ModemSerial = Serial1;
#define RS485PHY_TX 5  // AltSoftSerial Tx pin 
#define RS485PHY_RX 6  // AltSoftSerial Rx pin

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
#define DEBUG_STREAMDEBUGGER
#if !defined(DEBUG_STREAMDEBUGGER)
TinyGsm *tinyModem = new TinyGsm(ModemSerial);
#endif //DEBUG_STREAMDEBUGGER

// Use this to create a modem if you want to spy on modem communication through
// a secondary Arduino stream.  Make sure you install the StreamDebugger library!
// https://github.com/vshymanskyy/StreamDebugger
#ifdef DEBUG_STREAMDEBUGGER
 #include <StreamDebugger.h>
 StreamDebugger modemDebugger(Serial1, Serial);
 TinyGsm *tinyModem = new TinyGsm(modemDebugger);
#endif //DEBUG_STREAMDEBUGGER
// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);

#if defined(TINY_GSM_MODEM_XBEE)
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 9600;        // Communication speed of the modem
const int8_t modemVccPin = -2;      // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = LOW;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
// After enabling pin sleep, the sleep request pin is held LOW to keep the XBee on
// Enable pin sleep in the setup function or using XCTU prior to connecting the XBee
bool sleepFxn(void)
{
    if (modemSleepRqPin >= 0)  // Don't go to sleep if there's not a wake pin!
    {
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
        Serial.println(F("sleepFxnH"));
        return true;
    }
    Serial.println(F("sleepFxn!"));
    return true;
}
bool wakeFxn(void)
{
    if (modemVccPin >= 0){  // Turns on when power is applied
        Serial.print(F("wakeFxnV!="));
        Serial.println(modemVccPin);
        return true;
    }else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);  // Because the XBee doesn't have any lights
        Serial.println(F("wakeFxnL"));
        return true;
    }
    Serial.print(F("wakeFxn!"));
    return true;
}

#elif defined(TINY_GSM_MODEM_ESP8266)
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 57600;        // Communication speed of the modem
const int8_t modemVccPin = -2;       // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemResetPin = -1;     // MCU pin connected to ESP8266's RSTB pin (-1 if unconnected)
const int8_t espSleepRqPin = 13;     // ESP8266 GPIO pin used for wake from light sleep (-1 if not applicable)
const int8_t modemSleepRqPin = 19;   // MCU pin used for wake from light sleep (-1 if not applicable)
const int8_t espStatusPin = -1;      // ESP8266 GPIO pin used to give modem status (-1 if not applicable)
const int8_t modemStatusPin = -1;    // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool sleepFxn(void)
{
    // Use this if you have an MCU pin connected to the ESP's reset pin to wake from deep sleep
    if (modemResetPin >= 0)
    {
        digitalWrite(redLED, LOW);
        return tinyModem->poweroff();
    }
    // Use this if you have GPIO16 connected to the reset pin to wake from deep sleep
    // but no other MCU pin connected to the reset pin.
    // NOTE:  This will NOT work nicely with things like "testingMode" and the
    // initial 2-minute logging interval at boot up.
    // if (loggingInterval > 1)
    // {
    //     uint32_t sleepSeconds = (((uint32_t)loggingInterval) * 60 * 1000) - 75000L;
    //     String sleepCommand = String(sleepSeconds);
    //     tinyModem->sendAT(F("+GSLP="), sleepCommand);
    //     // Power down for 1 minute less than logging interval
    //     // Better:  Calculate length of loop and power down for logging interval - loop time
    //     return tinyModem->waitResponse() == 1;
    // }
    // Use this if you don't have access to the ESP8266's reset pin for deep sleep but you
    // do have access to another GPIO pin for light sleep.  This also sets up another
    // pin to view the sleep status.
    else if (modemSleepRqPin >= 0 && modemStatusPin >= 0)
    {
        tinyModem->sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0,"),
                          String(espStatusPin), F(","), modemStatusLevel);
        bool success = tinyModem->waitResponse() == 1;
        tinyModem->sendAT(F("+SLEEP=1"));
        success &= tinyModem->waitResponse() == 1;
        digitalWrite(redLED, LOW);
        return success;
    }
    // Light sleep without the status pin
    else if (modemSleepRqPin >= 0 && modemStatusPin < 0)
    {
        tinyModem->sendAT(F("+WAKEUPGPIO=1,"), String(espSleepRqPin), F(",0"));
        bool success = tinyModem->waitResponse() == 1;
        tinyModem->sendAT(F("+SLEEP=1"));
        success &= tinyModem->waitResponse() == 1;
        digitalWrite(redLED, LOW);
        return success;
    }
    else return true;  // DON'T go to sleep if we can't wake up!
}
bool wakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
    {
        digitalWrite(redLED, HIGH);  // Because the ESP8266 doesn't have any lights
        return true;
    }
    else if (modemResetPin >= 0)
    {
        digitalWrite(modemResetPin, LOW);
        delay(1);
        digitalWrite(modemResetPin, HIGH);
        digitalWrite(redLED, HIGH);
        return true;
    }
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        delay(1);
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, HIGH);
        return true;
    }
    else return true;
}

#elif defined(TINY_GSM_MODEM_UBLOX)
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 9600;         // Communication speed of the modem
const int8_t modemVccPin = 23;       // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 20;   // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;    // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool sleepFxn(void)
{
    if (modemVccPin >= 0 && modemSleepRqPin < 0)
        return tinyModem->poweroff();
    else if (modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);  // A light to watch to verify pulse timing
        delay(1100);  // >1s pulse for power down
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
        return true;
    }
    else return true;  // DON'T go to sleep if we can't wake up!
}
bool wakeFxn(void)
{
    if (modemVccPin >= 0)  // Turns on when power is applied
        return true;
    else if(modemSleepRqPin >= 0)
    {
        digitalWrite(modemSleepRqPin, LOW);
        digitalWrite(redLED, HIGH);
        delay(200); // 0.15-3.2s pulse for wake on SARA R4/N4
        // delayMicroseconds(65); // 50-80µs pulse for wake on SARA/LISA U2/G2
        digitalWrite(modemSleepRqPin, HIGH);
        digitalWrite(redLED, LOW);
        return true;
    }
    else return true;
}

#else
// Describe the physical pin connection of your modem to your board
const long ModemBaud = 9600;         // Communication speed of the modem
const int8_t modemVccPin = -2;       // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;   // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;    // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)
// And create the wake and sleep methods for the modem
// These can be functions of any type and must return a boolean
bool wakeFxn(void)
{
    digitalWrite(modemSleepRqPin, HIGH);
    digitalWrite(redLED, HIGH);  // A light just for show
    return true;
}
bool sleepFxn(void)
{
    digitalWrite(modemSleepRqPin, LOW);
    digitalWrite(redLED, LOW);
    return true;
}
#endif

// And we still need the connection information for the network
#if !defined(BOARD_NAME)
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define SENSOR_CONFIG_KELLER_ACCULEVEL 1
#define SENSOR_CONFIG_KELLER_NANOLEVEL 1
#endif
// Create the loggerModem instance
#include <LoggerModem.h>
// A "loggerModem" is a combination of a TinyGSM Modem, a Client, and functions for wake and sleep
#if defined(TINY_GSM_MODEM_ESP8266)
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, wifiId, wifiPwd);
#elif defined(TINY_GSM_MODEM_XBEE)
loggerModem modemPhy(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, wifiId, wifiPwd);
// loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, apn);
#elif defined(TINY_GSM_MODEM_UBLOX)
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, apn);
#else
loggerModem modem(modemVccPin, modemStatusPin, modemStatusLevel, wakeFxn, sleepFxn, tinyModem, tinyClient, apn);
#endif


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);
// ==========================================================================
//    External Voltage via TI ADS1115
// ==========================================================================
#include <sensors/ExternalVoltage.h>
const int8_t VoltPower = -1;  // Pin to switch power on and off (-1 if unconnected)
const int8_t VoltData0 = 0;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
const int8_t VoltData1 = 1;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float VoltGain = 2; // Voltage divider 100K+100K Iternnaly 0-3.6V Default 1/gain for grove voltage divider is 10x
const uint8_t Volt_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
const uint8_t VoltReadsToAvg = 1; // Only read one sample
// Create and return the External Voltage sensor object
ExternalVoltage extvolt0(VoltPower, VoltData0, VoltGain, Volt_ADS1115Address, VoltReadsToAvg);
ExternalVoltage extvolt1(VoltPower, VoltData1, VoltGain, Volt_ADS1115Address, VoltReadsToAvg);

const int8_t I2CPower = -1;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SDI12Data = 7;  // The pin the 5TM is attached to
const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
#if defined(SENSOR_CONFIG_IA921)
// ==========================================================================
//    Ti INA219 High Side Current/Voltage Sensor (Current mA, Voltage, Power)
// ==========================================================================
#include <sensors/TiIna219.h>
//uint8_t INA219i2c_addr = 0x40; // 1000000 (Board A0+A1=GND)
// The INA219 can be addressed either as 0x40 (Adafruit default) or 0x41 44 45
// Either can be physically mofidied for the other address
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Bosch BME280 sensor object
TiIna219 ina219_phy(I2CPower);
#endif //SENSOR_CONFIG_IA921

#ifdef SENSOR_CONFIG_GENERAL
// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>
//const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the AOSong AM2315 sensor object
AOSongAM2315 am2315(I2CPower);


// ==========================================================================
//    AOSong DHT 11/21 (AM2301)/22 (AM2302) Digital Humidity and Temperature
// ==========================================================================
#include <sensors/AOSongDHT.h>
const int8_t DHTPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t DHTPin = 10;  // DHT data pin
DHTtype dhtType = DHT11;  // DHT type, either DHT11, DHT21, or DHT22
// Create and return the AOSong DHT sensor object
AOSongDHT dht(DHTPower, DHTPin, dhtType);


// ==========================================================================
//    Apogee SQ-212 Photosynthetically Active Radiation (PAR) Sensor
// ==========================================================================
#include <sensors/ApogeeSQ212.h>
const int8_t SQ212Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SQ212Data = 2;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
const uint8_t SQ212_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
// Create and return the Apogee SQ212 sensor object
ApogeeSQ212 SQ212(SQ212Power, SQ212Data);


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <sensors/BoschBME280.h>
uint8_t BMEi2c_addr = 0x76;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);


// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>
const int8_t OBS3Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3numberReadings = 10;
const uint8_t OBS3_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowPin = 0;  // The low voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSLow_A = 4.0749E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 9.1011E+01;  // The "B" value (X) from the low range calibration
const float OBSLow_C = -3.9570E-01;  // The "C" value from the low range calibration
// Create and return the Campbell OBS3+ LOW RANGE sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C, OBS3_ADS1115Address, OBS3numberReadings);
// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighPin = 1;  // The high voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSHigh_A = 5.2996E+01;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 3.7828E+02;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = -1.3927E+00;  // The "C" value from the high range calibration
// Create and return the Campbell OBS3+ HIGH RANGE sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C, OBS3_ADS1115Address, OBS3numberReadings);


// ==========================================================================
//    Decagon 5TM Soil Moisture Sensor
// ==========================================================================
#include <sensors/Decagon5TM.h>
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int8_t SDI12Data = 7;  // The pin the 5TM is attached to
const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Decagon 5TM sensor object
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);


// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDnumberReadings = 6;  // The number of readings to average
// const int8_t SDI12Data = 7;  // The pin the CTD is attached to
// const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Decagon CTD sensor object
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, CTDnumberReadings);


// ==========================================================================
//    Decagon ES2 Conductivity and Temperature Sensor
// ==========================================================================
#include <sensors/DecagonES2.h>
const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int8_t SDI12Data = 7;  // The pin the ES2 is attached to
// const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t ES2NumberReadings = 3;
// Create and return the Decagon ES2 sensor object
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data, ES2NumberReadings);


// ==========================================================================
//    External Voltage via TI ADS1115
// ==========================================================================
#include <sensors/ExternalVoltage.h>
const int8_t VoltPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t VoltData = 0;  // The data pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float VoltGain = 10; // Default 1/gain for grove voltage divider is 10x
const uint8_t Volt_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
const uint8_t VoltReadsToAvg = 1; // Only read one sample
// Create and return the External Voltage sensor object
ExternalVoltage extvolt(VoltPower, VoltData, VoltGain, Volt_ADS1115Address, VoltReadsToAvg);


// ==========================================================================
//    Freescale Semiconductor MPL115A2 Barometer
// ==========================================================================
#include <sensors/FreescaleMPL115A2.h>
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MPL115A2ReadingsToAvg = 1;
// Create and return the MPL115A2 barometer sensor object
MPL115A2 mpl115a2(I2CPower, MPL115A2ReadingsToAvg);


// ==========================================================================
//    Maxbotix HRXL Ultrasonic Range Finder
// ==========================================================================

// Set up a serial port for receiving sonar data - in this case, using software serial
// Because the standard software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts and define
// the interrrupts for it using the enableInterrup library.

// If enough hardware serial ports are available on your processor, you should
// use one of those instead.  If the proper pins are avaialbe, AltSoftSerial
// by Paul Stoffregen is also superior to SoftwareSerial for this sensor.
// Neither hardware serial nor AltSoftSerial require any modifications to
// deal with interrupt conflicts.

const int8_t SonarData = 11;     // data receive pin

#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts sonarSerial(SonarData, -1);  // No Tx pin is required, only Rx

// #include <NeoSWSerial.h>  // for the stream communication
// NeoSWSerial sonarSerial(SonarData, -1);  // No Tx pin is required, only Rx
// void NeoSWSISR()
// {
//   NeoSWSerial::rxISR( *portInputRegister( digitalPinToPort( SonarData ) ) );
// }

#include <sensors/MaxBotixSonar.h>
const int8_t SonarPower = 22;  // Excite (power) pin (-1 if unconnected)
const int8_t Sonar1Trigger = A1;  // Trigger pin (a negative number if unconnected) (A1 = 25)
const int8_t Sonar2Trigger = A2;  // Trigger pin (a negative number if unconnected) (A2 = 26)
// Create and return the MaxBotix Sonar sensor object
MaxBotixSonar sonar1(sonarSerial, SonarPower, Sonar1Trigger) ;
MaxBotixSonar sonar2(sonarSerial, SonarPower, Sonar2Trigger) ;


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
const int8_t OneWireBus = A0;  // Pin attached to the OneWire Bus (-1 if unconnected)
const int8_t OneWirePower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Maxim DS18 sensor objects (use this form for a known address)
MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
MaximDS18 ds18_2(OneWireAddress2, OneWirePower, OneWireBus);
MaximDS18 ds18_3(OneWireAddress3, OneWirePower, OneWireBus);
MaximDS18 ds18_4(OneWireAddress4, OneWirePower, OneWireBus);
MaximDS18 ds18_5(OneWireAddress5, OneWirePower, OneWireBus);
// Create and return the Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
// MaximDS18 ds18_u(OneWirePower, OneWireBus);


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <sensors/MeaSpecMS5803.h>
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 or 0x77
const int16_t MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;
// Create and return the MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);


// ==========================================================================
//    External I2C Rain Tipping Bucket Counter
// ==========================================================================
#include <sensors/RainCounterI2C.h>
const uint8_t RainCounterI2CAddress = 0x08;  // I2C Address for external tip counter
const float depthPerTipEvent = 0.2;  // rain depth in mm per tip event
// Create and return the Rain Counter sensor object
RainCounterI2C tbi2c(RainCounterI2CAddress, depthPerTipEvent);

#endif //SENSOR_CONFIG_GENERAL

// Set up a serial port for modbus communication - in this case, using AltSoftSerial
#include <AltSoftSerial.h>
AltSoftSerial modbusSerial;

const int8_t modbusSensorPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const int8_t rs485AdapterPower = 22;// Pin to switch RS485 adapter power on and off (-1 if unconnected)
// ==========================================================================
//    Keller Acculevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#ifdef SENSOR_CONFIG_KELLER_ACCULEVEL
#include <sensors/KellerAcculevel.h>
byte acculevelModbusAddress = 0x01;  // The modbus address of KellerAcculevel
//const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
//const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
//const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t acculevelNumberReadings = 5;  // The manufacturer recommends taking and averaging a few readings
// Create and return the Keller Acculevel sensor object
KellerAcculevel acculevel(acculevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, acculevelNumberReadings);
#endif //SENSOR_CONFIG_KELLER_ACCULEVEL

// ==========================================================================
//    Keller Nanolevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#ifdef SENSOR_CONFIG_KELLER_NANOLEVEL
#include <sensors/KellerNanolevel.h>
byte nanolevelModbusAddress = 0x01;  // The modbus address of KellerNanolevel
const uint8_t nanolevelNumberReadings = 3;  // The manufacturer recommends taking and averaging a few readings
// Create and return the Keller Nanolevel sensor object
KellerNanolevel nanolevelfn(nanolevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, nanolevelNumberReadings);
#endif //SENSOR_CONFIG_KELLER_NANOLEVEL
#ifdef SENSOR_CONFIG_GENERAL

// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/YosemitechY504.h>
byte y504ModbusAddress = 0x04;  // The modbus address of the Y504
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y504NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Yosemitech Y504 dissolved oxygen sensor object
YosemitechY504 y504(y504ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y504NumberReadings);


// ==========================================================================
//    Yosemitech Y510 Turbidity Sensor
// ==========================================================================
#include <sensors/YosemitechY510.h>
byte y510ModbusAddress = 0x0B;  // The modbus address of the Y510
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y510NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y510-B Turbidity sensor object
YosemitechY510 y510(y510ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y510NumberReadings);


// ==========================================================================
//    Yosemitech Y511 Turbidity Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY511.h>
byte y511ModbusAddress = 0x1A;  // The modbus address of the Y511
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y511NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y511-A Turbidity sensor object
YosemitechY511 y511(y511ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y511NumberReadings);


// ==========================================================================
//    Yosemitech Y514 Chlorophyll Sensor
// ==========================================================================
#include <sensors/YosemitechY514.h>
byte y514ModbusAddress = 0x14;  // The modbus address of the Y514
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y514NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y514 chlorophyll sensor object
YosemitechY514 y514(y514ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y514NumberReadings);


// ==========================================================================
//    Yosemitech Y520 Conductivity Sensor
// ==========================================================================
#include <sensors/YosemitechY520.h>
byte y520ModbusAddress = 0x20;  // The modbus address of the Y520
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y520NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y520 conductivity sensor object
YosemitechY520 y520(y520ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y520NumberReadings);


// ==========================================================================
//    Yosemitech Y532 pH
// ==========================================================================
#include <sensors/YosemitechY532.h>
byte y532ModbusAddress = 0x32;  // The modbus address of the Y532
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y532NumberReadings = 1;  // The manufacturer actually doesn't mention averaging for this one
// Create and return the Yosemitech Y532 pH sensor object
YosemitechY532 y532(y532ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y532NumberReadings);


// ==========================================================================
//    Yosemitech Y550 COD Sensor with Wiper
// ==========================================================================
#include <sensors/YosemitechY550.h>
byte y550ModbusAddress = 0x50;  // The modbus address of the Y550
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y550NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y550 conductivity sensor object
YosemitechY550 y550(y550ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y550NumberReadings);


// ==========================================================================
//    Yosemitech Y4000 Multiparameter Sonde (DOmgL, Turbidity, Cond, pH, Temp, ORP, Chlorophyll, BGA)
// ==========================================================================
#include <sensors/YosemitechY4000.h>
byte y4000ModbusAddress = 0x05;  // The modbus address of the Y4000
// const int8_t rs485AdapterPower = 22;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y4000NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Yosemitech Y4000 multi-parameter sensor object
YosemitechY4000 y4000(y4000ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y4000NumberReadings);


// ==========================================================================
//    Zebra Tech D-Opto Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/ZebraTechDOpto.h>
const char *DOptoDI12address = "5";  // The SDI-12 Address of the Zebra Tech D-Opto
// const int8_t SDI12Data = 7;  // The pin the D-Opto is attached to
// const int8_t SDI12Power = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create and return the Zebra Tech DOpto dissolved oxygen sensor object
ZebraTechDOpto dopto(*DOptoDI12address, SDI12Power, SDI12Data);
#endif //SENSOR_CONFIG_GENERAL

// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
//#pragma message( "including lib/ModularSensors/src/VariableBase.h")
//#include "lib/ModularSensors/src/VariableBase.h"
//#pragma message( "including <VariableArray.h>")
#include <VariableArray.h>
//#pragma message( "including VariableArray.h")
//#include "lib/ModularSensors/src/VariableArray.h"
// Create pointers for all of the variables from the sensors
// at the same time putting them into an array
Variable *variableList[] = {
#if defined(ProcessorStats_SampleNum_UUID)
    //Always have this first so can see on debug screen
    //new ProcessorStats_SampleNumber(&mayflyPhy,ProcessorStats_SampleNum_UUID),
    new ProcessorStats_SampleNumber(&mayflyPhy,"SampleNum_UUID"),
#endif
#if defined(ProcessorStats_Batt_UUID)
    //new ProcessorStats_Batt(&mayflyPhy,   ProcessorStats_Batt_UUID),
    new ProcessorStats_Batt(&mayflyPhy, "Batt_UUID"),
#endif
#if defined(Volt0_UUID)
    //new ExternalVoltage_Volt(&extvolt0, Volt0_UUID),
    new ExternalVoltage_Volt(&extvolt0, "Volt0_UUID"),
#endif
#if defined(Volt1_UUID)
    new ExternalVoltage_Volt(&extvolt1, "Volt1_UUID"),
#endif
#if defined(SENSOR_CONFIG_IA921)
    #if defined(INA219_MA_UUID)
    new TiIna219_mA  (&ina219_phy, "INA219_MA_UUID"),
    #endif
    #if defined(INA219_VOLT_UUID)
    new TiIna219_Volt(&ina219_phy, "INA219_VOLT_UUID"),
    #endif
#endif //SENSOR_CONFIG_IA921
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
#ifdef SENSOR_CONFIG_KELLER_ACCULEVEL
    new KellerAcculevel_Pressure(&acculevel, "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerAcculevel_Temp(&acculevel, "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerAcculevel_Height(&acculevel, "12345678-abcd-1234-efgh-1234567890ab"),
#endif // SENSOR_CONFIG_KELLER_ACCULEVEL
#ifdef SENSOR_CONFIG_KELLER_NANOLEVEL
//   new KellerNanolevel_Pressure(&nanolevelfn, "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerNanolevel_Temp(&nanolevelfn,   "KellerNanolevel_Temp_UUID"),
    new KellerNanolevel_Height(&nanolevelfn, "KellerNanolevel_Height_UUID"),
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
    new ProcessorStats_FreeRam(&mayflyPhy, "12345678-abcd-1234-efgh-1234567890ab"),
#endif // SENSOR_CONFIG_GENERAL
#if defined(MaximDS3231_Temp_UUID)
    new MaximDS3231_Temp(&ds3231,      "MaximDS3231_Temp_UUID"),
#endif //MaximDS3231_Temp_UUID
    //new Modem_RSSI(&modemPhy, "12345678-abcd-1234-efgh-1234567890ab"),
#if defined(Modem_SignalPercent_UUID)
    //new Modem_SignalPercent(&modemPhy, Modem_SignalPercent_UUID),
#endif
    // new YOUR_variableName_HERE(&)
};
// Count up the number of pointers in the array
const int variableCount = sizeof(variableList) / sizeof(variableList[0]);
// Create the VariableArray object
VariableArray varArray(variableCount, variableList);
//Need to define a ram place holder for UUIDs
#define MAX_UUID_SZ 38
char varUuid[variableCount][MAX_UUID_SZ];
//#include <array.h>
//std::array<String,variableCount> varUuid_arry;
//std::array<std::array<char,MAX_UUID_SZ>,variableCount> varUuid_array;

// Create a new logger instance
#include <LoggerEnviroDIY.h>
//#include "lib\ModularSensors\src\LoggerEnviroDIY.h"
LoggerEnviroDIY EnviroDIYLogger(LoggerID, loggingInterval, sdCardPin, wakePin, &varArray);


// ==========================================================================
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
// ==========================================================================
const char *registrationToken = registrationToken_UUID;   // Device registration token
const char *samplingFeature  =  samplingFeature_UUID;     // Sampling feature UUID


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
// ==========================================================================
// inihUnhandled 
// For any Unhandled sections this is called
// ==========================================================================

static int inihUnhandledFn( const char* section, const char* name,
                  const char* value)
{
    static char prev_section[50] = "";
    String currentUuid="tbd";
    if (strcmp("UUIDs",section)== 0)
    {
        int8_t value_idx = atoi(value); //or atol??
        if ((value_idx <= variableCount) && (value_idx>0) )
        {
            value_idx -= 1;
            currentUuid = variableList[value_idx]->getVarUUID();
            strcpy(&varUuid[value_idx][0], name);
            variableList[value_idx]->setVarUUID(&varUuid[value_idx][0]);
            #if 1
            //Serial.print(&varUuid[value_idx][0]-&varUuid[0][0]);
            Serial.print(F("["));
            Serial.print(value);
            Serial.print(F("]{"));
            //Serial.print(name);
            //Serial.print(F("}="));
            Serial.print(currentUuid);
            Serial.print(F("}-->"));
            Serial.println(variableList[value_idx]->getVarUUID() );
            #endif //
        } else {
            Serial.print(F("UUIDs idxError:"));
            Serial.println(value_idx);
        }
    } else if (strcmp("COMMON",section)== 0) {
        Serial.print(F("set "));
        Serial.print(name);
        Serial.print(F(" to "));  
        Serial.println(value);  

    } else {
        if (strcmp(section, prev_section)) {
            //printf("%s[%s]\n", (prev_section[0] ? "\n" : ""), section);
            if (prev_section[0] ) {
                Serial.println();
            } 
            Serial.print(F("["));
            Serial.print(section);
            Serial.println(F("]"));
            strncpy(prev_section, section, sizeof(prev_section));
            prev_section[sizeof(prev_section) - 1] = '\0';
        }
        //Serial.print(section);
        //printf("%s = %s\n", name, value);
        Serial.print(name);
        Serial.print(F("="));  
        Serial.println(value);  
    }
    return 1;
}
// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    //ADCSRA |= _BV(ADEN);
    //uint8_t mcu_status = MCUSR; is already cleared by Arduino startup???
    //MCUSR = 0; //reset for unique read
    // Start the primary serial connection
    Serial.begin(serialBaud);
    Serial.print(F("---Boot ")); 
    Serial.print(compile_date);
    extern int16_t __heap_start, *__brkval;
    uint16_t top_stack = (int) &top_stack  - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    Serial.print(F(" Ram available:"));
    Serial.println(top_stack );// Stack and heap ??
    //MCUSR Serial.println(mcu_status,HEX);
    Serial.println(file_name); //Dir and filename
    Serial.print(F("Mayfly Sn: "));
    Serial.print(MFsn);
    Serial.print(F(" "));
    Serial.print(MFVersion);
    Serial.print(F(" Logger:"));
    Serial.print(LoggerID);
    Serial.print(F("ModularSensors vers "));
    Serial.print(MODULAR_SENSORS_VERSION);
 
    // Start the serial connection with the modem
    ModemSerial.begin(ModemBaud);

#if !defined(SENSOR_RS485_PHY)
    modbusSerial.begin(9600);
#else
    digitalWrite(RS485PHY_TX, LOW);   // Reset AltSoftSerial Tx pin to LOW
    digitalWrite(RS485PHY_RX, LOW);   // Reset AltSoftSerial Rx pin to LOW
#endif

#ifdef SENSOR_CONFIG_GENERAL
    // Start the SoftwareSerial stream for the sonar
    sonarSerial.begin(9600);
#endif // SENSOR_CONFIG_GENERAL
    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
        enableInterrupt(SonarData, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif
    #if defined NeoSWSerial_h
        enableInterrupt(SonarData, NeoSWSISR, CHANGE);
    #endif

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Set up the sleep/wake pin for the modem and put its inital value as "off"
    #if defined(TINY_GSM_MODEM_XBEE)
        if(modemSleepRqPin >= 0) 
        {
            Serial.print(F("Setting up sleep mode on the XBee. "));
            Serial.println(modemSleepRqPin);
            pinMode(modemSleepRqPin, OUTPUT);
            digitalWrite(modemSleepRqPin, LOW);  // Turn it on to talk, just in case
        }
        if (tinyModem->commandMode())
        {
            tinyModem->sendAT(F("SM"),1);  // Pin sleep
            tinyModem->waitResponse();
            tinyModem->sendAT(F("DO"),0);  // Disable remote manager
            tinyModem->waitResponse();
            //tinyModem->sendAT(F("SO"),0);  // For Cellular - disconnected sleep
            //tinyModem->waitResponse();
        } else {
            PRINTOUT(F("Xbee Modem - not available! Not set to pin sleep"));
        }
        if(modemSleepRqPin >= 0) {
            digitalWrite(modemSleepRqPin, HIGH);  // back to sleep
        }
    #elif defined(TINY_GSM_MODEM_ESP8266)
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
    #elif defined(TINY_GSM_MODEM_UBLOX)
        pinMode(modemSleepRqPin, OUTPUT);
        digitalWrite(modemSleepRqPin, HIGH);
    #else
        pinMode(modemSleepRqPin, OUTPUT);
        digitalWrite(modemSleepRqPin, LOW);
    #endif

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);
#if !defined(CHECK_SLEEP_POWER)
    // Attach the modem and information pins to the logger
    EnviroDIYLogger.attachModem(modemPhy);
    EnviroDIYLogger.setAlertPin(greenLED);
    EnviroDIYLogger.setTestingModePin(buttonPin);

    // Enter the tokens for the connection with EnviroDIY
    EnviroDIYLogger.setToken(registrationToken);
    EnviroDIYLogger.setSamplingFeatureUUID(samplingFeature);

    // Begin the logger
    PRINTOUT(F("***beginAndNoSync "));
    EnviroDIYLogger.beginLogger();
    PRINTOUT(F("***parseIni "));
    EnviroDIYLogger.parseIniSd(MayflyIniID,inihUnhandledFn);
    #if 1
    Serial.println(F(" List of UUIDs"));
    uint8_t i_lp;
    for (i_lp=0;i_lp<variableCount;i_lp++)
    {
        Serial.print(F("["));
        Serial.print(i_lp);
        Serial.print(F("] "));
        Serial.println(variableList[i_lp]->getVarUUID() );
    }
#endif
    PRINTOUT(F("***timeSync "));
    EnviroDIYLogger.timeSync();
#if defined(TINY_GSM_MODEM_XBEE)
    wakeFxn();
    if (tinyModem->commandMode() )
    {
        PRINTOUT(F("IP number is "));
        tinyModem->sendAT(F("MY"));  // Request IP #
        tinyModem->waitResponse();
        if( XBEE_S6B_WIFI == tinyModem->getBeeType()) {
            MS_DBG(F("  Set XB WiFi\n"));
            // Cellular 3G Global SM yes, SO no
            // Cellular LTE-M SM yes, SO no
            // Cellular LTE CAT1 - SM yes, SO 0
            // WiFi S6B SM yes, SO yes
            //For WiFi AP  Bit4/0x140 Associate in sleep or default 0x100 Disassociate for Deep Sleep
            tinyModem->sendAT(F("SO"),100); //0X140 or 320 decimal
            tinyModem->waitResponse();
            tinyModem->writeChanges();
        }
        tinyModem->exitCommand();
    } else {
        PRINTOUT(F("nh: Check IP number. not in CMD modem!"));
    }
    sleepFxn();
    #endif //TINY_GSM_MODEM_XBEE
#endif //CHECK_SLEEP_POWER

}


// ==========================================================================
// Main loop function
// ==========================================================================
void processEverything()
{
    // Log the data
#if !defined(SENSOR_RS485_PHY)
    //older EnviroDIYLogger.logAndSend();
    EnviroDIYLogger.logDataAndSend();
#else //SENSOR_RS485_PHY

    // If the number of intervals is negative, then the sensors and file on
    // the SD card haven't been setup and we want to set them up.
    // NOTE:  Unless it completed in less than one second, the sensor set-up
    // will take the place of logging for this interval!
    #if 0
    if (EnviroDIYLogger._numIntervals < 0)
    {
        // Set up the sensors
        PRINTOUT(F("Sensors and data file had not been set up!  Setting them up now."));
        varArray.setupSensors();

       // Create the log file, adding the default header to it
       if (EnviroDIYLogger._autoFileName) generateAutoFileName();
       if (createLogFile(true)) PRINTOUT(F("Data will be saved as "), _fileName);
       else PRINTOUT(F("Unable to create a file to save data to!"));

       // Now, set the number of intervals to 0
       EnviroDIYLogger._numIntervals = 0;
    }
#endif
    // Reset AltSoftSerial pins to LOW, to reduce power bleed on sleep, 
    // because Modbus Stop bit leaves these pins HIGH

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (EnviroDIYLogger.checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        //Logger::isLoggingNow = true;

        // Print a line to show new reading
        PRINTOUT(F("---NewReading------------------------------\n"));

#if !defined(CHECK_SLEEP_POWER)
        // Turn on the LED to show we're taking a reading
        digitalWrite(greenLED, HIGH);

        // Start the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.begin(9600);

        // Do a complete sensor update
        MS_DBG(F("    Running a complete sensor update...\n"));
        //_internalArray->completeUpdate();
        varArray.completeUpdate();

        // End the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.end();
        // Reset AltSoftSerial pins to LOW, to reduce power bleed on sleep, 
        // because Modbus Stop bit leaves these pins HIGH
        digitalWrite( RS485PHY_TX, LOW);   // Reset AltSoftSerial Tx pin to LOW
        digitalWrite( RS485PHY_RX, LOW);   // Reset AltSoftSerial Rx pin to LOW

        // Create a csv data record and save it to the log file
        EnviroDIYLogger.logToSD();
         // Turn on the modem to let it start searching for the network
        //if (_logModem != NULL) _logModem->modemPowerUp();
        if (EnviroDIYLogger._logModem != NULL) modemPhy.modemPowerUp();
        //modemPhy.modemPowerUp();
        if (EnviroDIYLogger._logModem != NULL)
        {
            // Connect to the network
            MS_DBG(F("  Connecting to the Internet...\n"));
            if (modemPhy.connectInternet())
            {
                MS_DBG(F("  sending..\n"));
                // Post the data to the WebSDL
                EnviroDIYLogger.postDataEnviroDIY();

                // Sync the clock every 288 readings (1/day at 5 min intervals)
                //MS_DBG(F("  Running a daily clock sync...\n"));
                //if (_numTimepointsLogged % 288 == 0)
                //{
                //    EnviroDIYLogger.syncRTClock(_logModem->getNISTTime());
                //}

                // Disconnect from the network
                MS_DBG(F("  Disconnecting from the Internet...\n"));
                modemPhy.disconnectInternet();
            } else {MS_DBG(F("  No internet connection...\n"));}
            // Turn the modem off
            modemPhy.modemSleepPowerDown();
        } else MS_DBG(F("  No Modem configured.\n"));

        // Turn off the LED
        digitalWrite(greenLED, LOW);
        // Print a line to show reading ended
        PRINTOUT(F("---Complete-------------------------------\n\n"));
#endif //(CHECK_SLEEP_POWER)
        // Unset flag
        //Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (EnviroDIYLogger.startTesting) EnviroDIYLogger.testingMode();

    // Sleep
    //if(_mcuWakePin >= 0){systemSleep();}
    EnviroDIYLogger.systemSleep();
#if defined(CHECK_SLEEP_POWER)
    PRINTOUT(F("A"));
#endif //(CHECK_SLEEP_POWER)
#endif //SENSOR_RS485_PHY
}
// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    processEverything();
    #if 0
    for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
    #endif
}
