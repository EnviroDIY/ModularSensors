/**
 * @file VariableArray.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the VariableArray class.
 *
 * @copydetails VariableArray
 */

// Header Guards
#ifndef SRC_VARIABLEARRAY_H_
#define SRC_VARIABLEARRAY_H_

// Debugging Statement
// #define MS_VARIABLEARRAY_DEBUG
// #define MS_VARIABLEARRAY_DEBUG_DEEP

#ifdef MS_VARIABLEARRAY_DEBUG
#define MS_DEBUGGING_STD "VariableArray"
#endif

#ifdef MS_VARIABLEARRAY_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "VariableArray"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "SensorBase.h"


/**
 * @brief The variable array class defines the logic for iterating through many
 * variable objects.
 *
 * This takes advantage of various time stamps within the Sensor class to
 * optimize the timing of communications with many sensors.
 *
 * In general, the order of the variables in the variable list or array should
 * not matter.  The library attempts to minimize sensor on time as much as
 * possible by requesting data from the each sensor as soon as it is able to
 * report a result, regardless of its position in the array.  The only place the
 * order of the variables will be reflected is in the order of the data columns
 * in data saved by a logger or when sending data to ThingSpeak.
 *
 * To use the VariableArray module, you must first create the array of pointers.
 * This should be done outside of the setup() or loop() functions.
 * Remember that for measured variables you must first create a new sensor
 * instance and then one or more new variable instances for that sensor
 * (depending on how many values it can return).
 * The sensor functions for sensors within a variable array take advantage of
 * all of the timestamps and status bits within the sensor object to minimize
 * the amount of time that all sensors are powered and the processor is awake.
 * That is, the first sensor to be warmed up will be set up or activated first;
 * the first sensor to stabilize will be asked for values first.
 * All calculations for any calculated variables happen after all the sensor
 * updating has finished.
 * The order of the variables within the array should not matter, though for
 * code readability, I strongly suggest putting all the variables attached to a
 * single sensor next to each other in the array.
 * When creating a logger, the order of variables in the array determines the
 * order the values will be written to the data file.
 *
 * @ingroup base_classes
 *
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
     * @brief Begins the VariableArray.  Suppiles a variable array, checks the
     * validity of all UUID and outputs the results.
     *
     * @param variableCount The number of variables in the array.  Supercedes
     * any value given in the constructor.
     * @param variableList An array of pointers to variable objects.  The
     * pointers may be to calculated or measured variable objects.  Supercedes
     * any value given in the constructor.
     */
    void begin(uint8_t variableCount, Variable* variableList[]);
    /**
     * @brief Begins the VariableArray.  Suppiles a variable array and UUIDs,
     * checks the validity of all UUID and outputs the results.
     *
     * @param variableCount The number of variables in the array.  Supercedes
     * any value given in the constructor.
     * @param variableList An array of pointers to variable objects.  The
     * pointers may be to calculated or measured variable objects.  Supercedes
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
     * @return **uint8_t** the number of variables
     */
    uint8_t getVariableCount(void) {
        return _variableCount;
    }

    /**
     * @brief Get the number of calculated variables
     *
     * @return **uint8_t** The number of calculated (ie, not measured by a
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
     * @return **uint8_t** The number of sensors
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
     * @return **bool** True indicates all sensors have been set up
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
     * @return **bool** True if all wake functions were run successfully.
     */
    bool sensorsWake(void);

    /**
     * @brief Put all sensors to sleep
     *
     * Runs the sleep sensor function for each unique sensor.
     *
     * @return **bool** True if all sleep functions were run successfully.
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
     * Does not power or wake/sleep sensors.  Returns a boolean indication the
     * overall success.  Does NOT return any values.  Repeatedly checks each
     * sensor's readiness state to optimize timing.
     *
     * @return **bool** True if all steps of the update succeeded.
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
     * @return **bool** True if all steps of the update succeeded.
     */
    bool completeUpdate(void);

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
    /**
     * @brief The maximum number of samples to average of an single sensor.
     */
    uint8_t _maxSamplestoAverage;

 private:
    bool    isLastVarFromSensor(int arrayIndex);
    uint8_t countMaxToAverage(void);
    bool    checkVariableUUIDs(void);

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
        DEEP_DEBUGGING_SERIAL_OUTPUT.print("[,\t");
        for (uint8_t i = 0; i < _variableCount; i++) {
            DEEP_DEBUGGING_SERIAL_OUTPUT.print(arrayToPrint[i]);
            DEEP_DEBUGGING_SERIAL_OUTPUT.print(",\t");
        }
        DEEP_DEBUGGING_SERIAL_OUTPUT.println("]");
    }
#else
/**
 * @brief Prints out the contents of an array with even spaces and commas
 * between the members
 */
#define prettyPrintArray(...)
#endif  // DEEP_DEBUGGING_SERIAL_OUTPUT
};

#endif  // SRC_VARIABLEARRAY_H_
