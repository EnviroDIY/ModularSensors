[//]: # ( @page page_modem_notes Notes about Modems )
# Notes about Modems

[//]: # ( @tableofcontents )

[//]: # ( Start GitHub Only )
- [Notes about Modems](#notes-about-modems)
  - [Summary of Classes to use for Various Manufactured Modules](#summary-of-classes-to-use-for-various-manufactured-modules)
  - [Default baud rates of supported modules](#default-baud-rates-of-supported-modules)
  - [Power Requirements of Supported Modems](#power-requirements-of-supported-modems)
  - [Sleep and Reset Pin Labels](#sleep-and-reset-pin-labels)
  - [Pin Numbers to Use when Connecting to the Mayfly](#pin-numbers-to-use-when-connecting-to-the-mayfly)

[//]: # ( End GitHub Only )

If you are having trouble, please see the pages for the specific modems and the TinyGSM [getting started](https://github.com/vshymanskyy/TinyGSM#getting-started) and [troubleshooting](https://github.com/vshymanskyy/TinyGSM#troubleshooting) sections.


[//]: # ( @section modem_notes_classes Summary of Classes to use for Various Manufactured Modules )
## Summary of Classes to use for Various Manufactured Modules

|                    Module                     |                      Class                       |
| :-------------------------------------------: | :----------------------------------------------: |
|     Digi XBee3 LTE-M (u-blox SARA R410M)      | DigiXBeeCellularTransparent or DigiXBeeLTEBypass |
|       Digi 3G Global (u-blox SARA U201)       | DigiXBeeCellularTransparent or DigiXBee3GBypass  |
|   Digi XBee LTE Cat 1 Verizon (Telit LE866)   |           DigiXBeeCellularTransparent            |
| Digi XBee3 LTE Cat 1 Verizon (Telit LE866-SV) |           DigiXBeeCellularTransparent            |
| Digi XBee3 LTE Cat 1 AT&T (Telit LE866A1-NA)  |           DigiXBeeCellularTransparent            |
|              Digi XBee S6B WiFi               |                   DigiXBeeWifi                   |
|               Espressif ESP8266               |                 EspressifESP8266                 |
|                Espressif ESP32                |                 EspressifESP8266                 |
|                 Quectel BG96                  |                   QuectelBG96                    |
|       Mikroe LTE IOT 2 Click (_BG96_)¹        |                   QuectelBG96                    |
|         Dragino NB IOT Bee (_BG96_)¹          |                   QuectelBG96                    |
|   Nimbelink Skywire LTE-M Global (_BG96_)¹    |                   QuectelBG96                    |
|            Sequans Monarch VZM20Q             |                  SequansMonarch                  |
|      Nimbelink Skywire LTE-M for Verizon      |                  SequansMonarch                  |
|            SIMCom SIM7000 variants            |                  SIMComSIM7000                   |
|      SIMCom SIM800, SIM900 and variants       |                   SIMComSIM800                   |
|          Adafruit Fona (2G, SIM800H)          |                   SIMComSIM800                   |
|           Sodaq GPRSBee R4 (SIM900)           |                   SIMComSIM800                   |
|         Mikroe GSM-GPS Click (SIM808)         |                   SIMComSIM800                   |
|       Mikroe GSM/GNSS 2 Click (SIM868)        |                   SIMComSIM800                   |
|       Sodaq GPRSBee R6 or R7 (SIM800H)        |                   Sodaq2GBeeR6                   |
|          u-blox SARA R4 or N4 series          |                  SodaqUBeeR410M                  |
|     Sodaq UBee LTE-M² (u-blox SARA R410M)     |                  SodaqUBeeR410M                  |
|   Mikroe LTE IOT Click (u-blox SARA R410M)    |                  SodaqUBeeR410M                  |
|         u-blox 2G, 3G, and 4G modules         |                  SodaqUBeeU201                   |
|       Sodaq UBee 3G² (u-blox SARA U201)       |                  SodaqUBeeU201                   |

¹ The Quenctel BC95G based variant is _not_ supported.

² The NB IOT UBee based on the SARA N211 is _not_ supported.


[//]: # ( @section modem_notes_bauds Default Baud Rates of Supported Modems )
## Default baud rates of supported modules

|               Module               |                    Default Baud Rate                     |
| :--------------------------------: | :------------------------------------------------------: |
| Digi XBee and XBee3 _all variants_ |                           9600                           |
|     Espressif ESP8266 or ESP32     | 115200; Use `AT+UART_DEF=9600,8,1,0,0` to slow if needed |
|            Quectel BG96            |       115200; Use `AT+IPR=9600` to slow if needed        |
|       Sequans Monarch VZM20Q       |            921600; Use `AT+IPR=9600` to slow             |
|      SIMCom SIM7000 variants       |                           9600                           |
| SIMCom SIM800, SIM900 and variants |                           9600                           |
|    u-blox SARA R4 or N4 series     |  115200; _reverts to this speed after every power loss_  |
|   u-blox 2G, 3G, and 4G modules    |       varies by module, most auto-baud or use 9600       |


[//]: # ( @section modem_notes_power Power Requirements of Supported Modems )
## Power Requirements of Supported Modems

@note Standard USB ports and most Arduino boards (including the Mayfly) are only cabable of supplying **500mA** of power.
Any model that requires a higher level of current (almost all of them) should be given a separate power supply than the main processor.
**The most common symptom of insufficient power is that the module will not connect to the internet.**
Most modules are capable of serial communication and some level of functionality at current levels much below ideal, but will silently refuse to make a network connection.

@see https://github.com/vshymanskyy/TinyGSM/wiki/Powering-GSM-module

|                Module                | Operating Voltage |     Minimum Current Required      | Power Pin Label |
| :----------------------------------: | :---------------: | :-------------------------------: | :-------------: |
|           Digi XBee3 LTE-M           |    3.3 - 4.3 V    |              750 mA               |      `Vcc`      |
|            Digi 3G Global            |     3.8 - 5 V     |               1.5 A               |      `Vcc`      |
|     Digi XBee LTE Cat 1 Verizon      |    3.0 - 5.5 V    |                2A                 |      `Vcc`      |
|     Digi XBee3 LTE Cat 1 Verizon     |    3.0 - 5.5 V    |               1.5A                |      `Vcc`      |
|      Digi XBee3 LTE Cat 1 AT&T       |    3.0 - 5.5 V    |               1.5A                |      `Vcc`      |
|          Digi XBee S6B WiFi          |   3.14 - 3.46 V   |               350mA               |      `Vcc`      |
|          Espressif ESP8266           |    3.0 - 3.6 V    |               350mA               |     `VDDA`      |
|             Quectel BG96             |    3.3 - 4.3 V    |     350mA, 1A for 2G fallback     |     `VBAT`      |
|        Sequans Monarch VZM20Q        |    3.1 - 4.5 V    |               500mA               |     `VBAT1`     |
|  SIMCom SIM800, SIM900 and variants  |    3.4 - 4.4 V    |                2A                 |     `VBAT`      |
|        Sodaq GPRSBee R6 or R7        |    3.4 - 4.4 V    |                2A                 |    `ON/OFF`     |
|       SIMCom SIM7000 variants        |    3.0 - 4.3 V    |    600mA¹, 2A for 2G fallback     |     `VBAT`      |
|     u-blox SARA R4 or N4 series      |    3.2 - 4.2 V    | 500mA, 2A for 2G fallback on R412 |      `VCC`      |
| Sodaq UBee LTE-M (u-blox SARA R410M) |    3.2 - 4.2 V    |               500mA               |    `ON/OFF`     |
|    u-blox 2G, 3G, and 4G modules     | varies by module  |         varies by module          |      `VCC`      |
|   Sodaq UBee 3G (u-blox SARA U201)   |    3.3 - 4.4 V    |                2A                 |    `ON/OFF`     |

¹ This is a firm minimum; the SIM7000 _will not connect to the internet_ if only powered at 500mA.

[//]: # ( @section modem_notes_sleep Sleep and Reset Pin Labels )
## Sleep and Reset Pin Labels

|            Module             |              Status Pin Label               | Reset Label |            Wake / Sleep Request             |
| :---------------------------: | :-----------------------------------------: | :---------: | :-----------------------------------------: |
|       Digi XBee3 LTE-M        |      `ON/SLEEP_/DIO9` _or_ `CTS_/DIO7`      |  `RESET_`   |            `DTR_/SLEEP_RQ/DIO8`             |
|        Digi 3G Global         |      `ON/SLEEP_/DIO9` _or_ `CTS_/DIO7`      |  `RESET_`   |            `DTR_/SLEEP_RQ/DIO8`             |
|  Digi XBee LTE Cat 1 Verizon  |      `ON/SLEEP_/DIO9` _or_ `CTS_/DIO7`      |  `RESET_`   |            `DTR_/SLEEP_RQ/DIO8`             |
| Digi XBee3 LTE Cat 1 Verizon  |      `ON/SLEEP_/DIO9` _or_ `CTS_/DIO7`      |  `RESET_`   |            `DTR_/SLEEP_RQ/DIO8`             |
| Digi XBee3 LTE Cat 1 Verizon  |      `ON/SLEEP_/DIO9` _or_ `CTS_/DIO7`      |  `RESET_`   |            `DTR_/SLEEP_RQ/DIO8`             |
|      Digi XBee S6B WiFi       |      `DIO9/ON_SLEEP_` _or_ `DIO7/CTS_`      |  `RESET_`   |            `DIO8/DTR_/SLEEP_RQ`             |
|       Espressif ESP8266       | N/A in deep sleep; GPIO1-15 for light sleep | `EXT_RSTB`  | N/A in deep sleep; GPIO1-15 for light sleep |
|         Quectel BG96          |                  `STATUS`                   |  `RESET_N`  |                  `PWRKEY`                   |
|    Sequans Monarch VZM20Q     |     `GPIO3/STATUS_LED` _or_ `POWER_MON`     |  `RESETN`   |              `RTS` _or_ `RTS0`              |
|     SIMCom SIM800, SIM900     |                  `STATUS`                   |  `RESETN`   |                  `PWRKEY`                   |
|    Sodaq GPRSBee R6 or R7     |                  `STATUS`                   |     N/A     |                     N/A                     |
|    SIMCom SIM7000 variants    |                  `STATUS`                   |  `RESETN`   |                  `PWRKEY`                   |
|  u-blox SARA R4 or N4 series  |                   `V_INT`                   |  `RESET_N`  |                  `PWR_ON`                   |
|       Sodaq UBee LTE-M        |      `STATUS` also mislabeled as `CTS`      |   `RESET`   |                  `PWR_ON`                   |
| u-blox 2G, 3G, and 4G modules |                   `V_INT`                   |  `RESET_N`  |                  `PWR_ON`                   |
|         Sodaq UBee 3G         |      `STATUS` also mislabeled as `CTS`      |   `RESET`   |                  `PWR_ON`                   |



[//]: # ( @section modem_notes_mayfly_pins Pin Numbers to Use when Connecting to the Mayfly )
## Pin Numbers to Use when Connecting to the Mayfly

Here are the pin numbers to use for modules that can be attached directly to an EnviroDIY Mayfly using its Bee socket.

|                       Module                       |     Power      |     Status     |     Reset      | Sleep Request  |
| :------------------------------------------------: | :------------: | :------------: | :------------: | :------------: |
| Digi XBee/XBee3, all variants (direct connection)¹ |       -1       |      19²       |       -1       |       23       |
|  Digi XBee/XBee3, all variants (with LTE adapter)  | -1<sup>3</sup> | 19<sup>4</sup> |       20       |       23       |
|          Itead Wee (ESP8266)<sup>8</sup>           |       -1       |       -1       | -1<sup>5</sup> |       -1       |
|             DFRobot WiFi Bee (ESP8266)             |       -1       |       -1       |       -1       | 19<sup>6</sup> |
|             Dragino NB IOT Bee (BG96)              |       -1       |       -1       | -1<sup>7</sup> | -1<sup>7</sup> |
|                  Sodaq GPRSBee R4                  |       -1       |       19       |       -1       |       23       |
|               Sodaq GPRSBee R6 or R7               |       23       |       19       |       -1       |       -1       |
|                  Sodaq UBee LTE-M                  |       23       |       19       |       -1       |       20       |
|                   Sodaq UBee 3G                    |       23       |       19       |       -1       |       20       |


¹ To use the cellular Digi XBee's without the LTE adapter, your Mayfly must be at least v0.5b, you must use SJ13 to connect the Bee directly to the LiPo, and you must always have a battery connected to provide enough power for the XBee to make a cellular connection.
If you turn off the Mayfly via its switch but leave the XBee connected as above, it will drain your battery very quickly.
Disconnect the battery if you turn off the Mayfly.

² The Digi XBee reports ON/SLEEP_N on pin 13, but this is not connected to a Mayfly pin.
Instead, you must use the XBee's `CTS` pin (pin 12) which is connected to Mayfly pin 19.

<sup>3</sup> If you close solder jumper 1 (SJ1) on the LTE adapter and use connect solder jumper 7 (SJ7) on the Mayfly to te A5 to ASSOC, you can use A5 as the power pin for the XBee.

<sup>4</sup> The LTE adapter switches pins 12 and 13 so that the true `STATUS` pn of the XBee is connected to Mayfly pin 19.

<sup>5</sup> I *strongly* recommend running a new wire along the back of the Mayfly to connect pin 5 of the XBee socket to pin A4.
   This will enable you to use A4 as the reset pin which allows you to use deep sleep.

<sup>6</sup> Use 13 as the `espSleepRqPin` for light sleep.

<sup>7</sup> I *strongly* recommend running two new wires along the back of the Mayfly to connect pin 5 of the XBee socket to pin A4 and pin of the XBee socket to A3.
This will enable you to use A4 as the reset pin and A3 as the sleep request pin.
With those connections made, the Dragino BG96 becomes the _**only**_ LTE module that can be run using only the 500mA regulator on the Mayfly (ie, without a separate battery connection for the modem).

<sup>8</sup> This module is no longer produced or sold.