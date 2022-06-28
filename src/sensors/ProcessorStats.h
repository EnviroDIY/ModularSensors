/**
 * @file ProcessorStats.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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

// Debugging Statement
// #define MS_PROCESSORSTATS_DEBUG

#ifdef MS_PROCESSORSTATS_DEBUG
#define MS_DEBUGGING_STD "ProcessorStats"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_processor */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the processor can report 3 values.
#define PROCESSOR_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; sample number is (sort-of) calculated.
#define PROCESSOR_INC_CALC_VARIABLES 1


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
     * @brief Construct a new Processor Stats object.
     *
     * Need to know the Mayfly version because the battery resistor depends on
     * it
     *
     * @param version The version of the MCU, if applicable.
     * - For an EnviroDIY Mayfly, the version should be one of "v0.3", "v0.4",
     * "v0.5", "v0.5b", "v1.0", or "v1.1".  There *is* a difference between some
     * of the versions!
     *
     * @note It is not possible to average more than one measurement for
     * processor variables - it just doesn't make sense for them.
     */
    explicit ProcessorStats(const char* version);
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

 private:
    const char* _version;
    int8_t      _batteryPin;
    int16_t     sampNum = 0;
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
        : Variable(parentSense, (const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
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
        : Variable((const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
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
        : Variable(parentSense, (const uint8_t)PROCESSOR_RAM_VAR_NUM,
                   (uint8_t)PROCESSOR_RAM_RESOLUTION, PROCESSOR_RAM_VAR_NAME,
                   PROCESSOR_RAM_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_FreeRam object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_FreeRam()
        : Variable((const uint8_t)PROCESSOR_RAM_VAR_NUM,
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
        : Variable(parentSense, (const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
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
        : Variable((const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                   (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION,
                   PROCESSOR_SAMPNUM_VAR_NAME, PROCESSOR_SAMPNUM_UNIT_NAME,
                   PROCESSOR_SAMPNUM_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ProcessorStats_SampleNumber() object - no action
     * needed.
     */
    ~ProcessorStats_SampleNumber() {}
};
/**@}*/
#endif  // SRC_SENSORS_PROCESSORSTATS_H_
