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
 * Classes that inherit from this base must implement the
 * readVoltageSingleEnded method to provide their specific voltage reading
 * implementation.
 */
class AnalogVoltageBase {
 public:
    /**
     * @brief Construct a new AnalogVoltageBase object
     *
     * @param analogChannel The analog channel/pin for voltage readings
     * @param voltageMultiplier The voltage multiplier for any voltage dividers
     * @param supplyVoltage The supply/operating voltage for the analog system
     * @param analogDifferentialChannel The second channel for differential
     * measurements (-1 if not used)
     */
    AnalogVoltageBase(int8_t analogChannel, float voltageMultiplier = 1.0,
                      float  supplyVoltage             = OPERATING_VOLTAGE,
                      int8_t analogDifferentialChannel = -1)
        : _analogChannel(analogChannel),
          _voltageMultiplier(voltageMultiplier),
          _supplyVoltage(supplyVoltage),
          _analogDifferentialChannel(analogDifferentialChannel) {}

    /**
     * @brief Destroy the AnalogVoltageBase object
     */
    virtual ~AnalogVoltageBase() = default;

    /**
     * @brief Set the voltage multiplier for voltage divider calculations
     *
     * @param voltageMultiplier The multiplier value for voltage scaling
     */
    void setVoltageMultiplier(float voltageMultiplier) {
        _voltageMultiplier = voltageMultiplier;
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
        _supplyVoltage = supplyVoltage;
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
     * @brief Check if this instance is configured for differential measurements
     *
     * @return True if both analog channels are valid pins (>=0) for
     * differential measurements
     */
    bool isDifferential(void) const {
        return (_analogChannel >= 0 && _analogDifferentialChannel >= 0);
    }

    /**
     * @brief Read a single-ended voltage measurement
     *
     * This pure virtual function must be implemented by derived classes to
     * provide their specific method of reading analog voltages.
     *
     * @param resultValue Reference to store the resulting voltage measurement
     * @return True if the voltage reading was successful and within valid range
     */
    virtual bool readVoltageSingleEnded(float& resultValue) = 0;

    /**
     * @brief Read a differential voltage measurement
     *
     * This virtual function provides a default implementation for differential
     * voltage readings. Derived classes can override this to provide their
     * specific differential reading implementation.
     *
     * @param resultValue Reference to store the resulting voltage measurement
     * @return True if the voltage reading was successful and within valid range
     */
    virtual bool readVoltageDifferential(float& resultValue) {
        resultValue = -9999.0;
        return false;
    }

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
     * @brief Internal reference to the analog channel/pin
     *
     * For TIADS1x15: ADS channel (0-3)
     * For ProcessorAnalog: processor ADC pin number
     */
    int8_t _analogChannel;

    /**
     * @brief Internal reference to the voltage multiplier
     *
     * Multiplier to apply for voltage divider calculations
     */
    float _voltageMultiplier;

    /**
     * @brief Internal reference to the supply voltage
     *
     * For TIADS1x15: the ADS supply voltage
     * For ProcessorAnalog: the processor operating voltage
     */
    float _supplyVoltage;

    /**
     * @brief Internal reference to the second analog channel for differential
     * measurements
     *
     * For TIADS1x15: second ADS channel for differential readings (-1 if not
     * used) For ProcessorAnalog: not used (-1)
     */
    int8_t _analogDifferentialChannel;
};

#endif  // SRC_SENSORS_ANALOGVOLTAGEBASE_H_
