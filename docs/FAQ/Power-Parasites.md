# Power Draw over Data Lines<!--! {#page_power_parasites} -->

When deploying a logger out into the wild and depending on only battery or solar charging, getting the power draw from sensors to be as low as possible is crucial.
This library assumes that the main power/Vcc supply to each sensor can be turned on by setting its powerPin high and off by setting its powerPin low.
For most well-designed sensors, this should stop all power draw from the sensor.
Real sensors, unfortunately, aren't as well designed as one might hope and some sensors (and particularly RS485 adapters) can continue to suck power from by way of high or floating data pins.
For most sensors, this library attempts to set all data pins low when sending the sensors and then logger to sleep.

If you are still seeing "parasitic" power draw, here are some work-arounds you can try:

- For sensors (and adapters) drawing power over a serial line:
  - Write-out your entire loop function.
(Don't just use `logData()`.)
  - Add a `SerialPortName.begin(BAUD);` statement to the beginning of your loop, before `sensorsPowerUp()`.
  - After `sensorsPowerDown()` add `SerialPortName.end(BAUD);`.
  - After "ending" the serial communication, explicitly set your Rx and Tx pins low using `digitalWrite(#, LOW);`.
- For sensors drawing power over I2C:
  - Many (most?) boards have external pull-up resistors on the hardware I2C/Wire pins which cannot be disconnected from the main power supply.
This means I2C parasitic power draw is best solved via hardware, not software.
  - Use a specially designed I2C isolator
  - Use a generic opto-isolator or other type of isolator on both the SCL and SDA lines
  - In this future, this library _may_ offer the option of using software I2C, which would allow you to use the same technique as is currently usable to stop serial parasitic draw.
Until such an update happens, however, hardware solutions are required.

The ["data_saving"](@todo add link to loop of data saving example) example shows setting ending a serial stream and setting pins low to prevent an RS485 adapter from drawing power during sleep.
