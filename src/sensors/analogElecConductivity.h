/**
 * @file analogElecConductivity.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library
 * @copyright 2020 Neil Hancock
 * @author Written By: Neil Hancock <neilh20+aec2008@wLLw.net>; Edited by Sara
 * Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 *
 * @brief Encapsulates an Electrical Conductivity sensors using an anlog
 * input and onboard ADC and ADC ref.
 */
/* clang-format off */
/**
 * @defgroup sensor_analog_cond Conductivity via Analog Electrical Resistance
 * Classes for the Keller Nanolevel capacitive level sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_analog_cond_notes Quick Notes
 * EC from IEC probe, requires ADC_PIN, source resistance, applied
 * voltage, water temperature.
 *
 * Requires switched power pin. The water temperature (if used) must be suplied
 * seperately for a calculation.
 *
 * To use, must switch power to series resistor [default 499R], then measurement
 * from anlog pin A[0-7] and then sensor
 *
 *  SensorV-- adcPin/Ra --- R1 ---- Sensorconnector&Wire  -- Rwater --- Groond
 * R1 series resistance ~ 500ohms
 * Rwater - Resistenace of Water
 * Ra - Resistance of applied Source - possibly uP Digital Pin
 *
 * Returns microSiemens the inverse of resistance
 * @section sensor_analog_cond_parts Parts & Wiring for Mayfly
 * - Mayfly 0.5b or later
 * - Sensor  AC Power Cord 12t with male IEC 320-C8 connector.
 * - 2x10 J3 for any pins A0-A7
 * - Rlmt eg 499R series 1/4W
 *
 * @note Analog JP8 JP9 for ADC 16bit/ADS1115 uses different algorithim
 *
 * @section sensor_analog_cond_wiring Wiring
 * Will use J3-Pin2 A0 but could be any of A0-A3, A5
 *  and one additional pin switches power eg AA (PwrPin)
 *  A7 by default is tied to DS3231 Timer Int
 *  Rlmt/500ohms to A7 and Sensor; J3-Pin10 to Sensor J3-Pin2
 *  Sensors one side; J3-Pin2
 *  Sensor  one side to GND Mayfly J3-Pin20
 *
 * @section sensor_analog_cond_ref References
 * https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino
 * http://www.reagecon.com/pdf/technicalpapers/Effect_of_Temperature_TSP-07_Issue3.pdf
 *
 * @section sensor_analog_cond_ctor Sensor Constructors
 * {{ @ref analogElecConductivity::analogElecConductivity }}
 *
 * ___
 * @section sensor_analog_cond_examples Example Code
 * This sensor is not currently used in any of the examples
 *
 * @menusnip{analog_cond}
 *
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ANALOGELECCONDUCTIVITY_H_
#define SRC_SENSORS_ANALOGELECCONDUCTIVITY_H_

#ifdef MS_ANALOGELECCONDUCTIVITY_DEBUG
#define MS_DEBUGGING_STD "analogElecConductivity"
#endif
#ifdef MS_ANALOGELECCONDUCTIVITY_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "analogElecConductivity"
#endif
// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "SensorBase.h"
#include "VariableBase.h"
#include "math.h"

// Sensor Specific Defines
/** @ingroup sensor_analog_cond */
/**@{*/

#define ANALOGELECCONDUCTIVITY_NUM_VARIABLES 1

/**
 * @anchor sensor_analog_cond_parts_timing
 * @name Sensor Timing
 * The timing for analog conductivity via resistance.
 */
/// @brief Sensor::_warmUpTime_ms; giving 2ms for warm-up.
#define ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS 2
/// @brief Sensor::_stabilizationTime_ms; we assume the analog voltage is stable
/// immediately after warm-up
#define ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; we assume the analog voltage is measured
/// instantly
#define ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS 0
/**@}*/


/**
 * @anchor sensor_analog_cond_parts_ec
 * @name Electrical Conductance
 * The humidity variable from an AOSong DHT
 * - Range: low 100's when open air, for short circuit: a high number
 * - Accuracy: needs determining for each combination of ADC. ADC_REF, and
 * series R. its designed as a very simple relative EC measurement
 *
 * {{ @ref analogElecConductivity_EC::analogElecConductivity_EC }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; EC should have 1
 *
 * Range of 0-3V3 with 10bit ADC - resolution of 0.003 = 3 µS/cm.
 */
#define ANALOGELECCONDUCTIVITY_EC_RESOLUTION 1
/// @brief Variable number; EC is stored in sensorValues[0].
#define ANALOGELECCONDUCTIVITY_EC_VAR_NUM 0
/// @brief Variable name; "electricalConductivity"
#define ANALOGELECCONDUCTIVITY_EC_VAR_NAME "electricalConductivity"
/// @brief Variable unit name; "microsiemenPerCentimeter" (µS/cm)
#define ANALOGELECCONDUCTIVITY_EC_UNIT_NAME "microsiemenPerCentimeter"
/// @brief Default variable short code; "anlgEc"
#define ANALOGELECCONDUCTIVITY_EC_DEFAULT_CODE "anlgEc"
/**@}*/

/**
 * @brief Default resolution (in bits) of the voltage measurement
 *
 * This is the default for all boards, use a build flag to change this, if
 * necessary.
 */
