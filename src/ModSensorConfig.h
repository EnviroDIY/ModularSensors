/**
 * @file ModSensorConfig.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief This file is used to configure the clock and other library
 * settings/preferences for the Modular Sensors Library and some of the
 * underlying libraries.
 *
 * @note This file must be included in every header file in the library - before
 * including anything else - to ensure that the settings are applied
 * consistently across all files and that compile time settings trickle down to
 * the underlying libraries.
 *
 * For the Arduino IDE, this is the only way to configure these settings. If you
 * are using PlatformIO, you have the option of using this file or changing your
 * configuration using build flags in your platformio.ini. Compile times will be
 * shorter if you use this file instead of build flags, but it is much easier to
 * track changes in your platformio.ini file than in this file.
 */


// Header Guards
#ifndef SRC_MODSENSORCONFIG_H_
#define SRC_MODSENSORCONFIG_H_

// Include Arduino.h to ensure variants file is pulled first
#include <Arduino.h>

//==============================================================
// Select the Real Time Clock to use by uncommenting one of the options below.
// NOTE: This is optional for a EnviroDIY Mayfly or Stonefly but **REQUIRED**
// for all other boards!
// #define MS_USE_RV8803
// #define MS_USE_DS3231
// #define MS_USE_RTC_ZERO
//==============================================================

//==============================================================
// Select ADS1015 instead of the ADS1115, if desired
// This is for sensors that use the external ADC for analog voltage
// measurements.
// #define MS_USE_ADS1015
//==============================================================

//==============================================================
// Disable concurrent polling of SDI-12 sensors, if needed
// Concurrent measurement support was introduced in version 1.2 (April 12, 1996)
// of the specification and all sensors that claim to use version 1.2 or higher
// must support it.
// That being said.. some new sensors are fussy and will incorrectly abort
// concurrent measurements due to noise or other sensors on the bus.
// NOTE:  By default, concurrent measurements are used for all SDI-12 sensors!
// NOTE: This single setting applies to all SDI-12 sensors; it cannot be set on
// a per-sensor basis.
// #define MS_SDI12_NON_CONCURRENT

// Disable CRC checking for SDI-12 sensors, if needed
// This may be necessary if using older or fussy sensors
// CRC support in SDI-12 was implemented with version 1.3 (April 7, 2000) and
// all sensors that claim to use version 1.3 or higher must support it.
// NOTE:  By default, CRCs are used for all SDI-12 sensors!
// NOTE: This single setting applies to all SDI-12 sensors; it cannot be set on
// a per-sensor basis.
// #define MS_SDI12_NO_CRC_CHECK
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
 * January 1, 2025
 */
#define EARLIEST_SANE_UNIX_TIMESTAMP 1735689600
#endif

#ifndef LATEST_SANE_UNIX_TIMESTAMP
/**
 * @brief The latest unix timestamp that can be considered sane.
 *
 * January 1, 2035
 */
#define LATEST_SANE_UNIX_TIMESTAMP 2051222400
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
// Analog voltage configuration
//==============================================================
#ifndef MS_PROCESSOR_ADC_RESOLUTION
/**
 * @brief Select or adjust the processor analog resolution.
 *
 * This is the resolution of the **built-in** processor ADC and it cannot be set
 * higher than what your processor actually supports. This does **not** apply to
 * the TI ADS1115 or ADS1015 external ADS.
 *
 * The default for AVR boards is 10 and for other boards is 12.
 *
 * Future note: The ESP32 has a 12 bit ADC and the ESP8266 has a 10 bit ADC.
 */
#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
#define MS_PROCESSOR_ADC_RESOLUTION 10
#else
#define MS_PROCESSOR_ADC_RESOLUTION 12
#endif
#if !defined(MS_PROCESSOR_ADC_RESOLUTION)
#error The processor ADC resolution must be defined!
#endif  // MS_PROCESSOR_ADC_RESOLUTION
#endif

/// @brief The maximum possible value of the ADC - one less than the resolution
/// shifted up one bit.
#define PROCESSOR_ADC_MAX ((1 << MS_PROCESSOR_ADC_RESOLUTION) - 1)
/// @brief The maximum possible range of the ADC - the resolution shifted up one
/// bit.
#define PROCESSOR_ADC_RANGE (1 << MS_PROCESSOR_ADC_RESOLUTION)

