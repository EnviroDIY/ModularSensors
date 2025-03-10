# Extra Helper Sketches<!--! {#page_extra_helper_sketches} -->

A collection of helper sketches to test individual sensor timing and configurations.

- powerOn.ino
  - Testing sketch that simply turns on power to the sensors on the Mayfly.
- oneWireSearch.ino
  - Testing sketch to scan for 1-Wire devices + code snippet generator
- i2c_scanner.ino
  - Testing sketch to scan for attached I2C devices
- i2c_warmUp.ino
  - Testing sketch to see how long an attached I2C device takes to begin to respond to commands.
- interrupt_counter.ino
  - Testing sketch counting pin change interrupts.
- resetBee.ino
  - Testing sketch to fully reset an XBee
- LTExBee_FirstConnection.ino
  - Testing sketch to set up a never-previously-connected LTE XBee running in standard (transparent) mode.
- LTExBee_FirstConnectionBypass.ino
  - Testing sketch to set up a never-previously-connected LTE XBee running in bypass mode.
- mega_serial_spy.ino
  - Testing sketch to run on an Arduino Mega to print all output from connected serial ports to the terminal.
- sdi12_address_change.ino
  - Copy of SDI-12 Example B: Changing the Address of your SDI-12 sensor
- Stream_Debug.ino
  - Testing sketch to run StreamDebugger to copy text from one serial output to another.
