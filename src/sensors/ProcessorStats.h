/**
 * @file ProcessorStats.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the ProcessorStats sensor subclass and the variable
 * subclasses ProcessorStats_Battery, ProcessorStats_FreeRam, and
 * ProcessorStats_SampleNumber.
 *
 * These are for metadata on the processor functionality.
 */
/* clang-format off */
/**
 * @defgroup sensor_processor Processor Metadata
 * Classes for the using the processor as a sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_processor_intro Introduction
 *
 * The processor can return the number of "samples" it has taken, the amount of
 * RAM it has available and, for some boards, the battery voltage (EnviroDIY
 * Mayfly, Sodaq Mbili, Ndogo, Autonomo, and One, Adafruit Feathers).  The
 * version of the board is required as input (ie, for a EnviroDIY Mayfly: "v0.3"
 * or "v0.4" or "v0.5").  Use a blank value (ie, "") for un-versioned boards.
 * Please note that while you cannot opt to average more than one sample, it really
 * makes no sense to do so for the processor.  These values are only intended to be
 * used as diagnostics.
 *
 * @section sensor_processor_datasheet Sensor Datasheet
 * - [Atmel ATmega1284P Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet-Summary.pdf)
 * - [Atmel ATmega1284P Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet.pdf)
 * - [Atmel SAMD21 Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet-Summary.pdf)
 * - [Atmel SAMD21 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet.pdf)
 * - [Atmel ATmega16U4 32U4 Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet-Summary.pdf)
 * - [Atmel ATmega16U4 32U4 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet.pdf)
 *
 * @section sensor_processor_sensor_ctor Sensor Constructor
 * {{ @ref ProcessorStats::ProcessorStats }}
 *
 * ___
 * @section sensor_processor_sensor_examples Example Code
 * The processor is used as a sensor in all of the examples, including the
 * @menulink{processor_stats} example.
 *
 * @menusnip{processor_stats}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_PROCESSORSTATS_H_
#define SRC_SENSORS_PROCESSORSTATS_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_PROCESSORSTATS_DEBUG
#define MS_DEBUGGING_STD "ProcessorStats"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_processor */
/**@{*/

/**
 * @anchor sensor_processor_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by the main processor
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the processor can report 4 values:
/// battery, free ram, sample number, and reset cause.
#define PROCESSOR_NUM_VARIABLES 4
/// @brief Sensor::_incCalcValues; sample number is (sort-of) calculated.
#define PROCESSOR_INC_CALC_VARIABLES 1
/**@}*/

/**
 * @anchor sensor_processor_sensor_timing
 * @name Sensor Timing
 * The sensor timing for the processor/mcu
 * - Timing variables do not apply to the processor in the same way they do to
 * other sensors.
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the processor is never powered down - there
/// is no waiting for the processor to warmup.
#define PROCESSOR_WARM_UP_TIME_MS 0
/// @brief Sensor::_stabilizationTime_ms; the processor is never powered down -
/// there is no waiting for the processor to stabilize.
#define PROCESSOR_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; the processor measurement times aren't
/// measurable.
#define PROCESSOR_MEASUREMENT_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_processor_battery
 * @name Battery Voltage
 * The battery voltage variable from the processor/mcu
 * This is the voltage as measured on the battery attached to the MCU using the
 * inbuilt ADC, if applicable.
 * - Range is assumed to be 0 to 5V
 * - Accuracy is processor dependent
 */
/**@{*/
/**
 * @brief Decimals places in string representation; battery voltage should
 * have 3.
 *
 * The resolution is of the EnviroDIY Mayfly is 0.005V, we will use that
 * resolution for all processors.
 *
 * {{ @ref ProcessorStats_Battery::ProcessorStats_Battery }}
 */
#define PROCESSOR_BATTERY_RESOLUTION 3
/// @brief Battery voltage is stored in sensorValues[0]
#define PROCESSOR_BATTERY_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// batteryVoltage
#define PROCESSOR_BATTERY_VAR_NAME "batteryVoltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define PROCESSOR_BATTERY_UNIT_NAME "volt"
/// @brief Default variable short code; "Battery"
#define PROCESSOR_BATTERY_DEFAULT_CODE "Battery"
/**@}*/

/**
 * @anchor sensor_processor_ram
 * @name Available RAM
 * The RAM variable from the processor/mcu
 * This is the amount of free space on the processor when running the program.
 * This is just a diagnostic value.  This number _**should always remain the
 * same for a single logger program**_.  If this number is not constant over
 * time, there is a memory leak and something wrong with your logging program.
 * - Range is 0 to full RAM available on processor
 *
 * {{ @ref ProcessorStats_FreeRam::ProcessorStats_FreeRam }}
 */
