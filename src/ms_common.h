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
//#include "ms_cfg.h"

#if defined USE_PS_EEPROM
#include <util/crc16.h>
#endif  // USE_PS_EEPROM

#endif  // ms_common_h
