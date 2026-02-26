/**
 * @file ProcessorAnalog.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief This file contains the ProcessorAnalog sensor subclass and the
 * ProcessorAnalog_Voltage variable subclass.
 *
 * These are used for any voltage measurable on an analog processor pin.  There
 * is a multiplier allowed for a voltage divider between the raw voltage and the
 * processor pin.
 */
/**
 * @defgroup sensor_processor_analog Processor Analog Voltage Sensor
 * Classes for simple external analog voltage measurements using the built-in
 * processor ADC.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_processor_analog_intro Introduction
 *
 * The Processor Analog module is used for any case where the voltage itself is
 * the desired value (as for an external battery) and the voltage will be
 * measured on a processor pin using the built-in ADC.  It can also be used in
 * combination with a calculated variable to support any other analog sensor not
 * explicitly supported by ModularSensors.  To increase the range available for
 * voltage measurements, this module supports the use of a voltage divider.
 *
 * @section sensor_processor_analog_flags Build flags
 * - `-D MS_PROCESSOR_ADC_RESOLUTION=##`
 *      - used to set the resolution of the processor ADC
 *      - @see #MS_PROCESSOR_ADC_RESOLUTION
 * - `-D MS_PROCESSOR_ADC_REFERENCE_MODE=xxx`
 *      - used to set the processor ADC value reference mode
 *      - @see #MS_PROCESSOR_ADC_REFERENCE_MODE
 *
 * @section sensor_processor_analog_ctor Sensor Constructor
 * {{ @ref ProcessorAnalog::ProcessorAnalog }}
 *
 * ___
 * @section sensor_processor_analog_examples Example Code
 * The processor analog voltage sensor is used in the
 * @menulink{processor_analog} example.
 *
 * @menusnip{processor_analog}
 */

// Header Guards
#ifndef SRC_SENSORS_PROCESSOR_ANALOG_H_
#define SRC_SENSORS_PROCESSOR_ANALOG_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Include the known processors for default values
#include "KnownProcessors.h"

// Define the print label[s] for the debugger
#ifdef MS_PROCESSOR_ANALOG_DEBUG
#define MS_DEBUGGING_STD "ProcessorAnalog"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include "AnalogVoltageBase.h"

/** @ingroup sensor_processor_analog */
/**@{*/

/**
 * @anchor sensor_processor_analog_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned from an analog reading on a
 * processor pin.
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; an analog reading on a processor pin is 1
/// value.
#define PROCESSOR_ANALOG_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define PROCESSOR_ANALOG_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_processor_analog_timing
 * @name Sensor Timing
 * The sensor timing for the processor ADC
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the processor ADC does not need to warm up,
#define PROCESSOR_ANALOG_WARM_UP_TIME_MS 0
/**
 * @brief Sensor::_stabilizationTime_ms; the processor ADC is stable 0ms after
 * warm-up - we assume a voltage is instantly ready.
 *
 * It's not really *quite* instantly, but it is very fast and the time to
 * measure is included in the read function.
 * On ATmega based boards (UNO, Nano, Mini, Mega), it takes about 100
 * microseconds (0.0001 s) to read an analog input, so the maximum reading rate
 * is about 10,000 times a second.
 */
#define PROCESSOR_ANALOG_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the processor ADC measurement time is
 * variable, but we assume it is effectively instant.
 */
