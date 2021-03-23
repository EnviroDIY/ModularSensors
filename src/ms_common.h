#ifndef ms_common_h
#define ms_common_h
/*****************************************************************************
ms_common.h
Written By:  Neil Hancock www.envirodiy.org/members/neilh20/
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2018, Neil Hancock - all rights assigned to Stroud Water
Research Center (SWRC) and they may change this title to Stroud Water Research
Center as required and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.xx.0

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/
/*****************************************************************************
 * Support   ARDUINO_ARCH_AVR ARDUINO_ARCH_SAM ARDUINO_ARCH_SAMD
 * Implementation: Due to ram limitations and of course Atmel Harvard (von
 Neuman) Architecture and Princeton (eg ARM)
 * some special macros a required.
 * Atmel Harvard has seperate space for constants/program, where as Princeton is
 all the same memory space
 * C/Cpp only uses one space for all data and program
 * For Mayfly Atmel  ARDUINO_ARCH_AVR  assume Harvard Architecture and copy of
 CONSTs from PROGMEM
 * For not defined PROGMEM assume Princeton Architecture
 * examples
 * \TinyGSM\src\TinyGsmCommon.h and subsequent useage eg TinyGsmClientXBee.h
 .platformio\platforms\atmelsam\boards adafruit feather_m0 feather_m4
 ARDUINO_ARCH_SAMD .platformio\platforms\teensy\boards  teensylc -DTEENSYLC
 teensy36 -DTEENSY36


 ??.platformio\packages\framework-arduinoavr\libraries\WiFi\extras\wifiHD\src\SOFTWARE_FRAMEWORK\SERVICES\LWIP\lwip-1.3.2\src\core\pbuf.c
   SYS_ARCH_;
 *
*/
#include <Arduino.h>  // The base Arduino library
// Local routing defitions here
#if defined(__AVR__)
#define EDIY_PROGMEM PROGMEM
typedef const __FlashStringHelper* EdiyConstStr;
#define EFP(x) (reinterpret_cast<EdiyConstStr>(x))
#define EF(x) F(x)
#else
#define EDIY_PROGMEM
typedef const char* EdiyConstStr;
#define EFP(x) x
#define EF(x) x
#endif
#include "ms_cfg.h"

#if defined USE_PS_EEPROM
#include <util/crc16.h>
#endif  // USE_PS_EEPROM

/*****************************************************************************
 * Persistent structures.
 * Defines data structures for per software build & geographical location
 customizations
 * Needs to be extensible. If a change is made, ensure size increments for
 strucuture.
 * This is initially implemented as reading form .ini file on nicroSD.
 * The implementation could change in the future for parts of initial
 * Board level (including internal) Persistent Storage -
 *    eg Internal EEPROM, though any board level data EEPROM or data flash is
 the same.
 * This may be very useful for hw tracking (eg sn/rev)  and essential boot
 elements
 *   - Internal persistent storage presents special problems with upgrades
 *

 *
 */
//#define USE_PLAN_FOR_UPGRADE 1
#if defined(USE_PLAN_FOR_UPGRADE)
#define STRCT_SZ(parm1) uint16_t parm1;
#else
#define STRCT_SZ(parm1) sizof(parm1)
#endif  // USE_PLAN_FOR_UPGRADE
//#define USE_PS_HW_BOOT 1
//******
#if defined(USE_PS_HW_BOOT)
// Hardware Boot Structure - rarely expected to change
#define HW_BOOT_BOARD_NAME_SZ 21
#define HW_BOOT_SERIAL_NUM_SZ 21
#define HW_BOOT_REV_SZ 11
#define HW_BOOT_EXP 17
typedef struct {
    uint16_t crc16;       // Across just the hw_boot_t except crc16
    uint8_t  struct_ver;  // 1-255 - increment for any changes in this structure
    uint8_t  board_name[HW_BOOT_BOARD_NAME_SZ];  // eg Mayfly
    uint8_t  serial_num[HW_BOOT_SERIAL_NUM_SZ];  // eg 1234
    uint8_t  rev[HW_BOOT_REV_SZ];                // eg 0.5b
    uint8_t  exp[HW_BOOT_EXP];
} hw_boot001_t;
#define HW_BOOT_STRUCT_VER_001 001
#define HW_BOOT_STRUCT_VER HW_BOOT_STRUCT_VER_001
#define mHw_boot_t(p1) hw_boot001_t p1
#define sizeof_hw_boot sizeof(hw_boot001_t)
#else
#define mHw_boot_t(p1)
#endif  // USE_PS_HW_BOOT
//******
// For extensibility - each structures data size (not including sz) can be
// defined at the beginning of the structure this allows any parsing algorithims
// to step through the structures easly and also to invoke the right parm1:
// 1-65535 - struct size increase for any changes in this structure

