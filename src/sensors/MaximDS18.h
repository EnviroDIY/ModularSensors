/**
 * @file MaximDS18.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MaximDS18 sensor subclass and the MaximDS18_Temp variable
 * subclass.
 *
 * These are for the Maxim DS18B20, DS18S20, MAX31820, DS1822, and DS1820
 * vtemperature sensors.
 *
 * This depends on Dallas Temperature library, which itself is dependent on the
 * OneWire library.
 *
 * The resolution of the DS18B20, DS1822, and MAX31820 temperature sensors are
 * user-configurable to 9, 10, 11, or 12 bits, corresponding to increments of
 * 0.5°C, 0.25°C, 0.125°C, and 0.0625°C, respectively.
 * The default resolution at power-up is 12-bit, unless it has previously been
 * set to something else. The resolution of the DS18S20 is set at 9-bit
 *
 * Accuracy is ± 0.5°C from -10°C to +85°C for DS18S20 and DS18B20
 *             ± 2°C for DS1822 and MAX31820
 *
 * Max time to take reading at 12-bit: 750ms
 * Reset time is < 480 µs
 */

// Header Guards
#ifndef SRC_SENSORS_MAXIMDS18_H_
#define SRC_SENSORS_MAXIMDS18_H_

// Debugging Statement
// #define MS_MAXIMDS18_DEBUG

#ifdef MS_MAXIMDS18_DEBUG
#define MS_DEBUGGING_STD "MaximDS18"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <DallasTemperature.h>
#include <OneWire.h>

// Sensor Specific Defines
#define DS18_NUM_VARIABLES 1
#define DS18_WARM_UP_TIME_MS 2
#define DS18_STABILIZATION_TIME_MS 0
#define DS18_MEASUREMENT_TIME_MS 750
#define DS18_TEMP_VAR_NUM 0
#define DS18_TEMP_RESOLUTION 4

// The main class for the DS18
class MaximDS18 : public Sensor {
 public:
    MaximDS18(DeviceAddress OneWireAddress, int8_t powerPin, int8_t dataPin,
              uint8_t measurementsToAverage = 1);
    MaximDS18(int8_t powerPin, int8_t dataPin,
              uint8_t measurementsToAverage = 1);
    ~MaximDS18();

    bool   setup(void) override;
    String getSensorLocation(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;

 private:
    DeviceAddress _OneWireAddress;
    bool          _addressKnown;
    // Setup an internal OneWire instance to communicate with any OneWire
    // devices (not just Maxim/Dallas temperature ICs)
    OneWire _internalOneWire;
    // Set up the internal a "Dallas Temperature" instance for communication
    // specifically with the temperature sensors.
    DallasTemperature _internalDallasTemp;
    // Turns the address into a printable string
    String makeAddressString(DeviceAddress OneWireAddress);
};


// The class for the Temperature Variable
class MaximDS18_Temp : public Variable {
 public:
    explicit MaximDS18_Temp(MaximDS18* parentSense, const char* uuid = "",
                            const char* varCode = "DS18Temp")
        : Variable(parentSense, (const uint8_t)DS18_TEMP_VAR_NUM,
                   (uint8_t)DS18_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    MaximDS18_Temp()
        : Variable((const uint8_t)DS18_TEMP_VAR_NUM,
                   (uint8_t)DS18_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "DS18Temp") {}
    ~MaximDS18_Temp() {}
};

#endif  // SRC_SENSORS_MAXIMDS18_H_
