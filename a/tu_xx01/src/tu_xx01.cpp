/*****************************************************************************
tu_ctd.cpp
Based on examples/logging_to MMW.ino
Adapted by Matt Bartney
 and Neil Hancock
 Based on fork <tbd>
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2020, Trout Unlimited, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This shows most of the standard functions of the library at once.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Defines for the Arduino IDE
//    In PlatformIO, set these build flags in your platformio.ini
// ==========================================================================
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 64
#endif
#ifndef TINY_GSM_YIELD_MS
#define TINY_GSM_YIELD_MS 2
#endif
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 240
#endif

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include "ms_cfg.h"  //must be before ms_common.h & Arduino.h

// Use  MS_DBG()
#ifdef MS_TU_CTD_DEBUG
#undef MS_DEBUGGING_STD
#define MS_DEBUGGING_STD "tu_ctd"
#define MS_DEBUG_THIS_MODULE 1
#endif  // MS_TU_CTD_DEBUG

#ifdef MS_TU_CTD_DEBUG_DEEP
#undef MS_DEBUGGING_DEEP
#define MS_DEBUGGING_DEEP "tu_ctdD"
#undef MS_DEBUG_THIS_MODULE
#define MS_DEBUG_THIS_MODULE 2
#endif  // MS_TU_CTD_DEBUG_DEEP
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include <Arduino.h>          // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerBase.h>       // The modular sensors library
#if defined USE_PS_EEPROM
#include "EEPROM.h"
#endif  // USE_PS_EEPROM
#include "ms_common.h"

// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The name of this file
extern const String build_ref = __FILE__ " " __DATE__ " " __TIME__ " ";
#ifdef PIO_SRC_REV
const char git_branch[] = PIO_SRC_REV;
#else
const char git_branch[] = ".";
#endif

// Logger ID, also becomes the prefix for the name of the data file on SD card
// const char *LoggerID = "TU001";
const char* LoggerID        = LOGGERID_DEF_STR;
const char* configIniID_def = configIniID_DEF_STR;
// How frequently (in minutes) to log data
const uint8_t loggingInterval_def_min = loggingInterval_CDEF_MIN;

// How frequently (in minutes) to log data
const uint8_t loggingInterval = loggingInterval_CDEF_MIN;
// Your logger's timezone.
int8_t timeZone = CONFIG_TIME_ZONE_DEF;
// NOTE:  Daylight savings time will not be applied!  Please use standard time!

uint8_t collectReadings = COLLECT_READINGS_DEF;
uint8_t sendOffset_min = SEND_OFFSET_MIN_DEF;

// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long serialBaud =
    115200;  // Baud rate for the primary serial port for debugging
const int8_t greenLED  = 8;  // MCU pin for the green LED (-1 if not applicable)
const int8_t redLED    = 9;  // MCU pin for the red LED (-1 if not applicable)
const int8_t buttonPin = 21;  // MCU pin for a button to use to enter debugging
                              // mode  (-1 if not applicable)
const int8_t wakePin = A7;    // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin = -1;  // MCU SD card power pin (-1 if not applicable)
const int8_t sdCardSSPin =
    12;  // MCU SD card chip select/slave select pin (must be given!)
const int8_t sensorPowerPin =
    22;  // MCU pin controlling main sensor power (-1 if not applicable)

// Create the main processor chip "sensor" - for general metadata
const char*    mcuBoardVersion = "v0.5b";
ProcessorStats mcuBoard(mcuBoardVersion);

// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================

// The modem and a number of sensors communicate over UART/TTL - often called
// "serial". "Hardware" serial ports (automatically controlled by the MCU) are
// generally the most accurate and should be configured and used for as many
// peripherals as possible.  In some cases (ie, modbus communication) many
// sensors can share the same serial port.

#if defined(ARDUINO_ARCH_AVR) || defined(__AVR__)  // For AVR boards
// Unfortunately, most AVR boards have only one or two hardware serial ports,
// so we'll set up three types of extra software serial ports to use

// AltSoftSerial by Paul Stoffregen
// (https://github.com/PaulStoffregen/AltSoftSerial) is the most accurate
// software serial port for AVR boards. AltSoftSerial can only be used on one
// set of pins on each board so only one AltSoftSerial port can be used. Not all
// AVR boards are supported by AltSoftSerial. AltSoftSerial is capable of
// running up to 31250 baud on 16 MHz AVR. Slower baud rates are recommended
// when other code may delay AltSoftSerial's interrupt response.
// Pins In/Rx 6  Out/Tx=5
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerialPhy;

// NeoSWSerial (https://github.com/SRGDamia1/NeoSWSerial) is the best software
// serial that can be used on any pin supporting interrupts.
// You can use as many instances of NeoSWSerial as you want.
// Not all AVR boards are supported by NeoSWSerial.
#if 0
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
#endif
#if 0  // Not used
// The "standard" software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts.
// NOTE:  Only use if necessary.  This is not a very accurate serial port!
const int8_t softSerialRx = A3;     // data in pin
const int8_t softSerialTx = A4;     // data out pin

#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts softSerial1(softSerialRx, softSerialTx);
#endif
#endif  // End software serial for avr boards
// ==========================================================================
//    Wifi/Cellular Modem Settings
// ==========================================================================

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for
// Additional Serial Ports" section
HardwareSerial& modemSerial = Serial1;  // Use hardware serial if possible
// AltSoftSerial &modemSerial = altSoftSerialPhy;  // For software serial if
// needed NeoSWSerial &modemSerial = neoSSerial1;  // For software serial if
// needed Use this to create a modem if you want to monitor modem communication
// through a secondary Arduino stream.  Make sure you install the StreamDebugger
// library! https://github.com/vshymanskyy/StreamDebugger
#if defined STREAMDEBUGGER_DBG
#include <StreamDebugger.h>
StreamDebugger modemDebugger(modemSerial, STANDARD_SERIAL_OUTPUT);
#define modemSerHw modemDebugger
#else
#define modemSerHw modemSerial
#endif  // STREAMDEBUGGER_DBG

// Modem Pins - Describe the physical pin connection of your modem to your board
const int8_t modemVccPin =
    -2;  // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemStatusPin =
    19;  // MCU pin used to read modem status (-1 if not applicable)
const int8_t modemResetPin =
    20;  // MCU pin connected to modem reset pin (-1 if unconnected)
const int8_t modemSleepRqPin =
    23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemLEDPin = redLED;  // MCU pin connected an LED to show modem
                                    // status (-1 if unconnected)
const int8_t I2CPower = -1;  // sensorPowerPin;  // Pin to switch power on and
                             // off (-1 if unconnected)

#if defined UseModem_Module
// Network connection information
const char* apn_def =
    APN_CDEF;  // The APN for the gprs connection, unnecessary for WiFi
const char* wifiId_def =
    WIFIID_CDEF;  // The WiFi access point, unnecessary for gprs
const char* wifiPwd_def =
    WIFIPWD_CDEF;  // The password for connecting to WiFi, unnecessary for gprs
#endif             // UseModem_Module

#ifdef DigiXBeeCellularTransparent_Module
// For any Digi Cellular XBee's
// NOTE:  The u-blox based Digi XBee's (3G global and LTE-M global) can be used
// in either bypass or transparent mode, each with pros and cons
// The Telit based Digi XBees (LTE Cat1) can only use this mode.
#include <modems/DigiXBeeCellularTransparent.h>
const long modemBaud       = 9600;   // All XBee's use 9600 by default
const bool useCTSforStatus = false;  // Flag to use the XBee CTS pin for status
// NOTE:  If possible, use the STATUS/SLEEP_not (XBee pin 13) for status, but
// the CTS pin can also be used if necessary
DigiXBeeCellularTransparent modemXBCT(&modemSerHw, modemVccPin, modemStatusPin,
                                      useCTSforStatus, modemResetPin,
                                      modemSleepRqPin, apn_def);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBeeCellularTransparent modemPhy = modemXBCT;
#endif  // DigiXBeeCellularTransparent_Module

#ifdef DigiXBeeWifi_Module
// For the Digi Wifi XBee (S6B)

#include <modems/DigiXBeeWifi.h>
const long modemBaud = 9600;  // All XBee's use 9600 by default
const bool useCTSforStatus =
    true;  // true? Flag to use the XBee CTS pin for status
// NOTE:  If possible, use the STATUS/SLEEP_not (XBee pin 13) for status, but
// the CTS pin can also be used if necessary
// useCTSforStatus is overload with  useCTSforStatus!-> loggerModem.statusLevel
// for detecting Xbee SleepReqAct==1
DigiXBeeWifi modemXBWF(&modemSerHw, modemVccPin, modemStatusPin,
                       useCTSforStatus, modemResetPin, modemSleepRqPin,
                       wifiId_def, wifiPwd_def);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBeeWifi modemPhy = modemXBWF;
#endif  // DigiXBeeWifi_Module

#if 0
// ==========================================================================
//    Campbell OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>

const int8_t OBS3Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3NumberReadings = 10;
const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC
#endif  // 0
#if defined Decagon_CTD_UUID
// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>

const char*   CTDSDI12address   = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDNumberReadings = 6;    // The number of readings to average
const int8_t  SDI12Power =
    sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create a Decagon CTD sensor object
DecagonCTD ctdPhy(*CTDSDI12address, SDI12Power, SDI12Data, CTDNumberReadings);
#endif  // Decagon_CTD_UUID

#if defined Insitu_TrollSdi12_UUID
// ==========================================================================
//    Insitu Aqua/Level Troll Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/InsituTrollSdi12.h>

const char*   ITROLLSDI12address   = "1";  // The SDI-12 Address of the ITROLL
const uint8_t ITROLLNumberReadings = 2;    // The number of readings to average
const int8_t  IT_SDI12Power =
    sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t IT_SDI12Data = 7;  // The SDI12 data pin

// Create a  ITROLL sensor object
InsituTrollSdi12 itrollPhy(*ITROLLSDI12address, IT_SDI12Power, IT_SDI12Data,
                           ITROLLNumberReadings);
#endif  // Insitu_TrollSdi12_UUID

// ==========================================================================
//    Insitu Level/Aqua Troll High Accuracy Submersible Level Transmitter
// wip Tested for Level Troll 500
// ==========================================================================
#ifdef InsituLTrs485_ACT
#include <sensors/InsituTrollModbus.h>

const byte ltModbusAddress =
    InsituLTrs485ModbusAddress_DEF;  // The modbus address of InsituLTrs485
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485
// adapter power on and off (-1 if unconnected) const int8_t modbusSensorPower =
// A3;  // Pin to switch sensor power on and off (-1 if unconnected) const
// int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip
// (-1 if unconnected)
const uint8_t ltNumberReadings =
    3;  // The manufacturer recommends taking and averaging a few readings

// Create a Keller Nanolevel sensor object

InsituLevelTroll InsituLT_snsr(ltModbusAddress, modbusSerial, rs485AdapterPower,
                               modbusSensorPower, max485EnablePin,
                               ltNumberReadings);

// Create pressure, temperature, and height variable pointers for the Nanolevel
// Variable *nanolevPress =  new InsituLTrs485_Pressure(&InsituLT_snsr,
// "12345678-abcd-1234-efgh-1234567890ab"); Variable *nanolevTemp =   new
// InsituLTrs485_Temp(&InsituLT_snsr, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *nanolevHeight = new InsituLTrs485_Height(&InsituLT_snsr,
// "12345678-abcd-1234-efgh-1234567890ab");

#endif  // InsituLTrs485_ACT

// ==========================================================================
//    Electrical Conductivity using the processors analog pins
// ==========================================================================
#ifdef AnalogProcEC_ACT
#include <sensors/analogElecConductivity.h>
const int8_t           ECpwrPin   = ECpwrPin_DEF;
const int8_t           ECdataPin1 = ECdataPin1_DEF;
analogElecConductivity EC_procPhy(ECpwrPin, ECdataPin1);
#endif  // AnalogProcEC_ACT

// ==========================================================================
//    Keller Acculevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#if defined(KellerAcculevel_ACT) || defined(KellerNanolevel_ACT)
#define KellerXxxLevel_ACT 1
//#include <sensors/KellerAcculevel.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for
// Additional Serial Ports" section
#if defined SerialModbus && (defined ARDUINO_ARCH_SAMD || defined ATMEGA2560)
HardwareSerial& modbusSerial = SerialModbus;  // Use hardware serial if possible
#else
AltSoftSerial& modbusSerial =
    altSoftSerialPhy;  // For software serial if needed
// NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
#endif

// byte acculevelModbusAddress = KellerAcculevelModbusAddress;  // The modbus
// address of KellerAcculevel
const int8_t rs485AdapterPower =
    rs485AdapterPower_DEF;  // Pin to switch RS485 adapter power on and off (-1
                            // if unconnected)
const int8_t modbusSensorPower =
    modbusSensorPower_DEF;  // Pin to switch sensor power on and off (-1 if
                            // unconnected)
const int8_t max485EnablePin =
    max485EnablePin_DEF;  // Pin connected to the RE/DE on the 485 chip (-1 if
                          // unconnected)

const int8_t RS485PHY_TX_PIN  = CONFIG_HW_RS485PHY_TX_PIN;
const int8_t RS485PHY_RX_PIN  = CONFIG_HW_RS485PHY_RX_PIN;
const int8_t RS485PHY_DIR_PIN = CONFIG_HW_RS485PHY_DIR_PIN;

#endif  // defined KellerAcculevel_ACT  || defined KellerNanolevel_ACT

#if defined KellerAcculevel_ACT
#include <sensors/KellerAcculevel.h>

byte acculevelModbusAddress =
    KellerAcculevelModbusAddress_DEF;  // The modbus address of KellerAcculevel
const uint8_t acculevelNumberReadings =
    3;  // The manufacturer recommends taking and averaging a few readings

// Create a Keller Acculevel sensor object
KellerAcculevel acculevel_snsr(acculevelModbusAddress, modbusSerial,
                               rs485AdapterPower, modbusSensorPower,
                               max485EnablePin, acculevelNumberReadings);

// Create pressure, temperature, and height variable pointers for the Acculevel
// Variable *acculevPress = new KellerAcculevel_Pressure(&acculevel,
// "12345678-abcd-1234-efgh-1234567890ab"); Variable *acculevTemp = new
// KellerAcculevel_Temp(&acculevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *acculevHeight = new KellerAcculevel_Height(&acculevel,
// "12345678-abcd-1234-efgh-1234567890ab");
#endif  // KellerAcculevel_ACT


// ==========================================================================
//    Keller Nanolevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#ifdef KellerNanolevel_ACT
#include <sensors/KellerNanolevel.h>

byte nanolevelModbusAddress =
    KellerNanolevelModbusAddress_DEF;  // The modbus address of KellerNanolevel
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485
// adapter power on and off (-1 if unconnected) const int8_t modbusSensorPower =
// A3;  // Pin to switch sensor power on and off (-1 if unconnected) const
// int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip
// (-1 if unconnected)
const uint8_t nanolevelNumberReadings =
    3;  // The manufacturer recommends taking and averaging a few readings

// Create a Keller Nanolevel sensor object

KellerNanolevel nanolevel_snsr(nanolevelModbusAddress, modbusSerial,
                               rs485AdapterPower, modbusSensorPower,
                               max485EnablePin, nanolevelNumberReadings);

// Create pressure, temperature, and height variable pointers for the Nanolevel
// Variable *nanolevPress = new KellerNanolevel_Pressure(&nanolevel,
// "12345678-abcd-1234-efgh-1234567890ab"); Variable *nanolevTemp = new
// KellerNanolevel_Temp(&nanolevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *nanolevHeight = new KellerNanolevel_Height(&nanolevel,
// "12345678-abcd-1234-efgh-1234567890ab");

#endif  // KellerNanolevel_ACT

#if defined(ASONG_AM23XX_UUID)
// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>

// const int8_t I2CPower = 1;//sensorPowerPin;  // Pin to switch power on and
// off (-1 if unconnected)

// Create an AOSong AM2315 sensor object
// Data sheets says AM2315 and AM2320 have same address 0xB8 (8bit addr) of 1011
// 1000 or 7bit 0x5c=0101 1100 AM2320 AM2315 address 0x5C
AOSongAM2315 am23xx(I2CPower);

// Create humidity and temperature variable pointers for the AM2315
// Variable *am2315Humid = new AOSongAM2315_Humidity(&am23xx,
// "12345678-abcd-1234-ef00-1234567890ab"); Variable *am2315Temp = new
// AOSongAM2315_Temp(&am23xx, "12345678-abcd-1234-ef00-1234567890ab");
#endif  // ASONG_AM23XX_UUID


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#if 0
#include <sensors/BoschBME280.h>

const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t BMEi2c_addr = 0x76;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address

// Create a Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <sensors/MaximDS18.h>

// OneWire Address [array of 8 hex characters]
// If only using a single sensor on the OneWire bus, you may omit the address
// DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
const int8_t OneWirePower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t OneWireBus = 6;  // Pin attached to the OneWire Bus (-1 if unconnected) (D24 = A0)

// Create a Maxim DS18 sensor objects (use this form for a known address)
// MaximDS18 ds18(OneWireAddress1, OneWirePower, OneWireBus);

// Create a Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
MaximDS18 ds18(OneWirePower, OneWireBus);
#endif  // 0
#ifdef ExternalVoltage_ACT
// ==========================================================================
//    External Voltage via TI ADS1115
// ==========================================================================
#include <sensors/ExternalVoltage.h>

const int8_t ADSPower = 1;     // sensorPowerPin;  // Pin to switch power on and
                               // off (-1 if unconnected)
const int8_t ADSChannel0 = 0;  // The ADS channel of interest
const int8_t ADSChannel1 = 1;  // The ADS channel of interest
const int8_t ADSChannel2 = 2;  // The ADS channel of interest
const int8_t ADSChannel3 = 3;  // The ADS channel of interest
const float  dividerGain =
    2;  //  Default 1/gain for grove voltage divider is 10x  assumes Rev001
const uint8_t ADSi2c_addr    = 0x48;  // The I2C address of the ADS1115 ADC
const uint8_t VoltReadsToAvg = 1;     // Only read one sample

// Create an External Voltage sensor object
ExternalVoltage extvolt0(ADSPower, ADSChannel0, dividerGain, ADSi2c_addr,
                         VoltReadsToAvg);
// ExternalVoltage extvolt1(ADSPower, ADSChannel1, dividerGain, ADSi2c_addr,
// VoltReadsToAvg); special Vcc 3.3V
ExternalVoltage extvolt1(ADSPower, ADSChannel2, (const float)1.0, ADSi2c_addr,
                         VoltReadsToAvg);
// ExternalVoltage extvolt1(ADSPower, ADSChannel2, (const float)1.0,
// ADSi2c_addr, VoltReadsToAvg);

// Create a voltage variable pointer
// Variable *extvoltV = new ExternalVoltage_Volt(&extvolt,
// "12345678-abcd-1234-ef00-1234567890ab");
#endif  // ExternalVoltage_ACT
// ==========================================================================
// Units conversion functions
// ==========================================================================
#define SENSOR_T_DEFAULT_F -0.009999
float convertDegCtoF(float tempInput) {  // Simple deg C to deg F conversion
    if (-9999 == tempInput) return SENSOR_T_DEFAULT_F;
    if (SENSOR_T_DEFAULT_F == tempInput) return SENSOR_T_DEFAULT_F;
    return tempInput * 1.8 + 32;
}

float convertMmtoIn(float mmInput) {  // Simple millimeters to inches conversion
    if (-9999 == mmInput) return SENSOR_T_DEFAULT_F;
    if (SENSOR_T_DEFAULT_F == mmInput) return SENSOR_T_DEFAULT_F;
    return mmInput / 25.4;
}
// ==========================================================================
// Creating Variable objects for those values for which we're reporting in
// converted units, via calculated variables Based on baro_rho_correction.ino
// and VariableBase.h from enviroDIY.
// ==========================================================================
#if defined Decagon_CTD_UUID
// Create a temperature variable pointer for the Decagon CTD
Variable* CTDTempC = new DecagonCTD_Temp(&ctdPhy, "NotUsed");
float     CTDTempFgetValue(void) {  // Convert temp for the CTD
    return convertDegCtoF(CTDTempC->getValue());
}
// Create the calculated water temperature Variable object and return a pointer
// to it
Variable* CTDTempFcalc = new Variable(
    CTDTempFgetValue,     // function that does the calculation
    1,                    // resolution
    "temperatureSensor",  // var name. This must be a value from
                          // http://vocabulary.odm2.org/variablename/
    "degreeFahrenheit",   // var unit. This must be a value from This must be a
                          // value from http://vocabulary.odm2.org/units/
    "TempInF",            // var code
    CTD10_TEMP_UUID);

// Create a depth variable pointer for the Decagon CTD
Variable* CTDDepthMm = new DecagonCTD_Depth(&ctdPhy, "NotUsed");
float     CTDDepthInGetValue(void) {  // Convert depth for the CTD
    // Pass true to getValue() for the Variables for which we're only sending a
    // calculated version of the sensor reading; this forces the sensor to take
    // a reading when getValue is called.
    return convertMmtoIn(CTDDepthMm->getValue(true));
}
// Create the calculated depth Variable object and return a pointer to it
Variable* CTDDepthInCalc =
    new Variable(CTDDepthInGetValue,  // function that does the calculation
                 1,                   // resolution
                 "CTDdepth",          // var name. This must be a value from
                              // http://vocabulary.odm2.org/variablename/
                 "Inch",  // var unit. This must be a value from This must be a
                          // value from http://vocabulary.odm2.org/units/
                 "waterDepth",  // var code
                 CTD10_DEPTH_UUID);
#endif  // Decagon_CTD_UUID

#if defined MaximDS3231_TEMP_UUID || defined MaximDS3231_TEMPF_UUID
// Create a temperature variable pointer for the DS3231
#if defined MaximDS3231_TEMP_UUID
Variable*   ds3231TempC = new MaximDS3231_Temp(&ds3231, MaximDS3231_TEMP_UUID);
#else
Variable* ds3231TempC = new MaximDS3231_Temp(&ds3231);
#endif
float ds3231TempFgetValue(void) {  // Convert temp for the DS3231
    // Pass true to getValue() for the Variables for which we're only sending a
    // calculated version of the sensor reading; this forces the sensor to take
    // a reading when getValue is called.
    return convertDegCtoF(ds3231TempC->getValue(true));
}
#endif  // MaximDS3231_TEMP_UUID
// Create the calculated Mayfly temperature Variable object and return a pointer
// to it
#if defined MaximDS3231_TEMPF_UUID
Variable*   ds3231TempFcalc = new Variable(
    ds3231TempFgetValue,      // function that does the calculation
    1,                        // resolution
    "temperatureDatalogger",  // var name. This must be a value from
                              // http://vocabulary.odm2.org/variablename/
    "degreeFahrenheit",       // var unit. This must be a value from
                              // http://vocabulary.odm2.org/units/
    "TempInF",                // var code
    MaximDS3231_TEMPF_UUID);
#endif  // MaximDS3231_Temp_UUID


// ==========================================================================
//    Creating the Variable Array[s] and Filling with Variable Objects
// ==========================================================================

Variable* variableList[] = {
    new ProcessorStats_SampleNumber(&mcuBoard,
                                    ProcessorStats_SampleNumber_UUID),
    new ProcessorStats_Battery(&mcuBoard, ProcessorStats_Batt_UUID),
#if defined(ExternalVoltage_Volt0_UUID)
    new ExternalVoltage_Volt(&extvolt0, ExternalVoltage_Volt0_UUID),
#endif
#if defined(ExternalVoltage_Volt1_UUID)
    new ExternalVoltage_Volt(&extvolt1, ExternalVoltage_Volt1_UUID),
#endif
#if defined AnalogProcEC_ACT
    new analogElecConductivity_EC(&EC_procPhy, EC1_UUID),
#endif  // AnalogProcEC_ACT
#if defined Decagon_CTD_UUID
    // new DecagonCTD_Depth(&ctdPhy,CTD10_DEPTH_UUID),
    CTDDepthInCalc,
    // new DecagonCTD_Temp(&ctdPhy, CTD10_TEMP_UUID),
    CTDTempFcalc,
#endif  // Decagon_CTD_UUID
#if defined Insitu_TrollSdi12_UUID
    new InsituTrollSdi12_Depth(&itrollPhy, ITROLL_DEPTH_UUID),
    new InsituTrollSdi12_Temp(&itrollPhy, ITROLL_TEMP_UUID),
#endif  // Insitu_TrollSdi12_UUID
#if defined KellerAcculevel_ACT
    // new KellerAcculevel_Pressure(&acculevel,
    // "12345678-abcd-1234-ef00-1234567890ab"),
    new KellerAcculevel_Temp(&acculevel_snsr, KellerXxlevel_Temp_UUID),
    new KellerAcculevel_Height(&acculevel_snsr, KellerXxlevel_Height_UUID),
#endif  // KellerAcculevel_ACT
#if defined KellerNanolevel_ACT
    //   new KellerNanolevel_Pressure(&nanolevel_snsr,
    //   "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerNanolevel_Temp(&nanolevel_snsr, KellerXxlevel_Temp_UUID),
    new KellerNanolevel_Height(&nanolevel_snsr, KellerXxlevel_Height_UUID),
#endif  // SENSOR_CONFIG_KELLER_NANOLEVEL
#if defined InsituLTrs485_ACT
    //   new insituLevelTroll_Pressure(&InsituLT_snsr,
    //   "12345678-abcd-1234-efgh-1234567890ab"),
    new InsituLevelTroll_Temp(&InsituLT_snsr, InsituLTrs485_Temp_UUID),
    new InsituLevelTroll_Height(&InsituLT_snsr, InsituLTrs485_Height_UUID),
#endif  // SENSOR_CONFIG_KELLER_NANOLEVEL
// new BoschBME280_Temp(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
// new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
// new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
// new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
// new MaximDS18_Temp(&ds18, "12345678-abcd-1234-ef00-1234567890ab"),
#if defined ASONG_AM23XX_UUID
    new AOSongAM2315_Humidity(&am23xx, ASONG_AM23_Air_Humidity_UUID),
    new AOSongAM2315_Temp(&am23xx, ASONG_AM23_Air_Temperature_UUID),
// ASONG_AM23_Air_TemperatureF_UUID
// calcAM2315_TempF
#endif  // ASONG_AM23XX_UUID
#if defined DIGI_RSSI_UUID
    new Modem_RSSI(&modemPhy, DIGI_RSSI_UUID),
// new Modem_RSSI(&modemPhy, "12345678-abcd-1234-ef00-1234567890ab"),
#endif  // DIGI_RSSI_UUID
#if defined MaximDS3231_TEMP_UUID
    // new MaximDS3231_Temp(&ds3231,      MaximDS3231_Temp_UUID),
    ds3231TempC,
#endif  //  MaximDS3231_TEMP_UUID
#if defined MaximDS3231_TEMPF_UUID
    ds3231TempFcalc,
#endif  // MaximDS3231_TempF_UUID

};


// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);

// ==========================================================================
//     Local storage - evolving
// ==========================================================================
#ifdef USE_MS_SD_INI
persistent_store_t ps_ram;
#define epc ps_ram
#endif  //#define USE_MS_SD_INI

// ==========================================================================
//     The Logger Object[s]
// ==========================================================================

// Create a new logger instance
Logger dataLogger(LoggerID, loggingInterval, &varArray);


// ==========================================================================
//    A Publisher to Monitor My Watershed / EnviroDIY Data Sharing Portal
// ==========================================================================
// Device registration and sampling feature information can be obtained after
// registration at https://monitormywatershed.org or https://data.envirodiy.org
const char* registrationToken =
    registrationToken_UUID;  // Device registration token
const char* samplingFeature = samplingFeature_UUID;  // Sampling feature UUID

#if defined UseModem_Module
// Create a data publisher for the EnviroDIY/WikiWatershed POST endpoint
#include <publishers/EnviroDIYPublisher.h>
// EnviroDIYPublisher EnviroDIYPOST(dataLogger, &modemPhy.gsmClient,
// registrationToken, samplingFeature);
EnviroDIYPublisher EnviroDIYPOST(dataLogger, 15, 0);
#endif  // UseModem_Module
// ==========================================================================
//    Working Functions
// ==========================================================================
#define SerialStd Serial
#include "iniHandler.h"

// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, uint8_t rate = 75) {
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
float getBatteryVoltage() {
    if (mcuBoard.sensorValues[0] == -9999) mcuBoard.update();
    return mcuBoard.sensorValues[0];
}

// ==========================================================================
// Manages the Modbus Physical Pins.
// Pins pulled high when powered off will cause a ghost power leakage.
#if defined KellerXxxLevel_ACT
void        modbusPinPowerMng(bool status) {
    MS_DBG(F("  **** modbusPinPower"), status);
#if 1
    if (status) {
        modbusSerial.setupPhyPins();
    } else {
        modbusSerial.disablePhyPins();
    }
#endif
}
#endif  // KellerXxxLevel_ACT

#define PORT_SAFE(pinNum)   \
    pinMode(pinNum, INPUT); \
    digitalWrite(pinNum, LOW);
void unusedBitsMakeSafe() {
    // Set all unused Pins to a safe no current mode for sleeping
    // Mayfly variant.h: D0->23  (Analog0-7) or D24-31
    // PORT_SAFE( 0); Rx0  Tty
    // PORT_SAFE( 1); Tx0  TTy
    // PORT_SAFE( 2); Rx1  Xb?
    // PORT_SAFE( 3); Tx1  Xb?
#if !defined KellerXxxLevel_ACT
    PORT_SAFE(04);
    PORT_SAFE(05);
#endif  // KellerXxxLevel_ACT
    PORT_SAFE(06);
    // PORT_SAFE(07); SDI12
    // PORT_SAFE(08); Grn Led
    // PORT_SAFE(09); Red LED
    PORT_SAFE(10);  //?? RTC Int
    PORT_SAFE(11);
    PORT_SAFE(12);
    // mosi LED PORT_SAFE(13);
    // miso PORT_SAFE(14);
    // sck PORT_SAFE(15);
    // scl PORT_SAFE(16);
    // sda PORT_SAFE(17);
    PORT_SAFE(18);
    PORT_SAFE(19);  // Xbee CTS
    PORT_SAFE(20);  // Xbee RTS
    PORT_SAFE(21);
    // PORT_SAFE(22);  //Pwr Sw
    PORT_SAFE(23);  // Xbee DTR
    // Analog from here on
    // PORT_SAFE(24);//A0 ECData1
    PORT_SAFE(25);  // A1
    PORT_SAFE(26);  // A2
    PORT_SAFE(27);  // A3
    // PORT_SAFE(28); //A4  ECpwrPin
    PORT_SAFE(29);  // A5
    PORT_SAFE(30);  // A6
    // PORT_SAFE(31); //A7 Timer Int
};

// ==========================================================================
// Main setup function
// ==========================================================================
void setup() {
    // uint8_t resetCause = REG_RSTC_RCAUSE;        AVR ?//Reads from hw
    // uint8_t resetBackupExit = REG_RSTC_BKUPEXIT; AVR ?//Reads from hw
    bool     LiBattPower_Unseable;
    uint16_t lp_wait = 1;
// Wait for USB connection to be established by PC
// NOTE:  Only use this when debugging - if not connected to a PC, this
// could prevent the script from starting
#if defined SERIAL_PORT_USBVIRTUAL
    while (!SERIAL_PORT_USBVIRTUAL && (millis() < 10000)) {}
#endif

    // Start the primary serial connection
    Serial.begin(serialBaud);
    Serial.print(F("\n---Boot. Build: "));
    Serial.print(build_ref);
    Serial.print(" ");
    Serial.println(git_branch);

    Serial.print(F("ModularSensors version "));
    Serial.println(MODULAR_SENSORS_VERSION);

#if defined UseModem_Module
    Serial.print(F("TinyGSM Library version "));
    Serial.println(TINYGSM_VERSION);
#else
    Serial.println(F("TinyGSM - none"));
#endif

    unusedBitsMakeSafe();
    readAvrEeprom();

    // A vital check on power availability
    do {
        LiBattPower_Unseable =
            ((PS_LBATT_UNUSEABLE_STATUS ==
              mcuBoard.isBatteryStatusAbove(true, PS_PWR_USEABLE_REQ))
                 ? true
                 : false);
        if (LiBattPower_Unseable) {
/* Sleep
 * If can't collect data wait for more power to accumulate.
 * This sleep appears to taking 5mA, where as later sleep takes 3.7mA
 * Under no other load conditions the mega1284 takes about 35mA
 * Another issue is that onstartup currently requires turning on comms device to
 * set it up. On an XbeeS6 WiFi this can take 20seconds for some reason.
 */
