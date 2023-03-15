# Example showing all possible functionality <!-- {#example_menu} -->

This shows most of the functionality of the library at once.
It has code in it for every possible sensor and modem and for both AVR and SAMD boards.
This example should *never* be used directly; it is intended to document all possibilities and to verify compilating.

To create your own code, I recommend starting from a much simpler targeted example, like the [Logging to MMW](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/logging_to_MMW) example, and then adding to it based on only the parts of this menu example that apply to you.

_______

# Walking Through the Code <!-- {#example_menu_walk} -->

[//]: # ( @note )
*NOTE:  This walkthrough is intended to be viewed on GitHub pages at https://envirodiy.github.io/ModularSensors/example_menu.html*

[//]: # ( @warning )
WARNING:  This example is long.
This walk-through is really, really long.
Make use of the table of contents to skip to the parts you need.
___

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [Example showing all possible functionality](#example-showing-all-possible-functionality)
- [Walking Through the Code](#walking-through-the-code)
  - [Defines and Includes](#defines-and-includes)
    - [Defines for the Arduino IDE](#defines-for-the-arduino-ide)
    - [Library Includes](#library-includes)
  - [Logger Settings](#logger-settings)
    - [Creating Extra Serial Ports](#creating-extra-serial-ports)
      - [AVR Boards](#avr-boards)
        - [AltSoftSerial](#altsoftserial)
        - [NeoSWSerial](#neoswserial)
        - [SoftwareSerial with External Interrupts](#softwareserial-with-external-interrupts)
        - [Software I2C/Wire](#software-i2cwire)
      - [SAMD Boards](#samd-boards)
    - [Assigning Serial Port Functionality](#assigning-serial-port-functionality)
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
    - [SIMCom SIM7080G (EnviroDIY LTE Bee])](#simcom-sim7080g-envirodiy-lte-bee)
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
    - [Bosch BMP388 and BMP398 Pressure Sensors](#bosch-bmp388-and-bmp398-pressure-sensors)
    - [Campbell ClariVUE SDI-12 Turbidity Sensor](#campbell-clarivue-sdi-12-turbidity-sensor)
    - [Campbell OBS3+ Analog Turbidity Sensor](#campbell-obs3-analog-turbidity-sensor)
    - [Campbell RainVUE SDI-12 Precipitation Sensor](#campbell-rainvue-sdi-12-precipitation-sensor)
    - [Decagon CTD-10 Conductivity, Temperature, and Depth Sensor](#decagon-ctd-10-conductivity-temperature-and-depth-sensor)
    - [Decagon ES2 Conductivity and Temperature Sensor](#decagon-es2-conductivity-and-temperature-sensor)
    - [Everlight ALS-PT19 Ambient Light Sensor](#everlight-als-pt19-ambient-light-sensor)
    - [External Voltage via TI ADS1x15](#external-voltage-via-ti-ads1x15)
    - [Freescale Semiconductor MPL115A2 Miniature I2C Digital Barometer](#freescale-semiconductor-mpl115a2-miniature-i2c-digital-barometer)
    - [In-Situ Aqua/Level TROLL Pressure, Temperature, and Depth Sensor](#in-situ-aqualevel-troll-pressure-temperature-and-depth-sensor)
    - [In-Situ RDO PRO-X Rugged Dissolved Oxygen Probe](#in-situ-rdo-pro-x-rugged-dissolved-oxygen-probe)
    - [Keller RS485/Modbus Water Level Sensors](#keller-rs485modbus-water-level-sensors)
      - [Keller Acculevel High Accuracy Submersible Level Transmitter](#keller-acculevel-high-accuracy-submersible-level-transmitter)
      - [Keller Nanolevel Level Transmitter](#keller-nanolevel-level-transmitter)
    - [Maxbotix HRXL Ultrasonic Range Finder](#maxbotix-hrxl-ultrasonic-range-finder)
    - [Maxim DS18 One Wire Temperature Sensor](#maxim-ds18-one-wire-temperature-sensor)
    - [Measurement Specialties MS5803-14BA Pressure Sensor](#measurement-specialties-ms5803-14ba-pressure-sensor)
    - [Meter SDI-12 Sensors](#meter-sdi-12-sensors)
      - [Meter ECH2O Soil Moisture Sensor](#meter-ech2o-soil-moisture-sensor)
      - [Meter Hydros 21 Conductivity, Temperature, and Depth Sensor](#meter-hydros-21-conductivity-temperature-and-depth-sensor)
      - [Meter Teros 11 Soil Moisture Sensor](#meter-teros-11-soil-moisture-sensor)
    - [PaleoTerra Redox Sensors](#paleoterra-redox-sensors)
    - [Trinket-Based Tipping Bucket Rain Gauge](#trinket-based-tipping-bucket-rain-gauge)
    - [Sensirion SHT4X Digital Humidity and Temperature Sensor](#sensirion-sht4x-digital-humidity-and-temperature-sensor)
    - [Northern Widget Tally Event Counter](#northern-widget-tally-event-counter)
    - [TI INA219 High Side Current Sensor](#ti-ina219-high-side-current-sensor)
    - [Turner Cyclops-7F Submersible Fluorometer](#turner-cyclops-7f-submersible-fluorometer)
    - [Analog Electrical Conductivity using the Processor's Analog Pins](#analog-electrical-conductivity-using-the-processors-analog-pins)
    - [Yosemitech RS485/Modbus Environmental Sensors](#yosemitech-rs485modbus-environmental-sensors)
      - [Yosemitech Y504 Dissolved Oxygen Sensor](#yosemitech-y504-dissolved-oxygen-sensor)
      - [Yosemitech Y510 Turbidity Sensor](#yosemitech-y510-turbidity-sensor)
      - [Yosemitech Y511 Turbidity Sensor with Wiper](#yosemitech-y511-turbidity-sensor-with-wiper)
      - [Yosemitech Y514 Chlorophyll Sensor](#yosemitech-y514-chlorophyll-sensor)
      - [Yosemitech Y520 Conductivity Sensor](#yosemitech-y520-conductivity-sensor)
      - [Yosemitech Y532 pH Sensor](#yosemitech-y532-ph-sensor)
      - [Yosemitech Y533 Oxidation Reduction Potential (ORP) Sensor](#yosemitech-y533-oxidation-reduction-potential-orp-sensor)
      - [Yosemitech Y551 Carbon Oxygen Demand (COD) Sensor with Wiper](#yosemitech-y551-carbon-oxygen-demand-cod-sensor-with-wiper)
      - [Yosemitech Y560 Ammonium Sensor](#yosemitech-y560-ammonium-sensor)
      - [Yosemitech Y700 Pressure Sensor](#yosemitech-y700-pressure-sensor)
      - [Yosemitech Y4000 Multi-Parameter Sonde](#yosemitech-y4000-multi-parameter-sonde)
    - [Zebra Tech D-Opto Dissolved Oxygen Sensor](#zebra-tech-d-opto-dissolved-oxygen-sensor)
  - [Calculated Variables](#calculated-variables)
  - [Creating the array, logger, publishers](#creating-the-array-logger-publishers)
    - [The variable array](#the-variable-array)
      - [Creating Variables within an Array](#creating-variables-within-an-array)
      - [Creating Variables and Pasting UUIDs from MonitorMyWatershed](#creating-variables-and-pasting-uuids-from-monitormywatershed)
      - [Creating Variables within an Array](#creating-variables-within-an-array-1)
    - [The Logger Object](#the-logger-object)
    - [Data Publishers](#data-publishers)
      - [Monitor My Watershed](#monitor-my-watershed)
      - [DreamHost](#dreamhost)
      - [ThingSpeak](#thingspeak)
      - [Ubidots](#ubidots)
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
      - [SimCom SIM7080G Network Mode](#simcom-sim7080g-network-mode)
      - [XBee Cellular Carrier](#xbee-cellular-carrier)
      - [SARA R4 Cellular Carrier](#sara-r4-cellular-carrier)
    - [Sync the Real Time Clock](#sync-the-real-time-clock)
    - [Setup File on the SD card](#setup-file-on-the-sd-card)
    - [Sleep until the First Data Collection Time](#sleep-until-the-first-data-collection-time)
    - [Setup Complete](#setup-complete)
  - [Arduino Loop Function](#arduino-loop-function)
    - [A Typical Loop](#a-typical-loop)
    - [A Complex Loop](#a-complex-loop)

[//]: # ( End GitHub Only )


## Defines and Includes <!-- {#menu_walk_defines_includes} -->

### Defines for the Arduino IDE <!-- {#menu_walk_defines} -->
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

### Library Includes <!-- {#menu_walk_includes} -->

Next, include the libraries needed for every program using ModularSensors.

[//]: # ( @menusnip{includes} )

___

## Logger Settings <!-- {#menu_walk_logger_and_modem_settings} -->

### Creating Extra Serial Ports <!-- {#menu_walk_serial_ports} -->

This section of the example has all the code to create and link to serial ports for both AVR and SAMD based boards.
The EnviroDIY Mayfly, the Arduino Mega, UNO, and Leonardo are all AVR boards.
The Arduino Zero, the M0 and the Sodaq Autonomo are all SAMD boards.

Many different sensors communicate using some sort of serial or transistor-transistor-logic (TTL) protocol.
Among these are any sensors using RS232, RS485, RS422.
Generally each serial variant (or sometimes each sensor) needs a dedicated serial "port" - its own connection to the processor.
Most processors have built in dedicated wires for serial communication - "Hardware" serial.
See the page on [Arduino streams](@ref page_arduino_streams) for much more detail about serial connections with Arduino processors.
_______

#### AVR Boards <!-- {#menu_walk_avr_serial_ports} -->

Most Arduino AVR style boards have very few (ie, one, or none) dedicated serial ports _available_ after counting out the programming serial port.
So to connect anything else, we need to try to emulate the processor serial functionality with a software library.
This example shows three possible libraries that can be used to emulate a serial port on an AVR board.


##### AltSoftSerial <!-- {#menu_walk_altsoftserial} -->

[AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial) by Paul Stoffregen is the most accurate software serial port for AVR boards.
AltSoftSerial can only be used on one set of pins on each board so only one AltSoftSerial port can be used.
Not all AVR boards are supported by AltSoftSerial.
See the [processor compatibility](@ref page_processor_compatibility) page for more information on which pins are used on supported boards.

[//]: # ( @menusnip{altsoftserial} )


##### NeoSWSerial <!-- {#menu_walk_neoswserial} -->

[NeoSWSerial](https://github.com/SRGDamia1/NeoSWSerial) is the best software serial that can be used on any pin supporting interrupts.
You can use as many instances of NeoSWSerial as you want.
Each instance requires two pins, one for data in and another for data out.
If you only want to use the serial line for incoming or outgoing data, set the other pin to -1.
Not all AVR boards are supported by NeoSWSerial.

[//]: # ( @menusnip{neoswserial} )

When using NeoSWSerial we will also have to actually set the data receiving (Rx) pin modes for interrupt in the [setup function](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_setup_serial_interrupts).


##### SoftwareSerial with External Interrupts <!-- {#menu_walk_softwareserial} -->

The "standard" software serial library uses interrupts that conflict with several other libraries used within this program.
I've created a [version of software serial that has been stripped of interrupts](https://github.com/EnviroDIY/SoftwareSerial_ExtInts) but it is still far from ideal.
This should be used only use if necessary.
It is not a very accurate serial port!

Accepting its poor quality, you can use as many instances of SoftwareSerial as you want.
Each instance requires two pins, one for data in and another for data out.
If you only want to use the serial line for incoming or outgoing data, set the other pin to -1.

[//]: # ( @menusnip{softwareserial} )

When using SoftwareSerial with External Interrupts we will also have to actually set the data receiving (Rx) pin modes for interrupt in the [setup function](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_setup_serial_interrupts).


##### Software I2C/Wire <!-- {#menu_walk_softwarewire} -->

This creates a software I2C (wire) instance that can be shared between multiple sensors.
Only Testato's [SoftwareWire](https://github.com/Testato/SoftwareWire) library is supported.

[//]: # ( @menusnip{softwarewire} )

---

#### SAMD Boards <!-- {#menu_walk_samd_serial_ports} -->

The SAMD21 supports up to 6 _hardware_ serial ports, which is _awesome_.
But, the Arduino core doesn't make use of all of them, so we have to assign them ourselves.

This section of code assigns SERCOM's 1 and 2 to act as Serial2 and Serial3 on pins 10/11 and 5/2 respectively.
These pin selections are based on the Adafruit Feather M0.

[//]: # ( @menusnip{serial_ports_SAMD} )

In addition to creating the extra SERCOM ports here, the pins must be set up as the proper pin peripherals after the serial ports are begun.
This is shown in the [SAMD Pin Peripherals section](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_setup_pin_periph) of the setup function.


NOTE:  The SAMD51 board has an amazing _8_ available SERCOM's, but I do not have any exmple code for using them.

---

### Assigning Serial Port Functionality <!-- {#menu_walk_assign_ports_sw} -->

This section just assigns all the serial ports from the @ref menu_walk_serial_ports section above to specific functionality.
For a board with the option of up to 4 hardware serial ports, like the SAMD21 or Arduino Mega, we use the Serial1 to talk to the modem, Serial2 for modbus, and Serial3 for the Maxbotix.

[//]: # ( @menusnip{assign_ports_hw} )

For an AVR board where we're relying on a mix of hardware and software ports, we use hardware Serial 1 for the modem, AltSoftSerial for modbus, and NeoSWSerial for the Maxbotix.
Depending on how you rank the importance of each component, you can adjust these to your liking.

[//]: # ( @menusnip{assign_ports_sw} )

---

### Logging Options <!-- {#menu_walk_logging_options} -->

Here we set options for the logging and dataLogger object.
This includes setting the time zone (daylight savings time is **NOT** applied) and setting all of the input and output pins related to the logger.

[//]: # ( @menusnip{logging_options} )

___


## Wifi/Cellular Modem Options <!-- {#menu_walk_modem_settings} -->

This modem section is very lengthy because it contains the code with the constructor for every possible supported modem module.
Do _NOT_ try to use more than one modem at a time - it will _NOT_ work.

To create any of the modems, we follow a similar pattern:

First, we'll create a pointer to the serial port (Arduino Stream object) that we'll use for communication between the modem and the MCU.
We also assign the baud rate to a variable here.
There is a table of @ref modem_notes_bauds on the @ref page_modem_notes page.
The baud rate of any of the modules can be changed using AT commands or the `modem.gsmModem.setBaud(uint32_t baud)` function.

Next, we'll assign all the pin numbers for all the other pins connected between the modem and the MCU.
Pins that do not apply should be set as -1.
There is a table of general @ref modem_notes_sleep and @ref modem_notes_mayfly_0_pins on the @ref page_modem_notes page.

All the modems also need some sort of network credentials for internet access.
For WiFi modems, you need the network name and password (assuming WPA2).
For cellular models, you will need the APN assigned to you by the carrier you bought your SIM card from.


### Digi XBee Cellular - Transparent Mode <!-- {#menu_walk_digi_xbee_cellular_transparent} -->

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

Pins that do not apply should be set as -1.
A helpful table detailing the pins to use with the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

@note  The u-blox based Digi XBee's (3G global and LTE-M global) may be more stable used in bypass mode (below).
The Telit based Digi XBees (LTE Cat1 both Verizon and AT&T) can only use this mode.

[//]: # ( @menusnip{digi_xbee_cellular_transparent} )

Depending on your cellular carrier, it is best to select the proper carrier profile and network.
Setting these helps the modem to connect to network faster.
This is shows in the [XBee Cellular Carrier](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_setup_xbeec_carrier) chunk of the setup function.


### Digi XBee3 LTE-M - Bypass Mode <!-- {#menu_walk_digi_xbee_lte_bypass} -->

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
A helpful table detailing the pins to use with the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

[//]: # ( @menusnip{digi_xbee_lte_bypass} )

Depending on your cellular carrier, it is best to select the proper carrier profile and network.
Setting these helps the modem to connect to network faster.
This is shows in the [SARA R4 Cellular Carrier](@ref setup_r4_carrrier) chunk of the setup function.


### Digi XBee 3G - Bypass Mode <!-- {#menu_walk_digi_xbee_3g_bypass} -->

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
A helpful table detailing the pins to use with the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

[//]: # ( @menusnip{digi_xbee_3g_bypass} )

### Digi XBee S6B Wifi <!-- {#menu_walk_digi_xbee_wifi} -->

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
A helpful table detailing the pins to use with the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

[//]: # ( @menusnip{digi_xbee_wifi} )


### Espressif ESP8266 <!-- {#menu_walk_espressif_esp8266} -->

This code is for the Espressif ESP8266 or ESP32 operating with "AT" firmware.
To create a EspressifESP8266 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the reset pin (MCU pin connected to the ESP's `RSTB/DIO16`),
- the wifi access point name,
- and the wifi WPA2 password.

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{espressif_esp8266} )

Because the ESP8266's default baud rate is too fast for an 8MHz board like the Mayfly, to use it you need to drop the baud rate down for sucessful communication.
You can set the slower baud rate using some external method, or useing the code from the ESP8266 Baud Rate(https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_setup_esp) part of the setup function below.


### Quectel BG96 <!-- {#menu_walk_quectel_bg96} -->

This code is for the Dragino, Nimbelink or other boards based on the Quectel BG96.
To create a QuectelBG96 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `STATUS` pin,
- the MCU pin connected to the `RESET_N` pin,
- the MCU pin connected to the `PWRKEY` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{quectel_bg96} )

If you are interfacing with a Nimbelink Skywire board via the Skywire development board, you also need to handle the fact that the development board reverses the levels of the status, wake, and reset pins.
Code to invert the pin levels is in the [Skywire Pin Inversions](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_setup_skywire) part of the setup function below.


### Sequans Monarch <!-- {#menu_walk_sequans_monarch} -->

This code is for the Nimbelink LTE-M Verizon/Sequans or other boards based on the Sequans Monarch series SoC.
To create a SequansMonarch object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to either the `GPIO3/STATUS_LED` or `POWER_MON` pin,
- the MCU pin connected to the `RESETN` pin,
- the MCU pin connected to the `RTS` or `RTS0` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{sequans_monarch} )

If you are interfacing with a Nimbelink Skywire board via the Skywire development board, you also need to handle the fact that the development board reverses the levels of the status, wake, and reset pins.
Code to invert the pin levels is in the [Skywire Pin Inversions](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_setup_skywire) part of the setup function below.

The default baud rate of the SVZM20 is much too fast for almost all Arduino boards.
_Before_ attampting to connect a SVZM20 to an Arduino you should connect it to your computer and use AT commands to decrease the baud rate.
The proper command to decrease the baud rate to 9600 (8N1) is: `AT+IPR=9600`.


### SIMCom SIM800 <!-- {#menu_walk_sim_com_sim800} -->

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

[//]: # ( @menusnip{sim_com_sim800} )


### SIMCom SIM7000 <!-- {#menu_walk_sim_com_sim7000} -->

This code is for a SIMCom SIM7000 or one of its variants.
To create a SIMComSIM7000 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `STATUS` pin,
- the MCU pin connected to the `RESET` pin,
- the MCU pin connected to the `PWRKEY` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.

[//]: # ( @menusnip{sim_com_sim7000} )


### SIMCom SIM7080G (EnviroDIY LTE Bee]) <!-- {#menu_walk_sim_com_sim7080} -->

This code is for a SIMCom SIM7080G or one of its variants, including the [EnviroDIY LTE Bee](https://www.envirodiy.org/product/envirodiy-lte-bee-pack-of-5/).

To create a SIMComSIM7080 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `STATUS` pin,
- the MCU pin connected to the `PWRKEY` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.
A helpful table detailing the pins to use with the EnviroDIY LTE Bee and the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

[//]: # ( @menusnip{sim_com_sim7080} )


### Sodaq GPRSBee <!-- {#menu_walk_sodaq_2g_bee_r6} -->

This code is for the Sodaq 2GBee R6 and R7 based on the SIMCom SIM800.
To create a Sodaq2GBeeR6 object we need to know
- the serial object name,
- the MCU pin controlling modem power, (**NOTE:**  On the GPRSBee R6 and R7 the pin labeled as ON/OFF in Sodaq's diagrams is tied to _both_ the SIM800 power supply and the (inverted) SIM800 `PWRKEY`.
You should enter this pin as the power pin.)
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.
The GPRSBee R6/R7 does not expose the `RESET` pin of the SIM800.
The `PWRKEY` is held `LOW` as long as the SIM800 is powered (as mentioned above).
A helpful table detailing the pins to use with the Sodaq GPRSBee and the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

[//]: # ( @menusnip{sodaq_2g_bee_r6} )

### u-blox SARA R410M <!-- {#menu_walk_sodaq_ubee_r410m} -->

This code is for modules based on the 4G LTE-M u-blox SARA R410M including the Sodaq UBee.
To create a SodaqUBeeR410M object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `V_INT` pin (for status),
- the MCU pin connected to the `RESET_N` pin,
- the MCU pin connected to the `PWR_ON` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.
A helpful table detailing the pins to use with the UBee R410M and the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

[//]: # ( @menusnip{sodaq_ubee_r410m} )

Depending on your cellular carrier, it is best to select the proper carrier profile and network.
Setting these helps the modem to connect to network faster.
This is shows in the [SARA R4 Cellular Carrier](@ref setup_r4_carrrier) chunk of the setup function.

### u-blox SARA U201 <!-- {#menu_walk_sodaq_ubee_u201} -->

This code is for modules based on the 3G/2G u-blox SARA U201 including the Sodaq UBee or the Sodaq 3GBee.
To create a SodaqUBeeU201 object we need to know
- the serial object name,
- the MCU pin controlling modem power,
- the MCU pin connected to the `V_INT` pin (for status),
- the MCU pin connected to the `RESET_N` pin,
- the MCU pin connected to the `PWR_ON` pin (for sleep request),
- and the SIM card's cellular access point name (APN).

Pins that do not apply should be set as -1.
A helpful table detailing the pins to use with the Sodaq UBee U201 and the EnviroDIY Mayfly is available on the [Modem Notes](@ref page_modem_notes) page.

[//]: # ( @menusnip{sodaq_ubee_u201} )

### Modem Measured Variables <!-- {#menu_walk_modem_variables} -->

After creating the modem object, we can create Variable objects for each of the variables the modem is capable of measuring (Modem_SignalPercent, Modem_BatteryState, Modem_BatteryPercent, Modem_BatteryVoltage, and Modem_Temp).
When we create the modem-linked variable objects, the first argument of the constructor, the loggerModem to like the variables to is required.
The second and third arguments (the UUID and the variable code) included here are optional.
Note that here we create the variables for anything measured by _any_ of the modems, but most modems are not capable of measuring all of the values.
Some modem-measured values may be meaningless depending on the board configuration - often the battery parameters returned by a cellular component have little meaning because the module is downstream of a voltage regulator.

[//]: # ( @menusnip{modem_variables} )

___

## Sensors and Measured Variables <!-- {#menu_walk_sensors_and_vars} -->

### The processor as a sensor <!-- {#menu_walk_processor_stats} -->

Set options and create the objects for using the processor as a sensor to report battery level, processor free ram, and sample number.

The processor can return the number of "samples" it has taken, the amount of RAM it has available and, for some boards, the battery voltage (EnviroDIY Mayfly, Sodaq Mbili, Ndogo, Autonomo, and One, Adafruit Feathers).
The version of the board is required as input (ie, for a EnviroDIY Mayfly: "v0.3" or "v0.4" or "v0.5").
Use a blank value (ie, "") for un-versioned boards.
Please note that while you can opt to average more than one sample, it really makes no sense to do so for the processor.
The number of "samples" taken will increase by one for each time another processor "measurement" is taken so averaging multiple measurements from the processor will result in the number of samples increasing by more than one with each loop.

@see @ref sensor_processor

[//]: # ( @menusnip{processor_stats} )

___

### Maxim DS3231 RTC as a sensor <!-- {#menu_walk_maxim_ds3231} -->

In addition to the time, we can also use the required DS3231 real time clock to report the temperature of the circuit board.
This temperature is _not_ equivalent to an environmental temperature measurement and should only be used to as a diagnostic.
As above, we create both the sensor and the variables measured by it.

@see @ref sensor_ds3231

[//]: # ( @menusnip{maxim_ds3231} )

___

### AOSong AM2315 <!-- {#menu_walk_ao_song_am2315} -->

Here is the code for the AOSong AM2315 temperature and humidity sensor.
This is an I2C sensor with only one possible address so the only argument required for the constructor is the pin on the MCU controlling power to the AM2315 (AM2315Power).
The number of readings to average from the sensor is optional, but can be supplied as the second argument for the constructor if desired.

@see @ref sensor_am2315

[//]: # ( @menusnip{ao_song_am2315} )

___

### AOSong DHT <!-- {#menu_walk_ao_song_dht} -->

Here is the code for the AOSong DHT temperature and humidity sensor.
To create the DHT Sensor we need the power pin, the data pin, and the DHT type.
The number of readings to average from the sensor is optional, but can be supplied as the fourth argument for the constructor if desired.

@see @ref sensor_dht

[//]: # ( @menusnip{ao_song_dht} )

___

### Apogee SQ-212 Quantum Light Sensor <!-- {#menu_walk_apogee_sq212} -->

Here is the code for the Apogee SQ-212 Quantum Light Sensor.
The SQ-212 is not directly connected to the MCU, but rather to an TI ADS1115 that communicates with the MCU.
The Arduino pin controlling power on/off and the analog data channel _on the TI ADS1115_ are required for the sensor constructor.
If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the third argument.
The number of readings to average from the sensor is optional, but can be supplied as the fourth argument for the constructor if desired.

@see @ref sensor_sq212

[//]: # ( @menusnip{apogee_sq212} )

___


### Atlas Scientific EZO Circuits <!-- {#menu_walk_atlas_scientific_sensors} -->

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

@see @ref atlas_group


#### Atlas Scientific EZO-CO2 Embedded NDIR Carbon Dioxide Sensor <!-- {#menu_walk_atlas_scientific_co2} -->

@see @ref sensor_atlas_co2

[//]: # ( @menusnip{atlas_scientific_co2} )

___


#### Atlas Scientific EZO-DO Dissolved Oxygen Sensor <!-- {#menu_walk_atlas_scientific_do} -->

@see @ref sensor_atlas_do

[//]: # ( @menusnip{atlas_scientific_do} )

___


#### Atlas Scientific EZO-ORP Oxidation/Reduction Potential Sensor <!-- {#menu_walk_atlas_scientific_orp} -->

@see @ref sensor_atlas_orp

[//]: # ( @menusnip{atlas_scientific_orp} )

___


#### Atlas Scientific EZO-pH Sensor <!-- {#menu_walk_atlas_scientific_ph} -->

@see @ref sensor_atlas_ph

[//]: # ( @menusnip{atlas_scientific_ph} )

___


#### Atlas Scientific EZO-RTD Temperature Sensor <!-- {#menu_walk_atlas_scientific_rtd} -->

@see @ref sensor_atlas_rtd

[//]: # ( @menusnip{atlas_scientific_rtd} )

___


#### Atlas Scientific EZO-EC Conductivity Sensor <!-- {#menu_walk_atlas_scientific_ec} -->

@see @ref sensor_atlas_cond

[//]: # ( @menusnip{atlas_scientific_ec} )

___


### Bosch BME280 Environmental Sensor <!-- {#menu_walk_bosch_bme280} -->

Here is the code for the Bosch BME280 environmental sensor.
The only input needed is the Arduino pin controlling power on/off; the i2cAddressHex is optional as is the number of readings to average.
Keep in mind that the possible I2C addresses of the BME280 match those of the MS5803; when using those sensors together, make sure they are set to opposite addresses.

@see @ref sensor_bme280

[//]: # ( @menusnip{bosch_bme280} )

___


### Bosch BMP388 and BMP398 Pressure Sensors <!-- {#menu_walk_bosch_bmp3xx} -->

@see @ref sensor_bmp3xx

[//]: # ( @menusnip{bosch_bmp3xx} )

___


#### Campbell ClariVUE SDI-12 Turbidity Sensor <!-- {#menu_walk_campbell_clari_vue10} -->

@see @ref sensor_clarivue

[//]: # ( @menusnip{campbell_clari_vue10} )

___


### Campbell OBS3+ Analog Turbidity Sensor <!-- {#menu_walk_campbell_obs3} -->

This is the code for the Campbell OBS3+.
The Arduino pin controlling power on/off, analog data channel _on the TI ADS1115_, and calibration values _in Volts_ for Ax^2 + Bx + C are required for the sensor constructor.
A custom variable code can be entered as a second argument in the variable constructors, and it is very strongly recommended that you use this otherwise it will be very difficult to determine which return is high and which is low range on the sensor.
If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the third argument.
Do NOT forget that if you want to give a number of measurements to average, that comes _after_ the i2c address in the constructor!

Note that to access both the high and low range returns, two instances must be created, one at the low range return pin and one at the high pin.

@see @ref sensor_obs3

[//]: # ( @menusnip{campbell_obs3} )

___


#### Campbell RainVUE SDI-12 Precipitation Sensor <!-- {#menu_walk_campbell_rain_vue10} -->

@see @ref sensor_rainvue

[//]: # ( @menusnip{campbell_rain_vue10} )

___


#### Decagon CTD-10 Conductivity, Temperature, and Depth Sensor <!-- {#menu_walk_decagon_ctd} -->

@see @ref sensor_decagon_ctd

[//]: # ( @menusnip{decagon_ctd} )

___


### Decagon ES2 Conductivity and Temperature Sensor <!-- {#menu_walk_decagon_es2} -->

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.
Optionally, you can include a number of distinct readings to average.
The data pin must be a pin that supports pin-change interrupts.

@see @ref sensor_es2

[//]: # ( @menusnip{decagon_es2} )

___


#### Everlight ALS-PT19 Ambient Light Sensor <!-- {#menu_walk_everlight_alspt19} -->

@see @ref sensor_alspt19

[//]: # ( @menusnip{everlight_alspt19} )

___



### External Voltage via TI ADS1x15 <!-- {#menu_walk_external_voltage} -->

The Arduino pin controlling power on/off and the analog data channel _on the TI ADS1115_ are required for the sensor constructor.
If using a voltage divider to increase the measurable voltage range, enter the gain multiplier as the third argument.
If your ADD converter is not at the standard address of 0x48, you can enter its actual address as the fourth argument.
The number of measurements to average, if more than one is desired, goes as the fifth argument.

@see @ref sensor_ads1x15

[//]: # ( @menusnip{tiads1x15} )

___


### Freescale Semiconductor MPL115A2 Miniature I2C Digital Barometer <!-- {#menu_walk_mpl115a2} -->

The only input needed for the sensor constructor is the Arduino pin controlling power on/off and optionally the number of readings to average.
Because this sensor can have only one I2C address (0x60), it is only possible to connect one of these sensors to your system.

@see @ref sensor_mpl115a2

[//]: # ( @menusnip{freescale_mpl115a2} )

___


#### In-Situ Aqua/Level TROLL Pressure, Temperature, and Depth Sensor <!-- {#menu_walk_in_situ_troll_sdi12a} -->

@see @ref sensor_insitu_troll

[//]: # ( @menusnip{in_situ_troll_sdi12a} )

___


#### In-Situ RDO PRO-X Rugged Dissolved Oxygen Probe <!-- {#menu_walk_in_situ_rdo} -->

@see @ref sensor_insitu_rdo

[//]: # ( @menusnip{in_situ_rdo} )

___


### Keller RS485/Modbus Water Level Sensors <!-- {#menu_walk_keller_sensors} -->

The next two sections are for Keller RS485/Modbus water level sensors.
The sensor class constructors for each are nearly identical, except for the class name.
The sensor constructors require as input: the sensor modbus address,  a stream instance for data (ie, `Serial`), and one or two power pins.
The Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter and the number of readings to average are optional.
(Use -1 for the second power pin and -1 for the enable pin if these don't apply and you want to average more than one reading.)  Please see the section "[Notes on Arduino Streams and Software Serial](https://envirodiy.github.io/ModularSensors/page_arduino_streams.html)" for more information about what streams can be used along with this library.
In tests on these sensors, SoftwareSerial_ExtInts _did not work_ to communicate with these sensors, because it isn't stable enough.
AltSoftSerial and HardwareSerial work fine.

The serial ports for this example are created in the @ref menu_walk_serial_ports section and then assigned to modbus functionality in the @ref menu_walk_assign_ports_sw section.

Up to two power pins are provided so that the RS485 adapter, the sensor and/or an external power relay can be controlled separately.
If the power to everything is controlled by the same pin, use -1 for the second power pin or omit the argument.
If they are controlled by different pins _and no other sensors are dependent on power from either pin_ then the order of the pins doesn't matter.
If the RS485 adapter, sensor, or relay are controlled by different pins _and any other sensors are controlled by the same pins_ you should put the shared pin first and the un-shared pin second.
Both pins _cannot_ be shared pins.

@see @ref keller_group


#### Keller Acculevel High Accuracy Submersible Level Transmitter <!-- {#menu_walk_keller_acculevel} -->

@see @ref sensor_acculevel

[//]: # ( @menusnip{keller_acculevel} )

___


#### Keller Nanolevel Level Transmitter <!-- {#menu_walk_keller_nanolevel} -->

@see @ref sensor_nanolevel

[//]: # ( @menusnip{keller_nanolevel} )

___


### Maxbotix HRXL Ultrasonic Range Finder <!-- {#menu_walk_max_botix_sonar} -->

The Arduino pin controlling power on/off, a stream instance for received data (ie, `Serial`), and the Arduino pin controlling the trigger are required for the sensor constructor.
(Use -1 for the trigger pin if you do not have it connected.)
Please see the section "[Notes on Arduino Streams and Software Serial](https://envirodiy.github.io/ModularSensors/page_arduino_streams.html)" for more information about what streams can be used along with this library.

The serial ports for this example are created in the @ref menu_walk_serial_ports section and then assigned to the sonar functionality in the @ref menu_walk_assign_ports_sw section.

@see @ref sensor_maxbotix

[//]: # ( @menusnip{max_botix_sonar} )

___


### Maxim DS18 One Wire Temperature Sensor <!-- {#menu_walk_maxim_ds18} -->

The OneWire hex address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor, though the address can be omitted if only one sensor is used.
The OneWire address is an array of 8 hex values, for example:  {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0}.
To get the address of your sensor, plug a single sensor into your device and run the [oneWireSearch](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/oneWireSearch/oneWireSearch.ino) example or the [Single](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Single/Single.pde) example provided within the Dallas Temperature library.
The sensor address is programmed at the factory and cannot be changed.

@see @ref sensor_ds18

[//]: # ( @menusnip{maxim_ds18} )

___


### Measurement Specialties MS5803-14BA Pressure Sensor <!-- {#menu_walk_mea_spec_ms5803} -->

The only input needed is the Arduino pin controlling power on/off; the i2cAddressHex and maximum pressure are optional as is the number of readings to average.
Keep in mind that the possible I2C addresses of the MS5803 match those of the BME280.

@see @ref sensor_ms5803

[//]: # ( @menusnip{mea_spec_ms5803} )

___


### Meter SDI-12 Sensors <!-- {#menu_walk_meter_sensors} -->

The next few sections are for Meter SDI-12 sensors.
The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.
Optionally, you can include a number of distinct readings to average.
The data pin must be a pin that supports pin-change interrupts.


#### Meter ECH2O Soil Moisture Sensor <!-- {#menu_walk_decagon_5tm} -->

@see @ref sensor_fivetm

[//]: # ( @menusnip{decagon_5tm} )

___


#### Meter Hydros 21 Conductivity, Temperature, and Depth Sensor <!-- {#menu_walk_meter_hydros21} -->

@see @ref sensor_hydros21

[//]: # ( @menusnip{meter_hydros21} )

___


#### Meter Teros 11 Soil Moisture Sensor <!-- {#menu_walk_meter_teros11} -->

@see @ref sensor_teros11

[//]: # ( @menusnip{meter_teros11} )

___


### PaleoTerra Redox Sensors <!-- {#menu_walk_paleo_terra_redox} -->

Because older versions of these sensors all ship with the same I2C address, and more than one is frequently used at different soil depths in the same profile, this module has an optional dependence on Testato's [SoftwareWire](https://github.com/Testato/SoftwareWire) library for software I2C.

To use software I2C, compile with the build flag `-D MS_PALEOTERRA_SOFTWAREWIRE`.
See the [software wire](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_softwarewire) section for an example of creating a software I2C instance to share between sensors.

The constructors for the software I2C implementation requires either the SCL and SDA pin numbers or a reference to the I2C object as arguments.
All variants of the constructor require the Arduino power pin.
The I2C address can be given if it the sensor is not set to the default of 0x68.
A number of readings to average can also be given.
****
@warning Either all or none of your attached redox probes may use software I2C.
Using some with software I2C and others with hardware I2C is not supported.

@see @ref sensor_pt_redox

[//]: # ( @menusnip{paleo_terra_redox} )

___


### Trinket-Based Tipping Bucket Rain Gauge <!-- {#menu_walk_rain_counter_i2c} -->

This is for use with a simple external I2C tipping bucket counter based on the [Adafriut Trinket](https://www.adafruit.com/product/1501).
All constructor arguments are optional, but the first argument is for the I2C address of the tip counter (if not 0x08) and the second is for the depth of rain (in mm) per tip event (if not 0.2mm).
Most metric tipping buckets are calibrated to have 1 tip per 0.2mm of rain.
Most English tipping buckets are calibrated to have 1 tip per 0.01" of rain, which is 0.254mm.
Note that you cannot input a number of measurements to average because averaging does not make sense with this kind of counted variable.

@see @ref sensor_i2c_rain

[//]: # ( @menusnip{rain_counter_i2c} )

___


#### Sensirion SHT4X Digital Humidity and Temperature Sensor <!-- {#menu_walk_sensirion_sht4x} -->

@see @ref sensor_sht4x

[//]: # ( @menusnip{sensirion_sht4x} )

___


### Northern Widget Tally Event Counter <!-- {#menu_walk_tally} -->

This is for use with Northern Widget's Tally event counter

@warning Northern Widget considers this sensor to be one of their "bleeding edge" sensors.
As such, it is subject to change at any time.
This library may not be updated immediately to reflect changes on the part of Northern Widget.

The only option for the constructor is an optional setting for the I2C address, if the counter is not set at the default of 0x33.
The counter should be continuously powered.

@see @ref sensor_tally

[//]: # ( @menusnip{tally_counter_i2c} )

___


### TI INA219 High Side Current Sensor <!-- {#menu_walk_ti_ina219} -->

This is the code for the TI INA219 high side current and voltage sensor.
The Arduino pin controlling power on/off is all that is required for the constructor.
If your INA219 is not at the standard address of 0x40, you can enter its actual address as the fourth argument.
The number of measurements to average, if more than one is desired, goes as the fifth argument.

@see @ref sensor_ina219

[//]: # ( @menusnip{ti_ina219} )

___


### Turner Cyclops-7F Submersible Fluorometer <!-- {#menu_walk_turner_cyclops} -->

This is the code for the Turner Cyclops-7F submersible fluorometer.
The Arduino pin controlling power on/off and all calibration information is needed for the constructor.
The address of the ADS1x15, if it is different than the default of 0x48, can be entered after the calibration information.
The number of measurements to average, if more than one is desired, is the last argument.

The Cyclops sensors are *NOT* pre-calibrated and must be calibrated prior to deployment.

@see @ref sensor_cyclops

[//]: # ( @menusnip{turner_cyclops} )

___



### Analog Electrical Conductivity using the Processor's Analog Pins <!-- {#menu_walk_analog_elec_conductivity} -->

This is the code for the measuring electrical conductivity using the processor's internal ADC and analog input pins.
The Arduino pin controlling power on/off and the sensing pin are required for the constuctor.
The power supply for the sensor *absolutely must be switched on and off between readings*!
The resistance of your in-circuit resistor, the cell constant for your power cord, and the number of measurements to average are the optional third, fourth, and fifth arguments.
If your processor has an ADS with resolution greater or less than 10-bit, compile with the build flag `-D ANALOG_EC_ADC_RESOLUTION=##`.
For best results, you should also connect the AREF pin of your processors ADC to the power supply for the and compile with the build flag `-D ANALOG_EC_ADC_REFERENCE_MODE=EXTERNAL`.

@see @ref sensor_analog_cond

[//]: # ( @menusnip{analog_elec_conductivity} )

___


### Yosemitech RS485/Modbus Environmental Sensors <!-- {#menu_walk_yosemitech_sensors} -->

The next several sections are for Yosemitech brand sensors.
The sensor class constructors for each are nearly identical, except for the class name.
The sensor constructor requires as input: the sensor modbus address,  a stream instance for data (ie, `Serial`), and one or two power pins.
The Arduino pin controlling the receive and data enable on your RS485-to-TTL adapter and the number of readings to average are optional.
(Use -1 for the second power pin and -1 for the enable pin if these don't apply and you want to average more than one reading.)
For most of the sensors, Yosemitech strongly recommends averaging multiple (in most cases 10) readings for each measurement.
Please see the section "[Notes on Arduino Streams and Software Serial](https://envirodiy.github.io/ModularSensors/page_arduino_streams.html)" for more information about what streams can be used along with this library.
In tests on these sensors, SoftwareSerial_ExtInts _did not work_ to communicate with these sensors, because it isn't stable enough.
AltSoftSerial and HardwareSerial work fine.
NeoSWSerial is a bit hit or miss, but can be used in a pinch.

The serial ports for this example are created in the @ref menu_walk_serial_ports section and then assigned to modbus functionality in the @ref menu_walk_assign_ports_sw section.

@see @ref yosemitech_group


#### Yosemitech Y504 Dissolved Oxygen Sensor <!-- {#menu_walk_yosemitech_y504} -->

@see @ref sensor_y504

[//]: # ( @menusnip{yosemitech_y504} )

___


#### Yosemitech Y510 Turbidity Sensor <!-- {#menu_walk_yosemitech_y510} -->

@see @ref sensor_y510

[//]: # ( @menusnip{yosemitech_y510} )

___


#### Yosemitech Y511 Turbidity Sensor with Wiper <!-- {#menu_walk_yosemitech_y511} -->

@see @ref sensor_y511

[//]: # ( @menusnip{yosemitech_y511} )

___


#### Yosemitech Y514 Chlorophyll Sensor <!-- {#menu_walk_yosemitech_y514} -->

@see @ref sensor_y514

[//]: # ( @menusnip{yosemitech_y514} )

___


#### Yosemitech Y520 Conductivity Sensor <!-- {#menu_walk_yosemitech_y520} -->

@see @ref sensor_y520

[//]: # ( @menusnip{yosemitech_y520} )

___


#### Yosemitech Y532 pH Sensor <!-- {#menu_walk_yosemitech_y532} -->

@see @ref sensor_y532

[//]: # ( @menusnip{yosemitech_y532} )

___


#### Yosemitech Y533 Oxidation Reduction Potential (ORP) Sensor <!-- {#menu_walk_yosemitech_y533} -->

@see @ref sensor_y533

[//]: # ( @menusnip{yosemitech_y533} )

___


#### Yosemitech Y551 Carbon Oxygen Demand (COD) Sensor with Wiper <!-- {#menu_walk_yosemitech_y551} -->

@see @ref sensor_y551

[//]: # ( @menusnip{yosemitech_y551} )

___


#### Yosemitech Y560 Ammonium Sensor <!-- {#menu_walk_yosemitech_y560} -->

@see @ref sensor_y551

[//]: # ( @menusnip{yosemitech_y560} )

___


#### Yosemitech Y700 Pressure Sensor <!-- {#menu_walk_yosemitech_y700} -->

@see @ref sensor_y700

[//]: # ( @menusnip{yosemitech_y700} )

___


#### Yosemitech Y4000 Multi-Parameter Sonde <!-- {#menu_walk_yosemitech_y4000} -->

@see @ref sensor_y4000

[//]: # ( @menusnip{yosemitech_y4000} )

___


### Zebra Tech D-Opto Dissolved Oxygen Sensor <!-- {#menu_walk_zebra_tech_d_opto} -->

The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and the Arduino pin sending and receiving data are required for the sensor constructor.
Optionally, you can include a number of distinct readings to average.
The data pin must be a pin that supports pin-change interrupts.

@see @ref sensor_dopto

[//]: # ( @menusnip{zebra_tech_d_opto} )

___


## Calculated Variables <!-- {#menu_walk_calculated_variables} -->

Create new Variable objects calculated from the measured variables.
For these calculate variables, we must not only supply a function for the calculation, but also all of the metadata about the variable - like the name of the variable and its units.

[//]: # ( @menusnip{calculated_variables} )

___

## Creating the array, logger, publishers <!-- {#menu_walk_create_objs} -->

### The variable array <!-- {#menu_walk_variable_array} -->

Create a VariableArray containing all of the Variable objects that we are logging the values of.

This shows three differnt ways of creating the same variable array and filling it with variables.
You should only use **ONE** of these in your own code

#### Creating Variables within an Array <!-- {#menu_walk_variables_create_in_array} -->

Here we use the `new` keyword to create multiple variables and get pointers to them all at the same time within the arry.

[//]: # ( @menusnip{variables_create_in_array} )


#### Creating Variables and Pasting UUIDs from MonitorMyWatershed <!-- {#menu_walk_variables_separate_uuids} -->

If you are sending data to monitor my watershed, it is much easier to create the variables in an array and then to paste the UUID's all together as copied from the "View Token UUID List" link for a site.
If using this method, be very, very, very careful to make sure the order of your variables exactly matches the order of your UUID's.

[//]: # ( @menusnip{variables_separate_uuids} )


#### Creating Variables within an Array <!-- {#menu_walk_variables_pre_named} -->

You can also create and name variable pointer objects outside of the array (as is demonstrated in all of the code chunks here) and then reference those pointers inside of the array like so:

[//]: # ( @menusnip{variables_pre_named} )

___


### The Logger Object <!-- {#menu_walk_logger_obj} -->

Now that we've created the array, we can actually create the #Logger object.

[//]: # ( @menusnip{loggers} )

___

### Data Publishers <!-- {#menu_walk_data_publisher} -->

Here we set up all three possible data publisers and link all of them to the same Logger object.

#### Monitor My Watershed <!-- {#menu_walk_enviro_diy_publisher} -->

To publish data to the Monitor My Watershed / EnviroDIY Data Sharing Portal first you must register yourself as a user at https://monitormywatershed.org or https://data.envirodiy.org.
Then you must register your site.
After registering your site, a sampling feature and registration token for that site should be visible on the site page.

[//]: # ( @menusnip{enviro_diy_publisher} )

___

#### DreamHost <!-- {#menu_walk_dream_host_publisher} -->

It is extrmemly unlikely you will use this.
You should ignore this section.

[//]: # ( @menusnip{dream_host_publisher} )

___

#### ThingSpeak <!-- {#menu_walk_thing_speak_publisher} -->

After you have set up channels on ThingSpeak, you can use this code to publish your data to it.

Keep in mind that the order of variables in the VariableArray is **crucial** when publishing to ThingSpeak.

[//]: # ( @menusnip{thing_speak_publisher} )

___

#### Ubidots <!-- {#menu_walk_ubidots_publisher} -->

Use this to publish data to Ubidots.

[//]: # ( @menusnip{ubidots_publisher} )

___

## Extra Working Functions <!-- {#menu_walk_working} -->

Here we're creating a few extra functions on the global scope.
The flash function is used at board start up just to give an indication that the board has restarted.
The battery function calls the #ProcessorStats sensor to check the battery level before attempting to log or publish data.

[//]: # ( @menusnip{working_functions} )

___

## Arduino Setup Function <!-- {#menu_walk_setup} -->

This is our setup function.
In Arduino coding, the classic "main" function is replaced by two functions: setup() and loop().
The setup() function runs once when the board boots or restarts.
It usually contains many functions that set the mode of input and output pins and prints out some debugging information to the serial port.
These functions are frequently named "begin".
Because we have a _lot_ of parts to set up, there's a lot going on in this function!

Let's break it down.

### Starting the Function <!-- {#menu_walk_setup_open} -->

First we just open the function definitions:

```cpp
void setup() {
```

### Wait for USB <!-- {#menu_walk_setup_wait} -->

Next we wait for the USB debugging port to initialize.
This only applies to SAMD and 32U4 boards that have built-in USB support.
This code should not be used for deployed loggers; it's only for using a USB for debugging.

[//]: # ( @menusnip{setup_wait} )

### Printing a Hello <!-- {#menu_walk_setup_prints} -->

Next we print a message out to the debugging port.
This is also just for debugging - it's very helpful when connected to the logger via USB to see a clear indication that the board is starting

[//]: # ( @menusnip{setup_prints} )

### Serial Interrupts <!-- {#menu_walk_setup_serial_interrupts} -->

If we're using either NeoSWSerial or SoftwareSerial_ExtInts we need to assign the data receiver pins to interrupt functionality here in the setup.

The [NeoSWSerial](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_neoswserial) and [SoftwareSerial_ExtInts](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_softwareserial) objects were created way up in the [Extra Serial Ports](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_serial_ports) section.

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

[//]: # ( @menusnip{setup_softserial} )

### Serial Begin <!-- {#menu_walk_setup_serial_begin} -->

Every serial port setup and used in the program must be "begun" in the setup function.
This section calls the begin functions for all of the various ports defined in the [Extra Serial Ports](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_serial_ports) section

[//]: # ( @menusnip{setup_serial_begins} )

### SAMD Pin Peripherals <!-- {#menu_walk_setup_pin_periph} -->

After beginning all of the serial ports, we need to set the pin peripheral settings for any SERCOM's we assigned to serial functionality on the SAMD boards.
These were created in the [Extra Serial Ports](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_samd_serial_ports) section above.
This does not need to be done for an AVR board (like the Mayfly).

[//]: # ( @menusnip{setup_samd_pins} )

### Flash the LEDs <!-- {#menu_walk_setup_flash} -->

Like printing debugging information to the serial port, flashing the board LED's is a very helpful indication that the board just restarted.
Here we set the pin modes for the LED pins and flash them back and forth using the greenredflash() function we created back in the [working functions](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_walk_working) section.

[//]: # ( @menusnip{setup_flashing_led} )

### Begin the Logger <!-- {#menu_walk_setup_logger} -->

Next get ready and begin the logger.
We set the logger time zone and the clock time zone.
The clock time zone is what the RTC will report; the logger time zone is what will be written to the SD card and all data publishers.
The values are set with the `Logger::` prefix because they are static variables of the Logger class rather than member variables.
Here we also tie the logger and modem together and set all the logger pins.
Then we finally run the logger's begin function.

[//]: # ( @menusnip{setup_logger} )

### Setup the Sensors <!-- {#menu_walk_setup_sensors} -->

After beginning the logger, we setup all the sensors.
Unlike all the previous chuncks of the setup that are preparation steps only requiring the mcu processor, this might involve powering up the sensors.
To prevent a low power restart loop, we put a battery voltage condition on the sensor setup.
This prevents a solar powered board whose battery has died from continuously restarting as soon as it gains any power on sunrise.
Without the condition the board would boot with power, try to power hungry sensors, brown out, and restart over and over.

[//]: # ( @menusnip{setup_sensors} )

### Custom Modem Setup <!-- {#menu_walk_setup_modem} -->

Next we can opt to do some special setup needed for a few of the modems.
You should only use the one chunk that applies to your specific modem configuration and delete the others.

#### ESP8266 Baud Rate <!-- {#menu_walk_setup_esp} -->

This chunk of code reduces the baud rate of the ESP8266 from its default of 115200 to 9600.
This is only needed for 8MHz boards (like the Mayfly) that cannot communicate at 115200 baud.

[//]: # ( @menusnip{setup_esp} )

#### Skywire Pin Inversions <!-- {#menu_walk_setup_skywire} -->

This chunk of code inverts the pin levels for status, wake, and reset of the modem.
This is necessary for the Skywire development board and some other breakouts.

[//]: # ( @menusnip{setup_skywire} )

#### SimCom SIM7080G Network Mode <!-- {#setup_sim7080} -->

This chunk of code sets the network mode and preferred mode for the SIM7080G.

[//]: # ( @menusnip{setup_sim7080} )

#### XBee Cellular Carrier <!-- {#menu_walk_setup_xbeec_carrier} -->

This chunk of code sets the carrier profile and network technology for a Digi XBee or XBee3.
You should change the lines with the `CP` and `N#` commands to the proper number to match your SIM card.

[//]: # ( @menusnip{setup_xbeec_carrier} )

#### SARA R4 Cellular Carrier <!-- {#setup_r4_carrrier} -->

This chunk of code sets the carrier profile and network technology for a u-blox SARA R4 or N4 module, including a Sodaq R410 UBee or a Digi XBee3 LTE-M in bypass mode..
You should change the lines with the `UMNOPROF` and `URAT` commands to the proper number to match your SIM card.

[//]: # ( @menusnip{setup_r4_carrrier} )

### Sync the Real Time Clock <!-- {#menu_walk_setup_clock} -->

After any special modem options, we can opt to use the modem to synchronize the real time clock with the NIST time servers.
This is very helpful in keeping the clock from drifting or resetting it if it lost time due to power loss.
Like the sensor setup, we also apply a battery voltage voltage condition before attempting the clock sync.
(All of the supported modems are large power eaters.)
Unlike the sensor setup, we have an additional check for "sanity" of the clock time.
To be considered "sane" the clock has to set somewhere between 2020 and 2025.
It's a broad range, but it will automatically flag values like Jan 1, 2000 - which are the default start value of the clock on power up.

[//]: # ( @menusnip{setup_clock} )

### Setup File on the SD card <!-- {#menu_walk_setup_file} -->

We're getting close to the end of the setup function!
This section verifies that the SD card is communicating with the MCU and sets up a file on it for saved data.
Like with the sensors and the modem, we check for battery level before attempting to communicate with the SD card.

[//]: # ( @menusnip{setup_file} )

### Sleep until the First Data Collection Time <!-- {#menu_walk_setup_sleep} -->

We're finally fished with setup!
This chunk puts the system into low power deep sleep until the next logging interval.

[//]: # ( @menusnip{setup_sleep} )

### Setup Complete <!-- {#menu_walk_setup_done} -->

Set up is done!
This setup function is *really* long.
But don't forget you need to close it with a final curly brace.

```cpp
}
```

___

## Arduino Loop Function <!-- {#menu_walk_loop} -->

This is the loop function which will run repeatedly as long as the board is turned on.
**NOTE:**  This example has code for both a typical simple loop and a complex loop that calls lower level logger functions.
You should only pick _one_ loop function and delete the other.

### A Typical Loop <!-- {#menu_walk_simple_loop} -->

After the incredibly long setup function, we can do the vast majority of all logger work in a very simple loop function.
Every time the logger wakes we check the battery voltage and do 1 of three things:
1. If the battery is very low, go immediately back to sleep and hope the sun comes back out
2. If the battery is at a moderate level, attempt to collect data from sensors, but do not attempt to publish data.
The modem the biggest power user of the whole system.
3.
At full power, do everything.

[//]: # ( @menusnip{simple_loop} )

### A Complex Loop <!-- {#menu_walk_complex_loop} -->

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

If you need more help in writing a complex loop, the [double_logger example program](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/double_logger) demonstrates using a custom loop function in order to log two different groups of sensors at different logging intervals.
The [data_saving example program](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/data_saving) shows using a custom loop in order to save cellular data by saving data from many variables on the SD card, but only sending a portion of the data to the EnviroDIY data portal.

[//]: # ( @section example_menu_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} menu_a_la_carte/platformio.ini )

[//]: # ( @section example_menu_code The Complete Code )

[//]: # ( @include{lineno} menu_a_la_carte/menu_a_la_carte.ino )
