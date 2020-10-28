[//]: # ( @page example_data_saving Data Saving Example )
# Using ModularSensors to Record data from Many Variables but Only Send a Portion to the EnviroDIY Data Portal

This is another double logger example, but in this case, both loggers are going at the same interval and the only difference between the loggers is the list of variables.
There are two sets of variables, all coming from Yosemitech sensors.
Because each sensor outputs temperature and we don't want to waste cellular data sending out multiple nearly identical temperature values, we have one logger that logs every possible variable result to the SD card and another logger that sends only unique results to the EnviroDIY data portal.

The modem used in this example is a SIM800 based Sodaq GPRSBee r6.

The sensors used in this example are Yosemitech Y504 Dissolved Oxygen Sensor, Yosemitech Y511 Turbidity Sensor with Wiper, Yosemitech Y514 Chlorophyll Sensor, and Yosemitech Y520 Conductivity Sensor.

_______

[//]: # ( @section example_data_saving_walk Walking Through the Code )
# Walking Through the Code

_NOTE:  The code snippets in this walkthrough will not appear on GitHub._

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to Record data from Many Variables but Only Send a Portion to the EnviroDIY Data Portal](#using-modularsensors-to-record-data-from-many-variables-but-only-send-a-portion-to-the-envirodiy-data-portal)
- [Walking Through the Code](#walking-through-the-code)
  - [PlatformIO Configuration](#platformio-configuration)
  - [The Complete Code](#the-complete-code)

[//]: # ( End GitHub Only )


[//]: # ( @section example_data_saving_pio PlatformIO Configuration )
## PlatformIO Configuration

[//]: # ( @include{lineno} data_saving/platformio.ini )

[//]: # ( @section example_data_saving_code The Complete Code )
## The Complete Code