#if 1  // defined(CHECK_SLEEP_POWER)
            SerialStd.print(lp_wait++);
            SerialStd.print(F(": BatteryLow-Sleep60sec, BatV="));
            SerialStd.println(mcuBoard.getBatteryVm1(false));
#endif  //(CHECK_SLEEP_POWER)
        // delay(59000); //60Seconds
        // if(_mcuWakePin >= 0){systemSleep();}
            dataLogger.systemSleep(1);
            delay(1000);  // debug
            SerialStd.println(F("----Wakeup"));
        }
    } while (LiBattPower_Unseable);
    SerialStd.print(F("Good BatV="));
    SerialStd.println(mcuBoard.getBatteryVm1(false));

// Allow interrupts for software serial
#if defined SoftwareSerial_ExtInts_h
    enableInterrupt(softSerialRx, SoftwareSerial_ExtInts::handle_interrupt,
                    CHANGE);
#endif
#if defined NeoSWSerial_h
    enableInterrupt(neoSSerial1Rx, neoSSerial1ISR, CHANGE);
#endif

// Start the serial connection with the modem
#if defined UseModem_Module
    MS_DEEP_DBG("***modemSerial.begin");
    modemSerial.begin(modemBaud);
#endif  // UseModem_Module

#if defined(CONFIG_SENSOR_RS485_PHY)
    // Start the stream for the modbus sensors; all currently supported modbus
    // sensors use 9600 baud
    MS_DEEP_DBG("***modbusSerial.begin");
    delay(10);

    modbusSerial.begin(MODBUS_BAUD_RATE);
    modbusPinPowerMng(false);  // Turn off pins
