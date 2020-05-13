/*****************************************************************************
kn_depth.cpp   (Keller Nanolevel Depth)
Written By:  Neil Hancock from great example /menu_a_la_carte by Sara Damiano
Development Environment: PlatformIO
Hardware Platform Supported: EnviroDIY Feather M4 Express with custom wing
Software License: BSD-3.
  Copyright (c) 2019, Neil Hancock & Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

Use with  ModularSensors library

This sends readings to EnviroDIY data portal, and logs to an SD card.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Defines for the Arduino IDE
//    In PlatformIO, set these build flags in your platformio.ini
// ==========================================================================
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 512
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

#include "ms_cfg.h" //must be before ms_common.h & Arduino.h

//After other includes that redifine MS_DEBUGGING_STD
#ifdef MS_KN_DEPTH_DEBUG
#undef MS_DEBUGGING_STD
#define MS_DEBUGGING_STD "kn_depth"
#define MS_DEBUG_THIS_MODULE 1
#endif //MS_KN_DEPTH_DEBUG

#ifdef MS_KN_DEPTH_DEBUG_DEEP
#undef MS_DEBUGGING_DEEP
#define MS_DEBUGGING_DEEP "kn_depthD"
#undef MS_DEBUG_THIS_MODULE
#define MS_DEBUG_THIS_MODULE 2
#endif
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
//#include <Arduino.h>  // The base Arduino library
#ifdef ARDUINO_AVR_ENVIRODIY_MAYFLY
#include <EnableInterrupt.h>  // for external and pin change interrupts
#endif
#include <LoggerBase.h>  // The modular sensors library
#include <Time.h>
#include <errno.h>
#include "ms_common.h"
#include "Adafruit_NeoPixel.h"

#include "PortExpanderB031.h"
#if defined USE_RTCLIB
#include <RTClib.h> 
#endif //USE_RTCLIB
#define KCONFIG_SHOW_NETWORK_INFO 1
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
#define KCONFIG_DEBUG_LEVEL 1
#else
#define KCONFIG_DEBUG_LEVEL 1
#endif


#if !defined SerialStd
#define SerialStd STANDARD_SERIAL_OUTPUT
#endif //SerialStd

//#define WIRING_DIGITALEXT_ACT
//#include "wiring_digtalext.h"

// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The library version this example was written for
//const char *libraryVersion = "0.23.16";
// The name of this file
const char *sketchName = __FILE__; //"xxx.cpp";
const char build_date[] = __DATE__ " " __TIME__;
//Windows processing of PIO_SRC_REV drops the "" making it a variable, which then can't be found!!!
//Encoded as hex to make it come through
#ifdef PIO_SRC_REV
const char git_branch[] = PIO_SRC_REV;
#else 
const char git_branch[] = "wip";
#endif
//const char build_epochTime = __BUILD_TIMESTAMP__;
//const char build_epochTime = __TIME_UNIX__;

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID_def = LOGGERID_DEF_STR;
const char *configIniID_def = configIniID_DEF_STR;  
// How frequently (in minutes) to log data
const uint8_t loggingInterval_def_min = loggingInterval_CDEF_MIN;
// The logger's timezone default.
int8_t timeZone =  CONFIG_TIME_ZONE_DEF;
uint32_t sysStartTime_epochTzSec=1;
bool nistSyncRtc = true; //true no battery. NIST sync RTC is required
static int loggingMultiplierCnt=0;
#if defined loggingMultiplier_MAX_CDEF
static int loggingMultiplierTop=loggingMultiplier_MAX_CDEF; //Working TOP threshold
#endif //loggingMultiplier_MAX_CDEF
static bool varArrayPub=false;

const int8_t I2CPower = -1;//sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <sensors/ProcessorStats.h>

const long SerialStdBaud = 115200;   // Baud rate for the primary serial port for debugging
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
//ms_cfg:#define greenLEDPin 8        // MCU pin for the green LED (-1 if not applicable)
//ms_cfg:#define redLEDPin   9        // MCU pin for the red LED (-1 if not applicable)
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
//#define greenLEDPin   8       //D8 // MCU pin for the green LED (-1 if not applicable)
//ms_cfg.h:#define redLEDPin    13       //D13 // MCU pin for the red LED (-1 if not applicable)
#if !defined greenLEDPin 
#define greenLEDPin LED_BUILTIN       //Built in LED is RED. MCU pin for the green LED (-1 if not applicable)
#endif //greenLEDPin 
// #define redLEDPin -1                  //Doesn't exist 
//NeoPixel WS2812 on FeatherM4express
#define NUM_NEOPIXELS 1
#ifdef NUM_NEOPIXELS
#define NEOPIXEL_PIN 8
Adafruit_NeoPixel neoPixelPhy(NUM_NEOPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif //NUM_NEOPIXELS

#elif defined(ARDUINO_SAMD_FEATHER_M0)
#define greenLEDPin   8       //D8 // MCU pin for the green LED (-1 if not applicable)
#define redLEDPin    13       //D13 // MCU pin for the red LED (-1 if not applicable)

#elif defined(ARDUINO_SODAQ_AUTONOMO)
//Only One LED Green - LED_BUILTIN
//const int8_t greenLED=LED_BUILTIN;  //MCU pin for the green LED (-1 if not applicable)
#define greenLEDPin LED_BUILTIN       //MCU pin for the green LED (-1 if not applicable)
#define redLEDPin -1                  //Doesn't exist 
#else
#error Undefined LEDS
#endif

#if defined redLEDPin && (redLEDPin != -1)
#define redLEDphy redLEDPin
#define setRedLED(state) digitalWrite(redLEDphy, state);
#else
#define redLEDphy -1
#define setRedLED(state)
#endif //redLED

#if defined greenLEDPin && (greenLEDPin != -1)
#define greenLEDphy greenLEDPin
#define setGreenLED(state) digitalWrite(greenLEDphy, state);
#define setGreenLEDon()  digitalWrite(greenLEDphy, HIGH);
#define setGreenLEDoff() digitalWrite(greenLEDphy, LOW);
#else
#define greenLEDphy -1
#define setGreenLED(state)
#endif //greenLED


#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
const int8_t buttonPin = -1;      // 21 Not used -MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = A7;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin = -1;    // MCU SD card power pin (-1 if not applicable)
const int8_t sdCardSSPin = 12;      // MCU SD card chip select/slave select pin (must be given!)
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
//.platformio\packages\framework-arduinosam\variants\feather_m0_express\variant.cpp
const int8_t buttonPin = -1;      // 21 Not used -MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = -1;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
//FEATHER_M4_EXPRESS has internal flash on QSPI P
//QSPI const int8_t sdCardSSPin = 10;  // PA08 MCU SD card chip select/slave select pin (must be given!)
//and has FEATHER_RTC_SD_CARD
    #if defined ADAFRUIT_FEATHERWING_eInk1_5in_SD
    const int8_t sdCardPwrPin = -1;    // MCU SD card power pin (-1 if not applicable)
    const int8_t sdCardSSPin = SD_SPI_CS_PIN_DEF; 
    #elif defined ADAFRUIT_FEATHERWING_RTC_SD 
    //SD on std port with SD_CS JP3-D10 PA18  RTC PCF8522+ SD
    const int8_t sdCardSSPin = SD_SPI_CARD_PIN_DEF;  //JP3-D10 PA18
    #else 
    const int8_t sdCardSSPin = -1; 
    #endif //ADAFRUIT_FEATHERWING_RTC_SD

#elif defined(ARDUINO_SAMD_FEATHER_M0)
const int8_t buttonPin = -1;      // 21 Not used -MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = -1;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardSSPin = 4;      // PA08 MCU SD card chip select/slave select pin (must be given!)
#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
//.platformio\packages\framework-arduinosam\variants\feather_m0_express\variant.cpp
//FEATHER_M0_EXPRESS has internal flash on secondary SPI PA13
const int8_t buttonPin = -1;      // 21 Not used -MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = A7;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardSSPin = 4;      // PA08 MCU SD card chip select/slave select pin (must be given!)
//and has FEATHER_RTC_SD_CARD
    #if defined(ADAFRUIT_FEATHERWING_RTC_SD)  //nh made up
    //SD on std port with SD_CS JP3-D10 PA18                 RTC PCF8522+ SD
    const int8_t sdCardSSPin = 10;  //JP3-D10 PA18
    #endif //ADAFRUIT_FEATHERWING_RTC_SD

#elif defined(ARDUINO_SODAQ_AUTONOMO)
const int8_t buttonPin = -1;      // 21 Not used -MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
const int8_t wakePin = -1;        // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPwrPin = -1;    //TODO check MCU SD card power pin (-1 if not applicable)
//uSD card & SerialFlash is SCOM3
// SAMD21 MOSI/PA20 MISO/PA22 SCK/PA21
// 
const int8_t sdCardSSPin = 46;//SS_2 PA27 variant.cpp 'pin'46  uSD  card  chip select/slave select pin 
const int8_t sfSsPin = 43;  //SS   PA23 variant.cpp 'pin'43 serialFlash chip select/slave select pin 
#else
#error Undefined SD
#endif

const int8_t sensorPowerPin = sensorPowerPin_DEF; // MCU pin controlling main sensor power (-1 if not applicable)
// Create and return the main processor chip "sensor" - for general metadata
const char *mcuBoardName    = HwName_DEF;
const char *mcuBoardVersion = HwVersion_DEF;
#if 1 //defined(ProcessorStats_ACT)
ProcessorStats mcuBoard(mcuBoardVersion);
#endif //ProcessorStats_ACT

// Create sample number, battery voltage, and free RAM variable pointers for the processor
// Variable *mcuBoardBatt = new ProcessorStats_Battery(&mcuBoard, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *mcuBoardAvailableRAM = new ProcessorStats_FreeRam(&mcuBoard, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *mcuBoardSampNo = new ProcessorStats_SampleNumber(&mcuBoard, "12345678-abcd-1234-ef00-1234567890ab");


//#define analogTh_T1_UUID "analogTh_T1_UUID"
#if defined(analogTh_T1_UUID)
#info "analogThermistor needs UT"
#include "analogThermistor.h"
//Real pins tbd
const int8_t analogThermistor_PowerPin = sensorPowerPin_DEF;
const int8_t analogThermistor_adcPin = 5;//sensorPowerPin_DEF;
analogThermistor analogTherm1parent(analogThermistor_PowerPin,analogThermistor_adcPin);
#endif //
// ==========================================================================
//    Settings for Additional Serial Ports
// ==========================================================================

// The modem and a number of sensors communicate over UART/TTL - often called "serial".
// "Hardware" serial ports (automatically controlled by the MCU) are generally
// the most accurate and should be configured and used for as many peripherals
// as possible.  In some cases (ie, modbus communication) many sensors can share
// the same serial port.

#if defined(ARDUINO_ARCH_AVR) || defined(__AVR__)  // For AVR boards
// Unfortunately, most AVR boards have only one or two hardware serial ports,
// so we'll set up three types of extra software serial ports to use

// AltSoftSerial by Paul Stoffregen (https://github.com/PaulStoffregen/AltSoftSerial)
// is the most accurate software serial port for AVR boards.
// AltSoftSerial can only be used on one set of pins on each board so only one
// AltSoftSerial port can be used.
// Not all AVR boards are supported by AltSoftSerial.
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;

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

// The "standard" software serial library uses interrupts that conflict
// with several other libraries used within this program, we must use a
// version of software serial that has been stripped of interrupts.
// NOTE:  Only use if necessary.  This is not a very accurate serial port!
const int8_t softSerialRx = A3;     // data in pin
const int8_t softSerialTx = A4;     // data out pin

#include <SoftwareSerial_ExtInts.h>  // for the stream communication
SoftwareSerial_ExtInts softSerial1(softSerialRx, softSerialTx);
#endif  // End software serial for avr boards


// The SAMD21/SAMD51 has 6 "SERCOM" ports, any of which can be used for UART communication.
// The "core" code for most boards defines one or more UART (Serial) ports with
// the SERCOMs and uses others for I2C and SPI.  We can create new UART ports on
// any available SERCOM.  The table below shows definitions for select boards.
// Variant.cpp/.h must be updated for extending SERCOM from std arduino framework.
//SODAQ AUTONOMO M0 board looks like its being phased out, and not supported.
/*Board FeatherM4Express    FeatherM0Express   FeatherM0            
 -----  ---------------    ----------------    ----------------
  USB   Serial                Serial           Serial
SERCOM0 Serial2 (A1/A4/A5)    Serial1 (D0/D1)  Serial1 (D0/D1)     
SERCOM1 SPI (MISO/MOSI/SCK)   Available        Available           
SERCOM2 I2C (SDA/SCL)         SPI2(Flash)      Available                 
SERCOM3 Serial3 (D12/D13/D10) I2C (D26/D27)    I2C (D20/D21)           
SERCOM4 Serial4 (A3/A2/D10)   SPI1 (D28/29/30) SPI (D21/22/23)    
SERCOM5 Serial1/Bee (DO/D1)   Available        EDBG/Serial5               
QSPI     Yes                     no                no                 
*/

