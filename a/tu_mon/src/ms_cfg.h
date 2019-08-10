/*****************************************************************************
ms_cfg.h  - ModularSensors Configuration - a wip - WorkInProgress
Written By:  Neil Hancock www.envirodiy.org/members/neilh20/
Development Environment: PlatformIO
Hardware Platform(s): EnviroDIY Mayfly Arduino Datalogger+RS485 Wingboard
    Adafruit ADAFRUIT_FEATHER_M4_EXPRESS, (Transitional support for FEATHER_M0_EXPRESS/AUTONOMO but likely to be superseded)
Software License: BSD-3.
  Copyright (c) 2018, Neil Hancock - all rights assigned to Stroud Water Research Center (SWRC)
  and they may change this title to Stroud Water Research Center as required
  and the EnviroDIY Development Team

This is written for ModularSensors library version 0.21.3 or greater

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/
#ifndef ms_cfg_h
#define ms_cfg_h
// Local default defitions here
// FUT: Some board level would be in a per board level persistent storage - eg FLASH
// These are either pre .ini read or per board defintions
#define MFsn_def  F("unknown")

// Boards functions are modified based on name
//#define USE_SD_MAYFLY_INI0 0



//Required for TinyGsmClient.h - select one
#define TINY_GSM_MODEM_XBEE
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
// End TinyGsmClient.h options

// Function testing
// The following MS_PROFILES are supported.
// A profile is a defined set of Modular Sensor features 
#define PROFILE01_MAYFLY_AVR 01
#define PROFILE04_ADAFRUIT_FEATHER_M4 02
#define PROFILE02_ADAFRUIT_FEATHER_M0 03
#define PROFILE03_SODAQ_AUTONOMO_M0   04



//The following is expected to be steered by PlatformIo.ini [env:xxx]
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
#define PROFILE_NAME PROFILE01_MAYFLY_AVR
#elif defined(ADAFRUIT_FEATHER_M4_EXPRESS)
//"ARDUINO_SAMD_FEATHER_M4"  ADAFRUIT_FEATHER_M4_EXPRESS
#define PROFILE_NAME PROFILE04_ADAFRUIT_FEATHER_M4
//#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
#elif defined(ARDUINO_SAMD_FEATHER_M0)
#define PROFILE_NAME PROFILE02_ADAFRUIT_FEATHER_M0
//#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS)
#elif defined(ARDUINO_SODAQ_AUTONOMO)
#define PROFILE_NAME PROFILE03_SODAQ_AUTONOMO_M0
#else
#error undefinded
#endif
//Still a WIP
//#define PROFILE02 02
//#define  PROFILE_NAME PROFILE02

#if PROFILE_NAME == PROFILE01_MAYFLY_AVR
//**************************************************************************

//Standard - target TU power Mon using INA219 0-10A, 0-16V
// Wireless XbeeS6 wiFi and Xbee LTE 
//This is hardcoded to mean things in ProcessorStats !!!!
//This defines rev 0.5ba changes for Mayfly. 
// Rev0.5ba is an enhancement on 0.5b
// C4 removed, strap for AA2/Vbat AA3/SolarV, C 100uF Cer across XbeeVcc
#define MFVersion_DEF "v0.5b"
#define MFName_DEF "Mayfly"
#define HwVersion_DEF MFVersion_DEF
#define HwName_DEF MFName_DEF
#define USE_SD_MAYFLY_INI 1

#define greenLEDPin 8        // MCU pin for the green LED (-1 if not applicable)
#define redLEDPin   9        // MCU pin for the red LED (-1 if not applicable)

#define sensorPowerPin_DEF 22
#define modemSleepRqPin_DEF 23
#define modemStatusPin_DEF  19  // MCU pin used to read modem status (-1 if not applicable)
#define modemResetPin_DEF A4    // MCU pin connected to modem reset pin (-1 if unconnected)

#define LOGGERID_DEF_STR "msLog01"
#define NEW_LOGGERID_MAX_SIZE 40
//#define NEW_LOGGERID_MAX_SIZE  3 ///Test
#define configIniID_DEF_STR "ms_cfg.ini"  
#define CONFIG_TIME_ZONE_DEF -8

// How frequently (in minutes) to log data
//#define loggingInterval_CDEF_MIN 15
//define one  _Module
//#define DigiXBeeLTE_Module 
#define DigiXBeeWifi_Module

//end of _Module

#define APN_CDEF  "xxxx" // The APN for the gprs connection, unnecessary for WiFi
#define WIFIID_CDEF  "xxxx"  // The WiFi access point, unnecessary for gprs
#define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define KellerAcculevel_ACT 1
//Defaults for data.envirodiy.org
#define registrationToken_UUID "registrationToken_UUID"
#define samplingFeature_UUID   "samplingFeature_UUID"
#define KellerNanolevel_ACT 1
#ifdef KellerNanolevel_ACT
  #define CONFIG_SENSOR_RS485_PHY 1
  //Mayfly definitions
  #define CONFIG_HW_RS485PHY_TX_PIN 5  //FUT: mv to ms_cfg.h AltSoftSerial Tx pin 
  #define CONFIG_HW_RS485PHY_RX_PIN 6  // AltSoftSerial Rx pin
  #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
  #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
#endif // KellerNanolevel_ACT
#define INA219M_PHY_ACT 1
#ifdef INA219M_PHY_ACT
  #define INA219M_MA_UUID      "INA219_MA_UUID"
  #define INA219M_VOLT_UUID    "INA219_VOLT_UUID"
#endif //INA219_PHY_ACT

#ifdef ARDUINO_AVR_ENVIRODIY_MAYFLY
#define MaximDS3231_Temp_UUID       "MaximDS3231_Temp_UUID"
#endif //ARDUINO_AVR_ENVIRODIY_MAYFLY
//#define Modem_RSSI_UUID ""
// Seems to cause XBEE WiFi S6 to crash
//#define Modem_SignalPercent_UUID    "SignalPercent_UUID"
#define ProcessorStats_ACT 1
#define ProcessorStats_SampleNumber_UUID  "SampleNumber_UUID"
#define ProcessorStats_Batt_UUID          "Batt_UUID"

#define ExternalVoltage_ACT 1
#ifdef ExternalVoltage_ACT
  #define ExternalVoltage_Volt0_UUID "Volt0_UUID"
  //#define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
#endif //ExternalVoltage_ACT

#elif PROFILE_NAME == PROFILE04_ADAFRUIT_FEATHER_M4
  //**************************************************************************
  /*cmd line -D ARDUINO_ARCH_SAMD  ARDUINO_FEATHER_M4 ADAFRUIT_FEATHER_M4_EXPRESS
    __SAMD51J19A__ __SAMD51__ __FPU_PRESENT ARM_MATH_CM4 
    Add to cmd line
    -D SERIAL2_EN -D SERIAL4_EN 

    Board FeatherM4Express            
  -----  ---------------    
    USB   Serial SerialUSB
  SERCOM0 Serial2 (A1/A4/A5)      
  SERCOM1 SPI (MISO/MOSI/SCK)           
  SERCOM2 I2C (SDA/SCL)                        
  SERCOM3 Serial3 (D12/D13/D10)           
  SERCOM4 Serial4 (A3/A2/D10)     
  SERCOM5 Serial1/Bee (DO/D1)                      
  QSPI    2Mbytes SD Flash drive 
  */
  //#define SENSOR_RS485_PHY 1
  //Standard 
  //This is hardcode to mean things in ProcessorStats !!!!
  //For Adafruit Feather M0 (not Feather M0 Express?)
  #define HwVersion_DEF "r1"
  #define HwName_DEF "FeatherM4"

  //The FEATHER_M4_EXPRESS has 1 neoPixel + RedLED
  #define NUM_NEOPIXELS 1
  #define NEOPIXEL_PIN 8
  #define redLEDPin    13 
  //FEATHER_M4_EXPRESS has QSPI 2M FLASH
  #define SD_QSPI_2MFLASH

  //The FEATHERWING_RTC_SD has RTC PCF8523 + uSD/SPI with CS/GPI10
  #define ADAFRUIT_FEATHERWING_RTC_SD
  #define USE_SD_MAYFLY_INI 1
  #define SD_SPI_CARD_PIN_DEF 10


  #define LOGGERID_DEF_STR "msLog01"
  #define NEW_LOGGERID_MAX_SIZE 40
  //#define NEW_LOGGERID_MAX_SIZE  3 ///Test
  #define configIniID_DEF_STR "ms_cfg.ini"  
  #define CONFIG_TIME_ZONE_DEF -8

  // How frequently (in minutes) to log data
  //#define  loggingInterval_CDEF_MIN 5
  #define  loggingInterval_MAX_CDEF_MIN 120
