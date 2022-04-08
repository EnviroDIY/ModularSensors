/**
 * @file AtlasParent.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Initial developement for Atlas Sensors was done by Adam Gold
 * Files were edited by Sara Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AtlasParent sensor subclass which is itself the
 * parent class for all Atlas sensors.
 *
 * This depends on the Arduino core Wire library.  It does *not* use the Atlas
 * Arduino library.
 */
/* clang-format off */
/**
 * @defgroup atlas_group Atlas Scientific EZO Circuits
 * The Sensor and Variable objects for all Atlas EZO circuits.
 *
 * @ingroup the_sensors
 *
 * This library currently supports the following Atlas Scientific sensors:
 *
 * - [EZO-CO2 Embedded NDIR CO2 Sensor](https://www.atlas-scientific.com/probes/ezo-co2-carbon-dioxide-sensor/)
 *     - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EZO_CO2_Datasheet.pdf)
 *     - [Class Documentation](@ref sensor_atlas_co2)
 * - [EZO-DO Dissolved Oxygen Circuit and Probe](https://www.atlas-scientific.com/dissolved-oxygen.html)
 *     - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_DO_EZO_Datasheet.pdf)
 *     - [Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_DO_probe.pdf)
 *     - [Class Documentation](@ref sensor_atlas_do)
 * - [EZO-EC Conductivity Circuit and Probes](https://www.atlas-scientific.com/conductivity.html)
 *     - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_EZO_Datasheet.pdf)
 *     - [K0.1 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_0.1_probe.pdf)
 *     - [K1.0 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_1.0_probe.pdf)
 *     - [K10 Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EC_K_10_probe.pdf)
 *     - [Class Documentation](@ref sensor_atlas_cond)
 * - [EZO-ORP Oxidation/Reduction Potential Circuit and Probes](https://www.atlas-scientific.com/orp.html)
 *     - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_ORP_EZO_datasheet.pdf)
 *     - [Class Documentation](@ref sensor_atlas_orp)
 * - [EZO-pH Circuit and Probe](https://www.atlas-scientific.com/ph.html)
 *     - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_pH_EZO_Datasheet.pdf)
 *     - [Class Documentation](@ref sensor_atlas_ph)
 * - [EZO-RTD Temperature Circuit and Probes](https://www.atlas-scientific.com/temperature.html)
 *     - [Circuit Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_EZO_RTD_Datasheet.pdf)
 *     - [Probe Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/AtlasScientific_PT-1000-probe.pdf)
 *     - [Class Documentation](@ref sensor_atlas_rtd)
 *
 * The chips have operating voltages between 3.3V and 5V; power can be stopped
 * between measurements.  The probes and sensors can (and should) be calibrated
 * using sketches provided by Atlas Scientific on their website.  Atlas
 * Scientific recommends that you integrate their sensor chips into a board, so
 * you can buy one from them or make your own (example:
 * https://github.com/acgold/Atlas-Scientific-Carrier-Board).
 *
 * The code in ModularSensors _**requires the Atlas Sensors to communicate over
 * I2C**_.  Atlas scientific sensors are shipped with probes and sensor chips
 * that communicate using UART by default.  Data sheets, found on the Atlas
 * Scientific website, show how to manually switch the chips to I2C.  Before
 * deploying your Atlas chip chip and sensor, I recommend locking the protocol
 * (plock) to I2C so the sensors do not accidentally switch back to UART mode.
 * Legacy chips and EZO chips that do not support I2C are not supported.
 *
 * @warning **You must isolate the data lines of all Atlas circuits from the
 * main I2C bus if you wish to turn off their power!**  If you do not isolate
 * them from your main I2C bus and you turn off power to the circuits between
 * measurements the I2C lines will be pulled down to ground causing the I2C bus
 * (and thus your logger) to crash.
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_ATLASPARENT_H_
#define SRC_SENSORS_ATLASPARENT_H_

// Debugging Statement
// #define MS_ATLASPARENT_DEBUG

#ifdef MS_ATLASPARENT_DEBUG
#define MS_DEBUGGING_STD "AtlasParent"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

/**
 * @brief A parent class for Atlas EZO circuits and sensors
 *
 * This contains the main I2C functionality for all Atlas EZO circuits.
 *
 * @ingroup atlas_group
 */
