/**
 * @file ProcessorStats.h
 * @copyright 2020 Stroud Water Research Center
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
 * @defgroup processor_group Processor Metadata
 * Classes for the @ref processor_sensor_page
 *
 * @copydoc processor_sensor_page
 *
 * @ingroup the_sensors
 */
/* clang-format on */
/* clang-format off */
/**
 * @page processor_sensor_page Processor Metadata
 *
 * @tableofcontents
 *
 * @section processor_intro Introduction
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
 * @section processor_datasheet Sensor Datasheet
 * - [Atmel ATmega1284P Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet-Summary.pdf)
 * - [Atmel ATmega1284P Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega1284P-Datasheet.pdf)
 * - [Atmel SAMD21 Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet-Summary.pdf)
 * - [Atmel SAMD21 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-SAMD21-Datasheet.pdf)
 * - [Atmel ATmega16U4 32U4 Datasheet Summary](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet-Summary.pdf)
 * - [Atmel ATmega16U4 32U4 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Processor-Datasheets/Atmel-ATmega16U4-32U4-Datasheet.pdf)
 *
 * @section processor_sensor The Processor as a Sensor
 * @ctor_doc{ProcessorStats, const char* version}
 * @subsection processor_sensor_timing Sensor Timing
 * - Timing variables do not apply to the processor in the same way they do to
 * other sensors.
 *
 * @section processor_battery Battery Voltage
 * This is the voltage as measured on the battery attached to the MCU using the
 * inbuilt ADC, if applicable.
 * @variabledoc{ProcessorStats,Battery}
 * - Range is assumed to be 0 to 5V
 * - Accuracy is processor dependent
 * - Result stored in sensorValues[0]
 * - Resolution is 0.005V
 *   - 0-5V with a 10bit ADC
 * - Reported as volts (V)
 * - Default variable code is batteryVoltage
 *
 * @section processor_ram Free RAM
 * This is the amount of free space on the processor when running the program.
 * This is just a diagnostic value.  This number _**should always remain the
 * same for a single logger program**_.  If this number is not constant over
 * time, there is a memory leak and something wrong with your logging program.
 * @variabledoc{ProcessorStats,FreeRam}
 * - Range is 0 to full RAM available on processor
 * - Result stored in sensorValues[1]
 * - Resolution is 1 bit
 * - Reported in bits
 * - Default variable code is freeSRAM
 *
 * @section processor_sampno Sample Number
 * This is a board diagnostic.  It is _**roughly**_ the number of samples
 * measured since the processor was last restarted.  This value simply
 * increments up by one every time the addSingleMeasurementResult() function is
 * called for the processor sensor.  It is intended only as a rough diagnostic
 * to show when the processor restarts.
 * @variabledoc{ProcessorStats,SampleNumber}
 * - Result stored in sensorValues[2]
 * - Reported as a dimensionless sequence number
 * - Default variable code is SampNum

 * ___
 * @section processor_sensor_examples Example Code
 * The processor is used as a sensor in all of the examples, including the
 * @menulink{processor_sensor} example.
 *
 * @menusnip{processor_sensor}
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

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the processor can report 3 values.
#define PROCESSOR_NUM_VARIABLES 3

/**
 * @brief Sensor::_warmUpTime_ms; the processor is never powered down - there is
 * no waiting for the processor to warmup.
 */
#define PROCESSOR_WARM_UP_TIME_MS 0
/**
 * @brief Sensor::_stabilizationTime_ms; the processor is never powered down -
 * there is no waiting for the processor to stabilize.
 */
#define PROCESSOR_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the processor measurement times aren't
 * measurable
 */
#define PROCESSOR_MEASUREMENT_TIME_MS 0

/// Decimals places in string representation; battery voltage should have 3.
#define PROCESSOR_BATTERY_RESOLUTION 3
/// Battery voltage is stored in sensorValues[0]
#define PROCESSOR_BATTERY_VAR_NUM 0

/// Decimals places in string representation; RAM should have 0.
#define PROCESSOR_RAM_RESOLUTION 0
/// Free RAM is stored in sensorValues[1]
#define PROCESSOR_RAM_VAR_NUM 1

/// Decimals places in string representation; sample number should have 0.
#define PROCESSOR_SAMPNUM_RESOLUTION 0
/// Sample number is stored in sensorValues[2]
#define PROCESSOR_SAMPNUM_VAR_NUM 2


