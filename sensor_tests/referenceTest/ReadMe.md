[//]: # ( @page example_mmw Monitor My Watershed Example )
# A ModularSensors reference Test for pushing data to  Monitor My Watershed/EnviroDIY

A sketch for easy testing and configuration of a https://monitormywatershed.org/
It can be used by a newbie to easly verify their environment, an expert to easly download and test the latest.

Required: A minimium hardware configuration of a Mayfly (rev0.5b or later) with a DigiWiFi S6 to provide a reference test environment.

The code base is referenced as src/* instead of  https://github.com/EnviroDIY/ModularSensors (develop)
This is the one gottcha - to sync to the latest online code, the user needs to do a "git pull"

This can be used for 
1) Stability testing of  https://github.com/EnviroDIY/ModularSensors (develop) 
    It can be modified in platform.io to point to any other stream eg https://github.com/EnviroDIY/ModularSensors (master) 
2) Easly verifying program changes before submitting a PR to the (develop). Requies some knowledge of git 

The test level sensors are Mayfly Sample Number, Mayfly Temperature, 
The ms_mmw_cfg.h will contain Token and UUID as downloaded from https://monitormywatershed.org/

Use: Start platformIo (actually VSC configured with PlatformIO)
From the top left click on File
- File -> Open Folder   
- select tools\retTestDevelop
(wait for it to start up and stabilize 1-5minutes)
on the bottom hover over the "tick" icon which will then show "PlatformIO: Build" 
Click on the "tick" icon - and it will start the build process. This requires the internet, and brings in all the dependencies - libraries which are stored in  .pio\libdeps\mayfly
Then it fill build - and should end with this and the RAM and FLASH per the project

Building in release mode
Checking size .pio\build\mayfly\firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [===       ]  28.7% (used 4708 bytes from 16384 bytes)
Flash: [=====     ]  54.6% (used 71046 bytes from 130048 bytes)
_______

Next 

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to log data to Monitor My Watershed/EnviroDIY](#using-modularsensors-to-log-data-to-monitor-my-watershedenvirodiy)
- [Walking Through the Code](#walking-through-the-code)
  - [PlatformIO Configuration](#platformio-configuration)
  - [The Complete Code](#the-complete-code)

[//]: # ( End GitHub Only )


[//]: # ( @section example_mmw_pio PlatformIO Configuration )
## PlatformIO Configuration

[//]: # ( @include{lineno} logging_to_MMW/platformio.ini )

[//]: # ( @section example_mmw_code The Complete Code )
## The Complete Code
