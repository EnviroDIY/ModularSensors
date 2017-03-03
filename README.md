# ModularSensors

A "library" of sensors to give all sensors a common interface of functions.  This library was written primarily for the [EnviroDIY Mayfly](https://envirodiy.org/mayfly/) but should be applicable to other Arduino based boards as well.

Each sensor is implemented as a subclass of the "SensorBase" class.  Within each sensor, there are subclasses for each variable that the sensor can return.  At this time, all sensors return values as floats.

To use a sensor in your sketch, you must include SensorBase.h in your script AND separately include xxx.h for each sensor you intend to use.  While this may force you to write many more include statements, it makes the library much lighter weight by not requiring you to install the functions for every sensor when only one is needed.

### These are the functions available for each sensor:
- setup(void) - This "sets up" the sensor - setting up serial ports, etc required for the given sensor.  This is always called when creating a new sensor instance.
- getStatus(void) - This returns the current status of the sensor, if the sensor has some way of giving it to you.  (Most do not).
- sleep(void) - This puts the sensor to sleep, often by stopping the power.  Returns true.
- wake(void) - This wakes the sensor up and sends it power.  Returns true.
- getSensorName(void) - This gets the name of the sensor and returns it as a string.
- getSensorLocation(void) - This returns the data pin or other sensor installation information as a string.
- update(void) - This updates the sensor values and returns true when finished.  For digital sensors with a single infomation return, this only needs to be called once for each sensor, even if there are multiple variable subclasses for the sensor.

### These are the functions for each variable returned by a sensor
- getVarName(void) - This returns the variable's name using http://vocabulary.odm2.org/variablename/ as a string
- getVarUnit(void) - This returns the variable's unit using http://vocabulary.odm2.org/units/ as a string
- getValue(void) - This returns the current value of the variable as a float.  You should call the update function before calling getValue.  If the getValue function sees that the update function has not been called within the last 30 seconds, it will re-call it.


## Available sensors

**Mayfly Onboard Sensors**

The version of the Mayfly is required as input (ie, "v0.3" or "v0.4")  You must have the [Sodaq](https://github.com/SodaqMoja/Sodaq_DS3231) or [EnviroDIY DS-3231](https://github.com/EnviroDIY/Sodaq_DS3231) library installed to use this sensor.  Because the sensors operate independently, you must call the update function for each one before calling getValue.
- MayflyOnboardTemp(char const *version)
- MayflyOnboardBatt(char const *version)
- MayflyFreeRam(char const *version)

**[MaxBotix MaxSonar](http://www.maxbotix.com/Ultrasonic_Sensors/High_Accuracy_Sensors.htm) - WR or WRS Series with TTL Outputs**

The power/excite pin and digital data pin are needed as input.  The power pin must provide smoothed digital power.  You must have SoftwareSerial or the [EnviroDIY modified version](https://github.com/EnviroDIY/SoftwareSerialMod) of it installed to use this sensor.
- MaxBotixSonar_Depth(int powerPin, int dataPin)

**[Campbell Scientific OBS-3+](https://www.campbellsci.com/obs-3plus)**

The power pin, analog data pin, and calibration values for Ax^2 + Bx + C are required as inputs and the sensor must be attached to a TI ADS1115 ADD converter (such as on the analog pins of the Mayfly).  You must have the Adafruit ADS1015 library installed to use this sensor.

Note that to access both the high and low range returns, two instances must be created, one at the low range return pin and one at the high pin.
- CampbellOBS3_Turbidity(int powerPin, int dataPin, float A, float B, float C)

**[Decagon Devices 5TM Soil Moisture and Temperature Sensor](https://www.decagon.com/en/soils/volumetric-water-content-sensors/5tm-vwc-temp/)**

The SDI-12 address of the sensor, the power pin, and the data pin are required as inputs.  You must have the [EnviroDIY Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12) installed to use this sensor.  Calling the update function for any one of the three variables will update all three.

- Decagon5TM_Ea(char TMaddress, int powerPin, int dataPin)
- Decagon5TM_Temp(char TMaddress, int powerPin, int dataPin)
- Decagon5TM_VWC(char TMaddress, int powerPin, int dataPin)

**[Decagon Devices CTD-10](https://www.decagon.com/en/hydrology/water-level-temperature-electrical-conductivity/ctd-10-sensor-electrical-conductivity-temperature-depth/)**

The SDI-12 address of the sensor, the power pin, and the data pin are required as inputs.  You must have the [EnviroDIY Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12) installed to use this sensor.  Calling the update function for any one of the three variables will update all three.

- DecagonCTD_Cond(int numReadings, char CTDaddress, int powerPin, int dataPin)
- DecagonCTD_Temp(int numReadings, char CTDaddress, int powerPin, int dataPin)
- DecagonCTD_Depth(int numReadings, char CTDaddress, int powerPin, int dataPin)