#define PROCESSOR_ANALOG_MEASUREMENT_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_processor_analog_volt
 * @name Voltage
 * The voltage variable for the processor analog voltage sensor
 * - Range is dependent on the supply voltage and any voltage divider
 *
 * {{ @ref ProcessorAnalog_Voltage::ProcessorAnalog_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[0].
#define PROCESSOR_ANALOG_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define PROCESSOR_ANALOG_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define PROCESSOR_ANALOG_UNIT_NAME "volt"
/// @brief Default variable short code; "analogVoltage"
#define PROCESSOR_ANALOG_DEFAULT_CODE "analogVoltage"

#if MS_PROCESSOR_ADC_RESOLUTION == 12
/// @brief Decimals places in string representation; a 3.3V processor at 12-bit
/// resolution should have 4 [3.3V / 4096 ~= 0.0008] .
#define PROCESSOR_ANALOG_RESOLUTION 4
#elif MS_PROCESSOR_ADC_RESOLUTION == 10
/// @brief Decimals places in string representation; a 3.3V processor at 10-bit
/// resolution should have 3 [3.3V / 1024 ~= 0.0032] .
#define PROCESSOR_ANALOG_RESOLUTION 3
#else
// Default to 10-bit behavior if not specified
#define PROCESSOR_ANALOG_RESOLUTION 3
#endif
/**@}*/

/**
 * @brief Processor analog base class that inherits from AnalogVoltageBase
 *
 * This class provides processor-specific analog functionality on top of
 * the generic AnalogVoltageBase class. It handles processor ADC configuration
 * and maintains the data pin information for analog readings.
 */
class ProcessorAnalogBase : public AnalogVoltageBase {
 public:
    /**
     * @brief Construct a new ProcessorAnalogBase object
     *
     * @param voltageMultiplier Any multiplier needed to convert raw battery
     * readings from `analogRead()` into true battery values based on any
     * resistors or voltage dividers
     * @param operatingVoltage The processor's operating voltage; most
     * likely 3.3 or 5.
     */
    ProcessorAnalogBase(float voltageMultiplier = 1.0f,
                        float operatingVoltage  = OPERATING_VOLTAGE);

    /**
     * @brief Destroy the ProcessorAnalogBase object
     */
    virtual ~ProcessorAnalogBase() = default;

    /**
     * @brief Read a single-ended voltage measurement from the processor ADC
     *
     * @param analogChannel The processor ADC pin used to read the target
     * voltage.  Negative or invalid channel numbers are not clamped and will
     * cause the reading to fail and emit a warning.
     * @param resultValue Reference to store the resulting voltage measurement
     * @return True if the voltage reading was successful
     */
    bool readVoltageSingleEnded(int8_t analogChannel,
                                float& resultValue) override;

    /**
     * @brief Read a differential voltage measurement from the processor ADC
     *
     * ProcessorAnalog does not support differential measurements, so this
     * always returns false.
     *
     * @param analogChannel The primary analog channel (ignored)
     * @param analogReferenceChannel The secondary (reference) analog channel
     * (ignored)
     * @param resultValue Reference to store the resulting voltage measurement.
     * This will be set to -9999.0 to indicate an invalid reading.
     * @return Always false (differential not supported)
     */
    bool readVoltageDifferential(int8_t analogChannel,
                                 int8_t analogReferenceChannel,
                                 float& resultValue) override;

    String getAnalogLocation(int8_t analogChannel,
                             int8_t analogReferenceChannel) override;

    /**
     * @brief Calculate the analog resolution in volts for the processor ADC
     *
     * For processor ADCs, this calculates the voltage resolution based on the
     * configured ADC resolution and supply voltage. The calculation uses:
     * - ADC resolution in bits: MS_PROCESSOR_ADC_RESOLUTION
     * - Full scale range: processor supply voltage (single-ended, 0V to Vcc)
     *
     * @return The analog resolution in volts per LSB
     */
    float calculateAnalogResolutionVolts(void) override;

};

// Inline utility function implementation
/**
 * @brief Create a ProcessorAnalogBase analog voltage reader with ownership
 * tracking
 *
 * This utility function safely creates a new ProcessorAnalogBase object with
 * default settings and verifies it was created successfully. It handles the
 * pattern of creating default analog voltage readers when none is provided to
 * sensor constructors.
 *
 * @param[out] ownsAnalogVoltageReader Reference to bool that tracks ownership.
 *   Set to true if the reader was created successfully, false if creation
 * failed.
 * @return Pointer to created ProcessorAnalogBase object, or nullptr if creation
 * failed
 *
 * @note This function is designed for use in sensor constructors that need a
 * default analog voltage reader. The ownership flag should be used to determine
 * whether the returned pointer needs to be deleted in the sensor's destructor.
 */
inline ProcessorAnalogBase*
createProcessorAnalogBase(bool& ownsAnalogVoltageReader) {
    ProcessorAnalogBase* reader = new ProcessorAnalogBase();
    // verify that the voltage reader was created successfully
    // this could fail silently on no-exceptions Arduino targets
    ownsAnalogVoltageReader = (reader != nullptr);
    return reader;
}

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [external voltage as measured by the processor ADC](@ref sensor_processor_analog).
 *
 * @ingroup sensor_processor_analog
 */
/* clang-format on */
class ProcessorAnalog : public Sensor {
 public:
    /**
     * @brief Construct a new Processor Analog object - need the power pin and
     * the data pin on the processor.
     *
     * @param powerPin The pin on the mcu controlling power to the sensor
     * Use -1 if it is continuously powered.
     * @param dataPin The processor ADC pin used to read the target voltage. Not
     * all processor pins can be used as analog pins.  Those usable as analog
     * pins generally are numbered with an "A" in front of the number
     * - ie, A1.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param analogVoltageReader Pointer to ProcessorAnalogBase object for
     * analog functionality. If nullptr (default), creates a new
     * ProcessorAnalogBase with default settings.
     */
    ProcessorAnalog(int8_t powerPin, int8_t dataPin,
                    uint8_t              measurementsToAverage = 1,
                    ProcessorAnalogBase* analogVoltageReader   = nullptr);
    /**
     * @brief Destroy the Processor Analog object
     */
    ~ProcessorAnalog();

    // Delete copy constructor and copy assignment operator to prevent shallow
    // copies
    ProcessorAnalog(const ProcessorAnalog&)            = delete;
    ProcessorAnalog& operator=(const ProcessorAnalog&) = delete;

    // Delete move constructor and move assignment operator
    ProcessorAnalog(ProcessorAnalog&&)            = delete;
    ProcessorAnalog& operator=(ProcessorAnalog&&) = delete;

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief Pointer to the ProcessorAnalogBase object providing analog
     * functionality
     */
    ProcessorAnalogBase* _analogVoltageReader = nullptr;

    /**
     * @brief Whether this object owns the _analogVoltageReader pointer and
     * should delete it
     */
    bool _ownsAnalogVoltageReader = false;
};


// The single available variable is voltage
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [voltage output](@ref sensor_processor_analog_volt) from a
 * [analog processor pin](@ref sensor_processor_analog).
 *
 * @ingroup sensor_processor_analog
 */
/* clang-format on */
class ProcessorAnalog_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new ProcessorAnalog_Voltage object.
     *
     * @param parentSense The parent ProcessorAnalog providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "analogVoltage".
     */
    explicit ProcessorAnalog_Voltage(
        ProcessorAnalog* parentSense, const char* uuid = "",
        const char* varCode = PROCESSOR_ANALOG_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)PROCESSOR_ANALOG_VAR_NUM,
                   (uint8_t)PROCESSOR_ANALOG_RESOLUTION,
                   PROCESSOR_ANALOG_VAR_NAME, PROCESSOR_ANALOG_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new ProcessorAnalog_Voltage object.
     *
     * @note This must be tied with a parent ProcessorAnalog before it can be
     * used.
     */
    ProcessorAnalog_Voltage()
        : Variable((uint8_t)PROCESSOR_ANALOG_VAR_NUM,
                   (uint8_t)PROCESSOR_ANALOG_RESOLUTION,
                   PROCESSOR_ANALOG_VAR_NAME, PROCESSOR_ANALOG_UNIT_NAME,
                   PROCESSOR_ANALOG_DEFAULT_CODE) {}
    /**
     * @brief Destroy the ProcessorAnalog_Voltage object - no action needed.
     */
    ~ProcessorAnalog_Voltage() {}
};

/**@}*/
#endif  // SRC_SENSORS_PROCESSOR_ANALOG_H_