#endif

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    digitalWrite(greenLED, LOW);
    pinMode(redLED, OUTPUT);
    digitalWrite(redLED, LOW);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();
    // not in this scope Wire.begin();


    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    Logger::setRTCTimeZone(0);

#ifdef UseModem_Module
    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modemPhy);
    // modemPhy.setModemLED(modemLEDPin); //Used in UI_status subsystem
#if defined Modem_SignalPercent_UUID || defined DIGI_RSSI_UUID  //|| or others
    modemPhy.pollModemMetadata(POLL_MODEM_META_DATA_ON);
#endif
#endif  // UseModem_Module
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin,
                             greenLED);

#ifdef USE_MS_SD_INI
    // Set up SD card access
    Serial.println(F("---parseIni "));
    dataLogger.setPs_cache(&ps_ram);
    dataLogger.parseIniSd(configIniID_def, inihUnhandledFn);
    Serial.println(F("\n\n---parseIni complete "));
#endif  // USE_MS_SD_INI

    mcuBoard.printBatteryThresholds();

    // Begin the logger
    MS_DBG(F("---dataLogger.begin "));
    dataLogger.begin();
    // dataLogger.serzBegin(); //Data walkthrough on card
#if defined UseModem_Module
    EnviroDIYPOST.begin(dataLogger, &modemPhy.gsmClient,
                        ps_ram.app.provider.s.registration_token,
                        ps_ram.app.provider.s.sampling_feature);
    EnviroDIYPOST.setQued(true);
    dataLogger.setSendEveryX(collectReadings);
    dataLogger.setSendOffset(sendOffset_min);  // delay Minutes