#define analogElecConductivityDef_Resolution 10
#define analogElecConductivityAdc_Max \
    ((1 << analogElecConductivityDef_Resolution) - 1)
#define EC_SENSOR_ADC_RANGE (1 << analogElecConductivityDef_Resolution)

#if !defined PROC_ADC_DEF_REFERENCE
/// @brief One of eAnalogReference for all host platforms.
#define PROC_ADC_DEF_REFERENCE AR_DEFAULT
#endif  // PROC_ADC_DEF_REFERENCE

#if !defined RSERIES_OHMS_DEF
/// @brief The default resistance (in ohms) of the measuring resistor.
#define RSERIES_OHMS_DEF 499
#endif  // RSERIES_OHMS_DEF

/**
 * @brief Class for the analog Electrical Conductivity monitor
 *
 * @ingroup sensor_analog_cond
 */
class analogElecConductivity : public Sensor {
 public:
    /**
     * @brief Construct a new analogElecConductivity object.
     *
     * @param powerPin The port pin providing power to the EC probe.
     * Needs to be switched, and assumed to be same V as the dataPin's ADC.
     * @param dataPin The adc port pin to read the voltage from the EC probe.
     * @param Rseries_ohms The series R in the line. Used to calculate the
     * measured value. optional with default value of 499, that maybe overridden
     * with a define/build flag.
     * @param measurementsToAverage The number of measurements to average;
     * optional with default value of 1.
     *
     */
    analogElecConductivity(int8_t powerPin, int8_t dataPin,
                           float   Rseries_ohms          = RSERIES_OHMS_DEF,
                           uint8_t measurementsToAverage = 1);

    /**
     * @brief Destroy the analogElecConductivity object - no action needed.
     */
    ~analogElecConductivity();

    /**
     * @brief Report the sensor info.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

    /**
     * @brief Set where to find (a pointer) WaterTemperature for internal
     * calculations. The reference needs to be updated before every calculation,
     * (if temperature has changed)
     */
    void setWaterTemperature(float* ptrWaterTemperature_C) {
        _ptrWaterTemperature_C = ptrWaterTemperature_C;
    }

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

    /**
     * @brief reads the calculated EC from an analog pin.
     *
     * @param analogPinNum (optional) Analog Port, or if not supplied the
     * internal portNumber.
     *
     * @return EC value
     */
    float readEC(void);
    float readEC(uint8_t analogPinNum);

 private:
    const char* _version;
    int8_t      _EcPowerPin;
    int8_t      _EcAdcPin;

    float*      _ptrWaterTemperature_C;
    const float SensorV = 3.3;

    // _Rseries_ohms ~ that is R1 + any series port resistance
    float _Rseries_ohms = RSERIES_OHMS_DEF;

#if !defined TEMPERATURECOEF_DEF
/// @brief TEMPERATURECOEF_DEF defined for chemical/transport under measurement
#define TEMPERATURECOEF_DEF 0.019
#endif  // TEMPERATURECOEF_DEF
    const float TemperatureCoef = TEMPERATURECOEF_DEF;

#if !defined SENSOREC_KONST_DEF
/**
 * @brief  Cell Constant For Ec Measurements - place holder.
 *
 * Mine was around 2.9 with plugs being a standard size they should all be
 * around the same. If you get bad readings you can use the calibration script
 * and fluid to get a better estimate for K.
 */
#define SENSOREC_KONST_DEF 2.88
#endif  // SENSOREC_KONST_DEF
    const float sensorEC_Konst = SENSOREC_KONST_DEF;
};

/**
 * @brief The variable class used for electricalConductivity measured using an
 * analog pin connected to electrodes submerged in the medium
 *
 * @ingroup sensor_analog_cond
 *
 */
class analogElecConductivity_EC : public Variable {
 public:
    /**
     * @brief Construct a new  analogElecConductivity_EC object.
     *
     * @param parentSense The parent analogElecConductivity providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "anlgEc".
     */
    analogElecConductivity_EC(
        analogElecConductivity* parentSense, const char* uuid = "",
        const char* varCode = ANALOGELECCONDUCTIVITY_EC_DEFAULT_CODE)
        : Variable(parentSense,
                   (const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                   (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                   ANALOGELECCONDUCTIVITY_EC_VAR_NAME,
                   ANALOGELECCONDUCTIVITY_EC_UNIT_NAME, varCode, uuid) {}

    /**
     * @brief Construct a new analogElecConductivity_EC object.
     *
     * @note This must be tied with a parent analogElecConductivity before it
     * can be used.
     */
    analogElecConductivity_EC()
        : Variable((const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                   (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                   ANALOGELECCONDUCTIVITY_EC_VAR_NAME,
                   ANALOGELECCONDUCTIVITY_EC_UNIT_NAME,
                   ANALOGELECCONDUCTIVITY_EC_DEFAULT_CODE) {}
    /**
     * @brief Destroy the analogElecConductivity_EC object - no action needed.
     */
    ~analogElecConductivity_EC() {}
};
/**@}*/
#endif  // SRC_SENSORS_ANALOGELECCONDUCTIVITY_H_
