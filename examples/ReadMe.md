# Examples using the Modular Sensors Library

These example programs demonstrate how to  use the modular sensors library.

To adjust any of these to work with your own sensor arrangements:
1. Set your real time clock to the correct UTC time.
2. Select the sketch that most closely matches what you intend to do.
3. If applicable, select the correct "modem" that you will use to connect your logger to the internet.
4. Fill out your SKETCH_NAME, LoggerID, LOGGING_INTERVAL, and TIME_ZONE.
5. Remove the chunks of code from the top of the program that apply to extranous sensors.
6. Duplicate the sensor specific chunks of code for any sensors you have more than one of.
7. Correct all of the sensor power and data pins and sensor addresses to match your actual sensors and logger board attachment points.
8. Delete or add lines from the variableList[] to match your true arrangement.
9. If logging data to EnviroDIY, register your site and sensors at http://data.envirodiy.org/.
10. After registering your site and sensors, copy the "code snippet" from your site into the correct block in the example.
11. If applicable, fill out your modem pins, sleep mode, serial port, and connection options.
12. Fill out your logger board information, including the serial baud rate and LED pins.
13. Verify that the setup() and loop() functions are doing what you expect.
14. Program your board!

### single_sensor.ino
This shows making use of the unified set of commands to print data from a MaxBotix ultrasonic range finder to the serial port.

### multisensor_print.ino
This shows using an array of sensors to easily update all of them and print all results to the serial port.  This example calls on at least one of every single sensor available in this library.

### simple_logging.ino
This calls the same group of sensors as in multisensor_print.ino but now records the data to a SD card instead of simply printing it to the serial port.

### logging_to_EnviroDIY.ino
This calls the same group of sensors as in multisensor_print.ino and simple_logging.ino and sends the data to the EnviroDIY data portal in addition to logging it to an SD card.
