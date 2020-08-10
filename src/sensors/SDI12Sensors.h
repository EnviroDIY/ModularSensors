/**
 * @file SDI12Sensors.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the SDI12Sensors sensor subclass, itself a parent class for
 * all devices that communicate over SDI-12.
 *
 * This depends on the EnviroDIY SDI-12 library.
 *
 * Documentation for the SDI-12 Protocol commands and responses can be found at:
 * http://www.sdi-12.org/
 *
 * @defgroup sdi12_group SDI-12 Sensors
 * Classes for all @ref sdi12_group
 *
 * @copydoc sdi12_page
 */
/* clang-format off */
/**
 * @page sdi12_page SDI-12 Sensors
 *
 * @tableofcontents
 *
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_SDI12SENSORS_H_
#define SRC_SENSORS_SDI12SENSORS_H_

// Debugging Statement
// #define MS_SDI12SENSORS_DEBUG

#ifdef MS_SDI12SENSORS_DEBUG
#define MS_DEBUGGING_STD "SDI12Sensors"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#ifdef SDI12_EXTERNAL_PCINT
#include <SDI12.h>
#else
#include <SDI12_ExtInts.h>
#endif
// NOTE:  Can use the "regular" sdi-12 library with build flag -D
// SDI12_EXTERNAL_PCINT Unfortunately, that is not compatible with the Arduino
// IDE

/**
 * @brief The main class for SDI-12 Sensors
 */
class SDI12Sensors : public Sensor {
 public:
    /**
     * @brief Construct a new SDI 12 Sensors.  This is only intended to be used within
     * this library.
     *
     * @param SDI12address The SDI-12 address of the sensor.
     * @param powerPin A pin on the mcu controlling power to the sensor.
     * Defaults to -1.
     * @param dataPin A pin on the mcu receiving data from the sensor.  Defaults
     * to -1.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor.  Defaults to 1.
     * @param sensorName The name of the sensor.  Defaults to "SDI12-Sensor".
     * @param numReturnedVars The number of variable results returned by the
     * sensor.  Defaults to 1.
     * @param warmUpTime_ms The time in ms between when the sensor is powered on
     * and when it is ready to receive a wake command.  Defaults to 0.
     * @param stabilizationTime_ms The time in ms between when the sensor
     * receives a wake command and when it is able to return stable values.
     * Defaults to 0.
     * @param measurementTime_ms The time in ms between when a measurement is
     * started and when the result value is available.  Defaults to 0.
     */
    SDI12Sensors(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                 uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0);
    /**
     * @copydoc SDI12Sensors::SDI12Sensors
     */
    SDI12Sensors(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                 uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0);
    /**
     * @copydoc SDI12Sensors::SDI12Sensors
     */
    SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                 uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0);
    /**
     * @brief Destroy the SDI12Sensors object - no action taken
     */
    virtual ~SDI12Sensors();

    /**
     * @brief Get the stored sensor vendor name returned by a previously called
     * SDI-12 get sensor information (aI!) command.
     *
     * @return String The name of the sensor vendor as reported by the sensor
     * itself.
     */
    String getSensorVendor(void);
    /**
     * @brief Get the stored sensor model name returned by a previously called
     * SDI-12 get sensor information (aI!) command.
     *
     * @return String The name of the sensor model as reported by the sensor
     * itself.
     */
    String getSensorModel(void);
    /**
     * @brief Get the stored sensor version returned by a previously called
     * SDI-12 get sensor information (aI!) command.
     *
     * @return String The version of the sensor as reported by the sensor
     * itself.
     */
    String getSensorVersion(void);
    /**
     * @brief Get the stored sensor serial number returned by a previously
     * called SDI-12 get sensor information (aI!) command.
     *
     * @return String The serial number of the sensor as reported by the sensor
     * itself.
     */
    String getSensorSerialNumber(void);
    /**
     * @copydoc Sensor::getSensorLocation()
     *
     * For SDI-12 sensors this returns a concatenation of the data pin number
     * and the SDI-12 address.
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the pin modes for the data and power pin, sets the stream
     * timeout time and value, and sets the status bit.  This also sets the
     * mcu's timer prescaler values to clock the serial communication.  After
     * setting the pins and prescaler, the setup tests for a response from the
     * sensor and calls the getSensorInfo() function.  Sensor power **is**
     * required.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;

    /**
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the #_millisMeasurementRequested timestamp.
     *
     * @note This function does NOT include any waiting for the sensor to be
     * warmed up or stable!
     *
     * @return **bool** True if the start measurement function completed successfully.
     */
    bool startSingleMeasurement(void) override;
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 protected:
    /**
     * @brief Send the SDI-12 'acknowledge active' command [address][!] to a
     * sensor and confirm that the correct sensor responded.
     *
     * @return **bool** True if the correct SDI-12 sensor replied to the command.
     */
    bool requestSensorAcknowledgement(void);
    /**
     * @brief Send the SDI-12 'info' command [address][I][!] to a sensor and
     * parse the result into the vendor, model, version, and serial number.
     *
     * @return **bool** True if all expected information fields returned by the sensor.
     */
    bool getSensorInfo(void);
    /**
     * @brief Internal reference to the SDI-12 object.
     */
    SDI12 _SDI12Internal;
    /**
     * @brief Internal reference to the SDI-12 address.
     */
    char _SDI12address;

 private:
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
};

#endif  // SRC_SENSORS_SDI12SENSORS_H_
