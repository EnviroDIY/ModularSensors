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


DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/
#ifndef ms_cfg_h
#define ms_cfg_h
#include <Arduino.h>  // The base Arduino library
// Local default defitions here
// FUT: Some board level would be in a per board level persistent storage - eg FLASH
// These are either pre .ini read or per board defintions
#define MFsn_def  F("unknown")

// Boards functions are modified based on name 1 or 2
//#define USE_MS_SD_INI 1



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
//"ARDUINO_SAMD_FEATHER_M4" or  ADAFRUIT_FEATHER_M4_EXPRESS
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
//This configuration is for a standard Mayfly0.bb
// Not expected to be used as softserial stopped working
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
#define USE_MS_SD_INI 1

#define greenLEDPin 8        // MCU pin for the green LED (-1 if not applicable)
#define redLEDPin   9        // MCU pin for the red LED (-1 if not applicable)

#define sensorPowerPin_DEF 22
#define modemSleepRqPin_DEF 23
#define modemStatusPin_DEF  19  // MCU pin used to read modem status (-1 if not applicable)
#define modemResetPin_DEF   20  // MCU pin connected to modem reset pin (-1 if unconnected)

#define LOGGERID_DEF_STR "msLog01"
#define NEW_LOGGERID_MAX_SIZE 40
//#define NEW_LOGGERID_MAX_SIZE  3 ///Test
#define configIniID_DEF_STR "ms_cfg.ini"  
#define CONFIG_TIME_ZONE_DEF -8

//??#define USE_RTCLIB RTC_DS3231  //needs checking

// ** How frequently (in minutes) to log data **
//For two Loggers defined logger2Mult with the faster loggers timeout and the multiplier to the slower loggger 
#define  loggingInterval_Fast_MIN (1)
#define  logger2Mult 5 //Not working for mayfly
// How frequently (in minutes) to log data
#if defined logger2Mult
#define  loggingInterval_CDEF_MIN (loggingInterval_Fast_MIN*logger2Mult) 
#else
#define  loggingInterval_CDEF_MIN 15
#endif //logger2Mult
// Maximum logging setting allowed
#define  loggingInterval_MAX_CDEF_MIN 6*60

//define one  _Module
#define DigiXBeeWifi_Module 1
#warning infoMayflyWithDigiXBeeWiFi
//#define DigiXBeeCellularTransparent_Module 1
//#warning infoMayflyWithDigiXBeeCellTransparent
// #define DigiXBeeLTE_Module 1 - unstable

//end of _Module

#define APN_CDEF  "def_apn" // The APN for the gprs connection, unnecessary for WiFi
#define WIFIID_CDEF  "xxxx"  // The WiFi access point, unnecessary for gprs
#define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define KellerAcculevel_ACT 1
//Defaults for data.envirodiy.org
#define registrationToken_UUID "registrationToken_UUID"
#define samplingFeature_UUID   "samplingFeature_UUID"
//#define KellerNanolevel_ACT 1
#ifdef KellerNanolevel_ACT
  #define CONFIG_SENSOR_RS485_PHY 1
  //Mayfly definitions
  #define CONFIG_HW_RS485PHY_TX_PIN 5  //Mayfly OCRA1 map AltSoftSerial Tx pin 
  #define CONFIG_HW_RS485PHY_RX_PIN 6  //Mayfly ICES1 map AltSoftSerial Rx pin
  #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
  #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
#endif // KellerNanolevel_ACT
//#define INA219M_PHY_ACT 1
#ifdef INA219M_PHY_ACT
  #define INA219M_MA_UUID      "INA219_MA_UUID"
  #define INA219M_VOLT_UUID    "INA219_VOLT_UUID"
#endif //INA219_PHY_ACT

 //On maylfy
//#define MaximDS3231_Temp_UUID       "MaximDS3231_TEMP_UUID"

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
//This configuration expects a standard ADAFRUIT_FEATHER_M4 
// plugged into a B031r3 with upgraded Arduino Wiring files for
// with a INA219(FeatherWing) connected on I2C 
// with a RTC_PCF2127 on I2C
// with a MCP on I2C
// Optional ADC Mux driven off MCP
// expects -DCUSTOM_B031
// B031r3 Hardware Support a numer of Buffered Phy, SOFTWARE only supports ONE of
//   CONFIG_SENSOR_RS485_PHY CONFIG_SENSOR_SDI12_PHY CONFIG_SENSOR_WIRE1_PHY
// Some SDI-12 instruments work with 3V data and 5Volt Power 
//This is hardcode to mean things in ProcessorStats !!!!
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

  //Standard 
  //This is hardcode to mean things in ProcessorStats !!!!
  #define HwVersion_DEF "r1"
  #define HwName_DEF "FeatherM4express"
