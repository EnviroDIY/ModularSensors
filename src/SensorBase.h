/**
 * @file SensorBase.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Sensor class.
 *
 * @copydetails Sensor
 */
/* clang-format off */
/**
 * @defgroup the_sensors Supported Sensors
 * All implemented Sensor classes
 *
 * @copydetails Sensor
 *
 * @see @ref page_sensor_notes
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORBASE_H_
#define SRC_SENSORBASE_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_SENSORBASE_DEBUG
#define MS_DEBUGGING_STD "SensorBase"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include <pins_arduino.h>

/// @brief The maximum number of power pins the library can support for a single
/// sensor.
#define NUMBER_SUPPORTED_POWER_PINS 2

class Variable;       // Forward declaration
class VariableArray;  // Forward declaration

/**
 * @brief The "Sensor" class is used for all sensor-level operations - waking,
 * sleeping, and taking measurements.
 *
 * A sensor is some sort of device that is capable of taking one or more
 * measurements using some sort of method.  Most often we can think of these as
 * probes or other instruments that can give back information about the world
 * around them.  Sensors can usually be given power or have that power cut. They
 * may be awoken or activated and then returned to a sleeping/low power use
 * state.  The may need to be asked to begin a single reading or they may
 * continuously return data.  They _**must**_ be capable of returning the value
 * of their readings to a logger of some type.
 *
 * @ingroup base_classes
 */
class Sensor {
 public:
    friend class Variable;
    friend class VariableArray;
    /**
     * @brief Construct a new Sensor object.
     *
     * @param sensorName The name of the sensor.  Defaults to "Unknown".
     * @param totalReturnedValues The total number of value results (raw or
     * calculated internally) returned by the sensor.  Defaults to 1.
     * @param warmUpTime_ms The time in ms between when the sensor is powered on
     * and when it is ready to receive a wake command.  Defaults to 0.
     * @param stabilizationTime_ms The time in ms between when the sensor
     * receives a wake command and when it is able to return stable values.
     * Defaults to 0.
     * @param measurementTime_ms The time in ms between when a measurement is
     * started and when the result value is available.  Defaults to 0.
     * @param powerPin The pin on the mcu controlling power to the sensor
     * Use -1 if it is continuously powered.
     * @param dataPin The pin on the mcu receiving data from the sensor.
     * Defaults to -1.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param incCalcValues The number of included calculated variables from the
     * sensor, if any.  These are used for values that we would always calculate
     * for a sensor and depend only on the raw results of that single sensor;
     * optional with a default value of 0.
     */
    Sensor(const char*   sensorName          = "Unknown",
           const uint8_t totalReturnedValues = 1, uint32_t warmUpTime_ms = 0,
           uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0,
           int8_t powerPin = -1, int8_t dataPin = -1,
           uint8_t measurementsToAverage = 1, uint8_t incCalcValues = 0);
    /**
     * @brief Dis-allowed constructor for a new Sensor object from a copy of
     * another Sensor object - the deleted copy constructor.
     *
     * The copy constructor is explicitly deleted.  Sensor objects should not
     * (and here cannot) be copied to avoid confusion over which sensor object
     * specific variables are tied to.
     *
     * @param copy_from_me The Sensor object to copy.
     */
    Sensor(const Sensor& copy_from_me) = delete;
    /**
     * @brief Dis-allowed assignment of one Sensor object to another - the
     * deleted assignment operator.
     *
     * The assignment operator is explicitly deleted.  Sensor objects should not
     * (and here cannot) be assigned to each other to avoid confusion over which
     * sensor object specific variables are tied to.
     */
    Sensor& operator=(const Sensor& copy_from_me) = delete;
    /**
     * @brief Destroy the Sensor object - no action taken.
     */
    virtual ~Sensor();