// If using a Sodaq board, do not define the new sercoms, instead:
// #define ENABLE_SERIAL2
// #define ENABLE_SERIAL3


#if defined ARDUINO_ARCH_SAMD
#include <wiring_private.h> // Needed for SAMD pinPeripheral() function
//#if defined(ARDUINO_SAMD_ZERO) //&& defined(SERIAL_PORT_USBVIRTUAL)
//#define Serial SERIAL_PORT_USBVIRTUAL
//#elif 
  // Required for Serial on Zero based boards

  #if !defined(ARDUINO_SODAQ_AUTONOMO)
  //https://support.sodaq.com/sodaq-one/autonom%D0%BE/serial-io/
  /* SAMD21 - USB+ 6 configurable as serial,TWI or SPI 
    SerialUSB the native USB port. 
    Serial is available on pins D0 (RX) & D1 (TX). 
    Serial1 is connected to the Bee socket.
    Serial2 ?
    Serial3 - RS485? /SDI12 ?
  */
  //#define SerialStd Serial
  //#define DEBUGGING_SERIAL_OUTPUT Serial1
  #else
  //#define DEBUGGING_SERIAL_OUTPUT Serial
  //#define SerialStd Serial
  #endif //

#if 0//ndef ENABLE_SERIAL2
// Autonomo uses sercom4 see variant.cpp
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

#if 0 //ndef ENABLE_SERIAL3
//Atuonomo uses sercom1 see variant.cpp
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
#else
//#define SerialStd Serial
//#define DEBUGGING_SERIAL_OUTPUT Serial
#endif  // End hardware serial on SAMD21 boards


// ==========================================================================
//    Wifi/Cellular Modem Settings
// ==========================================================================

// Create a reference to the serial port for the modem
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
#if defined ARDUINO_AVR_ENVIRODIY_MAYFLY
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible

#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
//requires special variant.cpp/h update
HardwareSerial &modemSerial = Serial1;  // TODO:  need to decide
#define MODEMPHY_RX_PIN 0 
#define MODEMPHY_TX_PIN 1
//SerialModbus Serial2 RS485
//RS485 pins
//ms_cfg.h:SerialTty Serial4 Available Pins

#elif defined ARDUINO_SAMD_FEATHER_M0
HardwareSerial &modemSerial = Serial1;  //TODO B031r1 ?(was for SAMD51):  

#elif defined ARDUINO_SODAQ_AUTONOMO
HardwareSerial &modemSerial = Serial1;  // Bee Socket 

#else
#error HardwareSerial undef 
#endif // ARDUINO_AVR_ENVIRODIY_MAYFLY


// Modem Pins - Describe the physical pin connection of your modem to your board
const int8_t modemVccPin = modemVccPin_DEF;         // -2 MCU pin controlling modem power (-1 if not applicable)
const int8_t modemStatusPin = modemStatusPin_DEF;   //RTS 19 MCU pin used to read modem status (-1 if not applicable)
const int8_t modemResetPin = modemResetPin_DEF;     // MCU pin connected to modem reset pin (-1 if unconnected)
const int8_t modemSleepRqPin =modemSleepRqPin_DEF;//DTR 23 MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemLEDPin = redLEDPin;  // MCU pin connected an LED to show modem status (-1 if unconnected)

bool modemSetup=false;
// Network connection information
const char *apn_def = APN_CDEF;  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId_def = WIFIID_CDEF;  // The WiFi access point, unnecessary for gprs
const char *wifiPwd_def = WIFIPWD_CDEF;  // The password for connecting to WiFi, unnecessary for gprs



// ==========================================================================
//    The modem object
//    Note:  Don't use more than one!
// ==========================================================================

// Use this to create a modem if you want to monitor modem communication through
// a secondary Arduino stream.  Make sure you install the StreamDebugger library!
// https://github.com/vshymanskyy/StreamDebugger
#if defined STREAMDEBUGGER_DBG
 #include <StreamDebugger.h>
 StreamDebugger modemDebugger(modemSerial, STANDARD_SERIAL_OUTPUT);
 #define modemSerHw modemDebugger
#else
 #define modemSerHw modemSerial
#endif //STREAMDEBUGGER_DBG

#ifdef DigiXBeeCellularTransparent_Module 
// For any Digi Cellular XBee's
// NOTE:  The u-blox based Digi XBee's (3G global and LTE-M global) can be used
// in either bypass or transparent mode, each with pros and cons
// The Telit based Digi XBees (LTE Cat1) can only use this mode.
#include <modems/DigiXBeeCellularTransparent.h>
const long modemBaud = 9600;  // All XBee's use 9600 by default
const bool useCTSforStatus = false;   // Flag to use the XBee CTS pin for status
// NOTE:  If possible, use the STATUS/SLEEP_not (XBee pin 13) for status, but
// the CTS pin can also be used if necessary
DigiXBeeCellularTransparent modemXBCT(&modemSerial,
                                      modemVccPin, modemStatusPin, useCTSforStatus,
                                      modemResetPin, modemSleepRqPin,
                                      apn_def);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBeeCellularTransparent modemPhy = modemXBCT;
#endif // DigiXBeeCellularTransparent_Module 
// // ==========================================================================

#if 0 //def DigiXBeeLTE_Module 
// For the u-blox SARA R410M based Digi LTE-M XBee3
// NOTE:  According to the manual, this should be less stable than transparent
// mode, but my experience is the complete reverse.
#include <modems/DigiXBeeLTEBypass.h>
const long modemBaud = 9600;  // All XBee's use 9600 by default
const bool useCTSforStatus = false;   // Flag to use the XBee CTS pin for status
// NOTE:  If possible, use the STATUS/SLEEP_not (XBee pin 13) for status, but
// the CTS pin can also be used if necessary
DigiXBeeLTEBypass modemXBLTEB(&modemSerial,
                              modemVccPin, modemStatusPin, useCTSforStatus,
                              modemResetPin, modemSleepRqPin,
                              apn_def);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBeeLTEBypass modemPhy = modemXBLTEB;
#endif //DigiXBeeLTE_Module 
// ==========================================================================

#if 0 //Don't use -kept for compare simplicity
// For the u-blox SARA U201 based Digi 3G XBee with 2G fallback
// NOTE:  According to the manual, this should be less stable than transparent
// mode, but my experience is the complete reverse.
#include <modems/DigiXBee3GBypass.h>
const long modemBaud = 9600;  // All XBee's use 9600 by default
const bool useCTSforStatus = false;   // Flag to use the XBee CTS pin for status
// NOTE:  If possible, use the STATUS/SLEEP_not (XBee pin 13) for status, but
// the CTS pin can also be used if necessary
DigiXBee3GBypass modemXB3GB(&modemSerial,
                             modemVccPin, modemStatusPin, useCTSforStatus,
                             modemResetPin, modemSleepRqPin,
                             apn);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBee3GBypass modem = modemXB3GB;
// ==========================================================================
#endif //0
#ifdef DigiXBeeWifi_Module 
// For the Digi Wifi XBee (S6B)

#include <modems/DigiXBeeWifi.h>
const long modemBaud = 9600;  // All XBee's use 9600 by default
const bool useCTSforStatus = true;   //true? Flag to use the XBee CTS pin for status
// NOTE:  If possible, use the STATUS/SLEEP_not (XBee pin 13) for status, but
// the CTS pin can also be used if necessary
// useCTSforStatus is overload with  useCTSforStatus!-> loggerModem.statusLevel for detecting Xbee SleepReqAct==1
DigiXBeeWifi modemXBWF(&modemSerHw,
                       modemVccPin, modemStatusPin, useCTSforStatus,
                       modemResetPin, modemSleepRqPin,
                       wifiId_def, wifiPwd_def);
// Create an extra reference to the modem by a generic name (not necessary)
DigiXBeeWifi modemPhy = modemXBWF;
#endif //DigiXBeeWifi_Module 
// ==========================================================================

#if 1
#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_ESP8266
// For almost anything based on the Espressif ESP8266 using the AT command firmware
#include <modems/EspressifESP8266.h>
const long modemBaud = 115200;  // Communication speed of the modem
// NOTE:  This baud rate too fast for an 8MHz board, like the Mayfly!  The module
// should be programmed to a slower baud rate or set to auto-baud using the
// AT+UART_CUR or AT+UART_DEF command *before* attempting control with this library.
// Pins for light sleep on the ESP8266.
// For power savings, I recommend NOT using these if it's possible to use deep sleep.
const int8_t espSleepRqPin = 13;  // GPIO# ON THE ESP8266 to assign for light sleep request (-1 if not applicable)
const int8_t espStatusPin = -1;  // GPIO# ON THE ESP8266 to assign for light sleep status (-1 if not applicable)
EspressifESP8266 modemESP(&modemSerial,
                          modemVccPin, modemStatusPin,
                          modemResetPin, modemSleepRqPin,
                          wifiId, wifiPwd,
                          espSleepRqPin, espStatusPin  // Optional arguments
                         );
// Create an extra reference to the modem by a generic name (not necessary)
EspressifESP8266 modem = modemESP;
// ==========================================================================

#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_BG96
// For the Dragino, Nimbelink or other boards based on the Quectel BG96
#include <modems/QuectelBG96.h>
const long modemBaud = 115200;  // Communication speed of the modem
QuectelBG96 modemBG96(&modemSerial,
                      modemVccPin, modemStatusPin,
                      modemResetPin, modemSleepRqPin,
                      apn);
// Create an extra reference to the modem by a generic name (not necessary)
QuectelBG96 modem = modemBG96;
// ==========================================================================

#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_MONARCH
// For the Nimbelink LTE-M Verizon/Sequans or other boards based on the Sequans Monarch series
#include <modems/SequansMonarch.h>
const long modemBaud = 921600;  // Default baud rate of SVZM20 is 921600
// NOTE:  This baud rate is much too fast for many Arduinos!  The module should
// be programmed to a slower baud rate or set to auto-baud using the AT+IPR command.
SequansMonarch modemSVZM(&modemSerial,
                         modemVccPin, modemStatusPin,
                         modemResetPin, modemSleepRqPin,
                         apn);
// Create an extra reference to the modem by a generic name (not necessary)
SequansMonarch modem = modemSVZM;
// ==========================================================================

#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_SIM800
// For almost anything based on the SIMCom SIM800 EXCEPT the Sodaq 2GBee R6 and higher
#include <modems/SIMComSIM800.h>
const long modemBaud = 9600;  //  SIM800 does auto-bauding by default
SIMComSIM800 modemS800(&modemSerial,
                       modemVccPin, modemStatusPin,
                       modemResetPin, modemSleepRqPin,
                       apn);
// Create an extra reference to the modem by a generic name (not necessary)
SIMComSIM800 modem = modemS800;
// ==========================================================================

#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_SIM7000
// For almost anything based on the SIMCom SIM7000
#include <modems/SIMComSIM7000.h>
const long modemBaud = 9600;  //  SIM7000 does auto-bauding by default
SIMComSIM7000 modem7000(&modemSerial,
                        modemVccPin, modemStatusPin,
                        modemResetPin, modemSleepRqPin,
                        apn);
// Create an extra reference to the modem by a generic name (not necessary)
SIMComSIM7000 modem = modem7000;
// ==========================================================================

#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_S2GB
// For the Sodaq 2GBee R6 and R7 based on the SIMCom SIM800
// NOTE:  The Sodaq GPRSBee doesn't expose the SIM800's reset pin
#include <modems/Sodaq2GBeeR6.h>
const long modemBaud = 9600;  //  SIM800 does auto-bauding by default
Sodaq2GBeeR6 modem2GB(&modemSerial,
                      modemVccPin, modemStatusPin,
                      apn);
// Create an extra reference to the modem by a generic name (not necessary)
Sodaq2GBeeR6 modem = modem2GB;
// ==========================================================================

