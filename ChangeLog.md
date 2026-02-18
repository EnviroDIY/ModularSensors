# ChangeLog<!--! {#change_log} -->

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and its stricter, better defined, brother [Common Changelog](https://common-changelog.org/).

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

***

## [Unreleased]

### Changed

- **BREAKING** Changed capitalization of `setInitialShortIntervals(#)` function
  - Previously the 'i' of initial was not capitalized.
- Made the enabling and disabling of the watchdog the very first and very last steps of sleep to keep the watchdog enabled through the whole getting ready for bed and waking up process.
- **ANB pH**
  - **BREAKING** The constructor has changed!
The logging interval has been added as a required parameter for the constructor!
  - Changed timing slightly and simplified timing logic.
- **Renamed** The EnviroDIYPublisher has been renamed the MonitorMyWatershedPublisher.
This reflects changes to the website from years ago.
There is a shell file and typedef to maintain backwards compatibility.
- Bumped several dependencies - including crucial bug fixes to SensorModbusMaster.
- Re-wrote most of the logic for looping variables within the complete update function of the VariableArray.
- Simplified the `addSingleMeasurementResult()` function of all sensors to use an internal function to help set the bits and timing values and to quit sooner if the measurement was not started successfully.
  - The `verifyAndAddMeasurementResult()` is now consistently used in all sensors and is only called when the sensor successfully returned a measurement response.
  - Also removed all places where sensor values were re-set to -9999 after a measurement failed and then that -9999 was sent to the `verifyAndAddMeasurementResult()` function.
These resets were an awkward attempt to deal with bad values before feeding any bad values to the `verifyAndAddMeasurementResult()` function which was previously always called even if the sensor returned junk.
This was probably a hold-over from incorrect implementation and calling of the clearValues function deep in the library history.
  - Also made the return from the `addSingleMeasurementResult()` function consistently false for a bad sensor response and true for a good one - where it's possible to tell the difference.
- The Sensor::clearValues() function now resets the attempt and retry counts in addition to setting all values in the value array to -9999.
- Re-wrote some of the logic of the `completeUpdate()` function.
Also added optional arguments to the `completeUpdate()` function to allow users to specify if the sensors should be powered/woken.
  - The `updateAllSensors()` function is now deprecated.
Use `completeUpdate(false, false, false, false)` instead.
    - Previously the `updateAllSensors()` function asked all sensors to update their values, skipping all power, wake, and sleep steps while the `completeUpdate()` function duplicated that functionality and added the power, wake, and sleep.
The two functions have been consolidated into one function with four arguments, one each for power on, wake, sleep, and power off.
To achieve the same functionality as the old `updateAllSensors()` function (ie, only updating values), set all the arguments to false.
- Applied many suggestions from Code Rabbit AI.
- Moved outdated examples to a new "Outdated" folder, with a subfolder for the DRWI examples
- When importing TinyGSM for the modem objects, the specific modem client headers are now imported directly rather than importing the TinyGsmClient.h header which defines typedefs for the sub-types.

### Added

- **Added support for retrying measurements for all sensors**.
  - Each sensor now supports a number of possible retry attempts for when the sensor returns a bad or no value.
The number of retry attempts can be set using the `setAllowedMeasurementRetries(uint8_t)` function.
  - The number of retries is independent of the number of measurements to average.
A retry is performed when a sensor doesn't report a value or reports an error value.
If multiple retries are needed, only the result of the final (successful) retry is stored.
When multiple 'measurements to average' are requested, the values of each successful measurement is stored and averaged.
Measurements that return bad values even after retries are still not included in averaging.
  - The default number of retry attempts for most sensors is 1.
- Made a secondary power pin a property of all sensors.
- Added internal function to run the steps of setting the timing and bits after a measurement.
- Added setter and getter functions for sensor timing variables.
These values should generally be set in the specific sensor constructors and only changed if you know what you're doing.
  - `setWarmUpTime(uint32_t warmUpTime_ms)`
  - `getWarmUpTime()`
  - `setStabilizationTime(uint32_t stabilizationTime_ms)`
  - `getStabilizationTime()`
  - `setMeasurementTime(uint32_t measurementTime_ms)`
  - `getMeasurementTime()`
- Added the function `Sensor::clearStatus()` which resets all status bits except setup and error and resets all timing values to 0.
- **NEW SENSOR** Added a new sensor for simple analog voltage using the built-in processor ADC
- Added KnownProcessors.h and moved defines values for supported built-in sensors on known processors to that file.
  - This affects ProcessorStats and the Everlight ALS PT-19.
- Added a new example specific to the [EnviroDIY Monitoring Station Kit](https://www.envirodiy.org/product/envirodiy-monitoring-station-kit/).

### Removed

- Remove the unused `_maxSamplesToAverage` parameter of the VariableArray and the `countMaxToAverage()` function which set the parameter.
- Removed unnecessary copy doc calls for inherited functions and properties.
- Removed all overrides of the powerUp and powerDown functions that are no longer needed since all sensors have two power pins built in.
- Removed references to the EnviroDIY data portal.
- Removed all defines from examples sketches.
Defining values to be used by TinyGSM and/or the MQTT library here in addition to any defines in the ModSensorConfig.h or in a build configuration can lead to One Definition Rule violations because the define values are used when creating the classes from the templates in TinyGSM.

### Fixed

- Fixed major bug where sensors with two power pins where either was shared with another sensor may be turned off inappropriately when one of the other sensors was turned off.
- Correctly retry NIST sync on XBees when a not-sane timestamp is returned.

***

## [0.37.0]

> [!note]
> This release has changes to nearly every file in the entire library (ie, hundreds of files).
> Many of the changes are spelling and typo fixes found by implementing CSpell code spell checking.
> All header files were also modified to include the new library configuration headers.

### Changed

- **BREAKING** Converted the watch-dog classes in to static classes with all static function and a **deleted constructor**.
  - Any code that attempted to interact with the watchdog (ie, with a "complex loop") must now call the extendedWatchDog class directly, ie: `extendedWatchDog::resetWatchDog();` rather than `dataLogger.watchDogTimer.resetWatchDog();`
- **BREAKING** Renamed `markedLocalEpochTime` to `markedLocalUnixTime` to clarify the start of the epoch that we're marking down.
- **BREAKING** Renamed `markedUTCEpochTime` to `markedUTCUnixTime` to clarify the start of the epoch that we're marking down.
- **Potentially BREAKING:** Changed the requirements for a "sane" timestamp to between 2025 and 2035.
  - Moved the value for the sane range into two defines: `EARLIEST_SANE_UNIX_TIMESTAMP` and `LATEST_SANE_UNIX_TIMESTAMP` so they can be more easily modified and tracked.
These defines can be set in the ModSensorConfig.h file.
- **Potentially BREAKING:** For calculated variables, the calculation function will only be called if `getValue(true)` or `getValueString(true)` is called - that is, the boolean for 'update value' must explicitly be set to true to rerun the calculation function.
  - Previously, the calculation function was re-run every time `getValue()` or `getValueString()` was called, regardless of the update value parameter.
For calculations that were based on the results of other variables that didn't change, this was fine.
But, for calculations based on new raw readings (ie, calling `analogRead()`) a new value would be returned each time the function was called.
I realized this was a problem for analog values I tried to read that reported correctly in the first round, but were saved as junk in the csv and publishers because a new analog reading was being attempted when the thing I was attempting to read was now powered down.
  - The variable array update functions have been modified accordingly.
  - Verify you have the functionality you expect if you use calculated variables.
- Removed the enable/disable wake pin interrupt at every sleep interval in favor of a single attachment during the begin.
- Moved all code for communication with the RTC into the new static class loggerClock().
- Deprecated functions, to be removed in a future version:
  - `Logger::setRTCTimeZone(timeZone)`; use `loggerClock::setRTCOffset(_offsetHours)` in new code.
  - `Logger::getRTCTimeZone()`; use `loggerClock::getRTCOffset()` in new code.
  - `Logger::setRTClock(UTCEpochSeconds)`; use `loggerClock::setRTClock(ts, utcOffset, epoch)` in new code.
  - `Logger::isRTCSane()`; use `loggerClock::isRTCSane()` in new code.
  - `Logger::wakeISR()`; use `loggerClock::rtcISR()` in new code.
- Support timestamps as time_t objects instead of uint32_t where every sensible.
  - The size of a uint32_t is always 32 bits, but the size of the time_t object varies by processor - for some it is 32 bits, for other 64.
- Changed the watchdog from a fixed 15 minute reset timer to 2x the logging interval (or at least 5 minutes).
- Modified all examples which define a sercom serial port for SAMD21 processors to require the defines for the supported processors.
This should only make a difference for my compilation tests, real users should pick out only the chunks of code they want rather than leave conditional code in place.
- Changed some fill-in-the-blank spots in the menu example to only set the value in a single spot in the code.
- Unified all defines related to the resolution of the processor ADC and moved them to the new configuration file.
  - Applies only to sensors using the built-in processor ADC:
    - Internal battery, analog light sensor, and analog electrical conductivity
  - *You can no-longer set a separate processor resolution for each sensor.*
- You no longer need to input a client object into the constructor for any of the publishers if you are using a loggerModem object.
  - For best results, input a client object to the constructor only if you are **NOT** using a loggerModem object.
- Split functionality for the ESP8266 and the ESP32.
  - If you are using an EnviroDIY wifi bee, you should update your code to correctly select the ESP32, which is more feature rich than the ESP8266.
- Changed the way debugging printouts are generated in the publishers.
- Changed the default functionality of the "testing" mode.
  - When the button pin is changed to activate testing mode, a single sample will be taken and published immediately.
  - To restore the previous functionality (beginning a loop of 25 measurements) use the configuration/build flag `MS_LOGGERBASE_BUTTON_BENCH_TEST`.
  - The function for the original testing mode has been renamed to `benchTestingMode()`.
- Added check for `isnan(...)` to the `Sensor::verifyAndAddMeasurementResult(...)` function.
- Pass pointers to c-style character strings instead of String objects where easily done.
- Separated functions for setting button/rtc pin modes and attaching their interrupts.
- Corrected as many spelling errors as I could find.

### Added

- **CONFIGURATION** Added a two configuration files (ModSensorConfig.h and ModSensorDebugConfig.h) that all files read from to check for configuration-related defines.
This allows Arduino IDE users who are unable to use build flags to more easily configure the library or enable debugging.
It also allows PlatformIO users to avoid the time-consuming re-compile of all their libraries required when changing build flags.
  - **ALL** library configuration build flags previously in any other header file for the library have been moved into the ModSensorConfig.h file, including ADC, SDI-12, and variable array options.
- Added support for caching readings in RAM and sending in batches.
This currently only works on the EnviroDIY/Monitor My Watershed Publisher.
Thank you to [Thomas Watson](https://github.com/tpwrules) for this work.
- Created a new ClockSupport module with the loggerClock and epochStart static classes.
- Added support for the Micro Crystal RV-8803-C7 high accuracy, ultra low power Real-Time-Clock Module.
- Added support for multiple 'epoch' types starting at January 1, 1970 (UNIX), January 1, 2000 (Arduino and others), January 5, 1980 (GPST), and January 1, 1900 (NIST time and NTP protocols).
  - This allows you to input the epoch you're using in every single function that deals with a uint32_t or epoch type timestamp.
If no epoch start is given, it is assumed to be UNIX (January 1, 1970).
  - The supported epochs are given in the enum epochStart.
- Storing _buttonPinMode internally.
- Added a single define (`MS_OUTPUT`) to use for all outputs from ModularSensors.
- Added support for sending printouts and debugging to two different serial ports.  This is useful for devices (like SAMD) that use a built in USB serial port which is turned off when the device sleeps.  If `MS_2ND_OUTPUT` is defined, output will go to *both* `MS_2ND_OUTPUT` and to `MS_OUTPUT`.
- Added example code for flashing boards with a neo-pixel in the menu example.
- **NEW SENSOR** Added support for [Geolux HydroCam](https://www.geolux-radars.com/hydrocam)
- **NEW SENSOR** Added support for [ANB Sensors pH Sensors](https://www.anbsensors.com/)
- Added a generic time formatting function.
- **NEW PUBLISHER** Added a new publisher to AWS IoT Core over MQTT
  - A doorway to new possibilities: Unlike every other publisher, the AWS IoT Core publisher supports two-way communication with a settable callback on received messages.
- **NEW PUBLISHER** Added a new publisher to AWS S3 buckets using pre-signed URLs
- Added structure to publish *metadata* to publishers - intended to be used only at startup and once a day at noon.
- Added start-up helper function `makeInitialConnections()` to publish metadata and sync the clock.
- Added function `getVarResolutionAtI(uint8_t)`
- Added support for full CRC checking for SDI-12 sensors.
  - This includes simplistic retries, but does *not* fully implement the SDI-12 protocols triple inner and outer loop retry requirements.
- Added the ability for publishers to make an initial connection to publish metadata.
  - At present this is implemented in the IoT Core publisher - which sends all the metadata for each variable - and the ThingSpeak publisher - which changes channel and field names to match the logger configuration.
- Added a generic `generateFileName(bool include_time, const char* extension, const char* filePrefix)` function to the logger which can be used to assemble a prefix, timestamp, and extension into a new filename.
- Added more options to some of the logger and publisher begin functions.
- Added helper functions to create and delete clients using TinyGSM.
- Added more clear functions for checking and setting sensor status bits.
- Added code spell checking configuration (cspell).

### Removed

- **BREAKING:** Removed the "built-in" GsmClient within the LoggerModem objects.
  - This means you cannot access a client with `&modem.gsmClient`.
  - *Most of the examples used the `&modem.gsmClient` pattern!* This means many people will need to change their code!
- **BREAKING:** Removed the function `setNowUTCEpoch(uint32_t)`.
  - Although public, this was never intended to be used externally.
- **Potentially BREAKING:** Removed support for any functions using the Sodaq "DateTime" class.
- **Potentially BREAKING:** Removed ability to have `PRINTOUT`, `MS_DBG`, and `MS_DEEP_DBG` output going to different serial ports
  - Defines for `STANDARD_SERIAL_OUTPUT`, `DEBUGGING_SERIAL_OUTPUT`, and `DEEP_DEBUGGING_SERIAL_OUTPUT` are all ignored.
Use the single define `MS_OUTPUT` for all outputs.
If `MS_OUTPUT` is not defined, a default will be used (generally Serial or USBSerial).
If you do not want any output, define `MS_SILENT`.
- Removed internal functions for setting file times; replaced with SdFat's dateTimeCallback.
- Added python script to run clang-format on all source files.

### Fixed

- Tested and brought the SAMD51 processor to full functionality.
  - While the SAMD51 was nominally supported previously, it was not functional with the latest boot loaders and core for the SAMD51.
- Updated the ThingSpeak publisher to the current ThingSpeak MQTT protocol. The older protocol was deprecated and non-functional.
  - This requires a user name, password, and client ID for the MQTT connection in addition to the channel number. The MQTT key and channel key are no longer used.
- Ensure that the SDI-12 object is always ended.
- Fixed some timing issues discovered when implementing the status bit checking functions.

### Known Bugs

- **SEVERE** Sensors that require two or more power pins are treated as only requiring the first one within the variableArray and if the second or further power pin is a primary power pin with any other sensor, then the secondary pin will be turned off with the other sensor completes even if the sensor where that pin is secondary is not finished.
  - This is a serious issue for sensors that are both slow and require powered secondary communication adapters or relays - like the Geolux HydroCam or the ANB Sensors pH sensors.
  - *Possible work-arounds*
    - Wire required adapters to the same pin as that providing primary power.
    - Wire required adapters such that they are continuously powered.
    - If you must switch the power to both the sensor and an adapter and either the sensor power or the adapter power are shared with a pin that provides power to any other sensor, call the shared power pin the "sensor" power and the other the "adapter."


***

## [0.36.0]

### Changed

- Applied markdown lint to markdown files
- Bumped TinyGSM dependency
  - Changed data types for modem voltage outputs.
- Switched from Soligen fork of ADS1115 library to the standard Adafruit version.
- Rearranged documentation of examples
- Consistently apply all pre-processor `defined` checks with function-like syntax
- Consistently check for both `__AVR__` and `ARDUINO_ARCH_AVR` in all cases.
  - Not all AVR boards define `__AVR__` within the PlatformIO system, which was confusing the SCons preprocessor.
- Modified implementation of initial short logging intervals.
- Allow non-sleep before returning from testing or logging functions.
- Modified returns of modem variables from uint's to int's for consistency with the latest version of TinyGSM.
- Add pre-processor macro for number of times to attempt to update the clock.

### Added

- Added support for Yosemitech Y513 Blue Green Algae Sensor thanks to @aufdenkampe
- Added support for Alphasense CO2 Sensor thanks to @aufdenkampe
- Added support for Turner Turbidity Plus thanks to @mbarneytu
- Added complete parameter documentation where it was missing.

***

## [0.35.1]

### Changed

- **BREAKING** Switched default clock for SAMD21 from the built-in 32bit RTC to the DS3231.
*This is not be a permanent change.*
- Switched to reusable workflows for CI

### Fixed

Fixed clock configuration for SAMD21

***

## [0.35.0]

### Changed

- **BREAKING** Refactored how the publisher transmit buffer works. This will require adjustment to custom data publishers.
- Update GitHub actions
- Remove date from copyright for easier tracking
- Turn modem off at end of setup, regardless of time
- Clean function to set file timestamp on SD card
- Use equation rather than table for CSQ
- Only polling modem for enabled parameters
- INCREASED THE MAXIMUM NUMBER OF VARIABLES FROM A SINGLE SENSOR and implemented an option to set this by build flag.
  - This will increase memory use for the entire library.
If you are not using the GroPoint sensors which require many variables, I recommend you change this value via the build flag `-D MAX_NUMBER_VARS=8`
- Allow all WiFi modems to first attempt to connect using existing on-modem saved credentials rather than immediately sending new credentials.
- Add further debug printouts to the processor stats

### Added

- Support [GroPoint Profile GPLP-8 Eight-Segment Soil Moisture and Temperature Profiling Probe](https://www.gropoint.com/products/soil-sensors/gropoint-profile)
- Support [Vega Puls 21 Radar](https://www.vega.com/en-us/products/product-catalog/level/radar/vegapuls-21)
- Functions to enable and disable modem metadata polling by bit mask

### Removed

- Removed the (unused) sendOffset parameter from dataPublisherBase.

### Fixed

- Minor bug fixes for XBee Wifi
- Handle no SIM card response from SIM7080G (EnviroDIY LTE Bee)
- Fixed Keller debugging output.
- Fixed file reference for SDFat 2.2.3

### Known Issues

- The modem hardware, firmware, and serial number is only implemented for the Digi XBee WiFi.

## [0.34.1]

### Changed

- Incorporated improvements to the XBee Wifi - from [neilh10](https://github.com/EnviroDIY/ModularSensors/commits?author=neilh10)
  - #347 -WiFi S6B stability - tears down TCP/IP before going to sleep, doesn't automatically poll for meta data

### Added

- Added the ability to enable or disable polling of modem attached variables.
By default, all polling is off, but polling is enabled for a modem sensor when a sensor is created and attached to a modem.
This functionality is inspired from [neilh10](https://github.com/EnviroDIY/ModularSensors/commits?author=neilh10).

### Fixed

- Fixed GitHub actions for pull requests from forks.

## [0.34.0]

### Changed

- **BREAKING** - Removed support for light sleep on Espressif modules.
**This changes the order of the constructor for the ESP32 and ESP8266!**
  - The light sleep mode is non-functional anyway, and confusion over the sleep request pin was putting the board in a position not to sleep at all.
- Minor tweak to clang-format
- Moved all variable initialization to default header values and initializer lists
- Converted all "c-style" casts to static casts
- Switched cases of strcat and strcpy to snprintf
- Specify python version 3.x for actions (used by PlatformIO)

### Added

- Support Campbell RainVUE10 SDI-12 Precipitation Sensor [#416](https://github.com/EnviroDIY/ModularSensors/issues/416)
- Support YosemiTech Y700 Pressor Sensor ([#421](https://github.com/EnviroDIY/ModularSensors/issues/421))

### Removed

### Fixed

- Fixed bug in YosemiTech Y4000 Sonde ([#420](https://github.com/EnviroDIY/ModularSensors/issues/420))
- Fixed non-concurrent data fetch for SDI-12 when *NOT* using debugging.
- Fixed internet connection when in "testing mode"
- Allow a non-zero wait time for SDI-12 sensors
- Fixed outdated GitHub actions

***

## [0.33.4]

### Fixed

- Increased warm-up and measurement time for Campbell ClariVUE-10 to work with the latest version of the sensor.

***

## [0.33.3]

### Fixed

- Increased measurement time for Hydros21 to work with the latest version of the sensor.

***

## [0.33.2]

### Fixed

- Fixed script to install and zip libraries for a release

***

## [0.33.1] - 2022-04-11

### Changed

### Added

- Added a typedef and header for the ESP32
  - This is just another name to the ESP8266 class to help any who don't know they're identical for our purposes.
- **Example:**  Created a new DRWI wifi example for workshop.

### Removed

### Fixed

***

## [0.33.0] - 2022-04-01

### Changed

- **Breaking:** Renamed the static `markedEpochTime` variable to `markedLocalEpochTime`.
  - This was sometimes used in "complex" loops.  Code utilizing it will have to be changed.
  - This is part of the effort to clarify where localized and UTC time are being used.
We recommend a logger's real time clock always be set in UTC and then localized for printing and storing data.
- **Breaking:** Renamed the function `setNowEpoch(uint32_t)` to `setNowUTCEpoch(uint32_t)`.
  - Although public, this was never intended to be used externally.
- **Breaking:** Renamed the YosemiTech Y550 COD sensor as Y551. See below.
- **Potentially Breaking:** Changed the default "button" interrupt pin mode from `INPUT_PULLUP` to `INPUT` and created optional arguments to the `setTestingModePin` and `setLoggerPins` functions to specify the pin mode and pull-up resistor state.
  - `INPUT` is the proper mode for the Mayfly.
The Mayfly has an external pull *down* on the button pin with the button being active high.
This means having the pull-up resistors on negates the button signal.
The pin mode had been set as `INPUT_PULLUP` for the button, backwards for the Mayfly, since  [July of 2017](https://github.com/EnviroDIY/ModularSensors/commit/6bafb0fd149589f71ca6f46b761fe72b1f9523a6).
By some electrical luck, with the 0.x versions of the Mayfly, the external pull-down on the button pin was strong enough to out-weigh the incorrectly activated pull-up resistors and an interrupt was still registered when the button was pressed.
With a different pull-down resistor on the Mayfly 1.x, the button no longer registers with the pull-up resistors active.
So, for most of our users with Mayflies, this will be a ***fix***.
But for anyone using a different board/processor/button configuration that depended on the processor pull-up resistors, this will be a breaking change and they will need to specify the button mode in the `setTestingModePin` or `setLoggerPins` function to return to the previous behavior.
- Added a longer warm up time and removed some of the modem set-up to work with the ESP-IDF AT firmware versions >2.0
- Made sure that all example clock synchronization happens at noon instead of midnight.
- **Renamed Classes:** Renamed several classes for internal consistency.
These are *not* breaking changes at this time; the old class names are still usable.
  - Rename class `MPL115A2` to `FreescaleMPL115A2`
  - Rename class `MPL115A2_Pressure` to `FreescaleMPL115A2_Pressure`
  - Rename class `MPL115A2_Temp` to `FreescaleMPL115A2_Temp`
  - Rename class `TIINA219_Volt` to `TIINA219_Voltage`
  - Rename class `PaleoTerraRedox_Volt` to `PaleoTerraRedox_Voltage`
  - Rename class `ExternalVoltage` to `TIADS1x15`
  - Rename class `ExternalVoltage_Volt` to `TIADS1x15_Voltage`
- **Documentation:** Migrated to latest version of Doxygen (1.9.3).

### Added

- **Sensor** Added support for the [YosemiTech Y551 COD Sensor](http://en.yosemitech.com/aspcms/product/2020-5-8/94.html), which makes a UV254 light absorption and translates it to estimates of Chemical Oxygen Demand (COD) (or Total Organic Carbon (TOC)) and Turbidity.
  - NOTE that this upgrade removes the earlier Y550 from the library, as it was never tested and is no longer available form YosemiTech. If anyone has a Y550 sensor, the Y551 commands should work. Let us know if they don't.
- **Sensor** Added support for the [YosemiTech Y560 Ammonium Probe](http://en.yosemitech.com/aspcms/product/2020-4-23/61.html), which is a mini sonde for three Ion Selective Electrode (ISE) sensors (pH, NH4+, K+) that together are used to provide a corrected estimate of total ammonium nitrogen (NH4_N) in mg/L.
  - NOTE that this release only includes outputs for NH4_N, pH, and temperature. A future release will also include estimates of potassium (K) and raw potential values from each of the electrodes.
- **Sensor** Added support for the SDI-12 In-Situ [Level TROLL 400, 500 & 700 Data Loggers](https://in-situ.com/pub/media/support/documents/LevelTROLL_SS.pdf)
- **Sensor** Added support for the Sensirion SHT40 relative humidity and temperature sensor
- **Sensor** Added support for the Everlight ALS-PT19 ambient light sensor
- **Sensor** Added support for the Bosch SensorTec BMP388 and BMP390 pressure sensors

### Removed

### Fixed

- Fixed memory leak for AOSong AM2315 thanks to @neilh10

***

## [0.32.2] - 2021-11-23

### Changed

- Restructured SDI-12 slightly to break out the start measurement functionality into a new function.
- Modified Decagon 5-TM and Meter Teros 11 to use the SDI-12 get results function rather than addSingleMeasurementResult.
This will allow both sensors to honor the 'non-concurrent' flag, if that is set.
Previously, they would not have.
- **Documentation:** Migrated to latest version of Doxygen (1.9.2).
  This required some changes with m.css to properly ignore the doxyfile.xml the current version generates.

### Added

- **Board:** Adds 1.0 and 1.1 as valid version numbers for the Mayfly. Does not yet support any new features of those boards.
- Add a new parameter (internal variable) to the sensor base class for the number of internally calculated variables.
These are used for values that we would always calculate for a sensor and depend only on the raw results of that single sensor.
This is separate from any calculated variables that are created on-the-fly and depend on multiple other sensors.
In many cases, this is 0 and in most of the other cases the value is informational only.
For the SDI-12 sensors, I'm actually using this to make sure I'm getting the number of values expected.
- **Sensor:** Added support for [v0.2.0](https://github.com/EnviroDIY/TippingBucketRainCounter/releases) of the [EnviroDIY/TippingBucketRainCounter](https://github.com/EnviroDIY/TippingBucketRainCounter) device firmware, which added capability to count rotations on a reed-switch anemometer and fixed a critical bug that failed to count high rainfall rates.
  - For details, see: <https://github.com/EnviroDIY/TippingBucketRainCounter/releases/tag/v0.2.0>

***

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

- **Breaking:** Temporarily ***REMOVED*** support for AOSong DHT sensor.
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

- Modified `YosemitechY533.h` and examples to work with updated ORP `getValues()` function in <https://github.com/EnviroDIY/YosemitechModbus> released with v0.2.5.

***

## [0.29.0] - 2021-05-19

Create a ModularSensors.h

### Changed

- Modified examples to use Hydros 21

### Added

- Created a ModularSensors.h file to include.
This makes it much easier to install and use the library from the Arduino CLI.
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
Moving forward, the two can be used interchangeably.
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

- Fixes an ***EGREGIOUS*** error in the SDI-12 code causing the code to lock up if debugging was off (but always work with it on)
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
- Add enormous amounts of documentation
- Improved explanations and added walkthrough of menu a la carte example
- Added example calculating specific conductance where applicable
- For SDI-12 sensors, added calls to additional data commands (D1-D9) if full number of expected results are not returned by D0.
- For SDI-12 sensors, added ability to disable concurrent measurements using the build flag `MS_SDI12_NON_CONCURRENT`.
  - NOTE:  Setting the build flag disables concurrent measurements for *ALL* SDI-12 sensors!

### Added

- **Sensor:** PaleoTerra Redox sensors
- **Sensor:** Northern Widget Tally Counters
- **Sensor:** simple analog electrical conductance sensors
- **Sensor:** In-Situ RDO PRO-X rugged dissolved oxygen sensors
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

- **Documentation:** Automated code documentation using [Doxygen](https://www.doxygen.nl/index.html), now available at <https://envirodiy.github.io/ModularSensors/>

### Fixed

- Fixed issue where the Digi XBee LTE-M modem did not wake during normal logging mode to transmit data to the publisher. See <https://github.com/EnviroDIY/ModularSensors/pull/309#commitcomment-39786167>

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

More aggressive attempts to set clock

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
- Added support for Meter Teros 11 soil moisture and temperature sensor
- Implemented a function to verify that UUIDs are at least correctly formed and unique - though it does not verify that they are valid.
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

- All code must be updated to the current format for modems and time zones to work with this version of the library.

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

- Fixed bug introduced in 0.21.0 preventing writing to SD card - file must be closed (not sync'ed)

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

- **Sensor:** Added full support for all Atlas Scientific EZO circuits and sensors that support I2C
  - CO2
  - DO (dissolved oxygen)
  - EC (conductivity)
  - ORP (oxidation/reduction potential)
  - pH
  - RTD (temperature
- Created empty constructors for the logger, publisher, variable array, and variable classes and all of their subclasses.  For all classes created a corresponding "begin" function to set internal class object values.
  - See note for more details: <https://github.com/EnviroDIY/ModularSensors/commit/b1a619ed74bc790743bce35b3a4e78a2d2237b22>
  - The order of input arguments for all variable objects has changed.  For variable subclasses (ie, variables from sensors), there is no change to the user.  ****For calculated variable objects, all code must be updated!**** Please check the structure in the examples!  Older code will compile without error but the variable metadata fields will be incorrectly populated.
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

- Fixes bug in sending data to [Monitor My Watershed](https://monitormywatershed.org).

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
- A huge number of other changes, with most of them are documented here:  <https://github.com/EnviroDIY/ModularSensors/pull/173>

***

## [0.12.2] - 2018-09-25

Calculated variables and bug fixes

NOTE: This **THIS RELEASE DESTROYS BACKWARDS COMPATIBILITY!!** All `.ino` files will need to be updated to follow the updated examples.

### Added

- Implemented real calculated variables, from PR #153, closing issue #127 (Create generic calculated variables).

### Fixed

- Fixes issue with MaxSonar giving weird readings, due to buffer not being cleared between readings, as described in <https://www.envirodiy.org/topic/minor-glitch-reading-maxbotix-mb7389-with-mayfly/>

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

- Tweaked code generating csv's and json outputs to allow a modified csv or json to be written to the SD card or posted to data.EnviroDIY.org

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
  By default, it will *not* ask the parent sensor to update, but only return the last value received or -9999 if a value has never been received.

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

Improved setup functions

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

[Unreleased]: https://github.com/EnviroDIY/ModularSensors/compare/v0.37.0...HEAD
[0.37.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.37.0
[0.36.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.36.0
[0.35.1]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.35.1
[0.35.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.35.0
[0.34.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.34.0
[0.33.4]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.33.4
[0.33.3]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.33.3
[0.33.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.33.2
[0.33.1]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.33.1
[0.33.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.33.0
[0.32.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.32.2
[0.32.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.32.0
[0.31.2]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.31.2
[0.31.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.31.0
[0.30.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.30.0
[0.29.1]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.29.1
[0.29.0]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.29.0
[0.28.5]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.5
[0.28.4]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.4
[0.28.3]: https://github.com/EnviroDIY/ModularSensors/releases/tag/v0.28.3
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

<!--! @tableofcontents{HTML:1} -->

<!--! @m_footernavigation -->
