/**
 * @file KnownProcessors.h
 * @copyright Stroud Water Research Center
 * @author Sara Damiano (sdamiano@stroudcenter.org)
 *
 * @brief Defines for parameters of known processor types for the EnviroDIY
 * ModularSensors library
 */

// Header Guards
#ifndef SRC_SENSORS_KNOWN_PROCESSORS_H_
#define SRC_SENSORS_KNOWN_PROCESSORS_H_

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
 */

// EnviroDIY boards
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
#define LOGGER_BOARD "EnviroDIY Mayfly"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN A6
#define BATTERY_MULTIPLIER 4.7  // for v0.5 and later
/// @brief The power pin for the ALS on the EnviroDIY Mayfly v1.x
#define BUILT_IN_ALS_POWER_PIN -1
/// @brief The data pin for the ALS on the EnviroDIY Mayfly v1.x
#define BUILT_IN_ALS_DATA_PIN A4
/// @brief The supply voltage for the ALS on the EnviroDIY Mayfly v1.x
#define BUILT_IN_ALS_SUPPLY_VOLTAGE 3.3
/// @brief The loading resistance for the ALS on the EnviroDIY Mayfly v1.x
#define BUILT_IN_ALS_LOADING_RESISTANCE 10
#elif defined(ENVIRODIY_STONEFLY_M4)
#define LOGGER_BOARD "EnviroDIY Stonefly"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN A9
#define BATTERY_MULTIPLIER 4.7
/// @brief The power pin for the ALS on the EnviroDIY Stonefly v0.x
#define BUILT_IN_ALS_POWER_PIN -1
/// @brief The data pin for the ALS on the EnviroDIY Stonefly v0.x
#define BUILT_IN_ALS_DATA_PIN A8
/// @brief The supply voltage for the ALS on the EnviroDIY Stonefly v0.x
#define BUILT_IN_ALS_SUPPLY_VOLTAGE 3.3
/// @brief The loading resistance for the ALS on the EnviroDIY Stonefly v0.x
#define BUILT_IN_ALS_LOADING_RESISTANCE 10

// Sodaq boards
#elif defined(ARDUINO_SODAQ_EXPLORER)
#define LOGGER_BOARD "SODAQ ExpLoRer"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_SODAQ_AUTONOMO)
#define LOGGER_BOARD "SODAQ Autonomo"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 48  // for version v0.1
#define BATTERY_MULTIPLIER 1.47
#elif defined(ARDUINO_SODAQ_ONE_BETA)
#define LOGGER_BOARD "SODAQ ONE Beta"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 10
#define BATTERY_MULTIPLIER 2  // for version v0.1
#elif defined(ARDUINO_SODAQ_ONE)
#define LOGGER_BOARD "SODAQ ONE"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 10
#define BATTERY_MULTIPLIER 2  // for version v0.1
#elif defined(ARDUINO_AVR_SODAQ_MBILI)
#define LOGGER_BOARD "SODAQ Mbili"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN A6
#define BATTERY_MULTIPLIER 1.47
#elif defined(ARDUINO_AVR_SODAQ_NDOGO)
#define LOGGER_BOARD "SODAQ Ndogo"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 10
#define BATTERY_MULTIPLIER 1.47
#elif defined(ARDUINO_AVR_SODAQ_TATU)
#define LOGGER_BOARD "SODAQ Tatu"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_SODAQ_MOJA)
#define LOGGER_BOARD "SODAQ Moja"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1

// Adafruit boards
#elif defined(ARDUINO_AVR_FEATHER328P)
#define LOGGER_BOARD "Adafruit Feather 328p"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
#elif defined(ARDUINO_AVR_FEATHER32U4)
#define LOGGER_BOARD "Adafruit Feather 32u4"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS) || \
    defined(ADAFRUIT_FEATHER_M0_EXPRESS)
#define LOGGER_BOARD "Adafruit Feather M0 Express"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
#elif defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
#define LOGGER_BOARD "Adafruit Feather M0"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
#elif defined(ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define LOGGER_BOARD "Adafruit Feather M4"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
#elif defined(ARDUINO_FEATHER_M4_CAN) || defined(ADAFRUIT_FEATHER_M4_CAN)
#define LOGGER_BOARD "Feather M4 CAN"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
#elif defined(ADAFRUIT_FEATHER_M4_ADALOGGER)
#define LOGGER_BOARD "Adafruit Feather M4 Adalogger"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN 9
#define BATTERY_MULTIPLIER 2
#elif defined(ADAFRUIT_GRAND_CENTRAL_M4)
#define LOGGER_BOARD "Adafruit Grand Central"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1

// Arduino boards
#elif defined(ARDUINO_AVR_ADK)
#define LOGGER_BOARD "Arduino Mega ADK"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
// Bluetooth
#elif defined(ARDUINO_AVR_BT)
#define LOGGER_BOARD "Arduino BT"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_DUEMILANOVE)
#define LOGGER_BOARD "Arduino Duemilanove"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_ESPLORA)
#define LOGGER_BOARD "Arduino Esplora"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_ETHERNET)
#define LOGGER_BOARD "Arduino Ethernet"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_FIO)
#define LOGGER_BOARD "Arduino Fio"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_GEMMA)
#define LOGGER_BOARD "Arduino Gemma"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_LEONARDO)
#define LOGGER_BOARD "Arduino Leonardo"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_LILYPAD)
#define LOGGER_BOARD "Arduino Lilypad"
// NOTE: The operating voltage is 2.7-5.5V
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_LILYPAD_USB)
#define LOGGER_BOARD "Arduino Lilypad USB"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_MEGA)
#define LOGGER_BOARD "Arduino Mega"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_MEGA2560)
#define LOGGER_BOARD "Arduino Mega 2560"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_MICRO)
#define LOGGER_BOARD "Arduino Micro"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_MINI)
#define LOGGER_BOARD "Arduino Mini 05"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_NANO)
#define LOGGER_BOARD "Arduino Nano"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_NG)
#define LOGGER_BOARD "Arduino NG"
// WARNING: I can't find confirmation of the operating voltage online!
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_PRO)
#define LOGGER_BOARD "Arduino Pro"
// NOTE: The operating voltage is 3.3V or 5V depending on the model
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_UNO)
#define LOGGER_BOARD "Arduino Uno"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_AVR_YUN)
#define LOGGER_BOARD "Arduino Yun"
#define OPERATING_VOLTAGE 5
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
#elif defined(ARDUINO_SAMD_ZERO)
#define LOGGER_BOARD "Arduino Zero"
#define OPERATING_VOLTAGE 3.3
#define BATTERY_PIN -1
#define BATTERY_MULTIPLIER -1
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
    "If you have specified the operating voltage in your code, you can ignore this message\n." \
    "The operating voltage can be added by editing KnownProcessors.h."
#endif
#ifndef BATTERY_PIN
#define BATTERY_PIN -1
#pragma message                                                                          \
    "Warning: BATTERY_PIN is not defined for this processor.\n"                          \
    "If your processor does not have a built-in pin for measuring the battery voltage,"  \
    "or you have specified a different pin in your code, you can ignore this message\n." \
    "The battery pin can be added by editing KnownProcessors.h."
#endif

#ifndef BATTERY_MULTIPLIER
#define BATTERY_MULTIPLIER -1
#pragma message                                                                         \
    "Warning: BATTERY_MULTIPLIER is not defined for this processor.\n"                  \
    "If your processor does not have a built-in pin for measuring the battery voltage," \
    "or you have specified the multiplier in your code, you can ignore this message\n." \
    "The battery multiplier can be added by editing KnownProcessors.h."
#endif

#endif
