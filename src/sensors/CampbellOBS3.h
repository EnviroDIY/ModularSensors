/**
 * @file CampbellOBS3.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the CampbellOBS3 sensor subclass and the variable subclasses
 * CampbellOBS3_Turbidity and CampbellOBS3_Voltage.
 *
 * These are used for the Campbell Scientific OBS-3+.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * Ranges: (depends on sediment size, particle shape, and reflectivity)
 *  Turbidity (low/high): 250/1000 NTU; 500/2000 NTU; 1000/4000 NTU
 *  Mud: 5000 to 10,000 mg L–1
 *  Sand: 50,000 to 100,000 mg L–1
 * Accuracy: (whichever is larger)
 *  Turbidity: 2% of reading or 0.5 NTU
 *  Mud: 2% of reading or 1 mg L–1
 *  Sand: 4% of reading or 10 mg L–1
 * Resolution:
 *  16-bit ADC
 *      Turbidity: 0.03125/0.125 NTU; 0.0625/0.25 NTU; 0.125/0.5 NTU
 *  12-bit ADC
 *      Turbidity: 0.5/2.0 NTU; 1.0/4.0 NTU; 2.0/8.0 NTU
 *
 * Minimum stabilization time: 2s
 * Maximum data rate = 10Hz (100ms/sample)
 */

// Header Guards
#ifndef SRC_SENSORS_CAMPBELLOBS3_H_
#define SRC_SENSORS_CAMPBELLOBS3_H_

// Debugging Statement
// #define MS_CAMPBELLOBS3_DEBUG

#ifdef MS_CAMPBELLOBS3_DEBUG
#define MS_DEBUGGING_STD "CampbellOBS3"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines

// low and high range are treated as completely independent, so only 2
// "variables" One for the raw voltage and another for the calibrated turbidity.
// To get both high and low range values, create two sensor objects!
/// Sensor::_numReturnedValues; the OBS3 can report 2 values.
#define OBS3_NUM_VARIABLES 2
// Using the warm-up time of the ADS1115
/// Sensor::_warmUpTime_ms; OBS3 warms up in 2ms.
#define OBS3_WARM_UP_TIME_MS 2
/// Sensor::_stabilizationTime_ms; OBS3 is stable after 2000ms.
#define OBS3_STABILIZATION_TIME_MS 2000
/// Sensor::_measurementTime_ms; OBS3 takes 100ms to complete a measurement.
#define OBS3_MEASUREMENT_TIME_MS 100

/// Variable number; turbidity is stored in sensorValues[0].
#define OBS3_TURB_VAR_NUM 0
#ifdef MS_USE_ADS1015
/// Decimals places in string representation; turbidity should have 1.
#define OBS3_RESOLUTION 1
#else
/// Decimals places in string representation; turbidity should have 5.
#define OBS3_RESOLUTION 5
#endif

/// Variable number; voltage is stored in sensorValues[1].
#define OBS3_VOLTAGE_VAR_NUM 1
#ifdef MS_USE_ADS1015
/// Decimals places in string representation; voltage should have 1.
#define OBS3_VOLT_RESOLUTION 1
#else
/// Decimals places in string representation; voltage should have 4.
#define OBS3_VOLT_RESOLUTION 4
#endif

// The main class for the Campbell OBS3
class CampbellOBS3 : public Sensor {
 public:
    // The constructor - need the power pin, the ADS1X15 data channel, and the
    // calibration info
    /**
     * @brief Construct a new Campbell OBS3 object - need the power pin, the
     * ADS1X15 data channel, and the calibration info
     *
     * @param powerPin  The pin on the mcu controlling power to the Apogee
     * SQ-212.  Use -1 if the sensor is continuously powered.
     * @param adsChannel The ACS channel the OBS3 is connected to (0-3).
     * @param x2_coeff_A The x2 (A) coefficient for the calibration in volts
     * @param x1_coeff_B The x (B) coefficient for the calibration in volts
     * @param x0_coeff_C The x0 (C) coefficient for the calibration in volts
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to average.
     */
    CampbellOBS3(int8_t powerPin, uint8_t adsChannel, float x2_coeff_A,
                 float x1_coeff_B, float x0_coeff_C, uint8_t i2cAddress = 0x48,
                 uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the Campbell OBS3 object
     */
    ~CampbellOBS3();

    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    uint8_t _adsChannel;
    float   _x2_coeff_A, _x1_coeff_B, _x0_coeff_C;
    uint8_t _i2cAddress;
};


// The main variable returned is turbidity
// To utilize both high and low gain turbidity, you must create *two* sensor
// objects on two different data channels and then create two variable objects,
// one tied to each sensor.
class CampbellOBS3_Turbidity : public Variable {
 public:
    /**
     * @brief Construct a new CampbellOBS3_Turbidity object.
     *
     * @param parentSense The parent CampbellOBS3 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is OBS3Turbidity
     */
    explicit CampbellOBS3_Turbidity(CampbellOBS3* parentSense,
                                    const char*   uuid    = "",
                                    const char*   varCode = "OBS3Turbidity")
        : Variable(parentSense, (const uint8_t)OBS3_TURB_VAR_NUM,
                   (uint8_t)OBS3_RESOLUTION, "turbidity",
                   "nephelometricTurbidityUnit", varCode, uuid) {}
    /**
     * @brief Construct a new CampbellOBS3_Turbidity object.
     *
     * @note This must be tied with a parent CampbellOBS3 before it can be used.
     */
    CampbellOBS3_Turbidity()
        : Variable((const uint8_t)OBS3_TURB_VAR_NUM, (uint8_t)OBS3_RESOLUTION,
                   "turbidity", "nephelometricTurbidityUnit", "OBS3Turbidity") {
    }
    ~CampbellOBS3_Turbidity() {}
};


// Also returning raw voltage
// This could be helpful if the calibration equation was typed incorrectly
class CampbellOBS3_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new CampbellOBS3_Voltage object.
     *
     * @param parentSense The parent CampbellOBS3 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is OBS3Voltage
     */
    explicit CampbellOBS3_Voltage(CampbellOBS3* parentSense,
                                  const char*   uuid    = "",
                                  const char*   varCode = "OBS3Voltage")
        : Variable(parentSense, (const uint8_t)OBS3_VOLTAGE_VAR_NUM,
                   (uint8_t)OBS3_VOLT_RESOLUTION, "voltage", "volt", varCode,
                   uuid) {}
    /**
     * @brief Construct a new CampbellOBS3_Voltage object.
     *
     * @note This must be tied with a parent CampbellOBS3 before it can be used.
     */
    CampbellOBS3_Voltage()
        : Variable((const uint8_t)OBS3_VOLTAGE_VAR_NUM,
                   (uint8_t)OBS3_VOLT_RESOLUTION, "voltage", "volt",
                   "OBS3Voltage") {}
    /**
     * @brief Destroy the CampbellOBS3_Voltage object - no action needed.
     */
    ~CampbellOBS3_Voltage() {}
};

#endif  // SRC_SENSORS_CAMPBELLOBS3_H_
