# Examples using the Modular Sensors Library

These example programs demonstrate how to  use the modular sensors library.  Each example has slightly different functionality.  If you are unsure which to use, the "menu_a_la_carte" example has code in it for every possible sensor and modem.  You can start with it and delete as necessary to cut down to only what you will be using.


### [menu_a_la_carte](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/menu_a_la_carte)

This shows most of the standard functions of the library at once.  It has code in it for every possible sensor and modem and for both AVR and SAMD boards.  It is also nearly 1500 lines long.


### [single_sensor](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/single_sensor)

This shows making use of the unified set of commands to print data from a MaxBotix ultrasonic range finder to the serial port.  It also shows creating a calculated variable which is the water depth.


### [simple_logging](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/simple_logging)

This shows how to create multiple sensors, create variables for the sensors in a variable array, and log the data from the sensors to an SD card.

### [DWRI_CitSci](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/)

This is examples uses the sensors and equipment standard to groups participating in the DWRI Citizen Science project with the Stroud Water Research Center (a CTD and turbidity sensor).  The results are saved to the SD card and posted to the WikiWatershed data portal via a Sodaq GPRSBee.


### [DWRI_NoCellular](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/DWRI_NoCellular)

This is uses the sensors and equipment standard to the DWRI Citizen Science grant but omits the data publisher for circumstances where there is no cellular signal.


### [logging_to_ThingSpeak](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/logging_to_ThingSpeak)
    This sends data to ThingSpeak instead of the WikiWatershed Data Portal.


### [baro_rho_correction](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/baro_rho_correction)

This example demonstrates how to work with calculated variables and calculates water depth by correcting the total pressure measured by a MeaSpec MS5803 with the atmospheric pressure measured by a Bosch BME280 environmental sensor and the temperature measured by a Maxim DS18 temperature probe.


### [double_logger](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/double_logger)

This is a more complicated example using two different logger instances to log data at two different intervals, in this case, an AM3215 logging every minute, while checking the battery voltage only every 5 minutes.  This showcases both how to use two different logging instances and how to use some of the functions to set up your own logging loop rather than using the logData() function.


### [data_saving](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/)

This is another double logger example, but in this case, both loggers are going at the same interval and the only difference between the loggers is the list of variables.  There are two sets of variables, all coming from Yosemitech sensors.  Because each sensor outputs temperature and we don't want to waste cellular data sending out multiple nearly identical temperature values, we have one logger that logs every possible variable result to the SD card and another logger that sends only unique results to the EnviroDIY data portal.  This example also shows how to stop power draw from an RS485 adapter with automatic flow detection.
