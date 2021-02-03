# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## v0.27.5 (2020-12-15)

## [Multiple new Sensors and Workflows](https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.27.5)

### Downloads

**[ModularSensors_Dependencies_0.27.5.zip](https://github.com/EnviroDIY/ModularSensors/releases/download/v0.27.5/ModularSensors_Dependencies_0.27.5.zip)** contains this version of ModularSensors _and the proper versions of all of its dependencies_.

### New Features:

#### New Sensors:
- PaleoTerra Redox sensors
- Northern Widget Tally Counters
- simple analog electrical conductance sensors
- InSitu RDO PRO-X rugged dissolved oxygen sensors

#### New Publisher:
- Add Soracom/Ubidots as a data publisher

#### Other New Features:
- Migrated from Travis CI to Github Actions for continuous integration
- Deprecated wiki, moving contents to docs folder where needed
- Add static variable for marked time in UTC
- Add user name and password for GSM/SIM for XBee Cellular
- Support software I2C or secondary hardware I2C for all sensors possible

### Improvements:
- Complete re-styling of the Doxygen output to be similar to envirodiy.org
- Add enourmous amounts of documentation
- Improved explanations and added walkthrough of menu a la carte example
- Added example calculating specific conductance where applicable
- For SDI-12 sensors, added calls to additional data commands (D1-D9) if full number of expected results are not returned by D0.
- For SDI-12 sensors, added ability to disable concurrent measurements using the build flag `MS_SDI12_NON_CONCURRENT`.
  - NOTE:  Setting the build flag disables concurrent measurements for *ALL* SDI-12 sensors!

### Known Issues
- The instructions for using most of the examples is out of date.


## v0.25.0

## [v0.25.0: Styling & Doxygen Code Documentation](https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.25.0)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3908530.svg)](https://doi.org/10.5281/zenodo.3908530)

### Downloads

**[libraries_v0.25.0.zip](https://github.com/EnviroDIY/ModularSensors/releases/download/v0.25.0/libraries_v0.25.0.zip)** contains this version of ModularSensors _and the proper versions of all of its dependencies_.

### New Features:
- Automated code documentation using [Doxygen](https://www.doxygen.nl/index.html), now available at https://envirodiy.github.io/ModularSensors/

### Improvements:
- Improved C++ code styling elements in every file in the library, to:
  - Better take advantage of the [Clang](https://clang.llvm.org) compiler and code error [linter](https://en.wikipedia.org/wiki/Lint_(software)) tool;
  - Generally follow [Clang Format](https://clang.llvm.org/docs/ClangFormat.html) and [Google's coding style guides](https://google.github.io/styleguide/cppguide.html) used with [cpplint](https://en.wikipedia.org/wiki/Cpplint) [linter](https://en.wikipedia.org/wiki/Lint_(software)), which:
    - includes massive white space changes to improve readability while not changing how the code is executed
  - Support Doxygen documentation.
- Encapsulated modem wake check logic into a function and checking for status using a quick AT ping if no other option is available.
    - This will only affect modems/breakouts that depend on a pulse on their wake pin to turn on or off but for whatever reason don't have a separate status pin connected to the mcu.  I *do not* recommend this configuration.
- Always re-set the pin mode of a sensor power pin before attempting to turn it on.
    - This could come into play when first attempting to power on a sensor before the initial setup if the pin mode on the sensor power pin was set to input by whatever program the mcu had run prior to running ModularSensors code.

### Bug Fixes
- Fixed issue where the Digi XBee LTE-M modem did not wake during normal logging mode to transmit data to the publisher. See https://github.com/EnviroDIY/ModularSensors/pull/309#commitcomment-39786167

For more details, see [Pull Request #309: The style sheet](https://github.com/EnviroDIY/ModularSensors/pull/309)


## v0.24.1

## [v0.24.1: Modem Restructuring](https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.24.1)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3693784.svg)](https://doi.org/10.5281/zenodo.3693784)

## Downloads

**[libraries_v0.24.1.zip](https://github.com/EnviroDIY/ModularSensors/releases/download/v0.24.1/libraries_v0.24.1.zip)** contains this version of ModularSensors _and the proper versions of all of its dependencies_.

### Improvements

Restructured modem so that it no longer operates as a sensor.  Variables tied to the modem are now effectively calculated variables and all values from the modem will be offset by 1 sending cycle (ie, the signal strength posted will always be the strength from the prior send, not the current one).


## v0.23.13

## [v0.23.13: More agressive attempts to set clock](https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.23.13)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3451413.svg)](https://doi.org/10.5281/zenodo.3451413)

## Downloads

**[libraries_v0.23.13.zip](https://github.com/EnviroDIY/ModularSensors/releases/download/v0.23.13/libraries_v0.23.13.zip)** contains this version of ModularSensors _and the proper versions of all of its dependencies_.

### Improvements:
- Much more aggressive attempts to set the clock if the time is not reasonable - before 01Sep2019 or after 01Jan2025.
    - The LED will flicker and warnings will be sent over the serial port for invalid times at every check.
    - The logDataAndPublish function will attempt to synchronize the clock at every measurement until a valid time is received.
   - The NIST sync will attempt to open the socket to the daytime server up to 12 times trying to get a valid response.
- Using automatic network technology and carrier profile for Digi LTE-M XBee3's - with the current firmware there is not a significant time savings in manually selecting the carrier and manually selecting the carrier gives no options when the relative signal strength of the carriers changes.


## v0.23.11

## [v0.23.11: Watchdogs and More](https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.23.11)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3405562.svg)](https://doi.org/10.5281/zenodo.3405562)

## Downloads

**[libraries_v0.23.11.zip](https://github.com/EnviroDIY/ModularSensors/releases/download/v0.23.11/libraries_v0.23.11.zip)** contains this version of ModularSensors _and the proper versions of all of its dependencies_.

#### Installing on the Arduino IDE:
In your the Arduino Software (IDE), import the libraries.zip file following the instructions for "Manual Installation" from https://www.arduino.cc/en/Guide/Libraries#toc5. You cannot directly import the zip using the Arduino Library Manager because it contains multiple libraries.

#### Installing for PlatformIO:
Extract the contents of libraries.zip to your project's 'lib' folder.

### New Features:
- A watch-dog timer has been implemented for both the AVR and SAMD21 (and 51) boards to restart the boards in case of failure during logging
    - The watch-dog is turned off during sleep to save power, so recovery is only possible if the failure is while the processor is awake.
- Added support for Meter Terros 11 soil moisture and temperature sensor
- Implemented a function to verify that UUID's are at least correctly formed and unique - though it does not verify that they are valid.
- Pushing to the master branch of this repo will now also cause a re-run of the travis script that updates the EnviroDIY "Libraries" repository.
- Added debugging variables to modems to track how long they are powered/active.

### Improvements:
- Added extra compile tests in the menu a la carte example.
- Improvements made to nearly all modem modules
- Restore XBee access to resets by TinyGSM

### Bug Fixes
- Fixed all compiler warnings seen with the -Wextra flag (mostly by adding technically-unnecessary-but-visually-helpful braces)
- Fixed issue with creating a calculated variable without a UUID

### Known Issues
- polling the AM2315 more frequently than every 2 seconds will now return a bad value (-9999) rather than returning the same value multiple times.  This is a reflection of a change in the Adafruit library.  The measurement time set for the sensor has always been this long so this issue should never be seen unless you attempt to call get measurement results from the AM2315 without first waiting for the measurement completion.  The update function and all variable array functions should behave properly.


## v0.22.5

## [v0.22.5: Modem Simplification](https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.22.5)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3255084.svg)](https://doi.org/10.5281/zenodo.3255084)

### NOTE:  This release is NOT backwards compatible with previous releases.
-  All code must be updated to the current format for modems and time zones to work with this version of the library.

## Downloads

**[libraries.zip](https://github.com/EnviroDIY/ModularSensors/releases/download/v0.22.5Libraries.zip)** contains this version of ModularSensors _**and the proper versions of all of its dependencies**_.

#### Installing on the Arduino IDE:
In your the Arduino Software (IDE), import the libraries.zip file following the instructions for "Manual Installation" from https://www.arduino.cc/en/Guide/Libraries#toc5. You cannot directly import the zip using the Arduino Library Manager because it contains multiple libraries.

#### Installing for PlatformIO:
Extract the contents of libraries.zip to your project's 'lib' folder.

### New Features:
- LoggerModem has become a parent class.  All modems now exist as separate subclass objects.
    - This should greatly simplify creating a modem object in code.
    - Currently supported modems:
        - Digi XBee and XBee3 cellular modems of all types running in transparent mode
        - Digi XBee3 cellular LTE-M modems in bypass mode (this is *in addition* to transparent mode)
        - Digi XBee cellular 3G global modems in bypass mode (this is *in addition* to transparent mode)
        - Digi XBee Wifi S6B modules
        - Sodaq UBee LTE-M (SARA R410M)
        - Sodaq UBee 3G (SARA U201)
        - Sodaq 2GBee R6/R7
        - Sodaq 2GBee R4, Adafruit Fona, and other SIMComm SIM800 modules
        - Botletics and other SIMCom SIM7000 modules
        - Dragino, Nimbelink, and other Quectel BG96 modules
        - Nimbelink LTE-M Verizon
        - Espressif ESP8266 based modules
    - The older way of creating a modem object and feeding it wake/sleep functions is no longer supported.
- The real time clock's timezone can now be explicit set (as opposed to setting the logger timezone and the offset between the RTC and the logger)
    - The function to set the logger timezone has been renamed from setTimeZone to setLoggerTimeZone.

### Improvements:
- Daily clock synchronization has been moved from midnight to noon.  For loggers with solar charging, this should place the extra draw of the modem to do the synchronization to a time of peak charging.
- All "send" data functions have been renamed to "publish" to line up with module names.
- Updated AM2315 to use most current Adafruit library (2.0.0)

### Bug Fixes
- Fixed #259 where time zones offset by more than 9 hours from UTC would not work correctly.
- Fixed #183 where enabling debugging would cause non AVR boards to crash.
- Adjusted some timing parameters for the SIM800 based on testing.

### Known Issues
- polling the AM2315 more frequently than every 2 seconds will now return a bad value (-9999) rather than returning the same value multiple times.  This is a reflection of a change in the Adafruit library.  The measurement time set for the sensor has always been this long so this issue should never be seen unless you attempt to call get measurement results from the AM2315 without first waiting for the measurement completion.  The update function and all variable array functions should behave properly.


## v0.21.4

## v0.21.4:  Deep debug error

## Downloads

**[libraries.zip](https://github.com/EnviroDIY/ModularSensors/releases/download/v0.21.4/Libraries.zip)** contains this version of ModularSensors _and the proper versions of all of its dependencies_.

#### Installing on the Arduino IDE:
In your the Arduino Software (IDE), import the libraries.zip file following the instructions for "Manual Installation" from https://www.arduino.cc/en/Guide/Libraries#toc5. You cannot directly import the zip using the Arduino Library Manager because it contains multiple libraries.

#### Installing for PlatformIO:
Extract the contents of libraries.zip to your project's 'lib' folder.

### Bug Fixes
- Fixed minor bug in debugging created in previous release


## v0.21.3

## v0.21.3: Minor Bugs and Simplified Debugging

## New Features:
 - Gave every header file a unique debugging define statement so each file can be debugged individually by building with the build flag -DMS_xxx_DEBUG where xxx is the file name in upper case.
    - Some files also have a "deep" debugging option with -DMS_xxx_DEBUG_DEEP
- Created examples for LearnEnviroDIYCode

## Bug Fixes:
- Fixed a bug causing modem to not be turned because the processor thought it was on already
- Corrected warm-up timing for Yosemitech Sonde
- Typo fixes in comments


## v0.21.2

## v0.21.2:  Fix write to SD card

### Bug Fixes
- Fixed bug intoduced in 0.21.0 preventing writing to SD card - file must be closed (not sync'ed)


## v0.21.0

## v0.21.0: Support for all Atlas Scientific I2C sensors, compiler-safe begin functions

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2586200.svg)](https://doi.org/10.5281/zenodo.2586200)

### New Features:
- Added full support for all Atlas Scientific EZO curcuits and sensors that support I2C
    - CO2
    - DO (dissolved oxygen)
    - EC (conductivity)
    - ORP (oxidation/reduction potential)
    - pH
    - RTD (temperature
- Created empty constructors for the logger, publisher, variable array, and variable classes and all of their subclasses.  For all classes created a corresponding "begin" function to set internal class object values.
    - See note for more details: https://github.com/EnviroDIY/ModularSensors/commit/b1a619ed74bc790743bce35b3a4e78a2d2237b22
    - The order of input arguments for all variable objects has changed.  For variable subclasses (ie, variables from sensors), there is no change to the user.  __**For calculated variable objects, all code must be updated!**__ Please check the structure in the examples!  Older code will compile without error but the variable metadata fields will be incorrectly populated.
- Very preliminary support for SD cards with switchable power

### Improvements:
- Eliminated any time-out waits on libraries using the Wire class
- Simplified the logger begin function - the removed functionality should be called in individual program setup functions
    - Removed clock sync
    - Removed logger file creation
    - Removed sensor setup
- Moved the check for setup from the wake function to the startSingleMeasurement function for all sensors
    - Previously, the first time the wake was called for a function, the status bit for setup was checked and if the sensor had not been setup, the setup function was called.  This will no longer happen the first time the wake function is called, but instead the first time the startSingleMeasurement function is called.  The main motivator for the change was the modem which could end up behaving strangely because the setup function actually called the wake function internally.
- Improved example coding for SARA R410 cellular modules

### Bug Fixes
- Fixed call to a null pointer causing the array's completeUpdate to crash with calculated variables
- Fixed source of ADS1x15 dependency
- Removed "default" wake/sleep functions from modem.  These were non-working skeleton functions.

### Known Issues
- Running some I2C sensors on switched power will cause unrecoverable hangs at the first call to any other I2C peripheral (ie, the DS3231 RTC) after sensor power is turned off.  This is a hardware problem and is un-fixable within this library.
- The sensor class and all of its subclasses still require input arguments in the constructor.


## v0.19.6

## v0.19.6: Modem Improvements & ADS1X15 Generalization

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2579301.svg)](https://doi.org/10.5281/zenodo.2579301)

### New Features:
- Added ADS1015 as an optional analog to digital converter (ADC) for boards other than the Mayfly

### Improvements:
- Modem waits for signal strength return before sending data
- Other modem tweaks
- Restructuring of data publishers

### Bug Fixes
- Fixed bug with ADS1115 for M0/SAM21


## v0.19.3

## v0.19.3: Bug fix and example re-working

### Improvements:
- Re-works the examples to remove duplication between them.

### Bug Fixes:
- Fixes bug in sending data to the WikiWatershed / [Monitor My Watershed](https://monitormywatershed.org/) data sharing portal.


## v0.19.2

## v0.19.2: Decreased Data Consumption

### New Features:
- Added ThingSpeak support via MQTT (PubSubClient is now a required dependency)
- Added support for TI INA219 thanks to @neilh21 (Adafruit INA219 library is now a required dependency)

### Improvements:
- Reduced telemetry data consumption (by half in one test) and therefore overall power use, by creating an outgoing text buffer.
  - Note : the default buffer size is quite large, decrease it to decrease program memory size.
- Refactored data publishers as a new class rather than subclasses of loggers


## v0.17.2

## v0.17.2: Major Update!

### New Features & Improvements
- Improved power management.
- Improved stability.
- A huge number of other changes, with most of them are documented here:  https://github.com/EnviroDIY/ModularSensors/pull/173

### NOTE: THIS RELEASE DESTROYS BACKWARDS COMPATIBILITY!!
- All `.ino` sketch files will need to be updated to follow the updated [examples](https://github.com/EnviroDIY/ModularSensors/tree/master/examples).
- All library dependencies will need to be updated to match versions in the [library.json](https://github.com/EnviroDIY/ModularSensors/blob/master/library.json) file.


## v0.12.2

## v0.12.2: Calculated variables and bug fixes

### New features and improvements:
* Implemented real calculated variables, from PR #153, closing issue #127 (Create generic calculated variables).

### Bug fixes
* Fixes issue with MaxSonar giving weird readings, due to buffer not being cleared between readings, as described in https://www.envirodiy.org/topic/minor-glitch-reading-maxbotix-mb7389-with-mayfly/

See PR #160 for a full list of improvements and fixes.

NOTE: This **THIS RELEASE DESTROYS BACKWARDS COMPATIBILITY!!** All `.ino` files will need to be updated to follow the updated examples.


## v0.11.6

## Fixed Longer Logger Intervals and Improved Documentation

- Fixed variable type for logging interval from uint8_t to uint16_t allowing consistent logging at intervals greater than 5 minutes

- Improved documentation for examples
- Applying "sensor testing mode" more consistently in examples
- Made variables necessary for sensor testing mode with custom loops public



## v0.11.3

## Added sensors and fixed timing bugs

New sensors:
- Added support for Freescale MPL115A2 pressure sensor
- Added support for Keller Acculevel
- Added support for Yosemitech Y4000 multi-parameter sonde
- Added support for a generic "voltage" measurement on an ADS1115 with and without a voltage divider
- Added support for an external rain tipping bucket counter deployed using the EnviroDIY TippingBucketRainGauge library and communicating with the main logger via I2C

New examples and documentation:
- Added some helpful details on the default I2C addresses of several sensors
- Expanded explanations of sensor and variable terminology in ReadMe
- Added 2 new examples of creating a calculated variable from those variables directly measured by multiple sensors
- Minor corrections to Yosemitech sensor resolution and accuracy.

Bug Fixes and other Tweeks:
- Fixed major bug causing sensors with long stabilization times to not be updated
- Tweeked code generating csv's and json outputs to allow a modified csv or json to be written to the SD card or posted to data.EnviroDIY.org
- Removed the "checkForUpdate()" function.  When asking for a value from a variable, you now must explicitly state whether you want the variable to ask its parent sensor for an updated value or not.  By default, it will _not_ ask the parent sensor to update, but only return the last value received or -9999 if a value has never been received.


## v0.9.0

## Timing Improvements

Major restructuring of sensor status and internal sensor time stamps to improve efficiency when using variables/sensors in arrays - this makes sensor/variable order much less important

Restructured Zebra Tech D-Opto

Added MS5803, external voltage, and external tip counter

Fixed bugs with modem and 5TM


## v0.6.10

## Fixes bugs in description and examples


## v0.6.9

## Uniformity of missing values, averaging for all sensors

- Allows all sensors to be averaged
- More uniform return of -9999 for missing/bad values
- Better time synching
- Example platformio.ini files


## 0.5.4-beta

## 0.5.4-beta

* Added Yosemitech brand sensors which communicate via RS485
* Improvements to modem support and debugging
* Clock synchronization fixes
* Improved examples
* Renaming of a large number of internal variables
* Library metadata and dependency fixes
* Inching closer to full support for SAMD21 (M0/Zero) processors, though still some bugs present.


## v0.3.0-beta

## Beta Release 0.3.0

Added the Apogee SQ-212 PAR sensor.

Changed the interrupt library to EnableInterrupt, which now controls all interrupts.

Added the modem as a "sensor" which can return it's signal strength as a variable.

Added a "debugging" mode, accessible by pushing a button while the checkForDebugMode function is running.


## v0.2.5-beta

## Impoved setup functions

Slight rearrangement of setup functions to improve efficiency.


## v0.2.4-beta

## Another beta release

Shorted time allowed to attempt to turn Bee on/off

Added a check that Bee is on before trying to connect to network

Shorted wait for network time to 55 sec (from 60)

Put disconnect step within if block so it only tries to disconnect if it was connected in the first place

Fixed extra spaces in CSV


## v0.2.2-beta

## Initial release

Our first release of the modular sensors library to support easily logging data from a variety of environmental sensors and sending that data to the EnviroDIY data portal.
