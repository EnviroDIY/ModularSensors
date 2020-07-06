/**
 * @file MaxBotixSonar.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MaxBotixSonar sensor subclass and the MaxBotixSonar_Range
 * variable subclass.
 *
 * These are for the MaxBotix HRXL-MaxSonar ultrasonic range finders.
 *
 * The output from the HRXL-MaxSonar-WRL sonar is the range in mm.
 *     Accuracy is ± 1%
 *     Range is 300-5000mm or 500 to 9999mm, depending on model
 *
 * Warm up time to completion of header:  160ms
 */

// Header Guards
#ifndef SRC_SENSORS_MAXBOTIXSONAR_H_
#define SRC_SENSORS_MAXBOTIXSONAR_H_

// Debugging Statement
// #define MS_MAXBOTIXSONAR_DEBUG

#ifdef MS_MAXBOTIXSONAR_DEBUG
#define MS_DEBUGGING_STD "MaxBotixSonar"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the HRXL can report 1 value.
#define HRXL_NUM_VARIABLES 1
/// Sensor::_warmUpTime_ms; the HRXL warms up in 160ms.
#define HRXL_WARM_UP_TIME_MS 160
/// Sensor::_stabilizationTime_ms; the HRXL is stable after 0ms.
#define HRXL_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the HRXL takes 166ms to complete a measurement.
#define HRXL_MEASUREMENT_TIME_MS 166
/// Decimals places in string representation; range should have 2.
#define HRXL_RESOLUTION 0
/// Variable number; range is stored in sensorValues[0].
#define HRXL_VAR_NUM 0

// The main class for the MaxBotix Sonar
class MaxBotixSonar : public Sensor {
 public:
    MaxBotixSonar(Stream* stream, int8_t powerPin, int8_t triggerPin = -1,
                  uint8_t measurementsToAverage = 1);
    MaxBotixSonar(Stream& stream, int8_t powerPin, int8_t triggerPin = -1,
                  uint8_t measurementsToAverage = 1);
    ~MaxBotixSonar();

    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets pin mode on the trigger pin.  It also sets the expected stream
     * timeout for modbus and updates the #_sensorStatus.  No sensor power is
     * required.  This will always return true.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;
    bool wake(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    int8_t  _triggerPin;
    Stream* _stream;
};


// The class for the Range Variable
class MaxBotixSonar_Range : public Variable {
 public:
    /**
     * @brief Construct a new MaxBotixSonar_Range object.
     *
     * @param parentSense The parent MaxBotixSonar providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is SonarRange
     */
    explicit MaxBotixSonar_Range(MaxBotixSonar* parentSense,
                                 const char*    uuid    = "",
                                 const char*    varCode = "SonarRange")
        : Variable(parentSense, (const uint8_t)HRXL_VAR_NUM,
                   (uint8_t)HRXL_RESOLUTION, "distance", "millimeter", varCode,
                   uuid) {}
    /**
     * @brief Construct a new MaxBotixSonar_Range object.
     *
     * @note This must be tied with a parent MaxBotixSonar before it can be
     * used.
     */
    MaxBotixSonar_Range()
        : Variable((const uint8_t)HRXL_VAR_NUM, (uint8_t)HRXL_RESOLUTION,
                   "distance", "millimeter", "SonarRange") {}
    /**
     * @brief Destroy the MaxBotixSonar_Range object - no action needed.
     */
    ~MaxBotixSonar_Range() {}
};

#endif  // SRC_SENSORS_MAXBOTIXSONAR_H_
