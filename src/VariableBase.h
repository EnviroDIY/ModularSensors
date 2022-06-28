/**
 * @file VariableBase.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Variable class.
 *
 * @copydetails Variable
 */

// Header Guards
#ifndef SRC_VARIABLEBASE_H_
#define SRC_VARIABLEBASE_H_

// Debugging Statement
// #define MS_VARIABLEBASE_DEBUG

#ifdef MS_VARIABLEBASE_DEBUG
#define MS_DEBUGGING_STD "VariableBase"
#endif

// Forward Declared Dependences
class Sensor;

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD

/**
 * @brief The variable class for a value and related metadata.
 *
 * A variable is a result value taken by a sensor _or_ calculated from the
 * results of one or more sensors.  It is characterized by a name (what it is a
 * measurement of), a unit of measurement, and a resolution.  The
 * [names](http://vocabulary.odm2.org/variablename/) and
 * [units](http://vocabulary.odm2.org/units/) of measurements for all variables
 * come from the controlled vocabularies developed for the ODM2 data system.
 * (http://vocabulary.odm2.org/)  The resolution is determined by the method
 * used to take the measurement by the sensor.  A variable may also be assigned
 * a universally unique identifier (UUID) and a unique variable code.  Many
 * sensors are capable of measuring multiple variables at a single time.  For
 * example, a Decagon CTD-10 is a _sensor_.  It is able to measure 3
 * _variables_: specific conductance, temperature, and water depth.  The
 * variable named "specificConductance" has _units_ of microsiemens per
 * centimeter (µS/cm) and a _resolution_ of 1 µS/cm.  Each measured variable is
 * explicitly tied to the "parent" sensor that "notifies" the variable when a
 * new value has been measured.  Each calculated variable has a parent function
 * returning a float which is the value for that variable.
 *
 * Variables are expected to be grouped together into VariableArrays.
 *
 * @ingroup base_classes
 */
class Variable {
 public:
    /**
     * @brief Construct a new Variable objectfor a measured variable - that is,
     * one whose values are updated by a sensor.
     *
     * @note This constructor is NOT inteneded to be used outside of this
     * libraries.  It is intended to be used internally with sensors defined in
     * this library.
     *
     * @param parentSense The Sensor object supplying values.
     * @param sensorVarNum The position in the sensor's value array of this
     * variable's value.
     * @param decimalResolution The resolution (in decimal places) of the value.
     * @param varName The name of the variable per the [ODM2 variable name
     * controlled vocabulary](http://vocabulary.odm2.org/variablename/)
     * @param varUnit The unit of the variable per the [ODM2 unit controlled
     * vocabulary](http://vocabulary.odm2.org/units/)
     * @param varCode A custom code for the variable.  This can be any short
     * text helping to identify the variable in files.
     * @param uuid A universally unique identifier for the variable.
     */
    Variable(Sensor* parentSense, const uint8_t sensorVarNum,
             uint8_t decimalResolution, const char* varName,
             const char* varUnit, const char* varCode, const char* uuid);
    /**
     * @brief Construct a new Variable objectfor a measured variable - that is,
     * one whose values are updated by a sensor - but do not tie it to a
     * specific sensor.
     *
     * @note This constructor is NOT inteneded to be used outside of this
     * libraries.  It is intended to be used internally with sensors defined in
     * this library.
     *
     * @param sensorVarNum The position in the sensor's value array of this
     * variable's value.
     * @param decimalResolution The resolution (in decimal places) of the value.
     * @param varName The name of the variable per the [ODM2 variable name
     * controlled vocabulary](http://vocabulary.odm2.org/variablename/)
     * @param varUnit The unit of the variable per the [ODM2 unit controlled
     * vocabulary](http://vocabulary.odm2.org/units/)
     * @param varCode A custom code for the variable.  This can be any short
     * text helping to identify the variable in files.
     */
    Variable(const uint8_t sensorVarNum, uint8_t decimalResolution,
             const char* varName, const char* varUnit, const char* varCode);

