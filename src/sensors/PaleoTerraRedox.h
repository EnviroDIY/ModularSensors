/**
 * @file PaleoTerraRedox.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Anthony Aufdenkampe <aaufdenkampe@limno.com> with help from Beth
 * Fisher, Evan Host and Bobby Schulz.
 * Heavliy edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the PaleoTerraRedox semsor subclass and the variable subclass
 * PaleoTerraRedox_Volt.
 *
 * These are for the PaleoTerra redox sensors.
 *
 * This depends on Testato's
 * [SoftwareWire](https://github.com/Testato/SoftwareWire) library if software
 * I2C is needed.
 */
/* clang-format off */
/**
 * @defgroup pt_redox_group PaleoTerra Redox
 * Classes for the PaleoTerra Redox digital preassure sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section pt_redox_intro Introduction
 *
 * Because older versions of these sensors all ship with the same I2C address,
 * and more than one is frequently used at different soil depths in the same profile,
 * this module has an optional dependence on Testato's
 * [SoftwareWire](https://github.com/Testato/SoftwareWire) library for software
 * I2C.
 *
 * @section pt_redox_datasheet Sensor Datasheet
 * Documentation for the sensor can be found at: https://paleoterra.nl/
 *
 * @section pt_redox_sensor The PaleoTerra Redox Sensor
 * @ctor_doc{PaleoTerraRedox, int8_t powerPin, uint8_t i2cAddressHex, int16_t maxPressure, uint8_t measurementsToAverage}
 * @subsection pt_redox_timing Sensor Timing
 * - Sensor takes about 0.5 / 1.1 / 2.1 / 4.1 / 8.22 ms to respond
 * at oversampling ratios: 256 / 512 / 1024 / 2048 / 4096, respectively.
 * - We assume the sensor is immediately stable.
 * @subsection pt_redox_flags Build flags
 * - `-D MS_PALEOTERRA_SOFTWAREWIRE`
 *      - switches from using hardware I2C to software I2C
 * @warning Either all or none your attached redox may use software I2C.
 * Using some with software I2C and others with hardware I2C is not supported.
 *
 * @section pt_redox_volt Voltate Output
 *   - Range is -40°C to +85°C
 *   - Accuracy is ±0.8°C
 *   - Result stored in sensorValues[0]
 *   - Resolution is <0.01°C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is PTRVoltage
 *
 * @variabledoc{pt_redox_volt,PaleoTerraRedox,Volt,PTRVoltage}
 *
 * ___
 * @section pt_redox_examples Example Code
 * The PaleoTerra Redox is used in the @menulink{pt_redox} example.
 *
 * @menusnip{pt_redox}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_PALEOTERRAREDOX_H_
#define SRC_SENSORS_PALEOTERRAREDOX_H_

// Debugging Statement
// #define MS_PALEOTERRAREDOX_DEBUG

#ifdef MS_PALEOTERRAREDOX_DEBUG
#define MS_DEBUGGING_STD "PaleoTerraRedox"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

#if defined MS_PALEOTERRA_SOFTWAREWIRE
#include <SoftwareWire.h>  // Testato's SoftwareWire
#endif

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the PaleoTerra redox sensor can report 1 value.
#define PTR_NUM_VARIABLES 1
/**
 * @brief Sensor::_warmUpTime_ms; the PaleoTerra redox sensor is
 * immediately warmed up.
 */
#define PTR_WARM_UP_TIME_MS 1
/**
 * @brief Sensor::_stabilizationTime_ms; the PaleoTerra redox sensor is
 * immediately stable.
 */
#define PTR_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the PaleoTerra redox sensor takes 67ms to
 * complete a measurement.
 */
#define PTR_MEASUREMENT_TIME_MS 67

/// Decimals places in string representation; voltage should have 2.
#define PTR_VOLT_RESOLUTION 2
/// Variable number; voltage is stored in sensorValues[0].
#define PTR_VOLT_VAR_NUM 0

/// The default I2C address of the PaleoTerra redox sensor
#define MCP3421_ADR 0x68

// The main class for the PaleoTerra Redox Sensor
/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [PaleoTerra redox sensor](@ref pt_redox_group).
 *
 * @ingroup pt_redox_group
 */