    // These functions are dependent on the constructor and return the
    // constructor values.
    /**
     * @brief Get the pin or connection location between the mcu and the sensor.
     *
     * @note This is NOT the position of the sensor in the environment, merely
     * how it is attached to the mcu.
     *
     * @return Text describing how the sensor is attached to the mcu.
     */
    virtual String getSensorLocation(void);
    /**
     * @brief Get the name of the sensor.
     *
     * @return The sensor name as given in the constructor.
     */
    virtual String getSensorName(void);
    /**
     * @brief Concatenate and returns the name and location of the sensor.
     *
     * @return A concatenation of the sensor name and its "location"
     * - how it is connected to the mcu.
     */
    String getSensorNameAndLocation(void);
    /**
     * @brief Get the pin number controlling sensor power.
     *
     * @return The pin on the mcu controlling power to the sensor.
     */
    virtual int8_t getPowerPin(void);
    /**
     * @brief Set the pin number controlling sensor power.
     * @param pin The pin on the mcu controlling power to the sensor.
     */
    virtual void setPowerPin(int8_t pin);
    /**
     * @brief Get the pin number controlling secondary sensor power.
     *
     * @return The pin on the mcu controlling secondary power
     *
     * This is for a second power needed to communicate with a sensor. Generally
     * to an adapter or converter needed to talk to the sensor - ie, an RS232
     * adapter, an RS485 adapter, or an IO multiplexer.
     */
    virtual int8_t getSecondaryPowerPin(void);
    /**
     * @brief Set the pin number controlling secondary sensor power.
     *
     * This is for a second power needed to communicate with a sensor. Generally
     * to an adapter or converter needed to talk to the sensor - ie, an RS232
     * adapter, an RS485 adapter, or an IO multiplexer.
     *
     * @param pin The pin on the mcu controlling secondary power
     */
    virtual void setSecondaryPowerPin(int8_t pin);

    /**
     * @brief Set the number measurements to average.
     *
     * @copydetails _measurementsToAverage
     *
     * @param nReadings The number of readings to take and average to create a
     * result from the sensor.  Overrides any value given in the constructor.
     */
    void setNumberMeasurementsToAverage(uint8_t nReadings);
    /**
     * @brief Get the number of measurements to average.
     *
     * @return The number of readings to take and average to create
     * a result from the sensor.
     *
     * @copydetails _measurementsToAverage
     */
    uint8_t getNumberMeasurementsToAverage(void);
    /**
     * @brief Get the number of measurement attempts that have been made and
     * completed (whether successful or not).
     *
     * @return The number of complete measurement attempts.
     */
    uint8_t getNumberCompleteMeasurementsAttempts(void);
    /**
     * @brief Get the number of retry attempts that have been made for a
     * measurement
     *
     * @return The number of retries that have been made for the current
     * measurement attempt.
     *
     * @note What is "successful" vs what qualifies for a retry varies by
     * sensor. For some it may be that if any values were returned, for others
     * that a specific value is in range, etc.
     */
    uint8_t getNumberRetryAttemptsMade(void);
    /**
     * @brief Get the number of allowed retries if a measurement fails.
     *
     * @return The number of allowed retries.
     */
    uint8_t getAllowedMeasurementRetries(void);
    /**
     * @brief Set the number of retries if a measurement fails.
     *
     * @param allowedMeasurementRetries The number of allowed retries.
     */
    void setAllowedMeasurementRetries(uint8_t allowedMeasurementRetries);

    // _warmUpTime_ms _stabilizationTime_ms _measurementTime_ms

    /**
     * @brief Set the warm-up time for the sensor.
     * @param warmUpTime_ms The warm-up time in milliseconds.
     * @remark This is the time between when the sensor is powered on and when
     * it is ready to receive a wake command.  It should be set in the sensor
     * constructor. Only change this if you know what you're doing!
     */
    void setWarmUpTime(uint32_t warmUpTime_ms);
    /**
     * @brief Get the warm-up time for the sensor.
     * @return The warm-up time in milliseconds.
     */
    uint32_t getWarmUpTime(void);
    /**
     * @brief Set the stabilization time for the sensor.
     * @param stabilizationTime_ms The stabilization time in milliseconds.
     * @remark This is the time between when the sensor receives a wake command
     * and when it is able to return stable values.  It should be set in the
     * sensor constructor. Only change this if you know what you're doing!
     */
    void setStabilizationTime(uint32_t stabilizationTime_ms);
    /**
     * @brief Get the stabilization time for the sensor.
     * @return The stabilization time in milliseconds.
     */
    uint32_t getStabilizationTime(void);
    /**
     * @brief Set the measurement time for the sensor.
     * @param measurementTime_ms The measurement time in milliseconds.
     * @remark This is the time between when a measurement is started and when
     * the result value is available.  It should be set in the sensor
     * constructor. Only change this if you know what you're doing!
     */
    void setMeasurementTime(uint32_t measurementTime_ms);
    /**
     * @brief Get the measurement time for the sensor.
     * @return The measurement time in milliseconds.
     */
    uint32_t getMeasurementTime(void);

