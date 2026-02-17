# Examples Using ModularSensors<!--! {#page_the_examples} -->

These example programs demonstrate how to use the modular sensors library.
Each example has slightly different functionality.

___

<!--! @if GITHUB -->

- [Examples Using ModularSensors](#examples-using-modularsensors)
  - [Basic Functionality](#basic-functionality)
    - [Single Sensor](#single-sensor)
    - [Simple Logging](#simple-logging)
  - [Publishing Data](#publishing-data)
    - [Publishing to Monitor My Watershed](#publishing-to-monitor-my-watershed)
    - [Publishing to ThingSpeak](#publishing-to-thingspeak)
    - [Publishing to Data and Images to AWS](#publishing-to-data-and-images-to-aws)
  - [Calculations and Complex Logging](#calculations-and-complex-logging)
    - [Barometric Pressure Correction](#barometric-pressure-correction)
    - [Multiple Logging Intervals](#multiple-logging-intervals)
  - [Everything at Once - a la carte](#everything-at-once---a-la-carte)
    - [Menu a la carte](#menu-a-la-carte)
  - [EnviroDIY Sensor Stations](#envirodiy-sensor-stations)
    - [The EnviroDIY Sensor Station Kit](#the-envirodiy-sensor-station-kit)
  - [Examples for Outdated Hardware](#examples-for-outdated-hardware)
    - [Simple Logging for the Learn EnviroDIY course](#simple-logging-for-the-learn-envirodiy-course)
    - [DRWI Citizen Science](#drwi-citizen-science)
      - [DRWI Mayfly 1.x LTE](#drwi-mayfly-1x-lte)
      - [DRWI CitSci No Cellular](#drwi-citsci-no-cellular)
      - [DRWI CitSci (2G)](#drwi-citsci-2g)
      - [DRWI Digi LTE](#drwi-digi-lte)
      - [DRWI EnviroDIY LTEbee](#drwi-envirodiy-ltebee)

<!--! @endif -->

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

## Basic Functionality<!--! {#examples_basic} -->

### Single Sensor<!--! {#examples_single_sensor} -->

The single_sensor example shows making use of the unified set of commands to print data from a MaxBotix ultrasonic range finder to the serial port.
It also shows creating a calculated variable which is the water depth.

- [Instructions for the single sensor example](https://envirodiy.github.io/ModularSensors/example_single_sensor.html)
- [The single sensor example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/single_sensor)

### Simple Logging<!--! {#examples_simple_logging} -->

The simple logging example shows how to create multiple sensors, create variables for the sensors in a variable array, and log the data from the sensors to an SD card.

- [Instructions for the simple logging example](https://envirodiy.github.io/ModularSensors/example_simple_logging.html)
- [The simple logging example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/simple_logging)

___

## Publishing Data<!--! {#examples_publishing} -->

### Publishing to Monitor My Watershed<!--! {#examples_mmw} -->

The logging to Monitor My Watershed example uses a Digi XBee in transparent mode to publish data live from a BME280 and Maxim DS18 to Monitor My Watershed.

- [Instructions for the logging to Monitor My Watershed example](https://envirodiy.github.io/ModularSensors/example_mmw.html)
- [The logging to Monitor My Watershed example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/logging_to_MMW)

### Publishing to ThingSpeak<!--! {#examples_thingspeak} -->

The logging to ThingSpeak example uses an ESP8266 to send data to ThingSpeak.
It also includes a Meter Hydros 21 (formerly know as a Decagon CTD) and a Campbell OBS3+.

- [Instructions for the logging to ThingSpeak example](https://envirodiy.github.io/ModularSensors/example_thingspeak.html)
- [The logging to ThingSpeak example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/logging_to_ThingSpeak)

### Publishing to Data and Images to AWS<!--! {#examples_aws} -->

The logging numeric data to AWS IoT core and images to S3.

- [Instructions for the logging to ThingSpeak example](https://envirodiy.github.io/ModularSensors/example_aws_iot_core.html)
- [The logging to ThingSpeak example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/AWS_IoT_Core)

___

## Calculations and Complex Logging<!--! {#examples_complex} -->

### Barometric Pressure Correction<!--! {#examples_baro_rho} -->

The barometric pressure correction  example demonstrates how to work with calculated variables and calculates water depth by correcting the total pressure measured by a MeaSpec MS5803 with the atmospheric pressure measured by a Bosch BME280 environmental sensor and the temperature measured by a Maxim DS18 temperature probe.

- [Instructions for the barometric pressure correction example](https://envirodiy.github.io/ModularSensors/example_baro_rho.html)
- [The barometric pressure correction example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/baro_rho_correction)

### Multiple Logging Intervals<!--! {#examples_double_log} -->

The more complicated double logger example using two different logger instances to log data at two different intervals, in this case, an AM3215 logging every minute, while checking the battery voltage only every 5 minutes.
This showcases both how to use two different logging instances and how to use some of the functions to set up your own logging loop rather than using the logData() function.

- [Instructions for the double logger example](https://envirodiy.github.io/ModularSensors/example_double_log.html)
- [The double logger example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/double_logger)

___

## Everything at Once - a la carte<!--! {#examples_everything} -->

### Menu a la carte<!--! {#examples_menu} -->

The "menu a la carte" example shows most of the functions of the library in one gigantic program.
It has code in it for every possible sensor and modem and for both AVR and SAMD boards.
It is also over 1500 lines long.
This example is intended to be used like an a la carte menu of all possible options where you selected only the portions of code pertinent to you and delete everything else.
This example is *NOT* intended to be run in its entirety

- [The menu a la carte walkthrough](https://envirodiy.github.io/ModularSensors/example_menu.html)
  - Unlike the instructions for the other examples which show how to modify the example for your own use, this is a chunk-by-chunk step through of the code with explanations of each portion of the code and links to further documentation on each sensor.
  - There is a table of contents at the top of the walkthrough; I *strongly* recommend using that to skip through to the portions you are interested in
- [The a la carte example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/menu_a_la_carte)

___

## EnviroDIY Sensor Stations<!--! {#examples_envirodiy} -->

### The EnviroDIY Sensor Station Kit<!--! {#examples_envirodiy_kit} -->

The EnviroDIY Sensor Station Kit is designed to be used with the [EnviroDIY Monitoring Station Kit](https://www.envirodiy.org/product/envirodiy-monitoring-station-kit/).

- [Instructions for the EnviroDIY sensor station kit example](https://envirodiy.github.io/ModularSensors/example_envirodiy_monitoring_kit.html)
- [The EnviroDIY sensor station kit example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/example_envirodiy_monitoring_kit)

___

## Examples for Outdated Hardware<!--! {#examples_outdated} -->

### Simple Logging for the Learn EnviroDIY course<!--! {#examples_learn_envirodiy} -->

The simple logging example for the [Learn EnviroDIY programming course](https://envirodiy.github.io/LearnEnviroDIY/) shows how to create multiple sensors, create variables for the sensors in a variable array, and log the data from the sensors to an SD card.
It is very similar to the other simple logging example, with just a few extra sensors.

- [Instructions for the learn EnviroDIY course example](https://envirodiy.github.io/ModularSensors/example_learn_envirodiy.html)
- [The learn EnviroDIY course example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/simple_logging_LearnEnviroDIY)

___

### DRWI Citizen Science<!--! {#examples_drwi} -->

#### DRWI Mayfly 1.x LTE<!--! {#examples_drwi_mayfly1} -->

The DRWI Mayfly 1.x LTE example uses the sensors and equipment used by most groups participating in the DRWI (Delaware River Watershed Initiative) Citizen Science project with the Stroud Water Research Center.
It includes a Meter Hydros 21 (CTD) and a SIM7080G-based EnviroDIY LTEbee for communication.
This example also makes use of the on-board light, temperature, and humidity sensors on the Mayfly 1.x.
The results are saved to the SD card and posted to Monitor My Watershed. Only to be used with newer Mayfly v1.0 and v1.1 boards.

- [Instructions for the Mayfly 1.x LTE DRWI Citizen Science example](https://envirodiy.github.io/ModularSensors/example_drwi_mayfly1.html)
- [The LTEG DRWI Citizen Science example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/OutdatedHardware/DRWI_CitizenScience/DRWI_Mayfly1)

#### DRWI CitSci No Cellular<!--! {#examples_drwi_no_cell} -->

The DRWI no cellular example uses the sensors and equipment standard to the DRWI Citizen Science project but omits the data publisher for circumstances where there is no cellular signal.
It includes a Meter Hydros 21 (CTD) and a Campbell OBS3+ (Turbidity).
The exclusion of the modem and publisher simplifies the code from the other DRWI examples and uses less power than running one of cellular versions without attaching the modem.

- [Instructions for the no-cellular DRWI Citizen Science example](https://envirodiy.github.io/ModularSensors/example_drwi_no_cell.html)
- [The no-cellular DRWI Citizen Science example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/OutdatedHardware/DRWI_CitizenScience/DRWI_NoCellular)

#### DRWI CitSci (2G)<!--! {#examples_drwi_2g} -->

The 2G DRWI Citizen Science example uses the sensors and equipment found on older stations used in the DRWI Citizen Science project prior to 2020. The 2G GPRSbee boards no longer function in the USA, so this code should not be used and is only provided to archival and reference purposes.
It includes a Meter Hydros 21 (formerly know as a Decagon CTD), a Campbell OBS3+, and a Sodaq GPRSBee for communication.
The results are saved to the SD card and posted to Monitor My Watershed.
The only difference between this and the other cellular DRWI examples is the type of modem used.

- [Instructions for the 2G DRWI Citizen Science example](https://envirodiy.github.io/ModularSensors/example_drwi_2g.html)
- [The 2G DRWI Citizen Science example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/OutdatedHardware/DRWI_CitizenScience/DRWI_CitSci)

#### DRWI Digi LTE<!--! {#examples_drwi_digilte} -->

The DRWI Digi LTE example uses the sensors and equipment common to older stations (2016-2020) deployed by groups participating in the DRWI Citizen Science project with the Stroud Water Research Center.
It includes a Meter Hydros 21 (formerly know as a Decagon CTD), a Campbell OBS3+, and a Digi XBee3 LTE-M for communication.
The results are saved to the SD card and posted to Monitor My Watershed.
The only difference between this and the other cellular DRWI examples is the type of modem used.

- [Instructions for the Digi LTE DRWI Citizen Science example](https://envirodiy.github.io/ModularSensors/example_drwi_digilte.html)
- [The Digi LTE DRWI Citizen Science example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/OutdatedHardware/DRWI_CitizenScience/DRWI_DigiLTE)

#### DRWI EnviroDIY LTEbee<!--! {#examples_drwi_ediylte} -->

The DRWI EnviroDIY LTEbee example uses the sensors and equipment common to newer stations (2016-2020) deployed by groups participating in the DRWI Citizen Science project with the Stroud Water Research Center.
It includes a Meter Hydros 21 (CTD), a Campbell OBS3+, (Turbidity) and a SIM7080G-based EnviroDIY LTEbee for communication.
The results are saved to the SD card and posted to Monitor My Watershed.
The only difference between this and the other cellular DRWI examples below is the type of modem used.

- [Instructions for the EnviroDIY LTEbee DRWI Citizen Science example](https://envirodiy.github.io/ModularSensors/example_drwi_ediylte.html)
- [The EnviroDIY LTE DRWI Citizen Science example on GitHub](https://github.com/EnviroDIY/ModularSensors/tree/master/examples/OutdatedHardware/DRWI_CitizenScience/DRWI_DigiLTE)
