#ifndef ms_common_h
#define ms_common_h
/*****************************************************************************
ms_common.h
Written By:  Neil Hancock www.envirodiy.org/members/neilh20/
Development Environment: PlatformIO
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2018, Neil Hancock - all rights assigned to Stroud Water Research Center (SWRC)
  and they may change this title to Stroud Water Research Center as required
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.xx.0

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/
/*****************************************************************************
 * Support   ARDUINO_ARCH_AVR ARDUINO_ARCH_SAM ARDUINO_ARCH_SAMD 
 * Implementation: Due to ram limitations and of course Atmel Harvard (von Neuman) Architecture and Princeton (eg ARM) 
 * some special macros a required.
 * Atmel Harvard has seperate space for constants/program, where as Princeton is all the same memory space
 * C/Cpp only uses one space for all data and program
 * For Mayfly Atmel  ARDUINO_ARCH_AVR  assume Harvard Architecture and copy of CONSTs from PROGMEM
 * For not defined PROGMEM assume Princeton Architecture
 * examples 
 * \TinyGSM\src\TinyGsmCommon.h and subsequent useage eg TinyGsmClientXBee.h
 .platformio\platforms\atmelsam\boards adafruit feather_m0 feather_m4 ARDUINO_ARCH_SAMD
 .platformio\platforms\teensy\boards  teensylc -DTEENSYLC teensy36 -DTEENSY36
 

 ??.platformio\packages\framework-arduinoavr\libraries\WiFi\extras\wifiHD\src\SOFTWARE_FRAMEWORK\SERVICES\LWIP\lwip-1.3.2\src\core\pbuf.c
   SYS_ARCH_;
 * 
*/
// Local routing defitions here
#if defined(__AVR__)
  #define EDIY_PROGMEM PROGMEM
  typedef const __FlashStringHelper* EdiyConstStr;
  #define EFP(x) (reinterpret_cast<EdiyConstStr>(x))
  #define EF(x)  F(x)
#else
  #define EDIY_PROGMEM
  typedef const char* EdiyConstStr;
  #define EFP(x) x
  #define EF(x)  x
#endif
/*****************************************************************************
 * Persistent structures.
 * Defines data structures for per software build & geographical location customizations 
 * Needs to be extensible. If a change is made, ensure size increments for strucuture.
 * This is initially implemented as reading form .ini file on nicroSD.
 * The implementation could change in the future for parts of initial
 * Board level (including internal) Persistent Storage - 
 *    eg Internal EEPROM, though any board level data EEPROM or data flash is the same.  
 * This may be very useful for hw tracking (eg sn/rev)  and essential boot elements
 *   - Internal persistent storage presents special problems with upgrades
 * 

 * 
 */
//#define USE_PLAN_FOR_UPGRADE 1
#if defined(USE_PLAN_FOR_UPGRADE) 
#define STRCT_SZ(parm1) uint16_t parm1;
#else
#define STRCT_SZ(parm1)
#endif //USE_PLAN_FOR_UPGRADE
//#define USE_PS_HW_BOOT 1
//******
#if defined(USE_PS_HW_BOOT)
//Hardware Boot Structure - rarely expected to change
#define HW_BOOT_SERIAL_NUM_SZ 10
#define HW_BOOT_REV_SZ 8
#define HW_BOOT_NAME_SZ 10
#define HW_BOOT_SD_BOOT_INI_SZ 12
typedef struct 
{
    uint8_t struct_ver; //1-255 - increment for any changes in this structure
    uint8_t serial_num[HW_BOOT_SERIAL_NUM_SZ];
    uint8_t rev[HW_BOOT_REV_SZ];
    uint8_t name[HW_BOOT_NAME_SZ];
    uint8_t sd_boot_ini[HW_BOOT_SD_BOOT_INI_SZ];
} hw_boot_t;
#define mHw_boot_t(p1) hw_boot_t p1 
#else
#define mHw_boot_t(p1) 
#endif //USE_PS_HW_BOOT
//******
// For extensibility - each structures data size (not including sz) can be defined at the beginning of the structure
// this allows any parsing algorithims to step through the structures easly and also to invoke the right
// parm1: 1-65535 - struct size increase for any changes in this structure

//****** [COMMON]
#define USE_PS_modularSensorsCommon 1
//
#if defined(USE_PS_modularSensorsCommon)
#define MSC_LOGGER_ID_SZ 8
#define MSC_GEOLOCATION_ID_SZ 160
typedef struct {
    //v01 Initial structure 
    uint8_t logging_interval_min; 
    uint8_t time_zone;
    uint8_t logger_id[MSC_LOGGER_ID_SZ];
    uint8_t geolocation_id[];
    } msc01_t;
#define MSC_ACTIVE msc01_t
typedef struct {
    STRCT_SZ(sz) 
    MSC_ACTIVE s;
} modularSensorsCommon_t;
#define mModularSensorsCommon_t(p1) modularSensorsCommon_t p1 
#else
#define mModularSensorsCommon_t(p1)
#endif //USE_PS_modularSensorsCommon)

//******
#define USE_PS_modularSensorsNetwork 1
//******
#if defined(USE_PS_modularSensorsNetwork)
#define MSCN_APN_SZ 32
#define MSCN_WIFI_ID_SZ 32
#define MSCN_WIFI_PWD_SZ 32
typedef struct {
    char apn[MSCN_APN_SZ];     //32
    char WiFiId[MSCN_WIFI_ID_SZ]; //32? 
    char WiFiPwd[MSCN_WIFI_PWD_SZ]; //32??
    } msn01_t;
#define MSN_ACTIVE msn01_t
typedef struct {
    STRCT_SZ(sz)
    MSN_ACTIVE s;
} modularSensorsNetwork_t;
#define mModularSensorsNetwork_t(p1) modularSensorsNetwork_t p1 
#else
#define mModularSensorsNetwork_t(p1)
#endif //USE_PS_modularSensorsNetwork

//******
#define USE_PS_Provider 1
//******
#if defined(USE_PS_Provider)
#define UUIDE_CLOUD_ID_SZ 38 
#define UUIDE_REGISTRATION_TOKEN_SZ 38
#define UUIDE_SAMPLING_FEAUTRE_SZ 38
#define UUIDE_SENSOR_UUID_SZ 38
#define UUIDE_SENSOR_CNT_MAX_SZ 10
typedef struct {
    //v01 initial structure
    // All are in ascii strings, with the first unused octet \0
    uint8_t uuid_provider; 
    char cloudId[UUIDE_CLOUD_ID_SZ]; //ASCII url 
    char registration_token[UUIDE_REGISTRATION_TOKEN_SZ];
    char sampling_feature[UUIDE_SAMPLING_FEAUTRE_SZ];
    char uuid[UUIDE_SENSOR_CNT_MAX_SZ][UUIDE_SENSOR_UUID_SZ];
} uuid_envirodiy01_t;
#define UUID_ACTIVE uuid_envirodiy01_t
typedef struct {
    STRCT_SZ(sz)
    UUID_ACTIVE s;
} provider_t;
#define mProvider_t(p1) provider_t p1 
#else
#define provider_t(p1)
#endif //USE_PS_provider

typedef struct {
  mHw_boot_t(hw_boot);
  mModularSensorsCommon_t(msc);
  mModularSensorsNetwork_t(msn);  
  mProvider_t(provider);
} persistent_store_t;

#define LOGGER_ID_ADDR ps.msc.s.logger_id
#endif //ms_common_h