    /// @brief The significance of the various status bits
    typedef enum {
        SETUP_SUCCESSFUL       = 0,  ///< Whether setup was successful
        POWER_ATTEMPTED        = 1,  ///< Whether power up was attempted
        POWER_SUCCESSFUL       = 2,  ///< Whether power up was successful
        WAKE_ATTEMPTED         = 3,  ///< Whether wake up was attempted
        WAKE_SUCCESSFUL        = 4,  ///< Whether wake up was successful
        MEASUREMENT_ATTEMPTED  = 5,  ///< Whether measurement was attempted
        MEASUREMENT_SUCCESSFUL = 6,  ///< Whether measurement was successful
        ERROR_OCCURRED         = 7   ///< Whether an error has occurred
    } sensor_status_bits;

    /**
     * @brief Get the 8-bit code for the current status of the sensor.
     *
     * Bit 0
     * - 0 => Has NOT been successfully set up
     * - 1 => Has been setup
     *
     * Bit 1
     * - 0 => No attempt made to power sensor
     * - 1 => Attempt made to power sensor
     *
     * Bit 2
     * - 0 => Power up attempt failed
     * - 1 => Power up attempt succeeded
     * - Use the isWarmedUp() function to check if enough time has passed to be
     * ready for sensor communication.
     *
     * Bit 3
     * - 0 => Activation/wake attempt made
     * - 1 => No activation/wake attempt made
     * - check _millisSensorActivated or bit 4 to see if wake() attempt was
     * successful
     * - a failed activation attempt will give _millisSensorActivated = 0
     *
     * Bit 4
     * - 0 => Wake/Activate failed
     * - 1 => Is awake/actively measuring
     * - Use the isStable() function to check if enough time has passed to begin
     * a measurement.
     *
     * Bit 5
     * - 0 => Start measurement requested attempt made
     * - 1 => No measurements have been requested
     * - check _millisMeasurementRequested or bit 6 to see if
     * startSingleMeasurement() attempt was successful
     * - a failed request attempt will give _millisMeasurementRequested = 0
     *
     * Bit 6
     * - 0 => Measurement start failed
     * - 1 => Measurement attempt succeeded
     * - Use the isMeasurementComplete() to check if enough time has passed for
     * a measurement to have been completed.
     *
     * Bit 7
     * - 0 => No known errors
     * - 1 => Some sort of error has occurred
     *
     * @return The status as a uint8_t.
     */
    uint8_t getStatus(void);

    /**
     * @brief Get the value of a specific status bit.
     *
     * @param bitToGet The status bit to retrieve.
     * @return The value of the specified status bit (0 or 1).
     */
    bool getStatusBit(sensor_status_bits bitToGet);

    /**
     * @brief Set a specific status bit to 1.
     *
     * @param bitToSet The status bit to set.
     */
    void setStatusBit(sensor_status_bits bitToSet);
    /// @brief A helper recursive template to set multiple status bits
    /// @tparam T The type of the status bit to set - must be a uint8_t
    /// @param bitToSet The first status bit to set.
    template <typename T>
    void setStatusBits(T bitToSet) {
        setStatusBit(bitToSet);
    }
    /// @brief A helper recursive template to set multiple status bits
    /// @tparam T The type of the status bit to set - must be a uint8_t
    /// @param firstBit The first status bit to set.
    /// @param otherBits Any additional status bits to set.
    template <typename T, typename... Args>
    void setStatusBits(T firstBit, Args... otherBits) {
        setStatusBit(firstBit);
        setStatusBits(otherBits...);
    }