#endif  // UseModem_Module

// Sync the clock  and we have battery to spare
#if defined UseModem_Module && !defined NO_FIRST_SYNC_WITH_NIST
    while ((PS_LBATT_UNUSEABLE_STATUS ==
            mcuBoard.isBatteryStatusAbove(true, PS_PWR_LOW_REQ))) {
        MS_DBG(F("Not enough power to sync with NIST "),
               mcuBoard.getBatteryVm1(false), F("Need"), PS_PWR_LOW_REQ);
        dataLogger.systemSleep();
    }
#if defined DigiXBeeWifi_Module
    // For the WiFi module, it may not be configured if no nscfg.ini file
    // present,
    // this supports the standalone logger, but need to get time at
    // factory/ms_cfg.ini present
    uint8_t cmp_result = modemPhy.getWiFiId().compareTo(wifiId_def);
    // MS_DBG(F("cmp_result="),cmp_result,"
    // ",modemPhy.getWiFiId(),"/",wifiId_def);
    if (!(cmp_result == 0)) {
        SerialStd.print(F("Sync with NIST over WiFi network "));
        SerialStd.println(modemPhy.getWiFiId());
        dataLogger.syncRTC();  // Will also set up the modemPhy
    }
#else
    MS_DBG(F("Sync with NIST "));
    dataLogger.syncRTC();  // Will also set up the modemPhy
