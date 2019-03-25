# Example using the Modular Sensors Library to log data to EnviroDIY/Monitor My Watershed

This sketch reduces menu_a_la_carte.ino to provide an example of how to log to https://monitormywatershed.org/ from two sensors, the BME280 and DS18. To complete the set up for logging to the web portal, the UUIDs for the site and each variable would need to be added to the sketch.

The settings for other data portals were removed from the example.

The modem settings were left unchanged because the sketch will test successfully without modem connection (wait patiently, it takes a few minutes).

This is the example you should use to deploy a logger with a modem to stream live data to the Monitor My Watershed data portal.