#define HwFeatherWing_B031ALL 1
  //The FEATHER_M4_EXPRESS has 1 neoPixel + RedLED
  #define NUM_NEOPIXELS 1
  #define NEOPIXEL_PIN 8
  #define redLEDPin    13 
  //FEATHER_M4_EXPRESS has QSPI 2M FLASH
  #define SD_QSPI_2MFLASH

//The RTClib.h has a number of PHY which work together and replaced Sodaq_DS3231
  #define USE_RTCLIB RTC_PCF2127
  //#define ADAFRUIT_FEATHERWING_RTC_SD
  //#define USE_RTCLIBRTC PCF8523

  //The FEATHERWING_eInk 1.5" has uSD/SPI with CS/GPI10
  //The Feather_m4 has an board SD0(flashSPI) which is the source of where to look for the ini
  //Fut: USE_MS_SD_INI 1 would be onboard
  //Fut: USE_MS_SD_INI 2 eg  microSD - SD1
  #define USE_MS_SD_INI 1
  //An internal FS, and the USB maps to the SD0flashSPI 
  //#define BOARD_SDQ_QSPI_FLASH 1
  //-DUSE_TINYUSB on cmd line , which then turns off ints somewhere, and then doesn't sleep.
  #if (defined USE_TINYUSB)  && (defined BOARD_SDQ_QSPI_FLASH) 
  // 1st USE_USB_MSC_SD0  2nd USE_USB_MSC_SD1 (not compiling)
  #define USE_USB_MSC_SD0 1
  //#define USE_USB_MSC_SD1 1
  #endif //BOARD_SDQ_QSPI_FLASH 

  #define ADAFRUIT_FEATHERWING_eInk1_5in_SD 1
  #if defined ADAFRUIT_FEATHERWING_eInk1_5in_SD
    //B031rev3 Shared SPI with microSD CS
    #define SD_SPI_CS_PIN_DEF  5
    #define EPD_CS       9  //10 B031r3 &r2 SPI_ECS
    #define EPD_DC      10  //eInkDC B031r3 &r2=6
    #define SRAM_CS     -1 //not used
    #define EPD_RESET   -1  // can set to -1 and share with microcontroller Reset!
    #define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)
 #endif// ADAFRUIT_FEATHERWING_eInk1_5in_SD

//define one Radio  _Module
#define DigiXBeeWifi_Module 1
//#warning infoAutonomoWithDigiXBeeWiFi
//#define DigiXBeeCellularTransparent_Module 1
//#warning infoAutonomoWithDigiXBeeCellTransparent
// #define DigiXBeeLTE_Module 1 - unstable LTE BYPASS
#if defined(DigiXBeeWifi_Module) || defined(DigiXBeeCellularTransparent_Module) 
#define UseModem_Module 1
#endif // Modules
//end of _Module

  #define APN_CDEF  "hologram" // The APN for the gprs connection, unnecessary for WiFi
  #define WIFIID_CDEF  "AzondeNetSsid"  // The WiFi access point, unnecessary for gprs
  #define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

/* Needs specifying for Feather M4 B031rX */
// For no sleep/Always ON add to gcc cmn_build_flags (platformio.ini)
// For modemVccPIN_DEF no need for modemSleepRqPin
#if defined DigiXBeeWifi_Module
#define modemVccPin_DEF  eMcpA_SwV3Out_pinnum //-1 // B031r1:V3Sw mcpExp:PA0  for pwr off, other pins must be 0V
#else
//For B031Rev3 S4 Xbee Pwr is 3v3 unless manually switched, and strapped Vbat
#define modemVccPin_DEF eMcpA_SwV3Out_pinnum
#endif //DigiXBeeWifi_Module
//#define MODEMPHY_NEVER_SLEEPS 1
#if 0 //defined MODEMPHY_NEVER_SLEEPS || (-1!=modemVccPin_DEF)
#define modemResetPin_DEF -1
#define modemStatusPin_DEF  -1
#define modemSleepRqPin_DEF -1
#else
#define modemResetPin_DEF -1;//B031rev3 doesn't implement. Power cycle
#define modemStatusPin_DEF  eMcpA_XbeeOnSleepNin_pinnum //B031r1:XbeeOnSleepN Exp:PA5
#define modemSleepRqPin_DEF eMcpA_XbeeSleeRqOut_pinnum  //B031r1:XbeeSleepRq Exp:PA6
#endif //MODEMPHY_NEVER_SLEEPS

