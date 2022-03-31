# DRWI Digi LTE Sites <!-- {#example_drwi_digilte} -->

This is the code example that should be used for all groups working with the Stroud Water Research Center within the Delaware River Watershed Initiative.
This should be used at all sites with cellular LTE service.

Before programming your board with this example, you must register your site and sensors at http://data.envirodiy.org/.
Stroud can provide directions for registering your site if you need assistance.

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [DRWI Digi LTE Sites](#drwi-digi-lte-sites)
- [Unique Features of the DRWI Digi LTE Example](#unique-features-of-the-drwi-digi-lte-example)
- [To Use this Example](#to-use-this-example)
  - [Prepare and set up PlatformIO](#prepare-and-set-up-platformio)
  - [Set the logger ID](#set-the-logger-id)
  - [Set the calibration coefficients for the Campbell OBS3+](#set-the-calibration-coefficients-for-the-campbell-obs3)
  - [Set the universally universal identifiers (UUID) for each variable](#set-the-universally-universal-identifiers-uuid-for-each-variable)
  - [Upload!](#upload)

[//]: # ( End GitHub Only )

_______

# Unique Features of the DRWI Digi LTE Example <!-- {#example_drwi_digilte_unique} -->
- Specifically for sites within the Delaware River Watershed Initiative.
- Uses a Digi XBee3 LTE-M for live data.

# To Use this Example <!-- {#example_drwi_digilte_using} -->

## Prepare and set up PlatformIO <!-- {#example_drwi_digilte_pio} -->
- Register a site and sensors at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- Create a new PlatformIO project
- Replace the contents of the platformio.ini for your new project with the [platformio.ini](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/DRWI_DigiLTE/platformio.ini) file in the examples/DRWI_DigiLTE folder on GitHub.
    - It is important that your PlatformIO configuration has the lib_ldf_mode and build flags set as they are in the example.
    - Without this, the program won't compile.
- Open [DRWI_DigiLTE.ino](https://raw.githubusercontent.com/EnviroDIY/ModularSensors/master/examples/DRWI_DigiLTE/DRWI_DigiLTE.ino) and save it to your computer.
    - After opening the link, you should be able to right click anywhere on the page and select "Save Page As".
    - Move it into the src directory of your project.
    - Delete main.cpp in that folder.

## Set the logger ID <!-- {#example_drwi_digilte_logger_id} -->
- Change the "XXXX" in this section of code to the loggerID assigned by Stroud:

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXX";
```

## Set the calibration coefficients for the Campbell OBS3+ <!-- {#example_drwi_digilte_obs3_calibration} -->
- The OBS3+ ships with a calibration certificate; you need this sheet!
- Change _**all**_ of the the `0.000E+00` and `1.000E+00` values in this section of code to the values on that calibration sheet.
Use numbers from the side of the calibration sheet that shows the calibration in _**volts**_.
    - The sketch will not compile if these values are not entered properly.
    - Do not change any values except those that are `0.000E+00` and `1.000E+00`!

```cpp
// ==========================================================================
//  Campbell OBS 3 / OBS 3+ Analog Turbidity Sensor
// ==========================================================================
#include <sensors/CampbellOBS3.h>
const int8_t OBS3Power = sensorPowerPin;  // Power pin (-1 if unconnected)
const uint8_t OBS3numberReadings = 10;
const uint8_t ADSi2c_addr = 0x48;  // The I2C address of the ADS1115 ADC
// Campbell OBS 3+ *Low* Range Calibration in Volts
const int8_t OBSLowADSChannel = 0;  // ADS channel for *low* range output
const float OBSLow_A = 0.000E+00;  // "A" value (X^2) [*low* range]
const float OBSLow_B = 1.000E+00;  // "B" value (X) [*low* range]
const float OBSLow_C = 0.000E+00;  // "C" value [*low* range]
CampbellOBS3 osb3low(OBS3Power, OBSLowADSChannel, OBSLow_A, OBSLow_B, OBSLow_C, ADSi2c_addr, OBS3numberReadings);
// Campbell OBS 3+ *High* Range Calibration in Volts
const int8_t OBSHighADSChannel = 1;  // ADS channel for *high* range output
const float OBSHigh_A = 0.000E+00;  // "A" value (X^2) [*high* range]
const float OBSHigh_B = 1.000E+00;  // "B" value (X) [*high* range]
const float OBSHigh_C = 0.000E+00;  // "C" value [*high* range]
CampbellOBS3 osb3high(OBS3Power, OBSHighADSChannel, OBSHigh_A, OBSHigh_B, OBSHigh_C, ADSi2c_addr, OBS3numberReadings);
```

## Set the universally universal identifiers (UUID) for each variable <!-- {#example_drwi_digilte_uuids} -->
- Go back to the web page for your site at the Monitor My Watershed/EnviroDIY data portal (http://monitormywatershed.org/)
- Find and click the white "View Token UUID List" button above the small map on your site page
- **VERY CAREFULLY** check that the variables are in exactly the same order as in the variable array:

```cpp
Variable* variableList[] = {
    ...
}
```

- If any of the variables are in a different order on the web page than in your code **reorder the variables in your code to match the website**.
- After you are completely certain that you have the order right in the variable section of your code use the teal "Copy" button on the website to copy the section of code containing all of the UUID's.
- Paste the code from the website into your program in this section below the variable array

```cpp
// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***
// Check the order of your variables in the variable list!!!
// Be VERY certain that they match the order of your UUID's!
// Rearrange the variables in the variable list if necessary to match!
// *** CAUTION --- CAUTION --- CAUTION --- CAUTION --- CAUTION ***
/* clang-format off */
const char* UUIDs[] = {
    "12345678-abcd-1234-ef00-1234567890ab",  // Electrical conductivity (Decagon_CTD-10_Cond)
    "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Decagon_CTD-10_Temp)
    "12345678-abcd-1234-ef00-1234567890ab",  // Water depth (Decagon_CTD-10_Depth)
    "12345678-abcd-1234-ef00-1234567890ab",  // Turbidity (Campbell_OBS3_Turb)
    "12345678-abcd-1234-ef00-1234567890ab",  // Turbidity (Campbell_OBS3_Turb)
    "12345678-abcd-1234-ef00-1234567890ab",  // Battery voltage (EnviroDIY_Mayfly_Batt)
    "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (EnviroDIY_Mayfly_Temp)
    "12345678-abcd-1234-ef00-1234567890ab",  // Received signal strength indication (Digi_Cellular_RSSI)
    "12345678-abcd-1234-ef00-1234567890ab"   // Percent full scale (Digi_Cellular_SignalPercent)
};
const char* registrationToken = "12345678-abcd-1234-ef00-1234567890ab";  // Device registration token
const char* samplingFeature = "12345678-abcd-1234-ef00-1234567890ab";  // Sampling feature UUID
/* clang-format on */

```

## Upload! <!-- {#example_drwi_digilte_upload} -->
- Test everything at home **before** deploying out in the wild!

_______


[//]: # ( @section example_drwi_digilte_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} DRWI_DigiLTE/platformio.ini )

[//]: # ( @section example_drwi_digilte_code The Complete Code )

[//]: # ( @include{lineno} DRWI_DigiLTE/DRWI_DigiLTE.ino )
