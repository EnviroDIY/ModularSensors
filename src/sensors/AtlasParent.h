/**
 * @file AtlasParent.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasParent sensor subclass which is itself the
 * parent class for all Atlas sensors.
 *
 * This depends on the Arduino core Wire library.  It does *not* use the Atlas
 * Arduino library.
 *
 * @copydetails AtlasParent
 *
 * @defgroup atlas_group Atlas Scientific
 * The Sensor and Variable objects for all Atlas EZO circuits.
 *
 * @copydetails AtlasParent
 */

// Header Guards
#ifndef SRC_SENSORS_ATLASPARENT_H_
#define SRC_SENSORS_ATLASPARENT_H_

// Debugging Statement
// #define MS_ATLASPARENT_DEBUG

#ifdef MS_ATLASPARENT_DEBUG
#define MS_DEBUGGING_STD "AtlasParent"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

/**
 * @brief A parent class for Atlas sensors
 *
 * This contains the main I2C functionality for all Atlas EZO circuits.
 *
 * @ingroup atlas_group
 *
 * @see atlas_page
 */
class AtlasParent : public Sensor {
 public:
    /**
     * @brief Construct a new Atlas Parent object
     *
     * @param powerPin The pin on the mcu controlling power to the Atlas
     * circuit.  Use -1 if the sensor is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit
     * @param measurementsToAverage The number of measurements to average
     * @param sensorName The name of the sensor, defaults to AtlasSensor.
     * @param numReturnedVars The number of results returned by the sensor.
     * Defaults to 1.
     * @param warmUpTime_ms The time needed from the when a sensor has power
     * until it's ready to talk (_warmUpTime_ms).
     * @param stabilizationTime_ms The time needed from the when a sensor is
     * activated until the readings are stable (_stabilizationTime_ms).
     * @param measurementTime_ms The time needed from the when a sensor is told
     * to take a single reading until that reading is expected to be complete
     * (_measurementTime_ms)
     */
    AtlasParent(int8_t powerPin, uint8_t i2cAddressHex,
                uint8_t       measurementsToAverage = 1,
                const char*   sensorName            = "AtlasSensor",
                const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                uint32_t stabilizationTime_ms = 0,
                uint32_t measurementTime_ms   = 0);
    /**
     * @brief Destroy the Atlas Parent object - no action needed
     */
    virtual ~AtlasParent();

    /**
     * @brief Return the I2C address of the EZO circuit.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the #_powerPin mode, begins the Wire library (sets pin levels
     * and modes for I2C), and updates the #_sensorStatus.  No sensor power is
     * required.
     *
     * @return **true** The setup was successful
     * @return **false** Some part of the setup failed
     */
    bool setup(void) override;

    // NOTE:  The sensor should wake as soon as any command is sent.
    // I assume that means we can use the command to take a reading to both
    // wake it and ask for a reading.
    // bool wake(void) override;

    /**
     * @brief Puts the sensor to sleep, if necessary.
     *
     * This also un-sets the #_millisSensorActivated timestamp (sets it to 0).
     * This does NOT power down the sensor!
     *
     * @return **true** The sleep function completed successfully.
     * @return **false** Sleep did not complete successfully.
     */
    bool sleep(void) override;

    /**
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the #_millisMeasurementRequested timestamp.
     *
     * @note This function does NOT include any waiting for the sensor to be
     * warmed up or stable!
     *
     * @return **true** The start measurement function completed successfully.
     * @return **false** The start measurement function did not complete
     * successfully.
     */
    bool startSingleMeasurement(void) override;
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 protected:
    /**
     * @brief The I2C address.
     */
    int8_t _i2cAddressHex;

    /**
     * @brief Wait for a command to process
     *
     * Most Atlas I2C commands have a 300ms processing time from the time the
     * command is written until it is possible to request a response or result,
     * except for the commands to take a calibration point or a reading which
     * have a 600ms processing/response time.
     *
     * @note This should ONLY be used as a wait when no response is expected
     * except a status code - the response will be "consumed" and become
     * unavailable.
     *
     * @param timeout The maximum amout of time to wait in ms.
     * @return **true** Processing completed and a status code was returned.
     * @return **false** The expected status code did not come within the wait
     * period.
     */
    bool waitForProcessing(uint32_t timeout = 1000L);
};

#endif  // SRC_SENSORS_ATLASPARENT_H_
