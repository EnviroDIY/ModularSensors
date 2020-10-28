[//]: # ( @page example_double_log Double %Logger Example )
# Using ModularSensors to Record data from Two Different Groups of Sensors at Two Different Time Intervals

This is a more complicated example using two different logger instances to log data at two different intervals, in this case, an AM3215 logging every minute, while checking the battery voltage only every 5 minutes.
This showcases both how to use two different logging instances and how to use some of the functions to set up your own logging loop rather than using the logData() function.

_______

[//]: # ( @section example_double_log_walk Walking Through the Code )
# Walking Through the Code

_NOTE:  The code snippets in this walkthrough will not appear on GitHub._

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Using ModularSensors to Record data from Two Different Groups of Sensors at Two Different Time Intervals](#using-modularsensors-to-record-data-from-two-different-groups-of-sensors-at-two-different-time-intervals)
- [Walking Through the Code](#walking-through-the-code)
  - [PlatformIO Configuration](#platformio-configuration)
  - [The Complete Code](#the-complete-code)

[//]: # ( End GitHub Only )


[//]: # ( @section example_double_log_pio PlatformIO Configuration )
## PlatformIO Configuration

[//]: # ( @include{lineno} double_logger/platformio.ini )

[//]: # ( @section example_double_log_code The Complete Code )
## The Complete Code
