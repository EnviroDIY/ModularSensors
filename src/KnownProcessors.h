/**
 * @file KnownProcessors.h
 * @copyright Stroud Water Research Center
 * @author Sara Damiano (sdamiano@stroudcenter.org)
 *
 * @brief Defines for parameters of known processor types for the EnviroDIY
 * ModularSensors library
 */

// Header Guards
#ifndef SRC_KNOWN_PROCESSORS_H_
#define SRC_KNOWN_PROCESSORS_H_

/**
 * @def LOGGER_BOARD
 * @brief Pretty text for the board name derived from the board's compiler
 * define.
 *
 * @def OPERATING_VOLTAGE
 * @brief The operating voltage of the board in volts.
 *
 * @def BATTERY_PIN
 * @brief The analog pin of the processor tied directly to the main battery
 * input and used to measure the battery voltage.
 * @note The battery pin is not available on all boards and may vary by board
 * version.  Where the battery pin is not available, it is set to -1.  Where
 * it's variable, it must be fixed in the ProcessorStats module or constructor.
 *
 * @def BATTERY_MULTIPLIER
 * @brief The multiplier to convert the raw "bits" measured on the battery pin
 * to voltage.  This value is based on any resistors or voltage dividers between
 * the battery and the pin.
 * @note The battery multiplier is not available on all boards and may vary by
 * board version.  Where the battery multiplier is not available, it is set to
 * -1. Where it's variable, it must be fixed in the ProcessorStats module or
 * constructor.
 *
 * @def BUILT_IN_ALS_POWER_PIN
 * @brief The digital pin controlling power to the built-in ambient light sensor
 * (ALS) on EnviroDIY boards.
 * @note Set to -1 when the ALS is always powered. *Leave undefined* when no
 * built-in ALS is available on the board.
 *
 * @def BUILT_IN_ALS_DATA_PIN
 * @brief The analog pin connected to the built-in ambient light sensor (ALS)
 * on EnviroDIY boards.
 * @note The data pin varies by board model and version. *Leave undefined* when
 * no built-in ALS is available on the board.
 *
 * @def BUILT_IN_ALS_SUPPLY_VOLTAGE
 * @brief The supply voltage for the built-in ambient light sensor (ALS) on
 * EnviroDIY boards, in volts.
 * @note Typically matches the board's operating voltage. *Leave undefined* when
 * no built-in ALS is available on the board.
 *
 * @def BUILT_IN_ALS_LOADING_RESISTANCE
 * @brief The loading resistance for the built-in ambient light sensor (ALS)
 * on EnviroDIY boards, in kΩ.
 * @note The loading resistance affects light measurement calculations. *Leave
 * undefined* when no built-in ALS is available on the board.
 */


//==============================================================
// EnviroDIY boards
//==============================================================

// https://envirodiy.org/mayfly/
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
#define LOGGER_BOARD "EnviroDIY Mayfly"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN A6
#define BATTERY_MULTIPLIER 4.7  // for v0.5 and later
#define BUILT_IN_ALS_POWER_PIN -1
#define BUILT_IN_ALS_DATA_PIN A4
#define BUILT_IN_ALS_SUPPLY_VOLTAGE 3.3
#define BUILT_IN_ALS_LOADING_RESISTANCE 10
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// Built in DS3231 RTC
#if (defined(MS_USE_RV8803) + defined(MS_USE_DS3231) + \
     defined(MS_USE_RTC_ZERO)) == 0
#define MS_USE_DS3231
#endif

// https://envirodiy.org/stonefly/
#elif defined(ENVIRODIY_STONEFLY_M4)
#define LOGGER_BOARD "EnviroDIY Stonefly"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN A9
#define BATTERY_MULTIPLIER 4.7
#define BUILT_IN_ALS_POWER_PIN -1
#define BUILT_IN_ALS_DATA_PIN A8
#define BUILT_IN_ALS_SUPPLY_VOLTAGE 3.3
#define BUILT_IN_ALS_LOADING_RESISTANCE 10
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// Built in RV-8803 RTC
#if (defined(MS_USE_RV8803) + defined(MS_USE_DS3231) + \
     defined(MS_USE_RTC_ZERO)) == 0
#define MS_USE_RV8803
#endif


//==============================================================
// Sodaq boards
//==============================================================

// https://learn.sodaq.com/Boards/ExpLoRer/ (Discontinued)
#elif defined(ARDUINO_SODAQ_EXPLORER)
#define LOGGER_BOARD "SODAQ ExpLoRer"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it

// https://learn.sodaq.com/Boards/Autonomo/ (Discontinued)
#elif defined(ARDUINO_SODAQ_AUTONOMO)
#define LOGGER_BOARD "SODAQ Autonomo"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 48  // for version v0.1
#define BATTERY_MULTIPLIER 1.47
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it