/* clang-format on */
class PaleoTerraRedox : public Sensor {
 public:
#if defined MS_PALEOTERRA_SOFTWAREWIRE
    /**
     * @brief Construct a new PaleoTerra Redox object using a *software* I2C
     * instance.
     *
     * The constructor - need the power pin, optionally can give an instance of
     * TwoWire for I2C communbication, an address, and  a number of measurements
     * to average.
     *
     * @param theI2C A [SoftwareWire](https://github.com/Testato/SoftwareWire)
     * instance for I2C communication.
     * @param powerPin The pin on the mcu controlling power to the PaleoTerra
     * redox sensor.  Use -1 if it is continuously powered.
     * @param i2cAddressHex The I2C address of the redox probe.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    PaleoTerraRedox(SoftwareWire* theI2C, int8_t powerPin,
                    uint8_t i2cAddressHex         = MCP3421_ADR,
                    uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new PaleoTerra Redox object, also creating a
     * [SoftwareWire](https://github.com/Testato/SoftwareWire) I2C instance for
     * communication with that object.
     *
     * @note Unless there are address conflicts between I2C devices, you should
     * not create a new I2C instance.
     *
     * @param powerPin The pin on the mcu controlling power to the PaleoTerra
     * redox sensor.  Use -1 if it is continuously powered.
     * @param dataPin The pin on the mcu that will be used for I2C data (SDA).
     * Must be a valid pin number.
     * @param clockPin The pin on the mcu that will be used for the I2C clock
     * (SCL).  Must be a valid pin number.
     * @param i2cAddressHex The I2C address of the redox probe.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    PaleoTerraRedox(int8_t powerPin, int8_t dataPin, int8_t clockPin,
                    uint8_t i2cAddressHex         = MCP3421_ADR,
                    uint8_t measurementsToAverage = 1);
#else
    /**
     * @brief Construct a new PaleoTerra Redox object using a secondary
     * *hardware* I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling power to the PaleoTerra
     * redox sensor.  Use -1 if it is continuously powered.
     * @param i2cAddressHex The I2C address of the redox probe.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    PaleoTerraRedox(TwoWire* theI2C, int8_t powerPin,
                    uint8_t i2cAddressHex         = MCP3421_ADR,
                    uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new PaleoTerra Redox object using the primary hardware
     * I2C instance.
     *
     * @param powerPin The pin on the mcu controlling power to the PaleoTerra
     * redox sensor.  Use -1 if it is continuously powered.
     * @param i2cAddressHex The I2C address of the redox probe.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit PaleoTerraRedox(int8_t  powerPin,
                             uint8_t i2cAddressHex         = MCP3421_ADR,
                             uint8_t measurementsToAverage = 1);
#endif
    /**
     * @brief Destroy the PaleoTerra Redox object.  Also destroy the software
     * I2C instance if one was created.
     */
    ~PaleoTerraRedox();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin levels and modes for I2C) and
     * updates the #_sensorStatus.  No sensor power is required.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;
    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief The I2C address of the redox sensor.
     */
    uint8_t _i2cAddressHex;
#if defined MS_PALEOTERRA_SOFTWAREWIRE
    /**
     * @brief An internal reference to the SoftwareWire instance.
     */
    SoftwareWire* _i2c;  // Software Wire
    /**
     * @brief A flag denoting whether a new SoftwareWire instance was created.
     * If it was created, it must be destroyed in the destructor to avoid a
     * memory leak.
     */
    bool createdSoftwareWire;
#else
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;  // Hardware Wire
#endif
};


// Defines the Redox Potential Variable
/**
 * @brief The Variable sub-class used for the
 * [voltage output](@ref pt_redox_volt) from a PaleoTerra redox sensor.
 *
 * @ingroup pt_redox_group
 */
class PaleoTerraRedox_Volt : public Variable {
 public:
    /**
     * @brief Construct a new PaleoTerraRedox_Volt object.
     *
     * @param parentSense The parent PaleoTerraRedox providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "PTRVoltage".
     */
    explicit PaleoTerraRedox_Volt(Sensor* parentSense, const char* uuid = "",
                                  const char* varCode = "PTRVoltage")
        : Variable(parentSense, (const uint8_t)PTR_VOLT_VAR_NUM,
                   (uint8_t)PTR_VOLT_RESOLUTION, "Voltage", "mV", varCode,
                   uuid) {}
    /**
     * @brief Construct a new PaleoTerraRedox_Volt object.
     *
     * @note This must be tied with a parent PaleoTerraRedox before it can be
     * used.
     */
    PaleoTerraRedox_Volt()
        : Variable((const uint8_t)PTR_VOLT_VAR_NUM,
                   (uint8_t)PTR_VOLT_RESOLUTION, "Voltage", "mV",
                   "PTRVoltage") {}
    /**
     * @brief Destroy the PaleoTerraRedox_Volt object - no action needed.
     */
};
#endif  // SRC_SENSORS_PALEOTERRAREDOX_H_
