[//]: # ( @page double_log_example Double %Logger Example )
# Using ModularSensors to Record data from Two Different Groups of Sensors at Two Different Time Intervals

This is a more complicated example using two different logger instances to log data at two different intervals, in this case, an AM3215 logging every minute, while checking the battery voltage only every 5 minutes.
This showcases both how to use two different logging instances and how to use some of the functions to set up your own logging loop rather than using the logData() function.