#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_UBEE_R410M
// For the Sodaq UBee based on the 4G LTE-M u-blox SARA R410M
#include <modems/SodaqUBeeR410M.h>
const long modemBaud = 115200;  // Default baud rate of the SARA R410M is 115200
// NOTE:  The SARA R410N DOES NOT save baud rate to non-volatile memory.  After
// every power loss, the module will return to the default baud rate of 115200.
// NOTE:  115200 is TOO FAST for an 8MHz Arduino.  This library attempts to
// compensate by sending a baud rate change command in the wake function.
// Because of this, 8MHz boards, LIKE THE MAYFLY, *MUST* use a HardwareSerial
// instance as modemSerial.
SodaqUBeeR410M modemR410(&modemSerial,
                         modemVccPin, modemStatusPin,
                         modemResetPin, modemSleepRqPin,
                         apn);
// Create an extra reference to the modem by a generic name (not necessary)
SodaqUBeeR410M modem = modemR410;
// ==========================================================================

#elif defined MS_BUILD_TESTING && defined MS_BUILD_TEST_UBEE_U201
// For the Sodaq UBee based on the 3G u-blox SARA U201
#include <modems/SodaqUBeeU201.h>
const long modemBaud = 9600;  //  SARA U2xx module does auto-bauding by default
SodaqUBeeU201 modemU201(&modemSerial,
                        modemVccPin, modemStatusPin,
                        modemResetPin, modemSleepRqPin,
                        apn);
// Create an extra reference to the modem by a generic name (not necessary)
SodaqUBeeU201 modem = modemU201;
// ==========================================================================
#endif


// Create RSSI and signal strength variable pointers for the modem
// Variable *modemRSSI = new Modem_RSSI(&modem, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *modemSignalPct = new Modem_SignalPercent(&modem, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *modemSignalPct = new Modem_BatteryState(&modem, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *modemSignalPct = new Modem_BatteryPercent(&modem, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *modemSignalPct = new Modem_BatteryVoltage(&modem, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *modemSignalPct = new Modem_Temp(&modem, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *modemSignalPct = new Modem_ActivationDuration(&modem, "12345678-abcd-1234-ef00-1234567890ab");


#if defined MaximDS3231_TEMP_UUID
// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <sensors/MaximDS3231.h>

// Create a DS3231 sensor object
MaximDS3231 ds3231(1);

// Create a temperature variable pointer for the DS3231
// Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-ef00-1234567890ab");
#endif


#ifdef SENSOR_CONFIG_GENERAL
// ==========================================================================
//    Atlas Scientific EZO-CO2 Embedded NDIR Carbon Dioxide Sensor
// ==========================================================================
#include <sensors/AtlasScientificCO2.h>

const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasCO2i2c_addr = 0x69;  // Default for CO2-EZO is 0x69 (105)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create an Atlas Scientific CO2 sensor object
// AtlasScientificCO2 atlasCO2(I2CPower, AtlasCO2i2c_addr);
AtlasScientificCO2 atlasCO2(I2CPower);

// Create concentration and temperature variable pointers for the EZO-CO2
// Variable *atlasCO2 = new AtlasScientificCO2_CO2(&atlasCO2, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *atlasCO2Temp = new AtlasScientificCO2_Temp(&atlasCO2, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-DO Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/AtlasScientificDO.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasDOi2c_addr = 0x61;  // Default for DO is 0x61 (97)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create an Atlas Scientific DO sensor object
// AtlasScientificDO atlasDO(I2CPower, AtlasDOi2c_addr);
AtlasScientificDO atlasDO(I2CPower);

// Create concentration and percent saturation variable pointers for the EZO-DO
// Variable *atlasDOconc = new AtlasScientificDO_DOmgL(&atlasDO, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *atlasDOpct = new AtlasScientificDO_DOpct(&atlasDO, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-EC Conductivity Sensor
// ==========================================================================
#include <sensors/AtlasScientificEC.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasECi2c_addr = 0x64;  // Default for EC is 0x64 (100)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create an Atlas Scientific Conductivity sensor object
// AtlasScientificEC atlasEC(I2CPower, AtlasECi2c_addr);
AtlasScientificEC atlasEC(I2CPower);

// Create four variable pointers for the EZO-ES
// Variable *atlasCond = new AtlasScientificEC_Cond(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *atlasTDS = new AtlasScientificEC_TDS(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *atlasSal = new AtlasScientificEC_Salinity(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *atlasGrav = new AtlasScientificEC_SpecificGravity(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-ORP Oxidation/Reduction Potential Sensor
// ==========================================================================
#include <sensors/AtlasScientificORP.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasORPi2c_addr = 0x62;  // Default for ORP is 0x62 (98)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create an Atlas Scientific ORP sensor object
// AtlasScientificORP atlasORP(I2CPower, AtlasORPi2c_addr);
AtlasScientificORP atlasORP(I2CPower);

// Create a potential variable pointer for the ORP
// Variable *atlasORPot = new AtlasScientificORP_Potential(&atlasORP, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-pH Sensor
// ==========================================================================
#include <sensors/AtlasScientificpH.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlaspHi2c_addr = 0x63;  // Default for pH is 0x63 (99)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create an Atlas Scientific pH sensor object
// AtlasScientificpH atlaspH(I2CPower, AtlaspHi2c_addr);
AtlasScientificpH atlaspH(I2CPower);

// Create a pH variable pointer for the pH sensor
// Variable *atlaspHpH = new AtlasScientificpH_pH(&atlaspH, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Atlas Scientific EZO-RTD Temperature Sensor
// ==========================================================================
#include <sensors/AtlasScientificRTD.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// uint8_t AtlasRTDi2c_addr = 0x66;  // Default for RTD is 0x66 (102)
// All Atlas sensors have different default I2C addresses, but any of them can
// be re-addressed to any 8 bit number.  If using the default address for any
// Atlas Scientific sensor, you may omit this argument.

// Create an Atlas Scientific RTD sensor object
// AtlasScientificRTD atlasRTD(I2CPower, AtlasRTDi2c_addr);
AtlasScientificRTD atlasRTD(I2CPower);

// Create a temperature variable pointer for the RTD
// Variable *atlasTemp = new AtlasScientificRTD_Temp(&atlasRTD, "12345678-abcd-1234-ef00-1234567890ab");

#endif // SENSOR_CONFIG_GENERAL
#if defined(ASONG_AM23XX_UUID)
// ==========================================================================
//    AOSong AM2315 Digital Humidity and Temperature Sensor
// ==========================================================================
#include <sensors/AOSongAM2315.h>

// const int8_t I2CPower = 1;//sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)

// Create an AOSong AM2315 sensor object
// Data sheets says AM2315 and AM2320 have same address 0xB8 (8bit addr) of 1011 1000 or 7bit 0x5c=0101 1100 
// AM2320 AM2315 address 0x5C
AOSongAM2315 am23xx(I2CPower);

// Create humidity and temperature variable pointers for the AM2315
// Variable *am2315Humid = new AOSongAM2315_Humidity(&am23xx, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *am2315Temp = new AOSongAM2315_Temp(&am23xx, "12345678-abcd-1234-ef00-1234567890ab");
#endif //ASONG_AM23XX_UUID
#ifdef SENSOR_CONFIG_GENERAL
// ==========================================================================
//    AOSong DHT 11/21 (AM2301)/22 (AM2302) Digital Humidity and Temperature
// ==========================================================================
#include <sensors/AOSongDHT.h>

const int8_t DHTPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t DHTPin = 10;  // DHT data pin
DHTtype dhtType = DHT11;  // DHT type, either DHT11, DHT21, or DHT22

// Create an AOSong DHT sensor object
AOSongDHT dht(DHTPower, DHTPin, dhtType);

// Create humidity, temperature, and heat index variable pointers for the DHT
// Variable *dhtHumid = new AOSongDHT_Humidity(&dht, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *dhtTemp = new AOSongDHT_Temp(&dht, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *dhtHI = new AOSongDHT_HI(&dht, "12345678-abcd-1234-ef00-1234567890ab");

// ==========================================================================
//    Apogee SQ-212 Photosynthetically Active Radiation (PAR) Sensor
// ==========================================================================
#include <sensors/ApogeeSQ212.h>

const int8_t SQ212Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SQ212ADSChannel = 3;  // The ADS channel for the SQ212
const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC

// Create an Apogee SQ212 sensor object
ApogeeSQ212 SQ212(SQ212Power, SQ212ADSChannel);

// Create a PAR variable pointer for the SQ212
// Variable *sq212PAR = new ApogeeSQ212_PAR(&SQ212, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <sensors/BoschBME280.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t BMEi2c_addr = 0x76;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address

// Create a Bosch BME280 sensor object
BoschBME280 bme280(I2CPower, BMEi2c_addr);

// Create four variable pointers for the BME280
// Variable *bme280Humid = new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *bme280Temp = new BoschBME280_Temp(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *bme280Press = new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *bme280Alt = new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Campbell OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>

const int8_t OBS3Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3NumberReadings = 10;
// const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowADSChannel = 0;  // The ADS channel for the low range output
const float OBSLow_A = 0.000E+00;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = 1.000E+00;  // The "B" value (X) from the low range calibration
const float OBSLow_C = 0.000E+00;  // The "C" value from the low range calibration

// Create a Campbell OBS3+ LOW RANGE sensor object
CampbellOBS3 osb3low(OBS3Power, OBSLowADSChannel, OBSLow_A, OBSLow_B, OBSLow_C, ADSi2c_addr, OBS3NumberReadings);

// Create turbidity and voltage variable pointers for the low range of the OBS3
// Variable *obs3TurbLow = new CampbellOBS3_Turbidity(&osb3low, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *obs3VoltLow = new CampbellOBS3_Voltage(&osb3low, "12345678-abcd-1234-ef00-1234567890ab");


// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighADSChannel = 1;  // The ADS channel for the high range output
const float OBSHigh_A = 0.000E+00;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = 1.000E+00;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = 0.000E+00;  // The "C" value from the high range calibration

// Create a Campbell OBS3+ HIGH RANGE sensor object
CampbellOBS3 osb3high(OBS3Power, OBSHighADSChannel, OBSHigh_A, OBSHigh_B, OBSHigh_C, ADSi2c_addr, OBS3NumberReadings);

// Create turbidity and voltage variable pointers for the high range of the OBS3
// Variable *obs3TurbHigh = new CampbellOBS3_Turbidity(&osb3high, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *obs3VoltHigh = new CampbellOBS3_Voltage(&osb3high, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Decagon 5TM Soil Moisture Sensor
// ==========================================================================
#include <sensors/Decagon5TM.h>

const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create a Decagon 5TM sensor object
Decagon5TM fivetm(*TMSDI12address, SDI12Power, SDI12Data);

// Create the matric potential, volumetric water content, and temperature
// variable pointers for the 5TM
// Variable *fivetmEa = new Decagon5TM_Ea(&fivetm, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *fivetmVWC = new Decagon5TM_VWC(&fivetm, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *fivetmTemp = new Decagon5TM_Temp(&fivetm, "12345678-abcd-1234-ef00-1234567890ab");
#endif //SENSOR_CONFIG_GENERAL
#ifdef Decagon_CTD_UUID

// ==========================================================================
//    Decagon CTD Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/DecagonCTD.h>

const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const uint8_t CTDNumberReadings = 6;  // The number of readings to average
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create a Decagon CTD sensor object
DecagonCTD ctd(*CTDSDI12address, SDI12Power, SDI12Data, CTDNumberReadings);

// Create conductivity, temperature, and depth variable pointers for the CTD
// Variable *ctdCond = new DecagonCTD_Cond(&ctd, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *ctdTemp = new DecagonCTD_Temp(&ctd, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *ctdDepth = new DecagonCTD_Depth(&ctd, "12345678-abcd-1234-ef00-1234567890ab");

#endif //Decagon_CTD_UUID
#ifdef SENSOR_CONFIG_GENERAL 
// ==========================================================================
//    Decagon ES2 Conductivity and Temperature Sensor
// ==========================================================================
#include <sensors/DecagonES2.h>

const char *ES2SDI12address = "3";  // The SDI-12 Address of the ES2
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin
const uint8_t ES2NumberReadings = 3;

// Create a Decagon ES2 sensor object
DecagonES2 es2(*ES2SDI12address, SDI12Power, SDI12Data, ES2NumberReadings);

// Create conductivity and temperature variable pointers for the ES2
// Variable *es2Cond = new DecagonES2_Cond(&es2, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *es2Temp = new DecagonES2_Temp(&es2, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create an External Voltage sensor object
ExternalVoltage extvolt0(ADSPower, ADSChannel0, dividerGain, ADSi2c_addr, VoltReadsToAvg);
ExternalVoltage extvolt1(ADSPower, ADSChannel1, dividerGain, ADSi2c_addr, VoltReadsToAvg);

