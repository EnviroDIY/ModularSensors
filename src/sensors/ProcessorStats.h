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
 *
 * @defgroup processor_group Processor Metadata
 * Classes for the @ref processor_sensor_page
 *
 * @copydoc processor_sensor_page
 */

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
/// Sensor::_warmUpTime_ms; the processor is never powered down - there is now
/// waiting for warmup.
#define PROCESSOR_WARM_UP_TIME_MS 0
/// Sensor::_stabilizationTime_ms; the processor is never powered down - there
/// is now waiting for stabilization.
#define PROCESSOR_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the processor measurement times aren't
/// measurable
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
 *
 * @see @ref processor_sensor_page
 */
class ProcessorStats : public Sensor {
 public:
    /**
     * @brief Construct a new Processor Stats object
     *
     * Need to know the Mayfly version because the battery resistor depends on
     * it
     *
     * @param version The version of the MCU, if applicable
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
 * @brief The variable class used for the battery supplying power to the
 * processor as measured by the processor itself.
 *
 * @ingroup processor_group
 *
 * @see @ref processor_sensor_page
 */
class ProcessorStats_Battery : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_Battery object.
     *
     * @param parentSense The parent ProcessorStats providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is batteryVoltage.
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
 * @brief The variable class used for "Free Ram" measured by the MCU.
 *
 * This is a board diagnostic.
 *
 * @ingroup processor_group
 *
 * @see @ref processor_sensor_page
 */
class ProcessorStats_FreeRam : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_FreeRam object.
     *
     * @param parentSense The parent ProcessorStats providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is FreeRam
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
 * @brief The variable class used for "Sample Number" on the main processor.
 *
 * This is a board diagnostic.
 *
 * @ingroup processor_group
 *
 * @see @ref processor_sensor_page
 */
class ProcessorStats_SampleNumber : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorStats_SampleNumber object.
     *
     * @param parentSense The parent ProcessorStats providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is SampNum
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