#define modemAssocPin_DEF   -1   //B031r1: not supported
//#define modemRssi??_DEF   -1   //B031r1:RssiSt ?
/**/
  #define LOGGERID_DEF_STR "msLog01"
  #define NEW_LOGGERID_MAX_SIZE 40
  //#define NEW_LOGGERID_MAX_SIZE  3 ///Test
  #define configIniID_DEF_STR "ms_cfg.ini"  
  #define CONFIG_TIME_ZONE_DEF -8

// ** How frequently (in minutes) to log data **
//For two Loggers - define loggingMultiplier_MAX_CDEF
// The loggingInterval_CDEF_MIN becomes sampling rate 
//#define loggingMultiplier_MAX_CDEF 48
// How frequently (in minutes) to sample data 
#ifdef loggingMultiplier_MAX_CDEF
//logging to SD/Publishing happens as loggingMultiplier_MAX_CDEF*loggingInterval_CDEF_MIN
#define  loggingInterval_CDEF_MIN 5
#else
//logging to SD/Publishing and sampling are same
#define  loggingInterval_CDEF_MIN 15
#endif //loggingMultiplier_MAX_CDEF

// Maximum logging setting allowed
#define  loggingInterval_MAX_CDEF_MIN 6*60

//The following are used as a routing for per board extensions
//From an Arduino Virtual Pin Numbering .. First Digital Extension then Analog Extension 
#define ARD_DIGITAL_EXTENSION_PINS 16
#define ARD_ANALOLG_EXTENSION_PINS 8
#define ARD_ANLAOG_MULTIPLEX_PIN PIN_A0
//#define ARD_COMMON_PIN  PIN_A0
#define PIN_EXT_ANALOG(extPin) (thisVariantNumPins+ARD_DIGITAL_EXTENSION_PINS+extPin)

// BG031 extensions AEM = ADC_EXT_MUX pin on HC4051
#define B031_AEM_EXTENSIONS 1
#ifdef B031_AEM_EXTENSIONS
#define B031_AEM_EXT0_PIN     0
#define B031_AEM_EXT1_PIN     1
#define B031_AEM_EXT2_PIN     2
#define B031_AEM_EXT3_PIN     3
#define B031_AEM_VBATT_PIN    4  // 1M/200k 
#define B031_AEM_TEMP_AIR_PIN 5
#define B031_AEM_V3V6_PIN     6  // 1M/200k 
#define B031_AEM_X7_PIN       7
#endif //B031_AEM_EXTENSIONS

