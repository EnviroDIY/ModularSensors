/**
 * @file ModSensorConfig.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief This file is used to configure the clock and debugging to be used by
 * the Modular Sensors Library.  If you are using PlatformIO, you should not
 * need to touch this file, as the settings can all be configured using build
 * flags in your platformio.ini. For the Arduino IDE, this is a alternative.
 */

// Header Guards
#ifndef SRC_MODSENSORCONFIG_H_
#define SRC_MODSENSORCONFIG_H_

// Include Arduino.h to ensure variants file is pulled first
#include <Arduino.h>

//==============================================================
// Select the Real Time Clock to use by uncommenting one of the options below.
// NOTE: This is optional for a EnviroDIY Mayfly or Stonefly
// #define MS_USE_RV8803
// #define MS_USE_DS3231
// #define MS_USE_RTC_ZERO
//==============================================================

//==============================================================
// Select ADS1015 instead of the ADS1115, if desired
// #define MS_USE_ADS1015
//==============================================================

//==============================================================
// Disable concurrent polling of SDI-12 sensors, if needed
// This may be necessary if using older or fussy sensors
// #define MS_SDI12_NON_CONCURRENT
//==============================================================

//==============================================================
// Configure the output streams for debugging
// If these are not defined, most output will go to either "Serial" or
// "SerialUSB" and there will be no secondary output.
// #define MS_OUTPUT Serial
// #define MS_2ND_OUTPUT Serial2
// #define TINY_GSM_DEBUG Serial2
// #define TINY_GSM_DEBUG_DEEP Serial1
// Or use this to turn of *ALL* output
// #define MS_SILENT
//==============================================================

//==============================================================
#ifndef MS_LOGGERBASE_BUTTON_BENCH_TEST
/**
 * @brief Enable bench testing mode for the testing button.
 *
 * When enabled, the testing button uses the benchTestingMode() function to
 * repeatedly read and print out sensor data. When disabled (default), the
 * testing button causes a reading to be taken and transmitted immediately
 * using the normal procedure.
 */
#define MS_LOGGERBASE_BUTTON_BENCH_TEST false
#endif
//==============================================================


//==============================================================
// Uncomment any debbugging options you might want to use

// #define MS_LOGGERBASE_DEBUG
// #define MS_LOGGERBASE_DEBUG_DEEP

// #define MS_CLOCKSUPPORT_DEBUG
// #define MS_CLOCKSUPPORT_DEBUG_DEEP

// #define MS_WATCHDOGAVR_DEBUG
// #define MS_WATCHDOGAVR_DEBUG_DEEP
// #define MS_WATCHDOGSAMD_DEBUG
// #define MS_WATCHDOGSAMD_DEBUG_DEEP

// #define MS_SENSORBASE_DEBUG
// #define MS_VARIABLEBASE_DEBUG
// #define MS_VARIABLEARRAY_DEBUG
// #define MS_VARIABLEARRAY_DEBUG_DEEP

// #define MS_DATAPUBLISHERBASE_DEBUG
// #define MS_DATAPUBLISHERBASE_DEBUG_DEEP

// #define MS_ENVIRODIYPUBLISHER_DEBUG
// #define MS_THINGSPEAKPUBLISHER_DEBUG
// #define MS_UBIDOTSPUBLISHER_DEBUG
// #define MS_DREAMHOSTPUBLISHER_DEBUG

// #define MS_LOGGERMODEM_DEBUG
// #define MS_LOGGERMODEM_DEBUG_DEEP

// #define MS_DIGIXBEE_DEBUG
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG
// #define MS_DIGIXBEECELLULARTRANSPARENT_DEBUG_DEEP
// #define MS_DIGIXBEE3GBYPASS_DEBUG
// #define MS_DIGIXBEE3GBYPASS_DEBUG_DEEP
// #define MS_DIGIXBEELTEBYPASS_DEBUG
// #define MS_DIGIXBEELTEBYPASS_DEBUG_DEEP
// #define MS_DIGIXBEEWIFI_DEBUG
// #define MS_DIGIXBEEWIFI_DEBUG_DEEP

// #define MS_ESPRESSIFESP8266_DEBUG
// #define MS_ESPRESSIFESP8266_DEBUG_DEEP

// #define MS_SIMCOMSIM7080_DEBUG
// #define MS_SIMCOMSIM7080_DEBUG_DEEP

// #define MS_QUECTELBG96_DEBUG
// #define MS_QUECTELBG96_DEBUG_DEEP
// #define MS_SEQUANSMONARCH_DEBUG
// #define MS_SEQUANSMONARCH_DEBUG_DEEP
// #define MS_SIMCOMSIM800_DEBUG
// #define MS_SIMCOMSIM800_DEBUG_DEEP
// #define MS_SIMCOMSIM7000_DEBUG
// #define MS_SIMCOMSIM7000_DEBUG_DEEP
// #define MS_SODAQ2GBEER6_DEBUG
// #define MS_SODAQ2GBEER6_DEBUG_DEEP
// #define MS_SODAQUBEER410M_DEBUG
// #define MS_SODAQUBEER410M_DEBUG_DEEP
// #define MS_SODAQUBEEU201_DEBUG
// #define MS_SODAQUBEEU201_DEBUG_DEEP

