# The EnviroDIY Monitoring Station Kit<!--! {#example_envirodiy_monitoring_kit} -->

Example sketch to be used with the [EnviroDIY Monitoring Station Kit](https://www.envirodiy.org/product/envirodiy-monitoring-station-kit/).

This example uses the sensors and equipment included with (or recommended for) the [EnviroDIY Monitoring Station Kit](https://www.envirodiy.org/product/envirodiy-monitoring-station-kit/).
It includes code for a Mayfly 1.x, a [Meter Hydros 21](https://metergroup.com/products/hydros-21/) and either a [SIM7080G-based EnviroDIY LTEbee](https://www.envirodiy.org/product/envirodiy-lte-bee/) or an [EnviroDIY ESP32 Bee](https://www.envirodiy.org/product/envirodiy-esp32-bee-wifi-bluetooth/) for communication.
This example also makes use of the on-board light, temperature, and humidity sensors on the Mayfly 1.x.
The results are saved to the SD card and posted to Monitor My Watershed.

> [!NOTE]
> The Meter Hydros 21 is **not** included in the [EnviroDIY Monitoring Station Kit](https://www.envirodiy.org/product/envirodiy-monitoring-station-kit/) and must be purchased separately from Meter Group or one of their distributors.

The exact hardware configuration used in this example:

- [EnviroDIY Mayfly Data Logger](https://www.envirodiy.org/product/envirodiy-mayfly-data-logger/)
- [EnviroDIY SIM7080 LTE Bee](https://www.envirodiy.org/product/envirodiy-lte-bee/) (with Hologram SIM card) **OR** [EnviroDIY ESP32 Bee](https://www.envirodiy.org/product/envirodiy-esp32-bee-wifi-bluetooth/)
- Hydros21 CTD sensor

The EnviroDIY LTE SIM7080 module includes 2 antennas in the package.  The small thin one is the cellular antenna, and should be connected to the socket labeled "CELL".  The thicker block is the GPS antenna, and should be connected to the "GPS" socket, but only if you intend to use the GPS functionality of the module.  ModularSensors does not currently support GPS functionality, but other libraries such as TinyGPS can work with the SIM7080 module.

The included cell antenna works best in high-signal-strength areas.  For most remote areas and logger deployments, we suggest a larger LTE antenna, like the W3907B0100
from PulseLarsen (Digikey 1837-1003-ND or Mouser 673-W3907B0100)

Users purchasing a new Hydros21 CTD sensor will need to change the SDI-12 address of the sensor in order to use this sketch.  Full instructions for using this sketch as part of a monitoring station can be found in the EnviroDIY Monitoring Station Manual.
_______

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [The EnviroDIY Monitoring Station Kit](#the-envirodiy-monitoring-station-kit)
  - [To Use this Example](#to-use-this-example)
    - [Setup Monitor My Watershed](#setup-monitor-my-watershed)
    - [Set Kit Configuration Options](#set-kit-configuration-options)
      - [Select the Connection Type](#select-the-connection-type)
      - [Add Connection Info](#add-connection-info)
    - [Set Data Logging Options](#set-data-logging-options)
      - [Set the logger ID](#set-the-logger-id)
      - [Set the logging interval](#set-the-logging-interval)
      - [Set the time zone](#set-the-time-zone)
    - [Set the universally unique identifiers (UUIDs) for each variable](#set-the-universally-unique-identifiers-uuids-for-each-variable)
    - [Upload!](#upload)

<!--! @endif -->

_______

## To Use this Example<!--! {#example_envirodiy_monitoring_kit_using} -->

### Setup Monitor My Watershed<!--! {#example_envirodiy_monitoring_kit_pio} -->

- If necessary, create a new account on  [Monitor My Watershed](http://monitormywatershed.org/) or log in with your current account.
- Register a new site on Monitor My Watershed
- Add the following sensors to your site using the manage sensors page:
  - METER - HYDROS 21 - Meter_Hydros21_Cond - µS/cm - Water
  - METER - HYDROS 21 - Meter_Hydros21_Depth - mm - Water
  - METER - HYDROS 21 - Meter_Hydros21_Temp - °C - Water
  - EnviroDIY - Mayfly Data Logger v1 - EnviroDIY_Mayfly_Batt - V - Equipment
  - **ONE** of the following signal strength variables:
    - EnviroDIY - Mayfly LTE Bee - EnviroDIY_LTEB_SignalPercent - % - Equipment
      - _**OR**_
    - Espressif - ESP32 - ESP_SignalStrength - % - Equipment
  - EnviroDIY - Mayfly Data Logger v1 - Sensirion_SHT40_Humidity - % by vol - Equipment
  - EnviroDIY - Mayfly Data Logger v1 - Sensirion_SHT40_Temperature - °C - Equipment
  - EnviroDIY - Mayfly Data Logger v1 - Everlight_AnalogALS_Illuminance - Lux - Equipment
  - EnviroDIY - Mayfly Data Logger v1 - Maxim_DS3231_Temp - °C - Equipment

### Set Kit Configuration Options<!--! {#example_envirodiy_monitoring_kit_config} -->

Customize the sketch for the version of the kit that you have: cellular, wifi, or no internet connection.

#### Select the Connection Type<!--! {#example_envirodiy_monitoring_kit_connection_type} -->

In the configuration section, select no more than one of the "bee" types that you will be using.

- Activate the modem you wish to use by _removing_ any slashes (`//`) before the bee module you will use.
  - The line should start with `#define`
- Add two slashes (`//`) in front of the modem you are NOT using.
- If you are not using any internet connection, put two slashes (`//`) in front of both lines.

```cpp
#define USE_WIFI_BEE
// #define USE_CELLULAR_BEE
```

#### Add Connection Info<!--! {#example_envirodiy_monitoring_kit_connection_info} -->

Replace `YourAPN` or both `YourWiFiSSID` and `YourWiFiPassword` with the appropriate APN or SSID and password for your network.

Your APN is assigned by your SIM card provider.
If you are using a Hologram SIM card (recommended with the kit) the APN is `hologram`.

The SSID is the name of the wifi network.

> [!TIP]
> Only wifi networks secured by WPA2 are supported!
> Unsecured, WEP, or corporate networks are not supported.

You can leave the configuration for the connection type you're not using as is.

```cpp
// APN for cellular connection
#define CELLULAR_APN "YourAPN"
// WiFi access point name
#define WIFI_ID "YourWiFiSSID"
// WiFi password (WPA2)
#define WIFI_PASSWD "YourWiFiPassword"
```

### Set Data Logging Options<!--! {#example_envirodiy_monitoring_kit_logging_options} -->

Customize your data logging options in `Data Logging Options` section of the example.

#### Set the logger ID<!--! {#example_envirodiy_monitoring_kit_logger_id} -->

We recommend using your logger's serial number as the logger ID.

```cpp
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "YourLoggerID";
```

#### Set the logging interval<!--! {#example_envirodiy_monitoring_kit_logging_interval} -->

The recommended logging interval is 15 minutes for most streams.
Very small or flashy streams may require a shorter interval.
Large rivers may change so slowly that 60 minute interval is acceptable.

```cpp
// How frequently (in minutes) to log data
const int8_t loggingInterval = 15;
```

#### Set the time zone<!--! {#example_envirodiy_monitoring_kit_time_zone} -->

This is the timezone that will be used in your data files and that will be published to Monitor My Watershed
_Daylight savings time will not be applied!_
Please use standard time!

```cpp
// Your logger's timezone.
const int8_t timeZone = -5;  // Eastern Standard Time
```

### Set the universally unique identifiers (UUIDs) for each variable<!--! {#example_envirodiy_monitoring_kit_uuids} -->

- Go back to the web page for your site on [Monitor My Watershed](http://monitormywatershed.org/)
- Find and click the white "View Token UUID List" button above the small map on your site page.
- Paste the copied UUIDs into your sketch, _replacing_ the text between `Beginning of Token UUID List` and `End of Token UUID List`.

```cpp
// ---------------------   Beginning of Token UUID List   ---------------------


const char* UUIDs[] =  // UUID array for device sensors
    {
        "12345678-abcd-1234-ef00-1234567890ab",  // Specific conductance (Meter_Hydros21_Cond)
        "12345678-abcd-1234-ef00-1234567890ab",  // Water depth (Meter_Hydros21_Depth)
        "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Meter_Hydros21_Temp)
        "12345678-abcd-1234-ef00-1234567890ab",  // Battery voltage (EnviroDIY_Mayfly_Batt)
        "12345678-abcd-1234-ef00-1234567890ab",  // Percent full scale (EnviroDIY_LTEB_SignalPercent)
        "12345678-abcd-1234-ef00-1234567890ab",  // Relative humidity (Sensirion_SHT40_Humidity)
        "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Sensirion_SHT40_Temperature)
        "12345678-abcd-1234-ef00-1234567890ab",  // Illuminance (Everlight_AnalogALS_Illuminance)
        "12345678-abcd-1234-ef00-1234567890ab",  // Temperature (Maxim_DS3231_Temp)
};
const char* registrationToken = "12345678-abcd-1234-ef00-1234567890ab";  // Device registration token
const char* samplingFeature = "12345678-abcd-1234-ef00-1234567890ab";  // Sampling feature UUID


// -----------------------   End of Token UUID List  -----------------------
```

- VERY CAREFULLY check the order of the UUIDs that you have copied in.
The UUIDs _**MUST**_ be in the following order:
  - Specific conductance (Meter_Hydros21_Cond)
  - Water depth (Meter_Hydros21_Depth)
  - Temperature (Meter_Hydros21_Temp)
  - Battery voltage (EnviroDIY_Mayfly_Batt)
  - Percent full scale (EnviroDIY_LTEB_SignalPercent or ESP32_SignalPercent)
  - Relative humidity (Sensirion_SHT40_Humidity)
  - Temperature (Sensirion_SHT40_Temperature)
  - Illuminance (Everlight_AnalogALS_Illuminance)
  - Temperature (Maxim_DS3231_Temp)
- If the UUIDs you copied are in a different order, reorder them in this chunk of code to match the order above _**EXACTLY**_.

> [!CAUTION]
> You _**MUST**_ have the UUIDs in _**EXACTLY**_ the order listed above.
> If you don't, your data will be sent to the wrong place on Monitor My Watershed and will be very difficult to fix after the fact.

### Upload!<!--! {#example_envirodiy_monitoring_kit_upload} -->

If you have compilation errors, first check the placement of all quotation marks and that all lines end with semicolons.

Test everything at home **before** deploying out in the wild!

<!--! @section example_envirodiy_monitoring_kit_pio_config PlatformIO Configuration -->

<!--! @include{lineno} EnviroDIY_Monitoring_Kit/platformio.ini -->

<!--! @section example_envirodiy_monitoring_kit_code The Complete Code -->

<!--! @include{lineno} EnviroDIY_Monitoring_Kit/EnviroDIY_Monitoring_Kit.ino -->
