[//]: # ( @page example_mmw Monitor My Watershed Example )
# Using ModularSensors to log data to Monitor My Watershed/EnviroDIY

This sketch reduces menu_a_la_carte.ino to provide an example of how to log to https://monitormywatershed.org/ from two sensors, the BME280 and DS18. To complete the set up for logging to the web portal, the UUIDs for the site and each variable would need to be added to the sketch.

The settings for other data portals were removed from the example.

The modem settings were left unchanged because the sketch will test successfully without modem connection (wait patiently, it takes a few minutes).

This is the example you should use to deploy a logger with a modem to stream live data to the Monitor My Watershed data portal.

_______

[//]: # ( @section example_mmw_walk Walking Through the Code )
# Walking Through the Code

_NOTE:  The code snippets in this walkthrough will not appear on GitHub._

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to log data to Monitor My Watershed/EnviroDIY](#using-modularsensors-to-log-data-to-monitor-my-watershedenvirodiy)
- [Walking Through the Code](#walking-through-the-code)
  - [PlatformIO Configuration](#platformio-configuration)
  - [The Complete Code](#the-complete-code)

[//]: # ( End GitHub Only )


[//]: # ( @section example_mmw_pio PlatformIO Configuration )
## PlatformIO Configuration

[//]: # ( @include{lineno} logging_to_MMW/platformio.ini )

[//]: # ( @section example_mmw_code The Complete Code )
## The Complete Code
