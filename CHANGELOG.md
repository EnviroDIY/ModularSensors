# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
and its stricter, better defined, brother [Common Changelog](https://common-changelog.org/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

***

## [Unreleased]

## [0.32.0] - 2021-11-19
Reinstate support for AOSong DHT

### Changed
- **Documentation:** Restructured the changelog
- **Documentation:** Restyled and corrected documentation links for new and renamed DRWI examples
- **Continuous Integration:** Updated changelog reader and automated releaser

### Added
- Restored support for AOSong DHT sensor.

***

## [0.31.2] - 2021-11-03
Fix build without AOSong DHT

### Fixed
- Fixed build matrix to remove DHT

***

## [0.31.0] - 2021-11-02
Remove support for AOSong DHT

### Added
- **Examples:** Added a new example for the DRWI site using the new EnviroDIY bee based on the SIMCOM SIM7080G

### Removed
- **Breaking:** Temporarily **_REMOVED_** support for AOSong DHT sensor.
  - Intend to restore this after updating to support newest Adafruit DHT library

***

## [0.30.0] - 2021-07-06
Remove support for SoftwareWire for Atlas sensors

### Changed
- Changed build flags and created a pre-commit hook for myself to update the menu build matrix

### Added
- **Sensor:** Added support for [Campbell ClariVUE10](https://www.campbellsci.com/clarivue10) turbidity sensor

### Removed
- **Breaking:** Removed support for SoftwareWire for Atlas sensors.
  - The only supported version of a bit-banged (software) version of I2C removed inheritance from the core Wire library.
Without inheritance, the parseFloat functions used by the Atlas sensors will not work.
As I think this feature was completely unused for the Atlas sensors and I see no reason to use it with sensors that have completely flexible addressing, I removed it.

### Fixed
- Fixed GitHub actions for pull requests

***

## [0.29.1] - 2021-07-01
Fix YosemiTech Y533 ORP sensor outputs

### Fixed
- Modified `YosemitechY533.h` and examples to work with updated ORP `getValues()` function in https://github.com/EnviroDIY/YosemitechModbus released with v0.2.5.

***

## [0.29.0] - 2021-05-19
Create a ModularSensors.h

### Changed
- Modified examples to use Hydros 21

### Added
- Created a ModularSensors.h file to include.
This makes it much easiler to install and use the library from the Arduino CLI.
  - Modified examples to include the ModularSensors.h file
- Added continuous integration testing with the Arduino CLI

***

## [0.28.5] - 2021-05-11
Duplicate and Rename Hydros 21

### Added
- **Sensor:** Created a new module for the Meter Hydros 21.
This is exactly identical to the Decagon CTD in everything but the name.
The Decagon CTD module still exists and can be used.
No old code needs to be adjusted for this change.
Moving forward, the two can be used interchangably.
The addition was only made to stop complaints about typing in an older name.

***

## [0.28.4] - 2021-05-05
SDI-12 Timing Sensor Customization

### Changed
- Allow each SDI-12 sensor to set the necessary command delay for that sensor.
  - Per protocol, sensors are allowed to take up to 100ms after receiving a break before being ready to receive a command.
This allows each sensor to specify what delay it needs.
This was added to support conflicting delay needs; the RDO needed a short delay, the newest Meter sensors do not respond properly if the delay is added.
- For SDI-12 sensors, add repeated attempts to start a measurement if the first attempt unexpectedly fails.

***

## [0.28.3] - 2021-03-24
Valid version number

### Fixed
- Use a valid semantic version number

***

## [0.28.01] - 2021-02-10
Gigantic SDI-12 bug

### Fixed
- Fixes an **_EGREGIOUS_** error in the SDI-12 code causing the code to lock up if debugging was off (but always work with it on)
  - [Issue #346](https://github.com/EnviroDIY/ModularSensors/issues/346)
  - This was first introduced in [0.27.5](https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.27.5)
- Fix GitHub action for pull requests.

***

## [0.28.0] - 2021-02-10
Add Support for Turner Cyclops

### Added
- **Sensor:** Add support for all standard configurations of the [Turner Cyclops-7F submersible fluorometer](https://www.turnerdesigns.com/cyclops-7f-submersible-fluorometer)

### Fixed
- **Documentation:** Fix some minor Doxygen warnings
- **Documentation:** Small grammar fixes
- **Documentation:** Fixes in the EnviroDIY fork of m.css:
  - Added extra navigation hints to classes leading back to the containing module
  - Correctly included script for the to-top element

***

## [0.27.8] - 2021-01-19
Fix GitHub Action

### Fixed
- Just fixed an error in the github action to post a release

***

## [0.27.7] - 2021-01-19
SDI-12 Bug Fix

### Fixed
- Fixed a compiler error for non-concurrent SDI-12 sensors.

***

## [0.27.6] - 2021-01-19
Update Documentation

### Changed
- Update instructions for examples
- Update developer instructions

***

## [0.27.5] - 2020-12-15
Multiple new Sensors and Workflows

### Changed
- Complete re-styling of the Doxygen output to be similar to envirodiy.org
- Add enourmous amounts of documentation
- Improved explanations and added walkthrough of menu a la carte example
- Added example calculating specific conductance where applicable
- For SDI-12 sensors, added calls to additional data commands (D1-D9) if full number of expected results are not returned by D0.
- For SDI-12 sensors, added ability to disable concurrent measurements using the build flag `MS_SDI12_NON_CONCURRENT`.
  - NOTE:  Setting the build flag disables concurrent measurements for *ALL* SDI-12 sensors!

### Added
- **Sensor:** PaleoTerra Redox sensors
- **Sensor:** Northern Widget Tally Counters
- **Sensor:** simple analog electrical conductance sensors
- **Sensor:** InSitu RDO PRO-X rugged dissolved oxygen sensors
- **Publisher:** Add Soracom/Ubidots as a data publisher
- Migrated from Travis CI to Github Actions for continuous integration
- Deprecated wiki, moving contents to docs folder where needed
- Add static variable for marked time in UTC
- Add user name and password for GSM/SIM for XBee Cellular
- Support software I2C or secondary hardware I2C for all sensors possible

### Known Issues
- The instructions for using most of the examples are out of date.

***

## [0.25.0] - 2020-06-25
Styling & Doxygen Code Documentation

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3908530.svg)](https://doi.org/10.5281/zenodo.3908530)

### Changed
- Improved C++ code styling elements in every file in the library, to:
  - Better take advantage of the [Clang](https://clang.llvm.org) compiler and code error [linter](https://en.wikipedia.org/wiki/Lint_(software)) tool;
  - Generally follow [Clang Format](https://clang.llvm.org/docs/ClangFormat.html) and [Google's coding style guides](https://google.github.io/styleguide/cppguide.html) used with [cpplint](https://en.wikipedia.org/wiki/Cpplint) [linter](https://en.wikipedia.org/wiki/Lint_(software)), which:
    - includes massive white space changes to improve readability while not changing how the code is executed
  - Support Doxygen documentation.
- Encapsulated modem wake check logic into a function and checking for status using a quick AT ping if no other option is available.
    - This will only affect modems/breakouts that depend on a pulse on their wake pin to turn on or off but for whatever reason don't have a separate status pin connected to the mcu.  I *do not* recommend this configuration.
- Always re-set the pin mode of a sensor power pin before attempting to turn it on.
    - This could come into play when first attempting to power on a sensor before the initial setup if the pin mode on the sensor power pin was set to input by whatever program the mcu had run prior to running ModularSensors code.

### Added
- **Documentation:** Automated code documentation using [Doxygen](https://www.doxygen.nl/index.html), now available at https://envirodiy.github.io/ModularSensors/


### Fixed
- Fixed issue where the Digi XBee LTE-M modem did not wake during normal logging mode to transmit data to the publisher. See https://github.com/EnviroDIY/ModularSensors/pull/309#commitcomment-39786167

For more details, see [Pull Request #309: The style sheet](https://github.com/EnviroDIY/ModularSensors/pull/309)

***

## [0.24.1] - 2020-03-02
Modem Restructuring

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3693784.svg)](https://doi.org/10.5281/zenodo.3693784)

### Changed
- Restructured modem so that it no longer operates as a sensor.
  Variables tied to the modem are now effectively calculated variables and all values from the modem will be offset by 1 sending cycle (ie, the signal strength posted will always be the strength from the prior send, not the current one).

***

## [0.23.13] - 2019-09-19
More agressive attempts to set clock

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3451413.svg)](https://doi.org/10.5281/zenodo.3451413)

### Changed
- Much more aggressive attempts to set the clock if the time is not reasonable - before 01Sep2019 or after 01Jan2025.
    - The LED will flicker and warnings will be sent over the serial port for invalid times at every check.
    - The logDataAndPublish function will attempt to synchronize the clock at every measurement until a valid time is received.
    - The NIST sync will attempt to open the socket to the daytime server up to 12 times trying to get a valid response.
- Now using automatic network technology and carrier profile for Digi LTE-M XBee3's
   - with the current firmware there is not a significant time savings in manually selecting the carrier and manually selecting the carrier gives no options when the relative signal strength of the carriers changes.

***

## [0.23.11] - 2019-09-11
Watchdogs and More

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3405562.svg)](https://doi.org/10.5281/zenodo.3405562)

### Changed
- Added extra compile tests in the menu a la carte example.
- Improvements made to nearly all modem modules
- Restore XBee access to resets by TinyGSM

### Added
- A watch-dog timer has been implemented for both the AVR and SAMD21 (and 51) boards to restart the boards in case of failure during logging
    - The watch-dog is turned off during sleep to save power, so recovery is only possible if the failure is while the processor is awake.
- Added support for Meter Terros 11 soil moisture and temperature sensor
- Implemented a function to verify that UUID's are at least correctly formed and unique - though it does not verify that they are valid.
- Pushing to the master branch of this repo will now also cause a re-run of the travis script that updates the EnviroDIY "Libraries" repository.
- Added debugging variables to modems to track how long they are powered/active.

### Fixed
- Fixed all compiler warnings seen with the -Wextra flag (mostly by adding technically-unnecessary-but-visually-helpful braces)
- Fixed issue with creating a calculated variable without a UUID

### Known Issues
- polling the AM2315 more frequently than every 2 seconds will now return a bad value (-9999) rather than returning the same value multiple times.  This is a reflection of a change in the Adafruit library.  The measurement time set for the sensor has always been this long so this issue should never be seen unless you attempt to call get measurement results from the AM2315 without first waiting for the measurement completion.  The update function and all variable array functions should behave properly.

***

## [0.22.5] - 2019-06-24
Modem Simplification

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3255084.svg)](https://doi.org/10.5281/zenodo.3255084)

**NOTE:  This release is NOT backwards compatible with previous releases.**
-  All code must be updated to the current format for modems and time zones to work with this version of the library.

### Changed
- Daily clock synchronization has been moved from midnight to noon.  For loggers with solar charging, this should place the extra draw of the modem to do the synchronization to a time of peak charging.
- All "send" data functions have been renamed to "publish" to line up with module names.
- Updated AM2315 to use most current Adafruit library (2.0.0)

### Added
- **Breaking:** LoggerModem has become a parent class.  All modems now exist as separate subclass objects.
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

### Fixed
- Fixed #259 where time zones offset by more than 9 hours from UTC would not work correctly.
- Fixed #183 where enabling debugging would cause non AVR boards to crash.
- Adjusted some timing parameters for the SIM800 based on testing.

### Known Issues
- polling the AM2315 more frequently than every 2 seconds will now return a bad value (-9999) rather than returning the same value multiple times.  This is a reflection of a change in the Adafruit library.  The measurement time set for the sensor has always been this long so this issue should never be seen unless you attempt to call get measurement results from the AM2315 without first waiting for the measurement completion.  The update function and all variable array functions should behave properly.

***

## [0.21.4] - 2019-05-02
Deep debug error

### Fixed
- Fixed minor bug in debugging created in previous release

***

## [0.21.3] - 2019-05-02
Minor Bugs and Simplified Debugging

### Added
 - Gave every header file a unique debugging define statement so each file can be debugged individually by building with the build flag -DMS_xxx_DEBUG where xxx is the file name in upper case.
    - Some files also have a "deep" debugging option with -DMS_xxx_DEBUG_DEEP
- Created examples for LearnEnviroDIYCode

### Fixed
- Fixed a bug causing modem to not be turned because the processor thought it was on already
- Corrected warm-up timing for Yosemitech Sonde
- Typo fixes in comments

***

## [0.21.2] - 2019-03-19
Fix write to SD card

### Fixed
- Fixed bug intoduced in 0.21.0 preventing writing to SD card - file must be closed (not sync'ed)

***

## [0.21.0] - 2019-03-06
Support for all Atlas Scientific I2C sensors, compiler-safe begin functions

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2586200.svg)](https://doi.org/10.5281/zenodo.2586200)

### Changed
- Eliminated any time-out waits on libraries using the Wire class
- Simplified the logger begin function - the removed functionality should be called in individual program setup functions
    - Removed clock sync
    - Removed logger file creation
    - Removed sensor setup
- Moved the check for setup from the wake function to the startSingleMeasurement function for all sensors
    - Previously, the first time the wake was called for a function, the status bit for setup was checked and if the sensor had not been setup, the setup function was called.  This will no longer happen the first time the wake function is called, but instead the first time the startSingleMeasurement function is called.  The main motivator for the change was the modem which could end up behaving strangely because the setup function actually called the wake function internally.
- Improved example coding for SARA R410 cellular modules

### Added
- **Sensor:** Added full support for all Atlas Scientific EZO curcuits and sensors that support I2C
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

### Removed
- Removed "default" wake/sleep functions from modem.  These were non-working skeleton functions.

### Fixed
- Fixed call to a null pointer causing the array's completeUpdate to crash with calculated variables
- Fixed source of ADS1x15 dependency

### Known Issues
- Running some I2C sensors on switched power will cause unrecoverable hangs at the first call to any other I2C peripheral (ie, the DS3231 RTC) after sensor power is turned off.  This is a hardware problem and is un-fixable within this library.
- The sensor class and all of its subclasses still require input arguments in the constructor.

***

## [0.19.6] - 2019-02-27
Modem Improvements & ADS1X15 Generalization

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2579301.svg)](https://doi.org/10.5281/zenodo.2579301)

### Changed
- Modem waits for signal strength return before sending data
- Other modem tweaks
- Restructuring of data publishers

### Added
- Added ADS1015 as an optional analog to digital converter (ADC) for boards other than the Mayfly

### Fixed
- Fixed bug with ADS1115 for M0/SAM21

***

## [0.19.3] - 2019-01-15
Bug fix and example re-working

### Changed
- Re-works the examples to remove duplication between them.

### Fixed
- Fixes bug in sending data to the WikiWatershed / [Monitor My Watershed](https://monitormywatershed.org/) data sharing portal.

***

## [0.19.2] - 2018-12-22
Decreased Data Consumption

### Changed
- Reduced telemetry data consumption (by half in one test) and therefore overall power use, by creating an outgoing text buffer.
  - Note : the default buffer size is quite large, decrease it to decrease program memory size.
- Refactored data publishers as a new class rather than subclasses of loggers

### Added
- Added ThingSpeak support via MQTT (PubSubClient is now a required dependency)
- Added support for TI INA219 thanks to @neilh21 (Adafruit INA219 library is now a required dependency)

***

## [0.17.2] - 2018-11-27
Major Update!

**NOTE: THIS RELEASE DESTROYS BACKWARDS COMPATIBILITY!!**
- All `.ino` sketch files will need to be updated to follow the updated [examples](https://github.com/EnviroDIY/ModularSensors/tree/master/examples).
- All library dependencies will need to be updated to match versions in the [library.json](https://github.com/EnviroDIY/ModularSensors/blob/master/library.json) file.

### Changed
- Improved power management.
- Improved stability.
- A huge number of other changes, with most of them are documented here:  https://github.com/EnviroDIY/ModularSensors/pull/173

***

## [0.12.2] - 2018-09-25
Calculated variables and bug fixes

NOTE: This **THIS RELEASE DESTROYS BACKWARDS COMPATIBILITY!!** All `.ino` files will need to be updated to follow the updated examples.

### Added
- Implemented real calculated variables, from PR #153, closing issue #127 (Create generic calculated variables).

### Fixed
- Fixes issue with MaxSonar giving weird readings, due to buffer not being cleared between readings, as described in https://www.envirodiy.org/topic/minor-glitch-reading-maxbotix-mb7389-with-mayfly/

See PR #160 for a full list of improvements and fixes.

***

## [0.11.6] - 2018-05-11
Fixed Longer Logger Intervals and Improved Documentation

### Changed
- **Documentation:** Improved documentation for examples
- Applying "sensor testing mode" more consistently in examples
- Made variables necessary for sensor testing mode with custom loops public

### Fixed
- Fixed variable type for logging interval from uint8_t to uint16_t allowing consistent logging at intervals greater than 5 minutes

***

## [0.11.3] - 2018-05-03
Added sensors and fixed timing bugs

### Changed
- Tweeked code generating csv's and json outputs to allow a modified csv or json to be written to the SD card or posted to data.EnviroDIY.org

### Added
- **Sensor:** Added support for Freescale MPL115A2 pressure sensor
- **Sensor:** Added support for Keller Acculevel
- **Sensor:** Added support for Yosemitech Y4000 multi-parameter sonde
- **Sensor:** Added support for a generic "voltage" measurement on an ADS1115 with and without a voltage divider
- **Sensor:** Added support for an external rain tipping bucket counter deployed using the EnviroDIY TippingBucketRainGauge library and communicating with the main logger via I2C
- **Documentation:** Added some helpful details on the default I2C addresses of several sensors
- **Documentation:** Expanded explanations of sensor and variable terminology in ReadMe
- **Documentation:** Added 2 new examples of creating a calculated variable from those variables directly measured by multiple sensors
- **Documentation:** Minor corrections to Yosemitech sensor resolution and accuracy.

### Removed
- Removed the "checkForUpdate()" function.
  When asking for a value from a variable, you now must explicitly state whether you want the variable to ask its parent sensor for an updated value or not.
  By default, it will _not_ ask the parent sensor to update, but only return the last value received or -9999 if a value has never been received.

### Fixed
- Fixed major bug causing sensors with long stabilization times to not be updated

***

## [0.9.0] - 2018-04-17
Timing Improvements

### Changed
- Major restructuring of sensor status and internal sensor time stamps to improve efficiency when using variables/sensors in arrays - this makes sensor/variable order much less important
- Restructured Zebra Tech D-Opto

### Added
- Added MS5803, external voltage, and external tip counter

### Fixed
- Fixed bugs with modem and 5TM

***

## [0.6.10] - 2018-02-26
Fixes bugs in description and examples

### Fixed
- Small bug-fixing release

***

## [0.6.9] - 2018-02-26
Uniformity of missing values, averaging for all sensors

### Changed
- Allows all sensors to be averaged
- More uniform return of -9999 for missing/bad values
- Better time synching
- Example platformio.ini files

## [0.5.4-beta] - 2018-01-18

### Changed
- Improvements to modem support and debugging
- Improved examples
- Renaming of a large number of internal variables
- Inching closer to full support for SAMD21 (M0/Zero) processors, though still some bugs present.

### Added
- **Sensor:** Added Yosemitech brand sensors which communicate via RS485

### Fixed
- Clock synchronization fixes
- Library metadata and dependency fixes

***

## [0.3.0]-beta - 2017-06-07
Beta Release

### Changed
- Changed the interrupt library to EnableInterrupt, which now controls all interrupts.

### Added
- **Sensor:** Added the Apogee SQ-212 PAR sensor.
- Added the modem as a "sensor" which can return it's signal strength as a variable.
- Added a "debugging" mode, accessible by pushing a button while the checkForDebugMode function is running.

***

## [0.2.5]-beta - 2017-05-31
Impoved setup functions

### Changed
- Slight rearrangement of setup functions to improve efficiency.

***

## [0.2.4]-beta - 2017-05-17
Another beta release

### Changed
- Shorted time allowed to attempt to turn Bee on/off
- Shorted wait for network time to 55 sec (from 60)
- Put disconnect step within if block so it only tries to disconnect if it was connected in the first place

### Added
- Added a check that Bee is on before trying to connect to network

### Fixed
- Fixed extra spaces in CSV

***

## [0.2.2]-beta - 2017-05-09
Initial release

Our first release of the modular sensors library to support easily logging data from a variety of environmental sensors and sending that data to the EnviroDIY data portal.

***

[Unreleased]: https://github.com/EnviroDIY/ModularSensors/compare/v0.31.2...HEAD
[0.31.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.31.2
[0.31.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.31.0
[0.30.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.30.0
[0.29.1]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.29.1
[0.29.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.29.0
[0.28.5]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.5
[0.28.4]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.4
[0.28.3]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.3
[0.28.1]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.1
[0.28.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.0
[0.27.8]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.27.8
[0.27.7]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.27.7
[0.27.6]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.27.6
[0.27.5]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.27.5
[0.25.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.25.0
[0.24.1]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.24.1
[0.23.13]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.23.13
[0.23.11]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.23.11
[0.22.5]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.22.5
[0.21.4]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.21.4
[0.21.3]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.21.3
[0.21.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.21.2
[0.21.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.21.0
[0.19.6]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.19.6
[0.19.3]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.19.3
[0.19.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.19.2
[0.17.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.17.2
[0.12.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.12.2
[0.11.6]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.11.6
[0.11.3]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.11.3
[0.9.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.9.0
[0.6.10]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.6.10
[0.6.9]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.6.9
[0.5.4-beta]: https://github.com/EnviroDIY/ModularSensors/releases/tag/0.5.4-beta
[0.3.0-beta]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.3.0-beta
[0.2.5-beta]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.2.5-beta
[0.2.4-beta]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.2.4-beta
[0.2.2-beta]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.2.2-beta