#endif  // DigiXBeeWifi_Module
#endif  // UseModem_Module
    // List start time, if RTC invalid will also be initialized
    PRINTOUT(F("Time "),
             dataLogger.formatDateTime_ISO8601(dataLogger.getNowEpoch()));

    Serial.println(F("Setting up sensors..."));
    varArray.setupSensors();
// Create the log file, adding the default header to it
// Do this last so we have the best chance of getting the time correct and
// all sensor names correct
// Writing to the SD card can be power intensive, so if we're skipping
// the sensor setup we'll skip this too.
// SDI12?
#if defined KellerNanolevel_ACT
    nanolevel_snsr.registerPinPowerMng(&modbusPinPowerMng);
#endif  // KellerNanolevel_ACT
#if defined KellerAcculevel_ACT
    acculevel_snsr.registerPinPowerMng(&modbusPinPowerMng);
#endif  // KellerAcculevel_ACT
    Serial.println(F("Setting up file on SD card"));
    dataLogger.turnOnSDcard(
        true);  // true = wait for card to settle after power up
    dataLogger.createLogFile(true);  // true = write a new header
    dataLogger.turnOffSDcard(
        true);  // true = wait for internal housekeeping after write

    MS_DBG(F("\n\nSetup Complete ****"));
}


// ==========================================================================
// Main loop function
// ==========================================================================