//****** [COMMON]
#define USE_PS_modularSensorsCommon 1
//
#if defined(USE_PS_modularSensorsCommon)
#define MSC_LOGGER_ID_SZ 21
#define MSC_GEOLOCATION_ID_SZ 61
typedef struct {
    // v01 Initial structure
    uint16_t logging_interval_min;
    int8_t   time_zone;  //-12,0 to +11?
    uint8_t  battery_type;
    uint16_t battery_mAhr; //0-65,536
    // uint8_t  colllectReadings;
    // uint8_t  sendOffset_min;
    uint8_t logger_id[MSC_LOGGER_ID_SZ];
    uint8_t geolocation_id[MSC_GEOLOCATION_ID_SZ];
} msc01_t;
#define MSC_ACTIVE msc01_t

#define epc_logging_interval_min epc.app.msc.s.logging_interval_min 
#define epc_battery_type  epc.app.msc.s.battery_type 
#define epc_battery_mAhr  epc.app.msc.s.battery_mAhr
#define BATTERY_mAhr_DEF  4400
#define BATTERY_mAhr_MAX  65501

#define epc_logger_id  (char*)epc.app.msc.s.logger_id
#define epc_logger_id1st      epc.app.msc.s.logger_id[0]

typedef struct {
    uint8_t    sz;
    MSC_ACTIVE s;
} modularSensorsCommon_t;
#define mModularSensorsCommon_t(p1) modularSensorsCommon_t p1
#else
#define mModularSensorsCommon_t(p1)
#endif  // USE_PS_modularSensorsCommon)

#if defined UseModem_Module
#define USE_PS_modularSensorsNetwork 1
#endif  // UseModem_Module
#if defined(USE_PS_modularSensorsNetwork)
//MSCN_TYPE_XXX is the Network Type modem 
#define MSCN_TYPE_NONE 0
#define MSCN_TYPE_CELL 1
#define MSCN_TYPE_WIFI 2
#define MSCN_TYPE_LORA 3
#define MSCN_APN_SZ 32
#define MSCN_APN_DEF_STR "APN_NONE"
#define MSCN_WIFI_ID_SZ 32
#define MSCN_WIFIID_DEF_STR "WIFIID_NONE"
#define MSCN_WIFI_PWD_SZ 32
#define MSCN_WIFIPWD_DEF_STR "WIFIPWD_NONE"
#ifndef MNGI_COLLECT_READINGS_DEF
#define MNGI_COLLECT_READINGS_DEF 1
#endif //MNGI_COLLECT_READINGS_DEF
#ifndef MNGI_SEND_OFFSET_MIN_DEF 
#define MNGI_SEND_OFFSET_MIN_DEF 100
#endif //MNGI_SEND_OFFSET_MIN_DEF 
typedef struct {
    uint8_t network_type; // 0=apn ,1=wifi network 
    uint8_t apn[MSCN_APN_SZ];           // 32
    uint8_t WiFiId[MSCN_WIFI_ID_SZ];    // 32?
    uint8_t WiFiPwd[MSCN_WIFI_PWD_SZ];  // 32??
    uint8_t collectReadings_num; // 1-30
    uint8_t sendOffset_min;      //0-30
} msn01_t;
#define MSN_ACTIVE msn01_t

#define epc_apn      (char*)epc.app.msn.s.apn
#define epc_apn1st          epc.app.msn.s.apn[0]
#define epc_WiFiId   (char*)epc.app.msn.s.WiFiId
#define epc_WiFiId1st       epc.app.msn.s.WiFiId[0]
#define epc_WiFiPwd  (char*)epc.app.msn.s.WiFiPwd
#define epc_WiFiPwd1st      epc.app.msn.s.WiFiPwd[0]

typedef struct {
    uint8_t    sz;
    MSN_ACTIVE s;
} modularSensorsNetwork_t;


#define mModularSensorsNetwork_t(p1) modularSensorsNetwork_t p1
#else
#define mModularSensorsNetwork_t(p1)
#endif  // USE_PS_modularSensorsNetwork

//******
#if defined UseModem_Module
#define USE_PS_Provider 1
#endif  // UseModem_Module
//******
//Provider types supported
#define PROVID_TYPE_NONE    0x00
#define PROVID_TYPE_MMW     0x01
#define PROVID_TYPE_TS      0x02
#define PROVID_TYPE_UBIDOTS 0x04

#define PROVID_CLOUD_ID_SZ 38
#define PROVID_DEF_STR "NONE"
#define PROVID_NULL_TERMINATOR 0

#define PROVID_MW_REGISTRATION_TOKEN_SZ 38
#define PROVID_MW_SAMPLING_FEAUTRE_SZ 38