//This is the SAMD51J PIN

  //#define SENSOR_CONFIG_GENERAL 1
 
  //Defaults for data.envirodiy.org
  #define registrationToken_UUID "registrationToken_UUID"
  #define samplingFeature_UUID   "samplingFeature_UUID"

  //#define Decagon_CTD_UUID 1 - place maker not include yet

 // #define Insitu_TrollSdi12_UUID 1
    //Not tested B031, needs to activate SDI-12 Pwr buffer
  #ifdef Insitu_TrollSdi12_UUID
    //SDI-12 Port - Setup Address 1, Depth/Height as Param 2 (3 total 0-2)
    #define ITROLL_DEPTH_UUID "KellerNanolevel_Height_UUID"
    #define ITROLL_TEMP_UUID  "KellerNanolevel_Temp_UUID"
    //#define ITROLL_PRESSURE_UUID  "ITROLL_PRESSURE_UUID"  
    #define CONFIG_SENSOR_SDI12_PHY 1
  #endif // Insitu_Troll_UUID

  //#define KellerNanolevel_ACT 1
  #ifdef KellerNanolevel_ACT 
    #define KellerNanolevelModbusAddress_DEF 0x01
    #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
    #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
    #define CONFIG_SENSOR_RS485_PHY 1
  #endif //KellerNanolevel_ACT

  //#define KellerAcculevel_ACT 1
  #ifdef KellerAcculevel_ACT 
    #define KellerAcculevelModbusAddress_DEF 0x01
    #define KellerAcculevel_Height_UUID "KellerNanolevel_Height_UUID"
    #define KellerAcculevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
    #define CONFIG_SENSOR_RS485_PHY 1
  #endif//KellerAcculevel_ACT 

  //#define InsituLTrs485_ACT 1 -not working, needs CRC flipped
  //  #define MODBUS_BAUD_RATE 19200

  #if defined CONFIG_SENSOR_RS485_PHY 
  //  SwVbat=1 for Vbst@12V and opt Sw12V
  //  SwVrs485 for Vbat-->Vrs to IC 
  //  Serial2 for Tx/A4/secom0.0 & Rx/A1/secom0.1
  //
    //??#define PWR_COMMS_PHY SwRs485
    #define modbusSensorPower_DEF eMcpA_SwVrs485Out_pinnum //Secondary RS485 Transciever
  #elif defined CONFIG_SENSOR_SDI12_PHY
    //#define PWR_COMMS_PHY SwVsdi
    #define sdi12SensorPower_DEF eMcpA_SwVsdiOut_pinnum //Secondary SDI12 Transciever
  #elif defined CONFIG_SENRSOR_WIRE1_PHY
    //#define PWR_COMMS_PHY SwV1w
    #define w1SensorPower_DEF eMcpA_swV1wOut_pinnum
  #else
    #define PWR_COMMS_PHY -1
  #endif //  #ifdef KellerNanolevel_ACT 

  #if (defined CONFIG_SENSOR_RS485_PHY) || (defined CONFIG_SENSOR_SDI12_PHY) || (defined CONFIG_SENRSOR_WIRE1_PHY)
    //Full Duplex
    #define CONFIG_HW_RS485PHY_TX_PIN PIN_A1  //Feather_M4 Serial2 Tx pin 
    #define CONFIG_HW_RS485PHY_RX_PIN PIN_A4  //Feather_M4 Serial2 Rx pin
    #define CONFIG_HW_RS485PHY_DIR_PIN PIN_A5 //Also needs -DSERIAL2_TE_CNTL or -DSERIAL2_TE_HALF_DUPLEX for for variant.cpp 
    #define max485EnablePin_DEF -1 //Hw Opt PIN_A5
    #define rs485AdapterPower_DEF eMcpA_SwVbatOut_pinnum //Boost Sensor Power
  #endif //CONFIG_SENSOR_RS485_PHY

  //#define AnalogProcEC_ACT 1
  //AnalogProcEC_ACT B031 not UT 
  #ifdef AnalogProcEC_ACT
    #define EC1_UUID      "EC1_UUID"
    //Simplest is ARED=VCC Power D4 - and then ADC_X2 or ADC_X3 - Wiring on J1
    #define PIN_D4_SQWAVE 4
    #define ECpwrPin_DEF PIN_D4_SQWAVE 
    #define ECdataPin1_DEF B031_AEM_EXT3_PIN
  #endif //AnalogProcEC_ACT

  //Needs enabling to put in to powerdown reduces sleep by 1mA
  //#define INA219M_PHY_ACT 
  #ifdef INA219M_PHY_ACT
  #if 0
    #define INA219M_MA_UUID       "INA219_MA_UUID"
    #define INA219M_A_MAX_UUID    "INA219_A_MAX_UUID"
    #define INA219M_A_MIN_UUID    "INA219_A_MIN_UUID"
    #define INA219M_VOLT_UUID     "INA219_VOLT_UUID"
    #else
      #define INA219M_MA_UUID       "OFF"
      #define INA219M_VOLT_UUID     "OFF"
    #endif 
  #endif //INA219_PHY_ACT

  //#define Modem_RSSI_UUID ""
  // Seems to cause XBEE WiFi S6 to crash 
  //#define Modem_SignalPercent_UUID    "SignalPercent_UUID"
  #define ProcessorStats_ACT 1
  #ifdef ProcessorStats_ACT
    #define ProcessorStats_SampleNumber_UUID  "SampleNumber_UUID"
    #define ProcessorStats_Batt_UUID          "Batt_UUID"
  #endif //ProcessorStats_ACT

  //To Use internal ADC (Needs  SAMD51 testing)
  // Needs testing for B031r1 range.
  //#define ProcVolt_ACT 1
  #if defined ProcVolt_ACT
    #define ProcVolt_Volt0_UUID "ProcVolt_Volt0_UUID"
    #define ProcVolt_batt_UUID  "ProcVolt_batt_UUID"
    #define ProcVolt_V3v6_UUID  "ProcVolt_V3v6_UUID"
  #endif //ProcVolt_ACT

  //Use sensor eg Adafruit_AM2314 or AM2320
  //#define ASONG_AM23XX_UUID 1
  #if defined ASONG_AM23XX_UUID
    #define ASONG_AM23_Air_Temperature_UUID "Air_Temperature_UUID" 
    #define ASONG_AM23_Air_Humidity_UUID    "Air_Humidity_UUID"
  #endif // ASONG_AM23XX_UUID
  
  //#define ExternalVoltage_ACT 1
  // As of 2020Jan03: These are not uniquie on data.enviroDIY.org
  #ifdef ExternalVoltage_ACT
  #define ExternalVoltage_Volt0_UUID "Volt0_UUID"
  #define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
  #endif //ExternalVoltage_ACT

  /* variant.cpp std defines  Serial1 
  variant.cpp enhanced for  Serial2 3 4
variant.h: has pin definitions
#define PIN_SERIALx_RX       (0ul)  FTDI Pin4
#define PIN_SERIALx_TX       (1ul)  FTDI Pin5
*/
#define SerialModem Serial1