void loop() {
    ps_Lbatt_status_t Lbatt_status;
    Lbatt_status = mcuBoard.isBatteryStatusAbove(true, PS_PWR_USEABLE_REQ);
    if (PS_LBATT_UNUSEABLE_STATUS == Lbatt_status) {
        PRINTOUT(F("---NewReading CANCELLED--Lbatt_V="),
                 mcuBoard.getBatteryVm1(false));
        dataLogger.systemSleep();
    }
    // If battery low, log data but don't send it over the modemPhy
    else
#if defined UseModem_PushData
        if (PS_LBATT_LOW_STATUS >= Lbatt_status)
#endif  // UseModem_PushData
    {
#if defined UseModem_PushData
        PRINTOUT(dataLogger.formatDateTime_ISO8601(dataLogger.getNowEpoch()),
                 F(" LogOnly. V too low batteryV="),
                 mcuBoard.getBatteryVm1(false), F(" Lbatt="), Lbatt_status);
#else
        PRINTOUT(F("Collect readings & log. batteryV="),
                 mcuBoard.getBatteryVm1(false), F(" status="), Lbatt_status);
#endif  // UseModem_PushData
        dataLogger.logData();
    }
#if defined UseModem_PushData
    // If the battery is good, send the data to the world
    else {
        PRINTOUT(dataLogger.formatDateTime_ISO8601(dataLogger.getNowEpoch()),
                 F(" log&Pub V_batt"), mcuBoard.getBatteryVm1(false),
                 F(" Lbatt="), Lbatt_status);
        dataLogger.logDataAndPublish();
    }
#endif  // UseModem_PushData
}