    /**
     * @brief Clear a specific status bit (ie, set it to 0).
     *
     * @param bitToClear The status bit to clear.
     */
    void clearStatusBit(sensor_status_bits bitToClear);
    /// @brief A helper recursive template to clear multiple status bits
    /// @tparam T The type of the status bit to set - must be a uint8_t
    /// @param bitToClear The first status bit to clear.
    template <typename T>
    void clearStatusBits(T bitToClear) {
        clearStatusBit(bitToClear);
    }
    /// @brief A helper recursive template to clear multiple status bits
    /// @tparam T The type of the status bit to set - must be a uint8_t
    /// @param firstBit The first status bit to clear.
    /// @param otherBits Any additional status bits to clear.
    template <typename T, typename... Args>
    void clearStatusBits(T firstBit, Args... otherBits) {
        clearStatusBit(firstBit);
        clearStatusBits(otherBits...);
    }

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the pin modes of the _powerPin and _dataPin, updates
     * #_sensorStatus, and returns true.
     *
     * @return True if the setup was successful.
     */
    virtual bool setup(void);

    /**
     * @brief Update the sensor's values.
     *
     * For digital sensors with a single information return, this only needs to
     * be called once for each sensor, even if there are multiple variable
     * subclasses for the sensor.
     *
     * In general, the update function clears the value results array, powers
     * the sensor, wakes or activates it, tells it one or more times to a start
     * measurement and get the result, averages all the values, notifies the
     * attached variables that new values are available, puts the sensor back to
     * sleep (if it had been asleep) and powers the sensor down (if it had been
     * un-powered).   All possible waits are included in this function.  To get
     * new results from a single sensor, this is the function that should be
     * used.  To work with many sensors together, use the VariableArray class
     * which optimizes the timing and waits for many sensors working together.
     *
     * @return True if all steps of the sensor update completed
     * successfully.
     */
    virtual bool update(void);

    /**
     * @brief Turn on the sensor power, if applicable.
     *
     * Generally this is done by setting the #_powerPin `HIGH`.  Also sets the
     * #_millisPowerOn timestamp and updates the #_sensorStatus.
     *
     * @todo Universally support power pins that are active LOW.
     */
    virtual void powerUp(void);
    /**
     * @brief Turn off the sensor power, if applicable.
     *
     * Generally this is done by setting the #_powerPin `LOW`.  Also un-sets
     * the #_millisPowerOn timestamp (sets #_millisPowerOn to 0) and updates the
     * #_sensorStatus.
     *
     * @todo Universally support power pins that are inactive HIGH.
     */
    virtual void powerDown(void);

    /**
     * @brief Wake the sensor up, if necessary.  Do whatever it takes to get a
     * sensor in the proper state to begin a measurement.
     *
     * Verifies that the power is on and updates the #_sensorStatus.  This also
     * sets the #_millisSensorActivated timestamp.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return True if the wake function completed successfully.
     */
    virtual bool wake(void);
    /**
     * @brief Puts the sensor to sleep, if necessary.
     *
     * Does not take any action if not necessary.
     *
     * @note This does NOT power down the sensor!
     *
     * @return True if the sleep function completed successfully.
     */
    virtual bool sleep(void);

    /**
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the #_millisMeasurementRequested timestamp and updates the
     * #_sensorStatus.
     *
     * @note This function does NOT include any waiting for the sensor to be
     * warmed up or stable!
     *
     * @return True if the start measurement function completed
     * successfully.
     */
    virtual bool startSingleMeasurement(void);

    /**
     * @brief Get the results from a single measurement.
     *
     * This asks the sensor for a new result, verifies that it passes sanity
     * range checks, and then adds the value to the result array.
     *
     * This also un-sets the #_millisMeasurementRequested timestamp (sets
     * #_millisMeasurementRequested to 0) and updates the #_sensorStatus.
     *
     * @note This function does NOT include any waiting for the sensor complete
     * a measurement.
     *
     * @return True if the function completed successfully.
     */
    virtual bool addSingleMeasurementResult(void) = 0;

    /**
     * @brief The array of result values for each sensor.
     *
     * @todo Support int16_t and int32_t directly in the value array so no
     * casting is needed. This could be done using a template or a union similar
     * to the modbus library's leFrame union.
     *
     * @note The values in this array will not be usable until after the sensor
     * completes all requested measurements! Prior to that, the values in this
     * array will be the sum of all good values measured so far (or -9999 if no
     * good values have been measured yet).
     */
    float sensorValues[MAX_NUMBER_VARS];