class AtlasParent : public Sensor {
 public:
    /**
     * @brief Construct a new Atlas Parent object using a secondary *hardware*
     * I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling power to the Atlas
     * circuit.  Use -1 if it is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param sensorName The name of the sensor, defaults to AtlasSensor.
     * @param totalReturnedValues The number of results returned by the sensor.
     * Defaults to 1.
     * @param warmUpTime_ms The time needed from the when a sensor has power
     * until it's ready to talk (_warmUpTime_ms).
     * @param stabilizationTime_ms The time needed from the when a sensor is
     * activated until the readings are stable (_stabilizationTime_ms).
     * @param measurementTime_ms The time needed from the when a sensor is told
     * to take a single reading until that reading is expected to be complete
     * (_measurementTime_ms)
     * @param incCalcValues The number of included calculated variables from the
     * sensor, if any.  These are used for values that we would always calculate
     * for a sensor and depend only on the raw results of that single sensor;
     * optional with a default value of 0.
     */
    AtlasParent(TwoWire* theI2C, int8_t powerPin, uint8_t i2cAddressHex,
                uint8_t       measurementsToAverage = 1,
                const char*   sensorName            = "AtlasSensor",
                const uint8_t totalReturnedValues   = 1,
                uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0,
                uint32_t measurementTime_ms = 0, uint8_t incCalcValues = 0);
    /**
     * @brief Construct a new Atlas Parent object using the primary hardware I2C
     * instance.
     *
     * @param powerPin The pin on the mcu controlling power to the Atlas
     * circuit.  Use -1 if it is continuously powered.
     * @param i2cAddressHex The I2C address of the Atlas circuit
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param sensorName The name of the sensor, defaults to AtlasSensor.
     * @param totalReturnedValues The number of results returned by the sensor.
     * Defaults to 1.
     * @param warmUpTime_ms The time needed from the when a sensor has power
     * until it's ready to talk (_warmUpTime_ms).
     * @param stabilizationTime_ms The time needed from the when a sensor is
     * activated until the readings are stable (_stabilizationTime_ms).
     * @param measurementTime_ms The time needed from the when a sensor is told
     * to take a single reading until that reading is expected to be complete
     * (_measurementTime_ms)
     * @param incCalcValues The number of included calculated variables from the
     * sensor, if any.  These are used for values that we would always calculate
     * for a sensor and depend only on the raw results of that single sensor;
     * optional with a default value of 0.
     */
    AtlasParent(int8_t powerPin, uint8_t i2cAddressHex,
                uint8_t       measurementsToAverage = 1,
                const char*   sensorName            = "AtlasSensor",
                const uint8_t totalReturnedValues   = 1,
                uint32_t warmUpTime_ms = 0, uint32_t stabilizationTime_ms = 0,
                uint32_t measurementTime_ms = 0, uint8_t incCalcValues = 0);

    /**
     * @brief Destroy the Atlas Parent object.  Also destroy the software I2C
     * instance if one was created.
     */
    virtual ~AtlasParent();

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
     * This sets the #_powerPin mode, begins the Wire library (sets pin levels
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
    bool startSingleMeasurement(void) override;
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 protected:
    /**
     * @brief The I2C address of the Atlas circuit.
     */
    int8_t _i2cAddressHex;
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;  // Hardware Wire

    /**
     * @brief Wait for a command to process
     *
     * Most Atlas I2C commands have a 300ms processing time from the time the
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
};

#endif  // SRC_SENSORS_ATLASPARENT_H_
