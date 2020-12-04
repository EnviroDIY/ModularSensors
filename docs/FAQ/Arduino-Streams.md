[//]: # ( @page page_arduino_streams Arduino Streams and Software Serial )
## Notes on Arduino Streams and Software Serial

In this library, the Arduino communicates with the computer for debugging, the modem for sending data, and some sensors (like the [MaxBotix MaxSonar](https://github.com/EnviroDIY/ModularSensors/wiki/MaxBotix-MaxSonar)) via instances of Arduino TTL "[streams](https://www.arduino.cc/en/Reference/Stream)..
The streams can either be an instance of [serial](https://www.arduino.cc/en/Reference/Serial) (aka hardware serial), [AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial), [the EnviroDIY modified version of SoftwareSerial](https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts), or any other stream type you desire.
The very commonly used build-in version of the software serial library for AVR processors uses interrupts that conflict with several other sub-libraries or this library and _cannot be used_.
I repeat.
**_You cannot use the built-in version of SoftwareSerial!_**.
You simply cannot.
It will not work.
Period.
This is not a bug that will be fixed.

For stream communication, **hardware serial** should _always_ be your first choice, if your processor has enough hardware serial ports.
Hardware serial ports are the most stable and have the best performance of any of the other streams.
Hardware serial ports are also the only option if you need to communicate with any device that uses even or odd parity, more than one stop bit, or does not use 8 data bits.
(That is, hardware serial ports are the only way to communicate with a device that doesn't use the 8N1 configuration.
Again, _always use a hardware serial port for communication if possible!_

If the [proper pins](https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) are available, **[AltSoftSerial](https://github.com/PaulStoffregen/AltSoftSerial)** by Paul Stoffregen is also superior to SoftwareSerial, especially at slow baud rates.
Neither hardware serial nor AltSoftSerial require any modifications.
Because of the limited number of serial ports available on most boards, I suggest giving first priority (i.e. the first (or only) hardware serial port, "Serial") to your debugging stream going to your PC (if you intend to debug), second priority to the stream for the modem, and third priority to any sensors that require a stream for communication.
See the section on [Processor/Board Compatibility](https://envirodiy.github.io/ModularSensors/processor_compatibility.html) for more specific notes on what serial ports are available on the various supported processors.

Another possible serial port emulator is [NeoSWSerial](https://github.com/SRGDamia1/NeoSWSerial).
While not as stable as AltSoftSerial, it supports using any pin with pin change interrupts for communication.
To use NeoSWSerial, you must add the line `-D NEOSWSERIAL_EXTERNAL_PCINT` to the build flags section of your platformio.ini file (or open the NeoSWSerial.h file and find and remove the two slashes from the start of the line `//#define NEOSWSERIAL_EXTERNAL_PCINT`) and then recompile the library.
There are instructions in the NeoSWSerial ReadMe on how to use EnableInterrupt to activate NeoSWSerial.
Note that NeoSWSerial is generally incompatible with the SDI-12 communication library on most 8MHz processors (including the EnviroDIY Mayfly).
The two libraries can be compiled together, but because they are in competition for a timer, they cannot be used together.
The way this (ModularSensors) uses the SDI-12 library resets the timer settings when ending communication, so you may be able to use the two libraries together if the communication is not simultaneous.
Please test your configuration before deploying it!

To use a hardware serial stream, you do not need to include any libraries or write any extra lines.
You can simply write in "Serial#" where ever you need a stream.
If you would like to give your hardware serial port an easy-to-remember alias, you can use code like this:

```cpp
HardwareSerial* streamName = &Serial;
```

To use AltSoftSerial:

```cpp
#include <AltSoftSerial.h.
// include the AltSoftSerial library
AltSoftSerial streamName.
// Create an instance of AltSoftSerial
```

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

Additionally, for the EnviroDIY modified version of SoftwareSerial, (or NeoSWSerial) you must enable the interrupts in your setup function:

```cpp
// Allow enableInterrrupt to control the interrupts for software serial
enableInterrupt(rx_pin, SoftwareSerial_ExtInts::handle_interrupt, CHANGE);
```

Example code for creating more serial ports on an Adafruit feather M0 using the SERCOMs is available here.
https://github.com/EnviroDIY/ModularSensors/wiki/SAMD21-SERCOMs and further examples in this wiki here.
https://github.com/EnviroDIY/ModularSensors/wiki/SAMD21-SERCOMs

Here are some helpful links for more information about the number of serial ports available on some of the different Arduino-style boards:

- For Arduino brand boards.
[https://www.arduino.cc/en/Reference/Serial](https://www.arduino.cc/en/Reference/Serial)
- For AtSAMD21 boards.
[https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview](https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview)