//define one  _Module
//#define DigiXBeeLTE_Module 
#define DigiXBeeWifi_Module

//end of _Module
  #define APN_CDEF  "xxxx" // The APN for the gprs connection, unnecessary for WiFi
  #define WIFIID_CDEF  "xxxx"  // The WiFi access point, unnecessary for gprs
  #define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

  //#define SENSOR_CONFIG_GENERAL 1
  //#define KellerAcculevel_ACT 1
  //Defaults for data.envirodiy.org
  #define registrationToken_UUID "registrationToken_UUID"
  #define samplingFeature_UUID   "samplingFeature_UUID"

  /* variant.cpp std defines  Serial1 
  variant.cpp enhanced for  Serial2 3 4
variant.h: has pin definitions
#define PIN_SERIALx_RX       (0ul)  FTDI Pin4
#define PIN_SERIALx_TX       (1ul)  FTDI Pin5
*/
#define SerialModem Serial1

#if defined SERIAL2_EN
#define SerialModbus Serial2
//to be used by modbusSerial 
#endif // SERIAL2_EN
#if defined SERIAL3_EN
//For M4express the RedLed is also on Serial3RX
//#define SerialExt3 Serial3
#endif //SERIAL3_EN
#if defined SERIAL4_EN
//#define SerialExt4 Serial4
//add -DSTANDARD_SERIAL_OUTPUT=Serial4
#define SerialTty Serial4
//#define SerialStd Serial4
#define STANDARD_SERIAL_OUTPUT Serial4
#endif // SERIAL4_EN

  //#define KellerNanolevel_ACT 1
  #ifdef KellerNanolevel_ACT
    #define SENSOR_RS485_PHY 1
    #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
    #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
  #endif //KellerNanolevel_ACT
  //#define INA219M_PHY_ACT 
  #ifdef INA219M_PHY_ACT
    #define INA219M_MA_UUID              "INA219_MA_UUID"
    #define INA219M_VOLT_UUID            "INA219_VOLT_UUID"
  #endif //INA219_PHY_ACT

  //#define Modem_RSSI_UUID ""
  // Seems to cause XBEE WiFi S6 to crash
  //#define Modem_SignalPercent_UUID    "SignalPercent_UUID"
  #define ProcessorStats_ACT 1
  #define ProcessorStats_SampleNumber_UUID  "SampleNumber_UUID"
  #define ProcessorStats_Batt_UUID          "Batt_UUID"

  //#define ExternalVoltage_ACT 1
  #ifdef ExternalVoltage_ACT
  #define ExternalVoltage_Volt0_UUID "Volt0_UUID"
  #define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
  #endif //ExternalVoltage_ACT

