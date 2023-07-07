# DRWI Sites with a Mayfly 1.x and EnviroDIY ESP32 WiFi Bees <!-- {#example_drwi_mayfly1_wifi} -->
Alpha test sketch for transmitting over WiFi Digi S6B 

The hardware configuration used in this example:
 * Mayfly v1.1 board
 * EnviroDIY Digi WiFi  S6B module

This is used to intergrate in Reliable Delivery
https://github.com/EnviroDIY/ModularSensors/issues/194
Setup ms_cfg.h for your network parameters

Merge PR Notes
 setFileTimestamp()  ~ assume all local view is at local time
on uSD internet connection synospsis  DBGyymm.log
 SdFat sd1_card_fatfs; - renamed to be readable and findable

rtc - renamed to rtcExtPhy
 going with compatible versions for SAMDxx

Note if "old date" that forces MMW to decompress 
gets 400 "bad request" 
-- Response Code -- 400 waited  108 mS Timeout 15432
data needs to be deleted
remove /* atl_extension */ 