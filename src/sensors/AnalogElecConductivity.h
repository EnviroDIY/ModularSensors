/**
 * @file AnalogElecConductivity.h
 * @copyright Stroud Water Research Center and Neil Hancock
 * Part of the EnviroDIY ModularSensors library
 * This library is published under the BSD-3 license.
 * @author Written By: Neil Hancock <neilh20+aec2008@wLLw.net>; Edited by Sara
 * Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Encapsulates an Electrical Conductivity sensors using an analog
 * input and onboard ADC and ADC ref.
 */
/* clang-format off */
/**
 * @defgroup sensor_analog_cond Conductivity via Analog Electrical Resistance
 * Classes for measuring conductivity using a simple analog voltage divider.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_analog_cond_notes Introduction
 * This is for a very basic conductivity circuit built with a single resistor
 * and an old power cord.  DC power is briefly supplied across the power cord
 * causing the water to act as one of the resistors on a voltage divider.
 * Knowing the voltage of the other resistor in the divider, we can calculate to
 * resistance from the water (and then its electrical conductivity) based on the
 * drop in volage across the divider.
 *
 * For this to work, the power across the circuit **MUST** be turned off between
 * readings.  If the power to the circuit is left on the water will become
 * polarized and the values will not be valid. The water temperature (if used)
 * must be supplied separately for a calculation.
 *
 * @note The return from this "sensor" is conductivity - not the typically
 * reported specific conductance, which is referenced to 25°C.  The temperature
 * compensation, if desired, should be done via a calculated variable.  See the
 * example code as a guide.
 *
 * @section sensor_analog_cond_circuit The Circuit
 * One pole of the power cord wire is connected to the ground of the main logger
 * board.  The other pole is connected to the sensor power supply via a resistor
 * of known resistance (R1) and then to an analog pin to measure the voltage.
 *
 * So the circuit is:
 *
 * @code{.unparsed}
 *  Vin (sensor power) --- R1 --- power cord  --- Vout
 *                                     |
 *                                     |
 *                        water between prongs (Rwater)
 *                                     |
 *                                     |
 *                                  ground
 * @endcode
 *
 * The above diagram and the calculations assume the resistance of the analog
 * pins themselves on the Arduino is negligible.
 *
 * @section sensor_analog_cond_calcs Calculating the Conductivity
 * First, we need to convert the bit reading of the ADC into volts based on the
 * range of the ADC (1 bit more than the resolution):
 *
 * \f[meas\_voltage = \frac{analog\_ref\_voltage * raw\_adc\_bits}{ANALOG\_EC\_ADC\_RANGE}\f]
 *
 * Assuming the voltage of the ADC reference is the same as that used to power
 * the EC resistor circuit we can replace the reference voltage with the sensor
 * power voltage:
 *
 * \f[meas\_voltage = \frac{sensor\_power\_voltage * raw\_adc\_bits}{ANALOG\_EC\_ADC\_RANGE}\f]
 *
 * @note The Vcc going to the circuit (~3.3V) can and will vary, as battery
 * level gets low.  If possible, you should use setup the processor to use an
 * external reference (`-D MS_PROCESSOR_ADC_REFERENCE_MODE=EXTERNAL`) and tie
 * the Aref pin to the sensor power pin.
 *
 * @note The analog reference of the Mayfly is not broken out (and is tied to
 * ground).  If using a Mayfly, you have no choice by to use the internal analog
 * reference.
 *
 * Now we can calculate the resistance of the water, knowing the resistance of
 * the resistor we put in the circuit and the voltage drop:
 *
 * \f[R_{water\_ohms} = \frac{meas\_voltage * R_{series\_ohms}}{sensor\_power\_voltage - meas\_voltage}\f]
 *
 * Combining the above equations and doing some rearranging, we get:
 *
 * \f[R_{water\_ohms} = \frac{R_{series\_ohms}}{\frac{ANALOG\_EC\_ADC\_RANGE}{raw\_adc\_bits} - 1}\f]
 *
 * The conductivity is then the inverse of the resistance - multiplied by a
 * measured cell constant and a 10^6 conversion to µS/cm.
 *
 * \f[water\_conductivity = \frac{1000000}{R_{water\_ohms} * sensor_{EC\_Konst}}\f]
 *
 * The real cell constant will vary based on the size of the "cell" - that is,
 * the size of the plug on the power cord.  You can calculate the cell constant
 * for each power cord sensor you use following the
 * [calibration program](https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino).
 *
 *  For one AC Power Cord 12t with male IEC 320-C8 connector the cell constant
 * was 2.88.
 *
 * @note These calculations are for the on-board processor ADC, not an external
 * ACD like the TI ADS1115 built into the Mayfly!
 *
 * @section sensor_analog_cond_ref References
 * - For the sensor setup and calculations:
 * https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino
 * - For temperature compensation:
 * https://link.springer.com/article/10.1023/B:EMAS.0000031719.83065.68
 *
 * @section sensor_analog_cond_flags Build flags
 * - `-D MS_PROCESSOR_ADC_RESOLUTION=##`
 *      - used to set the resolution of the processor ADC
 *      - @see #MS_PROCESSOR_ADC_RESOLUTION
 * - `-D MS_PROCESSOR_ADC_REFERENCE_MODE=xxx`
 *      - used to set the processor ADC value reference mode
 *      - @see #MS_PROCESSOR_ADC_REFERENCE_MODE
 *
 * @section sensor_analog_cond_ctor Sensor Constructor
 * {{ @ref AnalogElecConductivity::AnalogElecConductivity }}
 *
 * ___
 * @section sensor_analog_cond_examples Example Code
 * The analog electrical conductivity sensor is used in the
 * @menulink{analog_elec_conductivity} example.
 *
 * @menusnip{analog_elec_conductivity}
 *
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ANALOGELECCONDUCTIVITY_H_
#define SRC_SENSORS_ANALOGELECCONDUCTIVITY_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_ANALOGELECCONDUCTIVITY_DEBUG
#define MS_DEBUGGING_STD "AnalogElecConductivity"
#endif
#ifdef MS_ANALOGELECCONDUCTIVITY_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "AnalogElecConductivity"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include "math.h"

/** @ingroup sensor_analog_cond */
/**@{*/

