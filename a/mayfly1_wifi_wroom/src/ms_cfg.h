/*****************************************************************************
ms_cfg.h_wio_wifi - ModularSensors Config - MMW _Wio/Mayfly WiFi
Status 220617: 0.33.1.abaa
Written By:  Neil Hancock www.envirodiy.org/members/neilh20/
Development Environment: PlatformIO
Hardware Platform(s): EnviroDIY Mayfly Arduino Datalogger+RS485 Wingboard

Software License: BSD-3.
  Copyright (c) 2022, Neil Hancock - all rights assigned to Stroud Water
Research Center (SWRC) and they may change this title to Stroud Water Research
Center as required and the EnviroDIY Development Team


DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/
#ifndef mayfly1_wifi_wroom_ms_cfg_h
#define mayfly1_wifi_wroom_ms_cfg_h

#define WIFIID_SSID_DEF "WiFiSsid" 
#define WIFIPWD_DEF "pwd"
// Defaults for data.envirodiy.org
#define SENSIRION_SHT4X_UUID 1

// Defaults for data.envirodiy.org
#define LOGGERID_DEF_STR "itst01"
//https://monitormywatershed.org/sites/intg_test01/

const char *UUIDs[] =                                                      // UUID array for device sensors
{
    "a61813cd-ac96-49a3-b374-fec5dffb42xx",   // Sequence number (EnviroDIY_Mayfly_SampleNum)
    "597108b0-c147-49a2-a71d-704539d774xx",   // Battery voltage (EnviroDIY_Mayfly_Batt)
    "09fbbfde-5b9a-48c9-88a3-39d68463e0xx",   // Temperature (Sensirion_SHT40_Temperature)
    "e1f4579e-2d6d-4382-a6e8-a8b054fb02xx"    // Relative humidity (Sensirion_SHT40_Humidity)
};
const char *registrationToken = "945af9f2-709c-4d09-ab49-fc7070c337xx";   // Device registration token
const char *samplingFeature =   "63bec1f9-b704-435c-bf67-4b8a04a0b6xx";   // Sampling feature UUID


#endif  // mayfly1_wifi_wroom_ms_cfg_h