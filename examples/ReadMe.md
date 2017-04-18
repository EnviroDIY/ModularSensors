# Examples using the Modular Sensors Library

These example programs demonstrate how to  use the modular sensors library:

### single_sensor.ino
This shows making use of the unified set of commands to print data from a MaxBotix ultrasonic range finder to the serial port.

### multisensor_print.ino
This shows using an array of sensors to easily update all of them and print all results to the serial port.  This example calls on a MaxBotix ultrasonic range finder, a Decagon CTD, a Decagon 5TM, a Decagon ES2, a Campbell OBS3+, and the sensors built into the EnviroDIY Mayfly logger.

### simple_logging.ino
This calls the same group of sensors as in multisensor_print.ino but now records the data to a SD card instead of simply printing it to the serial port.

### logging_to_EnviroDIY.ino
This calls the same group of sensors as in multisensor_print.ino and simple_logging.ino and sends the data to the EnviroDIY data portal in addition to logging it to an SD card.
