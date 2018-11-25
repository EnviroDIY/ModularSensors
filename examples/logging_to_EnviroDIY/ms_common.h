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
// Local routing defitions here

//#define sensorsVL(p1,p2,p3) sensorsVL##p3(p1,p2)
//#define sensorsVL0(p1,p2) 
//#define sensorsVL1(p1,p2) new p1(&p2,p1##_UUID),    


//#define VLsensorsVL(p1,p2,p3)
/*****************************************************************************
 * Persistent structures.
 * Defines data structures for per software build & geographical location customizations 
 * Needs to be extensible. If a change is made, ensure size increments for strucuture.
 */

//******
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

//******
// For extensibility - each structures data size (not including sz) can be defined at the beginning of the structure
// this allows any parsing algorithims to step through the structures easly and also to invoke the right
// parm1: 1-65535 - struct size increase for any changes in this structure
#define STRCT_SZ(parm1) uint16_t parm1;
//#define STRCT_SZ(sz)

//******
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


#define MSCN_APN_SZ 32
#define MSCN_WIFI_ID_SZ 32
#define MSCN_WIFI_PWD_SZ 32
typedef struct {
    uint8_t apn[MSCN_APN_SZ];     //32
    uint8_t WiFiId[MSCN_WIFI_ID_SZ]; //32? 
    uint8_t WiFiPwd[MSCN_WIFI_PWD_SZ]; //32??
    } msn01_t;
#define MSN_ACTIVE msn01_t
typedef struct {
    STRCT_SZ(sz)
    MSN_ACTIVE s;
} modularSensorsNetwork_t;
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

typedef struct {
  hw_boot_t               hw_boot;
  modularSensorsCommon_t  msc;
  modularSensorsNetwork_t msn;  
  provider_t              provider;
} persistent_store_t;
#endif //ms_common_h