// The main class for the Processor
// Only need a sleep and wake since these DON'T use the default of powering up
// and down
/**
 * @brief The main class to use the main processor (MCU) as a sensor.
 *
 * @ingroup processor_group
 */
class ProcessorStats : public Sensor {
 public:
    /**
     * @brief Construct a new Processor Stats object
     *
     * Need to know the Mayfly version because the battery resistor depends on
     * it
     *
     * @param version The version of the MCU, if applicable.
     * - For an EnviroDIY Mayfly, the version should be one of "v0.3", "v0.4",
     * "v0.5", or "v0.5b."  There *is* a difference between the versions!
     *
     * @note It is not possible to average more than one measurement for
     * processor variables - it just doesn't make sense for them.
     */
    explicit ProcessorStats(const char* version);
    /**
     * @brief Destroy the Processor Stats object
     *
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
    int16_t     sampNum;
};


/**
 * @brief The Variable sub-class used for the
 * [battery voltage output](@ref processor_battery) measured by the processor's
 * on-board ADC.
 *
 * @ingroup processor_group
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
    explicit ProcessorStats_Battery(ProcessorStats* parentSense,
                                    const char*     uuid    = "",
                                    const char*     varCode = "Battery")
        : Variable(parentSense, (const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                   (uint8_t)PROCESSOR_BATTERY_RESOLUTION, "batteryVoltage",
                   "volt", varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_Battery object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_Battery()
        : Variable((const uint8_t)PROCESSOR_BATTERY_VAR_NUM,
                   (uint8_t)PROCESSOR_BATTERY_RESOLUTION, "batteryVoltage",
                   "volt", "Battery") {}
    /**
     * @brief Destroy the ProcessorStats_Battery object - no action needed.
     */
    ~ProcessorStats_Battery() {}
};


/**
 * @brief The Variable sub-class used for the
 * [free RAM](@ref processor_ram) measured by the MCU.
 *
 * This is the amount of free space on the processor when running the program.
 * This is just a diagnostic value.  This number _**should always remain the
 * same for a single logger program**_.  If this number is not constant over
 * time, there is a memory leak and something wrong with your logging program.
 *
 * @ingroup processor_group
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
    explicit ProcessorStats_FreeRam(ProcessorStats* parentSense,
                                    const char*     uuid    = "",
                                    const char*     varCode = "FreeRam")
        : Variable(parentSense, (const uint8_t)PROCESSOR_RAM_VAR_NUM,
                   (uint8_t)PROCESSOR_RAM_RESOLUTION, "freeSRAM", "Bit",
                   varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_FreeRam object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_FreeRam()
        : Variable((const uint8_t)PROCESSOR_RAM_VAR_NUM,
                   (uint8_t)PROCESSOR_RAM_RESOLUTION, "freeSRAM", "Bit",
                   "FreeRam") {}
    /**
     * @brief Destroy the ProcessorStats_FreeRam object - no action needed.
     */
    ~ProcessorStats_FreeRam() {}
};


/**
 * @brief The Variable sub-class used for the
 * [sample number output](@ref processor_sampno) from the main processor.
 *
 * This is a board diagnostic.  It is _**roughly**_ the number of samples
 * measured since the processor was last restarted.  This value simply
 * increments up by one every time the addSingleMeasurementResult() function is
 * called for the processor sensor.  It is intended only as a rough diagnostic
 * to show when the processor restarts.
 *
 * @ingroup processor_group
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
    explicit ProcessorStats_SampleNumber(ProcessorStats* parentSense,
                                         const char*     uuid    = "",
                                         const char*     varCode = "SampNum")
        : Variable(parentSense, (const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                   (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION, "sequenceNumber",
                   "Dimensionless", varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorStats_SampleNumber object.
     *
     * @note This must be tied with a parent ProcessorStats before it can be
     * used.
     */
    ProcessorStats_SampleNumber()
        : Variable((const uint8_t)PROCESSOR_SAMPNUM_VAR_NUM,
                   (uint8_t)PROCESSOR_SAMPNUM_RESOLUTION, "sequenceNumber",
                   "Dimensionless", "SampNum") {}
    /**
     * @brief Destroy the ProcessorStats_SampleNumber() object - no action
     * needed.
     */
    ~ProcessorStats_SampleNumber() {}
};

#endif  // SRC_SENSORS_PROCESSORSTATS_H_
