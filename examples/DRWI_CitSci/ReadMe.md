# Examples using the Modular Sensors Library for DRWI Sites

This is the code example that should be used for all groups working with the Stroud Water Research Center within the Delaware River Watershed Initiative.  This should be used at all sites with cellular 2G service.

Before programming your board with this example, you must register your site and sensors at http://data.envirodiy.org/.  Stroud can provide directions for registering your site if you need assistance.

____

### Customizing this example sketch:


#### Set the proper address for data to go to Stroud's older "DreamHost" visualization tool

- Change the ```"TALK TO STROUD FOR THIS VALUE"``` in this section of code to the real URL given to you by Stroud:

```cpp
// Set up connection with the "DreamHost" data portal
#define DreamHostPortalRX "TALK TO STROUD FOR THIS VALUE"
```

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

#### Set the universally universal identifiers (UUID) for each variable

- Change _**all**_ of the the ```"12345678-abcd-1234-efgh-1234567890ab"``` values in this section of code to the values shown on the EnviroDIY data portal for your variables.
    - After you register your site and variables, you should see a group of empty plots on the page for your site.  The plots have titles like "Temperature" and below the plot will be a list of the "Medium", "Sensor", and "UUID" for that variable.
    - Copy the appropriate UUID from below each plot to its proper place in this section of the code.
    - For example, the ```"12345678-abcd-1234-efgh-1234567890ab"``` in the first line (```new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab")```) will be replaced by the UUID listed under the plot titled "Battery Voltage" with the sensor listed below as "EnviroDIY_Mayfly Data Logger".

```cpp
// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
Variable *variableList[] = {
    new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Cond(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Temp(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new DecagonCTD_Depth(&ctd, "12345678-abcd-1234-efgh-1234567890ab"),
    new CampbellOBS3_Turbidity(&osb3low, "12345678-abcd-1234-efgh-1234567890ab", "TurbLow"),
    new CampbellOBS3_Turbidity(&osb3high, "12345678-abcd-1234-efgh-1234567890ab", "TurbHigh"),
    new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab"),
    new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab"),
};
```

#### Set the universally universal identifiers (UUID) and sampling token for your site
- Change _**both**_ of the ```"12345678-abcd-1234-efgh-1234567890ab"``` values in this section of code to the values shown on the EnviroDIY data portal for your site.
    - After you register your site and variables, you should see a pane of site information next to a small map showing the location of your site.
    - Copy the registration token and sampling feature UUID from this pane to its proper place in this section of the code.

```cpp
// ==========================================================================
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
// ==========================================================================
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID
```
