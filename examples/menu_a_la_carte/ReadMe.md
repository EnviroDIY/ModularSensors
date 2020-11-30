[//]: # ( @page example_menu A la carte Example )
# Example showing all possible functionality

This shows most of the functionality of the library at once.
It has code in it for every possible sensor and modem and for both AVR and SAMD boards.
This example should *never* be used directly, but rather taken as a template and all parts that do not apply cut out.

Any line containing the word *MS_BUILD_TESTING* is to help ensure the library builds correctly in all environments.
Those lines should be removed when using the example on your own board.

_______

[//]: # ( @section example_menu_walk Walking Through the Code )
# Walking Through the Code

_NOTE:  The code snippets in this walkthrough will not appear on GitHub._

WARNING:  This example is long.
This walk-through is really, really long.
Make use of the table of contents to skip to the parts you need.
___

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Example showing all possible functionality](#example-showing-all-possible-functionality)
- [Walking Through the Code](#walking-through-the-code)
  - [Defines and Includes](#defines-and-includes)
    - [Defines for the Arduino IDE](#defines-for-the-arduino-ide)
    - [Library Includes](#library-includes)
  - [Logger Settings](#logger-settings)
    - [Extra Serial Ports](#extra-serial-ports)
      - [AVR Boards](#avr-boards)
        - [AltSoftSerial](#altsoftserial)
        - [NeoSWSerial](#neoswserial)
        - [SoftwareSerial with External Interrupts](#softwareserial-with-external-interrupts)
        - [Software I2C/Wire](#software-i2cwire)
      - [SAMD Boards](#samd-boards)
    - [Logging Options](#logging-options)
  - [Wifi/Cellular Modem Options](#wificellular-modem-options)
    - [Digi XBee Cellular - Transparent Mode](#digi-xbee-cellular---transparent-mode)
    - [Digi XBee3 LTE-M - Bypass Mode](#digi-xbee3-lte-m---bypass-mode)
    - [Digi XBee 3G - Bypass Mode](#digi-xbee-3g---bypass-mode)
    - [Digi XBee S6B Wifi](#digi-xbee-s6b-wifi)
    - [Espressif ESP8266](#espressif-esp8266)
    - [Quectel BG96](#quectel-bg96)
    - [Sequans Monarch](#sequans-monarch)
    - [SIMCom SIM800](#simcom-sim800)
    - [SIMCom SIM7000](#simcom-sim7000)
    - [Sodaq GPRSBee](#sodaq-gprsbee)
    - [u-blox SARA R410M](#u-blox-sara-r410m)
    - [u-blox SARA U201](#u-blox-sara-u201)
    - [Modem Measured Variables](#modem-measured-variables)
  - [Sensors and Measured Variables](#sensors-and-measured-variables)
    - [The processor as a sensor](#the-processor-as-a-sensor)
    - [Maxim DS3231 RTC as a sensor](#maxim-ds3231-rtc-as-a-sensor)
    - [AOSong AM2315](#aosong-am2315)
    - [AOSong DHT](#aosong-dht)
    - [Apogee SQ-212 Quantum Light Sensor](#apogee-sq-212-quantum-light-sensor)
    - [Atlas Scientific EZO Circuits](#atlas-scientific-ezo-circuits)
      - [Atlas Scientific EZO-CO2 Embedded NDIR Carbon Dioxide Sensor](#atlas-scientific-ezo-co2-embedded-ndir-carbon-dioxide-sensor)
      - [Atlas Scientific EZO-DO Dissolved Oxygen Sensor](#atlas-scientific-ezo-do-dissolved-oxygen-sensor)
      - [Atlas Scientific EZO-ORP Oxidation/Reduction Potential Sensor](#atlas-scientific-ezo-orp-oxidationreduction-potential-sensor)
      - [Atlas Scientific EZO-pH Sensor](#atlas-scientific-ezo-ph-sensor)
      - [Atlas Scientific EZO-RTD Temperature Sensor](#atlas-scientific-ezo-rtd-temperature-sensor)
      - [Atlas Scientific EZO-EC Conductivity Sensor](#atlas-scientific-ezo-ec-conductivity-sensor)
    - [Bosch BME280 Environmental Sensor](#bosch-bme280-environmental-sensor)
    - [Campbell OBS3+ Analog Turbidity Sensor](#campbell-obs3-analog-turbidity-sensor)
    - [Decagon ES2 Conductivity and Temperature Sensor](#decagon-es2-conductivity-and-temperature-sensor)
    - [External Voltage via TI ADS1x15](#external-voltage-via-ti-ads1x15)
    - [Freescale Semiconductor MPL115A2 Miniature I2C Digital Barometer](#freescale-semiconductor-mpl115a2-miniature-i2c-digital-barometer)
    - [Keller RS485/Modbus Water Level Sensors](#keller-rs485modbus-water-level-sensors)
      - [Keller Nanolevel Level Transmitter](#keller-nanolevel-level-transmitter)
      - [Keller Acculevel High Accuracy Submersible Level Transmitter](#keller-acculevel-high-accuracy-submersible-level-transmitter)
    - [Maxbotix HRXL Ultrasonic Range Finder](#maxbotix-hrxl-ultrasonic-range-finder)
    - [Maxim DS18 One Wire Temperature Sensor](#maxim-ds18-one-wire-temperature-sensor)
    - [Measurement Specialties MS5803-14BA Pressure Sensor](#measurement-specialties-ms5803-14ba-pressure-sensor)
    - [Meter SDI-12 Sensors](#meter-sdi-12-sensors)
      - [Meter ECH2O Soil Moisture Sensor](#meter-ech2o-soil-moisture-sensor)
      - [Meter Hydros 21 Conductivity, Temperature, and Depth Sensor](#meter-hydros-21-conductivity-temperature-and-depth-sensor)
      - [Meter Teros 11 Soil Moisture Sensor](#meter-teros-11-soil-moisture-sensor)
    - [PaleoTerra Redox Sensors](#paleoterra-redox-sensors)
    - [Trinket-Based Tipping Bucket Rain Gauge](#trinket-based-tipping-bucket-rain-gauge)
    - [Northern Widget Tally Event Counter](#northern-widget-tally-event-counter)
    - [TI INA219 High Side Current Sensor](#ti-ina219-high-side-current-sensor)
    - [Analog Electrical Conductivity using the Processor's Analog Pins](#analog-electrical-conductivity-using-the-processors-analog-pins)
    - [Yosemitech RS485/Modbus Environmental Sensors](#yosemitech-rs485modbus-environmental-sensors)
      - [Yosemitech Y504 Dissolved Oxygen Sensor](#yosemitech-y504-dissolved-oxygen-sensor)
      - [Yosemitech Y510 Yosemitech Y510 Turbidity Sensor](#yosemitech-y510-yosemitech-y510-turbidity-sensor)
      - [Yosemitech Y511 Yosemitech Y511 Turbidity Sensor with Wiper](#yosemitech-y511-yosemitech-y511-turbidity-sensor-with-wiper)
      - [Yosemitech Y514 Yosemitech Y514 Chlorophyll Sensor](#yosemitech-y514-yosemitech-y514-chlorophyll-sensor)
      - [Yosemitech Y520 Yosemitech Y520 Conductivity Sensor](#yosemitech-y520-yosemitech-y520-conductivity-sensor)
      - [Yosemitech Y532 Yosemitech Y532 pH Sensor](#yosemitech-y532-yosemitech-y532-ph-sensor)
      - [Yosemitech Y533 Yosemitech Y533 Oxidation Reduction Potential (ORP) Sensor](#yosemitech-y533-yosemitech-y533-oxidation-reduction-potential-orp-sensor)
      - [Yosemitech Y550 Yosemitech Y550 Carbon Oxygen Demand (COD) Sensor with Wiper](#yosemitech-y550-yosemitech-y550-carbon-oxygen-demand-cod-sensor-with-wiper)
      - [Yosemitech Y4000 Yosemitech Y4000 Multi-Parameter Sonde](#yosemitech-y4000-yosemitech-y4000-multi-parameter-sonde)
    - [Zebra Tech D-Opto Dissolved Oxygen Sensor](#zebra-tech-d-opto-dissolved-oxygen-sensor)
  - [Calculated Variables](#calculated-variables)
  - [Creating the array, logger, publishers](#creating-the-array-logger-publishers)
    - [The variable array](#the-variable-array)
    - [The Logger Object](#the-logger-object)
    - [Data Publishers](#data-publishers)
      - [Monitor My Watershed](#monitor-my-watershed)
      - [DreamHost](#dreamhost)
      - [ThingSpeak](#thingspeak)
  - [Extra Working Functions](#extra-working-functions)
  - [Arduino Setup Function](#arduino-setup-function)
    - [Starting the Function](#starting-the-function)
    - [Wait for USB](#wait-for-usb)
    - [Printing a Hello](#printing-a-hello)
    - [Serial Interrupts](#serial-interrupts)
    - [Serial Begin](#serial-begin)
    - [SAMD Pin Peripherals](#samd-pin-peripherals)
    - [Flash the LEDs](#flash-the-leds)
    - [Begin the Logger](#begin-the-logger)
    - [Setup the Sensors](#setup-the-sensors)
    - [Custom Modem Setup](#custom-modem-setup)
      - [ESP8266 Baud Rate](#esp8266-baud-rate)
      - [Skywire Pin Inversions](#skywire-pin-inversions)
      - [XBee Cellular Carrier](#xbee-cellular-carrier)
      - [SARA R4 Cellular Carrier](#sara-r4-cellular-carrier)
    - [Sync the Real Time Clock](#sync-the-real-time-clock)
    - [Setup File on the SD card](#setup-file-on-the-sd-card)
    - [Sleep until the First Data Collection Time](#sleep-until-the-first-data-collection-time)
    - [Setup Complete](#setup-complete)
  - [Arduino Loop Function](#arduino-loop-function)
    - [A Typical Loop](#a-typical-loop)
    - [A Complex Loop](#a-complex-loop)
  - [PlatformIO Configuration](#platformio-configuration)
  - [The Complete Code](#the-complete-code)

[//]: # ( End GitHub Only )


[//]: # ( @section menu_defines-and-includes Defines and Includes )
## Defines and Includes

[//]: # ( @subsection menu_defines Defines for the Arduino IDE )
### Defines for the Arduino IDE
The top few lines of the examples set defines of buffer sizes and yields needed for the Arduino IDE.
That IDE read any defines within the top few lines and applies them as build flags for the processor.
This is _not_ standard behavior for C++ (which is what Arduino code really is) - this is a unique aspect of the Arduino IDE.

[//]: # ( @menusnip{defines} )

If you are using PlatformIO, you should instead set these as global build flags in your platformio.ini.
This is standard behaviour for C++.

```ini
build_flags =
    -DSDI12_EXTERNAL_PCINT
    -DNEOSWSERIAL_EXTERNAL_PCINT
    -DMQTT_MAX_PACKET_SIZE=240
    -DTINY_GSM_RX_BUFFER=64
    -DTINY_GSM_YIELD_MS=2
```
___

[//]: # ( @subsection menu_includes Library Includes )
### Library Includes

Next, include the libraries needed for every program using ModularSensors.

[//]: # ( @menusnip{includes} )
___

[//]: # ( @section menu_logger_and_modem_settings Logger Settings )
## Logger Settings

[//]: # ( @subsection menu_serial_ports Extra Serial Ports )
### Extra Serial Ports

This section of the example has all the code to create and link to serial ports for both AVR and SAMD based boards.
The EnviroDIY Mayfly, the Arduino Mega, UNO, and Leonardo are all AVR boards.
The Arduino Zero, the M0 and the Sodaq Autonomo are all SAMD boards.

Many different sensors communicate using some sort of serial or transistor-transistor-logic (TTL) protocol.
Among these are any sensors using RS232, RS485, RS422.
Generally each serial variant (or sometimes each sensor) needs a dedicated serial "port" - its own connection to the processor.
Most processors have built in dedicated wires for serial communication - "Hardware" serial.
See the page on [Arduino streams](@ref page_arduino_streams) for much more detail about serial connections with Arduino processors.
_______

[//]: # ( @subsubsection menu_avr_serial_ports AVR Boards )
#### AVR Boards

Most Arduino AVR style boards have very few (ie, one, or none) dedicated serial ports _available_ after counting out the programming serial port.
So to connect anything else, we need to try to emulate the processor serial functionality with a software library.
This example shows three possible libraries that can be used to emulate a serial port on an AVR board.


[//]: # ( @paragraph menu_altsoftseral AltSoftSerial )
##### AltSoftSerial

[AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial) by Paul Stoffregen is the most accurate software serial port for AVR boards.
AltSoftSerial can only be used on one set of pins on each board so only one AltSoftSerial port can be used.
Not all AVR boards are supported by AltSoftSerial.
See the [processor compatibility](@ref page_processor_compatibility) page for more information on which pins are used on supported boards.

[//]: # ( @menusnip{altsoftserial} )


[//]: # ( @paragraph menu_neoswserial NeoSWSerial )
##### NeoSWSerial

[NeoSWSerial](https://github.com/SRGDamia1/NeoSWSerial) is the best software serial that can be used on any pin supporting interrupts.
You can use as many instances of NeoSWSerial as you want.
Each instance requires two pins, one for data in and another for data out.
If you only want to use the serial line for incoming or outgoing data, set the other pin to -1.
Not all AVR boards are supported by NeoSWSerial.

[//]: # ( @menusnip{neoswserial} )

When using NeoSWSerial we will also have to actually set the data receiving (Rx) pin modes for interrupt in the [setup function](@ref menu_setup_serial_interrupts).


[//]: # ( @paragraph menu_softwareseraial SoftwareSerial with External Interrupts )
##### SoftwareSerial with External Interrupts

The "standard" software serial library uses interrupts that conflict with several other libraries used within this program.
I've created a [version of software serial that has been stripped of interrupts](https://github.com/EnviroDIY/SoftwareSerial_ExtInts) but it is still far from ideal.
This should be used only use if necessary.
It is not a very accurate serial port!

Accepting its poor quality, you can use as many instances of SoftwareSerial as you want.
Each instance requires two pins, one for data in and another for data out.
If you only want to use the serial line for incoming or outgoing data, set the other pin to -1.

[//]: # ( @menusnip{softwareserial} )

When using SoftwareSerial with External Interrupts we will also have to actually set the data receiving (Rx) pin modes for interrupt in the [setup function](@ref menu_setup_serial_interrupts).


[//]: # ( @paragraph menu_softwarewire Software I2C/Wire )
##### Software I2C/Wire

This creates a software I2C (wire) instance that can be shared between multiple sensors.
Only Testato's [SoftwareWire](https://github.com/Testato/SoftwareWire) library is supported.

[//]: # ( @menusnip{softwarewire} )

---

[//]: # ( @subsubsection menu_samd_serial_ports SAMD Boards )
#### SAMD Boards

The SAMD21 supports up to 6 _hardware_ serial ports, which is _awesome_.
But, the Arduino core doesn't make use of all of them, so we have to assign them ourselves.

This section of code assigns SERCOM's 1 and 2 to act as Serial2 and Serial3 on pins 10/11 and 5/2 respectively.
These pin selections are based on the Adafruit Feather M0.

[//]: # ( @menusnip{serial_ports_SAMD} )

In addition to creating the extra SERCOM ports here, the pins must be set up as the proper pin peripherals after the serial ports are begun.
This is shown in the [SAMD Pin Peripherals section](@ref menu_setup_pin_periph) of the setup function.


NOTE:  The SAMD51 board has an amazing _8_ available SERCOM's, but I do not have any exmple code for using them.

---

[//]: # ( @subsection menu_logger_opts Logging Options )
### Logging Options

Here we set options for the logging and dataLogger object.
This includes setting the time zone (daylight savings time is **NOT** applied) and setting all of the input and output pins related to the logger.

[//]: # ( @menusnip{logging_options} )
___


[//]: # ( @section menu_modem_settings Wifi/Cellular Modem Options )
## Wifi/Cellular Modem Options

This modem section is very lengthy because it contains the code with the constructor for every possible supported modem module.
Do _NOT_ try to use more than one modem at a time - it will _NOT_ work.

To create any of the modems, we follow a similar pattern:

First, we'll create a pointer to the serial port (Arduino Stream object) that we'll use for communication between the modem and the MCU.
We also assign the baud rate to a variable here.
There is a table of @ref modem_notes_bauds on the @ref page_modem_notes page.
The baud rate of any of the modules can be changed using AT commands or the `modem.gsmModem.setBaud(uint32_t baud)` function.

Next, we'll assign all the pin numbers for all the other pins connected between the modem and the MCU.
Pins that do not apply should be set as -1.
There is a table of general @ref modem_notes_sleep and @ref modem_notes_mayfly_pins on the @ref page_modem_notes page.

All the modems also need some sort of network credentials for internet access.
For WiFi modems, you need the network name and password (assuming WPA2).
For cellular models, you will need the APN assigned to you by the carrier you bought your SIM card from.


[//]: # ( @subsection menu_xbee_cell_trans Digi XBee Cellular )
### Digi XBee Cellular - Transparent Mode

This is the code to use for _any_ of Digi's cellular XBee or XBee3 modules.
All of them can be implented as a DigiXBeeCellularTransparent object - a subclass of DigiXBee and loggerModem.
To create a DigiXBeeCellularTransparent object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the status pin,
- whether the status pin is the true status pin (`ON/SLEEP_N/DIO9`) or the `CTS_N/DIO7` pin,
- the MCU pin connected to the `RESET_N`pin,
- the `DTR_N/SLEEP_RQ/DIO8` pin,
- and the SIM card's cellular access point name (APN).

@note  The u-blox based Digi XBee's (3G global and LTE-M global) may be more stable used in bypass mode (below).
The Telit based Digi XBees (LTE Cat1 both Verizon and AT&T) can only use this mode.

[//]: # ( @menusnip{xbee_cell_transparent} )

Depending on your cellular carrier, it is best to select the proper carrier profile and network.
Setting these helps the modem to connect to network faster.
This is shows in the [XBee Cellular Carrier](@ref menu_setup_xbeec_carrier) chunk of the setup function.


[//]: # ( @subsection menu_xbee_ltem_by Digi XBee3 LTE-M Bypass )
### Digi XBee3 LTE-M - Bypass Mode

This code is for Digi's LTE-M XBee3 based on the u-blox SARA R410M - used in bypass mode.
To create a DigiXBeeLTEBypass object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the status pin,
- whether the status pin is the true status pin (`ON/SLEEP_N/DIO9`) or the `CTS_N/DIO7` pin,
- the MCU pin connected to the `RESET_N`pin,
- the `DTR_N/SLEEP_RQ/DIO8` pin,
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{xbee3_ltem_bypass} )

Depending on your cellular carrier, it is best to select the proper carrier profile and network.
Setting these helps the modem to connect to network faster.
This is shows in the [SARA R4 Cellular Carrier](@ref setup_r4_carrrier) chunk of the setup function.


[//]: # ( @subsubsection menu_digi_3gby Digi XBee 3G - Bypass Mode )
### Digi XBee 3G - Bypass Mode

This code is for Digi's 3G/2G XBee based on the u-blox SARA U201 - used in bypass mode.
To create a DigiXBee3GBypass object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the status pin,
- whether the status pin is the "true" status pin (`ON/SLEEP_N/DIO9`) or the `CTS_N/DIO7` pin,
- the MCU pin connected to the `RESET_N`pin,
- the `DTR_N/SLEEP_RQ/DIO8` pin,
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{xbee_3g_bypass} )


[//]: # ( @subsection menu_xbee_wifi Digi XBee S6B Wifi )
### Digi XBee S6B Wifi

This code is for the Digi's S6B wifi module.
To create a DigiXBeeWifi object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the status pin,
- whether the status pin is the "true" status pin (`ON/SLEEP_N/DIO9`) or the `CTS_N/DIO7` pin,
- the MCU pin connected to the `RESET_N`pin,
- the `DTR_N/SLEEP_RQ/DIO8` pin,
- the wifi access point name,
- and the wifi WPA2 password.

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{xbee_wifi} )


[//]: # ( @subsection menu_esp Espressif ESP8266 )
### Espressif ESP8266

This code is for the Espressif ESP8266 or ESP32 operating with "AT" firmware.
To create a EspressifESP8266 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the light sleep status pin (on both ESP and MCU),
- the reset pin (MCU pin connected to the ESP's `RSTB/DIO16`),
- the light sleep wake pin (on both the ESP and the MCU),
- the wifi access point name,
- and the wifi WPA2 password.

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{esp8266} )

Because the ESP8266's default baud rate is too fast for an 8MHz board like the Mayfly, to use it you need to drop the baud rate down for sucessful communication.
You can set the slower baud rate using some external method, or useing the code from the ESP8266 Baud Rate(@ref menu_setup_esp) part of the setup function below.


[//]: # ( @subsection menu_bg96 Quectel BG96 )
### Quectel BG96

This code is for the Dragino, Nimbelink or other boards based on the Quectel BG96.
To create a QuectelBG96 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `STATUS` pin,
- the MCU pin connected to the `RESET_N` pin,
- the MCU pin connected to the `PWRKEY` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{bg96} )

If you are interfacing with a Nimbelink Skywire board via the Skywire development board, you also need to handle the fact that the development board reverses the levels of the status, wake, and reset pins.
Code to invert the pin levels is in the [Skywire Pin Inversions](@ref menu_setup_skywire) part of the setup function below.


[//]: # ( @subsection menu_monarch Sequans Monarch )
### Sequans Monarch

This code is for the Nimbelink LTE-M Verizon/Sequans or other boards based on the Sequans Monarch series SoC.
To create a SequansMonarch object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to either the `GPIO3/STATUS_LED` or `POWER_MON` pin,
- the MCU pin connected to the `RESETN` pin,
- the MCU pin connected to the `RTS` or `RTS0` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{monarch} )

If you are interfacing with a Nimbelink Skywire board via the Skywire development board, you also need to handle the fact that the development board reverses the levels of the status, wake, and reset pins.
Code to invert the pin levels is in the [Skywire Pin Inversions](@ref menu_setup_skywire) part of the setup function below.

The default baud rate of the SVZM20 is much too fast for almost all Arduino boards.
_Before_ attampting to connect a SVZM20 to an Arduino you should connect it to your computer and use AT commands to decrease the baud rate.
The proper command to decrease the baud rate to 9600 (8N1) is: `AT+IPR=9600`.


[//]: # ( @subsection menu_sim800 SIMCom SIM800 )
### SIMCom SIM800

This code is for a SIMCom SIM800 or SIM900 or one of their many variants, including the Adafruit Fona and the Sodaq 2GBee R4.
To create a SIMComSIM800 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `STATUS` pin,
- the MCU pin connected to the `RESET` pin,
- the MCU pin connected to the `PWRKEY` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

_NOTE:_  This is NOT the correct form for a Sodaq 2GBee R6 or R7.
See the  section for a 2GBee R6.

[//]: # ( @menusnip{sim800} )


[//]: # ( @subsection menu_sim7000 SIMCom SIM7000 )
### SIMCom SIM7000

This code is for a SIMCom SIM7000 or one of its variants.
To create a SIMComSIM7000 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `STATUS` pin,
- the MCU pin connected to the `RESET` pin,
- the MCU pin connected to the `PWRKEY` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{sim7000} )


[//]: # ( @subsection menu_gprsbee Sodaq GPRSBee )
### Sodaq GPRSBee

This code is for the Sodaq 2GBee R6 and R7 based on the SIMCom SIM800.
To create a Sodaq2GBeeR6 object we need to know
- the serial object name,
- the MCU pin controlling modem power, (**NOTE:**  On the GPRSBee R6 and R7 the pin labeled as ON/OFF in Sodaq's diagrams is tied to _both_ the SIM800 power supply and the (inverted) SIM800 `PWRKEY`.
You should enter this pin as the power pin.)
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.
The GPRSBee R6/R7 does not expose the `RESET` pin of the SIM800.
The `PWRKEY` is held `LOW` as long as the SIM800 is powered (as mentioned above).

[//]: # ( @menusnip{gprsbee} )


[//]: # ( @subsection menu_ubeer410 u-blox SARA R410M )
### u-blox SARA R410M

This code is for modules based on the 4G LTE-M u-blox SARA R410M including the Sodaq UBee.
To create a SodaqUBeeR410M object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `V_INT` pin (for status),
- the MCU pin connected to the `RESET_N` pin,
- the MCU pin connected to the `PWR_ON` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{sara_r410m} )

Depending on your cellular carrier, it is best to select the proper carrier profile and network.
Setting these helps the modem to connect to network faster.
This is shows in the [SARA R4 Cellular Carrier](@ref setup_r4_carrrier) chunk of the setup function.

[//]: # ( @subsection menu_ubeeu201 u-blox SARA U201 )
### u-blox SARA U201

This code is for modules based on the 3G/2G u-blox SARA U201 including the Sodaq UBee or the Sodaq 3GBee.
To create a SodaqUBeeU201 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `V_INT` pin (for status),
- the MCU pin connected to the `RESET_N` pin,
- the MCU pin connected to the `PWR_ON` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{sara_u201} )


[//]: # ( @subsection menu_modem_vars Modem Measured Variables )
### Modem Measured Variables

After creating the modem object, we can create Variable objects for each of the variables the modem is capable of measuring (Modem_SignalPercent, Modem_BatteryState, Modem_BatteryPercent, Modem_BatteryVoltage, and Modem_Temp).
When we create the modem-linked variable objects, the first argument of the constructor, the loggerModem to like the variables to is required.
The second and third arguments (the UUID and the variable code) included here are optional.
Note that here we create the variables for anything measured by _any_ of the modems, but most modems are not capable of measuring all of the values.
Some modem-measured values may be meaningless depending on the board configuration - often the battery parameters returned by a cellular component have little meaning because the module is downstream of a voltage regulator.

[//]: # ( @menusnip{modem_variables} )

___

[//]: # ( @section menu_sensors_and_vars Sensors and Measured Variables )
## Sensors and Measured Variables

[//]: # ( @subsection menu_processor_sensor The processor as a sensor )
### The processor as a sensor

Set options and create the objects for using the processor as a sensor to report battery level, processor free ram, and sample number.

The processor can return the number of "samples" it has taken, the amount of RAM it has available and, for some boards, the battery voltage (EnviroDIY Mayfly, Sodaq Mbili, Ndogo, Autonomo, and One, Adafruit Feathers).
The version of the board is required as input (ie, for a EnviroDIY Mayfly: "v0.3" or "v0.4" or "v0.5").
Use a blank value (ie, "") for un-versioned boards.
Please note that while you can opt to average more than one sample, it really makes no sense to do so for the processor.
The number of "samples" taken will increase by one for each time another processor "measurement" is taken so averaging multiple measurements from the processor will result in the number of samples increasing by more than one with each loop.

@see @ref sensor_processor

[//]: # ( @menusnip{processor_sensor} )
___

[//]: # ( @subsection menu_ds3231 Maxim DS3231 RTC as a sensor )
### Maxim DS3231 RTC as a sensor

In addition to the time, we can also use the required DS3231 real time clock to report the temperature of the circuit board.
This temperature is _not_ equivalent to an environmental temperature measurement and should only be used to as a diagnostic.
As above, we create both the sensor and the variables measured by it.

@see @ref sensor_ds3231

[//]: # ( @menusnip{ds3231} )
___

[//]: # ( @subsection menu_am2315 AOSong AM2315 )
### AOSong AM2315

Here is the code for the AOSong AM2315 temperature and humidity sensor.
This is an I2C sensor with only one possible address so the only argument required for the constructor is the pin on the MCU controlling power to the AM2315 (AM2315Power).
The number of readings to average from the sensor is optional, but can be supplied as the second argument for the constructor if desired.

@see @ref sensor_am2315

[//]: # ( @menusnip{am2315} )
___

[//]: # ( @subsection menu_dht AOSong DHT )
### AOSong DHT

Here is the code for the AOSong DHT temperature and humidity sensor.
To create the DHT Sensor we need the power pin, the data pin, and the DHT type.
The number of readings to average from the sensor is optional, but can be supplied as the fourth argument for the constructor if desired.

@see @ref sensor_dht

[//]: # ( @menusnip{dht} )
___

[//]: # ( @subsection menu_sq212 Apogee SQ-212 Quantum Light Sensor )
### Apogee SQ-212 Quantum Light Sensor

Here is the code for the Apogee SQ-212 Quantum Light Sensor.
The SQ-212 is not directly connected to the MCU, but rather to an TI ADS1115 that communicates with the MCU.
The Arduino pin controlling power on/off and the analog data channel _on the TI ADS1115_ are required for the sensor constructor.
If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the third argument.
The number of readings to average from the sensor is optional, but can be supplied as the fourth argument for the constructor if desired.

@see @ref sensor_sq212

[//]: # ( @menusnip{dht} )
___


[//]: # ( @subsection menu_atlas_sensors Atlas Scientific EZO Circuits )
### Atlas Scientific EZO Circuits

The next several sections are for Atlas Scientific EZO circuts and sensors.
The sensor class constructors for each are nearly identical, except for the class name.
In the most common setup, with hardware I2C, the only required argument for the constructor is the Arduino pin controlling power on/off; the i2cAddressHex is optional as is the number of readings to average.

@warning **You must isolate the data lines of all Atlas circuits from the main I2C bus if you wish to turn off their power!**
If you do not isolate them from your main I2C bus and you turn off power to the circuits between measurements the I2C lines will be pulled down to ground causing the I2C bus (and thus your logger) to crash.

The default I2C addresses for the circuits are:
- CO2: 0x69 (105)
- DO: 0x61 (97)
- EC (conductivity): 0x64 (100)
- ORP (redox): 0x62 (98)
- pH: 0x63 (99)
- RTD (temperature): 0x66 (102)
All of the circuits can be re-addressed to any other 8 bit number if desired.
To use multiple circuits of the same type, re-address them.

If for some reason you want or need to use software I2C, compile with the build flag `-D MS_ATLAS_SOFTWAREWIRE`.
The constructors for the software I2C implementation requires either the SCL and SDA pin numbers or a reference to the I2C object as arguments.
You must use Testato's [SoftwareWire](https://github.com/Testato/SoftwareWire) library for software I2C.
See the [software wire](@ref menu_softwarewire) section for an example of creating a software I2C instance to share between sensors.

@warning Either all or none of the Atlas sensors can be using software I2C.
Using some Altas sensors with software I2C and others with hardware I2C is not supported.

@see @ref atlas_group


[//]: # ( @subsubsection menu_atlas_co2 Atlas Scientific EZO-CO2 Embedded NDIR Carbon Dioxide Sensor )
#### Atlas Scientific EZO-CO2 Embedded NDIR Carbon Dioxide Sensor

@see @ref sensor_atlas_co2

[//]: # ( @menusnip{atlas_co2} )
___


[//]: # ( @subsubsection menu_atlas_do Atlas Scientific EZO-DO Dissolved Oxygen Sensor )
#### Atlas Scientific EZO-DO Dissolved Oxygen Sensor

@see @ref sensor_atlas_do

[//]: # ( @menusnip{atlas_do} )
___


[//]: # ( @subsubsection menu_atlas_orp Atlas Scientific EZO-ORP Oxidation/Reduction Potential Sensor )
#### Atlas Scientific EZO-ORP Oxidation/Reduction Potential Sensor

@see @ref sensor_atlas_orp

[//]: # ( @menusnip{atlas_orp} )
___


[//]: # ( @subsubsection menu_atlas_ph Atlas Scientific EZO-pH Sensor )
#### Atlas Scientific EZO-pH Sensor

@see @ref sensor_atlas_ph

[//]: # ( @menusnip{atlas_ph} )
___


[//]: # ( @subsubsection menu_atlas_rtd Atlas Scientific EZO-RTD Temperature Sensor )
#### Atlas Scientific EZO-RTD Temperature Sensor

@see @ref sensor_atlas_rtd

[//]: # ( @menusnip{atlas_rtd} )
___


[//]: # ( @subsubsection menu_atlas_ec Atlas Scientific EZO-EC Conductivity Sensor )
#### Atlas Scientific EZO-EC Conductivity Sensor

@see @ref sensor_atlas_cond

[//]: # ( @menusnip{atlas_ec} )
___


[//]: # ( @subsection menu_bme280 Bosch BME280 Environmental Sensor )
### Bosch BME280 Environmental Sensor

Here is the code for the Bosch BME280 environmental sensor.
The only input needed is the Arduino pin controlling power on/off; the i2cAddressHex is optional as is the number of readings to average.
Keep in mind that the possible I2C addresses of the BME280 match those of the MS5803; when using those sensors together, make sure they are set to opposite addresses.

@see @ref sensor_bme280

[//]: # ( @menusnip{bme280} )
___


[//]: # ( @subsection menu_obs3 Campbell OBS3+ Analog Turbidity Sensor )
### Campbell OBS3+ Analog Turbidity Sensor

This is the code for the Campbell OBS3+.
The Arduino pin controlling power on/off, analog data channel _on the TI ADS1115_, and calibration values _in Volts_ for Ax^2 + Bx + C are required for the sensor constructor.
A custom variable code can be entered as a second argument in the variable constructors, and it is very strongly recommended that you use this otherwise it will be very difficult to determine which return is high and which is low range on the sensor.
If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the third argument.
Do NOT forget that if you want to give a number of measurements to average, that comes _after_ the i2c address in the constructor!

Note that to access both the high and low range returns, two instances must be created, one at the low range return pin and one at the high pin.

@see @ref sensor_obs3

[//]: # ( @menusnip{obs3} )
___


[//]: # ( @subsection menu_es2 Decagon ES2 Conductivity and Temperature Sensor )
### Decagon ES2 Conductivity and Temperature Sensor

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.
Optionally, you can include a number of distinct readings to average.
The data pin must be a pin that supports pin-change interrupts.

@see @ref sensor_es2

[//]: # ( @menusnip{es2} )
___


[//]: # ( @subsection menu_ext_volt External Voltage via TI ADS1x15 )
### External Voltage via TI ADS1x15

The Arduino pin controlling power on/off and the analog data channel _on the TI ADS1115_ are required for the sensor constructor.
If using a voltage divider to increase the measurable voltage range, enter the gain multiplier as the third argument.
If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the fourth argument.
The number of measurements to average, if more than one is desired, goes as the fifth argument.

@see @ref sensor_ads1x15

[//]: # ( @menusnip{ext_volt} )
___


[//]: # ( @subsection menu_mpl115a2 Freescale Semiconductor MPL115A2 Miniature I2C Digital Barometer )
### Freescale Semiconductor MPL115A2 Miniature I2C Digital Barometer

The only input needed for the sensor constructor is the Arduino pin controlling power on/off and optionally the number of readings to average.
Because this sensor can have only one I2C address (0x60), it is only possible to connect one of these sensors to your system.

@see @ref sensor_mpl115a2

[//]: # ( @menusnip{mpl115a2} )
___


[//]: # ( @subsection menu_keller_sensors Keller RS485/Modbus Water Level Sensors )
### Keller RS485/Modbus Water Level Sensors

The next two sections are for Keller RS485/Modbus water level sensors.
The sensor class constructors for each are nearly identical, except for the class name.
The sensor constructors require as input: the sensor modbus address,  a stream instance for data (ie, `Serial`), and one or two power pins.
The Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter and the number of readings to average are optional.
(Use -1 for the second power pin and -1 for the enable pin if these don't apply and you want to average more than one reading.)  Please see the section "[Notes on Arduino Streams and Software Serial](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)" for more information about what streams can be used along with this library.
In tests on these sensors, SoftwareSerial_ExtInts _did not work_ to communicate with these sensors, because it isn't stable enough.
AltSoftSerial and HardwareSerial work fine.
Up to two power pins are provided so that the RS485 adapter, the sensor and/or an external power relay can be controlled separately.
If the power to everything is controlled by the same pin, use -1 for the second power pin or omit the argument.
If they are controlled by different pins _and no other sensors are dependent on power from either pin_ then the order of the pins doesn't matter.
If the RS485 adapter, sensor, or relay are controlled by different pins _and any other sensors are controlled by the same pins_ you should put the shared pin first and the un-shared pin second.
Both pins _cannot_ be shared pins.

@see @ref keller_group


[//]: # ( @subsubsection menu_nanolevel Keller Nanolevel Level Transmitter )
#### Keller Nanolevel Level Transmitter

@see @ref sensor_nanolevel

[//]: # ( @menusnip{nanolevel} )
___


[//]: # ( @subsubsection menu_acculevel Keller Acculevel High Accuracy Submersible Level Transmitter )
#### Keller Acculevel High Accuracy Submersible Level Transmitter

@see @ref sensor_acculevel

[//]: # ( @menusnip{acculevel} )
___


[//]: # ( @subsection menu_maxbotics Maxbotix HRXL Ultrasonic Range Finder )
### Maxbotix HRXL Ultrasonic Range Finder

The Arduino pin controlling power on/off, a stream instance for received data (ie, `Serial`), and the Arduino pin controlling the trigger are required for the sensor constructor.
(Use -1 for the trigger pin if you do not have it connected.)  Please see the section "[Notes on Arduino Streams and Software Serial](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)" for more information about what streams can be used along with this library.

@see @ref sensor_maxbotix

[//]: # ( @menusnip{maxbotics} )
___


[//]: # ( @subsection menu_ds18 Maxim DS18 One Wire Temperature Sensor )
### Maxim DS18 One Wire Temperature Sensor

The OneWire hex address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor, though the address can be omitted if only one sensor is used.
The OneWire address is an array of 8 hex values, for example:  {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0}.
To get the address of your sensor, plug a single sensor into your device and run the [oneWireSearch](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/oneWireSearch/oneWireSearch.ino) example or the [Single](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Single/Single.pde) example provided within the Dallas Temperature library.
The sensor address is programmed at the factory and cannot be changed.

@see @ref sensor_ds18

[//]: # ( @menusnip{ds18} )
___


[//]: # ( @subsection menu_ms5803 Measurement Specialties MS5803-14BA Pressure Sensor )
### Measurement Specialties MS5803-14BA Pressure Sensor

The only input needed is the Arduino pin controlling power on/off; the i2cAddressHex and maximum pressure are optional as is the number of readings to average.
Keep in mind that the possible I2C addresses of the MS5803 match those of the BME280.

@see @ref sensor_ms5803

[//]: # ( @menusnip{ms5803} )
___


[//]: # ( @subsection menu_meter_sensors Meter SDI-12 Sensors )
### Meter SDI-12 Sensors

The next few sections are for Meter SDI-12 sensors.
The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.
Optionally, you can include a number of distinct readings to average.
The data pin must be a pin that supports pin-change interrupts.


[//]: # ( @subsubsection menu_fivetm Meter ECH2O Soil Moisture Sensor )
#### Meter ECH2O Soil Moisture Sensor

@see @ref sensor_fivetm

[//]: # ( @menusnip{fivetm} )
___


[//]: # ( @subsubsection menu_hydros21 Meter Hydros 21 Conductivity, Temperature, and Depth Sensor )
#### Meter Hydros 21 Conductivity, Temperature, and Depth Sensor

@see @ref sensor_hydros21

[//]: # ( @menusnip{hydros21} )
___


[//]: # ( @subsubsection menu_teros Meter Teros 11 Soil Moisture Sensor )
#### Meter Teros 11 Soil Moisture Sensor

@see @ref sensor_teros11

[//]: # ( @menusnip{teros} )
___


[//]: # ( @subsection menu_pt_redox PaleoTerra Redox Sensors )
### PaleoTerra Redox Sensors

Because older versions of these sensors all ship with the same I2C address, and more than one is frequently used at different soil depths in the same profile, this module has an optional dependence on Testato's [SoftwareWire](https://github.com/Testato/SoftwareWire) library for software I2C.

To use software I2C, compile with the build flag `-D MS_PALEOTERRA_SOFTWAREWIRE`.
See the [software wire](@ref menu_softwarewire) section for an example of creating a software I2C instance to share between sensors.

The constructors for the software I2C implementation requires either the SCL and SDA pin numbers or a reference to the I2C object as arguments.
All variants of the constructor require the Arduino power pin.
The I2C address can be given if it the sensor is not set to the default of 0x68.
A number of readings to average can also be given.

@warning Either all or none your attached redox may use software I2C.
Using some with software I2C and others with hardware I2C is not supported.

@see @ref sensor_pt_redox

[//]: # ( @menusnip{pt_redox} )
___


[//]: # ( @subsection menu_i2c_rain Trinket-Based Tipping Bucket Rain Gauge )
### Trinket-Based Tipping Bucket Rain Gauge

This is for use with a simple external I2C tipping bucket counter based on the [Adafriut Trinket](https://www.adafruit.com/product/1501).
All constructor arguments are optional, but the first argument is for the I2C address of the tip counter (if not 0x08) and the second is for the depth of rain (in mm) per tip event (if not 0.2mm).
Most metric tipping buckets are calibrated to have 1 tip per 0.2mm of rain.
Most English tipping buckets are calibrated to have 1 tip per 0.01" of rain, which is 0.254mm.
Note that you cannot input a number of measurements to average because averaging does not make sense with this kind of counted variable.

@see @ref sensor_i2c_rain

[//]: # ( @menusnip{i2c_rain} )
___


[//]: # ( @subsection menu_tally Northern Widget Tally Event Counter )
### Northern Widget Tally Event Counter

This is for use with Northern Widget's Tally event counter

@warning Northern Widget considers this sensor to be one of their "bleeding edge" sensors.
As such, it is subject to change at any time.
This library may not be updated immediately to reflect changes on the part of Northern Widget.

The only option for the constructor is an optional setting for the I2C address, if the counter is not set at the default of 0x33.
The counter should be continuously powered.

@see @ref sensor_tally

[//]: # ( @menusnip{i2c_wind_tally} )
___


[//]: # ( @subsection menu_ina219 TI INA219 High Side Current Sensor )
### TI INA219 High Side Current Sensor

This is the code for the TI INA219 high side current and voltage sensor.
The Arduino pin controlling power on/off is all that is required for the constructor.
If your INA219 is not at the standard address of 0x40, you can enter its actual address as the fourth argument.
The number of measurements to average, if more than one is desired, goes as the fifth argument.

@see @ref sensor_ina219

[//]: # ( @menusnip{ina219} )
___



[//]: # ( @subsection menu_analog_cond Analog Electrical Conductivity using the Processor's Analog Pins )
### Analog Electrical Conductivity using the Processor's Analog Pins

This is the code for the measuring electrical conductivity using the processor's internal ADC and analog input pins.
The Arduino pin controlling power on/off and the sensing pin are required for the constuctor.
The power supply for the sensor *absolutely must be switched on and off between readings*!
The resistance of your in-circuit resistor, the cell constant for your power cord, and the number of measurements to average are the optional third, fourth, and fifth arguments.
If your processor has an ADS with resolution greater or less than 10-bit, compile with the build flag `-D ANALOG_EC_ADC_RESOLUTION=##`.
For best results, you should also connect the AREF pin of your processors ADC to the power supply for the and compile with the build flag `-D ANALOG_EC_ADC_REFERENCE_MODE=EXTERNAL`.

@see @ref sensor_analog_cond

[//]: # ( @menusnip{analog_cond} )
___


[//]: # ( @subsection menu_yosemitech_sensors Yosemitech RS485/Modbus Environmental Sensors )
### Yosemitech RS485/Modbus Environmental Sensors

The next several sections are for Yosemitech brand sensors.
The sensor class constructors for each are nearly identical, except for the class name.
The sensor constructor requires as input: the sensor modbus address,  a stream instance for data (ie, `Serial`), and one or two power pins.
The Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter and the number of readings to average are optional.
(Use -1 for the second power pin and -1 for the enable pin if these don't apply and you want to average more than one reading.)
For most of the sensors, Yosemitech strongly recommends averaging multiple (in most cases 10) readings for each measurement.
Please see the section "[Notes on Arduino Streams and Software Serial](https://github.com/EnviroDIY/ModularSensors/wiki/Arduino-Streams)" for more information about what streams can be used along with this library.

@see @ref yosemitech_group


[//]: # ( @subsubsection menu_y504 Yosemitech Y504 Dissolved Oxygen Sensor )
#### Yosemitech Y504 Dissolved Oxygen Sensor

@see @ref sensor_y504

[//]: # ( @menusnip{y504} )
___


[//]: # ( @subsubsection menu_y510 Yosemitech Y510 Turbidity Sensor )
#### Yosemitech Y510 Yosemitech Y510 Turbidity Sensor

@see @ref sensor_y510

[//]: # ( @menusnip{y510} )
___


[//]: # ( @subsubsection menu_y511 Yosemitech Y511 Turbidity Sensor with Wiper )
#### Yosemitech Y511 Yosemitech Y511 Turbidity Sensor with Wiper

@see @ref sensor_y511

[//]: # ( @menusnip{y511} )
___


[//]: # ( @subsubsection menu_y514 Yosemitech Y514 Chlorophyll Sensor )
#### Yosemitech Y514 Yosemitech Y514 Chlorophyll Sensor

@see @ref sensor_y514

[//]: # ( @menusnip{y514} )
___


[//]: # ( @subsubsection menu_y520 Yosemitech Y520 Conductivity Sensor )
#### Yosemitech Y520 Yosemitech Y520 Conductivity Sensor

@see @ref sensor_y520

[//]: # ( @menusnip{y520} )
___


[//]: # ( @subsubsection menu_y532 Yosemitech Y532 pH Sensor )
#### Yosemitech Y532 Yosemitech Y532 pH Sensor

@see @ref sensor_y532

[//]: # ( @menusnip{y532} )
___


[//]: # ( @subsubsection menu_y533 Yosemitech Y533 Oxidation Reduction Potential (ORP) Sensor )
#### Yosemitech Y533 Yosemitech Y533 Oxidation Reduction Potential (ORP) Sensor

@see @ref sensor_y533

[//]: # ( @menusnip{y533} )
___


[//]: # ( @subsubsection menu_y550 Yosemitech Y550 Carbon Oxygen Demand (COD) Sensor with Wiper )
#### Yosemitech Y550 Yosemitech Y550 Carbon Oxygen Demand (COD) Sensor with Wiper

@see @ref sensor_y550

[//]: # ( @menusnip{y550} )
___


[//]: # ( @subsubsection menu_y4000 Yosemitech Y4000 Multi-Parameter Sonde )
#### Yosemitech Y4000 Yosemitech Y4000 Multi-Parameter Sonde

@see @ref sensor_y4000

[//]: # ( @menusnip{y4000} )
___


[//]: # ( @subsection menu_dopto Zebra Tech D-Opto Dissolved Oxygen Sensor )
### Zebra Tech D-Opto Dissolved Oxygen Sensor

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.
Optionally, you can include a number of distinct readings to average.
The data pin must be a pin that supports pin-change interrupts.

@see @ref sensor_dopto

[//]: # ( @menusnip{dopto} )
___


[//]: # ( @section menu_calc_vars Calculated Variables )
## Calculated Variables

Create new #Variable objects calculated from the measured variables.
For these calculate variables, we must not only supply a function for the calculation, but also all of the metadata about the variable - like the name of the variable and its units.

[//]: # ( @menusnip{calculated_variables} )
___

[//]: # ( @section menu_create_objs Creating the array, logger, publishers )
## Creating the array, logger, publishers

[//]: # ( @subsection menu_variable_array The variable array )
### The variable array

Create a #VariableArray containing all of the #Variable objects that we are logging the values of.
Since we've created all of the variables above, we only need to call them by name here.

[//]: # ( @menusnip{variable_arrays} )
___

[//]: # ( @subsection menu_logger_obj The Logger Object )
### The Logger Object

Now that we've created the array, we can actually create the #Logger object.

[//]: # ( @menusnip{loggers} )
___

[//]: # ( @subsection menu_data_publisher Data Publisher )
### Data Publishers

Here we set up all three possible data publisers and link all of them to the same Logger object.

[//]: # ( @subsubsection menu_mmw_publisher Monitor My Watershed )
#### Monitor My Watershed

To publish data to the Monitor My Watershed / EnviroDIY Data Sharing Portal first you must register yourself as a user at https://monitormywatershed.org or https://data.envirodiy.org.
Then you must register your site.
After registering your site, a sampling feature and registration token for that site should be visible on the site page.

[//]: # ( @menusnip{monitormw} )
___

[//]: # ( @subsubsection menu_dh_publisher DreamHost )
#### DreamHost

It is extrmemly unlikely you will use this.
You should ignore this section.

[//]: # ( @menusnip{dreamhost} )
___

[//]: # ( @subsubsection menu_thingspeak_publisher ThingSpeak )
#### ThingSpeak

After you have set up channels on ThingSpeak, you can use this code to publish your data to it.

Keep in mind that the order of variables in the VariableArray is **crucial** when publishing to ThingSpeak.

[//]: # ( @menusnip{dreamhost} )
___

[//]: # ( @section menu_working Extra Working Functions )
## Extra Working Functions

Here we're creating a few extra functions on the global scope.
The flash function is used at board start up just to give an indication that the board has restarted.
The battery function calls the #ProcessorStats sensor to check the battery level before attempting to log or publish data.

[//]: # ( @menusnip{working_functions} )
___

[//]: # ( @section menu_setup Arduino Setup Function )
## Arduino Setup Function

This is our setup function.
In Arduino coding, the classic "main" function is replaced by two functions: setup() and loop().
The setup() function runs once when the board boots or restarts.
It usually contains many functions that set the mode of input and output pins and prints out some debugging information to the serial port.
These functions are frequently named "begin".
Because we have a _lot_ of parts to set up, there's a lot going on in this function!

Let's break it down.

[//]: # ( @subsection menu_setup_open Starting the Function )
### Starting the Function

First we just open the function definitions:

```cpp
void setup() {
```

[//]: # ( @subsection menu_setup_wait Wait for USB )
### Wait for USB

Next we wait for the USB debugging port to initialize.
This only applies to SAMD and 32U4 boards that have built-in USB support.
This code should not be used for deployed loggers; it's only for using a USB for debugging.

[//]: # ( @menusnip{setup_wait} )

[//]: # ( @subsection menu_setup_prints Printing a Hello )
### Printing a Hello

Next we print a message out to the debugging port.
This is also just for debugging - it's very helpful when connected to the logger via USB to see a clear indication that the board is starting

[//]: # ( @menusnip{setup_prints} )

[//]: # ( @subsection menu_setup_serial_interrupts Serial Interrupts )
### Serial Interrupts

If we're using either NeoSWSerial or SoftwareSerial_ExtInts we need to assign the data receiver pins to interrupt functionality here in the setup.

The [NeoSWSerial](@ref menu_neoswserial) and [SoftwareSerial_ExtInts](@ref menu_softwareseraial) objects were created way up in the [Extra Serial Ports](@ref menu_serial_ports) section.

**NOTE:**  If you create more than one NeoSWSerial or Software serial object, you need to call the enableInterrupt function for each Rx pin!

For NeoSWSerial we use:
```cpp
    enableInterrupt(neoSSerial1Rx, neoSSerial1ISR, CHANGE);
```

For SoftwareSerial with External interrupts we use:
```cpp
    enableInterrupt(softSerialRx, SoftwareSerial_ExtInts::handle_interrupt,
                    CHANGE);
```

[//]: # ( @subsection menu_setup_serial_begin Serial Begin )
### Serial Begin

Every serial port setup and used in the program must be "begun" in the setup function.
This section calls the begin functions for all of the various ports defined in the [Extra Serial Ports](@ref menu_serial_ports) section

[//]: # ( @menusnip{setup_serial_begins} )

[//]: # ( @subsection menu_setup_pin_periph SAMD Pin Peripherals )
### SAMD Pin Peripherals

After beginning all of the serial ports, we need to set the pin peripheral settings for any SERCOM's we assigned to serial functionality on the SAMD boards.
These were created in the [Extra Serial Ports](@ref menu_samd_serial_ports) section above.
This does not need to be done for an AVR board (like the Mayfly).

[//]: # ( @menusnip{setup_samd_pins} )

[//]: # ( @subsection menu_setup_flash Flash the LEDs )
### Flash the LEDs

Like printing debugging information to the serial port, flashing the board LED's is a very helpful indication that the board just restarted.
Here we set the pin modes for the LED pins and flash them back and forth using the greenredflash() function we created back in the [working functions](@ref menu_working) section.

[//]: # ( @menusnip{setup_flashing_led} )

[//]: # ( @subsection menu_setup_logger Begin the Logger )
### Begin the Logger

Next get ready and begin the logger.
We set the logger time zone and the clock time zone.
The clock time zone is what the RTC will report; the logger time zone is what will be written to the SD card and all data publishers.
The values are set with the `Logger::` prefix because they are static variables of the Logger class rather than member variables.
Here we also tie the logger and modem together and set all the logger pins.
Then we finally run the logger's begin function.

[//]: # ( @menusnip{setup_logger} )

[//]: # ( @subsection menu_setup_sensors Setup the Sensors )
### Setup the Sensors

After beginning the logger, we setup all the sensors.
Unlike all the previous chuncks of the setup that are preparation steps only requiring the mcu processor, this might involve powering up the sensors.
To prevent a low power restart loop, we put a battery voltage condition on the sensor setup.
This prevents a solar powered board whose battery has died from continuously restarting as soon as it gains any power on sunrise.
Without the condition the board would boot with power, try to power hungry sensors, brown out, and restart over and over.

[//]: # ( @menusnip{setup_sesors} )

[//]: # ( @subsection menu_setup_modem Custom Modem Setup )
### Custom Modem Setup

Next we can opt to do some special setup needed for a few of the modems.
You should only use the one chunk that applies to your specific modem configuration and delete the others.

[//]: # ( @subsubsection menu_setup_esp ESP8266 Baud Rate )
#### ESP8266 Baud Rate

This chunk of code reduces the baud rate of the ESP8266 from its default of 115200 to 9600.
This is only needed for 8MHz boards (like the Mayfly) that cannot communicate at 115200 baud.

[//]: # ( @menusnip{setup_esp} )

[//]: # ( @subsubsection menu_setup_skywire Skywire Pin Inversions )
#### Skywire Pin Inversions

This chunk of code reduces the baud rate of the ESP8266 from its default of 115200 to 9600.
This is only needed for 8MHz boards (like the Mayfly) that cannot communicate at 115200 baud.

[//]: # ( @menusnip{setup_skywire} )

[//]: # ( @subsubsection menu_setup_xbeec_carrier XBee Cellular Carrier )
#### XBee Cellular Carrier

This chunk of code sets the carrier profile and network technology for a Digi XBee or XBee3.
You should change the lines with the `CP` and `N#` commands to the proper number to match your SIM card.

[//]: # ( @menusnip{setup_xbeec_carrier} )

[//]: # ( @subsubsection setup_r4_carrrier SARA R4 Cellular Carrier )
#### SARA R4 Cellular Carrier

This chunk of code sets the carrier profile and network technology for a u-blox SARA R4 or N4 module, including a Sodaq R410 UBee or a Digi XBee3 LTE-M in bypass mode..
You should change the lines with the `UMNOPROF` and `URAT` commands to the proper number to match your SIM card.

[//]: # ( @menusnip{setup_r4_carrrier} )

[//]: # ( @subsection menu_setup_rtc Sync the Real Time Clock )
### Sync the Real Time Clock

After any special modem options, we can opt to use the modem to synchronize the real time clock with the NIST time servers.
This is very helpful in keeping the clock from drifting or resetting it if it lost time due to power loss.
Like the sensor setup, we also apply a battery voltage voltage condition before attempting the clock sync.
(All of the supported modems are large power eaters.)
Unlike the sensor setup, we have an additional check for "sanity" of the clock time.
To be considered "sane" the clock has to set somewhere between 2020 and 2025.
It's a broad range, but it will automatically flag values like Jan 1, 2000 - which are the default start value of the clock on power up.

[//]: # ( @menusnip{setup_clock} )

[//]: # ( @subsection menu_setup_file Setup a File on the SD card )
### Setup File on the SD card

We're getting close to the end of the setup function!
This section verifies that the SD card is communicating with the MCU and sets up a file on it for saved data.
Like with the sensors and the modem, we check for battery level before attempting to communicate with the SD card.

[//]: # ( @menusnip{setup_file} )

[//]: # ( @subsection menu_setup_sleep Sleep until the First Data Collection Time )
### Sleep until the First Data Collection Time

We're finally fished with setup!
This chunk puts the system into low power deep sleep until the next logging interval.

[//]: # ( @menusnip{setup_sleep} )

[//]: # ( @subsection menu_setup_done Setup Complete )
### Setup Complete

Set up is done!
This setup function is *really* long.
But don't forget you need to close it with a final curly brace.

```cpp
}
```

___

[//]: # ( @section menu_loop Arduino Loop Function )
## Arduino Loop Function

This is the loop function which will run repeatedly as long as the board is turned on.
**NOTE:**  This example has code for both a typical simple loop and a complex loop that calls lower level logger functions.
You should only pick _one_ loop function and delete the other.

[//]: # ( @subsection menu_simple_loop A Typical Loop )
### A Typical Loop

After the incredibly long setup function, we can do the vast majority of all logger work in a very simple loop function.
Every time the logger wakes we check the battery voltage and do 1 of three things:
1. If the battery is very low, go immediately back to sleep and hope the sun comes back out
2. If the battery is at a moderate level, attempt to collect data from sensors, but do not attempt to publish data.
The modem the biggest power user of the whole system.
3.
At full power, do everything.

[//]: # ( @menusnip{simple_loop} )

[//]: # ( @subsection menu_complex_loop A Complex Loop )
### A Complex Loop

If you need finer control over the steps of the logging function, this code demonstrates how the loop should be constructed.

Here are some guidelines for writing a loop function:

- If you want to log on an even interval, use `if (checkInterval())` or `if (checkMarkedInterval())` to verify that the current or marked time is an even interval of the logging interval..
- Call the `markTime()` function if you want associate with a two iterations of sensor updates with the same timestamp.
This allows you to use `checkMarkedInterval()` to check if an action should be preformed based on the exact time when the logger woke rather than upto several seconds later when iterating through sensors.
- Either:
  - Power up all of your sensors with `sensorsPowerUp()`.
  - Wake up all your sensors with `sensorsWake()`.
  - Update the values all the sensors in your VariableArray together with `updateAllSensors()`.
  - Immediately after running `updateAllSensors()`, put sensors to sleep to save power with `sensorsSleep()`.
  - Power down all of your sensors with `sensorsPowerDown()`.
- Or:
  - Do a full update loop of all sensors, including powering them with `completeUpdate()`.
(This combines the previous 5 functions.)
- After updating the sensors, then call any functions you want to send/print/save data.
- Finish by putting the logger back to sleep, if desired, with `systemSleep()`.

All together, this gives:

[//]: # ( @menusnip{complex_loop} )

[//]: # ( @todo fix links )

If you need more help in writing a complex loop, the [double_logger example program](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/double_logger) demonstrates using a custom loop function in order to log two different groups of sensors at different logging intervals.
The [data_saving example program](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/data_saving) shows using a custom loop in order to save cellular data by saving data from many variables on the SD card, but only sending a portion of the data to the EnviroDIY data portal.

[//]: # ( @section example_menu_pio PlatformIO Configuration )
## PlatformIO Configuration

[//]: # ( @include{lineno} menu_a_la_carte/platformio.ini )

[//]: # ( @section example_menu_code The Complete Code )
## The Complete Code