/**@{*/
/// @brief Decimals places in string representation; ram should have 0 -
/// resolution is 1 bit.
#define PROCESSOR_RAM_RESOLUTION 0
/// @brief Free RAM is stored in sensorValues[1]
#define PROCESSOR_RAM_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// freeSRAM
#define PROCESSOR_RAM_VAR_NAME "freeSRAM"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "Bit"
#define PROCESSOR_RAM_UNIT_NAME "Bit"
/// @brief Default variable short code; "FreeRam"
#define PROCESSOR_RAM_DEFAULT_CODE "FreeRam"
/**@}*/

/**
 * @anchor sensor_processor_sampno
 * @name Sample Number
 * The sample number variable from the processor/mcu
 *
 * @note This is a board diagnostic.  It is _**roughly**_ the number of samples
 * measured since the processor was last restarted.  This value simply
 * increments up by one every time the addSingleMeasurementResult() function is
 * called for the processor sensor.  It is intended only as a rough diagnostic
 * to show when the processor restarts.
 *
 * {{ @ref ProcessorStats_SampleNumber::ProcessorStats_SampleNumber }}
 */
/**@{*/
/// @brief Decimals places in string representation; sample number should have
/// 0 - resolution is 1.
#define PROCESSOR_SAMPNUM_RESOLUTION 0
/// @brief Sample number is stored in sensorValues[2]
#define PROCESSOR_SAMPNUM_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// sequenceNumber
#define PROCESSOR_SAMPNUM_VAR_NAME "sequenceNumber"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "Dimensionless" (sequence number)
#define PROCESSOR_SAMPNUM_UNIT_NAME "Dimensionless"
/// @brief Default variable short code; "SampNum"
#define PROCESSOR_SAMPNUM_DEFAULT_CODE "SampNum"
/**@}*/


/**
 * @anchor sensor_processor_reset
 * @name Cause of last processor reset.
 * The reset cause code variable from the processor/mcu.
 * This value only changes when the board is reset.
 * You must look up the meaning of the code in the processor datasheet.
 *
 * @warning Not provided by all processors.
 *
 * {{ @ref ProcessorStats_ResetCode::ProcessorStats_ResetCode }}
 */
/**@{*/
/// @brief Decimals places in string representation; ram should have 0 -
/// it's just a code
#define PROCESSOR_RESET_RESOLUTION 0
/// @brief Free RAM is stored in sensorValues[1]
#define PROCESSOR_RESET_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "instrumentStatusCode"
#define PROCESSOR_RESET_VAR_NAME "instrumentStatusCode"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "dimensionless"
#define PROCESSOR_RESET_UNIT_NAME "dimensionless"
/// @brief Default variable short code; "ResetCode"
#define PROCESSOR_RESET_DEFAULT_CODE "ResetCode"
/**@}*/

/**
 * @def LOGGER_BOARD
 * @brief Pretty text for the board name derived from the board's compiler
 * define.
 */

// EnviroDIY boards
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
#define LOGGER_BOARD "EnviroDIY Mayfly"
#elif defined(ENVIRODIY_STONEFLY_M4)
#define LOGGER_BOARD "EnviroDIY Stonefly"

// Sodaq boards
#elif defined(ARDUINO_SODAQ_EXPLORER)
#define LOGGER_BOARD "SODAQ ExpLoRer"
#elif defined(ARDUINO_SODAQ_AUTONOMO)
#define LOGGER_BOARD "SODAQ Autonomo"
#elif defined(ARDUINO_SODAQ_ONE_BETA)
#define LOGGER_BOARD "SODAQ ONE Beta"
#elif defined(ARDUINO_SODAQ_ONE)
#define LOGGER_BOARD "SODAQ ONE"
#elif defined(ARDUINO_AVR_SODAQ_MBILI)
#define LOGGER_BOARD "SODAQ Mbili"
#elif defined(ARDUINO_AVR_SODAQ_NDOGO)
#define LOGGER_BOARD "SODAQ Ndogo"
#elif defined(ARDUINO_AVR_SODAQ_TATU)
#define LOGGER_BOARD "SODAQ Tatu"
#elif defined(ARDUINO_AVR_SODAQ_MOJA)
#define LOGGER_BOARD "SODAQ Moja"