/**
 * @anchor sensor_analog_cond_parts_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by the analog conductivity
 * sensor
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; we only get one value from the analog
/// conductivity sensor.
#define ANALOGELECCONDUCTIVITY_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values
/// though we recommend users include a temperature sensor and calculate
/// specific conductance in their own program.
#define ANALOGELECCONDUCTIVITY_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_analog_cond_parts_config
 * @name Configuration Defines
 * Defines to help configure the range and resolution of the home-made
 * conductivity sensor depending on the processor and ADC in use.
 */
/**@{*/
#if !defined(RSERIES_OHMS_DEF) || defined(DOXYGEN)
/**
 * @brief The default resistance (in ohms) of the measuring resistor.
 * This should not be less than 300 ohms when measuring EC in water.
 */
#define RSERIES_OHMS_DEF 499
#endif  // RSERIES_OHMS_DEF

#if !defined(SENSOREC_KONST_DEF) || defined(DOXYGEN)
/**
 * @brief Cell Constant For EC Measurements.
 *
 * This should be measured following the calibration example on
 * https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino.
 *
 * Mine was around 2.9 with plugs being a standard size they should all be
 * around the same. If you get bad readings you can use the calibration script
 * and fluid to get a better estimate for K.
 * Default to 1.0, and can be set at startup.
 */
#define SENSOREC_KONST_DEF 1.0
#endif  // SENSOREC_KONST_DEF
/**@}*/

/**
 * @anchor sensor_analog_cond_parts_timing
 * @name Sensor Timing
 * The timing for analog conductivity via resistance.
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; giving 2ms for warm-up.
#define ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS 2
/// @brief Sensor::_stabilizationTime_ms; we give just a tiny delay for
/// stabilization.
#define ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS 1
/**
 * @brief Sensor::_measurementTime_ms; we assume the analog voltage is measured
 * instantly.
 *
 * It's not really *quite* instantly, but it is very fast and the time to
 * measure is included in the read function.
 * On ATmega based boards (UNO, Nano, Mini, Mega), it takes about 100
 * microseconds (0.0001 s) to read an analog input, so the maximum reading rate
 * is about 10,000 times a second.
 */
#define ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS 0
/**@}*/
/**
 * @anchor sensor_analog_cond_parts_ec
 * @name Electrical Conductance
 * The electrical conductance variable from a home-made analog sensor.
 *
 * {{ @ref AnalogElecConductivity_EC::AnalogElecConductivity_EC }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; EC should have 1
 *
 * Range of 0-3V3 with 10bit ADC - resolution of 0.003 = 3 µS/cm.
 */
#define ANALOGELECCONDUCTIVITY_EC_RESOLUTION 1
/// @brief Sensor variable number; EC is stored in sensorValues[0].
#define ANALOGELECCONDUCTIVITY_EC_VAR_NUM 0
/// @brief Variable name in [ODM2 controlled
/// vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricalConductivity"
#define ANALOGELECCONDUCTIVITY_EC_VAR_NAME "electricalConductivity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "microsiemenPerCentimeter" (µS/cm)
#define ANALOGELECCONDUCTIVITY_EC_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "anlgEc"
#define ANALOGELECCONDUCTIVITY_EC_DEFAULT_CODE "anlgEc"
/**@}*/

