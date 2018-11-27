# Examples using the Modular Sensors Library

These example programs demonstrate how to  use the modular sensors library.

To adjust any of these to work with your own sensor arrangements:
1. Set your real time clock to the correct _UTC_ time.
2. Select the sketch that most closely matches what you intend to do.
3. If applicable, select the correct "modem" that you will use to connect your logger to the internet.
4. Fill out your SKETCH_NAME, LoggerID, LOGGING_INTERVAL, and TIME_ZONE.
5. Remove the chunks of code that apply to extraneous sensors.
6. Duplicate the sensor specific chunks of code for any sensors you have more than one of.
7. Correct all of the pin numbers, stream id's, and sensor addresses to match your actual sensors and logger board attachment points.
8. Delete or add lines from the variableList[] to match your true arrangement.
9. If logging data to EnviroDIY, register your site and sensors at http://data.envirodiy.org/.
10. After registering your site and sensors, copy the UUID's and tokens from your site into the correct places in the example.
11. If applicable, fill out your modem pins, sleep mode, serial port, and connection options.
12. Fill out your logger board information, including the serial baud rate and LED pins.
13. Verify that the setup() and loop() functions are doing what you expect.
14. Program your board!

### single_sensor.ino
This shows making use of the unified set of commands to print data from a MaxBotix ultrasonic range finder to the serial port.  It also shows creating a calculated variable which is the water depth.  

### multisensor_print.ino
This shows using an array of sensors to easily update all of them and print all results to the serial port.  This example calls on at least one of every single sensor available in this library.

### simple_logging.ino
This calls the same group of sensors as in multisensor_print.ino but now records the data to a SD card instead of simply printing it to the serial port.

### logging_to_EnviroDIY.ino
This calls the same group of sensors as in multisensor_print.ino and simple_logging.ino and sends the data to the EnviroDIY data portal in addition to logging it to an SD card.

### logging_to_EnviroDIY_Zero.ino
Identical to logging_to_EnviroDIY, but written for an Adafruit Feather M0 (SAMD21) processor.

### DWRI_CitSci.ino
This is a simplified version of logging_to_EnviroDIY.ino using just the sensors and equipment standard to the DWRI Citizen Science grant.

### DWRI_NoCellular.ino
This is uses just the sensors and equipment standard to the DWRI Citizen Science grant for circumstances where there is no cellular signal.

### baro_rho_correction.ino
This example demonstrates how to work with calculated variables.

### double_logger.ino
This is a more complicated example using two different logger instances to log data at two different intervals, in this case, an AM3215 logging every minute, while checking the battery voltage only every 5 minutes.  This showcases both how to use two different logging instances and how to use some of the functions to set up your own logging loop rather than using the logData() function.

### data_saving.ino
This is another double logger example, but in this case, both loggers are going at the same interval and the only difference between the loggers is the list of variables.  There are two sets of variables, all coming from Yosemitech sensors.  Because each sensor outputs temperature and we don't want to waste cellular data sending out multiple nearly identical temperature values, we have one logger that logs every possible variable result to the SD card and another logger that sends only unique results to the EnviroDIY data portal.
