# Notes about Sensors<!--! {#page_sensor_notes} -->

There are a number of sensors supported by this library.
Depending on the sensor, it may communicate with the Arduino board using as a serial peripheral interface (SPI), inter-integrated circuit (I2C, also called "Wire," "Two Wire", or "TWI"), or some type of universal synchronous/asynchronous receiver/transmitter (UART/USART, or simply "serial") protocol.
(USART or serial includes transistor-transistor logic (TTL), RS232 (adapter needed), and RS485 (adapter needed) communication).
See the section on [Processor Compatibility](https://envirodiy.github.io/ModularSensors/page_processor_compatibility.html) for more specific notes on which pins are available for each type of communication on the various supported processors.

Essentially all of the sensors can have their power supplies turned off between readings, but not all boards are able to switch output power on and off.
When the sensor constructor asks for the Arduino pin controlling power on/off, use -1 for any board which is not capable of switching the output power on and off.

Please, please, when setting up multiple sensors on a logger, be smart about it.
Don't try to connect too many sensors all at once or you're likely to exceed your logger's power regulator or come across strange interferences between them.
_**TEST YOUR LOGGER WITH ALL SENSORS ATTACHED BEFORE DEPLOYING IT TO THE FIELD!**_
Don't even think about skipping the in-lab testing!
Theoretically every single sensor possible could be attached to the same processor, but the reality is that boards have finite numbers of pins, solar panels can only create so much charge, and not all sensors like each other very much.
