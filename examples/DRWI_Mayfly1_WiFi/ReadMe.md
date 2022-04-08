# DRWI Sites with a Mayfly 1.x and EnviroDIY ESP32 WiFi Bees <!-- {#example_drwi_mayfly1_wifi} -->
Example sketch for using the EnviroDIY ESP32 WiFi cellular module with an EnviroDIY Mayfly Data Logger.

The exact hardware configuration used in this example:
 * Mayfly v1.1 board
 * EnviroDIY ESP32 WiFi module
 * Hydros21 CTD sensor

An EnviroDIY ESP32 WiFi module can be used with the older Mayfly v0.5b boards if you change line 101 (for modemVccPin) from 18 to -1.
This is because the Mayfly v1.x board has a separate 3.3v regulator to power the Bee socket and is controlled by turning pin 18 on or off.
Mayfly v0.5b has the Bee socket constantly powered, therefore using "-1" is the proper setting for that line of code.

The WiFi antenna is built into the ESP32 Bee - no external antenna is needed

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [DRWI Sites with a Mayfly 1.x and EnviroDIY ESP32 WiFi Bees](#drwi-sites-with-a-mayfly-1x-and-envirodiy-esp32-wifi-bees)
- [Unique Features of the DRWI Mayfly 1.x WiFi Example](#unique-features-of-the-drwi-mayfly-1x-wifi-example)

[//]: # ( End GitHub Only )

_______

# Unique Features of the DRWI Mayfly 1.x WiFi Example <!-- {#example_drwi_mayfly1_wifi_unique} -->
- Specifically for sites within the Delaware River Watershed Initiative.
- Uses a EnviroDIY WiFi Bee based on the Espressif ESP32-WROOM-32


[//]: # ( @section example_drwi_mayfly1_wifi_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} DRWI_Mayfly1_WiFi/platformio.ini )

[//]: # ( @section example_drwi_mayfly1_wifi_code The Complete Code )

[//]: # ( @include{lineno} DRWI_Mayfly1_WiFi/DRWI_Mayfly1_._WiFi.ino )
