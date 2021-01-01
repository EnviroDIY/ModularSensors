[//]: # ( @page page_arduino_streams Arduino Streams and Software Serial )
## Notes on Arduino Streams and Software Serial

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Notes on Arduino Streams and Software Serial](#notes-on-arduino-streams-and-software-serial)
- [Hardware Serial](#hardware-serial)
- [AltSoftSerial](#altsoftserial)
- [NeoSWSerial](#neoswserial)
- [Neutered SoftwareSerial](#neutered-softwareserial)
- [SAMD SERCOMs](#samd-sercoms)

[//]: # ( End GitHub Only )

In this library, the Arduino communicates with the computer for debugging, the modem for sending data, and some sensors (like the [MaxBotix MaxSonar](https://github.com/EnviroDIY/ModularSensors/wiki/MaxBotix-MaxSonar)) via instances of Arduino TTL [streams](https://www.arduino.cc/en/Reference/Stream).
The streams can either be an instance of
- [serial (hardware serial)](https://www.arduino.cc/en/Reference/Serial),
- [AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial),
- [NeoSWSerial](https://github.com/SRGDamia1/NeoSWSerial),
- [the EnviroDIY modified version of SoftwareSerial](https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts),
- or any other stream type you desire.
The very commonly used build-in version of the software serial library for AVR processors uses interrupts that conflict with several other sub-libraries or this library and _cannot be used_.
I repeat.
_**You cannot use the built-in version of SoftwareSerial!**_.
You simply cannot.
It will not work.
Period.
This is not a bug that will be fixed.

[//]: # ( @section page_arduino_hw Hardware Serial )
## Hardware Serial

For stream communication, **hardware serial** should _always_ be your first choice, if your processor has enough hardware serial ports.
Hardware serial ports are the most stable and have the best performance of any of the other streams.
Hardware serial ports are also the only option if you need to communicate with any device that uses even or odd parity, more than one stop bit, or does not use 8 data bits.
(That is, hardware serial ports are the only way to communicate with a device that doesn't use the 8N1 configuration.
Again, _**always use a hardware serial port for communication if possible!**_

To use a hardware serial stream, you do not need to include any libraries or write any extra lines.
You can simply write in "Serial#" where ever you need a stream.
If you would like to give your hardware serial port an easy-to-remember alias, you can use code like this:

```cpp
HardwareSerial* streamName = &Serial;
```


[//]: # ( @section page_arduino_altss AltSoftSerial )
## AltSoftSerial

If the [proper pins](https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) are available, **[AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial)** by Paul Stoffregen is also superior to SoftwareSerial, especially at slow baud rates.
Neither hardware serial nor AltSoftSerial require any modifications.
Because of the limited number of serial ports available on most boards, I suggest giving first priority (i.e. the first (or only) hardware serial port, "Serial") to your debugging stream going to your PC (if you intend to debug), second priority to the stream for the modem, and third priority to any sensors that require a stream for communication.
See the section on [Processor/Board Compatibility](https://envirodiy.github.io/ModularSensors/page_processor_compatibility.html) for more specific notes on what serial ports are available on the various supported processors.

To use AltSoftSerial:

```cpp
#include <AltSoftSerial.h.
// include the AltSoftSerial library
AltSoftSerial streamName.
// Create an instance of AltSoftSerial
```


[//]: # ( @section page_arduino_neosw NeoSWSerial )
## NeoSWSerial

Another possible serial port emulator is [NeoSWSerial](https://github.com/SRGDamia1/NeoSWSerial).
While not as stable as AltSoftSerial, it supports using any pin with pin change interrupts for communication.
To use NeoSWSerial, you must add the line `-D NEOSWSERIAL_EXTERNAL_PCINT` to the build flags section of your platformio.ini file (or open the NeoSWSerial.h file and find and remove the two slashes from the start of the line `//#define NEOSWSERIAL_EXTERNAL_PCINT`) and then recompile the library.
There are instructions in the NeoSWSerial ReadMe on how to use EnableInterrupt to activate NeoSWSerial.
Note that NeoSWSerial is generally incompatible with the SDI-12 communication library on most 8MHz processors (including the EnviroDIY Mayfly).
The two libraries can be compiled together, but because they are in competition for a timer, they cannot be used together.
The way this (ModularSensors) uses the SDI-12 library resets the timer settings when ending communication, so you may be able to use the two libraries together if the communication is not simultaneous.
Please test your configuration before deploying it!

After correctly compiling NeoSWSerial, to use it:

```cpp
#include <NeoSWSerial.h>          // for the stream communication
const int8_t neoSSerial1Rx = 11;  // data in pin
const int8_t neoSSerial1Tx = -1;  // data out pin
NeoSWSerial  neoSSerial1(neoSSerial1Rx, neoSSerial1Tx);
// To use NeoSWSerial in this library, we define a function to receive data
// This is just a short-cut for later
void neoSSerial1ISR() {
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(neoSSerial1Rx)));
}
```

After creating the stream instances, you must always remember to "begin" your stream within the main setup function.

```cpp
streamName.begin(BAUD_RATE);
```

Additionally, for the EnviroDIY modified version of SoftwareSerial, (or NeoSWSerial) you must enable the interrupts in your setup function:

```cpp
// Allow enableInterrrupt to control the interrupts for software serial
enableInterrupt(rx_pin, neoSSerial1ISR, CHANGE);
```


[//]: # ( @section page_arduino_ss Neutered SoftwareSerial )
## Neutered SoftwareSerial

[The EnviroDIY modified version of SoftwareSerial](https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts) removes direct interrupt control from the SoftwareSerial library, making it dependent on another interrupt library, but able to be compiled with ModularSensors.
This is, by far, the least stable serial port option and should only be used on sensors that are not very picky about the quality of the serial stream or that only require one-way communication (ie, only posting data rather than needing to receive commands).

To use the EnviroDIY modified version of SoftwareSerial:

```cpp
#include <SoftwareSerial_ExtInts.h.
// include the SoftwareSerial library
SoftwareSerial_ExtInts streamName(tx_pin, rx_pin);
```

After creating the stream instances, you must always remember to "begin" your stream within the main setup function.

```cpp
streamName.begin(BAUD_RATE);
```

Additionally, for the EnviroDIY modified version of SoftwareSerial, you must enable the interrupts in your setup function:

```cpp
// Allow enableInterrrupt to control the interrupts for software serial
enableInterrupt(rx_pin, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
```


[//]: # ( @section page_arduino_samd SAMD SERCOMs )
## SAMD SERCOMs

Example code for creating more serial ports on an Adafruit feather M0 using the SERCOMs is available [in the menu a la carte example](https://envirodiy.github.io/ModularSensors/menu_a_la_carte_8ino-example.html#enu_samd_serial_ports).

Here are some helpful links for more information about the number of serial ports available on some of the different Arduino-style boards:

- For Arduino brand boards.
[https://www.arduino.cc/en/Reference/Serial](https://www.arduino.cc/en/Reference/Serial)
- For AtSAMD21 boards.
[https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview](https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview)
