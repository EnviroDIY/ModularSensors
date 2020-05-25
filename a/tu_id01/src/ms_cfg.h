/*****************************************************************************
ms_cfg.h  - ModularSensors Configuration - a wip - WorkInProgress
Written By:  Neil Hancock www.envirodiy.org/members/neilh20/
Development Environment: PlatformIO
Hardware Platform(s): EnviroDIY Mayfly Arduino Datalogger+RS485 Wingboard

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
#define USE_PS_EEPROM  1 //Alterantive USE_PS_2ND_SD
#define USE_PS_HW_BOOT 1

//#define USE_PS_modularSensorsCommon 1

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

// Maximum logging setting allowed
#define  loggingInterval_MAX_CDEF_MIN 6*60


// FUT: Some board level would be in a per board level persistent storage - eg FLASH
// These are either pre .ini read or per board defintions
#define MFsn_def  F("unknown")

//define one  _Module
//#define DigiXBeeWifi_Module 1
//#warning infoMayflyWithDigiXBeeWiFi
//#define DigiXBeeCellularTransparent_Module 1
//#warning infoMayflyWithDigiXBeeCellTransparent
// #define DigiXBeeLTE_Module 1 - unstable
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
// End TinyGsmClient.h options
#if defined(DigiXBeeWifi_Module) || defined(DigiXBeeCellularTransparent_Module) 
#define UseModem_Module 1
//Required for TinyGsmClient.h 
#define TINY_GSM_MODEM_XBEE
#endif // Modules
//end of _Module

  #define APN_CDEF  "hologram" // The APN for the gprs connection, unnecessary for WiFi
  //#define WIFIID_CDEF  "NameSsid"  // The WiFi access point, unnecessary for gprs
  //#define WIFIPWD_CDEF  NULL  // NULL for none, or  password for connecting to WiFi, unnecessary for gprs
  #define WIFIID_CDEF  "xxx"  // The WiFi access point, unnecessary for gprs
  #define WIFIPWD_CDEF  "yyy" // NULL for none, or  password for connecting to WiFi, unnecessary for gprs

//This might need revisiting
#define ARD_ANLAOG_MULTIPLEX_PIN A6

//#define SENSOR_CONFIG_GENERAL 1
//#define KellerAcculevel_ACT 1
//Defaults for data.envirodiy.org
#define registrationToken_UUID "registrationToken_UUID"
#define samplingFeature_UUID   "samplingFeature_UUID"

//#define Decagon_CTD_UUID 1
#ifdef Decagon_CTD_UUID
  //Mayfly definitions
  #define CTD10_DEPTH_UUID "CTD10_DEPTH_UUID"
  #define CTD10_TEMP_UUID  "CTD10_TEMP_UUID"
  #define CTD10_COND_UUID  "CTD10_COND_UUID"  
#endif // Decagon_CTD_UUID

//#define Insitu_TrollSdi12_UUID 1
#ifdef Insitu_TrollSdi12_UUID
  //Mayfly definitions
  #define ITROLL_DEPTH_UUID "KellerNanolevel_Height_UUID"
  #define ITROLL_TEMP_UUID  "KellerNanolevel_Temp_UUID"
  //#define ITROLL_PRESSURE_UUID  "ITROLL_PRESSURE_UUID"  
#endif // Insitu_Troll_UUID

//#define KellerNanolevel_ACT 1
#ifdef KellerNanolevel_ACT
  #define CONFIG_SENSOR_RS485_PHY 1
  #define KellerNanolevel_Height_UUID "KellerNanolevel_Height_UUID"
  #define KellerNanolevel_Temp_UUID   "KellerNanolevel_Temp_UUID"
  #define KellerNanolevelModbusAddress_DEF 0x01
#endif // KellerNanolevel_ACT

//#define InsituLTrs485_ACT 1 -not working
#ifdef InsituLTrs485_ACT
  #define CONFIG_SENSOR_RS485_PHY 1
  #define InsituLTrs485_Height_UUID "KellerNanolevel_Height_UUID"
  #define InsituLTrs485_Temp_UUID   "KellerNanolevel_Temp_UUID"
  #define InsituLTrs485ModbusAddress_DEF 0x01
  //Default is 19200 lets hope serial works with it.
  #define MODBUS_BAUD_RATE 19200
#endif // InsituLTrs485_ACT

#ifdef CONFIG_SENSOR_RS485_PHY
  //Mayfly definitions
  #define CONFIG_HW_RS485PHY_TX_PIN 5  //Mayfly OCRA1 map AltSoftSerial Tx pin 
  #define CONFIG_HW_RS485PHY_RX_PIN 6  //Mayfly ICES1 map AltSoftSerial Rx pin
  #define CONFIG_HW_RS485PHY_DIR_PIN -1 
  #define max485EnablePin_DEF   -1  
  #define rs485AdapterPower_DEF 22 // Pin to switch RS485 adapter power on and off (-1 if unconnected)
  #define modbusSensorPower_DEF 22;  // Pin to switch power on and off (-1 if unconnected) 
  #ifndef MODBUS_BAUD_RATE
  #define MODBUS_BAUD_RATE 9600
  #endif //MODBUS_BAUD_RATE
#endif //CONFIG_SENSOR_RS485_PHY

#define AnalogProcEC_ACT
#ifdef AnalogProcEC_ACT
  #define EC1_UUID      "EC1_UUID"
  #define ECpwrPin_DEF A4
  #define ECdataPin1_DEF A0
#endif //AnalogProcEC_ACT

//#define INA219M_PHY_ACT 1
#ifdef INA219M_PHY_ACT
  #define INA219M_MA_UUID      "INA219_MA_UUID"
  #define INA219M_VOLT_UUID    "INA219_VOLT_UUID"
#endif //INA219_PHY_ACT


//#define ASONG_AM23XX_UUID 1
#if defined ASONG_AM23XX_UUID
  #define ASONG_AM23_Air_Temperature_UUID  "Air_Temperature_UUID" 
  #define ASONG_AM23_Air_TemperatureF_UUID "Air_TemperatureF_UUID"   
  #define ASONG_AM23_Air_Humidity_UUID    "Air_Humidity_UUID"
#endif // ASONG_AM23XX_UUID

#ifdef ARDUINO_AVR_ENVIRODIY_MAYFLY
//#define MaximDS3231_TEMP_UUID  "MaximDS3231_TEMP_UUID"
#define MaximDS3231_TEMPF_UUID "MaximDS3231_TEMPF_UUID"
#endif //ARDUINO_AVR_ENVIRODIY_MAYFLY
//#define DIGI_RSSI_UUID "DIGI_RSSI_UUID"
// Seems to cause XBEE WiFi S6 to crash
//#define Modem_SignalPercent_UUID    "SignalPercent_UUID"
#define ProcessorStats_ACT 1
#define ProcessorStats_SampleNumber_UUID  "SampleNumber_UUID"
#define ProcessorStats_Batt_UUID          "Batt_UUID"

//#define ExternalVoltage_ACT 1
#ifdef ExternalVoltage_ACT
  #define ExternalVoltage_Volt0_UUID "Volt0_UUID"
  //#define ExternalVoltage_Volt1_UUID "VOLT1_UUID"
#endif //ExternalVoltage_ACT

#endif //ms_cfg_h