// Adafruit boards
#elif defined(ARDUINO_AVR_FEATHER328P)
#define LOGGER_BOARD "Feather 328p"
#elif defined(ARDUINO_AVR_FEATHER32U4)
#define LOGGER_BOARD "Feather 32u4"
#elif defined(ARDUINO_SAMD_FEATHER_M0_EXPRESS) || \
    defined(ADAFRUIT_FEATHER_M0_EXPRESS)
#define LOGGER_BOARD "Feather M0 Express"
#elif defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
#define LOGGER_BOARD "Feather M0"
#elif defined(ADAFRUIT_GRAND_CENTRAL_M4)
#define LOGGER_BOARD "Grand Central"
#elif defined(ADAFRUIT_FEATHER_M4_ADALOGGER)
#define LOGGER_BOARD "Feather M4 Adalogger"
#elif defined(ARDUINO_FEATHER_M4_CAN) || defined(ADAFRUIT_FEATHER_M4_CAN)
#define LOGGER_BOARD "Feather M4 CAN"
#elif defined(ARDUINO_FEATHER_M4) || defined(ADAFRUIT_FEATHER_M4_EXPRESS)
#define LOGGER_BOARD "Feather M4"

// Arduino boards
#elif defined(ARDUINO_AVR_ADK)
#define LOGGER_BOARD "Mega Adk"
// Bluetooth
#elif defined(ARDUINO_AVR_BT)
#define LOGGER_BOARD "Bt"
#elif defined(ARDUINO_AVR_DUEMILANOVE)
#define LOGGER_BOARD "Duemilanove"
#elif defined(ARDUINO_AVR_ESPLORA)
#define LOGGER_BOARD "Esplora"
#elif defined(ARDUINO_AVR_ETHERNET)
#define LOGGER_BOARD "Ethernet"
#elif defined(ARDUINO_AVR_FIO)
#define LOGGER_BOARD "Fio"
#elif defined(ARDUINO_AVR_GEMMA)
#define LOGGER_BOARD "Gemma"
#elif defined(ARDUINO_AVR_LEONARDO)
#define LOGGER_BOARD "Leonardo"
#elif defined(ARDUINO_AVR_LILYPAD)
#define LOGGER_BOARD "Lilypad"
#elif defined(ARDUINO_AVR_LILYPAD_USB)
#define LOGGER_BOARD "Lilypad Usb"
#elif defined(ARDUINO_AVR_MEGA)
#define LOGGER_BOARD "Mega"
#elif defined(ARDUINO_AVR_MEGA2560)
#define LOGGER_BOARD "Mega 2560"
#elif defined(ARDUINO_AVR_MICRO)
#define LOGGER_BOARD "Micro"
#elif defined(ARDUINO_AVR_MINI)
#define LOGGER_BOARD "Mini"
#elif defined(ARDUINO_AVR_NANO)
#define LOGGER_BOARD "Nano"
#elif defined(ARDUINO_AVR_NG)
#define LOGGER_BOARD "NG"
#elif defined(ARDUINO_AVR_PRO)
#define LOGGER_BOARD "Pro"
#elif defined(ARDUINO_AVR_ROBOT_CONTROL)
#define LOGGER_BOARD "Robot Ctrl"
#elif defined(ARDUINO_AVR_ROBOT_MOTOR)
#define LOGGER_BOARD "Robot Motor"
#elif defined(ARDUINO_AVR_UNO)
#define LOGGER_BOARD "Uno"
#elif defined(ARDUINO_AVR_YUN)
#define LOGGER_BOARD "Yun"
#elif defined(ARDUINO_SAMD_ZERO)
#define LOGGER_BOARD "Zero"

#else
#define LOGGER_BOARD "Unknown"
#endif


// The main class for the Processor
// Only need a sleep and wake since these DON'T use the default of powering
// up and down
/**
 * @brief The main class to use the main processor (MCU) as a sensor.
 *
 * @ingroup sensor_processor
 */
