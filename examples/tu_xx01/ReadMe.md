# Geographical Customizations seperate file, to log data to EnviroDIY/Monitor My Watershed

This sketch uses 
a) ms_cfg.ini where  all the MMW UUID keys are added. This enables scaling multiple Mayflys with same software.  
b) sensors to provide a simple test bed for verifying access to log to https://monitormywatershed.org/ .
c) ms_cfg.h - that has all the constants for .ini file in one place.

Software Releases
=================
https://github.com/neilh10/ms_releases  - software available in .hex



Compatibility:
This section uses the following UUID steering, read from the ms_cfg.ini

ms_cfg.h : LT5_xxx
==================
Summary: Interfaces an LT500 over SDI-12 and a Keller Acculevel over RS485.
Requires  an SDI-12 interface board and RS485 wingboard for RS485
Requires ALL the following UUIDs be configured correctly, with vales from MMW.
[UUIDs]
ITROLL_DEPTH_UUID=        ;Gage height ft LT500 CTD-10_Depth
ITROLL_TEMP_UUID=         ;Temperature C LT500 Temp CTD-10_Temp
KellerXxlevel_Height_UUID=;Gage height m Keller_Acculevel_gageHeight
KellerXxlevel_Temp_UUID=  ;Temperature C Keller_Acculevel_Temp
DIGI_RSSI_UUID=           ;Received signal strength indication (Digi_Cellular_RSSI)
Batt_UUID=                ;Battery voltage EnviroDIY_Mayfly_Batt
Volt0_UUID=               ;Battery Voltage accurate 
SampleNumber_UUID=        ;Sequence number EnviroDIY_Mayfly_SampleNum

[PROVIDER]
REGISTRATION_TOKEN= ; registration token Test06
SAMPLING_FEATURE=   ; Sampling feature

There are two variants of the LT5_xx  LT5_lte & LT5_wifi
The LT5_lte requires the apn set, eg for verizon and Mayfly Xbee LTE board+ Xbee LTE modem condigured
[NETWORK]
apn=VZWINTERNET

[NETWORK]
The LT5_wifi requires the following and  have the apn set, and a Digi S6B wifi module 
WiFiId= XxxSsid ;Add yours
WiFiPwd= XxxPwd ; Needs to be a protected network.

ms_cfg.h : nano  with WiFi 
===============
Summary: Interfaces are Keller Nanolevel over RS485
Requires an RS485 wingboard for Keller Nanolevel
requires Wifi Xbee S6b modem and  [NETWORK] WiFiID and WiFiPwd
requires AOSONG2320 sensor plugged into JP4 I2C socket
[UUIDs]
KellerXxlevel_Height_UUID=;Gage height Keller_Acculevel_gageHeight
KellerXxlevel_Temp_UUID=  ;Temperature Keller_Acculevel_Temp
Air_Humidity_UUID=        ;AOSong_AM2315_Humidity
Air_Temperature_UUID=     ;AOSong_AM2315_Temp
MaximDS3231_TEMP_UUID =   ;EnviroDIY_Mayfly_Temp
Volt0_UUID=               ;Battery EnviroDIY_Mayfly_Batt
SampleNumber_UUID=        1da9af8c-168b-42e1-a0f7-e405bf1a307c ;Sequence EnviroDIY_Mayfly_SampleNum
DIGI_RSSI_UUID=           381bdd6a-f39f-465a-9f64-ddfcbc0c3e7b ;Digi_Cellular_RSSI



ms_cfg.h : test
===============
[UUIDs]
requires Wifi Xbee S6b modem and  [NETWORK] WiFiID and WiFiPwd
requires AOSONG2320 sensor plugged into JP4 I2C socket
Air_Humidity_UUID=        ;AOSong_AM2315_Humidity
Air_Temperature_UUID=     ;AOSong_AM2315_Temp
MaximDS3231_TEMP_UUID =   ;EnviroDIY_Mayfly_Temp
Volt0_UUID=               ;Battery EnviroDIY_Mayfly_Batt
SampleNumber_UUID=        1da9af8c-168b-42e1-a0f7-e405bf1a307c ;Sequence EnviroDIY_Mayfly_SampleNum
DIGI_RSSI_UUID=           381bdd6a-f39f-465a-9f64-ddfcbc0c3e7b ;Digi_Cellular_RSSI

ms_cfg.h : CTD (future Meteor CTD10)
====================================

EC - no ms_cfg.h to use
=======================
This doesn't use any UUDs as its standalone
Instrument configuration settings are stored in the microProcessors EEPROM.
Readings are accessed by swopping the uSD out.
ms_cfg.h is used to configure internal EEPROM


Other ms_cfg.ini settings
========================
<tbd>


Build Software.
==============
To build the output files, on a windows system in Platformio cmd shell enter .\do.ps1
To enable prviliges  
> Set-ExecutionPolicy -Scope CurrentUser unrestricted
> Get-Executionpolicy

Software Development
====================
This code can be built against current (and future releases), as is, and the same microSD card used for configuring a test MMW acces.

Other sensors capability will be added in time, and the sensors configuration will be from ms_cfg.ini/SD card.

To build this (assuming you've installed platformio),  on your local machine in a cmd window

$ git clone https://github.com/neilh10/ModularSensors.git  ModularSensors_neilh10

Then startup platformio, on top right open File->Open Folder 
Select ModularSensors_neilh10/examples/tu_id01

then build it, bottom left icons - either icone 'tick' or '->'