#define PROVID_TSMQTTKEY_SZ    17  
#define PROVID_TSCHANNELID_SZ   7
#define PROVID_TSCHANNELKEY_SZ 17

#define PROVID_UB_AUTH_TOKEN_SZ 38
#define PROVID_UB_DEVICEID_SZ 38

// If provider requires mapping, then use common mapping to whatever type
#define PROVID_UUID_SENSOR_NAME_SZ 40
#define PROVID_UUID_SENSOR_VALUE_SZ 38
#define PROVID_UUID_SENSOR_CNTMAX_SZ 10

#if defined(USE_PS_Provider)
typedef struct {
    char name[PROVID_UUID_SENSOR_NAME_SZ];
    char value[PROVID_UUID_SENSOR_VALUE_SZ];
} ini_name_value_t;

typedef struct {
    // v01 initial structure
    // All are in ascii strings, with the first unused octet \0
    char    cloudId[PROVID_CLOUD_ID_SZ];  // ASCII url
    char    registration_token[PROVID_MW_REGISTRATION_TOKEN_SZ];
    char    sampling_feature[PROVID_MW_SAMPLING_FEAUTRE_SZ];
    uint16_t timerPostTout_ms; // Gateway Timeout (ms)
    uint16_t timerPostPace_ms; // Gateway Pacing (ms)
    uint16_t postMax_num; //0 no limit, else max num POSTs one session
    ini_name_value_t uuid[PROVID_UUID_SENSOR_CNTMAX_SZ];
} provid_envirodiy01_t;
typedef struct {
    // v01 initial structure
    // All are in ascii strings, with the first unused octet \0
    /*
const char* thingSpeakMQTTKey =     17x "XXXXXXXXXXXXXXXX";  // Your MQTT API Key from Account > MyProfile.
const char* thingSpeakChannelID =    7x "######";  // The numeric channel id for your channel
const char* thingSpeakChannelKey =   17x "XXXXXXXXXXXXXXXX";  // The Write API Key for your channel 
    */
    char    cloudId[PROVID_CLOUD_ID_SZ];  // ASCII url
    char    thingSpeakMQTTKey[PROVID_TSMQTTKEY_SZ];
    char    thingSpeakChannelID[PROVID_TSCHANNELID_SZ];
    char    thingSpeakChannelKey [PROVID_TSCHANNELKEY_SZ];
    uint16_t timerPostTout_ms; // Gateway Timeout (ms)
    uint16_t timerPostPace_ms; // Gateway Pacing (ms)
    uint16_t postMax_num; //0 no limit, else max num POSTs one session
    //uuid[] not used sequential 
} provid_thingspeak01_t;

typedef struct {
    // v01 initial structure
    // All are in ascii strings, with the first unused octet \0
    char    cloudId[PROVID_CLOUD_ID_SZ];  // ASCII url
    char    authentificationToken[PROVID_UB_AUTH_TOKEN_SZ];
    char    deviceID[PROVID_UB_DEVICEID_SZ];
    uint16_t timerPostTout_ms; // Gateway Timeout (ms)
    uint16_t timerPostPace_ms; // Gateway Pacing (ms)
    uint16_t postMax_num; //0 no limit, else max num POSTs one session
    ini_name_value_t uuid[PROVID_UUID_SENSOR_CNTMAX_SZ];
} provid_ubidots01_t;
typedef struct {
    //Providers meta data stored here. 
    // Only one provider using variables/uuid supported. 
    /// Fut : union or simulataneous?
    provid_envirodiy01_t ed;
    provid_thingspeak01_t ts;
    provid_ubidots01_t ub;
} msp01_t;
#define MSP_ACTIVE msp01_t

typedef struct {
    uint8_t     sz;
    uint8_t provider_type; // Bit pos. 1=enviroDIY, 2=?    
    MSP_ACTIVE s;
} provider_t;
#define mProvider_t(p1) provider_t p1
#else
#define mProvider_t(p1)
#endif  // USE_PS_provider

#define EP_HW_BOOT_ADDR 0
#define EP_PERSISTENT_STORE_ADDR (sizeof_hw_boot)
typedef struct {
    uint16_t crc16;        // Across persistent_store_t
    uint16_t struct_size;  // Of struct including crc8, struct_ver struct_size
    uint8_t  struct_ver;  // 1-255 - increment for any changes in this structure
    mModularSensorsCommon_t(msc);
    mModularSensorsNetwork_t(msn);
    mProvider_t(provider);
} app_storage_t;
typedef struct {
    mHw_boot_t(hw_boot);
    app_storage_t app;
} persistent_store_t;

//#define LOGGER_ID_ADDR ps.msc.s.logger_id

#endif  // ms_common_h
