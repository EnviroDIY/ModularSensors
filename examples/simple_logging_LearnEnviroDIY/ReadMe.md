[//]: # ( @page example_learn_envirodiy Learn EnviroDIY Example )
# Using ModularSensors to save data to an SD card

This shows the simplest use of a "logger" object.
That is, creating an array of variable objects and then creating a logger object that utilizes those variables to update all of the variable results together and save the data to a SD card.
The processor then goes to sleep between readings.
This example calls on two of the sensors available in this library.
The example may be run exactly as written.

This is the example you should use to deploy a logger somewhere where you don't want or have access to a way of streaming live data and you won't want to upload data to the Monitor My Watershed data portal.

_______

[//]: # ( @section simple_logging_diy_full The Complete Example Code: )
