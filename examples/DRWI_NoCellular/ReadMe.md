# Examples using the Modular Sensors Library for DRWI sites with no Cellular Service

This is the code example that should be used for all groups working with the Stroud Water Research Center within the Delaware River Watershed Initiative.  This example should be used in cases where no cellular service of any kind is available and the data will only be logged on the SD card.

____

### Customizing this example sketch:


#### Set the logger ID

- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

#### Set the calibration coefficients for the Campbell OBS3+

 - The OBS3+ ships with a calibration certificate; you need this sheet!
 - Change _**all**_ of the the ```xxxxE+xx``` values in this section of code to the values on that calibration sheet.  Use numbers from the side of the calibration sheet that shows the calibration in _**volts**_.
    - The sketch will not compile if these values are not entered properly.
    - Do not change any values except those that are ```xxxxE+xx```!

```cpp
// ==========================================================================
//    CAMPBELL OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <CampbellOBS3.h>
const int8_t OBS3Power = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t OBS3numberReadings = 10;
const uint8_t OBS3_ADS1115Address = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ Low Range calibration in Volts
const int8_t OBSLowPin = 0;  // The low voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSLow_A = xxxxE+xx;  // The "A" value (X^2) from the low range calibration
const float OBSLow_B = xxxxE+xx;  // The "B" value (X) from the low range calibration
const float OBSLow_C = xxxxE+xx;  // The "C" value from the low range calibration
CampbellOBS3 osb3low(OBS3Power, OBSLowPin, OBSLow_A, OBSLow_B, OBSLow_C, OBS3_ADS1115Address, OBS3numberReadings);
// Campbell OBS 3+ High Range calibration in Volts
const int8_t OBSHighPin = 1;  // The high voltage analog pin ON THE ADS1115 (NOT the Arduino Pin Number)
const float OBSHigh_A = xxxxE+xx;  // The "A" value (X^2) from the high range calibration
const float OBSHigh_B = xxxxE+xx;  // The "B" value (X) from the high range calibration
const float OBSHigh_C = xxxxE+xx;  // The "C" value from the high range calibration
CampbellOBS3 osb3high(OBS3Power, OBSHighPin, OBSHigh_A, OBSHigh_B, OBSHigh_C, OBS3_ADS1115Address, OBS3numberReadings);
```