// #define MS_ALPHASENSECO2_DEBUG
// #define MS_ANALOGELECCONDUCTIVITY_DEBUG
// #define MS_ANALOGELECCONDUCTIVITY_DEBUG_DEEP
// #define MS_AOSONGAM2315_DEBUG
// #define MS_AOSONGDHT_DEBUG
// #define MS_APOGEESQ212_DEBUG
// #define MS_BOSCHBME280_DEBUG
// #define MS_BOSCHBMP3XX_DEBUG
// #define MS_CAMPBELLOBS3_DEBUG
// #define MS_EVERLIGHTALSPT19_DEBUG
// #define MS_FREESCALEMPL115A2_DEBUG

// #define MS_GEOLUXHYDROCAM_DEBUG
// #define GEOLUX_DEBUG Serial2

// #define MS_MAXBOTIXSONAR_DEBUG
// #define MS_MAXIMDS18_DEBUG
// #define MS_MAXIMDS3231_DEBUG
// #define MS_MEASPECMS5803_DEBUG
// #define MS_PALEOTERRAREDOX_DEBUG
// #define MS_PROCESSORSTATS_DEBUG
// #define MS_RAINCOUNTERI2C_DEBUG
// #define MS_SENSIRION_SHT4X_DEBUG
// #define MS_TALLYCOUNTERI2C_DEBUG
// #define MS_TIADS1X15_DEBUG
// #define MS_TIINA219_DEBUG
// #define MS_TURNERCYCLOPS_DEBUG
// #define MS_TURNERTURBIDITYPLUS_DEBUG

// #define MS_SDI12SENSORS_DEBUG
// #define MS_SDI12SENSORS_DEBUG_DEEP
// #define MS_DECAGON5TM_DEBUG
// #define MS_METERTEROS11_DEBUG

// #define MS_ATLASPARENT_DEBUG
// #define MS_ATLASSCIENTIFICCO2_DEBUG
// #define MS_ATLASSCIENTIFICDO_DEBUG
// #define MS_ATLASSCIENTIFICEC_DEBUG

// #define MS_GROPOINTPARENT_DEBUG
// #define MS_GROPOINTPARENT_DEBUG_DEEP

// #define MS_KELLERPARENT_DEBUG
// #define MS_KELLERPARENT_DEBUG_DEEP

// #define MS_YOSEMITECHPARENT_DEBUG
// #define MS_YOSEMITECHPARENT_DEBUG_DEEP


//==============================================================


//==============================================================
// SPI Configuration, iff needed
#if !defined(SDCARD_SPI)
/**
 * @brief The SPI port to use for the SD card.
 *
 * This is typically defined in the board variant file.
 */
#define SDCARD_SPI SPI
#endif
//==============================================================


//==============================================================
// Other library configuration

#ifndef MS_LOGGER_EPOCH
/**
 * @brief The epoch start to use for the logger
 */
#define MS_LOGGER_EPOCH epochStart::unix_epoch
#endif

#ifndef EARLIEST_SANE_UNIX_TIMESTAMP
/**
 * @brief The earliest unix timestamp that can be considered sane.
 *
 * January 1, 2023
 */
#define EARLIEST_SANE_UNIX_TIMESTAMP 1672531200
#endif

#ifndef LATEST_SANE_UNIX_TIMESTAMP
/**
 * @brief The latest unix timestamp that can be considered sane.
 *
 * January 1, 2030
 */
#define LATEST_SANE_UNIX_TIMESTAMP 1893456000
#endif

#ifndef MAX_NUMBER_VARS
/**
 * @brief The largest number of variables from a single sensor
 */
#define MAX_NUMBER_VARS 21
// GroPoint Profile GPLP-8 has 8 Moisture and 13 Temperature values
#endif

#ifndef MAX_NUMBER_SENDERS
/**
 * @brief The largest number of publishers that can be attached to a logger
 */
#define MAX_NUMBER_SENDERS 4
#endif

#ifndef MS_SEND_BUFFER_SIZE
/**
 * @brief Send Buffer
 *
 * This determines how many characters to set out at once over the TCP
 * connection. Increasing this may decrease data use by a logger, while
 * decreasing it will save memory. Do not make it smaller than 32 or bigger
 * than 1500 (a typical TCP Maximum Transmission Unit).
 *
 * This can be changed by setting the build flag MS_SEND_BUFFER_SIZE when
 * compiling.
 */
#define MS_SEND_BUFFER_SIZE 750
#endif

#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 */
#define TINY_GSM_RX_BUFFER 64
#endif
//==============================================================


#endif  // SRC_MODSENSORCONFIG_H_