class ProcessorStats : public Sensor {
 public:
    /**
     * @brief Construct a new Processor Stats object for a **known, unmodified
     * development board** using the standard manufacturer core for that board.
     *
     * Boards that can be used with this constructor:
     * - EnviroDIY
     *   - Mayfly
     *     - the version must be one of "v0.3", "v0.4", "v0.5", "v0.5b", "v1.0",
     * or "v1.1"
     *   - Stonefly
     *     - the version must be "v0.1"
     * - Adafruit
     *   - Feather M0 variants (M0, M0 Express, M0 Adalogger, etc)
     *   - Feather M4 variants
     *   - Feather 328p variants
     *     - WARNING: The processor isn't powerful enough for this library. To
     * use it, you would have to strip the library down.
     *   - Feather 32U4 variants (Basic proto, RadioFruit, BlueFruit, etc)
     *     - WARNING: The processor isn't powerful enough for this library. To
     * use it, you would have to strip the library down.
     * - Sodaq
     *   - Mbili
     *   - Ndogo
     *   - One
     *     - the version must be "v0.1" or "v0.2"
     *   - Autonomo
     *     - the version must be "v0.1"
     *
     * @param version The version of the MCU, if applicable. This is used to
     * fill in the correct battery connection information.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     *
     * @note The measurements to average will only be applied to the battery
     * voltage measurement!
     */
    ProcessorStats(const char* version, uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new Processor Stats object for any processor type
     *
     * @param boardName The name of the board. For many boards, you can use the
     * defined LOGGER_BOARD value. Whatever you type here will be printed out as
     * part of the location for the processor. It has no other effect.
     * @param version The version of the MCU, if applicable. Whatever you type
     * here will be printed out as part of the location for the processor. It
     * has no other effect.
     * @param batteryPin The analog pin on the processor connected to the
     * battery.
     * @param batteryMultiplier Any multiplier needed to convert raw battery
     * readings from `analogRead()` into true battery values based on any
     * resistors or voltage dividers
     * @param operatingVoltage The processor's operating voltage; most
     * likely 3.3 or 5.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     *
     * @note The measurements to average will only be applied to the battery
     * voltage measurement!
     */
    ProcessorStats(const char* boardName, const char* version,
                   int8_t batteryPin, float batteryMultiplier,
                   float operatingVoltage, uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the Processor Stats object
     */
    ~ProcessorStats();

    /**
     * @copydoc Sensor::getSensorLocation()
     *
     * This returns the processor name as read from the compiler variable.
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

    /**
     * @brief A helper to get battery voltage as measured by a direct connection
     * between the battery and a processor analog pin.
     *
     * @return The battery voltage in volts
     */
    float getBatteryVoltage(void);

/**
 * @brief Get the processor code for the last reset cause
 *
 * @return The processor code for the last reset cause
 */
#if !defined(__SAMD51__)
    uint8_t getLastResetCode(void);
#else
    uint16_t getLastResetCode(void);
#endif
    /**
     * @brief Get the cause of the last reset as a string description.
     *
     * @return A string describing the last reset cause
     */
    String getLastResetCause(void);

 private:
    const char* _version;      ///< Internal reference to the board version
    const char* _boardName;    ///< Internal reference to the board name
    int8_t      _batteryPin;   ///< Internal reference to the battery pin
    float _batteryMultiplier;  ///< Internal reference to any multiplier needed
                               ///< to convert raw battery readings into true
                               ///< battery values based on any resistors or
                               ///< voltage dividers
    float _operatingVoltage;   ///< Internal reference to processor's operating
                               ///< voltage
    int16_t sampNum = 0;       ///< The current sample number
};


/**
 * @brief The Variable sub-class used for the
 * [battery voltage output](@ref sensor_processor_battery) measured by the
 * processor's on-board ADC.
 *
 * @ingroup sensor_processor
 */
class ProcessorStats_Battery : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_Battery object.
     *
     * @param parentSense The parent ProcessorStats providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "batteryVoltage".
     */
    explicit ProcessorStats_Battery(
        ProcessorStats* parentSense, const char* uuid = "",
        const char* varCode = PROCESSOR_BATTERY_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                   (uint8_t)PROCESSOR_BATTERY_RESOLUTION,
                   PROCESSOR_BATTERY_VAR_NAME, PROCESSOR_BATTERY_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_Battery object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_Battery()
        : Variable((uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                   (uint8_t)PROCESSOR_BATTERY_RESOLUTION,
                   PROCESSOR_BATTERY_VAR_NAME, PROCESSOR_BATTERY_UNIT_NAME,
                   PROCESSOR_BATTERY_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ProcessorStats_Battery object - no action needed.
     */
    ~ProcessorStats_Battery() {}
};


/**
 * @brief The Variable sub-class used for the
 * [free RAM](@ref sensor_processor_ram) measured by the MCU.
 *
 * This is the amount of free space on the processor when running the program.
 * This is just a diagnostic value.  This number _**should always remain the
 * same for a single logger program**_.  If this number is not constant over
 * time, there is a memory leak and something wrong with your logging program.
 * There is almost never a reason to store or transmit this value, but it is
 * helpful to check for debugging.
 *
 * @ingroup sensor_processor
 */
class ProcessorStats_FreeRam : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_FreeRam object.
     *
     * @param parentSense The parent ProcessorStats providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "FreeRam".
     * @note While this variable is included, the value of it should never
     * change.  If it does change, that's a sign of a memory leak in your
     * program which will eventually cause your board to crash.
     */
    explicit ProcessorStats_FreeRam(
        ProcessorStats* parentSense, const char* uuid = "",
        const char* varCode = PROCESSOR_RAM_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)PROCESSOR_RAM_VAR_NUM,
                   (uint8_t)PROCESSOR_RAM_RESOLUTION, PROCESSOR_RAM_VAR_NAME,
                   PROCESSOR_RAM_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_FreeRam object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_FreeRam()
        : Variable((uint8_t)PROCESSOR_RAM_VAR_NUM,
                   (uint8_t)PROCESSOR_RAM_RESOLUTION, PROCESSOR_RAM_VAR_NAME,
                   PROCESSOR_RAM_UNIT_NAME, PROCESSOR_RAM_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ProcessorStats_FreeRam object - no action needed.
     */
    ~ProcessorStats_FreeRam() {}
};


/**
 * @brief The Variable sub-class used for the
 * [sample number output](@ref sensor_processor_sampno) from the main processor.
 *
 * This is a board diagnostic.  It is _**roughly**_ the number of samples
 * measured since the processor was last restarted.  This value simply
 * increments up by one every time the addSingleMeasurementResult() function is
 * called for the processor sensor.  It is intended only as a rough diagnostic
 * to show when the processor restarts.
 *
 * @ingroup sensor_processor
 */
class ProcessorStats_SampleNumber : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_SampleNumber object.
     *
     * @param parentSense The parent ProcessorStats providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SampNum".
     */
    explicit ProcessorStats_SampleNumber(
        ProcessorStats* parentSense, const char* uuid = "",
        const char* varCode = PROCESSOR_SAMPNUM_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                   (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION,
                   PROCESSOR_SAMPNUM_VAR_NAME, PROCESSOR_SAMPNUM_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_SampleNumber object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_SampleNumber()
        : Variable((uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                   (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION,
                   PROCESSOR_SAMPNUM_VAR_NAME, PROCESSOR_SAMPNUM_UNIT_NAME,
                   PROCESSOR_SAMPNUM_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ProcessorStats_SampleNumber() object - no action
     * needed.
     */
    ~ProcessorStats_SampleNumber() {}
};


/**
 * @brief The Variable sub-class used for the
 * [reset cause](@ref sensor_processor_reset) recorded by the MCU.
 *
 * This is a code for the last processor reset cause.  This number _**only
 * changes when the processor resets**_.  There is almost never a reason to
 * store or transmit this value, but it is helpful to check for debugging.
 *
 * @ingroup sensor_processor
 */
class ProcessorStats_ResetCode : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_ResetCode object.
     *
     * @param parentSense The parent ProcessorStats providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "ResetCode".
     * @note While this variable is included, the value of it should never
     * change.  If it does change, that's a sign of a memory leak in your
     * program which will eventually cause your board to crash.
     */
    explicit ProcessorStats_ResetCode(
        ProcessorStats* parentSense, const char* uuid = "",
        const char* varCode = PROCESSOR_RESET_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)PROCESSOR_RESET_VAR_NUM,
                   (uint8_t)PROCESSOR_RESET_RESOLUTION,
                   PROCESSOR_RESET_VAR_NAME, PROCESSOR_RESET_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new ProcessorStats_ResetCode object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_ResetCode()
        : Variable((uint8_t)PROCESSOR_RESET_VAR_NUM,
                   (uint8_t)PROCESSOR_RESET_RESOLUTION,
                   PROCESSOR_RESET_VAR_NAME, PROCESSOR_RESET_UNIT_NAME,
                   PROCESSOR_RESET_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ProcessorStats_ResetCode object - no action needed.
     */
    ~ProcessorStats_ResetCode() {}
};
/**@}*/
#endif  // SRC_SENSORS_PROCESSORSTATS_H_

// cSpell:ignore SAMPNUM sampno Tatu Moja Adalogger Duemilanove Esplora Lilypad