#elif PROFILE_NAME == PROFILE03_SODAQ_AUTONOMO_M0
//**************************************************************************
//#define SENSOR_RS485_PHY 1
//Standard 
//This is hardcode to mean things in ProcessorStats !!!!
//For Autonomo
#define AutonomoRev_DEF "r5"
#define AutonomoName_DEF "Autonomo"
#define HwVersion_DEF AutonomoRev_DEF
#define HwName_DEF AutonomoName_DEF

#define USE_SD_MAYFLY_INI 1
/*Autonom has built in BEE, on sleep1 with 5 control pins
BEE_VCC PowerEn=H -seperate regulator 
input DTR shared ArduinoA8
inputXbee CTS
Out  Xbee autonomoModemAssocPin
inXbee RTS

*/
#define modemVccPin_DEF BEE_VCC
#define autonomoModemRtsPin   BEERTS //PB22 same as MCU_CTS output
#define autonomoModemCtsPin   BEECTS //PB23 output
#define autonomoModemDtrPin   PIN_A13 //Shared
#define autonomoModemAssocPin RI_AS  //input


#define LOGGERID_DEF_STR "msLog01"
#define NEW_LOGGERID_MAX_SIZE 40
//#define NEW_LOGGERID_MAX_SIZE  3 ///Test
#define configIniID_DEF_STR "ms_cfg.ini"  
#define CONFIG_TIME_ZONE_DEF -8