    /**
     * @brief Construct a new Variable object for a calculated variable - that
     * is, one whose value is calculated by the calcFxn which returns a float.
     *
     * @param calcFxn Any function returning a float value
     * @param decimalResolution The resolution (in decimal places) of the value.
     * @param varName The name of the variable per the [ODM2 variable name
     * controlled vocabulary](http://vocabulary.odm2.org/variablename/)
     * @param varUnit The unit of the variable per the [ODM2 unit controlled
     * vocabulary](http://vocabulary.odm2.org/units/)
     * @param varCode A custom code for the variable.  This can be any short
     * text helping to identify the variable in files.
     * @param uuid A universally unique identifier for the variable.
     */
    Variable(float (*calcFxn)(), uint8_t decimalResolution, const char* varName,
             const char* varUnit, const char* varCode, const char* uuid);
    /**
     * @brief Construct a new Variable object for a calculated variable - that
     * is, one whose value is calculated by the calcFxn which returns a float.
     *
     * @param calcFxn Any function returning a float value
     * @param decimalResolution The resolution (in decimal places) of the value.
     * @param varName The name of the variable per the [ODM2 variable name
     * controlled vocabulary](http://vocabulary.odm2.org/variablename/)
     * @param varUnit The unit of the variable per the [ODM2 unit controlled
     * vocabulary](http://vocabulary.odm2.org/units/)
     * @param varCode A custom code for the variable.  This can be any short
     * text helping to identify the variable in files.
     */
    Variable(float (*calcFxn)(), uint8_t decimalResolution, const char* varName,
             const char* varUnit, const char* varCode);
    /**
     * @brief Construct a new Variable object
     */
    Variable();

    /**
     * @brief Destroy the Variable object - no action taken.
     */
    ~Variable();

    /**
     * @brief Begin for the Variable object
     *
     * @param parentSense The Sensor object supplying values.  Supercedes any
     * Sensor supplied in the constructor.
     * @param uuid A universally unique identifier for the variable.
     * Supercedes any value supplied in the constructor.
     * @param customVarCode A custom code for the variable.  Supercedes
     * any value supplied in the constructor.
     * @return Variable A pointer to the variable object
     */
    Variable* begin(Sensor* parentSense, const char* uuid,
                    const char* customVarCode);
    /**
     * @brief Begin for the Variable object
     *
     * @param parentSense The Sensor object supplying values.  Supercedes any
     * Sensor supplied in the constructor.
     * @param uuid A universally unique identifier for the variable.
     * Supercedes any value supplied in the constructor.
     * @return Variable A pointer to the variable object
     */
    Variable* begin(Sensor* parentSense, const char* uuid);
    /**
     * @brief Begin for the Variable object
     *
     * @param parentSense The Sensor object supplying values.  Supercedes any
     * Sensor supplied in the constructor.
     * @return Variable A pointer to the variable object
     */
    Variable* begin(Sensor* parentSense);

    /**
     * @brief Begin for the Variable object
     *
     * @param calcFxn Any function returning a float value.  Supercedes any
     * function supplied in the constructor.
     * @param decimalResolution The resolution (in decimal places) of the value.
     * Supercedes any value supplied in the constructor.
     * @param varName The name of the variable per the ODM2 variable name
     * controlled vocabulary.  Supercedes any value supplied in the constructor.
     * @param varUnit The unit of the variable per the ODM2 unit controlled
     * vocabulary.  Supercedes any value supplied in the constructor.
     * @param varCode A custom code for the variable.  Supercedes any value
     * supplied in the constructor.
     * @param uuid A universally unique identifier for the variable.
     * Supercedes any value supplied in the constructor.
     * @return Variable A pointer to the variable object
     */
    Variable* begin(float (*calcFxn)(), uint8_t decimalResolution,
                    const char* varName, const char* varUnit,
                    const char* varCode, const char* uuid);
    /**
     * @brief Begin for the Variable object
     *
     * @param calcFxn Any function returning a float value.  Supercedes any
     * function supplied in the constructor.
     * @param decimalResolution The resolution (in decimal places) of the value.
     * Supercedes any value supplied in the constructor.
     * @param varName The name of the variable per the ODM2 variable name
     * controlled vocabulary.  Supercedes any value supplied in the constructor.
     * @param varUnit The unit of the variable per the ODM2 unit controlled
     * vocabulary.  Supercedes any value supplied in the constructor.
     * @param varCode A custom code for the variable.  Supercedes any value
     * supplied in the constructor.
     * @return Variable A pointer to the variable object
     */
    Variable* begin(float (*calcFxn)(), uint8_t decimalResolution,
                    const char* varName, const char* varUnit,
                    const char* varCode);

    // This sets up the variable (generally attaching it to its parent)
    // bool setup(void);