/**
 * @brief Class for the analog [Electrical Conductivity monitor](@ref
 * sensor_analog_cond)
 *
 * @ingroup sensor_analog_cond
 */
class AnalogElecConductivity : public Sensor {
 public:
    /**
     * @brief Construct a new AnalogElecConductivity object.
     *
     * @param powerPin The port pin providing power to the EC probe.
     * Needs to be switched, and assumed to be same V as the dataPin's ADC.
     * @param dataPin The processor ADC port pin to read the voltage from the EC
     * probe.  Not all processor pins can be used as analog pins.  Those usable
     * as analog pins generally are numbered with an "A" in front of the number
     * - ie, A1.
     * @param Rseries_ohms The resistance of the resistor series (R) in the
     * line; optional with default value of 499.
     * @param sensorEC_Konst The cell constant for the sensing circuit; optional
     * with default value of 2.88 - which is what has been measured for a
     * typical standard sized lamp-type plug.
     * @param measurementsToAverage The number of measurements to average;
     * optional with default value of 1.
     */
    AnalogElecConductivity(int8_t powerPin, int8_t dataPin,
                           float   Rseries_ohms          = RSERIES_OHMS_DEF,
                           float   sensorEC_Konst        = SENSOREC_KONST_DEF,
                           uint8_t measurementsToAverage = 1,
                           AnalogVoltageBase* analogVoltageReader = nullptr);

    /**
     * @brief Destroy the AnalogElecConductivity object - no action needed.
     */
    ~AnalogElecConductivity();

    // Delete copy constructor and copy assignment operator to prevent shallow
    // copies
    AnalogElecConductivity(const AnalogElecConductivity&)            = delete;
    AnalogElecConductivity& operator=(const AnalogElecConductivity&) = delete;

    // Delete move constructor and move assignment operator
    AnalogElecConductivity(AnalogElecConductivity&&)            = delete;
    AnalogElecConductivity& operator=(AnalogElecConductivity&&) = delete;

    /**
     * @brief Report the sensor info.
     *
     * @return Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

    /**
     * @brief Set EC constants for internal calculations.
     * Needs to be set at startup if different from defaults
     *
     * @param sourceResistance_ohms series R used in calculations for EC
     *
     * other possible K, not specified yet:
     *    float  appliedV_V,
     *    uint8_t probeType
     */
    void setEC_k(float sourceResistance_ohms) {
        _Rseries_ohms = sourceResistance_ohms;
    }

 private:
    /// @brief The resistance of the circuit resistor plus any series port
    /// resistance
    float _Rseries_ohms = RSERIES_OHMS_DEF;

    /// @brief the cell constant for the circuit
    float _sensorEC_Konst = SENSOREC_KONST_DEF;
    /// @brief Pointer to analog voltage reader
    AnalogVoltageBase* _analogVoltageReader;
    /// @brief Flag to track if this object owns the analog voltage reader and
    /// should delete it in the destructor
    bool _ownsAnalogVoltageReader;
};

/**
 * @brief The Variable sub-class used for electrical conductivity measured using
 * an analog pin connected to electrodes submerged in the medium
 *
 * @ingroup sensor_analog_cond
 */
class AnalogElecConductivity_EC : public Variable {
 public:
    /**
     * @brief Construct a new  AnalogElecConductivity_EC object.
     *
     * @param parentSense The parent AnalogElecConductivity providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "anlgEc".
     */
    AnalogElecConductivity_EC(
        AnalogElecConductivity* parentSense, const char* uuid = "",
        const char* varCode = ANALOGELECCONDUCTIVITY_EC_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                   (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                   ANALOGELECCONDUCTIVITY_EC_VAR_NAME,
                   ANALOGELECCONDUCTIVITY_EC_UNIT_NAME, varCode, uuid) {}

    /**
     * @brief Construct a new AnalogElecConductivity_EC object.
     *
     * @note This must be tied with a parent AnalogElecConductivity before it
     * can be used.
     */
    AnalogElecConductivity_EC()
        : Variable((uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                   (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                   ANALOGELECCONDUCTIVITY_EC_VAR_NAME,
                   ANALOGELECCONDUCTIVITY_EC_UNIT_NAME,
                   ANALOGELECCONDUCTIVITY_EC_DEFAULT_CODE) {}
    /**
     * @brief Destroy the AnalogElecConductivity_EC object - no action needed.
     */
    ~AnalogElecConductivity_EC() {}
};
/**@}*/
#endif  // SRC_SENSORS_ANALOGELECCONDUCTIVITY_H_

// cSpell:ignore AnalogElecConductivity Rseries_ohms sensorEC_Konst Rwater
// cSpell:ignore _elec_ _Konst anlgEc
