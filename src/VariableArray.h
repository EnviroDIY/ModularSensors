/**
 * @file VariableArray.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the VariableArray class.
 *
 * @copydetails VariableArray
 */

// Header Guards
#ifndef SRC_VARIABLEARRAY_H_
#define SRC_VARIABLEARRAY_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_VARIABLEARRAY_DEBUG
#define MS_DEBUGGING_STD "VariableArray"
#endif
#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "VariableArray"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"


/**
 * @brief Manage and operate a collection of Variable objects and their sensors.
 *
 * Coordinates sensor setup, power/wake/sleep cycles, and value updates across
 * variables to minimize sensor on-time and optimize communication timing.
 */

/**
 * @brief Construct a new VariableArray object.
 */

/**
 * @brief Construct a new VariableArray object with an initial variable list.
 *
 * @param variableCount Number of variables in variableList.
 * @param variableList Array of pointers to Variable objects.
 */

/**
 * @brief Construct a new VariableArray object with variables and UUIDs.
 *
 * @param variableCount Number of variables in variableList.
 * @param variableList Array of pointers to Variable objects.
 * @param uuids Array of UUID strings, matched 1:1 with variableList positions.
 */

/**
 * @brief Destroy the VariableArray object.
 */

/**
 * @brief Initialize the VariableArray with a variable list and validate UUIDs.
 *
 * @param variableCount Number of variables in variableList. Overrides any constructor value.
 * @param variableList Array of pointers to Variable objects. Overrides any constructor value.
 */

/**
 * @brief Initialize the VariableArray with variables and UUIDs and validate them.
 *
 * @param variableCount Number of variables in variableList. Overrides any constructor value.
 * @param variableList Array of pointers to Variable objects. Overrides any constructor value.
 * @param uuids Array of UUID strings, matched 1:1 with variableList positions.
 */

/**
 * @brief Finalize initialization using the internally stored variable list and validate UUIDs.
 */

/**
 * @brief Pointer to the array of Variable pointers.
 */

/**
 * @brief Get the count of variables managed by this VariableArray.
 *
 * @return The number of variables.
 */

/**
 * @brief Get the number of calculated variables (not measured by sensors).
 *
 * @return The number of calculated variables.
 */

/**
 * @brief Get the number of unique sensors associated with the variables.
 *
 * Many sensors may provide multiple variables; this returns the sensor count.
 *
 * @return The number of sensors.
 */

/**
 * @brief Overwrite variables' UUIDs using the provided array, matched by index.
 *
 * @param uuids Array of UUID strings to assign to variables by position.
 */

/**
 * @brief Run setup for all unique sensors referenced by variables.
 *
 * Retries sensor setup up to multiple times when needed. Each sensor is set up once.
 *
 * @return `true` if all sensors were set up successfully, `false` otherwise.
 */

/**
 * @brief Power up each unique sensor referenced by the variable list.
 */

/**
 * @brief Wake each unique sensor and wait for readiness as needed.
 *
 * @return `true` if all sensors were awakened successfully, `false` otherwise.
 */

/**
 * @brief Put each unique sensor to sleep.
 *
 * @return `true` if all sensors were put to sleep successfully, `false` otherwise.
 */

/**
 * @brief Power down each unique sensor referenced by the variable list.
 */

/**
 * @brief Deprecated: update sensor values using the legacy update flow.
 *
 * @m_deprecated_since{0,38,0}
 *
 * Use completeUpdate() with appropriate flags instead.
 *
 * @return `true` if the legacy update flow reported success, `false` otherwise.
 */

/**
 * @brief Perform a full sensor update sequence with configurable phases.
 *
 * Optionally runs powerUp, wake, value update, sleep, and powerDown phases for
 * all unique sensors, waiting for readiness where appropriate.
 *
 * @param powerUp If `true`, power up sensors before updating.
 * @param wake If `true`, wake sensors before updating.
 * @param sleep If `true`, put sensors to sleep after updating.
 * @param powerDown If `true`, power down sensors after updating.
 * @return `true` if all requested phases completed successfully, `false` otherwise.
 */

/**
 * @brief Print current sensor values and metadata to the given Stream.
 *
 * @param stream Arduino Stream instance to write output to (default: &Serial).
 */

/**
 * @brief The count of variables in the array.
 */

/**
 * @brief The count of unique sensors tied to variables in the array.
 */

/**
 * @brief Check whether the variable at arrayIndex is the last produced by its sensor.
 *
 * @param arrayIndex Index of the variable in the array.
 * @return `true` if the variable is the last for its sensor, `false` otherwise.
 */