#if defined(SERIAL2_EN) || defined(SERIAL2_TE_CNTL) || defined(SERIAL2_TE_HALF_DUPLEX)
//to be used by modbusSerial 
#define SerialModbus Serial2
#endif // SERIAL2_EN
#if defined SERIAL3_EN
//For M4express the RedLed is also on Serial3RX
//#define SerialExt3 Serial3
#endif //SERIAL3_EN
#if defined SERIAL4_EN
#define SerialTty Serial4
//needs to be or all outputs so put on platformio.ini build_flags = 
//#define STANDARD_SERIAL_OUTPUT Serial4
//    -DSTANDARD_SERIAL_OUTPUT=Serial4
//    -DDEBUGGING_SERIAL_OUTPUT=Serial4
//    -DDEEP_DEBUGGING_SERIAL_OUTPUT=Serial4
#endif // SERIAL4_EN

#elif PROFILE_NAME == PROFILE03_SODAQ_AUTONOMO_M0
//**************************************************************************
//This configuration expects a standard AUTONOMO with a Adafruit-3650 INA219(FeatherWing) connected on I2C 
// .platformio\packages\framework-arduinosam\variants\sodaq_autonomo
//This is hardcode to mean things in ProcessorStats !!!!
//For Autonomo
#define AutonomoRev_DEF "r5"
#define AutonomoName_DEF "Autonomo"
#define HwVersion_DEF AutonomoRev_DEF
#define HwName_DEF AutonomoName_DEF

#define USE_MS_SD_INI 1
/*nh debug cable to Autonomo Serial
  FTDI/Yellow ProcTx/JP2-3/D0/PA9/SCOM2PAD1 & 
  FTDI/Orange ProcRx/JP2-4/D1/PA10/SCOM2PAD2 

Autonom has built in BEE, on sleep1 with 5 control pins
BEE_VCC PowerEn=H -seperate regulator 
input DTR shared ArduinoA8
inputXbee CTS
Out  Xbee autonomoModemAssocPin
inXbee RTS
BEE TX Serial PB30/SCOM5PAD0 To Bee from Proc
BEE RX Serial PB31/SCOM5PAD1 From Bee to Proc
*/
#define modemVccPin_DEF     BEE_VCC //PA28 to XC6220 
#define autonomoModemRtsPin 38u//BEERTS  //PB22=RTS same as MCU_CTS output XbeePin16_RTS
#define modemStatusPin_DEF  39u//BEECTS  //PB23=CTSoutput XbeePin12_CTS
#define modemSleepRqPin_DEF PIN_A13 //PB1=A13. Xbee Pin 9 DTR (DTS Shared with JP1-A13)
#define modemAssocPin_DEF   RI_AS   //PB17=ASSOC output XbeePin15_Assoc

