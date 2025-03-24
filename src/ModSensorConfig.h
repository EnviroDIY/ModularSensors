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
// #define MS_AWS_IOT_PUBLISHER_DEBUG
// #define MS_S3PRESIGNEDPUBLISHER_DEBUG

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
//==============================================================
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
// Time-stamp configurations
//==============================================================
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
//==============================================================


//==============================================================
// Variable configurations
//==============================================================
#ifndef MAX_NUMBER_VARS
/**
 * @brief The largest number of variables from a single sensor.
 *
 * Every sensor will create a buffer of this length for holding variable values.
 * Decrease this value to save a memory.
 */
#define MAX_NUMBER_VARS 21
// GroPoint Profile GPLP-8 has 8 Moisture and 13 Temperature values
#endif
//==============================================================


//==============================================================
// Publisher configuration
//==============================================================
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
 * than TINY_GSM_SEND_MAX_SIZE for you module if using TinyGSM or 1500 (a
 * typical TCP Maximum Transmission Unit) if you aren't using TinyGSM.
 *
 * When using the ThingSpeak publisher, make sure this is at least 240 bytes.
 *
 * This can be changed by setting the build flag MS_SEND_BUFFER_SIZE when
 * compiling.
 */
#define MS_SEND_BUFFER_SIZE 1460
#endif

#ifndef TINY_GSM_RX_BUFFER
/**
 * @brief The size of the buffer for incoming data.
 *
 * If using a module that buffers data internally, this can be 64 or lower.  If
 * using a module that doesn't buffer data, this as large as your largest
 * expected incoming message; up to ~1500.
 */
#define TINY_GSM_RX_BUFFER 64
#endif

#ifndef MS_MQTT_MAX_PACKET_SIZE
/**
 * @brief Configure the size of the PubSubClient buffer for MQTT publishers.
 *
 * This is the maximum size of any single MQTT message - incoming or outgoing.
 * The same value applies to all MQTT publishers. The default within
 * PubSubClient is 256 bytes, but to receive long S3 URLs from AWS IoT Core, we
 * need to jack this way up. Incoming pre-signed S3 URLs are >1200 bytes.  If
 * you're not using both AWS IoT Core and S3, you should drop this down. When
 * using the ThingSpeak publisher, ~240 bytes should suffice.  If you're not
 * using any MQTT publishers, you can ignore this.
 */
#define MS_MQTT_MAX_PACKET_SIZE 1280
#endif
//==============================================================


//==============================================================
// Special configurations for the AWS IoT Core publisher
//==============================================================
#ifndef MS_AWS_IOT_PUBLISHER_SUB_COUNT
/**
 * @brief The maximum number of extra subscriptions that can be added to the AWS
 * IoT Core publisher.
 *
 * @note AWS sets a hard maximum of 8 subscriptions by one client at a time.
 */
#define MS_AWS_IOT_PUBLISHER_SUB_COUNT 4
#endif
#ifndef MS_AWS_IOT_PUBLISHER_PUB_COUNT
/**
 * @brief The maximum number of extra publish messages that can be added to the
 * AWS IoT Core publisher.
 */
#define MS_AWS_IOT_PUBLISHER_PUB_COUNT 4
#endif
#ifndef MS_AWS_IOT_MAX_CONNECTION_TIME
/**
 * @brief The maximum time to wait for subscriptions after publishing data to
 * AWS IoT Core.
 *
 * @note This is only used if the publisher is set to wait for subscriptions.
 */
#define MS_AWS_IOT_MAX_CONNECTION_TIME 30000L
#endif
//==============================================================


//==============================================================
// Special configurations for the S3 publisher
//==============================================================
#ifndef S3_DEFAULT_FILE_EXTENSION
/**
 * @brief The default file extension to use to send to S3: ".jpg"
 *
 * This assumes you are using S3 to send images. If you want to put your basic
 * numeric data into S3, you should use the IoT publisher and create a rule and
 * corresponding lambda function to transfer the json data from IoT Core to S3.
 */
#define S3_DEFAULT_FILE_EXTENSION ".jpg"
#endif
/**
 * @brief Define this to tell the S3 publisher verify that the expected filename
 * is contained in the S3 URL.
 */
// #define MS_S3PRESIGNED_VALIDATE_URL_FILENAME
/**
 * @brief Define this to tell the S3 publisher to prevent reuse of pre-signed
 * URLs or upload filenames.
 */
// #define MS_S3PRESIGNED_PREVENT_REUSE
//==============================================================


#endif  // SRC_MODSENSORCONFIG_H_
