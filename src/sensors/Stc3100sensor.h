/**
 * @file Stc3100sensor.h
 * @copyright 2020 Neil Hancock, assigned to the Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Neil Hancock.  Based on the AtlasParent.xx of the Stroud Water Research Center
 * 
 *
 * @brief Contains the Stc3100sensor sensor subclass which is itself the
 * parent class for all Stc3100 sensors.
 *
 * This depends on the Arduino core Wire library.  
 *
 */
/* clang-format off */
/**
 * @defgroup stc3100_group Stc3100 Circuit
 * The Sensor and Variable objects for Stc3100 circuits.
 *
 * @ingroup the_sensors
 *
 * This library currently supports the Stc3100 IC as a sensors
 *
 *
 * The chips have operating voltages between 3.3V and 5V; power can be stopped
 * between measurements.  
 *
 * The code in ModularSensors _**requires the Stc3100 Sensors to communicate over
 * I2C**_.  
 *
 * @section stc3100_group_flags Build flags
 *
 * - `-D MS_STC3100SENSOR_DEBUG`
 *      - switches on debug
 *
 * @warning **The STC3100 as a battery monitor is expected to be powered at all times.
 * It can be put into low power mode** 
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_STC3100SENSOR_H_
#define SRC_SENSORS_STC3100SENSOR_H_

// Debugging Statement
// #define MS_STC3100SENSOR_DEBUG

#ifdef MS_STC3100SENSOR_DEBUG
#define MS_DEBUGGING_STD "Stc3100sensor"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>
#include "STC3100dd.h"

// Sensor Specific Defines
/** @ingroup sensor_stc3100 */
/**@{*/

/// @brief Sensor::_numReturnedValues; the STC3100 can report 3 values at present - possibly more in future
#define STC3100_NUM_VARIABLES 3

/**
 * @anchor sensor_ina219_timing
 * @name Sensor Timing
 * The sensor timing for a ST STC3100
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the STC3100 warms up in 100ms.
#define STC3100_WARM_UP_TIME_MS 100
/**
 * @brief Sensor::_stabilizationTime_ms; the STC3100 is stable after 4000ms.
 *
 * Stable numbers can be acheived after 500ms, but waiting up to 4s gave more
 * consistent numbers based on tests using STC3100timingTest.ino
 */
#define STC3100_STABILIZATION_TIME_MS 4000
/**
 * @brief Sensor::_measurementTime_ms; the STC3100 takes 1100ms to complete a
 * measurement.
 *
 * A single ADC conversion takes >532 µs (586 µs typical) at 12 bit resolution,
 * but in tests waiting closer to 1.1s gave data with a slightly better standard
 * deviation.
 */
#define STC3100_MEASUREMENT_TIME_MS 1100
/**@}*/

#warning more work  * Range is dependent on exernal R
/**
 * @anchor sensor_ina219_current
 * @name Current
 * The current variable from a ST STC3100

 * - Range is between +/-0.4 Amps and +/-3.2 Amps
 * - Absolute accuracy is range dependent, and approx 2LSB (R accuracy
 * unknown)
 *
 * {{ @ref TISTC3100M_Current::TISTC3100M_Current }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; current should have 1.
 *  - resolution is 12-bit
 *     - 0.8mA using +/-3.2 Amp range
 *     - 0.1mA using +/-0.4 Amp range
 */
#define STC3100_CURRENT_MA_RESOLUTION 1
/// @brief Sensor variable number; current is stored in sensorValues[0].
#define STC3100_CURRENT_MA_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricCurrent"
#define STC3100_CURRENT_MA_VAR_NAME "electricCurrent"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "milliamp"
#define STC3100_CURRENT_MA_UNIT_NAME "milliamp"
/// @brief Default variable short code; "STSTC3100Amp"
#define STC3100_CURRENT_MA_DEFAULT_CODE "STSTC3100Amp"
/**@}*/

/**
 * @anchor sensor_ina219_volt
 * @name Bus Voltage
 * The bus voltage variable from a ST STC3100
 * - Range is 0 to 26V
 * - Accuracy is ±4mV (1 LSB step size)
 *
 * {{ @ref STSTC3100_Volt::STSTC3100_Volt }}
 */
/**@{*/
/// @brief Decimals places in string representation; bus voltage should have 4 -
/// resolution is 0.001V.
#define STC3100_BUS_VOLTAGE_RESOLUTION 3
/// @brief Sensor variable number; bus voltage is stored in sensorValues[1].
#define STC3100_BUS_VOLTAGE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define STC3100_BUS_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define STC3100_BUS_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "STSTC3100Volt"
#define STC3100_BUS_VOLTAGE_DEFAULT_CODE "STSTC3100Volt"
/**@}*/

/**
 * @anchor sensor_stc3100_energy
 * @name energy_mAh
 * The energy variable from a ST STC3100
 *
 * {{ @ref STC3100M_mAh::STC3100M_mAh }}
 */