// Create a voltage variable pointer
// Variable *extvoltV = new ExternalVoltage_Volt(&extvolt, "12345678-abcd-1234-ef00-1234567890ab");
#endif //ExternalVoltage_ACT
#if defined(ProcVolt_ACT) && defined(B031_AEM_EXTENSIONS)
// ==========================================================================
//    External Voltage  ProcessorAdc
// ==========================================================================
#include <sensors/processorAdc.h>


const int8_t procVoltPower = -1;//eMcpA_SwVbatOut_pinnum;  //Requires VbtSw Pin to switch power on and off (-1 if unconnected)
//B031 has expanded channels - Assume PCB default. Opts for 8more ADC Pins.
//J5 B031rev2  - which is ArduinioFramework PIN_A5 (Feather M4E pin 10). No Mux
const int8_t procVoltChan0 = ARD_ANLAOG_MULTIPLEX_PIN;//PIN_EXT_ANALOG(01);  // B031r2 J2Pin2 MC74VHC4051PinX1 MUX to PIN_A5
//const int8_t procVoltChan1 = 1;  // The AdcProc channel of interest
//const int8_t procVoltChan2 = 2;  // The AdcProc channel of interest
//const int8_t procVoltChan3 = 3;  // The AdcProc channel of interest
//const float procVoltDividerGain = 30.3; //  pwr_mon 1M/33K* measuredAdc(V) or 30.3 15.15
const float procVoltDividerGain = 6.0; //  for ext 1M/200k = 66 measuredAdc(V wrt 3.3V) 
const uint8_t procVoltReadsToAvg = 1; // Only read one sample

// Create an External Voltage sensor object
//processorAdc procVolt0(procVoltPower, procVoltChan0, procVoltDividerGain, procVoltReadsToAvg);
//processorAdc procVolt1(procVoltPower, procVoltChan1, procVoltDividerGain, procVoltReadsToAvg);


const int8_t sensor_Vbatt_PIN = PIN_EXT_ANALOG(B031_AEM_VBATT_PIN);
const int8_t sensor_V3V6_PIN =  PIN_EXT_ANALOG(B031_AEM_V3V6_PIN);
processorAdc sensor_batt_V(procVoltPower,  sensor_Vbatt_PIN, procVoltDividerGain, procVoltReadsToAvg);
processorAdc sensor_V3v6_V(procVoltPower, sensor_V3V6_PIN,  procVoltDividerGain, procVoltReadsToAvg);

#endif //ExternalVoltage_ACT
#ifdef SENSOR_CONFIG_GENERAL


// ==========================================================================
//    Freescale Semiconductor MPL115A2 Barometer
// ==========================================================================
#include <sensors/FreescaleMPL115A2.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MPL115A2ReadingsToAvg = 1;

// Create an MPL115A2 barometer sensor object
MPL115A2 mpl115a2(I2CPower, MPL115A2ReadingsToAvg);

// Create pressure and temperature variable pointers for the MPL
// Variable *mplPress = new MPL115A2_Pressure(&mpl115a2, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *mplTemp = new MPL115A2_Temp(&mpl115a2, "12345678-abcd-1234-ef00-1234567890ab");


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
const int8_t Sonar1Trigger = -1;  // Trigger pin (a unique negative number if unconnected) (D25 = A1)
const uint8_t sonar1NumberReadings = 3;  // The number of readings to average

// Create a MaxBotix Sonar sensor object
MaxBotixSonar sonar1(sonarSerial, SonarPower, Sonar1Trigger, sonar1NumberReadings);

// Create an ultrasonic range variable pointer
// Variable *sonar1Range = new MaxBotixSonar_Range(&sonar1, "12345678-abcd-1234-ef00-1234567890ab");


// const int8_t Sonar2Trigger = A2;  // Trigger pin (a unique negative number if unconnected) (D26 = A2)
// MaxBotixSonar sonar2(sonarSerial, SonarPower, Sonar2Trigger) ;
// Create an ultrasonic range variable pointer
// Variable *sonar2Range = new MaxBotixSonar_Range(&sonar2, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <sensors/MaximDS18.h>

// OneWire Address [array of 8 hex characters]
// If only using a single sensor on the OneWire bus, you may omit the address
DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
const int8_t OneWirePower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t OneWireBus = A0;  // Pin attached to the OneWire Bus (-1 if unconnected) (D24 = A0)

// Create a Maxim DS18 sensor objects (use this form for a known address)
MaximDS18 ds18(OneWireAddress1, OneWirePower, OneWireBus);

// Create a Maxim DS18 sensor object (use this form for a single sensor on bus with an unknown address)
// MaximDS18 ds18(OneWirePower, OneWireBus);

// Create a temperature variable pointer for the DS18
// Variable *ds18Temp = new MaximDS18_Temp(&ds18, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <sensors/MeaSpecMS5803.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 (default) or 0x77
const int16_t MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;

// Create a MeaSpec MS5803 pressure and temperature sensor object
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);

// Create pressure and temperature variable pointers for the MS5803
// Variable *ms5803Press = new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *ms5803Temp = new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    METER TEROS 11 Soil Moisture Sensor
// ==========================================================================
#include <sensors/MeterTeros11.h>

const char *teros11SDI12address = "4";  // The SDI-12 Address of the Teros 11
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin
const uint8_t teros11NumberReadings = 3;  // The number of readings to average

// Create a METER TEROS 11 sensor object
MeterTeros11 teros11(*teros11SDI12address, SDI12Power, SDI12Data, teros11NumberReadings);

// Create the matric potential, volumetric water content, and temperature
// variable pointers for the Teros 11
// Variable *teros11Ea = new MeterTeros11_Ea(&teros11, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *teros11Temp = new MeterTeros11_Temp(&teros11, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *teros11VWC = new MeterTeros11_VWC(&teros11, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    External I2C Rain Tipping Bucket Counter
// ==========================================================================
#include <sensors/RainCounterI2C.h>

const uint8_t RainCounterI2CAddress = 0x08;  // I2C Address for external tip counter
const float depthPerTipEvent = 0.2;  // rain depth in mm per tip event

// Create a Rain Counter sensor object
RainCounterI2C tbi2c(RainCounterI2CAddress, depthPerTipEvent);

// Create number of tips and rain depth variable pointers for the tipping bucket
// Variable *tbi2cTips = new RainCounterI2C_Tips(&tbi2c, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *tbi2cDepth = new RainCounterI2C_Depth(&tbi2c, "12345678-abcd-1234-ef00-1234567890ab");
#endif //SENSOR_CONFIG_GENERAL
#if defined(INA219ORIG_PHY_ACT)


// ==========================================================================
//    TI INA219 High Side Current/Voltage Sensor (Current mA, Voltage, Power)
// ==========================================================================
#include <sensors/TIINA219.h>

// const int8_t I2CPower = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t INA219i2c_addr = 0x40;  // 1000000 (Board A0+A1=GND)
// The INA219 can have one of 16 addresses, depending on the connections of A0 and A1
const uint8_t INA219ReadingsToAvg = 1;

// Create an INA219 sensor object
TIINA219 ina219(I2CPower, INA219i2c_addr, INA219ReadingsToAvg);

// Create current, voltage, and power variable pointers for the INA219
// Variable *inaCurrent = new TIINA219_Current(&ina219, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *inaVolt = new TIINA219_Volt(&ina219, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *inaPower = new TIINA219_Power(&ina219, "12345678-abcd-1234-ef00-1234567890ab");
#endif //INA219ORIG_PHY_ACT
#if defined(INA219M_PHY_ACT)


/*TI INA219M High Side Current/Voltage Sensor (Current mA, Voltage, Power)*/
#include <sensors/TIINA219M.h>
//const int8_t I2CPower = -1;//sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
uint8_t INA219i2c_addr = 0x40;  // 1000000 (Board A0+A1=GND)
// The INA219 can have one of 16 addresses, depending on the connections of A0 and A1
const uint8_t INA219ReadingsToAvg = 1;
// Create an INA219 sensor object
TIINA219M ina219m_phy(I2CPower, INA219i2c_addr, INA219ReadingsToAvg);
//was TIINA219M ina219_phy(I2CPower);

// Create the current, voltage, and power variable objects for the Nanolevel and return variable-type pointers to them
// Use these to create variable pointers with names to use in multiple arrays or any calculated variables.
// Variable *inaCurrent = new TIINA219_Current(&ina219, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *inaVolt = new TIINA219_Volt(&ina219, "12345678-abcd-1234-efgh-1234567890ab");
// NO Power
void ina219m_voltLowThresholdAlertFn(bool exceed,float value_V) {
    //Place holder for processing a measured low alert.
    //Expect to orginate a Cell TXT msg the first time receive this.
    MS_DBG(F("ina219m_voltLowThresholdAlert "),exceed,F(":"),value_V);
}
#endif //INA219M_PHY_ACT

#if defined Insitu_TrollSdi12_UUID
// ==========================================================================
//    Insitu Aqua/Level Troll Conductivity, Temperature, and Depth Sensor
// ==========================================================================
#include <sensors/InsituTrollSdi12.h>

const char *ITROLLSDI12address = "1";  // The SDI-12 Address of the ITROLL
const uint8_t ITROLLNumberReadings = 2;  // The number of readings to average
const int8_t IT_SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
const int8_t IT_SDI12Data = 7;  // The SDI12 data pin

// Create a  ITROLL sensor object
InsituTrollSdi12 itrollPhy(*ITROLLSDI12address, IT_SDI12Power, IT_SDI12Data, ITROLLNumberReadings);
#endif //Insitu_TrollSdi12_UUID

// ==========================================================================
//    Keller Acculevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#if defined(KellerAcculevel_ACT) || defined(KellerNanolevel_ACT)
#define KellerXxxLevel_ACT 1
//#include <sensors/KellerAcculevel.h>

// Create a reference to the serial port for modbus
// Extra hardware and software serial ports are created in the "Settings for Additional Serial Ports" section
#if defined SerialModbus && (defined ARDUINO_ARCH_SAMD || defined ATMEGA2560)
HardwareSerial &modbusSerial = SerialModbus;  // Use hardware serial if possible
#else
AltSoftSerial &modbusSerial = altSoftSerial;  // For software serial if needed
// NeoSWSerial &modbusSerial = neoSSerial1;  // For software serial if needed
#endif

//byte acculevelModbusAddress = KellerAcculevelModbusAddress;  // The modbus address of KellerAcculevel
const int8_t rs485AdapterPower = rs485AdapterPower_DEF;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
const int8_t modbusSensorPower = modbusSensorPower_DEF;  // Pin to switch sensor power on and off (-1 if unconnected)
const int8_t max485EnablePin = max485EnablePin_DEF;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)

const int8_t RS485PHY_TX_PIN = CONFIG_HW_RS485PHY_TX_PIN;
const int8_t RS485PHY_RX_PIN = CONFIG_HW_RS485PHY_RX_PIN;
const int8_t RS485PHY_DIR_PIN = CONFIG_HW_RS485PHY_DIR_PIN;

#endif //defined KellerAcculevel_ACT  || defined KellerNanolevel_ACT

#if defined KellerAcculevel_ACT
#include <sensors/KellerAcculevel.h>

byte acculevelModbusAddress = KellerAcculevelModbusAddress_DEF;  // The modbus address of KellerAcculevel
const uint8_t acculevelNumberReadings = 3;  // The manufacturer recommends taking and averaging a few readings

// Create a Keller Acculevel sensor object
KellerAcculevel acculevel_snsr(acculevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, acculevelNumberReadings);

// Create pressure, temperature, and height variable pointers for the Acculevel
// Variable *acculevPress = new KellerAcculevel_Pressure(&acculevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *acculevTemp = new KellerAcculevel_Temp(&acculevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *acculevHeight = new KellerAcculevel_Height(&acculevel, "12345678-abcd-1234-efgh-1234567890ab");
#endif //KellerAcculevel_ACT 


// ==========================================================================
//    Keller Nanolevel High Accuracy Submersible Level Transmitter
// ==========================================================================
#ifdef KellerNanolevel_ACT
#include <sensors/KellerNanolevel.h>

byte nanolevelModbusAddress = KellerNanolevelModbusAddress_DEF;  // The modbus address of KellerNanolevel
// const int8_t rs485AdapterPower = sensorPowerPin;  // Pin to switch RS485 adapter power on and off (-1 if unconnected)
// const int8_t modbusSensorPower = A3;  // Pin to switch sensor power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t nanolevelNumberReadings = 3;  // The manufacturer recommends taking and averaging a few readings

// Create a Keller Nanolevel sensor object
KellerNanolevel nanolevel_snsr(nanolevelModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, nanolevelNumberReadings);