    /**
     * @brief Clear the values array - that is, sets all values to -9999.
     */
    void clearValues();
    /**
     * @brief Verify that a measurement is OK (ie, not -9999) before adding it
     * to the result array
     *
     * @param resultNumber The position of the result within the result array.
     * @param resultValue The value of the result.
     */
    void verifyAndAddMeasurementResult(uint8_t resultNumber, float resultValue);
    /**
     * @brief Verify that a measurement is OK (ie, not -9999) before adding it
     * to the result array
     *
     * @param resultNumber The position of the result within the result array.
     * @param resultValue The value of the result.
     */
    void verifyAndAddMeasurementResult(uint8_t resultNumber,
                                       int16_t resultValue);
    /**
     * @brief Average the results of all measurements by dividing the sum of
     * all measurements by the number of measurements taken.
     *
     * @param resultNumber The position of the result within the result array.
     * @param resultValue The value of the result.
     */
    void verifyAndAddMeasurementResult(uint8_t resultNumber,
                                       int32_t resultValue);
    /**
     * @brief Average the results of all measurements by dividing the sum of
     * all measurements by the number of measurements taken.
     */
    void averageMeasurements(void);

    /**
     * @brief Register a variable object to a sensor.
     *
     * @param sensorVarNum The position the variable result holds in the
     * variable result array.
     * @param var A pointer to the Variable object.
     *
     * @note Only one variable can be assigned to each place in the array!
     */
    void registerVariable(int sensorVarNum, Variable* var);
    /**
     * @brief Notify attached variables of new values.
     */
    void notifyVariables(void);


    /**
     * @brief Check if the #_powerPin is currently high.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates the #_powerPin is currently `HIGH`.
     */
    bool checkPowerOn(bool debug = false);
    /**
     * @brief Check whether or not enough time has passed between the sensor
     * receiving power and being ready to respond to logger commands.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that enough time has passed that the
     * sensor should be ready to respond to commands.
     *
     * @note A true response does _NOT_ indicate that the sensor will respond to
     * commands, merely that the specified time for wake has passed.
     */
    virtual bool isWarmedUp(bool debug = false);
    /**
     * @brief Hold all further program execution until this sensor is ready to
     * receive commands.
     */
    void waitForWarmUp(void);

    /**
     * @brief Check whether or not enough time has passed between the sensor
     * being awoken/activated and being ready to output stable values.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that enough time has passed that the
     * sensor should have stabilized.
     *
     * @note A true response does _NOT_ indicate that the sensor is now giving
     * stable values, merely that the specified time for sensor stabilization
     * has passed.
     */
    virtual bool isStable(bool debug = false);
    /**
     * @brief Hold all further program execution until this sensor is reporting
     * stable values.
     */
    void waitForStability(void);

    /**
     * @brief Check whether or not enough time has passed between when the
     * sensor was asked to take a single measurement and when that measurement
     * is expected to be complete.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that enough time has passed the
     * measurement should have completed
     *
     * @note A true response does _NOT_ indicate that the sensor will now
     * successfully report a result, merely that the specified time for a
     * measurement has passed.
     */
    virtual bool isMeasurementComplete(bool debug = false);
    /**
     * @brief Hold all further program execution until this sensor is has
     * finished the current measurement.
     */
    void waitForMeasurementCompletion(void);


