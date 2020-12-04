[//]: # ( @page page_physical_dependencies Physical Dependencies )
# Physical Dependencies

This library is designed for wireless, solar-powered environmental data logging applications, that is, to log data from many physical sensors and to put the processor and all peripherals to sleep to conserver power between readings.
The most banal functions of the library require only an AVR or SAMD processor, but making real use of this library requires:

- A sufficiently powerful AVR or SAMD processor mounted on some sort of circuit board.
(See [Processor/Board Compatibility](https://envirodiy.github.io/ModularSensors/processor_compatibility.html) for more details on specific processors and boards that are supported.)
    - For all AVR processors, you must also have a [Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) high precision I2C real-time clock with the SQE/INT pin connected to a pin on your processor which supports either external or pin-change interrupts.
    - For SAMD boards, this library makes use of their on-board (though less accurate) real-time clock.
- A SD card reader attached to the processor via SPI.
- Environmental sensors
- A battery to power the system
- A solar charging circuit
- A modem-type unit to communicate remote data (Optional for logging data, but required for sending data directly to the internet.
    - The list of supported modems is available here:  https://envirodiy.github.io/ModularSensors/index.html#mainpage_modems
- Protected water-proof enclosures and mountings for all of the above
- An OTG cable to connect serial output from the board to a cell phone (Optional, but very helpful for debugging.)
