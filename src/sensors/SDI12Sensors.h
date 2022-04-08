/**
 * @file SDI12Sensors.h
 * @copyright 2017-2022 Stroud Water Research Center
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
 */
/* clang-format off */
/**
 * @defgroup sdi12_group SDI-12 Sensors
 * The Sensor and Variable objects for all SDI-12 Sensors
 *
 * @ingroup the_sensors
 *
 * SDI-12 is a common single-wire data protocol in environmental sensors.
 * The details of the communication are managed by the
 * [Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12). In
 * short, data is transferred between a master and a slave on a single wire at
 * 1200 baud. The number of possible SDI-12 commands is fairly limited, focused
 * only on taking measurements and collecting data. The voltage range for the
 * data communication and the sensors wake and sleep timings are also limited by
 * the protocol.
 *
 * The SDI-12 protocol specifies that all new devices should come from the
 * manufacturer with a pre-programmed address of "0". For Meter brand sensors,
 * you *must* change the sensor address before you can begin to use it. For
 * other sensors, the address may be left at 0 if you are only using a single
 * sensor. If you want to use more than one SDI-12 sensor, you must ensure that
 * each sensor has a different address. To find or change the SDI-12 address of
 * your sensor, load and run the
 * [sdi12_address_change](https://github.com/EnviroDIY/ModularSensors/blob/master/extras/sdi12_address_change/sdi12_address_change.ino)
 * program from the
 * [extras](https://github.com/EnviroDIY/ModularSensors/tree/master/extras)
 * directory or the
 * [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change)
 * example within the SDI-12 library.
 *
 * Keep in mind that SDI12 is a slow communication protocol (only 1200 baud) and
 * _ALL interrupts are turned off during communication_. This means that if you
 * have any interrupt driven sensors (like a tipping bucket) attached with an
 * SDI12 sensor, no interrupts (or tips) will be registered during SDI12
 * communication.
 *
 * @section sdi12_group_flags Build flags
 * - `-D MS_SDI12_NON_CONCURRENT`
 *    - Instructs *all* SDI-12 sensors to take non-concurrent measurements
 *    - After starting a measurement on an SDI-12 sensor, the processor will
 * stop everything, waiting for that measurement to complete before moving on
 * to the next sensor.
 *    - This may be necessary if your sensor uses a version of the SDI-12
 * protocol prior to 1.2 or if your sensor is not properly compliant with the
 * protocol.
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

#ifdef MS_SDI12SENSORS_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "SDI12Sensors"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
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
     * @brief Construct a new SDI 12 Sensors object.  This is only intended to
     * be used within this library.
     *
     * @param SDI12address The SDI-12 address of the sensor; can be a char,
     * char*, or int.
     * @param powerPin The pin on the mcu controlling power to the sensor
     * Use -1 if it is continuously powered.
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param sensorName The name of the sensor.  Defaults to "SDI12-Sensor".
     * @param totalReturnedValues The number of variable results returned by the
     * sensor.  Defaults to 1.
     * @param warmUpTime_ms The time in ms between when the sensor is powered on
     * and when it is ready to receive a wake command.  Defaults to 0.
     * @param stabilizationTime_ms The time in ms between when the sensor
     * receives a wake command and when it is able to return stable values.
     * Defaults to 0.
     * @param measurementTime_ms The time in ms between when a measurement is
     * started and when the result value is available.  Defaults to 0.
     * @param extraWakeTime Any extra time needed by the sensor between the
     * start of the break and when the sensor is ready to accept commands.  This
     * will be between 0 and 100ms per SDI-12 protocol.
     * @param incCalcValues The number of included calculated variables from the
     * sensor, if any.  These are used for values that we would always calculate
     * for a sensor and depend only on the raw results of that single sensor;
     * optional with a default value of 0.
     */
    SDI12Sensors(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t totalReturnedValues   = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0,
                 uint32_t measurementTime_ms = 0, int8_t extraWakeTime = 0,
                 uint8_t incCalcValues = 0);
    /**
     * @copydoc SDI12Sensors::SDI12Sensors
     */
    SDI12Sensors(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t totalReturnedValues   = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0,
                 uint32_t measurementTime_ms = 0, int8_t extraWakeTime = 0,
                 uint8_t incCalcValues = 0);
    /**
     * @copydoc SDI12Sensors::SDI12Sensors
     */
    SDI12Sensors(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t       measurementsToAverage = 1,
                 const char*   sensorName            = "SDI12-Sensor",
                 const uint8_t totalReturnedValues   = 1,
                 uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0,
                 uint32_t measurementTime_ms = 0, int8_t extraWakeTime = 0,
                 uint8_t incCalcValues = 0);
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

// Only need this for concurrent measurements.
// NOTE:  By default, concurrent measurements are used!
#ifndef MS_SDI12_NON_CONCURRENT
    /**
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the #_millisMeasurementRequested timestamp.
     *
     * @note This function does NOT include any waiting for the sensor to be
     * warmed up or stable!
     *
     * @return **bool** True if the start measurement function completed
     * successfully.
     */
    bool startSingleMeasurement(void) override;
#endif
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 protected:
    /**
     * @brief Send the SDI-12 'acknowledge active' command [address][!] to a
     * sensor and confirm that the correct sensor responded.
     *
     * @return **bool** True if the correct SDI-12 sensor replied to the
     * command.
     */
    bool requestSensorAcknowledgement(void);
    /**
     * @brief Send the SDI-12 'info' command [address][I][!] to a sensor and
     * parse the result into the vendor, model, version, and serial number.
     *
     * @return **bool** True if all expected information fields returned by the
     * sensor.
     */
    bool getSensorInfo(void);
    /**
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the #_millisMeasurementRequested timestamp.
     *
     * @note This function does NOT include any waiting for the sensor to be
     * warmed up or stable!
     *
     * @param isConcurrent Whether to start a concurrent or standard
     * measurement.  Defaults to 'true' for a concurrent measurement.
     *
     * @return **int8_t** The length of time the measurement is expected to
     * take.
     */
    int8_t startSDI12Measurement(bool isConcurrent = true);
    /**
     * @brief Gets the results of either a standard or a concurrent measurement
     *
     * @return **bool** True if the full number of expected results was
     * returned.
     */
    virtual bool getResults(void);
    /**
     * @brief Internal reference to the SDI-12 object.
     */
    SDI12 _SDI12Internal;
    /**
     * @brief Internal reference to the SDI-12 address.
     */
    char _SDI12address;
    /**
     * @brief Extra wake time required for an SDI-12 sensor between the "break"
     * and the time the command is sent.
     */
    int8_t _extraWakeTime;

 private:
    String _sensorVendor;
    String _sensorModel;
    String _sensorVersion;
    String _sensorSerialNumber;
};

#endif  // SRC_SENSORS_SDI12SENSORS_H_
