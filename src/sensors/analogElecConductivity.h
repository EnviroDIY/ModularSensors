/**
 * @file analogElecConductivity.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library
 * @author Written By: Neil Hancock <neilh20+aec2008@wLLw.net>
 *
 *
 * @brief This encapsulates an Electrical Conductivity sensors using an anlog
 *input and onboard ADC and ADC ref.
 *
 * EC from IEC probe, requires ADC_PIN, Source Resistance, Applied
 * Voltage, water temperature.
 *
 * Requires switched power pin. The water Temperature (if used) must be suplied
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
 * Parts & Wiring for Mayfly
 *  Mayfly 0.5b or later
 *  Sensor  AC Power Cord 12t with male IEC 320-C8 connector.
 *  2x10 J3 for any pins A0-A7
 *  Rlmt eg 499R series 1/4W
 *
 *  Analog JP8 JP9 for ADC 16bit/ADS1115 uses different algorithim
 *
 * Wiring
 * Will use J3-Pin2 A0 but could be any of A0-A3,A5
 *  and one pin switches pwr eg AA (PwrPin)
 *  A7 by default is tied to DS3231 Timer Int
 *  Rlmt/500ohms to A7 and Sensor; J3-Pin10 to Sensor J3-Pin2
 *  Sensors one side; J3-Pin2
 *  Sensor  one side to GND Mayfly J3-Pin20
 *
 *For EC calculation
 *  (better to be ratio meteric))
 *  Range of 0-3V3 with 10bit ADC - resolution of 0.003
 *
 * https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino
 * http://www.reagecon.com/pdf/technicalpapers/Effect_of_Temperature_TSP-07_Issue3.pdf
 *
 */

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
#define ANALOGELECCONDUCTIVITY_NUM_VARIABLES 1
#define ANALOGELECCONDUCTIVITY_WARM_UP_TIME_MS 2
#define ANALOGELECCONDUCTIVITY_STABILIZATION_TIME_MS 0
#define ANALOGELECCONDUCTIVITY_MEASUREMENT_TIME_MS 0

#define ANALOGELECCONDUCTIVITY_EC_RESOLUTION 1
#define ANALOGELECCONDUCTIVITY_EC_VAR_NUM 0

#define analogElecConductivityDef_Resolution \
    10  // Default for all boards, change through API as needed
#define analogElecConductivityAdc_Max \
    ((1 << analogElecConductivityDef_Resolution) - 1)
#define EC_SENSOR_ADC_RANGE (1 << analogElecConductivityDef_Resolution)

#if !defined SENSOR_UNINIT_VAL
#define SENSOR_UNINIT_VAL -0.1
#endif  // SENSOR_UNINIT_VAL

#if !defined ProcAdcDef_Reference
// one of eAnalogReference for all host platforms
#define ProcAdcDef_Reference AR_DEFAULT
#endif  // ProcAdcDef_Reference

/**
 * @brief Class for the analog Electrical Conductivity monitor
 *
 */
class analogElecConductivity : public Sensor {
 public:
    /**
     * @brief Construct a new analogElecConductivity object.
     *
     * @param powerPin The port pin providing power to the EC probe.
     * Needs to be switched, and assumed to be same V as the dataPin's ADC.
     *
     * @param dataPin The adc port pin to read the voltage from the EC probe.
     *
     * @param measurementsToAverage The number of measurements to average;
     * optional with default value of 1.
     */
    analogElecConductivity(int8_t powerPin, int8_t dataPin,
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
    // void set_active_sensors(uint8_t sensors_mask);
    // uint8_t which_sensors_active(void);
    // void setWaterTemperature(float  WaterTemperature_C);

    /**
     * @brief Set where to find (a pointer) WaterTemperature for internal
     * calculations. The reference needs to be updated before every calculation,
     * (if temperature has changed)
     *
     * @return none
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
     *
     * @return none
     */
    void setEC_k(float sourceResistance_ohms) {
        Rseries_ohms = sourceResistance_ohms;
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

    // float _WaterTemperature_C;
    float*      _ptrWaterTemperature_C;
    const float SensorV = 3.3;
#if !defined RSERIES_OHMS_DEF
#define RSERIES_OHMS_DEF 499
#endif  // RSERIES_OHMS_DEF
    float Rseries_ohms =
        RSERIES_OHMS_DEF;  // that is R1 + any series port resistance
#if !defined TEMPERATURECOEF_DEF
#define TEMPERATURECOEF_DEF 0.019
#endif  // TEMPERATURECOEF_DEF
    const float TemperatureCoef =
        TEMPERATURECOEF_DEF;  // depends on what chemical/transport is being
                              // measured
//********************** Cell Constant For Ec Measurements
//*********************// Mine was around 2.9 with plugs being a standard size
// they should all be around the same But If you get bad readings you can use
// the calibration script and fluid to get a better estimate for K
#if !defined SENSOREC_KONST_DEF
#define SENSOREC_KONST_DEF 2.88
#endif  // SENSOREC_KONST_DEF
    const float sensorEC_Konst = SENSOREC_KONST_DEF;
};

/**
 * @brief The variable class used for electricalConductivity measured using an
 * analog pin connected to electrodes submerged in the medium
 *
 * - Units are uScm,
 * - Range: low 100's when open air,
 *   for short circuit: a high number
 * - Accuracy: needs determining for each combination of ADC. ADC_REF, and
 * series R. its designed as a very simple relative EC measurement
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
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is anlgEc
     */
    analogElecConductivity_EC(analogElecConductivity* parentSense,
                              const char*             uuid    = "",
                              const char*             varCode = "anlgEc")
        : Variable(parentSense,
                   (const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                   (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                   "electricalConductivity", "uScm", varCode, uuid) {}

    /**
     * @brief Construct a new analogElecConductivity_EC object.
     *
     * @note This must be tied with a parent BoschBME280 before it can be used.
     */
    analogElecConductivity_EC()
        : Variable((const uint8_t)ANALOGELECCONDUCTIVITY_EC_VAR_NUM,
                   (uint8_t)ANALOGELECCONDUCTIVITY_EC_RESOLUTION,
                   "ElectricalConductivity", "uScm", "anlgEc") {}
    /**
     * @brief Destroy the analogElecConductivity_EC object - no action needed.
     */
    ~analogElecConductivity_EC() {}
};

#endif  // SRC_SENSORS_ANALOGELECCONDUCTIVITY_H_