 protected:
    /**
     * @brief Digital pin number on the mcu receiving sensor data
     *
     * @note SIGNED int, to allow negative numbers for unused pins
     */
    int8_t _dataPin;
    /**
     * @brief Digital pin number on the mcu controlling sensor power
     *
     * @note SIGNED int, to allow negative numbers for unused pins
     */
    int8_t _powerPin;
    /**
     * @brief Digital pin number on the mcu controlling secondary power
     *
     * This is for a second power needed to communicate with a sensor. Generally
     * to an adapter or converter needed to talk to the sensor - ie, an RS232
     * adapter, an RS485 adapter, or an IO multiplexer.
     *
     * @note SIGNED int, to allow negative numbers for unused pins
     */
    int8_t _powerPin2;
    /**
     * @brief The sensor name.
     */
    const char* _sensorName;
    /**
     * @brief The number of values the sensor is capable of reporting.
     *
     * This includes raw values from the sensor and any values that are always
     * calculated within the library for the sensor.  The @ref _incCalcValues
     * are *included* in this total.
     */
    const uint8_t _numReturnedValues;
    /**
     * @brief The number of included calculated variables from the sensor, if
     * any.
     *
     * These are used for values that we would always calculate for a sensor and
     * depend only on the raw results of that single sensor.  This is separate
     * from any calculated variables that are created on-the-fly and depend on
     * multiple other sensors.
     */
    uint8_t _incCalcValues;
    /**
     * @brief The number of measurements from the sensor to average.
     *
     * This will become the number of readings actually taken by a sensor prior
     * to data averaging.  Any "bad" (-9999) values returned by the sensor will
     * not be included in the final averaging.  This means that the actual
     * number of "good" values that are averaged may be less than what was
     * requested.
     */
    uint8_t _measurementsToAverage;
    /**
     * @brief The number of measurements attempts by the sensor that have
     * finished **since last power on**.
     */
    uint8_t _measurementAttemptsCompleted = 0;
    /**
     * @brief The number of retries that have been attempted so far for a single
     * measurement.
     */
    uint8_t _retryAttemptsMade = 0;
    /**
     * @brief The number of allowed retries if a measurement fails.
     */
    uint8_t _allowedMeasurementRetries = 1;
    /**
     * @brief Array with the number of valid measurement values per variable by
     * the sensor in the current update cycle.
     *
     * @note The number of good measurements may vary between variables if
     * some values are more likely to be invalid than others - ie, a pH sensor
     * may also measure temperature and report a valid temperature when the pH
     * is junk.
     */
    uint8_t numberGoodMeasurementsMade[MAX_NUMBER_VARS];

    /**
     * @brief The time needed from the when a sensor has power until it's ready
     * to talk.
     */
    uint32_t _warmUpTime_ms;
    /**
     * @brief The processor elapsed time when the power was turned on for the
     * sensor.
     *
     * The #_millisPowerOn value is set in the powerUp() function.  It is un-set
     * in the powerDown() function.
     */
    uint32_t _millisPowerOn = 0;

    /**
     * @brief The time needed from the when a sensor is activated until the
     * readings are stable.
     */
    uint32_t _stabilizationTime_ms;
    /**
     * @brief The processor elapsed time when the sensor was activated - ie,
     * when the wake() function was run.
     *
     * The #_millisSensorActivated value is *usually* set in the wake()
     * function, but may also be set in the startSingleMeasurement() function.
     * It is generally un-set in the sleep() function.
     */
    uint32_t _millisSensorActivated = 0;

    /**
     * @brief The time needed from the when a sensor is told to take a single
     * reading until that reading is expected to be complete
     */
    uint32_t _measurementTime_ms;
    /**
     * @brief The processor elapsed time when a measurement was started - ie,
     * when the startSingleMeasurement() function was run.
     *
     * The #_millisMeasurementRequested value is set in the
     * startSingleMeasurement() function.  It *may* be unset in the
     * addSingleMeasurementResult() function.
     */
    uint32_t _millisMeasurementRequested = 0;
    /**
     * @brief The processor elapsed time when a measurement was completed - ie,
     * when the addSingleMeasurementResult() function was run.
     *
     * The #_millisMeasurementCompleted value is set in the
     * addSingleMeasurementResult() function.
     */
    uint32_t _millisMeasurementCompleted = 0;

    /**
     * @brief An 8-bit code for the sensor status
     */
    uint8_t _sensorStatus = 0;

    /**
     * @brief An array for each sensor containing pointers to the variable
     * objects tied to that sensor.
     *
     * The #MAX_NUMBER_VARS cannot be determined on a per-sensor basis; it must
     * be defined at compile time and applied to the entire class.
     */
    Variable* variables[MAX_NUMBER_VARS];

    /**
     * @brief A helper function to correctly bump the measurement counts and
     * retries and set the appropriate timestamp and status bits.
     * @param wasSuccessful True if the measurement attempt was successful.
     * @return Returns the input parameter wasSuccessful.
     */
    bool bumpMeasurementAttemptCount(bool wasSuccessful);
};

#endif  // SRC_SENSORBASE_H_