// Create pressure, temperature, and height variable pointers for the Nanolevel
// Variable *nanolevPress = new KellerNanolevel_Pressure(&nanolevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *nanolevTemp = new KellerNanolevel_Temp(&nanolevel, "12345678-abcd-1234-efgh-1234567890ab");
// Variable *nanolevHeight = new KellerNanolevel_Height(&nanolevel, "12345678-abcd-1234-efgh-1234567890ab");

#endif //KellerNanolevel_ACT
#ifdef SENSOR_CONFIG_GENERAL

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

// Create a Yosemitech Y504 dissolved oxygen sensor object
YosemitechY504 y504(y504ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y504NumberReadings);

// Create the dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable pointers for the Y504
// Variable *y504DOpct = new YosemitechY504_DOpct(&y504, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y504DOmgL = new YosemitechY504_DOmgL(&y504, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y504Temp = new YosemitechY504_Temp(&y504, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create a Y510-B Turbidity sensor object
YosemitechY510 y510(y510ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y510NumberReadings);

// Create turbidity and temperature variable pointers for the Y510
// Variable *y510Turb = new YosemitechY510_Turbidity(&y510, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y510Temp = new YosemitechY510_Temp(&y510, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create a Y511-A Turbidity sensor object
YosemitechY511 y511(y511ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y511NumberReadings);

// Create turbidity and temperature variable pointers for the Y511
// Variable *y511Turb = new YosemitechY511_Turbidity(&y511, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y511Temp = new YosemitechY511_Temp(&y511, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create a Y514 chlorophyll sensor object
YosemitechY514 y514(y514ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y514NumberReadings);

// Create chlorophyll concentration and temperature variable pointers for the Y514
// Variable *y514Chloro = new YosemitechY514_Chlorophyll(&y514, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y514Temp = new YosemitechY514_Temp(&y514, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create a Y520 conductivity sensor object
YosemitechY520 y520(y520ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y520NumberReadings);

// Create specific conductance and temperature variable pointers for the Y520
// Variable *y520Cond = new YosemitechY520_Cond(&y520, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y520Temp = new YosemitechY520_Temp(&y520, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create a Yosemitech Y532 pH sensor object
YosemitechY532 y532(y532ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y532NumberReadings);

// Create pH, electrical potential, and temperature variable pointers for the Y532
// Variable *y532Voltage = new YosemitechY532_Voltage(&y532, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y532pH = new YosemitechY532_pH(&y532, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y532Temp = new YosemitechY532_Temp(&y532, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create a Y550 conductivity sensor object
YosemitechY550 y550(y550ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y550NumberReadings);

// Create COD, turbidity, and temperature variable pointers for the Y550
// Variable *y550COD = new YosemitechY550_COD(&y550, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y550Turbid = new YosemitechY550_Turbidity(&y550, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y550Temp = new YosemitechY550_Temp(&y550, "12345678-abcd-1234-ef00-1234567890ab");


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

// Create a Yosemitech Y4000 multi-parameter sensor object
YosemitechY4000 y4000(y4000ModbusAddress, modbusSerial, rs485AdapterPower, modbusSensorPower, max485EnablePin, y4000NumberReadings);

// Create all of the variable pointers for the Y4000
// Variable *y4000DO = new YosemitechY4000_DOmgL(&y4000, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y4000Turb = new YosemitechY4000_Turbidity(&y4000, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y4000Cond = new YosemitechY4000_Cond(&y4000, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y4000pH = new YosemitechY4000_pH(&y4000, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y4000Temp = new YosemitechY4000_Temp(&y4000, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y4000ORP = new YosemitechY4000_ORP(&y4000, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y4000Chloro = new YosemitechY4000_Chlorophyll(&y4000, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *y4000BGA = new YosemitechY4000_BGA(&y4000, "12345678-abcd-1234-ef00-1234567890ab");


// ==========================================================================
//    Zebra Tech D-Opto Dissolved Oxygen Sensor
// ==========================================================================
#include <sensors/ZebraTechDOpto.h>

const char *DOptoDI12address = "5";  // The SDI-12 Address of the Zebra Tech D-Opto
// const int8_t SDI12Power = sensorPowerPin;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t SDI12Data = 7;  // The SDI12 data pin

// Create a Zebra Tech DOpto dissolved oxygen sensor object
ZebraTechDOpto dopto(*DOptoDI12address, SDI12Power, SDI12Data);

// Create dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable pointers for the Zebra Tech
// Variable *dOptoDOpct = new ZebraTechDOpto_DOpct(&dopto, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *dOptoDOmgL = new ZebraTechDOpto_DOmgL(&dopto, "12345678-abcd-1234-ef00-1234567890ab");
// Variable *dOptoTemp = new ZebraTechDOpto_Temp(&dopto, "12345678-abcd-1234-ef00-1234567890ab");

#endif //SENSOR_CONFIG_GENERAL

// ==========================================================================
//    Electrical Conductivity using the processors analog pins
// ==========================================================================
#ifdef AnalogProcEC_ACT
#include <sensors/analogElecConductivity.h>
const int8_t ECpwrPin = ECpwrPin_DEF;
const int8_t ECdataPin1 = ECdataPin1_DEF;  
analogElecConductivity EC_procPhy(ECpwrPin, ECdataPin1);
#endif //AnalogProcEC_ACT

// ==========================================================================
//    Calculated Variables
// ==========================================================================

static float ina219M_A_LowReading=+9999; 
float ina219M_A_LowFn(void)
{
    //MS_DBG(F("ina219M_A_LowFn "),ina219M_A_LowReading);
    return ina219M_A_LowReading;
}
static float ina219M_A_HighReading=-9999; 
float ina219M_A_HighFn(void)
{
    //MS_DBG(F("ina219M_A_HighFn "),ina219M_A_HighReading);
    return ina219M_A_HighReading;
}
void ina219M_A_init()
{
    ina219M_A_LowReading=+9999; 
    ina219M_A_HighReading=-9999; 
}

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
    new ProcessorStats_Battery(&mcuBoard,   ProcessorStats_Batt_UUID),
#endif
#if defined AnalogProcEC_ACT
    new analogElecConductivity_EC(&EC_procPhy,EC1_UUID ),
#endif //AnalogProcEC_ACT
#if defined(ProcVolt_ACT)  && defined(B031_AEM_EXTENSIONS) 
    //new processorAdc_Volt(&procVolt0, ProcVolt_Volt0_UUID),
    new processorAdc_Volt(&sensor_batt_V,  ProcVolt_batt_UUID,"LiBat 4v2"),
    new processorAdc_Volt(&sensor_V3v6_V, ProcVolt_V3v6_UUID, "ExtBt 3v6"),    
#endif
#if defined(ExternalVoltage_Volt0_UUID)
    new ExternalVoltage_Volt(&extvolt0, ExternalVoltage_Volt0_UUID),
#endif
#if defined(ExternalVoltage_Volt1_UUID)
    new ExternalVoltage_Volt(&extvolt1, ExternalVoltage_Volt1_UUID),
#endif

#if defined(analogTh_T1_UUID)
    new analogThermistor_Temperature(&analogTherm1parent,analogTh_T1_UUID),
#endif 
#if defined(AdcProc_Volt1_UUID)
    new AdcProc_Volt(&extvolt1, AdcProc_Volt1_UUID),
#endif
#if defined(INA219M_MA_UUID)
    //new TIINA219M_Current(&ina219m_phy, INA219M_MA_UUID),
#endif
#if defined(INA219M_VOLT_UUID)
    new TIINA219M_Volt(&ina219m_phy, INA219M_VOLT_UUID),