#if !defined(MS_PROCESSOR_ADC_REFERENCE_MODE) || defined(DOXYGEN)
#if defined(ARDUINO_ARCH_AVR) || defined(DOXYGEN)
/**
 * @brief The voltage reference mode for the processor's ADC.
 *
 * For an AVR board, this must be one of:
 * - `DEFAULT`: the default built-in analog reference of 5 volts (on 5V Arduino
 * boards) or 3.3 volts (on 3.3V Arduino boards)
 * - `INTERNAL`: a built-in reference, equal to 1.1 volts on the ATmega168 or
 * ATmega328P and 2.56 volts on the ATmega32U4 and ATmega8 (not available on the
 * Arduino Mega)
 * - `INTERNAL1V1`: a built-in 1.1V reference (Arduino Mega only)
 * - `INTERNAL2V56`: a built-in 2.56V reference (Arduino Mega only)
 * - `EXTERNAL`: the voltage applied to the AREF pin (0 to 5V only) is used as
 * the reference.
 *
 * If not set on an AVR board `DEFAULT` is used.
 *
 * For the best accuracy, use an `EXTERNAL` reference with the AREF pin
 * connected to the power supply for the EC sensor.
 */
#define MS_PROCESSOR_ADC_REFERENCE_MODE DEFAULT
#endif
#if defined(ARDUINO_ARCH_SAMD) || defined(DOXYGEN)
/**
 * @brief The voltage reference mode for the processor's ADC.
 *
 * For a SAMD board, this must be one of:
 * - `AR_DEFAULT`: the default built-in analog reference of 3.3V
 * - `AR_INTERNAL`: a built-in 2.23V reference
 * - `AR_INTERNAL1V0`: a built-in 1.0V reference
 * - `AR_INTERNAL1V65`: a built-in 1.65V reference
 * - `AR_INTERNAL2V23`: a built-in 2.23V reference
 * - `AR_EXTERNAL`: the voltage applied to the AREF pin is used as the reference
 *
 * If not set on an SAMD board `AR_DEFAULT` is used.
 *
 * For the best accuracy, use an `AR_EXTERNAL` reference with the AREF pin
 * connected to the power supply for the EC sensor.  On most Adafruit SAMD51
 * boards, there is an optional solder jumper to connect the AREF pin to
 * the 3.3V supply. I suggest you close the jumper! On an EnviroDIY Stonefly,
 * there is also a solder jumper, but on the Stonefly the jumper is *closed by
 * default.*
 *
 * @see
 * https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
 */
#if defined(ENVIRODIY_STONEFLY_M4)
#define MS_PROCESSOR_ADC_REFERENCE_MODE AR_EXTERNAL
#else
#define MS_PROCESSOR_ADC_REFERENCE_MODE AR_DEFAULT
#endif
#endif
#if !defined(MS_PROCESSOR_ADC_REFERENCE_MODE)
#error The processor ADC reference type must be defined!
#endif  // MS_PROCESSOR_ADC_REFERENCE_MODE
#endif  // !defined(MS_PROCESSOR_ADC_REFERENCE_MODE) || defined(DOXYGEN)

#if !defined(MS_DEFAULT_ADS1X15_ADDRESS) || defined(DOXYGEN)
/// @brief The assumed address of the ADS1115 or ADS1015, 1001 000 (ADDR = GND)
#define MS_DEFAULT_ADS1X15_ADDRESS 0x48
#endif  // MS_DEFAULT_ADS1X15_ADDRESS
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

#ifndef MS_ALWAYS_FLUSH_PUBLISHERS
/**
 * @brief Set this to true to always force publishers to attempt to transmit
 * data.  If false, publishers will only transmit data at the sendEveryX
 * interval or when the buffer fills.
 */
#define MS_ALWAYS_FLUSH_PUBLISHERS false
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
#define MS_SEND_BUFFER_SIZE 1360
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

#ifndef TINY_GSM_YIELD_MS
/**
 * @brief The number of milliseconds to yield to the GSM module when using
 * TinyGSM.
 *
 * If you are using a slow baud rate to communicate with your module, this delay
 * is set to prevent command responses from being spliced apart.  This is
 * especially important when using a faster processor.
 */
#define TINY_GSM_YIELD_MS 2
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
 * using any MQTT publishers, you can ignore this or set it to 256.
 */
#define MS_MQTT_MAX_PACKET_SIZE 1536
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