// https://learn.sodaq.com/Boards/One/base/ (Discontinued)
#elif defined(ARDUINO_SODAQ_ONE_BETA)
#define LOGGER_BOARD "SODAQ ONE Beta"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 10
#define BATTERY_MULTIPLIER 2  // for version v0.1
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it

// https://learn.sodaq.com/Boards/One/base/ (Discontinued)
#elif defined(ARDUINO_SODAQ_ONE)
#define LOGGER_BOARD "SODAQ ONE"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 10
#define BATTERY_MULTIPLIER 2  // for version v0.1
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it

// https://learn.sodaq.com/Boards/Mbili/ (Discontinued)
#elif defined(ARDUINO_AVR_SODAQ_MBILI)
#define LOGGER_BOARD "SODAQ Mbili"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN A6
#define BATTERY_MULTIPLIER 1.47
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// Built in DS3231 RTC
#if (defined(MS_USE_RV8803) + defined(MS_USE_DS3231) + \
     defined(MS_USE_RTC_ZERO)) == 0
#define MS_USE_DS3231
#endif

// https://support.sodaq.com/Boards/NDOGO (Discontinued)
#elif defined(ARDUINO_AVR_SODAQ_NDOGO)
#define LOGGER_BOARD "SODAQ Ndogo"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 10
#define BATTERY_MULTIPLIER 1.47
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://support.sodaq.com/Boards/TATU (Discontinued)
#elif defined(ARDUINO_AVR_SODAQ_TATU)
#define LOGGER_BOARD "SODAQ Tatu"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://support.sodaq.com/Boards/MOJA (Discontinued)
#elif defined(ARDUINO_AVR_SODAQ_MOJA)
#define LOGGER_BOARD "SODAQ Moja"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required


//==============================================================
// Adafruit boards
//==============================================================

// https://www.adafruit.com/product/3458
#elif defined(ARDUINO_AVR_FEATHER328P)
#define LOGGER_BOARD "Adafruit Feather 328p"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://www.adafruit.com/product/2771
#elif defined(ARDUINO_AVR_FEATHER32U4)
#define LOGGER_BOARD "Adafruit Feather 32u4"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://www.adafruit.com/product/3403
#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS) || \
    defined(ADAFRUIT_FEATHER_M0_EXPRESS)
#define LOGGER_BOARD "Adafruit Feather M0 Express"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it

// https://www.adafruit.com/product/2772
#elif defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
#define LOGGER_BOARD "Adafruit Feather M0"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it

// https://www.adafruit.com/product/2796
#elif defined(ADAFRUIT_FEATHER_M0_ADALOGGER)
#define LOGGER_BOARD "Adafruit Feather M0 Adalogger"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it

// https://www.adafruit.com/product/3857
#elif defined(ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define LOGGER_BOARD "Adafruit Feather M4"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// An external RTC is required

// https://www.adafruit.com/product/4759
#elif defined(ARDUINO_FEATHER_M4_CAN) || defined(ADAFRUIT_FEATHER_M4_CAN)
#define LOGGER_BOARD "Feather M4 CAN"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// An external RTC is required

// https://www.adafruit.com/product/4064
#elif defined(ADAFRUIT_GRAND_CENTRAL_M4)
#define LOGGER_BOARD "Adafruit Grand Central"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// An external RTC is required


//==============================================================
// Official Arduino boards
//==============================================================

// https://docs.arduino.cc/retired/boards/arduino-mega-adk-rev3/ (Retired)
#elif defined(ARDUINO_AVR_ADK)
#define LOGGER_BOARD "Arduino Mega ADK"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/boards/arduino-mega/ (Retired)
#elif defined(ARDUINO_AVR_MEGA)
#define LOGGER_BOARD "Arduino Mega"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://store-usa.arduino.cc/products/arduino-mega-2560-rev3
#elif defined(ARDUINO_AVR_MEGA2560)
#define LOGGER_BOARD "Arduino Mega 2560"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/getting-started-guides/ArduinoBT/ (Retired)
#elif defined(ARDUINO_AVR_BT)
#define LOGGER_BOARD "Arduino BT"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/boards/arduino-duemilanove/ (Retired)
#elif defined(ARDUINO_AVR_DUEMILANOVE)
#define LOGGER_BOARD "Arduino Duemilanove"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/boards/arduino-esplora/ (Retired)
#elif defined(ARDUINO_AVR_ESPLORA)
#define LOGGER_BOARD "Arduino Esplora"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/boards/arduino-ethernet-rev3-without-poe/
// (Retired)
#elif defined(ARDUINO_AVR_ETHERNET)
#define LOGGER_BOARD "Arduino Ethernet"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/boards/arduino-fio/ (Retired)
#elif defined(ARDUINO_AVR_FIO)
#define LOGGER_BOARD "Arduino Fio"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// Arduino version: https://docs.arduino.cc/retired/boards/arduino-gemma/
// (Retired)
// Adafruit version: https://www.adafruit.com/product/1222
#elif defined(ARDUINO_AVR_GEMMA)
#define LOGGER_BOARD "Arduino Gemma"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/hardware/leonardo/ (Retired)
#elif defined(ARDUINO_AVR_LEONARDO)
#define LOGGER_BOARD "Arduino Leonardo"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://store-usa.arduino.cc/products/arduino-micro
#elif defined(ARDUINO_AVR_MICRO)
#define LOGGER_BOARD "Arduino Micro"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/boards/arduino-mini-05/ (Retired)
#elif defined(ARDUINO_AVR_MINI)
#define LOGGER_BOARD "Arduino Mini 05"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://store-usa.arduino.cc/products/arduino-nano
#elif defined(ARDUINO_AVR_NANO)
#define LOGGER_BOARD "Arduino Nano"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://store-usa.arduino.cc/products/arduino-uno-rev3
#elif defined(ARDUINO_AVR_UNO)
#define LOGGER_BOARD "Arduino Uno"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/retired/boards/arduino-yun/ (Retired)
#elif defined(ARDUINO_AVR_YUN)
#define LOGGER_BOARD "Arduino Yun"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for an AVR processor
// Use log buffer size defaults
// An external RTC is required

