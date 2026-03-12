# Processor Compatibility<!--! {#page_processor_compatibility} -->

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [Processor Compatibility](#processor-compatibility)
  - [Processor Configuration and Adding Support](#processor-configuration-and-adding-support)
    - [KnownProcessors.h Documentation](#knownprocessorsh-documentation)
    - [Adding New Processor Support](#adding-new-processor-support)
    - [Board-Specific Parameters](#board-specific-parameters)
    - [Configuration Override](#configuration-override)
  - [AtMega1284p (EnviroDIY Mayfly, Sodaq Mbili, Mighty 1284)](#atmega1284p-envirodiy-mayfly-sodaq-mbili-mighty-1284)
    - [Specific Supported AtMega1284p Boards](#specific-supported-atmega1284p-boards)
    - [AtMega1284p Processor Information](#atmega1284p-processor-information)
  - [AtSAMD21 (Arduino Zero, Adafruit Feather M0, Sodaq Autonomo)](#atsamd21-arduino-zero-adafruit-feather-m0-sodaq-autonomo)
    - [Specific Supported AtSAMD21 Boards](#specific-supported-atsamd21-boards)
    - [AtSAMD21 Processor Information](#atsamd21-processor-information)
  - [AtSAMD51 (Adafruit Feather M4, EnviroDIY Stonefly)](#atsamd51-adafruit-feather-m4-envirodiy-stonefly)
    - [Specific Supported AtSAMD51 Boards](#specific-supported-atsamd51-boards)
    - [AtSAMD51 Processor Information](#atsamd51-processor-information)
  - [AtMega2560 (Arduino Mega)](#atmega2560-arduino-mega)
    - [Specific Supported AtMega2560 Boards](#specific-supported-atmega2560-boards)
    - [AtMega2560 Processor Information](#atmega2560-processor-information)
  - [AtMega644p (Sanguino)](#atmega644p-sanguino)
    - [Specific Supported AtMega644p Boards](#specific-supported-atmega644p-boards)
    - [AtMega644p Processor Information](#atmega644p-processor-information)
  - [AtMega328p (Arduino Uno, Seeeduino Stalker, etc)](#atmega328p-arduino-uno-seeeduino-stalker-etc)
    - [Specific Supported AtMega328p Boards](#specific-supported-atmega328p-boards)
    - [AtMega328p Processor Information](#atmega328p-processor-information)
  - [AtMega32u4 (Arduino Leonardo/Micro, Adafruit Flora/Feather, etc)](#atmega32u4-arduino-leonardomicro-adafruit-florafeather-etc)
    - [Specific Supported AtMega32u4 Boards](#specific-supported-atmega32u4-boards)
    - [AtMega32u4 Processor Information](#atmega32u4-processor-information)
  - [Unsupported Processors](#unsupported-processors)

<!--! @endif -->

## Processor Configuration and Adding Support<!--! {#processor_configuration} -->

The specific processors supported by the ModularSensors library are defined in the [KnownProcessors.h](../src/sensors/KnownProcessors.h) file. This file contains compiler defines for board names, operating voltages, battery monitoring pins, and other board-specific parameters.

### KnownProcessors.h Documentation

The [KnownProcessors.h](../src/sensors/KnownProcessors.h) file defines board-specific parameters for optimal configuration:

**Basic Board Information:**
- `LOGGER_BOARD`: Pretty text name for the board
- `OPERATING_VOLTAGE`: Board operating voltage in volts
- `BATTERY_PIN`: Analog pin for battery voltage monitoring
- `BATTERY_MULTIPLIER`: Voltage divider multiplier for battery measurements
- Built-in ambient light sensor parameters (for compatible boards)

**ADC Configuration:**

- `MS_PROCESSOR_ADC_RESOLUTION`: ADC resolution in bits (10 for AVR, 12 for SAMD)
  - Only add if the default of 10 for a AVR board or 12 for a SAMD board does not apply
- `MS_PROCESSOR_ADC_REFERENCE_MODE`: Voltage reference (DEFAULT, AR_DEFAULT, AR_EXTERNAL, etc.)
  - Only add if the reference mode will not be `DEFAULT`

**Memory Management:**

- `MS_LOG_DATA_BUFFER_SIZE`: Log buffer size in bytes (optimized per board's memory)
  - Only add if something atypical for this processor is needed

**Clock Configuration:**

Only add one of these if the clock chip is integrated into the same circuit board the main processor!

- `MS_USE_DS3231`: Use DS3231 external RTC
- `MS_USE_RV8803`: Use RV8803 external RTC
- `MS_USE_RTC_ZERO`: Use internal RTC (SAMD21 boards only)

### Adding New Processor Support

To add support for a new board of a supported processor type:

1. **Identify the processor type**: Determine if your board uses an AtMega1284p, AtSAMD21, AtSAMD51, AtMega2560, AtMega644p, AtMega328p, or AtMega32u4 processor.

2. **Find the Arduino board define**: Use your board's Arduino IDE board definition to identify the compiler define (e.g., `ARDUINO_AVR_UNO` for Arduino Uno).

3. **Add to KnownProcessors.h**: Add a new `#elif defined()` section with your board's define and set the appropriate parameters:

```cpp
#elif defined(YOUR_BOARD_DEFINE)
#define LOGGER_BOARD "Your Board Name"
#define OPERATING_VOLTAGE 3.3  // or 5.0
#define BATTERY_PIN A0         // or -1 if not available
#define BATTERY_MULTIPLIER 2.0 // or -1 if not available

// ADC defaults; if and only if needed
#ifndef MS_PROCESSOR_ADC_RESOLUTION
#define MS_PROCESSOR_ADC_RESOLUTION 10  // 10 for AVR, 12 for SAMD
#endif
#ifndef MS_PROCESSOR_ADC_REFERENCE_MODE
#define MS_PROCESSOR_ADC_REFERENCE_MODE DEFAULT  // or AR_DEFAULT for SAMD
#endif

// Log buffer size - adjust for board's memory capacity; if and only if needed
#ifndef MS_LOG_DATA_BUFFER_SIZE
#define MS_LOG_DATA_BUFFER_SIZE 1024  // Adjust based on available RAM
#endif

// Built in clock; if and only if integrated into the board
#ifndef MS_USE_DS3231  // Choose appropriate RTC for your board
#define MS_USE_DS3231
#endif
```

4. **Submit contribution**: Create a pull request with your additions to help other users with the same board.

### Board-Specific Parameters

Each board entry should specify:

- **Operating voltage**: Typically 3.3V or 5V
- **Battery monitoring**: Pin and voltage divider information if available
- **Built-in sensors**: Light sensor configuration for boards that include them
- **Clock selection**: Appropriate RTC type for the board's capabilities
  - External RTC (DS3231, RV8803) for most AVR boards
  - Internal RTC (RTC_ZERO) for SAMD21/SAMD51 boards
- **ADC configuration**: Resolution and reference voltage optimized for the processor
- **Memory management**: Log buffer size appropriate for the board's available RAM

### Configuration Override

All board-specific defaults can be overridden in [ModSensorConfig.h](../src/ModSensorConfig.h) if needed:
- Users can uncomment and modify clock, ADC, or buffer size settings
- Build flags can also be used with PlatformIO for custom configurations
- Arduino IDE users should modify ModSensorConfig.h as build flags aren't available

___

## AtMega1284p (EnviroDIY Mayfly, Sodaq Mbili, Mighty 1284)<!--! {#processor_1284p} -->

The [EnviroDIY Mayfly](https://envirodiy.org/mayfly/) _is_ the test board for this library.
_Everything_ is designed to work with this processor.

### Specific Supported AtMega1284p Boards

- **EnviroDIY Mayfly** (`ARDUINO_AVR_ENVIRODIY_MAYFLY`)
- **SODAQ Mbili** (`ARDUINO_AVR_SODAQ_MBILI`)

### AtMega1284p Processor Information

[Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet-Summary.pdf)

[Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet.pdf)

- If using a non-Mayfly 1284p board, an external DS3231 or DS3232 RTC is required and the interrupt pin from the clock must be connected to the MCU.
  This RTC is already built into the Mayfly and the interrupt is connected at either pin A7 (default) or D10 (with solder jumper SJ1).
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 13 (MOSI) on a Mayfly/Mbili or pins 6 (MISO), 7 (SCK), and 5 (MOSI) on a Mighty 1284 or other AtMega1284p.
  Chip select/slave select is pin 12 on a Mayfly and card detect can be set to pin 18 with solder jumper 10.
  CS/SS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- This processor has two built-in hardware TTL serial ports, Serial and Serial1
  - On most boards, Serial is connected to the FTDI chip for USB communication with the computer.
    On both the Mayfly and the Mbili Serial1 is wired to the "Bee" sockets for communication with the modem.
- AltSoftSerial can be used on pins 5 (Tx) and 6 (Rx) on the Mayfly or Mbili.
  Pin 4 should not be used while using AltSoftSerial on the Mayfly or Mbili.
  - Unfortunately, the Rx and Tx pins are on different Grove plugs on both the Mayfly and the Mbili making AltSoftSerial somewhat inconvenient to use.
- AltSoftSerial can be used on pins 13 (Tx) and 14 (Rx) on the Mighty 1284 and other 1284p boards.
  Pin 12 should not be used while using AltSoftSerial on the Mighty 1284.
- Any digital pin can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtSAMD21 (Arduino Zero, Adafruit Feather M0, Sodaq Autonomo)<!--! {#processor_samd21} -->

Fully supported

### Specific Supported AtSAMD21 Boards

- **SODAQ ExpLoRer** (`ARDUINO_SODAQ_EXPLORER`)
- **SODAQ Autonomo** (`ARDUINO_SODAQ_AUTONOMO`)
- **SODAQ ONE Beta** (`ARDUINO_SODAQ_ONE_BETA`)
- **SODAQ ONE** (`ARDUINO_SODAQ_ONE`)
- **Adafruit Feather M0 Express** (`ARDUINO_SAMD_FEATHER_M0_EXPRESS`)
- **Adafruit Feather M0** (`ARDUINO_SAMD_FEATHER_M0`)
- **Arduino Zero** (`ARDUINO_SAMD_ZERO`)

### AtSAMD21 Processor Information

[Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet-Summary.pdf)

[Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet.pdf)

- This processor has an internal real time clock (RTC) and does not **require** an external RTC, though it can be used with one.
  The built-in RTC is not as accurate as the DS3231, however, and should be synchronized more frequently to keep the time correct.
  The processor clock will also reset if the system battery dies because unlike most external RTCs, there is no coin battery backing up the clock.
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
  If you need to debug, I recommend using a serial port monitor like [Tera Term](https://teratermproject.github.io/index-en.html) which will automatically renew its connection with the serial port when it connects and disconnects.
  Otherwise, you will have to rely on lights on your alert pin or your modem to verify the processor is waking/sleeping properly.
- There are also some oddities with debugging on the SAMD21 where turning on some of the debugging code will cause the native USB to fail (and the board appear to be bricked).
  Turn off the debugging and double-tap to reset and reprogram if this happens.

___

## AtSAMD51 (Adafruit Feather M4, EnviroDIY Stonefly)<!--! {#processor_samd51} -->

Fully supported with similar characteristics to AtSAMD21 but with enhanced performance.

### Specific Supported AtSAMD51 Boards

- **EnviroDIY Stonefly** (`ENVIRODIY_STONEFLY_M4`)
- **Adafruit Feather M4** (`ARDUINO_FEATHER_M4`)
- **Feather M4 CAN** (`ARDUINO_FEATHER_M4_CAN`)
- **Adafruit Feather M4 Adalogger** (`ADAFRUIT_FEATHER_M4_ADALOGGER`)
- **Adafruit Grand Central** (`ADAFRUIT_GRAND_CENTRAL_M4`)

### AtSAMD51 Processor Information

These boards share similar capabilities with the AtSAMD21 processor boards including built-in USB, internal RTC, and multiple hardware serial ports, but with increased processing power and memory.

___

## AtMega2560 (Arduino Mega)<!--! {#processor_mega} -->

Should be fully functional, but untested.

### Specific Supported AtMega2560 Boards

- **Arduino Mega ADK** (`ARDUINO_AVR_ADK`)
- **Arduino Mega** (`ARDUINO_AVR_MEGA`)
- **Arduino Mega 2560** (`ARDUINO_AVR_MEGA2560`)

### AtMega2560 Processor Information

- An external RTC (DS3231, DS3232, or RV8803) is required.
- There is a single SPI port on pins 50 (MISO), 52 (SCK), and 51 (MOSI).
  Chip select/slave select is on pin 53.
- There is a single I2C (Wire) interface on pins 20 (SDA) and 21 (SCL).
- This processor has 4 built-in hardware serial ports Serial, which is connected to the FTDI chip for USB communication with the computer, Serial1 on pins 19 (RX) and 18 (TX), Serial2 on pins 17 (RX) and 16 (TX), and Serial3 on pins 15 (RX) and 14 (TX).
- If you still need more serial ports, AltSoftSerial can be used on pins 46 (Tx) and 48 (Rx).
  Pins 44 and 45 cannot be used while using AltSoftSerial on the AtMega2560.
- Pins 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), and A15 (69) can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtMega644p (Sanguino)<!--! {#processor_664p} -->

Should be fully functional, but untested.

### Specific Supported AtMega644p Boards

- **SODAQ Ndogo** (`ARDUINO_AVR_SODAQ_NDOGO`)
- **SODAQ Tatu** (`ARDUINO_AVR_SODAQ_TATU`)
- **SODAQ Moja** (`ARDUINO_AVR_SODAQ_MOJA`)

### AtMega644p Processor Information

- An external RTC (DS3231, DS3232, or RV8803) is required.
- This processor has two built-in hardware serial ports, Serial and Serial1.
  On most boards, Serial is connected to the FTDI chip for USB communication with the computer.
- There is a single I2C (Wire) interface on pins 17 (SDA) and 16 (SCL).
- There is a single SPI port on pins 6 (MISO), 7 (SCK), and 5 (MOSI).
  Chip select/slave select and card detect pins vary by board.
- AltSoftSerial can be used on pins 13 (Tx) and 14 (Rx).
  Pin 12 cannot be used while using AltSoftSerial on the AtMega644p.
- Any digital pin can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtMega328p (Arduino Uno, Seeeduino Stalker, etc)<!--! {#processor_uno} -->

All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.
You will only be able to use a small number of sensors at one time and may not be able to log data.

### Specific Supported AtMega328p Boards

- **Adafruit Feather 328p** (`ARDUINO_AVR_FEATHER328P`)
- **Arduino BT** (`ARDUINO_AVR_BT`)
- **Arduino Duemilanove** (`ARDUINO_AVR_DUEMILANOVE`)
- **Arduino Ethernet** (`ARDUINO_AVR_ETHERNET`)
- **Arduino Fio** (`ARDUINO_AVR_FIO`)
- **Arduino Mini 05** (`ARDUINO_AVR_MINI`)
- **Arduino Nano** (`ARDUINO_AVR_NANO`)
- **Arduino Uno** (`ARDUINO_AVR_UNO`)

### AtMega328p Processor Information

- An external RTC (DS3231, DS3232, or RV8803) is required.
- There is a singe SPI ports on pins 12 (MISO), 13 (SCK), and 11 (MOSI).
  Chip select/slave select is pin 10 on an Uno.
  SS/CS and CD pins may vary for other boards.
- There is a single I2C (Wire) interface on pins A4 (SDA) and A5 (SCL).
- This processor only has a single hardware serial port, Serial, which is connected to the FTDI chip for USB communication with the computer.
- AltSoftSerial can be used on pins 9 (Tx) and 8 (Rx).
  Pin 10 cannot be used while using AltSoftSerial on the AtMega328p.
- Any digital pin can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.

___

## AtMega32u4 (Arduino Leonardo/Micro, Adafruit Flora/Feather, etc)<!--! {#processor_32u4} -->

All functions are supported, but processor doesn't have sufficient power to use all of the functionality of the library.
You will only be able to use a small number of sensors at one time and may not be able to log data.

### Specific Supported AtMega32u4 Boards

- **Adafruit Feather 32u4** (`ARDUINO_AVR_FEATHER32U4`)
- **Arduino Esplora** (`ARDUINO_AVR_ESPLORA`)
- **Arduino Gemma** (`ARDUINO_AVR_GEMMA`)
- **Arduino Leonardo** (`ARDUINO_AVR_LEONARDO`)
- **Arduino Micro** (`ARDUINO_AVR_MICRO`)
- **Arduino Yun** (`ARDUINO_AVR_YUN`)

### AtMega32u4 Processor Information

[Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet-Summary.pdf)

[Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet.pdf)

- An external RTC (DS3231, DS3232, or RV8803) is required.
- There is a single SPI port on pins 14 (MISO), 15 (SCK), and 16 (MOSI).
  Chip select/slave select and card detect pins vary by board.
- There is a single I2C (Wire) interface on pins 2 (SDA) and 3 (SCL).
- This processor has one hardware serial port, Serial, which can _only_ be used for USB communication with a computer
- There is one additional hardware serial port, Serial1, which can communicate with any serial device.
- AltSoftSerial can be used on pins 5 (Tx) and 13 (Rx).
- Only pins 8, 9, 10, 11, 14, 15, and 16 can be used with NeoSWSerial, SoftwareSerial_ExtInts, or SDI-12.
  (And pins 14, 15, and 16 will be eliminated if you are using any SPI devices (like an SD card).)
- Because the USB controller is built into the processor, your USB serial connection will close as soon as the processor goes to sleep.
  If you need to debug, I recommend using a serial port monitor like [Tera Term](https://teratermproject.github.io/index-en.html) which will automatically renew its connection with the serial port when it connects and disconnects.
  Otherwise, you will have to rely on lights on your alert pin or your modem to verify the processor is waking/sleeping properly.

___

## Unsupported Processors<!--! {#processor_unsupported} -->

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
