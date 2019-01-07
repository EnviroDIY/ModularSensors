# Example using the Modular Sensors Library to save data to an SD card

This shows the simplest use of a "logger" object.  That is, creating an array of variable objects and then creating a logger object that utilizes those variables to update all of the variable results together and save the data to a SD card.  The processor then goes to sleep between readings.  This example calls on at least one of every single sensor available in this library.  Please do not try to run the example exactly as written, but delete the chunks of code pertaining to sensors that you do not have attached.

This is the example you should use to deploy a logger somewhere where you don't want or have access to a way of streaming live data and you won't want to upload data to the WikiWatershed data portal.