#endif
#ifdef SENSOR_CONFIG_GENERAL
    new AtlasScientificCO2_CO2(&atlasCO2, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificCO2_Temp(&atlasCO2, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificDO_DOmgL(&atlasDO, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificDO_DOpct(&atlasDO, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificEC_Cond(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificEC_TDS(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificEC_Salinity(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificEC_SpecificGravity(&atlasEC, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificORP_Potential(&atlasORP, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificpH_pH(&atlaspH, "12345678-abcd-1234-ef00-1234567890ab"),
    new AtlasScientificRTD_Temp(&atlasRTD, "12345678-abcd-1234-ef00-1234567890ab"),
    #endif //SENSOR_CONFIG_GENERAL
    #if defined(ASONG_AM23XX_UUID)
    //new AOSongAM2315_Humidity(&am23xx,ASONG_AM23_Air_Humidity_UUID),
    new AOSongAM2315_Temp    (&am23xx,ASONG_AM23_Air_Temperature_UUID),
    #endif // ASONG_AM23XX_UUID
    #ifdef SENSOR_CONFIG_GENERAL
    new AOSongDHT_Humidity(&dht, "12345678-abcd-1234-ef00-1234567890ab"),
    new AOSongDHT_Temp(&dht, "12345678-abcd-1234-ef00-1234567890ab"),
    new AOSongDHT_HI(&dht, "12345678-abcd-1234-ef00-1234567890ab"),
    new ApogeeSQ212_PAR(&SQ212, "12345678-abcd-1234-ef00-1234567890ab"),
    new BoschBME280_Temp(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
    new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
    new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
    new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-ef00-1234567890ab"),
    new CampbellOBS3_Turbidity(&osb3low, "12345678-abcd-1234-ef00-1234567890ab", "TurbLow"),
    new CampbellOBS3_Voltage(&osb3low, "12345678-abcd-1234-ef00-1234567890ab", "TurbLowV"),
    new CampbellOBS3_Turbidity(&osb3high, "12345678-abcd-1234-ef00-1234567890ab", "TurbHigh"),
    new CampbellOBS3_Voltage(&osb3high, "12345678-abcd-1234-ef00-1234567890ab", "TurbHighV"),
    new Decagon5TM_Ea(&fivetm, "12345678-abcd-1234-ef00-1234567890ab"),
    new Decagon5TM_Temp(&fivetm, "12345678-abcd-1234-ef00-1234567890ab"),
    new Decagon5TM_VWC(&fivetm, "12345678-abcd-1234-ef00-1234567890ab"),
    new DecagonCTD_Cond(&ctd, "12345678-abcd-1234-ef00-1234567890ab"),
    new DecagonCTD_Temp(&ctd, "12345678-abcd-1234-ef00-1234567890ab"),
    new DecagonCTD_Depth(&ctd, "12345678-abcd-1234-ef00-1234567890ab"),
    new DecagonES2_Cond(&es2, "12345678-abcd-1234-ef00-1234567890ab"),
    new DecagonES2_Temp(&es2, "12345678-abcd-1234-ef00-1234567890ab"),
    new ExternalVoltage_Volt(&extvolt, "12345678-abcd-1234-ef00-1234567890ab"),
    new MaxBotixSonar_Range(&sonar1, "12345678-abcd-1234-ef00-1234567890ab"),
    new MaximDS18_Temp(&ds18, "12345678-abcd-1234-ef00-1234567890ab"),
    new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-ef00-1234567890ab"),
    new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-ef00-1234567890ab"),
    new MeterTeros11_Ea(&teros11, "12345678-abcd-1234-ef00-1234567890ab"),
    new MeterTeros11_Temp(&teros11, "12345678-abcd-1234-ef00-1234567890ab"),
    new MeterTeros11_VWC(&teros11, "12345678-abcd-1234-ef00-1234567890ab"),
    new MPL115A2_Temp(&mpl115a2, "12345678-abcd-1234-ef00-1234567890ab"),
    new MPL115A2_Pressure(&mpl115a2, "12345678-abcd-1234-ef00-1234567890ab"),
    new RainCounterI2C_Tips(&tbi2c, "12345678-abcd-1234-ef00-1234567890ab"),
    new RainCounterI2C_Depth(&tbi2c, "12345678-abcd-1234-ef00-1234567890ab"),
    new TIINA219_Current(&ina219, "12345678-abcd-1234-ef00-1234567890ab"),
    new TIINA219_Volt(&ina219, "12345678-abcd-1234-ef00-1234567890ab"),
    new TIINA219_Power(&ina219, "12345678-abcd-1234-ef00-1234567890ab"),
#endif //SENSOR_CONFIG_GENERAL

#if defined Insitu_TrollSdi12_UUID 
    new InsituTrollSdi12_Depth(&itrollPhy,ITROLL_DEPTH_UUID),
    //CTDDepthInCalc,
    new InsituTrollSdi12_Temp(&itrollPhy,ITROLL_TEMP_UUID),
    //CTDTempFcalc,
#endif //Insitu_TrollSdi12_UUID
#ifdef KellerAcculevel_ACT
    //new KellerAcculevel_Pressure(&acculevel, "12345678-abcd-1234-ef00-1234567890ab"),
    new KellerAcculevel_Temp(&acculevel_snsr, KellerAcculevel_Temp_UUID),
    new KellerAcculevel_Height(&acculevel_snsr, KellerAcculevel_Height_UUID),
#endif // KellerAcculevel_ACT
#ifdef KellerNanolevel_ACT
//   new KellerNanolevel_Pressure(&nanolevel_snsr, "12345678-abcd-1234-efgh-1234567890ab"),
    new KellerNanolevel_Temp(&nanolevel_snsr,   KellerNanolevel_Temp_UUID),
    new KellerNanolevel_Height(&nanolevel_snsr, KellerNanolevel_Height_UUID),
#endif //SENSOR_CONFIG_KELLER_NANOLEVEL
#ifdef SENSOR_CONFIG_GENERAL
    new YosemitechY504_DOpct(&y504, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY504_Temp(&y504, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY504_DOmgL(&y504, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY510_Temp(&y510, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY510_Turbidity(&y510, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY511_Temp(&y511, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY511_Turbidity(&y511, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY514_Temp(&y514, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY514_Chlorophyll(&y514, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY520_Temp(&y520, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY520_Cond(&y520, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY532_Temp(&y532, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY532_Voltage(&y532, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY532_pH(&y532, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_DOmgL(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_Turbidity(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_Cond(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_pH(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_Temp(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_ORP(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_Chlorophyll(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new YosemitechY4000_BGA(&y4000, "12345678-abcd-1234-ef00-1234567890ab"),
    new ZebraTechDOpto_Temp(&dopto, "12345678-abcd-1234-ef00-1234567890ab"),
    new ZebraTechDOpto_DOpct(&dopto, "12345678-abcd-1234-ef00-1234567890ab"),
    new ZebraTechDOpto_DOmgL(&dopto, "12345678-abcd-1234-ef00-1234567890ab"),
    new ProcessorStats_FreeRam(&mcuBoard, "12345678-abcd-1234-ef00-1234567890ab"),
    new ProcessorStats_Battery(&mcuBoard, "12345678-abcd-1234-ef00-1234567890ab"),
#endif // SENSOR_CONFIG_GENERAL

#if defined(MaximDS3231_Temp_UUID)
    new MaximDS3231_Temp(&ds3231,      MaximDS3231_Temp_UUID),
#endif //MaximDS3231_Temp_UUID
    //new Modem_RSSI(&modemPhy, "12345678-abcd-1234-ef00-1234567890ab"),
#if defined(Modem_SignalPercent_UUID) && defined(UseModem_Module)
    new Modem_SignalPercent(&modemPhy, Modem_SignalPercent_UUID),
#endif
    //new analogPinEc_EC(&analogPinEc_EC, "12345678-abcd-1234-ef00-1234567890ab"),
#ifdef SENSOR_CONFIG_GENERAL
    new Modem_BatteryState(&modemPhy, "12345678-abcd-1234-ef00-1234567890ab"),
    new Modem_BatteryPercent(&modemPhy, "12345678-abcd-1234-ef00-1234567890ab"),
    new Modem_BatteryVoltage(&modemPhy, "12345678-abcd-1234-ef00-1234567890ab"),
    new Modem_Temp(&modemPhy, "12345678-abcd-1234-ef00-1234567890ab"),
    new Modem_ActivationDuration(&modem, "12345678-abcd-1234-ef00-1234567890ab"),
#endif // SENSOR_CONFIG_GENERAL
#if defined INA219M_A_MIN_UUID
    new Variable(&ina219M_A_LowFn,2,"Min_A", "A","Min_A_Var", INA219M_A_MIN_UUID),
#endif
#if defined INA219M_A_MAX_UUID
    new Variable(&ina219M_A_HighFn,2,"Max_A","A","Max_A_Var",INA219M_A_MAX_UUID),
#endif
};
#if defined loggingMultiplier_MAX_CDEF
Variable *variableLstFast[] = {
    #if defined(INA219M_MA_UUID)
    new TIINA219M_Current(&ina219m_phy, INA219M_MA_UUID),
    #endif
    //Debug
    #if  0 //defined(ProcVolt_Volt0_UUID)
    new processorAdc_Volt(&procVolt0, ProcVolt_Volt0_UUID),
    #endif
};
#endif //loggingMultiplier_MAX_CDEF
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
    ina219M_A_Low
}
*/


// Count up the number of pointers in the array
int variableCount = sizeof(variableList) / sizeof(variableList[0]);

// Create the VariableArray object
VariableArray varArray(variableCount, variableList);
#if defined loggingMultiplier_MAX_CDEF
int variableCntFast = sizeof(variableLstFast) / sizeof(variableLstFast[0]);
VariableArray varArrFast(variableCntFast, variableLstFast);
#endif //loggingMultiplier_MAX_CDEF

// ==========================================================================
//     Port Expansion
// ==========================================================================
#if defined HwFeatherWing_B031ALL
#define MCP23017_ADDR 0x20
PortExpanderB031 mcpExp = PortExpanderB031(MCP23017_ADDR);
#endif //HwFeatherWing_B031ALL
// ==========================================================================
//     Local storage - evolving
// ==========================================================================
#ifdef USE_MS_SD_INI
 persistent_store_t ps;
#endif //#define USE_MS_SD_INI

// ==========================================================================
//     The Logger Object[s]
// ==========================================================================

// Create a new logger instance
Logger dataLogger(LoggerID_def, loggingInterval_def_min, sdCardSSPin, wakePin, &varArray);
#if defined loggingMultiplier_MAX_CDEF
//A 2 logger runs faster and raises the Nyquist sampling rate for the true dataLogger
Logger dataLogFast(LoggerID_def, loggingInterval_def_min,&varArrFast);
//Logger dataLogFast(LoggerID_def, loggingInterval_Fast_def_min,&varArrFast);
#endif //loggingMultiplier_MAX_CDEF

//now works with MS_DBG #if KCONFIG_DEBUG_LEVEL > 0   //0918
// ==========================================================================
//    A Publisher to Monitor My Watershed / EnviroDIY Data Sharing Portal
// ==========================================================================
// Device registration and sampling feature information can be obtained after
// registration at https://monitormywatershed.org or https://data.envirodiy.org
#if defined(registrationToken_UUID) && defined(samplingFeature_UUID)
const char *registrationToken_def = registrationToken_UUID;   // Device registration token
const char *samplingFeature_def = samplingFeature_UUID;     // Sampling feature UUID

// Create a data publisher for the EnviroDIY/WikiWatershed POST endpoint
#include <publishers/EnviroDIYPublisher.h>
//EnviroDIYPublisher EnviroDIYPOST(dataLogger, registrationToken_def, samplingFeature_def);
EnviroDIYPublisher EnviroDIYPOST(dataLogger, 15,0);
//EnviroDIYPublisher EnviroDIYPOST(); //"error: request for member 'begin' in 'EnviroDIYPOST', which is of non-class type 'EnviroDIYPublisher()'"
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
ThingSpeakPublisher TsMqtt(dataLogger, &modemPhy.gsmClient, thingSpeakMQTTKey, thingSpeakChannelID, thingSpeakChannelKey);

#endif //thingSpeakMQTTKey

//#endif //#if KCONFIG_DEBUG_LEVEL > 0   //0918
// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(uint8_t numFlash = 4, unsigned long timeOn_ms = 200,unsigned long timeOff_ms = 200)
{
    for (uint8_t i = 0; i < numFlash; i++) {
        //SerialStd.print(i);
        setGreenLED(HIGH);
        setRedLED( LOW);
        delay(timeOn_ms );
        setGreenLED(LOW);
        setRedLED( HIGH);
        delay(timeOff_ms );
    }
    setRedLED( LOW);
}

/* Status for User
 Consits of 
    Operational(1col) LED
        Status (3col) Led
        eInk  XbyY Display
*/
typedef enum {
  USR_OFF, //0
  USR_SENSORS_ACTIVE, //Green Water flashing light
  USR_COMMS_ACTIVE, //Blue Sky flashing
  USR_END

} UiStatus_req;
void UiStatus(uint8_t status_req, String ui_out = "");
void UiStatus(uint8_t status_req, String ui_out) {
   MS_DBG(F("UiStatus "),status_req);
  switch(status_req) {
    case USR_OFF:
        #ifndef SERIAL3_EN
        digitalWrite(LED_BUILTIN, LOW);
        #endif //SERIAL3_EN 
        neoPixelPhy.clear();
        neoPixelPhy.show();
       break;
    case USR_SENSORS_ACTIVE:
        #ifndef SERIAL3_EN    
        digitalWrite(LED_BUILTIN, HIGH);
        #endif // SERIAL3_EN
        neoPixelPhy.setPixelColor(0, neoPixelPhy.Color(0, 150, 0));
        neoPixelPhy.show();
        break;
    case USR_COMMS_ACTIVE:
        #ifndef SERIAL3_EN    
        digitalWrite(LED_BUILTIN, HIGH);
        #endif // SERIAL3_EN
        neoPixelPhy.setPixelColor(0, neoPixelPhy.Color(0, 0, 150));
        neoPixelPhy.show();
        break;          
    default:
        #ifndef SERIAL3_EN    
        digitalWrite(LED_BUILTIN, HIGH);
        #endif // SERIAL3_EN
        neoPixelPhy.setPixelColor(0, neoPixelPhy.Color(150, 0, 0));
        neoPixelPhy.show();
        break;
  }
#if 0
  //Output on eInk Display - slow updates
  if (0 != ui_out.length()){
    long timeNow_2ksec = uis_lastCall_now();

    SerialTty.println(ui_out);
    #ifdef SerialUSB
    SerialUSB.println(ui_out);
    #endif

    //Only update display if >= 180sec since last update
    if (eInkUpdateMin_sec <= (timeNow_2ksec - uis_lastCall_2ksec) ) {  
        uis_lastCall_2ksec = timeNow_2ksec;
        #if defined USE_EXT_DISPLAY
        extDisplay.print("\n\r");
        extDisplay.print(ui_out);
        extDisplay.display();
        #endif // USE_EXT_DISPLAY
    }
  }
  #endif
}//UiStatus

#include "iniHandler.h"

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
    bool LiBattPower_Unseable;
    uint16_t lp_wait=1;

    // Wait for USB connection to be established by PC
    // NOTE:  Only use this when debugging - if not connected to a PC, this
    // could prevent the script from starting
    //#if defined SERIAL_PORT_USBVIRTUAL
    //  while (!SERIAL_PORT_USBVIRTUAL && (millis() < 10000)){}
    //#endif

    //uint8_t mcu_status = MCUSR; is already cleared by Arduino startup???
    //MCUSR = 0; //reset for unique read
    // Start the primary SerialStd connection
    // Set up pins for the LED's
    #if defined greenLEDPin && (greenLEDPin != -1)
    pinMode(greenLEDphy, OUTPUT);
    setGreenLED(HIGH);
    #endif // greenLED
    #if defined redLEDPin  && (redLEDPin != -1)
       pinMode(redLEDphy, OUTPUT);
    setRedLED(LOW);
    #endif // redLED

    //#ifdef SerialUSB // SerialStd == SerialUSB
    //#error Serial Err

    while (!SerialStd && (millis() < 10000)){
        ledflash(100,1);
    }

    //#else
    //SerialStd.begin(SerialStdBaud);
    //#endif
    SerialStd.begin(SerialStdBaud);
    SerialStd.print(F("\n---Boot. Build date: ")); 
    SerialStd.print(build_date);

    //SerialStd.write('/');
    //SerialStd.print(build_epochTime,HEX);
    //SerialStd.print(__TIMESTAMP__); //still a ASC string Tue Dec 04 19:47:20 2018

    //MCUSR SerialStd.println(mcu_status,HEX);
    //SerialStd.println(file_name); //Dir and filename
    SerialStd.print(" ");
    SerialStd.print(sketchName); //Dir and filename
    SerialStd.print(" ");
    SerialStd.println(git_branch);

    SerialStd.print(mcuBoardName);
    SerialStd.print(" ");
    SerialStd.print(mcuBoardVersion);

    SerialStd.print(" variantPins=");
    SerialStd.print(thisVariantNumPins);
    SerialStd.print("/");
    SerialStd.print(totalNumPins); 
    //SerialStd.print("\n");
    //ledflash();//works
    #ifdef RAM_AVAILABLE
        RAM_AVAILABLE;
    #endif //RAM_AVAILABLE

    SerialStd.print(F("\nUsing ModularSensors Library version "));
    SerialStd.println(MODULAR_SENSORS_VERSION);
#if defined UseModem_Module
    SerialStd.print(F("TinyGSM Library version "));
    SerialStd.println(TINYGSM_VERSION);
#else 
    SerialStd.print(F("TinyGSM - none"));
#endif

    neoPixelPhy.begin();
    UiStatus(0);

    // A vital check on power availability
    do {
        LiBattPower_Unseable = ((PS_LBATT_UNUSEABLE_STATUS == mcuBoard.isBatteryStatusAbove(true,PS_PWR_USEABLE_REQ))?true:false);
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
            SerialStd.print(lp_wait++);
            SerialStd.print(F(": BatteryLow-Sleep60sec, BatV="));
            SerialStd.println(mcuBoard.getBatteryVm1(false));
            #endif //(CHECK_SLEEP_POWER)
            //delay(59000); //60Seconds
            //if(_mcuWakePin >= 0){systemSleep();}
            dataLogger.systemSleep(1); 
            delay(1000);//debug
            SerialStd.println(F("----Wakeup"));
        }
    } while (LiBattPower_Unseable); 
    SerialStd.print(F("Good BatV="));
    SerialStd.println(mcuBoard.getBatteryVm1(false));        
    /////// Measured LiIon voltage is good enough to start up
    //UiStatus(1);

    Wire.begin();

    #if defined HwFeatherWing_B031ALL  
    MS_DEEP_DBG("***mcpExp.init"); 
    delay(100);
    mcpExp.init();
    //Force a XBEE reset long enough for WiFi point to disconnect
    //and then allow enought time to comeout of reset.
    //mcpExp.pulseToggleBit(peB031_bit::eMcp_XbeeResetNout_bit,1000);
    //delay(1000);
    #endif //defined HwFeatherWing_B031ALL

    // Allow interrupts for software serial
    #if defined SoftwareSerial_ExtInts_h
        enableInterrupt(softSerialRx, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
    #endif
    #if defined NeoSWSerial_h
        enableInterrupt(neoSSerial1Rx, neoSSerial1ISR, CHANGE);
    #endif

    // Start the serial connection with the modem
    #if defined UseModem_Module
    MS_DEEP_DBG("***modemSerial.begin"); 
    delay(100);
    modemSetup=false;
    modemSerial.begin(modemBaud);
    #endif // UseModem_Module

#if defined(CONFIG_SENSOR_RS485_PHY) 
    // Start the stream for the modbus sensors; all currently supported modbus sensors use 9600 baud
    MS_DEEP_DBG("***modbusSerial.begin"); 
    #if defined CONFIG_HW_RS485PHY_DIR_PIN 
    pinPeripheral(CONFIG_HW_RS485PHY_DIR_PIN, PIO_SERCOM_ALT);
    #endif //CONFIG_HW_RS485PHY_DIR_PIN 
    delay(10);
    modbusSerial.begin(9600);
#endif

    // Start the SoftwareSerial stream for the sonar; it will always be at 9600 baud
    //sonarSerial.begin(9600);

#ifdef USE_MS_SD_INI
    //Set up SD card access, and also USB
    SerialStd.println(F("---parseIni "));
    dataLogger.parseIniSd(configIniID_def,inihUnhandledFn);
    SerialStd.println(F("\n\n---parseIni complete "));
#endif //USE_MS_SD_INI

#if 0
    SerialStd.print(F(" .ini-Logger:"));
    SerialStd.println(ps.msc.s.logger_id[0]);
    SerialStd.println(F(" List of UUIDs"));
    uint8_t i_lp;
    for (i_lp=0;i_lp<variableCount;i_lp++)
    {
        SerialStd.print(F("["));
        SerialStd.print(i_lp);
        SerialStd.print(F("] "));
        SerialStd.println(variableList[i_lp]->getVarUUID() );
    }
    //SerialStd.print(F("sF "))
    SerialStd.print(samplingFeature);
    SerialStd.print(F("/"));
    SerialStd.println(ps.provider.s.sampling_feature);
#endif //1
    //List PowerManagementSystem LiIon Bat thresholds

    mcuBoard.printBatteryThresholds();

#if 0
    uint8_t psilp,psolp;
    for (psolp=0; psolp<PSLR_NUM;psolp++) {
        SerialStd.print(psolp);
        SerialStd.print(F(": "));
        for (psilp=0; psilp<PS_LPBATT_TBL_NUM;psilp++) {
            SerialStd.print(mayflyPhy.PS_LBATT_TBL[psolp][psilp]);
            SerialStd.print(F(", "));
        }
        SerialStd.println();
    }
#endif //0

    // Set up some of the power pins so the board boots up with them off
    // NOTE:  This isn't necessary at all.  The logger begin() function
    // should leave all power pins off when it finishes.
    if (modemVccPin >= 0)
    {
        pinMode(modemVccPin, OUTPUT);
        digitalWrite(modemVccPin, LOW);
        MS_DBG(F("Set Power Off ModemVccPin "),modemVccPin);
    } else {MS_DBG(F("ModemVccPin not used "),modemVccPin);}
    if (sensorPowerPin >= 0)
    {
        pinMode(sensorPowerPin, OUTPUT);
        digitalWrite(sensorPowerPin, LOW);
        MS_DBG(F("Set sensorPowerPin "),sensorPowerPin);
    } else {MS_DBG(F("sensorPowerPin not used "),sensorPowerPin);}

    // Set up the sleep/wake pin for the modem and put its inital value as "off"

    if (modemSleepRqPin >= 0)
    {
        pinMode(modemSleepRqPin, OUTPUT);
        #define modemSleepRqState_DEF HIGH
        digitalWrite(modemSleepRqPin, modemSleepRqState_DEF); //Def sleep
        MS_DBG(F("Set Sleep on High modemSleepRqPin "),modemSleepRqPin);
    } else {MS_DBG(F("modemSleepRqPin not used "),modemSleepRqPin);}
    if (modemResetPin >= 0)
    {
        #define modemResetState_DEF HIGH
        pinMode(modemResetPin, OUTPUT);
        digitalWrite(modemResetPin, modemResetState_DEF);  //Def noReset
        MS_DBG(F("Set HIGH/!reset modemResetPin "),modemResetPin);
    } else {MS_DBG(F("modemResetPin not used "),modemResetPin);}

    // Set the timezones for the logger/data and the RTC
    // Logging in the given time zone
    Logger::setLoggerTimeZone(timeZone);
    // It is STRONGLY RECOMMENDED that you set the RTC to be in UTC (UTC+0)
    Logger::setRTCTimeZone(0);

    #if defined UseModem_Module
    // Attach the modem and information pins to the logger
    dataLogger.attachModem(modemPhy);
    //modemPhy.setModemLED(modemLEDPin);
    #endif // UseModem_Module
    dataLogger.setLoggerPins(wakePin, sdCardSSPin, sdCardPwrPin, buttonPin, greenLEDPin);

    // Begin the logger
    dataLogger.begin();
    #if defined USE_RTCLIB
    SerialStd.println(F("extRtcPhy check "));
    USE_RTCLIB * rtcPhyExt = dataLogger.rtcExtPhyObj();
    DateTime start_dt = rtcPhyExt->now();
    DateTime nxt_dt;
    int16_t dt_lp=0;
    #define DT_LP_MAX 10
    do { 
        delay(200);
        nxt_dt = rtcPhyExt->now();
        if (nxt_dt.second()!=start_dt.second()) {
            SerialStd.println(F("extRtcPhy sec changed "));
            break;
        }
        SerialStd.print(dt_lp);
        SerialStd.println(F("] extRtcPhy sec NOT changing "));    
    } while (++dt_lp<DT_LP_MAX ); 
    SerialStd.print(F("extRtcPhy start "));
    SerialStd.print(start_dt.timestamp(DateTime::TIMESTAMP_FULL));
        SerialStd.print(F(" nxt="));
    SerialStd.println(nxt_dt.timestamp(DateTime::TIMESTAMP_FULL));    
    #endif //USE_RTCLIB
    #if defined UseModem_Module
    EnviroDIYPOST.begin(dataLogger, &modemPhy.gsmClient, ps.provider.s.registration_token, ps.provider.s.sampling_feature);
    #endif // UseModem_Module
    #if defined loggingMultiplier_MAX_CDEF
    dataLogFast.begin();
    #endif //loggingMultiplier_MAX_CDEF

    SerialStd.print(F("Start Time: "));

    sysStartTime_epochTzSec = dataLogger.getNowEpochTz();
    //SerialStd.println(Logger::formatDateTime_ISO8601(dataLogger.getNowEpochTz()+(timeZone*60)) );
    SerialStd.print(Logger::formatDateTime_ISO8601(sysStartTime_epochTzSec  ));
    SerialStd.print(" TZ=");
    SerialStd.println(timeZone);
    // Attach the modem and information pins to the logger
    #ifdef RAM_AVAILABLE
        RAM_AVAILABLE;
    #endif //RAM_AVAILABLE
    //dataLogger.attachModem(modemPhy);
    //dataLogger.setAlertPin(-1);//greenLEDPin
    //dataLogger.setTestingModePin(buttonPin);
    //dataLogger.initializeSDCard(); //How to setup USB

        //modemPhy.modemPowerUp();
    varArray.setupSensors(); //Assumption pwr is available
    #if defined loggingMultiplier_MAX_CDEF
    varArrFast.setupSensors(); //Assumption pwr is available
    #endif //loggingMultiplier_MAX_CDEF

#if 1 //MS_DEBUG_THIS_MODULE
    //Enable this in debugging or where there is no valid RTC
    // defined ARDUINO_ARCH_SAMD && !defined USE_RTCLIB
    //ARCH_SAMD doesn't have persistent clock - get time
    //USE_RTCLIB implies extRtcPhy
    UiStatus(USR_COMMS_ACTIVE,"Comms");
    MS_DBG(F("  Modem setup & Timesync at init"));
    dataLogger.syncRTC();
    nistSyncRtc = false;
#endif //ARDUINO_ARCH_SAMD

    Logger::markTime(); //Init so never zero
    //del &DEEP_DEBUGGING_SERIAL_OUTPUT nanolevel_snsr.setDebugStream(&SerialTty);
    //modbusSerial.setDebugStream(&SerialTty);
    //dataLogger.systemSleep();
    //while (1) { greenredflash(4,500); delay(2000); }
    UiStatus(USR_OFF,"Started");
}


// **************************************************************************
// processSensors function
// **************************************************************************
//#if KCONFIG_DEBUG_LEVEL > 0
void processSensors()
{

    // Set sensors and file up if it hasn't happened already
    // NOTE:  Unless it completed in less than one second, the sensor set-up
    // will take the place of logging for this interval!
    // dataLogger.setupSensorsAndFile(); !v0.21.2 see v0.19.6 replaced varArray.setupSensors();

    dataLogger.watchDogTimer.resetWatchDog();
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (dataLogger.checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        //Logger::isLoggingNow = true;

        if (PS_LBATT_UNUSEABLE_STATUS==mcuBoard.isBatteryStatusAbove(true,PS_PWR_USEABLE_REQ)) {
            PRINTOUT(F("---NewReading CANCELLED--Lbatt_V="),mcuBoard.getBatteryVm1(false));
            return;
        }
        // Print a line to show new reading
        PRINTOUT(F("---NewReading--Complete Sensor Update"));
        MS_DBG(F("Lbatt_V="),mcuBoard.getBatteryVm1(false));
        //PRINTOUT(F("----------------------------\n"));
        #if !defined(CHECK_SLEEP_POWER)
        // Turn on the LED to show we're taking a reading
        //digitalWrite(greenLED, HIGH);
        // Turn on the LED to show we're taking a reading
        //dataLogger.alertOn();

        UiStatus(USR_SENSORS_ACTIVE,"Readings");
#if defined(CONFIG_SENSOR_RS485_PHY)
        // Start the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        #if defined CONFIG_HW_RS485PHY_DIR_PIN 
        pinPeripheral(CONFIG_HW_RS485PHY_DIR_PIN, PIO_SERCOM_ALT);
        #endif //CONFIG_HW_RS485PHY_DIR_PIN 
        modbusSerial.begin(9600);
#endif // CONFIG_SENSOR_RS485_PHY
        #ifdef loggingMultiplier_MAX_CDEF
        // Do a complete sensor update
        varArrFast.completeUpdate();
        //uint16 dataLogFast.getValueStringAtI(0)
        float lastReading=variableLstFast[0]->getValue();
        bool readingUpdated =false;
        if (lastReading < ina219M_A_LowReading) {
            MS_DBG(F("ina219Alow reading="),lastReading,F("lower than"),ina219M_A_LowReading);
            ina219M_A_LowReading =lastReading;
            readingUpdated =true;
        } 
        if  (lastReading >ina219M_A_HighReading){
            MS_DBG(F("ina219Ahigh reading="),lastReading,F("higher than"),ina219M_A_HighReading);
            ina219M_A_HighReading =lastReading;
            readingUpdated =true;
        }
        if (false==readingUpdated) {
             MS_DBG(F("ina219 reading="),lastReading,F("within"),ina219M_A_LowReading,F("~"),ina219M_A_HighReading);
        }
        if (loggingMultiplierTop<= ++loggingMultiplierCnt)
        #endif //loggingMultiplier_MAX_CDEF 
        {
            #if defined loggingMultiplier_MAX_CDEF
            dataLogger.watchDogTimer.resetWatchDog();
            varArrFast.completeUpdate();
            #endif //loggingMultiplier_MAX_CDEF
            dataLogger.watchDogTimer.resetWatchDog();
            varArray.completeUpdate();
            loggingMultiplierCnt=0;
            varArrayPub=true;
        }

#if defined(CONFIG_SENSOR_RS485_PHY)
        // End the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.end();
        // Reset AltSoftSerial pins to LOW, to reduce power bleed on sleep, 
        // because Modbus Stop bit leaves these pins HIGH
        pinMode(RS485PHY_TX_PIN, OUTPUT);  // AltSoftSerial Tx pin
        pinMode(RS485PHY_RX_PIN, OUTPUT);  // AltSoftSerial Rx pin
        pinMode(RS485PHY_DIR_PIN, OUTPUT);
        digitalWrite( RS485PHY_TX_PIN, LOW);   // Reset AltSoftSerial Tx pin to LOW
        digitalWrite( RS485PHY_RX_PIN, LOW);   // Reset AltSoftSerial Rx pin to LOW
        digitalWrite( RS485PHY_DIR_PIN, LOW);   // Reset AltSoftSerial Rx pin to LOW
#endif //CONFIG_SENSOR_RS485_PHY

        if (varArrayPub) {
            UiStatus(USR_COMMS_ACTIVE,"Comms");
            varArrayPub = false;
            dataLogger.watchDogTimer.resetWatchDog();
            // Create a csv data record and save it to the log file
            dataLogger.logToSD();

            // Turn on the modem to let it start searching for the network

            //if Modem  is Cellular then PS_PWR_HEAVY_REQ
            if (PS_LBATT_UNUSEABLE_STATUS==mcuBoard.isBatteryStatusAbove(false,PS_PWR_MEDIUM_REQ)) 
            {          
                PRINTOUT(F("---NewCloud Update CANCELLED--Lbatt_V="),mcuBoard.getBatteryVm1(false));
            } else 
            {
                //if (dataLogger._logModem != NULL)
                {
                    #if defined UseModem_Module
                    //Power up and enable pins
                    modemSerial.begin(modemBaud);
                    modemPhy.modemPowerUp();
                    if (modemResetPin >= 0)
                    {
                        pinMode(modemResetPin, OUTPUT);
                        digitalWrite(modemResetPin, modemResetState_DEF);
                    }
                    if (modemSleepRqPin >= 0)
                    {
                        pinMode(modemSleepRqPin, OUTPUT);
                        digitalWrite(modemSleepRqPin, modemSleepRqState_DEF);
                    }
                    #if 0                    
                    if (!modemSetup) {
                        modemSetup = true;
                        MS_DBG(F("  Modem setup up 1st pass"));
                        // The first time thru, setup modem. Can't do it in regular setup due to potential power drain.
                        modemPhy.modemWake();  // Turn it on to talk
                        //protected ?? modemPhy.extraModemSetup();//setupXBee();
                        nistSyncRtc = true;
                    }
                    #endif
                    dataLogger.watchDogTimer.resetWatchDog();
                    //MS_DBG(F("  Waking up... "));
                    MS_DBG(F("  Waking up"), modemPhy.getModemName(), F("..."));
                    if (modemPhy.modemWake()) {
                        // Connect to the network
                        MS_DBG(F("  Connecting to the Internet... "));
                        if (modemPhy.connectInternet())
                        {
                            dataLogger.watchDogTimer.resetWatchDog();
                            MS_DBG(F("  publishing... "),modemPhy.getModemName());
                            // Post the data to the WebSDL
                            dataLogger.publishDataToRemotes();
                            
                            //Sync the RTC once a day, or if debug once an hour.
                            #define DAY_SECS 86400
                            #define HOUR_SECS 3600
                            #if defined MS_DEBUG_THIS_MODULE
                            #define CONFIG_NIST_CHECK_SECS HOUR_SECS
                            #else
                            #define CONFIG_NIST_CHECK_SECS DAY_SECS
                            #endif
                            #define CONFIG_NIST_ERR_MASK (~0x3F) 
                            uint32_t nistCheckRemainder = Logger::markedEpochTime % CONFIG_NIST_CHECK_SECS;
                            bool nistSyncNow=false;
                            if (nistSyncRtc || ((nistCheckRemainder&CONFIG_NIST_ERR_MASK ) == 0)) 
                            {
                                nistSyncNow=true;
                                PRINTOUT(F("SyncTimeCheck Atmpt "),nistSyncRtc,Logger::markedEpochTime
                                ,"remainder ",nistCheckRemainder
                                ," check+-",(nistCheckRemainder&CONFIG_NIST_ERR_MASK) );
                            } else {
                                PRINTOUT(F("SyncTimeCheck NotNeeded "),Logger::markedEpochTime
                                ,"remainder ",nistCheckRemainder
                                ," check+-",(nistCheckRemainder&CONFIG_NIST_ERR_MASK) );                          
                            }
                            if (nistSyncNow )
                            {
                                dataLogger.watchDogTimer.resetWatchDog();
                                MS_DBG(F("  atl..Running a NIST clock sync. NeedSync "),nistSyncRtc);
                                nistSyncRtc = true; //Needs to run every access until sucess
                                if (true == dataLogger.syncRTC()) {
                                    nistSyncRtc = false; //Sucess
                                } 
                                //If time very different should ensure publish is accurate - possibly cancel
                            }

                            // Disconnect from the network
                            MS_DBG(F("  Disconnecting from the Internet..."));
                            modemPhy.disconnectInternet();
                        } else {MS_DBG(F("  No internet connection..."));}
                    }
                    // Ensure no leakage on pins
                    if (modemResetPin >= 0)
                    {
                        pinMode(modemResetPin, OUTPUT);
                        digitalWrite(modemResetPin, LOW);
                    }
                    if (modemSleepRqPin >= 0)
                    {
                        pinMode(modemSleepRqPin, OUTPUT);
                        digitalWrite(modemSleepRqPin, LOW);
                    }
                    pinMode(MODEMPHY_TX_PIN, OUTPUT);
                    pinMode(MODEMPHY_RX_PIN, OUTPUT);
                    digitalWrite(MODEMPHY_TX_PIN, LOW);
                    digitalWrite(MODEMPHY_RX_PIN, LOW);
                    // Turn the modem off
                    modemPhy.modemSleepPowerDown();
                    #endif //UseModem_Module
                } //else MS_DBG(F("  No Modem configured.\n"));
                PRINTOUT(F("---Complete "));
            }
            ina219M_A_init();
            // Cut power from the SD card - without additional housekeeping wait
            dataLogger.turnOffSDcard(false);        
            // Turn off the LED
            //digitalWrite(greenLED, LOW);
            dataLogger.alertOff();
            // Print a line to show reading ended
        }

        #endif //(CHECK_SLEEP_POWER)
        // Unset flag
        //Logger::isLoggingNow = false;
    }
    UiStatus(USR_OFF,"Sleep");
}
//#endif //KCONFIG_DEBUG_LEVEL > 0  
// ==========================================================================
int flash_lp=0;
void loop()
{

    #if KCONFIG_DEBUG_LEVEL==0
        flash_lp++;

        SerialStd.print(F("Current Time ("));
        SerialStd.print(flash_lp);
        SerialStd.print(F(" ):"));
        SerialStd.println(Logger::formatDateTime_ISO8601(dataLogger.getNowEpochTz()) );
        //SerialStd.println();
        greenredflash();
        delay(2000);
    #elif KCONFIG_DEBUG_LEVEL > 0

        processSensors();
        // Check if it was instead the testing interrupt that woke us up
        // not implemented yet: if (EnviroDIYLogger.startTesting) EnviroDIYLogger.testingMode();

        // Sleep
        //if(_mcuWakePin >= 0){systemSleep();}
            #if defined USE_USB_MSC_SD0 
            while (dataLogger.usbDriveActive()) {
                // USB is plugged in, uP can't sleep until USB is removed.
                MS_DBG(F(" USB is active, Poll for SD change, Wait 2Sec."));
                dataLogger.SDusbPoll(0);
                delay(2000);
            };
            #endif //USE_USB_MSC_SD0
        #define timeNow() dataLogger.formatDateTime_ISO8601(dataLogger.getNowEpoch())

            #if defined loggingMultiplier_MAX_CDEF
                MS_DBG(F("dataLogFast Sleep "),timeNow());
                dataLogFast.systemSleep();
            #else 
                MS_DBG(F("dataLogger Sleep "),timeNow());
                dataLogger.systemSleep();
            #endif //loggingMultiplier_MAX_CDEF
        MS_DBG(F("dataLogger Wake "),timeNow());
    #endif //KCONFIG_DEBUG_LEVEL
    #if defined(CHECK_SLEEP_POWER)
        PRINTOUT(F("A"));
    #endif //(CHECK_SLEEP_POWER)

}
//The following is a holding place for WIRING_DIGITAL_DEBUG

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
/* ************************************************************
 * Catch the extended MCP processing
 * This assumes the B031r2+ expansion
 */
void digitalWrExt( uint32_t ulPin, uint32_t ulVal );
void digitalWrExt( uint32_t ulPin, uint32_t ulVal ) {
    if (ulPin < thisVariantNumPins) {
        MS_DBG("***digitalWrExt Err ",ulPin,"=",ulVal);  
    } else {
        uint32_t vextPin =  ulPin - thisVariantNumPins;
        /* This extends the virtual "Arduino Pins" as follows
           virtualExtPins 
             0..15 Literal Digital Pins on MCP port extender
            16..23 Analog pins MC74VHC4051- 8pins
            Practically the 8 Analog Pins are encoded in 4pins (MSD)PB4,PB3,PB2(LSD)
            and a write here means write to hardware mux.
            16   000
            17   001
            ........
            23   111
        */
        if (ARD_DIGITAL_EXTENSION_PINS > vextPin) {
            MS_DEEP_DBG("***digitalWrExtD ",mcpExp.getPortStr(vextPin),ulPin,"(",vextPin,")=",ulVal); 
            mcpExp.setBit((peB031_bit)(vextPin),ulVal);
        } else {
            vextPin = vextPin - ARD_DIGITAL_EXTENSION_PINS; //Normalize of Analog range
            MS_DEEP_DBG("***digitalWrExtA ",ulPin,"(",vextPin,")=",ulVal); 
            mcpExp.setupAnalogPin(vextPin,ulVal);
        }
    }
}

void pinModExt( uint32_t ulPin, uint32_t ulMode ) {
    if (ulPin < thisVariantNumPins) {
        MS_DBG("***pinModeExt Err ",ulPin,"=",ulMode);  
    } else {
        #if MS_DEBUG_THIS_MODULE > 1
        uint32_t mcpPin =  ulPin - thisVariantNumPins;
        MS_DEEP_DBG("***pinModExt Unhandled ",mcpExp.getPortStr(mcpPin),ulPin,"(",mcpPin,")=",ulMode);  
        #endif //MS_DEBUG_THIS_MODULE  
    }
}
uint8_t digitalRdExt( uint32_t ulPin ) {
    uint8_t pinState=0;
    if (ulPin < thisVariantNumPins) {
        MS_DBG("***digitalRdExt Err",ulPin);  
    } else {
        uint32_t mcpPin =  ulPin - thisVariantNumPins;
        pinState=mcpExp.digitalRead(mcpPin);
        MS_DEEP_DBG("***digitalRdExt ",mcpExp.getPortStr(mcpPin),ulPin,"(",mcpPin,")=",pinState);  
    }    
    return pinState;
}
int digitalRdMir( uint32_t ulPin ) {
    bool pinState=0;
    if (ulPin < thisVariantNumPins) {
        MS_DBG("***digitalRdMir Err",ulPin);  
    } else {
        uint32_t mcpPin =  ulPin - thisVariantNumPins;
        pinState=mcpExp.rdMir((peB031_bit)mcpPin);
        MS_DEEP_DBG("***digitalRdMir ",mcpExp.getPortStr(mcpPin),ulPin,"(",mcpPin,")=",pinState);  
    }    
    return (int)pinState;
}

#ifdef __cplusplus
}
#endif //__cplusplus
