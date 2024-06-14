# Notes about Modems<!-- {#page_modem_notes} -->

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )

- [Notes about Modems](#notes-about-modems)
  - [Summary of Classes to use for Various Manufactured Modules](#summary-of-classes-to-use-for-various-manufactured-modules)
  - [Default baud rates of supported modules](#default-baud-rates-of-supported-modules)
  - [Power Requirements of Supported Modems](#power-requirements-of-supported-modems)
  - [Sleep and Reset Pin Labels](#sleep-and-reset-pin-labels)
  - [Pin Numbers to Use when Connecting to a Mayfly 0.x](#pin-numbers-to-use-when-connecting-to-a-mayfly-0x)
  - [Pin Numbers to Use when Connecting to a Mayfly 1.x](#pin-numbers-to-use-when-connecting-to-a-mayfly-1x)

[//]: # ( End GitHub Only )

If you are having trouble, please see the pages for the specific modems and the TinyGSM [getting started](https://github.com/vshymanskyy/TinyGSM#getting-started) and [troubleshooting](https://github.com/vshymanskyy/TinyGSM#troubleshooting) sections.

## Summary of Classes to use for Various Manufactured Modules<!-- {#modem_notes_classes} -->

|                    Module                     |                      Class                       |
| :-------------------------------------------: | :----------------------------------------------: |
|     Digi XBee3 LTE-M (u-blox SARA R410M)      | DigiXBeeCellularTransparent or DigiXBeeLTEBypass |
|       Digi 3G Global (u-blox SARA U201)       | DigiXBeeCellularTransparent or DigiXBee3GBypass  |
|   Digi XBee LTE Cat 1 Verizon (Telit LE866)   |           DigiXBeeCellularTransparent            |
| Digi XBee3 LTE Cat 1 Verizon (Telit LE866-SV) |           DigiXBeeCellularTransparent            |
| Digi XBee3 LTE Cat 1 AT&T (Telit LE866A1-NA)  |           DigiXBeeCellularTransparent            |
|              Digi XBee S6B WiFi               |                   DigiXBeeWifi                   |
|               Espressif ESP8266               |                 EspressifESP8266                 |
|                Espressif ESP32                |                 EspressifESP32                 |
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
|         EnviroDIY LTE Bee (SIM7080G)          |                    SIMCom7080                    |

¹ The Quenctel BC95G based variant is _not_ supported.

² The NB IOT UBee based on the SARA N211 is _not_ supported.

***

## Default baud rates of supported modules<!-- {#modem_notes_bauds} -->

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

***

## Power Requirements of Supported Modems<!-- {#modem_notes_power} -->

@note Standard USB ports and most Arduino boards (including the Mayfly) are only cabable of supplying **500mA** of power.
Any model that requires a higher level of current (almost all of them) should be given a separate power supply than the main processor.
**The most common symptom of insufficient power is that the module will not connect to the internet.**
Most modules are capable of serial communication and some level of functionality at current levels much below ideal, but will silently refuse to make a network connection.

@see <https://github.com/vshymanskyy/TinyGSM/wiki/Powering-GSM-module>

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
|            SIMCom SIM7080            |    2.7 - 4.8 V    |              < 500mA              |     `VBAT`      |
|     EnviroDIY LTE Bee (SIM7080G)     |    2.7 - 4.8 V    |              < 500mA              |      `VCC`      |
|     u-blox SARA R4 or N4 series      |    3.2 - 4.2 V    | 500mA, 2A for 2G fallback on R412 |      `VCC`      |
| Sodaq UBee LTE-M (u-blox SARA R410M) |    3.2 - 4.2 V    |               500mA               |    `ON/OFF`     |
|    u-blox 2G, 3G, and 4G modules     | varies by module  |         varies by module          |      `VCC`      |
|   Sodaq UBee 3G (u-blox SARA U201)   |    3.3 - 4.4 V    |                2A                 |    `ON/OFF`     |

¹ This is a firm minimum; the SIM7000 _will not connect to the internet_ if only powered at 500mA.

***

## Sleep and Reset Pin Labels<!-- {#modem_notes_sleep} -->

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
|        SIMCom SIM7080G        |                  `STATUS`                   |     N/A     |                  `PWRKEY`                   |
| EnviroDIY LTE Bee (SIM7080G)  |                  `STATUS`                   |     N/A     |                  `PWRKEY`"                  |
|  u-blox SARA R4 or N4 series  |                   `V_INT`                   |  `RESET_N`  |                  `PWR_ON`                   |
|       Sodaq UBee LTE-M        |      `STATUS` also mislabeled as `CTS`      |   `RESET`   |                  `PWR_ON`                   |
| u-blox 2G, 3G, and 4G modules |                   `V_INT`                   |  `RESET_N`  |                  `PWR_ON`                   |
|         Sodaq UBee 3G         |      `STATUS` also mislabeled as `CTS`      |   `RESET`   |                  `PWR_ON`                   |

***

## Pin Numbers to Use when Connecting to a Mayfly 0.x<!-- {#modem_notes_mayfly_0_pins} -->

Here are the pin numbers to use for modules that can be attached directly to an EnviroDIY Mayfly v0.3, 0.4, 0.5, 0.5b, or 0.5c using its Bee socket.

|                            Module                            |     Power      |     Status     |     Reset      | Sleep Request  |
| :----------------------------------------------------------: | :------------: | :------------: | :------------: | :------------: |
|      Digi XBee/XBee3, all variants (direct connection)¹      |       -1       |      19²       |       -1       |       23       |
| Digi XBee/XBee3, all variants (with LTE adapter<sup>8</sup>) | -1<sup>3</sup> | 19<sup>4</sup> |       20       |       23       |
|               Itead Wee (ESP8266)<sup>8</sup>                |       -1       |       -1       | -1<sup>5</sup> |       -1       |
|                  DFRobot WiFi Bee (ESP8266)                  |       -1       |       -1       |       -1       |      N/A       |
|                  Dragino NB IOT Bee (BG96)                   |       -1       |       -1       | -1<sup>7</sup> | -1<sup>7</sup> |
|                  Sodaq GPRSBee R4 (SIM900)                   |       -1       |       19       |       -1       |       23       |
|               Sodaq GPRSBee R6 or R7 (SIM800H)               |       23       |       19       |      N/A       |      N/A       |
|             Sodaq UBee LTE-M (u-blox SARA R410M)             |       23       |       19       |       -1       |       20       |
|               Sodaq UBee 3G (u-blox SARA U201)               |       23       |       19       |       -1       |       20       |
|                 EnviroDIY LTE Bee (SIM7080G)                 |       -1       |       19       |      N/A       | 23<sup>9</sup> |

¹ To use the cellular Digi XBee's without the LTE adapter, your Mayfly must be at least v0.5b, you must use SJ13 to connect the Bee directly to the LiPo, and you must always have a battery connected to provide enough power for the XBee to make a cellular connection.
If you turn off the Mayfly via its switch but leave the XBee connected as above, it will drain your battery very quickly.
Disconnect the battery if you turn off the Mayfly.

² The Digi XBee reports ON/SLEEP_N on pin 13, but this is not connected to a Mayfly pin.
Instead, you must use the XBee's `CTS` pin (pin 12) which is connected to Mayfly pin 19 and set the argument `useCTSforStatus` to `true` in the bee constructor.

<sup>3</sup> If you close solder jumper 1 (SJ1) on the LTE adapter and use connect solder jumper 7 (SJ7) on the Mayfly to connect A5 to ASSOC, you can use A5 as the power pin for the XBee.

<sup>4</sup> The LTE adapter switches pins 12 and 13 so that the true `STATUS` pin of the XBee is connected to Mayfly pin 19.
You should set the argument `useCTSforStatus` to `false` in the bee constructor

<sup>5</sup> I _strongly_ recommend running a new wire along the back of the Mayfly to connect pin 5 of the XBee socket to pin A4.
   This will enable you to use A4 as the reset pin which allows you to use deep sleep.

<sup>7</sup> I _strongly_ recommend running two new wires along the back of the Mayfly to connect pin 5 of the XBee socket to pin A4 and pin 18 of the XBee socket to A3.
This will enable you to use A4 as the reset pin and A3 as the sleep request pin.
With those connections made, the Dragino BG96 becomes the _**only**_ LTE module that can be run using only the 500mA regulator on the Mayfly (ie, without a separate battery connection for the modem).

<sup>8</sup> This module is no longer produced or sold.

<sup>9</sup> The EnviroDIY LTE Bee inverts the signal to the sleep request pin (`PWRKEY`) - which is also used for reset.
To use it, you must add these commands to your setup:

```cpp
modem.setModemWakeLevel(HIGH);
modem.setModemResetLevel(HIGH);
```

***

## Pin Numbers to Use when Connecting to a Mayfly 1.x<!-- {#modem_notes_mayfly_1_pins} -->

Here are the pin numbers to use for modules that can be attached directly to an EnviroDIY Mayfly v1.0 or 1.1 using its Bee socket.

|                      Module                       | Power | Status |     Reset      | Sleep Request  |
| :-----------------------------------------------: | :---: | :----: | :------------: | :------------: |
|            EnviroDIY WiFi Bee (ESP32)             |  18¹  |   -1   | A5<sup>3</sup> |       -1       |
|           EnviroDIY LTE Bee (SIM7080G)            |  18¹  |   19   |      N/A       | 23<sup>4</sup> |
| Digi XBee/XBee3, all variants (direct connection) |  18¹  |  19²   | A5<sup>3</sup> |       23       |
|            DFRobot WiFi Bee (ESP8266)             |  18¹  |   -1   |       -1       |       -1       |
|             Dragino NB IOT Bee (BG96)             |  18¹  |   -1   | A5<sup>3</sup> |       -1       |
|             Sodaq GPRSBee R4 (SIM900)             |  18¹  |   19   |       -1       |       23       |
|         Sodaq GPRSBee R6 or R7 (SIM800H)          |  23   |   19   |      N/A       |      N/A       |
|       Sodaq UBee LTE-M (u-blox SARA R410M)        |  23   |   19   | A5<sup>3</sup> |       20       |
|         Sodaq UBee 3G (u-blox SARA U201)          |  23   |   19   | A5<sup>3</sup> |       20       |

¹ This assumes you have not changed solder jumper 18.  If you have switched SJ18 to connect bee pin one directly to 3.3V, use -1.

² The Digi XBee reports ON/SLEEP_N on pin 13, but this is not connected to a Mayfly pin by default.
You can use the XBee's `CTS` pin (pin 12) which is connected to Mayfly pin 19 by default and set the argument `useCTSforStatus` to `true` in the bee constructor.
Alternately (and preferably) you can change solder jumper 19 (SJ19) to connect bee pin 13 to D19 and set the argument `useCTSforStatus` to `false`.

<sup>3</sup> Solder jumper 20 should be left in the default position, connecting pin A5 to bee pin 5.

<sup>4</sup> The EnviroDIY LTE Bee inverts the signal to the sleep request pin (`PWRKEY`) - which is also used for reset.
To use it, you must add these commands to your setup:

```cpp
modem.setModemWakeLevel(HIGH);
modem.setModemResetLevel(HIGH);
```
