# Getting Started<!--! {#page_getting_started} -->

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [Getting Started](#getting-started)
  - [IDE and Driver Installation](#ide-and-driver-installation)
  - [Library Installation](#library-installation)
  - [Setting the Clock](#setting-the-clock)
  - [Writing Your Logger Program](#writing-your-logger-program)
  - [Modifying the Examples](#modifying-the-examples)
  - [Deploying your Station](#deploying-your-station)

<!--! @endif -->

Note: These instructions pertain almost entirely to using this specific library.
There is an [extensive manual](https://www.envirodiy.org/mayfly-sensor-station-manual/) , set of [appendices](https://www.envirodiy.org/mayfly-sensor-station-manual/appendices/), and [video tutorials](https://www.envirodiy.org/videos/) for planning, installing, and maintaining a stream-side sensor station on the EnviroDIY website.

## IDE and Driver Installation<!--! {#page_getting_started_ide} -->

To interface with your board and send it programs, you'll need to install drivers and an [IDE](https://en.wikipedia.org/wiki/Integrated_development_environment) or editor with a compiler on your computer.

Many Arduino style boards, including the EnviroDIY Mayfly, interface with the computer using an FTDI chip as a virtual com port.
The drivers and installation instructions for them are available on the FTDI website:  <https://www.ftdichip.com/Drivers/VCP.htm>.
**You must be an administrator on your computer to install drivers!**  If you've used an Arduino before or installed the Arduino IDE, it's likely that these drivers are already on your computer.

Adafruit has a nice grouping of drivers for other boards and installation instructions here:  <https://learn.adafruit.com/adafruit-arduino-ide-setup/windows-driver-installation>.

After the drivers, you should install the IDE.
Even if you've used the official Arduino IDE before, I _very, **very** strongly_ suggest installing and using [PlatformIO](https://platformio.org/) on [Visual Studio Code](https://code.visualstudio.com/).

Visual Studio code can be downloaded from <https://code.visualstudio.com/> and installed following the directions on that website.
You do not need to be an administrator to install it.
Once VSCode has been installed, open it.
On the picture menu on the left click the extensions button.
(It looks kind of like a box floating off a group of boxes.)
Search for "PlatformIO" and install it.
You may need to restart or reload VSCode after the installation finishes.

Once you have PlatformIO installed, you can access things like the serial port monitor from the PlatformIO menu.
To access the menu, click the button that looks like a bug face on the left in VS code.
You create new "projects" in PlatformIO from the PlatformIO home page.
That home page can be accessed from the PlatformIO menu.

## Library Installation<!--! {#page_getting_started_libraries} -->

Before you can use this library, you'll need to install it and all of its [dependencies](https://github.com/EnviroDIY/ModularSensors/wiki/Library-Dependencies) so your compiler in the IDE can find them.
Because this library has a large number of dependencies, I, again, _very, **very** strongly_ suggest using [PlatformIO](https://platformio.org/).
If you use PlatformIO, the library will automatically be installed when you list it in your dependencies in your project's platformio.ini file.
If you really must use the Arduino IDE, this library and all is dependencies can be downloaded in one large zip file [here](https://github.com/EnviroDIY/Libraries/blob/master/libraries.zip?raw=true).

## Setting the Clock<!--! {#page_getting_started_clock} -->

Most of this library's functionality depends on having a working DS3231 real time clock attached to your Arduino, so the first thing you need to do is get the time right.
For the rank beginners out there; I feel your pain.
When I first started with an Arduino, the step of setting the clock took me _**three days**_ to figure out.
(Yes, three days.
Yes, I was that lost!
My background is _not_ in programming or engineering.)
After that experience, nearly the first program I decided was needed was an easier way to synchronize the clock.
In fact, almost all of the example programs in this library that use a modern to connect to the internet will attempt to check the clock at start up.
But, for safety, I suggest you set the clock separately.

NOTE:  These steps are only for AVR boards, for those of you using a SAMD board, the on-board processor RTC is used instead of the DS3231.

- Attach the DS3231 to your main board - they'll talk over I2C.
If you're using a Mayfly, the DS3231 is built into the board and you don't need to do anything.
- Put a coin battery in the supplemental power slot for the DS3231 (or you'll lose the time as soon as you unplug).
The Mayfly has this battery shot right next to the clock chip.
Every other DS3231 breakout I've seen has a similar way to power the chip.
On the Mayfly, the (+) side of the battery (with the words on it) goes up.
- Create a new PlatformIO project from the PlatformIO menu or home page.
Pick whatever board you'll be working with from the drop down.
For a new project, it's easiest to let PlatformIO set everything up in a new folder.
- Once PlatformIO sets up the new project, find and open the newly created platformio.ini file.
It should be a short file with one `[platformio]` section and one `[env]` section for the board you selected earlier.
  - In the `[platformio]` section add this line:  `src_dir = .pio/libdeps/mayfly/EnviroDIY_DS3231/examples/PCsync`
  - In the `[env]` section add this line:  `lib_deps = EnviroDIY_DS3231`
- Upload to your board.
You shouldn't have to open or modify the program at all.
- Download and run this tiny clock-sync program:  <https://github.com/EnviroDIY/Sodaq_DS3231/blob/master/examples/PCsync/PCsync.exe?raw=true>
- Your clock should be set!

## Writing Your Logger Program<!--! {#page_getting_started_program} -->

The set-up in for your logger program PlatformIO is pretty simple:

- Create another new PlatformIO project from the PlatformIO menu or home page.
Pick whatever board you'll be working with.
Again, it's easiest to let PlatformIO set everything up in a new folder.
- Find and open the newly created platformio.ini file in your directory.
In the `[env]` section add these lines:
  - It is important that your configuration has the lib_ldf_mode and build flags set as show below.
Without this, the library won't compile.

```ini
lib_deps = EnviroDIY_ModularSensors
lib_ldf_mode = deep+
build_flags =
    -DSDI12_EXTERNAL_PCINT
    -DNEOSWSERIAL_EXTERNAL_PCINT
    -DMQTT_MAX_PACKET_SIZE=240
    -DTINY_GSM_RX_BUFFER=64
    -DTINY_GSM_YIELD_MS=2
```

- Download the "ino" file for whatever example you think will be most similar to what you'll be doing.
Put the ino into the src directory of your project.
  - Delete main.cpp in that folder.
- Do a test build before changing the example just to make sure it compiles.
Note: before compiling the first time, PlatformIO has to download the library and is dependencies so be patient.
The download only happens once.
- If the build succeeds, you're ready to move on.

## Modifying the Examples<!--! {#page_getting_started_examples} -->

There are a number of examples in the [examples](https://github.com/EnviroDIY/ModularSensors/tree/master/examples) folder for different logger functionalities.
If you are unsure which to use, the "menu_a_la_carte" example has code in it for every possible sensor and modem.
You can start with it and delete as necessary to cut down to only what you will be using.
The pin numbers in all of the example code are simply the ones I've used as I've attached sensors to the Mayfly on my desk for testing.
There's no reason you need to use the same pin assignments.
Do make sure you create or reference all of the variables for your sensors in your variable array.

The examples currently available are:

- [menu_a_la_carte](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/menu_a_la_carte)
  - This shows most of the functions of the library at once.
It has code in it for every possible sensor and modem and for both AVR and SAMD boards.
It is also over 1500 lines long.
- [single_sensor](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/single_sensor)
  - This shows making use of the unified set of commands to print data from a MaxBotix ultrasonic range finder to the serial port.
It also shows creating a calculated variable which is the water depth.
- [simple_logging](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/simple_logging)
  - This shows how to log data a simple sample count and battery voltage to a SD card.
- [logging_to_ThingSpeak](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/logging_to_ThingSpeak)
  - This uses an ESP8266 to send data to ThingSpeak.
It also includes a Meter Hydros 21 (formerly know as a Decagon CTD) and a Campbell OBS3+.
- [baro_rho_correction](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/baro_rho_correction)
  - This example demonstrates how to work with calculated variables and calculates water depth by correcting the total pressure measured by a Measurement Specialties MS5803 with the atmospheric pressure measured by a Bosch BME280 environmental sensor and the temperature measured by a Maxim DS18 temperature probe.
- [double_logger](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/double_logger)
  - This is a more complicated example using two different logger instances to log data at two different intervals, in this case, an AM3215 logging every minute, while checking the battery voltage only every 5 minutes.
This showcases both how to use two different logging instances and how to use some of the functions to set up your own logging loop rather than using the logData() function.
- [data_saving](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/)
  - This is another double logger example, but in this case, both loggers are going at the same interval and the only difference between the loggers is the list of variables.
There are two sets of variables, all coming from Yosemitech sensors.
Because each sensor outputs temperature and we don't want to waste cellular data sending out multiple nearly identical temperature values, we have one logger that logs every possible variable result to the SD card and another logger that sends only unique results to Monitor My Watershed.
This example also shows how to stop power draw from an RS485 adapter with automatic flow detection.

## Deploying your Station<!--! {#page_getting_started_deploying} -->

To start getting data from the wild, you'll need some [stuff](https://github.com/EnviroDIY/ModularSensors/wiki/Physical-Dependencies) to power your logger and keep it safe from the elements.
There are [video instructions](https://www.envirodiy.org/videos/) on the EnviroDIY website showing how to prepare and install a typical steam-side logger box and seniors.

<!--! @section page_getting_started_other Other Helpful Pages to Get Started -->

<!--! @subpage page_library_dependencies -->

<!--! @subpage page_physical_dependencies -->

<!--! @subpage page_library_terminology -->
