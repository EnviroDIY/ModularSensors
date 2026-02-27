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

#if !defined(MS_DEFAULT_ADS1X15_ADDRESS) || defined(DOXYGEN)
/**
 * @brief The default I²C address of the ADS1115 or ADS1015 external ADC.
 *
 * Valid addresses depend on the ADDR pin connection:
 * - `0x48` – ADDR to GND (default)
 * - `0x49` – ADDR to VDD
 * - `0x4A` – ADDR to SDA
 * - `0x4B` – ADDR to SCL
 *
 * Override with a build flag: `-DMS_DEFAULT_ADS1X15_ADDRESS=0x49`
 */
#define MS_DEFAULT_ADS1X15_ADDRESS 0x48
#endif

// Static assert to validate ADS1X15 I2C address is valid
static_assert(
    MS_DEFAULT_ADS1X15_ADDRESS >= 0x08 && MS_DEFAULT_ADS1X15_ADDRESS <= 0x77,
    "MS_DEFAULT_ADS1X15_ADDRESS must be a valid 7-bit I2C address (0x08-0x77)");
static_assert(MS_DEFAULT_ADS1X15_ADDRESS == 0x48 ||
                  MS_DEFAULT_ADS1X15_ADDRESS == 0x49 ||
                  MS_DEFAULT_ADS1X15_ADDRESS == 0x4A ||
                  MS_DEFAULT_ADS1X15_ADDRESS == 0x4B,
              "MS_DEFAULT_ADS1X15_ADDRESS should be 0x48, 0x49, 0x4A, or 0x4B "
              "for ADS1X15");
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

// Static assert to the maximum number of variables is no more than the largest
// number of variables from any sensor. Anything more is a waste of memory.
static_assert(MAX_NUMBER_VARS > 0 && MAX_NUMBER_VARS <= 21,
              "MAX_NUMBER_VARS must be between 1 and 21");

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
 * The default for AVR boards is 10 and for SAMD boards is 12.  The library
 * currently only supports AVR and SAMD platforms.
 *
 * Future note: The ESP32 has a 12 bit ADC and the ESP8266 has a 10 bit ADC.
 */
#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
#define MS_PROCESSOR_ADC_RESOLUTION 10
#elif defined(ARDUINO_ARCH_SAMD)
#define MS_PROCESSOR_ADC_RESOLUTION 12
#endif
#if !defined(MS_PROCESSOR_ADC_RESOLUTION)
#error The processor ADC resolution must be defined!
#endif  // MS_PROCESSOR_ADC_RESOLUTION

// Static assert to validate ADC resolution is reasonable
static_assert(MS_PROCESSOR_ADC_RESOLUTION >= 8 &&
                  MS_PROCESSOR_ADC_RESOLUTION <= 16,
              "MS_PROCESSOR_ADC_RESOLUTION must be between 8 and 16 bits");
#endif

/// @brief The maximum possible value of the ADC - one less than the resolution
/// shifted up one bit.
#define PROCESSOR_ADC_MAX ((1 << MS_PROCESSOR_ADC_RESOLUTION) - 1)
/// @brief The maximum possible range of the ADC - the resolution shifted up one
/// bit.
#define PROCESSOR_ADC_RANGE (1 << MS_PROCESSOR_ADC_RESOLUTION)
#ifndef MS_PROCESSOR_ANALOG_MAX_CHANNEL
/**
 * @brief Upper bound used to sanity-check analog channel numbers at runtime.
 *
 * This is not a hardware limit but a validation ceiling that exceeds the
 * largest channel index found on any supported Arduino platform (e.g. Mega:
 * A0–A15). Override with -D MS_PROCESSOR_ANALOG_MAX_CHANNEL=x if needed.
 */
#define MS_PROCESSOR_ANALOG_MAX_CHANNEL 100
#endif  // MS_PROCESSOR_ANALOG_MAX_CHANNEL

// Static assert to validate analog channel maximum is reasonable
static_assert(MS_PROCESSOR_ANALOG_MAX_CHANNEL > 0 &&
                  MS_PROCESSOR_ANALOG_MAX_CHANNEL <= 255,
              "MS_PROCESSOR_ANALOG_MAX_CHANNEL must be between 1 and 255");

//==============================================================
// Environmental sensor configuration
//==============================================================
#if !defined(MS_SEA_LEVEL_PRESSURE_HPA) || defined(DOXYGEN)
/**
 * @brief The atmospheric pressure at sea level in hPa for barometric sensors.
 *
 * This value is used by environmental sensors (BME280, BMP3xx, MS5837) for
 * calculating altitude and depth measurements. The default value is standard
 * atmospheric pressure at sea level (1013.25 hPa). Adjust this value based on
 * local atmospheric conditions for more accurate calculations.
 *
 * @note In library versions prior to 0.37.0, this variable was named
 * SEALEVELPRESSURE_HPA. and was defined in the header files for the BME280 and
 * BMP3xx sensors.
 */
#define MS_SEA_LEVEL_PRESSURE_HPA 1013.25f
#endif

// Static assert to validate sea level pressure is reasonable
static_assert(MS_SEA_LEVEL_PRESSURE_HPA >= 800.0f &&
                  MS_SEA_LEVEL_PRESSURE_HPA <= 1200.0f,
              "MS_SEA_LEVEL_PRESSURE_HPA must be between 800 and 1200 hPa "
              "(reasonable atmospheric pressure range)");
//==============================================================

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
// Static asserts to validate AWS IoT publisher counts are reasonable
static_assert(MAX_NUMBER_SENDERS >= 0 && MAX_NUMBER_SENDERS <= 16,
              "MAX_NUMBER_SENDERS must be between 0 and 16");

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

// Static assert to validate send buffer size is reasonable
static_assert(MS_SEND_BUFFER_SIZE >= 32 && MS_SEND_BUFFER_SIZE <= 2048,
              "MS_SEND_BUFFER_SIZE must be between 32 and 2048 bytes");

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

// Static assert to validate GSM RX buffer size is reasonable
static_assert(TINY_GSM_RX_BUFFER >= 16 && TINY_GSM_RX_BUFFER <= 2048,
              "TINY_GSM_RX_BUFFER must be between 16 and 2048 bytes");


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

// Static assert to validate GSM yield time is reasonable
static_assert(TINY_GSM_YIELD_MS >= 0 && TINY_GSM_YIELD_MS <= 1000,
              "TINY_GSM_YIELD_MS must be between 0 and 1000 milliseconds");

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

// Static assert to validate MQTT packet size is reasonable
static_assert(MS_MQTT_MAX_PACKET_SIZE >= 128 && MS_MQTT_MAX_PACKET_SIZE <= 4096,
              "MS_MQTT_MAX_PACKET_SIZE must be between 128 and 4096 bytes");

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

// Static asserts to validate AWS IoT publisher counts are reasonable
static_assert(
    MS_AWS_IOT_PUBLISHER_SUB_COUNT >= 0 && MS_AWS_IOT_PUBLISHER_SUB_COUNT <= 8,
    "MS_AWS_IOT_PUBLISHER_SUB_COUNT must be between 0 and 8 (AWS limit)");
static_assert(MS_AWS_IOT_PUBLISHER_PUB_COUNT >= 0 &&
                  MS_AWS_IOT_PUBLISHER_PUB_COUNT <= 16,
              "MS_AWS_IOT_PUBLISHER_PUB_COUNT must be between 0 and 16");

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

// cSpell:words SEALEVELPRESSURE


#endif  // SRC_MODSENSORCONFIG_H_
