/*****************************************************************************
ms_cfg.h - ModularSensors Config - remove this file to delete personalization info
 
dvlpRelDlvry1\ModularSensors\a\mayfly2_wifi_s6b\src
Status 230629: 0.34.2.aaa  Unique tracker
Written By:  Neil Hancock www.envirodiy.org/members/neilh20/
Development Environment: PlatformIO
Hardware Platform(s): EnviroDIY Mayfly Arduino Datalogger

Software License: BSD-3.
  Copyright (c) 2023, Neil Hancock - all rights assigned to Stroud Water
Research Center (SWRC) and they may change this title to Stroud Water Research
Center as required and the EnviroDIY Development Team


DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/
#ifndef mayfly2_wifi_s6b_ms_cfg_h
#define mayfly2_wifi_s6b_ms_cfg_h

#define WIFIID_SSID_DEF "WiFiSsid" 
#define WIFIPWD_DEF "WiFi pwd"
// Defaults for data.envirodiy.org
#define SENSIRION_SHT4X_UUID 1

#define LOGGERID_DEF_STR "itst02"
//https://monitormywatershed.org/sites/intg_test02/

const char *UUIDs[] =                                                      // UUID array for device sensors
{
    "c317f9ee-d0ca-410b-b051-11cb683c89xx",   // Sequence number (EnviroDIY_Mayfly_SampleNum)
    "135714b5-30fa-4fd9-83f4-3d70edce3xxx",   // Battery voltage (EnviroDIY_Mayfly_Batt)
    "f24f4af6-c469-4597-893b-9e9eee9e2bxx",   // Temperature (Sensirion_SHT40_Temperature)
    "2b8d3edb-e0ac-455e-99a4-09b2a859d6xx"    // Relative humidity (Sensirion_SHT40_Humidity)
};
const char *registrationToken = "8467bd56-3659-41a7-9fad-bd0c93b6bfxx";   // Device registration token
const char *samplingFeature   = "8eed720a-345d-4587-9a68-24cd77771cxx";   // Sampling feature UUID

#endif  // mayfly2_wifi_s6b_ms_cfg_h
