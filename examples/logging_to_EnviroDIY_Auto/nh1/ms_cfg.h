#ifndef ms_cfg_h
#define ms_cfg_h
/*****************************************************************************
ms_cfg.h
Written By:  Neil Hancock www.envirodiy.org/members/neilh20/
Development Environment: PlatformIO
Hardware Platform: EnviroDIY  Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2018, Neil Hancock - all rights assigned to Stroud Water Research Center (SWRC)
  and they may change this title to Stroud Water Research Center as required
  and the EnviroDIY Development Team

This example sketch is written for ModularSensors library version 0.xx.0

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/
#define PROFILE01 1

#define PROFILE_NAME PROFILE01

#if   PROFILE_NAME == PROFILE01
//Autonomo SAMD21 - .platformio\packages\framework-arduinosam\variants\sodaq_autonomo
#define CONFIG_serialBaud 115200  // Baud rate for the primary serial port for debugging
#define CONFIG_greenLED  -1       // MCU pin for the green LED (-1 if not applicable)
#define CONFIG_redLED    LED_BUILTIN//D13 MCU pin for the red LED (-1 if not applicable)
#define CONFIG_buttonPin -1       // MCU pin for a button to use to enter debugging mode  (-1 if not applicable)
#define CONFIG_wakePin    1       // MCU interrupt/alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
#define CONFIG_sdCardPin  CS_SD //AutoPA27/46  MCU SD card chip select/slave select pin (must be given!)
#define CONFIG_sensorPowerPin -1 // MCU pin controlling main sensor power (-1 if not applicable)


#define CONFIG_VCC_SW_EXT_PIN  VCCSW
//Std SERIALUSB SERIAL1  - see variant.cpp for mapping to physical UARTS
// Serial -> PhySerial0
// Serial1 -->PhSerial5
// -DENABLE_SERIAL2 PhySerial4 ??D6  = RX, D7  = TX
// -DENABLE_SERIAL3 PhySerial3 ??D12 = RX, D13 = TX

// Describe the physical pin connection of your modem to your board
#define CONFIG_ModemBaud       9600 // Communication speed of the modem
#define CONFIG_modemVccPin       -2 // MCU pin controlling modem power (-1 if not applicable)
#define CONFIG_modemSleepRqPin   14 // MCU pin used for modem sleep/wake request (-1 if not applicable)
#define CONFIG_modemStatusPin    15 // MCU pin used to read modem status (-1 if not applicable)
#define CONFIG_modemStatusLevel LOW // The level of the status pin when the module is active (HIGH or LOW)
#define CONFIG_VCC_SW_XBEE_PIN  BEE_VCC
// MCU_CTS==BEERTS 
// MCU_RTS==BEECTS  
// BEEDTR
// RI_AI


#endif //PROFILE_NAME

#endif //ms_cfg_h