// How frequently (in minutes) to log data
//#define  loggingInterval_CDEF_MIN 5

//define one  _Module
//#define DigiXBeeLTE_Module 
#define DigiXBeeWifi_Module 1
#warning DigiXBeeWifi_Module
//end of _Module

#define  loggingInterval_MAX_CDEF_MIN 120
#define APN_CDEF  "xxxx" // The APN for the gprs connection, unnecessary for WiFi
#define WIFIID_CDEF  "xxxx"  // The WiFi access point, unnecessary for gprs
#define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define KellerAcculevel_ACT 1
//Defaults for data.envirodiy.org
#define registrationToken_UUID "registrationToken_UUID"
#define samplingFeature_UUID   "samplingFeature_UUID"
//#define KellerNanolevel_ACT 1
#ifdef KellerNanolevel_ACT
  #define SENSOR_RS485_PHY 1
  #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
  #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
#endif //KellerNanolevel_ACT
//#define INA219M_PHY_ACT 
#ifdef INA219M_PHY_ACT
  #define INA219M_MA_UUID              "INA219_MA_UUID"
  #define INA219M_VOLT_UUID            "INA219_VOLT_UUID"
#endif //INA219_PHY_ACT

//#define Modem_RSSI_UUID ""
// Seems to cause XBEE WiFi S6 to crash
//#define Modem_SignalPercent_UUID    "SignalPercent_UUID"
#define ProcessorStats_ACT 1
#define ProcessorStats_SampleNumber_UUID  "SampleNumber_UUID"
#define ProcessorStats_Batt_UUID          "Batt_UUID"

//#define ExternalVoltage_ACT 1
#ifdef ExternalVoltage_ACT
#define ExternalVoltage_Volt0_UUID "Volt0_UUID"
#define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
#endif //ExternalVoltage_ACT
#elif PROFILE_NAME == PROFILE02_ADAFRUIT_FEATHER_M0
//**************************************************************************
//#define SENSOR_RS485_PHY 1
//Standard 
//This is hardcode to mean things in ProcessorStats !!!!
//For Adafruit Feather M0 (not Feather M0 Express?)
#define HwVersion_DEF "r1"
#define HwName_DEF "FeatherM0"

#define USE_SD_MAYFLY_INI 1

#define LOGGERID_DEF_STR "msLog01"
#define NEW_LOGGERID_MAX_SIZE 40
//#define NEW_LOGGERID_MAX_SIZE  3 ///Test
#define configIniID_DEF_STR "ms_cfg.ini"  
#define CONFIG_TIME_ZONE_DEF -8

// How frequently (in minutes) to log data
//#define  loggingInterval_CDEF_MIN 5
#define  loggingInterval_MAX_CDEF_MIN 120
#define APN_CDEF  "xxxx" // The APN for the gprs connection, unnecessary for WiFi
#define WIFIID_CDEF  "xxxx"  // The WiFi access point, unnecessary for gprs
#define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define KellerAcculevel_ACT 1
//Defaults for data.envirodiy.org
#define registrationToken_UUID "registrationToken_UUID"
#define samplingFeature_UUID   "samplingFeature_UUID"
//#define KellerNanolevel_ACT 1
#ifdef KellerNanolevel_ACT
  #define SENSOR_RS485_PHY 1
  #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
  #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