// Serial3 shares with LED  -DENABLE_SERIAL3
//#define STANDARD_SERIAL_OUTPUT Serial
//#define DEBUGGING_SERIAL_OUTPUT Serial
//#define DEEP_DEBUGGING_SERIAL_OUTPUT Serial

#define LOGGERID_DEF_STR "msLog01"
#define NEW_LOGGERID_MAX_SIZE 40
//#define NEW_LOGGERID_MAX_SIZE  3 ///Test
#define configIniID_DEF_STR "ms_cfg.ini"  
#define CONFIG_TIME_ZONE_DEF -8

// ** How frequently (in minutes) to log data **
//For two Loggers - define loggingMultiplier_MAX_CDEF
// The loggingInterval_CDEF_MIN becomes sampling rate 
//#define loggingMultiplier_MAX_CDEF 5
// How frequently (in minutes) to sample data 
#ifdef loggingMultiplier_MAX_CDEF
//logging to SD/Publishing happens as loggingMultiplier_MAX_CDEF*loggingInterval_CDEF_MIN
#define  loggingInterval_CDEF_MIN 2
#else
//logging to SD/Publishing and sampling are same
#define  loggingInterval_CDEF_MIN 15
#endif //loggingMultiplier_MAX_CDEF

// Maximum logging setting allowed
#define  loggingInterval_MAX_CDEF_MIN 6*60

//define one Radio  _Module
//#define DigiXBeeWifi_Module 1
//#warning infoAutonomoWithDigiXBeeWiFi
#define DigiXBeeCellularTransparent_Module 1
#warning infoAutonomoWithDigiXBeeCellTransparent
// #define DigiXBeeLTE_Module 1 - unstable

//end of _Module


#define APN_CDEF  "hologram" // The APN for the gprs connection, unnecessary for WiFi
#define WIFIID_CDEF  "xxxx"  // The WiFi access point, unnecessary for gprs
#define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define KellerAcculevel_ACT 1
//Defaults for data.envirodiy.org
#define registrationToken_UUID "registrationToken_UUID"
#define samplingFeature_UUID   "samplingFeature_UUID"
//#define Smtp2goJsonAppKey "Smtp2goJson_AppKey"
//#define Smtp2goJsonAppKey "api-76228BACDD7511E99F26F23C91C88F4E"
//#define KellerNanolevel_ACT 1
#ifdef KellerNanolevel_ACT
  #define CONFIG_SENSOR_RS485_PHY 1
  #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
  #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
#endif //KellerNanolevel_ACT
//#define INA219M_PHY_ACT 
#ifdef INA219M_PHY_ACT
  #define INA219M_MA_UUID       "INA219_MA_UUID"
  #define INA219M_A_MAX_UUID    "INA219_A_MAX_UUID"
  #define INA219M_A_MIN_UUID    "INA219_A_MIN_UUID"
  #define INA219M_VOLT_UUID     "INA219_VOLT_UUID"
#endif //INA219_PHY_ACT

//#define Modem_RSSI_UUID ""
// Seems to cause XBEE WiFi S6 to crash
//#define Modem_SignalPercent_UUID    "SignalPercent_UUID"
#define ProcessorStats_ACT 1
#define ProcessorStats_SampleNumber_UUID  "SampleNumber_UUID"
#define ProcessorStats_Batt_UUID          "Batt_UUID"

//To Use internal ADC (Only SAMD21 tested)
#define ProcVolt_ACT 1
#define ProcVolt_Volt0_UUID "ProcVolt_Volt0_UUID"

//#define ExternalVoltage_ACT 1
#ifdef ExternalVoltage_ACT
#define ExternalVoltage_Volt0_UUID "Volt0_UUID"
#define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
#endif //ExternalVoltage_ACT

#elif PROFILE_NAME == PROFILE02_ADAFRUIT_FEATHER_M0
//**************************************************************************
//This configuration expects a standard FEATHER_M0 Adalogger
// connected to a B031r1 with RTC PCF2127, 
// MCP23017 port expander 16bits
// Requires updated C:\Users\neilh77\.platformio\packages\framework-arduinosam\variants\feather_m0
//#define CONFIG_SENSOR_RS485_PHY 1
//Standard 
//This is hardcode to mean things in ProcessorStats !!!!
//For Adafruit Feather M0 (not Feather M0 Express?)
#define HwVersion_DEF "r1"
#define HwName_DEF "FeatherM0"

