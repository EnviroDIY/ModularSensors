/*****************************************************************************
ms_cfg.h - ModularSensors Config - 
Testing envPr2a\ModularSensors\sensors_test\DRWI_SIM7080LTE\src
Status 230630: part of (develop) for 0.34.1 
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
#ifndef DRWI_SIM7080LTE_ms_cfg_h
#define ms_cfg_h
#define WIFIID_SSID_DEF "ArthurGuestSsid" 
#define WIFIPWD_DEF "Arthur8166"
// Defaults for data.envirodiy.org
//Site https://monitormywatershed.org/sites/bq_test01/
#define LOGGERID_DEF_STR "btest01"
#define NEW_LOGGERID_MAX_SIZE 40
#define registrationToken_UUID "22752220-5925-4a2c-aeb1-a57b58e1c246"
#define samplingFeature_UUID   "747478ef-4e80-4cc8-921e-89172d05ea42"
#define SEQUENCE_NUMBER_UUID   "e0d7b81b-0241-4017-b5dc-e90ecdb7c279"

//A tempororay repurpose of above UUIDs
//#define SENSIRON_HUMIDITY_UUID    ""
//#define SENSIRON_TEMPERATURE_UUID ""
//#define TEMPERATURE_D_UUID     ""

#define BAT_VOLTAGE_UUID       "9fdcefc1-b43f-4c3c-8d46-ca0e90845153"

#endif  // DRWI_SIM7080LTE_ms_cfg_h