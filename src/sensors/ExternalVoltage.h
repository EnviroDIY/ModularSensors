/**
 * @file ExternalVoltage.h *
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief This file contains the ExternalVoltage sensor subclass and the
 * ExternalVoltage_Volt variable subclass.
 *
 * These for any voltage measureable on a TI ADS1115 or ADS1015.  There is a
 * multiplier allowed for a voltage divider between the raw voltage and the ADS.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 *
 * The grove voltage divider is a simple voltage divider designed to measure
 * high external voltages on a low voltage ADC. This module employs a variable
 * gain via two pairs of voltage dividers, and a unity gain amplification to
 * reduce output impedance of the module.
 *
 * Range:
 *   NOTE:  Range is determined by supply voltage - No more than VDD + 0.3 V or
 *          5.5 V (whichever is smaller) must be applied to this device.
 *   without voltage divider:  0 - 3.6V [when ADC is powered at 3.3V]
 *   1/gain = 3x: 0.3 ~ 12.9V
 *   1/gain = 10x: 1 ~ 43V
 * Accuracy:
 *   16-bit ADC: < 0.25% (gain error), <0.25 LSB (offset errror)
 *   12-bit ADC: < 0.15% (gain error), <3 LSB (offset errror)
 * Resolution:
 *   NOTE:  1 bit of resolution is lost in single-ended reading.  The maximum
 *          possible resolution is over the differential range from
 *          negative to positive full scale, a single ended reading is only over
 *          the range from 0 to positive full scale)
 *   NOTE:  Assuming the ADC is powered at 3.3V, this program sets the the ACD's
 *          inbuilt gain to 1, which divides the bit resolution over range of
 *          0-4.096V
 *   16-bit ADC:
 *     without voltage divider:  0.125 mV
 *     1/gain = 3x: 0.375 mV
 *     1/gain = 10x: 1.25 mV
 *   12-bit ADC:
 *     without voltage divider:  2 mV
 *     1/gain = 3x: 6 mV
 *     1/gain = 10x: 20 mV
 *
 * Technical specifications for the Grove Voltage Divider can be found at:
 * http://wiki.seeedstudio.com/Grove-Voltage_Divider
 *
 * Technical specifications for the TI ADS1115 can be found at:
 * http://www.ti.com/product/ADS1115
 *
 * Response time: < 1ms
 * Resample time: max of ADC (860/sec)
 */

// Header Guards
#ifndef SRC_SENSORS_EXTERNALVOLTAGE_H_
#define SRC_SENSORS_EXTERNALVOLTAGE_H_

// Debugging Statement
// #define MS_EXTERNALVOLTAGE_DEBUG

#ifdef MS_EXTERNALVOLTAGE_DEBUG
#define MS_DEBUGGING_STD "ExternalVoltage"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

// Sensor Specific Defines
/// Sensor::_numReturnedValues; the ADS1115 can report 1 value.
#define EXT_VOLT_NUM_VARIABLES 1
// Using the warm-up time of the ADS1115
/// Sensor::_warmUpTime_ms; the ADS1115 warms up in 2ms.
#define EXT_VOLT_WARM_UP_TIME_MS 2
// Assume a voltage is instantly ready
/// Sensor::_stabilizationTime_ms; the ADS1115 is stable 0ms after warm-up.
#define EXT_VOLT_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the ADS1115 takes 0ms to complete a
/// measurement.
#define EXT_VOLT_MEASUREMENT_TIME_MS 0

/// Variable number; voltage is stored in sensorValues[0].
#define EXT_VOLT_VAR_NUM 0
#ifdef MS_USE_ADS1015
/// Decimals places in string representation; voltage should have 1.
#define EXT_VOLT_RESOLUTION 1
#else
/// Decimals places in string representation; voltage should have 4.
#define EXT_VOLT_RESOLUTION 4
#endif

/// The assumed address fo the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/**
 * @brief The main class for the external votlage monitor
 sensor
 */
class ExternalVoltage : public Sensor {
 public:
    /**
     * @brief Construct a new External Voltage object - need the power pin and
     * the data channel on the ADS1x15
     *
     * The gain value, I2C address, and number of measurements to average are
     * optional.  If nothing is given a 1x gain is used.
     *
     * @param powerPin  The pin on the mcu controlling power to the sensor.  Use
     * -1 if the sensor is continuously powered.
     * @param adsChannel The ACS channel of interest (0-3).
     * @param gain The gain multiplier, if a voltage divider is used.
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to average
     */
    ExternalVoltage(int8_t powerPin, uint8_t adsChannel, float gain = 1,
                    uint8_t i2cAddress            = ADS1115_ADDRESS,
                    uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the External Voltage object
     */
    ~ExternalVoltage();

    String getSensorLocation(void) override;

    bool addSingleMeasurementResult(void) override;

 protected:
    uint8_t _adsChannel;
    float   _gain;
    uint8_t _i2cAddress;
};


// The single available variable is voltage
class ExternalVoltage_Volt : public Variable {
 public:
    /**
     * @brief Construct a new ExternalVoltage_Volt object.
     *
     * @param parentSense The parent ExternalVoltage providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable.  Default is an empty string.
     * @param varCode A short code to help identify the variable in files.
     * Default is extVoltage
     */
    explicit ExternalVoltage_Volt(ExternalVoltage* parentSense,
                                  const char*      uuid    = "",
                                  const char*      varCode = "extVoltage")
        : Variable(parentSense, (const uint8_t)EXT_VOLT_VAR_NUM,
                   (uint8_t)EXT_VOLT_RESOLUTION, "voltage", "volt", varCode,
                   uuid) {}
    /**
     * @brief Construct a new ExternalVoltage_Volt object.
     *
     * @note This must be tied with a parent ExternalVoltage before it can be
     * used.
     */
    ExternalVoltage_Volt()
        : Variable((const uint8_t)EXT_VOLT_VAR_NUM,
                   (uint8_t)EXT_VOLT_RESOLUTION, "voltage", "volt",
                   "extVoltage") {}
    /**
     * @brief Destroy the ExternalVoltage_Volt object - no action needed.
     */
    ~ExternalVoltage_Volt() {}
};

#endif  // SRC_SENSORS_EXTERNALVOLTAGE_H_