/**
 * @brief Validate that assigned UUID strings are well-formed for all variables.
 *
 * This checks textual UUID format only, not whether a UUID is semantically correct
 * for a given variable.
 *
 * @return `true` if all present UUIDs are validly formed, `false` otherwise.
 */

/**
 * @brief Retrieve a specific status bit from the sensor associated with a variable.
 *
 * @param arrayIndex Index of the variable in the array.
 * @param bitToGet Specific sensor status bit to read.
 * @return The boolean value of the requested status bit.
 */

/**
 * @brief Print the contents of an array with aligned spacing and commas (debug only).
 *
 * @tparam T Element type of the array to print.
 * @param arrayToPrint Array of values to print; uses the internal variable count to format output.
 */
class VariableArray {
 public:
    // Constructors
    /**
     * @brief Construct a new Variable Array object
     */
    VariableArray();
    /**
     * @brief Construct a new Variable Array object
     *
     * @param variableCount The number of variables in the array
     * @param variableList An array of pointers to variable objects.  The
     * pointers may be to calculated or measured variable objects.
     */
    VariableArray(uint8_t variableCount, Variable* variableList[]);
    /**
     * @brief Construct a new Variable Array object
     *
     * @param variableCount The number of variables in the array
     * @param variableList An array of pointers to variable objects.  The
     * pointers may be to calculated or measured variable objects.
     * @param uuids An array of UUID's.  These are linked 1-to-1 with the
     * variables by array position.
     */
    VariableArray(uint8_t variableCount, Variable* variableList[],
                  const char* uuids[]);
    /**
     * @brief Destroy the Variable Array object - no action taken.
     */
    ~VariableArray();

    // "Begins" the VariableArray - attaches the number and array of variables
    // Not doing this in the constructor because we expect the VariableArray to
    // be created in the "global scope" and we cannot control the order in which
    // objects in that global scope will be created.  That is, we cannot
    // guarantee that the variables and their pointers in the array will
    // actually have been created unless we wait until in the setup or loop
    // function of the main program.
    /**
     * @brief Begins the VariableArray.  Supplies a variable array, checks the
     * validity of all UUID and outputs the results.
     *
     * @param variableCount The number of variables in the array.  Supersedes
     * any value given in the constructor.
     * @param variableList An array of pointers to variable objects.  The
     * pointers may be to calculated or measured variable objects.  Supersedes
     * any value given in the constructor.
     */
    void begin(uint8_t variableCount, Variable* variableList[]);
    /**
     * @brief Begins the VariableArray.  Supplies a variable array and UUIDs,
     * checks the validity of all UUID and outputs the results.
     *
     * @param variableCount The number of variables in the array.  Supersedes
     * any value given in the constructor.
     * @param variableList An array of pointers to variable objects.  The
     * pointers may be to calculated or measured variable objects.  Supersedes
     * any value given in the constructor.
     * @param uuids An array of UUID's.  These are linked 1-to-1 with the
     * variables by array position.
     */
    void begin(uint8_t variableCount, Variable* variableList[],
               const char* uuids[]);
    /**
     * @brief Begins the VariableArray.  Checks the validity of all UUID and
     * outputs the results.
     */
    void begin();

    /**
     * @brief Pointer to the array of variable pointers.
     */
    Variable** arrayOfVars;

    // Functions to return information about the list

    /**
     * @brief Get the count of variables in the variable array
     *
     * @return The number of variables
     */
    uint8_t getVariableCount(void) {
        return _variableCount;
    }

    /**
     * @brief Get the number of calculated variables
     *
     * @return The number of calculated (ie, not measured by a
     * sensor) variables
     */
    uint8_t getCalculatedVariableCount(void);

    // This counts and returns the number of sensors
    /**
     * @brief Get the number of sensors associated with the variables in the
     * array.
     *
     * This will often be different from the number of variables because many
     * sensors can return multiple variables.
     *
     * @return The number of sensors
     */
    uint8_t getSensorCount(void);

    /**
     * @brief Match UUID's from the given variables in the variable array.
     *
     * This over-writes all UUID's previously assigned to every variable.  The
     * match is 1-to-1 based on array position.
     *
     * @param uuids An array of UUID's
     */
    void matchUUIDs(const char* uuids[]);

    // Public functions for interfacing with a list of sensors
    /**
     * @brief Set up all of the sensors in the tied to variables in the array.
     *
     * This sets up all of the variables in the array and their respective
     * sensors by running all of their setup() functions.  A single sensor
     * attached to may variables is only set up one time.  If a sensor doesn't
     * respond to its setup command, the command is called 5 times in attempt to
     * make a connection.  If all sensors are set up successfully, returns true.
     *
     * @return True indicates all sensors have been set up
     * successfully.
     */
    bool setupSensors(void);