/**@{*/
/// @brief Decimals places in string representation; power draw should have 2 -
/// resolution is 0.01mW.
#define STC3100_ENERGY_MAH_RESOLUTION 2
/// @brief Sensor variable number; power draw is stored in sensorValues[2].
#define STC3100_ENERGY_MAH_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricPower"
#define STC3100_ENERGY_MAH_VAR_NAME "electricEnergy"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "milliwatt"
#define STC3100_ENERGY_MAH_UNIT_NAME "milliAmpHour"
/// @brief Default variable short code; "STSTC3100Power"
#define STC3100_ENERGY_MAH_DEFAULT_CODE "STSTC3100Energy"
/**@}*/

/**
 * @brief Class for Stc3100 Device with internal sensors
 *
 * This contains the main I2C functionality for all Stc3100 EZO circuits.
 *
 * @ingroup stc3100_group
 */
class Stc3100sensor : public Sensor {
 public:

#if 1//!defined(MS_ATLAS_SOFTWAREWIRE) | defined DOXYGEN
    /**
     * @brief Construct a new Stc3100 Parent object using a secondary *hardware*
     * I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param sensorName The name of the sensor, defaults to Stc3100Sensor.
     * @param numReturnedVars The number of results returned by the sensor.
     * Defaults to 1.
     * @param warmUpTime_ms The time needed from the when a sensor has power
     * until it's ready to talk (_warmUpTime_ms).
     * @param stabilizationTime_ms The time needed from the when a sensor is
     * activated until the readings are stable (_stabilizationTime_ms).
     * @param measurementTime_ms The time needed from the when a sensor is told
     * to take a single reading until that reading is expected to be complete
     * (_measurementTime_ms)
     */
    Stc3100sensor(TwoWire* theI2C, 
                uint8_t       measurementsToAverage = 1,
                const char*   sensorName            = "Stc3100Sensor",
                const uint8_t numReturnedVars = 3, uint32_t warmUpTime_ms = 0,
                uint32_t stabilizationTime_ms = 0,
                uint32_t measurementTime_ms   = 0);
    /**
     * @brief Construct a new Stc3100 Parent object using the primary hardware I2C
     * instance.
     *
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param sensorName The name of the sensor, defaults to Stc3100Sensor.
     * @param numReturnedVars The number of results returned by the sensor.
     * Defaults to 1.
     * @param warmUpTime_ms The time needed from the when a sensor has power
     * until it's ready to talk (_warmUpTime_ms).
     * @param stabilizationTime_ms The time needed from the when a sensor is
     * activated until the readings are stable (_stabilizationTime_ms).
     * @param measurementTime_ms The time needed from the when a sensor is told
     * to take a single reading until that reading is expected to be complete
     * (_measurementTime_ms)
     */
    Stc3100sensor( 
                uint8_t       measurementsToAverage = 1,
                const char*   sensorName            = "Stc3100Sensor",
                const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
                uint32_t stabilizationTime_ms = 0,
                uint32_t measurementTime_ms   = 0);
#endif
    /**
     * @brief Destroy the Stc3100 Parent object.  Also destroy the software I2C
     * instance if one was created.
     */
    virtual ~Stc3100sensor();

    /**
     * @brief Return the I2C address of the EZO circuit.
     *
     * @return **String** Text describing how the sensor is attached to the mcu.
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This  begins the Wire library (sets pin levels
     * and modes for I2C), and updates the #_sensorStatus.  No sensor power is
     * required.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;

    // NOTE:  The sensor should wake as soon as any command is sent.
    // I assume that means we can use the command to take a reading to both
    // wake it and ask for a reading.
    // bool wake(void) override;

    /**
     * @brief Puts the sensor to sleep, if necessary.
     *
     * This also un-sets the #_millisSensorActivated timestamp (sets it to 0).
     * This does NOT power down the sensor!
     *
     * @return **bool** True if the sleep function completed successfully.
     */
    bool sleep(void) override;

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
    //bool startSingleMeasurement(void) override;
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

    /**
     * @brief  setI2cAddress- call at beginning if need to change 
     * There only appears to be one IC that is commercially available,
     * but a number of IC addresses are defined in the manual
     * 
     */
    #if defined STC3100_USE_ADDR
        void setI2cAddress(int8_t i2cAddressHex=STC3100_BUS_ADDRESS) {_i2cAddressHex=i2cAddressHex;}
    #endif // STC3100_USE_ADDR

 protected:
    /**
     * @brief The I2C address of the Stc3100 circuit.
     */
    #if !defined STC3100_USE_ADDR
    const 
    #endif //STC3100_USE_ADDR
    int8_t _i2cAddressHex=STC3100_BUS_ADDRESS;

    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;  // Hardware Wire

    /**
     * @brief Wait for a command to process
     *
     * Most Stc3100 I2C commands have a 300ms processing time from the time the
     * command is written until it is possible to request a response or result,
     * except for the commands to take a calibration point or a reading which
     * have a 600ms processing/response time.
     *
     * @note This should ONLY be used as a wait when no response is expected
     * except a status code - the response will be "consumed" and become
     * unavailable.
     *
     * @param timeout The maximum amout of time to wait in ms.
     * @return **bool** True processing completed and a status code was returned
     * within the wait period.
     */
    bool waitForProcessing(uint32_t timeout = 1000L);

    STC3100dd stc3100_device;
};

#endif  // SRC_SENSORS_STC3100SENSOR_H_