    /**
     * @brief Notify the parent sensor that it has an observing variable.
     *
     * @note This should never be called for a calculated variable.
     *
     * @param parentSense The Sensor object supplying values.
     */
    void attachSensor(Sensor* parentSense);
    /**
     * @brief Updates the interally stored data value
     *
     * This is the function called by the parent sensor's notifyVariables
     * function
     *
     * @note This should never be called for a calculated variable.
     *
     * @param parentSense  The Sensor object supplying values.
     */
    void onSensorUpdate(Sensor* parentSense);
    /**
     * @brief Get the parent sensor name, if applicable
     *
     * This is a helper needed for dealing with variables in arrays
     *
     * @return **String** The parent sensor name
     */
    String getParentSensorName(void);
    /**
     * @brief Get the parent sensor name and location, if applicable.
     *
     * This is a helper needed for dealing with variables in arrays
     *
     * @return **String** The parent sensor's concatentated name and location.
     */
    String getParentSensorNameAndLocation(void);

    /**
     * @brief Set the calculation function for a calculted variable
     *
     * @param calcFxn Any function returning a float value.
     */
    void setCalculation(float (*calcFxn)());

    // This gets/sets the variable's resolution for value strings
    /**
     * @brief Get the variable's resolution - in decimal places
     *
     * @return **uint8_t** the variable resolution
     */
    uint8_t getResolution(void);
    /**
     * @brief Set the variable's resolution
     *
     * @param decimalResolution The resolution (in decimal places) of the value.
     */
    void setResolution(uint8_t decimalResolution);
    /**
     * @brief Get the variable name
     *
     * @return **String** The variable name
     */
    String getVarName(void);
    /**
     * @brief Set the variable name.
     *
     * Must be a value from the ODM2 variable name controlled vocabulary
     * available here:  http://vocabulary.odm2.org/variablename/
     *
     * @param varName The name of the variable per the ODM2 variable name
     * controlled vocabulary.
     */
    void setVarName(const char* varName);
    /**
     * @brief Get the variable unit
     *
     * @return **String** The variable unit
     */
    String getVarUnit(void);
    /**
     * @brief Set the variable unit.
     *
     * Must be a value from the ODM2 unit controlled vocabulary available
     * here: http://vocabulary.odm2.org/units/
     *
     * @param varUnit The unit of the variable per the ODM2 unit controlled
     * vocabulary.
     */
    void setVarUnit(const char* varUnit);
    /**
     * @brief Get the customized code for the variable
     *
     * @return **String** The customized code for the variable
     */
    String getVarCode(void);
    /**
     * @brief Set a customized code for the variable
     *
     * @param varCode A custom code for the variable.  This can be any short
     * text helping to identify the variable in files.
     */
    void setVarCode(const char* varCode);
    // This gets/sets the variable UUID, if one has been assigned
    /**
     * @brief Get the customized code for the variable
     *
     * @return **String** The customized code for the variable
     */
    String getVarUUID(void);
    /**
     * @brief Set a customized code for the variable
     *
     * @param uuid A universally unique identifier for the variable.
     */
    void setVarUUID(const char* uuid);
    /**
     * @brief Verify the the UUID is correctly formatted
     *
     * @return **bool** True if the UUID is correctly formatted.
     *
     * @note This only checks the _format_ of the UUID.  It does not in any way
     * indicate that the value of the UUID is correct.
     */
    bool checkUUIDFormat(void);

    /**
     * @brief Get current value of the variable as a float
     *
     * @param updateValue True to ask the parent sensor to measure and return a
     * new value.  Default is false.
     * @return **float** The current value of the variable
     */
    float getValue(bool updateValue = false);
    /**
     * @brief Get current value of the variable as a string with the correct
     * decimal resolution
     *
     * @param updateValue True to ask the parent sensor to measure and return a
     * new value.  Default is false.
     * @return **String** The current value of the variable
     */
    String getValueString(bool updateValue = false);

    /**
     * @brief Pointer to the parent sensor
     */
    Sensor* parentSensor = nullptr;
    /**
     * @brief Internal note as to whether the value is calculated.
     */
    bool isCalculated = false;

 protected:
    /**
     * @brief The current data value
     *
     * When we create the variable, we also want to initialize it with a current
     * value of -9999 (ie, a bad result).
     */
    float _currentValue = -9999;


 private:
    float (*_calcFxn)(void) = nullptr;

    const uint8_t _sensorVarNum      = 0;
    uint8_t       _decimalResolution = 0;

    const char* _varName = nullptr;
    const char* _varUnit = nullptr;
    const char* _varCode = nullptr;
    const char* _uuid    = nullptr;
};

#endif  // SRC_VARIABLEBASE_H_
