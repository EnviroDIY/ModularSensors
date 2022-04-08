/**
 * @file TallyCounterI2C.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TallyCounterI2C sensor subclass and variable subclasse
 * TallyCounterI2C_Events.
 *
 * This file is for NorthernWidget's Tally external event counter, which is
 * used to measure windspeed or rainfall from reed-switch analog sensors.
 *
 * This depends on the [Tally_Library]
 * (https://github.com/EnviroDIY/Tally_Library/tree/Dev_I2C)
 *
 * @copydetails TallyCounterI2C
 */
/* clang-format off */
/**
 * @defgroup sensor_tally Tally Counter I2C
 * Classes for the Tally Counter I2C external event counter.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_tally_intro Introduction
 * Northern Widget's Tally is
 * > a minimal ultra low power digital event counter, designed to be a
 * > counterpart to a traditional data logger to measure and average rapid
 * > events like an anemometer reading.
 * It communicates over I2C at either 3.3 or 5V.
 *
 * The [Tally_Library]
 * (https://github.com/EnviroDIY/Tally_Library/tree/Dev_I2C)
 * is used internally for communication with the Tally.
 *
 * @warning Northern Widget considers this sensor to be one of their "bleeding edge"
 * sensors.  As such, it is subject to change at any time.  This library may not
 * be updated immediately to reflect changes on the part of Northern Widget.
 *
 * @warning As an event counter, the Tally should be continuously powered (at 3.3V or 5V).
 * It does have a large on-board capacitor which can act as a battery to ride out power
 * shortages, but that shouldn't be expected to perform as a long-term solution.
 * If you do chose to switch the power for the Tally counter, be very conscious
 * of the fact that the library is designed to run power this (and all other) sensors
 * for the very minimum amount of time possible to get a reading.
 * This means the capacitor will be running at a very low duty cycle;
 * likely less than 1% if the sensors are only powered a few seconds every few minutes.
 * A super-capacitor is *NOT* an instantly charged battery and will not keep the counter
 * powered at too low a duty cycle.
 *
 * @section sensor_tally_datasheet Sensor Datasheet
 * Documentation for the sensor can be found at:
 * - https://github.com/EnviroDIY/Project-Tally​
 * - https://github.com/EnviroDIY/Tally_Library/tree/Dev_I2C
 *
 * @section sensor_tally_ctor Sensor Constructor
 * {{ @ref TallyCounterI2C::TallyCounterI2C }}
 *
 * ___
 * @section sensor_tally_examples Example Code
 * The Tally countetr is used in the @menulink{tally} example.
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


/** @ingroup sensor_tally */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Tally can report 1 value.
#define TALLY_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define TALLY_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_tally_timing
 * @name Sensor Timing
 * The sensor timing for a Northern Widget Tally event counter
 * - Readings transferred from the reed-switch counting device (i.e. anemometer
 * or tipping bucket) to the logger are from past events, so there is no need
 * to wait for stability or measuring.
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; Tally warms up in <10 ms.
#define TALLY_WARM_UP_TIME_MS 10
/// @brief Sensor::_stabilizationTime_ms; Tally is stable after <10ms.
#define TALLY_STABILIZATION_TIME_MS 10
/// @brief Sensor::_measurementTime_ms; Tally takes <10ms to complete a
/// measurement.
#define TALLY_MEASUREMENT_TIME_MS 10
/**@}*/

/**
 * @anchor sensor_tally_events
 * @name Events
 * The events variable from a Northern Widget Tally event counter
 * - Range and accuracy depend on the sensor used
 *     - For wind, we often use [Inspeed WS2R Version II Reed Switch Anemometer]
 *  (https://www.store.inspeed.com/Inspeed-Version-II-Reed-Switch-Anemometer-Sensor-Only-WS2R.htm)
 *
 * {{ @ref TallyCounterI2C_Events::TallyCounterI2C_Events }}
 */
/**@{*/
/// @brief Decimals places in string representation; events are an integer
/// should be 0 - resolution is 1 event.
#define TALLY_EVENTS_RESOLUTION 0
/// @brief Sensor variable number; events is stored in sensorValues[0].
#define TALLY_EVENTS_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "counter"
#define TALLY_EVENTS_VAR_NAME "counter"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "event"
#define TALLY_EVENTS_UNIT_NAME "event"
/// @brief Default variable short code; "TallyCounterI2CEvents"
#define TALLY_EVENTS_DEFAULT_CODE "TallyCounterI2CEvents"
/**@}*/

/// @brief The default address of the Tally
#define TALLY_ADDRESS_BASE 0x33

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Tally Counter I2C](@ref sensor_tally).
 *
 * @ingroup sensor_tally
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
     * @warning If you do chose to switch the power for the Tally counter, be
     * very conscious of the fact that the library is designed to run power this
     * (and all other) sensors for the very minimum amount of time possible to
     * get a reading.  This means the capacitor will be running at a very low
     * duty cycle; likely less than 1% if the sensors are only powered a few
     * seconds every few minutes. A super-capacitor is *NOT* an instantly
     * charged battery and will not keep the counter powered at too low a duty
     * cycle.
     * @param i2cAddressHex The I2C address of the Tally Counter I2C is 0x33
     * by default.
     * @note The event counter should be continuously powered.  It has extremely
     * low power draw.
     * @note There is no option for averaging measurements; that option does not
     * make sense in an event counter.
     */
    TallyCounterI2C(int8_t  powerPin      = -1,
                    uint8_t i2cAddressHex = TALLY_ADDRESS_BASE);
    /**
     * @brief Destroy the Tally Counter object
     */
    ~TallyCounterI2C();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C) and updates the
     * #_sensorStatus. It also engages sleep mode on the Tally counter and
     * clears the counter memory. The Tally must be powered for setup.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;
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
 * [events output](@ref sensor_tally_events) from a
 * [Tally Counter I2C](@ref sensor_tally) - shows the number of
 * events since last read.
 *
 * @ingroup sensor_tally
 */
/* clang-format on */
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
    explicit TallyCounterI2C_Events(
        TallyCounterI2C* parentSense, const char* uuid = "",
        const char* varCode = TALLY_EVENTS_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TALLY_EVENTS_VAR_NUM,
                   (uint8_t)TALLY_EVENTS_RESOLUTION, TALLY_EVENTS_VAR_NAME,
                   TALLY_EVENTS_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new TallyCounterI2C_Events object.
     *
     * @note This must be tied with a parent TallyCounterI2C before it can be
     * used.
     */
    TallyCounterI2C_Events()
        : Variable((const uint8_t)TALLY_EVENTS_VAR_NUM,
                   (uint8_t)TALLY_EVENTS_RESOLUTION, TALLY_EVENTS_VAR_NAME,
                   TALLY_EVENTS_UNIT_NAME, TALLY_EVENTS_DEFAULT_CODE) {}
    /**
     * @brief Destroy the BoschBME280_Temp object - no action needed.
     */
    ~TallyCounterI2C_Events() {}
};
/**@}*/
#endif  // SRC_SENSORS_TallyCounterI2C_H_
