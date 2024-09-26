# Processor Compatibility<!-- {#page_processor_compatibility} -->

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )

- [Processor Compatibility](#processor-compatibility)
  - [AtMega1284p (EnviroDIY Mayfly, Sodaq Mbili, Mighty 1284)](#atmega1284p-envirodiy-mayfly-sodaq-mbili-mighty-1284)
  - [AtSAMD21 (Arduino Zero, Adafruit Feather M0, Sodaq Autonomo)](#atsamd21-arduino-zero-adafruit-feather-m0-sodaq-autonomo)
  - [AtMega2560 (Arduino Mega)](#atmega2560-arduino-mega)
  - [AtMega644p (Sanguino)](#atmega644p-sanguino)
  - [AtMega328p (Arduino Uno, Duemilanove, LilyPad, Mini, Seeeduino Stalker, etc)](#atmega328p-arduino-uno-duemilanove-lilypad-mini-seeeduino-stalker-etc)
  - [AtMega32u4 (Arduino Leonardo/Micro, Adafruit Flora/Feather, etc)](#atmega32u4-arduino-leonardomicro-adafruit-florafeather-etc)
  - [Unsupported Processors](#unsupported-processors)

[//]: # ( End GitHub Only )

## AtMega1284p (EnviroDIY Mayfly, Sodaq Mbili, Mighty 1284)<!-- {#processor_1284p} -->

The [EnviroDIY Mayfly](https://envirodiy.org/mayfly/) _is_ the test board for this library.
_Everything_ is designed to work with this processor.

[Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet-Summary.pdf)

[Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet.pdf)

- If using a non-Mayfly 1284p board, an external DS3231 or DS3232 RTC is required and the interrupt pin from the clock must be connected to the MCU.
This RTC is already built into the Mayfly and the interrupt is connected at either pin A7 (default) or D10 (with solder jumper SJ1).
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 13 (MOSI) on a Mayfly/Mbili or pins 6 (MISO), 7 (SCK), and 5 (MOSI) on a Mighty 1284 or other AtMega1284p.
Chip select/slave select is pin 12 on a Mayfly and card detect can be set to pin 18 with solder jumper 10.
CS/SS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- This processor has two built-in hardware TTL serial ports, Serial and Serial1
  - On most boards, Serial is connected to the FDTI chip for USB communication with the computer.
On both the Mayfly and the Mbili Serial1 is wired to the "Bee" sockets for communication with the modem.
- AltSoftSerial can be used on pins 5 (Tx) and 6 (Rx) on the Mayfly or Mbili.
Pin 4 should not be used while using AltSoftSerial on the Mayfly or Mbili.
  - Unfortunately, the Rx and Tx pins are on different Grove plugs on both the Mayfly and the Mbili making AltSoftSerial somewhat inconvenient to use.
- AltSoftSerial can be used on pins 13 (Tx) and 14 (Rx) on the Mighty 1284 and other 1284p boards.
Pin 12 should not be used while using AltSoftSerial on the Mighty 1284.
- Any digital pin can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtSAMD21 (Arduino Zero, Adafruit Feather M0, Sodaq Autonomo)<!-- {#processor_samd21} -->

Fully supported

[Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet-Summary.pdf)

[Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet.pdf)

- This processor has an internal real time clock (RTC) and does not require a DS3231 to be installed.
The built-in RTC is not as accurate as the DS3231, however, and should be synchronized more frequently to keep the time correct.
The processor clock will also reset if the system battery dies because unlike most external RTC's, there is no coin battery backing up the clock.
At this time, the AtSAMD21 is only supported using the internal clock, but support with a more accurate external RTC is planned.
- This processor has built-in USB drivers.
Most boards connect the USB pins to a mini or microUSB connector for the computer connection.
Depending on the software core of the board, you send data to the USB port as either "USBSerial" or simply "Serial".
- Most variants have 2 hardware TTL serial ports ("Serial" on pins 30 (TX) and 31 (RX) and "Serial1" on pins 0 (TX) and 1 (RX)) configured by default.
  - On an Arduino Zero "Serial" goes to the EDBG programming port.
  - On a Sodaq Autonomo "Serial1" goes to the "Bee" port.
  - On an Adafruit Feather M0 only "Serial1" is configured, "Serial" will go to the native USB port.
- Most variants have one SPI port configured by default (likely pins 22 (MISO), 23 (MOSI), and 24 (SCK)).
Chip select/slave select and card detect pins vary by board.
- Most variants have one I2C (Wire) interface configured by default (likely pins 20 (SDA) and 21 (SCL)).
- There are up to _6_ total "sercom" ports hard which can be configured for either hardware serial, SPI, or I2C (wire) communication on this processor.
See <https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview> for more instructions on how to configure these ports, if you need them.
There are also examples for an Adafruit feather found in the menu a la carte example:  <https://github.com/EnviroDIY/ModularSensors/tree/master/examples/menu_a_la_carte>
- AltSoftSerial is not supported on the AtSAMD21.
- SoftwareSerial_ExtInts is not supported on the AtSAMD21.
- NeoSWSerial is not supported at all on the AtSAMD21.
- Any digital pin can be used with SDI-12.
- Because the USB controller is built into the processor, your USB serial connection will close as soon as the processor goes to sleep.
If you need to debug, I recommend using a serial port monitor like [Tera Term](https://ttssh2.osdn.jp/index.html.en) which will automatically renew its connection with the serial port when it connects and disconnects.
Otherwise, you will have to rely on lights on your alert pin or your modem to verify the processor is waking/sleeping properly.
- There are also some oddities with debugging on the SAMD21 where turning on some of the debugging code will cause the native USB to fail (and the board appear to be bricked).
Turn off the debugging and double-tap to reset and reprogram if this happens.

___

## AtMega2560 (Arduino Mega)<!-- {#processor_mega} -->

Should be fully functional, but untested.

- An external DS3231 or DS3232 RTC is required.
- There is a single SPI port on pins 50 (MISO), 52 (SCK), and 51 (MOSI).
Chip select/slave select is on pin 53.
- There is a single I2C (Wire) interface on pins 20 (SDA) and 21 (SCL).
- This processor has 4 built-in hardware serial ports Serial, which is connected to the FTDI chip for USB communication with the computer, Serial1 on pins 19 (RX) and 18 (TX), Serial2 on pins 17 (RX) and 16 (TX), and Serial3 on pins 15 (RX) and 14 (TX).
- If you still need more serial ports, AltSoftSerial can be used on pins 46 (Tx) and 48 (Rx).
Pins 44 and 45 cannot be used while using AltSoftSerial on the AtMega2560.
- Pins 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), and A15 (69) can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtMega644p (Sanguino)<!-- {#processor_664p} -->

Should be fully functional, but untested.

- An external DS3231 or DS3232 RTC is required.
- This processor has two built-in hardware serial ports, Serial and Serial1.
On most boards, Serial is connected to the FDTI chip for USB communication with the computer.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- There is a single SPI port on pins 6 (MISO), 7 (SCK), and 5 (MOSI).
Chip select/slave select and card detect pins vary by board.
- AltSoftSerial can be used on pins 13 (Tx) and 14 (Rx).
Pin 12 cannot be used while using AltSoftSerial on the AtMega644p.
- Any digital pin can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtMega328p (Arduino Uno, Duemilanove, LilyPad, Mini, Seeeduino Stalker, etc)<!-- {#processor_uno} -->

All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.
You will only be able to use a small number of sensors at one time and may not be able to log data.

- An external DS3231 or DS3232 RTC is required.
- There is a singe SPI ports on pins 12 (MISO), 13 (SCK), and 11 (MOSI).
Chip select/slave select is pin 10 on an Uno.
SS/CS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins A4 (SDA) and A5 (SCL).
- This processor only has a single hardware serial port, Serial, which is connected to the FTDI chip for USB communication with the computer.
- AltSoftSerial can be used on pins 9 (Tx) and 8 (Rx).
Pin 10 cannot be used while using AltSoftSerial on the AtMega328p.
- Any digital pin can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtMega32u4 (Arduino Leonardo/Micro, Adafruit Flora/Feather, etc)<!-- {#processor_32u4} -->

All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.
You will only be able to use a small number of sensors at one time and may not be able to log data.

[Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet-Summary.pdf)

[Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet.pdf)

- An external DS3231 or DS3232 RTC is required.
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 16 (MOSI).
Chip select/slave select and card detect pins vary by board.
- There is a single I2C (Wire) interface on pins 2 (SDA) and 3 (SCL).
- This processor has one hardware serial port, Serial, which can _only_ be used for USB communication with a computer
- There is one additional hardware serial port, Serial1, which can communicate with any serial device.
- AltSoftSerial can be used on pins 5 (Tx) and 13 (Rx).
- Only pins 8, 9, 10, 11, 14, 15, and 16 can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.
(And pins 14, 15, and 16 will be eliminated if you are using any SPI devices (like an SD card).)
- Because the USB controller is built into the processor, your USB serial connection will close as soon as the processor goes to sleep.
If you need to debug, I recommend using a serial port monitor like Tera Term which will automatically renew its connection with the serial port when it connects and disconnects.
Otherwise, you will have to rely on lights on your alert pin or your modem to verify the processor is waking/sleeping properly.

___

## Unsupported Processors<!-- {#processor_unsupported} -->

- **ESP8266/ESP32** - Supported _only_ as a communications module (modem) with the default AT command firmware, not supported as an independent controller
- **AtSAM3X (Arduino Due)** - Unsupported at this time due to clock and sleep issues.
  - There is one SPI port on pins 74 (MISO), 76 (MOSI), and 75 (SCK).
Pins 4, 10 and pin 52 can be used for CS/SS.
  - There are I2C (Wire) interfaces on pins 20 (SDA) and 21 (SCL) and 70 (SDA1) and 71 (SCL1).
  - This processor has one hardware serial port, USBSerial, which can _only_ be used for USB communication with a computer
  - There are three additional 3.3V TTL serial ports: Serial1 on pins 19 (RX) and 18 (TX); Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).
Pins 0 and 1 are also connected to the corresponding pins of the ATmega16U2 USB-to-TTL Serial chip, which is connected to the USB debug port.
  - AltSoftSerial is not directly supported on the AtSAM3X.
  - SoftwareSerial_ExtInts is not supported on the AtSAM3X.
  - SDI-12 is not supported on the AtSAM3X
  - Any digital pin can be used with SDI-12.
  - [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-AM3X-SAM3A-Datasheet.pdf)
- **ATtiny** - Unsupported.
This chip has too little processing power and far too few pins and communication ports to ever use this library.
  - [Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATtiny25-45-85-Datasheet.pdf)
- **Teensy 2.x/3.x** - Unsupported
- **STM32** - Unsupported
- **Nordic nRF52840** - Unsupported
- Anything else not listed above as being supported.
