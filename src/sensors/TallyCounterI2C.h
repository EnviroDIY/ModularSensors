/**
 * @file TallyCounterI2C.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 *
 * @brief Contains the TallyCounterI2C sensor subclass and variable subclasse
 * TallyCounterI2C_Events.
 *
 * This file is for NorthernWidget's Tally external event counter, which is
 * used to measure windspeed or rainfall from reed-switch analog sensors.
 *
 * This depends on the [Tally_Library]
 * (https://github.com/NorthernWidget-Skunkworks/Tally_Library/tree/Dev_I2C)
 *
 * @copydetails TallyCounterI2C
 */
/* clang-format off */
/**
 * @defgroup tallyCounterI2C_group Tally Counter I2C
 * Classes for the Tally Counter I2C external event counter.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section tallyCounterI2C_intro Introduction
 * > The Tally is a minimal ultra low power digital event counter, designed to
 * > be a counterpart to a traditional data logger to measure and average rapid
 * > events like an anemometer reading.
 *
 * The [Tally_Library]
 * (https://github.com/NorthernWidget-Skunkworks/Tally_Library/tree/Dev_I2C)
 * is used internally for communication with the Tally.
 *
 *
 * @section tallyCounterI2C_datasheet Sensor Datasheet
 * Documentation for the sensor can be found at:
 * - https://github.com/NorthernWidget-Skunkworks/Project-Tally​
 * - https://github.com/NorthernWidget-Skunkworks/Tally_Library/tree/Dev_I2C
 *
 *
 * @section tallyCounterI2C_sensor The Tally Counter Sensor
 * @ctor_doc{TallyCounterI2C, int8_t I2CPower, uint8_t i2cAddressHex}
 * @subsection tallyCounterI2C_timing Sensor Timing
 * - Readings transferred from the reed-switch counting device (i.e. anemometer
 * or tipping bucket) to the logger are from past events, so there is no need
 * to wait for stability or measuring.
 *
 * @section tallyCounterI2C_events Events Output
 *   - Range and accuracy depend on the sensor used
 *     - For wind, we often use [Inspeed WS2R Version II Reed Switch Anemometer]
 *  (https://www.store.inspeed.com/Inspeed-Version-II-Reed-Switch-Anemometer-Sensor-Only-WS2R.htm)
 *   - Result stored in sensorValues[0]
 *   - Resolution is 1 event
 *   - Reported as dimensionless counts
 *   - Default variable code is TallyCounterI2CEvents
 * @variabledoc{tallyCounterI2C_events,TallyCounterI2,TallyCounterI2CEvents}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_TallyCounterI2C_H_
#define SRC_SENSORS_TallyCounterI2C_H_

// Debugging Statement
// #define MS_TALLYCOUNTERI2C_DEBUG

#ifdef MS_TALLYCOUNTERI2C_DEBUG
#define MS_DEBUGGING_STD "TallyCounterI2C"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Tally_I2C.h>


// Sensor Specific Defines

/// Sensor::_numReturnedValues; the Tally can report 1 value.
#define TALLY_NUM_VARIABLES 1
/// Sensor::_warmUpTime_ms; Tally warms up in <10 ms.
#define TALLY_WARM_UP_TIME_MS 10
/// Sensor::_stabilizationTime_ms; Tally is stable after <10ms.
#define TALLY_STABILIZATION_TIME_MS 10
/// Sensor::_measurementTime_ms; Tally takes <10ms to complete a measurement.
#define TALLY_MEASUREMENT_TIME_MS 10

/// Decimals places in string representation; events are an integer should be 0.
#define TALLY_EVENTS_RESOLUTION 0
/// Variable number; events is stored in sensorValues[0].
#define TALLY_EVENTS_VAR_NUM 0

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Tally Counter I2C](@ref tallyCounterI2C_group).
 *
 * @ingroup tallyCounterI2C_group
 */
/* clang-format on */
class TallyCounterI2C : public Sensor {
 public:
    /**
     * @brief Construct a new Tally Counter I2C object using the primary
     * hardware I2C instance.
     *
     * @param powerPin The pin on the mcu controlling power to TallyCounterI2C.
     * - The default is to use -1 for continuous power because a counting
     * device must always be on. However, the Tally also has a super capacitor
     * that will keep it running even while powered down while the logger is
     * in sleep during the interval between measurements.
     * - The Tally Counter I2C can use either a 3.3V or 5V power source.
     * @param i2cAddressHex The I2C address of the Tally Counter I2C is 0x33
     * by default.
     * @param measurementsToAverage The number of measurements to take and
     * average must be 1 for a counting device. This is a non-optional default.
     */
   explicit TallyCounterI2C(int8_t powerPin, uint8_t i2cAddressHex = 0x33);
   /**
    * @brief Destroy the Bosch BME280 object
    */
    ~TallyCounterI2C();

    /**
     * @brief Wake the sensor up, if necessary.  Do whatever it takes to get a
     * sensor in the proper state to begin a measurement.
     *
     * Verifies that the power is on and updates the #_sensorStatus.  This also
     * sets the #_millisSensorActivated timestamp.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return **bool** True if the wake function completed successfully.
     */
    bool   wake(void) override;
    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C),
     * and updates the #_sensorStatus.
     * The Tally
      must be powered for setup.
     *
     * @return **bool** True if the setup was successful.
     */
    bool   setup(void) override;
    /**
     * @copydoc Sensor::getSensorLocation()
     */

    String getSensorLocation(void) override;

    // bool startSingleMeasurement(void) override;  // for forced mode
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief Private reference to the internal Tally counter object.
     */
    Tally_I2C counter_internal;
    /**
     * @brief The I2C address of the Tally counter.
     */
    uint8_t _i2cAddressHex;
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [events output](@ref tallyCounterI2C_events)
 * from a [[Tally Counter I2C](@ref tallyCounterI2C_group).
 *
 * @ingroup tallyCounterI2C_group
 */
/* clang-format on */

// Defines the Event varible, shows the number of Events since last read
class TallyCounterI2C_Events : public Variable {
 public:
     /**
      * @brief Construct a new TallyCounterI2C_Events object.
      *
      * @param parentSense The parent TallyCounterI2C providing the result
      * values.
      * @param uuid A universally unique identifier (UUID or GUID) for the
      * variable; optional with the default value of an empty string.
      * @param varCode A short code to help identify the variable in files;
      * optional with a default value of "TallyCounterI2CEvents".
      */
    explicit TallyCounterI2C_Events(TallyCounterI2C* parentSense,
                                const char* uuid = "",
                                const char* varCode = "TallyCounterI2CEvents")
        : Variable(parentSense, (const uint8_t)TALLY_EVENTS_VAR_NUM,
                   (uint8_t)TALLY_EVENTS_RESOLUTION, "counter", "event",
                   varCode, uuid) {}
   /**
    * @brief Construct a new TallyCounterI2C_Events object.
    *
    * @note This must be tied with a parent TallyCounterI2C before it can be
    * used.
    */
    TallyCounterI2C_Events()
        : Variable((const uint8_t)TALLY_EVENTS_VAR_NUM,
                   (uint8_t)TALLY_EVENTS_RESOLUTION, "counter", "event",
                   "TallyCounterI2CEvents") {}
   /**
    * @brief Destroy the BoschBME280_Temp object - no action needed.
    */
    ~TallyCounterI2C_Events() {}
};

#endif  // SRC_SENSORS_TallyCounterI2C_H_
