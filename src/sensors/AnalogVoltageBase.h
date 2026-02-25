/**
 * @file AnalogVoltageBase.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AnalogVoltageBase abstract class for providing a unified
 * interface for analog voltage reading sensors.
 *
 * This abstract base class provides a common interface for sensors that can
 * read single-ended analog voltages, either through external ADCs (like TI
 * ADS1x15) or built-in processor ADCs.
 */

// Header Guards
#ifndef SRC_SENSORS_ANALOGVOLTAGEBASE_H_
#define SRC_SENSORS_ANALOGVOLTAGEBASE_H_

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Include the known processors for default values
#include "KnownProcessors.h"

/**
 * @brief Abstract base class for analog voltage reading systems.
 *
 * This class provides a unified interface for analog voltage reading,
 * whether from external ADCs (like TI ADS1x15) or processor built-in ADCs.
 * Classes that inherit from this base must implement three pure virtual
 * methods:
 * - readVoltageSingleEnded() for single-ended voltage measurements,
 * - readVoltageDifferential() for differential voltage measurements, and
 * - getSensorLocation() to provide sensor location identification.
 */
class AnalogVoltageBase {
 public:
    /**
     * @brief Construct a new AnalogVoltageBase object
     *
     * @param voltageMultiplier The voltage multiplier for any voltage dividers
     * @param supplyVoltage The supply/operating voltage for the analog system
     */
    AnalogVoltageBase(float voltageMultiplier = 1.0f,
                      float supplyVoltage     = OPERATING_VOLTAGE)
        :  // NOTE: These clamps are intentionally silent — Serial/MS_DBG is NOT
           // safe to call during construction (the Serial object may not be
           // initialized yet on Arduino targets). Use setSupplyVoltage() at
           // runtime for logged clamping.
          _voltageMultiplier((voltageMultiplier > 0.0f) ? voltageMultiplier
                                                        : 1.0f),
          _supplyVoltage((supplyVoltage > 0.0f) ? supplyVoltage
                                                : OPERATING_VOLTAGE) {}

    /**
     * @brief Destroy the AnalogVoltageBase object
     */
    virtual ~AnalogVoltageBase() = default;

    /**
     * @brief Set the voltage multiplier for voltage divider calculations
     *
     * @param voltageMultiplier The multiplier value for voltage scaling
     *
     * @note The multiplier must be positive (> 0). Values <= 0 will be
     * set to 1 to prevent division-by-zero errors and maintain valid voltage
     * calculations.
     */
    virtual void setVoltageMultiplier(float voltageMultiplier) {
        // If the input voltage multiplier is not positive, set it to 1.
        _voltageMultiplier = (voltageMultiplier > 0.0f) ? voltageMultiplier
                                                        : 1.0f;
    }

    /**
     * @brief Get the voltage multiplier value
     *
     * @return The current voltage multiplier
     */
    float getVoltageMultiplier(void) const {
        return _voltageMultiplier;
    }

    /**
     * @brief Set the supply voltage for the analog system
     *
     * @param supplyVoltage The supply voltage in volts
     */
    virtual void setSupplyVoltage(float supplyVoltage) {
        _supplyVoltage = (supplyVoltage > 0.0f) ? supplyVoltage
                                                : OPERATING_VOLTAGE;
    }
    /**
     * @brief Get the supply voltage for the analog system
     *
     * @return The current supply voltage in volts
     */
    float getSupplyVoltage(void) const {
        return _supplyVoltage;
    }

    /**
     * @brief Read a single-ended voltage measurement
     *
     * This pure virtual function must be implemented by derived classes to
     * provide their specific method of reading analog voltages.
     *
     * @param analogChannel The analog channel/pin for voltage readings
     * @param resultValue Reference to store the resulting voltage measurement
     * @return True if the voltage reading was successful and within valid range
     */
    virtual bool readVoltageSingleEnded(int8_t analogChannel,
                                        float& resultValue) = 0;

    /**
     * @brief Read a differential voltage measurement
     *
     * This pure virtual function must be implemented by derived classes to
     * provide their specific method of reading differential voltages.
     *
     * If the sensor does not support differential measurements, this function
     * should set the resultValue to -9999.0 and return false.
     *
     * @param analogChannel The primary analog channel for differential
     * measurement
     * @param analogReferenceChannel The secondary (reference) analog channel
     * for differential measurement
     * @param resultValue Reference to store the resulting voltage measurement
     * @return True if the voltage reading was successful and within valid range
     */
    virtual bool readVoltageDifferential(int8_t analogChannel,
                                         int8_t analogReferenceChannel,
                                         float& resultValue) = 0;

    /**
     * @brief Get the sensor location string
     *
     * This pure virtual function must be implemented by derived classes to
     * provide their specific sensor location identification string.
     *
     * @return A string describing the sensor location
     */
    virtual String getSensorLocation(void) = 0;

 protected:
    /**
     * @brief Stored voltage multiplier value
     *
     * Multiplier to apply for voltage divider calculations
     */
    float _voltageMultiplier;

    /**
     * @brief Stored supply voltage value
     *
     * For TIADS1x15: the ADS supply voltage
     * For ProcessorAnalog: the processor operating voltage
     */
    float _supplyVoltage;
};

#endif  // SRC_SENSORS_ANALOGVOLTAGEBASE_H_