#endif //KellerNanolevel_ACT
//#define INA219M_PHY_ACT 
#ifdef INA219M_PHY_ACT
  #define INA219M_MA_UUID              "INA219_MA_UUID"
  #define INA219M_VOLT_UUID            "INA219_VOLT_UUID"
#endif //INA219_PHY_ACT

//#define Modem_RSSI_UUID ""
// Seems to cause XBEE WiFi S6 to crash
//#define Modem_SignalPercent_UUID    "SignalPercent_UUID"
#define ProcessorStats_ACT 1
#define ProcessorStats_SampleNumber_UUID  "SampleNumber_UUID"
#define ProcessorStats_Batt_UUID          "Batt_UUID"

//#define ExternalVoltage_ACT 1
#ifdef ExternalVoltage_ACT
#define ExternalVoltage_Volt0_UUID "Volt0_UUID"
#define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
#endif //ExternalVoltage_ACT

#elif 0 //PROFILE_NAME == PROFILE03
//**************************************************************************
//Keller Nanolevel with XBP-u.fl 
#define SENSOR_RS485_PHY 1
#define MFVersion_DEF "v0.5ba"

// How frequently (in minutes) to log data
//#define  loggingInterval_CDEF_MIN 5
const char *apn_def = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId_def = "AzondeNetSsid";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd_def = NULL;//"";  // The password for connecting to WiFi, unnecessary for gprs
//#define SENSOR_CONFIG_GENERAL 1
//#define SENSOR_CONFIG_KELLER_ACCULEVEL 1

#define registrationToken_UUID "d96bf9fb-faca-4cc3-bcb9-3d23255a1f3c"
#define samplingFeature_UUID   "79f702a9-368f-4940-9669-8978ffa3254b"

#define KellerNanolevel_ACT 1
#define KellerNanolevel_Height_UUID "67c22f5d-e5d8-4a26-a82b-7f59132e5c81"
#define KellerNanolevel_Temp_UUID   "b03d4384-4623-4df5-b705-f50710d5e4e9"

#define ProcessorStats_ACT 1
#define ProcessorStats_Batt_UUID    "8c796edd-7863-4fe7-9e54-0cbe0d694d59"
// The following mapped to Mayfly_FreeRAM
#define ProcessorStats_SampleNum_UUID "5fbb799d-630d-486f-a0ff-015f0195d393"

#define MaximDS3231_Temp_UUID       "e6159fe0-e30d-4a9d-bebc-1dc5c2435a22"

//#define Modem_RSSI_UUID ""
// Try without as something crashing Mayfly
//#define Modem_SignalPercent_UUID    ""

#define ExternalVoltage_ACT 1
#define Volt0_UUID "d3b78c2e-312b-4e2a-b804-8230c963f912"
#define Volt1_UUID "c7da692b-6661-4545-bd3d-04938faa285b"


#elif !defined PROFILE_NAME
#error Undefined PROFILE_NAME
#endif //PROFILE_NAME

/* Put defintions that need to be avaialbe 
*/

#if !defined modemVccPin_DEF
#define modemVccPin_DEF -2
#endif

#if !defined sensorPowerPin_DEF
#define      sensorPowerPin_DEF -1
#endif

#if !defined modemSleepRqPin_DEF
#define      modemSleepRqPin_DEF -1
#endif
#if !defined modemStatusPin_DEF
#define      modemStatusPin_DEF -1  
#endif
#if !defined modemResetPin_DEF
// MCU pin connected to modem reset pin (-1 if unconnected)
#define      modemResetPin_DEF -1    
#endif

#if !defined loggingInterval_CDEF_MIN
#define      loggingInterval_CDEF_MIN 15   
#endif
#if !defined loggingInterval_MAX_CDEF_MIN
//This is default for testing - platforms should set own MAX
#define  loggingInterval_MAX_CDEF_MIN 15
#endif 
#if !defined NEW_LOGGERID_MAX_SIZE
#define NEW_LOGGERID_MAX_SIZE 40
#endif 

#endif //ms_cfg_h