// https://docs.arduino.cc/hardware/zero/
#elif defined(ARDUINO_SAMD_ZERO)
#define LOGGER_BOARD "Arduino Zero"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Use ADC defaults for a SAMD processor
// Use log buffer size defaults
// The processor can be used as an RTC, but the user must manually select it


#endif

// Default ADC settings for unknown processors
// These provide fallbacks when board-specific settings aren't available
#ifndef MS_PROCESSOR_ADC_RESOLUTION
// Fallback ADC resolution based on processor architecture
#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
#define MS_PROCESSOR_ADC_RESOLUTION 10
#elif defined(ARDUINO_ARCH_SAMD)
#define MS_PROCESSOR_ADC_RESOLUTION 12
#else
#define MS_PROCESSOR_ADC_RESOLUTION 10  // Conservative default
#endif
#endif

#ifndef MS_PROCESSOR_ADC_REFERENCE_MODE
// Fallback ADC reference mode based on processor architecture
#if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
#define MS_PROCESSOR_ADC_REFERENCE_MODE DEFAULT
#elif defined(ARDUINO_ARCH_SAMD)
#define MS_PROCESSOR_ADC_REFERENCE_MODE AR_DEFAULT
#else
#define MS_PROCESSOR_ADC_REFERENCE_MODE DEFAULT  // Conservative default
#endif
#endif

#ifndef MS_LOG_DATA_BUFFER_SIZE
// Fallback log buffer size based on processor type
#if defined(__SAMD51__)
#define MS_LOG_DATA_BUFFER_SIZE 8192
#elif defined(ARDUINO_ARCH_SAMD)
#define MS_LOG_DATA_BUFFER_SIZE 4096
#elif defined(__AVR_ATmega1284P__)
#define MS_LOG_DATA_BUFFER_SIZE 1024  // 1284p has good memory
#elif defined(__AVR_ATmega2560__)
#define MS_LOG_DATA_BUFFER_SIZE 512  // Mega has moderate memory
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#define MS_LOG_DATA_BUFFER_SIZE 256  // 328p has limited memory
#else
#define MS_LOG_DATA_BUFFER_SIZE 1024  // Conservative default
#endif
#endif

// Print warnings if expected processor defines are missing
#ifndef LOGGER_BOARD
#define LOGGER_BOARD "Unknown"
#pragma message "Warning: LOGGER_BOARD is not defined for this processor.\n" \
                "The board name can be added by editing KnownProcessors.h."
#endif
#ifndef OPERATING_VOLTAGE
#define OPERATING_VOLTAGE 3.3
#pragma message                                                                                \
    "Warning: OPERATING_VOLTAGE is not defined for this processor.\n"                          \
    "If you have specified the operating voltage in your code, you can ignore this message.\n" \
    "The operating voltage can be added by editing KnownProcessors.h."
#endif
#ifndef BATTERY_PIN
#define BATTERY_PIN -1
#pragma message                                                                          \
    "Warning: BATTERY_PIN is not defined for this processor.\n"                          \
    "If your processor does not have a built-in pin for measuring the battery voltage, " \
    "or you have specified a different pin in your code, you can ignore this message.\n" \
    "The battery pin can be added by editing KnownProcessors.h."
#endif

#ifndef BATTERY_MULTIPLIER
#define BATTERY_MULTIPLIER -1
#pragma message                                                                          \
    "Warning: BATTERY_MULTIPLIER is not defined for this processor.\n"                   \
    "If your processor does not have a built-in pin for measuring the battery voltage, " \
    "or you have specified the multiplier in your code, you can ignore this message.\n"  \
    "The battery multiplier can be added by editing KnownProcessors.h."
#endif

#endif

// cSpell:words Tatu Moja Adalogger Duemilanove Esplora