#define USE_MS_SD_INI 1
/*nh debug cable to B031r1 Serial?
  FTDI/Yellow ???tbdProcTx/JP2-3/D0/PA9/SCOM2PAD1 & 
  FTDI/Orange ???tbdProcRx/JP2-4/D1/PA10/SCOM2PAD2 
Feather M0 has built in LED and microSD
B0311r1 has XBEE
  */

/* Needs specifying for Feather M0
#define modemVccPin_DEF     BEE_VCC // 
#define autonomoModemRtsPin 38u//PB22=RTS same as MCU_CTS output XbeePin16_RTS
#define modemStatusPin_DEF  39u//BEECTS  //PB23=CTSoutput XbeePin12_CTS
#define modemSleepRqPin_DEF PIN_A13 //PB1=A13. Xbee Pin 9 DTR (DTS Shared with JP1-A13)
#define modemAssocPin_DEF   RI_AS   //PB17=ASSOC output XbeePin15_Assoc
*/
#define LOGGERID_DEF_STR "msLog01"
#define NEW_LOGGERID_MAX_SIZE 40
//#define NEW_LOGGERID_MAX_SIZE  3 ///Test
#define configIniID_DEF_STR "ms_cfg.ini"  
#define CONFIG_TIME_ZONE_DEF -8

// How frequently (in minutes) to log data
//#define  loggingInterval_CDEF_MIN 5
#define  loggingInterval_MAX_CDEF_MIN 120

//define one Radio  _Module
#define DigiXBeeWifi_Module 1
#warning infoAutonomoWithDigiXBeeWiFi
//#define DigiXBeeCellularTransparent_Module 1
//#warning infoAutonomoWithDigiXBeeCellTransparent
// #define DigiXBeeLTE_Module 1 - unstable

//end of _Module
#define APN_CDEF  "hologram" // The APN for the gprs connection, unnecessary for WiFi
#define WIFIID_CDEF  "xxxx"  // The WiFi access point, unnecessary for gprs
#define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define KellerAcculevel_ACT 1
//Defaults for data.envirodiy.org
#define registrationToken_UUID "registrationToken_UUID"
#define samplingFeature_UUID   "samplingFeature_UUID"
//#define KellerNanolevel_ACT 1
#ifdef KellerNanolevel_ACT
  #define CONFIG_SENSOR_RS485_PHY 1
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

//To Use internal ADC (Only SAMD21 tested)
#define ProcVolt_ACT 1
#define ProcVolt_Volt0_UUID "ProcVolt_Volt0_UUID"

//#define ExternalVoltage_ACT 1
#ifdef ExternalVoltage_ACT
#define ExternalVoltage_Volt0_UUID "Volt0_UUID"
#define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
#endif //ExternalVoltage_ACT

#elif 0 //PROFILE_NAME == PROFILE03
//**************************************************************************
//Keller Nanolevel with XBP-u.fl 
#define CONFIG_SENSOR_RS485_PHY 1
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
#define      loggingInterval_CDEF_MIN (15)   
#endif
#if !defined loggingMultiplier_MAX_CDEF
//Maximium Logging Multiplier  
//This is default for testing - platforms should set own MAX
// don't define #define  loggingMultiplier_MAX_CDEF 0
#endif
#if !defined loggingInterval_MAX_CDEF_MIN
//Maximium Logging Interfaval Default - 
// issue for ARCH_SAMD may be impacted by WatchDog? 
//This is default for testing - platforms should set own MAX
#define  loggingInterval_MAX_CDEF_MIN 24+60
#endif
#if !defined loggingInterval_Fast_MIN  
#define  loggingInterval_Fast_MIN  1
#endif //loggingInterval_Fast_MIN  
#if !defined NEW_LOGGERID_MAX_SIZE
#define NEW_LOGGERID_MAX_SIZE 40
#endif 

#if !defined MODBUS_BAUD_RATE
  #define MODBUS_BAUD_RATE 9600
#endif //MODBUS_BAUD_RATE

#endif //ms_cfg_h