    /**
     * @brief Power up each sensor.
     *
     * Runs the powerUp sensor function for each unique sensor.
     */
    void sensorsPowerUp(void);

    /**
     * @brief Wake up each sensor.
     *
     * Runs the wake sensor function for each unique sensor.  Repeatedly checks
     * each sensor's readiness state to optimize timing.
     *
     * @return True if all wake functions were run successfully.
     */
    bool sensorsWake(void);

    /**
     * @brief Put all sensors to sleep
     *
     * Runs the sleep sensor function for each unique sensor.
     *
     * @return True if all sleep functions were run successfully.
     */
    bool sensorsSleep(void);

    /**
     * @brief Cut power to all sensors.
     * Runs the powerDown sensor function for each unique sensor.
     */
    void sensorsPowerDown(void);

    /**
     * @brief Update the values for all connected sensors.
     *
     * @m_deprecated_since{0,38,0}
     *
     * Use completeUpdate() instead and set the powerUp, wake, sleep and
     * powerDown parameters as needed.
     *
     * @return True if all steps of the update succeeded.
     */
    bool updateAllSensors(void);

    // This function powers, wakes, updates values, sleeps and powers down.

    /**
     * @brief Update the values for all connected sensors including powering
     * them and waking and putting them to sleep.
     *
     * Returns a boolean indication the overall success.  Does NOT return any
     * values.  Repeatedly checks each sensor's readiness state to optimize
     * timing.
     *
     * @param powerUp If true, powers up all sensors before updating.
     * @param wake If true, wakes all sensors before updating.
     * @param sleep If true, puts all sensors to sleep after updating.
     * @param powerDown If true, cuts power to all sensors after updating.
     *
     * @return True if all steps of the update succeeded.
     */
    bool completeUpdate(bool powerUp = true, bool wake = true,
                        bool sleep = true, bool powerDown = true);

    /**
     * @brief Print out the results for all connected sensors to a stream
     *
     * This prints current sensor values along with meta-data to a stream
     * (either hardware or software serial).  By default, it will print to the
     * first Serial port.  Note that the input is a pointer to a stream instance
     * - to use a hardware serial instance you must use an ampersand before the
     * serial name (ie, &Serial1).
     *
     * @param stream An Arduino Stream instance
     */
    void printSensorData(Stream* stream = &Serial);

 protected:
    /**
     * @brief The count of variables in the array
     */
    uint8_t _variableCount;
    /**
     * @brief The count of unique sensors tied to variables in the array
     */
    uint8_t _sensorCount;

 private:
    /**
     * @brief Check if the current variable is the last variable that the sensor
     * will return.
     *
     * This is used for formating output where the format is slightly different
     * for the last value. (ie, no comma after the last value)
     *
     * @param arrayIndex The index of the variable in the sensor variable array
     * @return True if the variable is the last in the array.
     */
    bool isLastVarFromSensor(int arrayIndex);
    /**
     * @brief Check that all variable have valid UUID's, if they are assigned
     *
     * @return True if all variables have valid UUID's.
     *
     * @warning This does not check that the UUID's are the true UUID's for the
     * variables, just that the text is a validly formed UUID.
     */
    bool checkVariableUUIDs(void);

    /**
     * @brief Get a specific status bit from the sensor tied to a variable in
     * the array.
     *
     * @param arrayIndex The index of the variable in the sensor variable array
     * @param bitToGet The specific status bit to get.
     * @return The value of the requested status bit.
     */
    bool getSensorStatusBit(int                        arrayIndex,
                            Sensor::sensor_status_bits bitToGet);

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
    /**
     * @brief Prints out the contents of an array with even spaces and commas
     * between the members
     *
     * @tparam T Any printable type
     * @param arrayToPrint The array of values to print.
     */
    template <typename T>
    void prettyPrintArray(T arrayToPrint[]) {
        MS_SERIAL_OUTPUT.print("[,\t");
        for (uint8_t i = 0; i < _variableCount; i++) {
            MS_SERIAL_OUTPUT.print(arrayToPrint[i]);
            MS_SERIAL_OUTPUT.print(",\t");
        }
        MS_SERIAL_OUTPUT.println("]");
    }
#else
/**
 * @brief Prints out the contents of an array with even spaces and commas
 * between the members
 */
#define prettyPrintArray(...)
#endif  // MS_VARIABLEARRAY_DEBUG_DEEP
};

#endif  // SRC_VARIABLEARRAY_H_