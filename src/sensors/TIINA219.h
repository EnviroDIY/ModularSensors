/**
 * @file TIINA219.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Neil Hancock
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the TIINA219 sensor subclass and the variale subclasses
 * TIINA219_Current, TIINA219_Voltage, and TIINA219_Power.
 *
 * These are for the Texas Instruments INA219 current/voltage sensor.
 *
 * This depends on the
 * [Adafruit INA219 Library](https://github.com/adafruit/Adafruit_INA219)
 */
/* clang-format off */
/**
 * @defgroup sensor_ina219 TI INA219
 * Classes for the TI INA219 current and voltage sensor.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_ina219_intro Intruduction
 *
 * The [TI INA219](http://www.ti.com/product/INA219) is a bi-directional,
 * high-side, current/power monitor that communicates with the board via I2C.
 * The I2C address of INA219 can be any number between 0x40 and 0x4F, depending
 * on the settings of pins A0 and A1.  See the datasheet for the possible
 * configurations.  The INA219 requires an input voltage of 3.0-5.5V.  The gain
 * of this sensor can be increased to increase sensitivity (at the expense of
 * range) but this library assumes the maximum range.
 *
 * Commuincation between the INA219 and the mcu is managed by the
 * [Adafruit INA219 Library](https://github.com/adafruit/Adafruit_INA219)
 *
 * @note Software I2C is *not* supported for the INA219.
 * A secondary hardware I2C on a SAMD board is supported.
 *
 * @section sensor_ina219_datasheet Sensor Datasheet
 *
 * Documentation for the sensor can be found at:
 * https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout and
 * http://www.ti.com/product/INA219
 *
 * @section sensor_ina219_ctor Sensor Constructor
 * {{ @ref TIINA219::TIINA219(int8_t, uint8_t, uint8_t) }}
 * {{ @ref TIINA219::TIINA219(TwoWire*, int8_t, uint8_t, uint8_t) }}
 *
 * ___
 * @section sensor_ina219_examples Example Code
 * The TI INA219 is used in the @menulink{ti_ina219} example.
 *
 * @menusnip{ti_ina219}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_TIINA219_H_
#define SRC_SENSORS_TIINA219_H_

// Debugging Statement
// #define MS_TIINA219_DEBUG

#ifdef MS_TIINA219_DEBUG
#define MS_DEBUGGING_STD "TIINA219"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_INA219.h>

/** @ingroup sensor_ina219 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the INA219 can report 3 values.
#define INA219_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define INA219_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_ina219_timing
 * @name Sensor Timing
 * The sensor timing for a TI INA219
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the INA219 warms up in 100ms.
#define INA219_WARM_UP_TIME_MS 100
/**
 * @brief Sensor::_stabilizationTime_ms; the INA219 is stable after 4000ms.
 *
 * Stable numbers can be acheived after 500ms, but waiting up to 4s gave more
 * consistent numbers based on tests using INA219timingTest.ino
 */
#define INA219_STABILIZATION_TIME_MS 4000
/**
 * @brief Sensor::_measurementTime_ms; the INA219 takes 1100ms to complete a
 * measurement.
 *
 * A single ADC conversion takes >532 µs (586 µs typical) at 12 bit resolution,
 * but in tests waiting closer to 1.1s gave data with a slightly better standard
 * deviation.
 */
#define INA219_MEASUREMENT_TIME_MS 1100
/**@}*/

/**
 * @anchor sensor_ina219_current
 * @name Current
 * The current variable from a TI INA219
 * - Range is between +/-0.4 Amps and +/-3.2 Amps
 * - Absolute accuracy is range dependent, and approx 2LSB (R accuracy
 * unknown)
 *
 * {{ @ref TIINA219_Current::TIINA219_Current }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; current should have 1.
 *  - resolution is 12-bit
 *     - 0.8mA using +/-3.2 Amp range
 *     - 0.1mA using +/-0.4 Amp range
 */
#define INA219_CURRENT_MA_RESOLUTION 1
/// @brief Sensor variable number; current is stored in sensorValues[0].
#define INA219_CURRENT_MA_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricCurrent"
#define INA219_CURRENT_MA_VAR_NAME "electricCurrent"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "milliamp"
#define INA219_CURRENT_MA_UNIT_NAME "milliamp"
/// @brief Default variable short code; "TIINA219Amp"
#define INA219_CURRENT_MA_DEFAULT_CODE "TIINA219Amp"
/**@}*/

/**
 * @anchor sensor_ina219_volt
 * @name Bus Voltage
 * The bus voltage variable from a TI INA219
 * - Range is 0 to 26V
 * - Accuracy is ±4mV (1 LSB step size)
 *
 * {{ @ref TIINA219_Voltage::TIINA219_Voltage }}
 */
/**@{*/
/// @brief Decimals places in string representation; bus voltage should have 4 -
/// resolution is 0.001V.
#define INA219_BUS_VOLTAGE_RESOLUTION 3
/// @brief Sensor variable number; bus voltage is stored in sensorValues[1].
#define INA219_BUS_VOLTAGE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define INA219_BUS_VOLTAGE_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define INA219_BUS_VOLTAGE_UNIT_NAME "volt"
/// @brief Default variable short code; "TIINA219Volt"
#define INA219_BUS_VOLTAGE_DEFAULT_CODE "TIINA219Volt"
/**@}*/

/**
 * @anchor sensor_ina219_power
 * @name Power
 * The power variable from a TI INA219
 *
 * {{ @ref TIINA219_Power::TIINA219_Power }}
 */
/**@{*/
/// @brief Decimals places in string representation; power draw should have 2 -
/// resolution is 0.01mW.
#define INA219_POWER_MW_RESOLUTION 2
/// @brief Sensor variable number; power draw is stored in sensorValues[2].
#define INA219_POWER_MW_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "electricPower"
#define INA219_POWER_MW_VAR_NAME "electricPower"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "milliwatt"
#define INA219_POWER_MW_UNIT_NAME "milliwatt"
/// @brief Default variable short code; "TIINA219Power"
#define INA219_POWER_MW_DEFAULT_CODE "TIINA219Power"
/**@}*/

/// @brief The default address of the INA219
#define INA219_ADDRESS_BASE 0x40

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [TexasInstruments INA219 sensor](@ref sensor_ina219).
 *
 * @ingroup sensor_ina219
 */
/* clang-format on */
class TIINA219 : public Sensor {
 public:
    /**
     * @brief Construct a new TI INA219 object using a secondary *hardware* I2C
     * instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling power to the INA219.
     * Use -1 if it is continuously powered.
     * - The INA219 requires input voltage of 3.0-5.5V, which can be turned off
     * between measurements.
     * @param i2cAddressHex The I2C address of the BME280; can be any number
     * between 0x40 and 0x4F.  The default value is 0x40.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    TIINA219(TwoWire* theI2C, int8_t powerPin,
             uint8_t i2cAddressHex         = INA219_ADDRESS_BASE,
             uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new TI INA219 object using the default hardware I2C
     * instance.
     *
     * @param powerPin The pin on the mcu controlling power to the INA219.
     * Use -1 if it is continuously powered.
     * - The INA219 requires input voltage of 3.0-5.5V, which can be turned off
     * between measurements.
     * @param i2cAddressHex The I2C address of the BME280; can be any number
     * between 0x40 and 0x4F.  The default value is 0x40.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit TIINA219(int8_t  powerPin,
                      uint8_t i2cAddressHex         = INA219_ADDRESS_BASE,
                      uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the TI INA219 object
     */
    ~TIINA219();

    /**
     * @brief Wake the sensor up and read the calibration coefficient from it.
     *
     * Verifies that the power is on and updates the #_sensorStatus.  This also
     * sets the #_millisSensorActivated timestamp.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return **bool** True if the wake function completed successfully.
     */
    bool wake(void) override;
    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the #_powerPin mode, begins the Wire library (sets pin levels
     * and modes for I2C).  This also sets the calibration range of the INA219,
     * and updates the #_sensorStatus.  The INA219 must be powered for setup.
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
     * @brief Private reference to the internal INA219 object.
     */
    Adafruit_INA219 ina219_phy;
    /**
     * @brief The I2C address of the INA219.
     */
    uint8_t _i2cAddressHex;  // Hardware slave address
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [current output](@ref sensor_ina219_current) from a [TI INA219 power and current monitor](@ref sensor_ina219).
 *
 * @ingroup sensor_ina219
 */
/* clang-format on */
class TIINA219_Current : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219_Current object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TIINA219Amp".
     */
    explicit TIINA219_Current(
        TIINA219* parentSense, const char* uuid = "",
        const char* varCode = INA219_CURRENT_MA_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)INA219_CURRENT_MA_VAR_NUM,
                   (uint8_t)INA219_CURRENT_MA_RESOLUTION,
                   INA219_CURRENT_MA_VAR_NAME, INA219_CURRENT_MA_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new TIINA219_Current object.
     *
     * @note This must be tied with a parent TIINA219 before it can be used.
     */
    TIINA219_Current()
        : Variable((const uint8_t)INA219_CURRENT_MA_VAR_NUM,
                   (uint8_t)INA219_CURRENT_MA_RESOLUTION,
                   INA219_CURRENT_MA_VAR_NAME, INA219_CURRENT_MA_UNIT_NAME,
                   INA219_CURRENT_MA_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TIINA219_Current object - no action needed.
     */
    ~TIINA219_Current() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [bus voltage output](@ref sensor_ina219_volt) from a [TI INA219 power and current monitor](@ref sensor_ina219).
 *
 * @ingroup sensor_ina219
 */
/* clang-format on */
class TIINA219_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219_Voltage object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TIINA219Volt".
     */
    explicit TIINA219_Voltage(
        TIINA219* parentSense, const char* uuid = "",
        const char* varCode = INA219_BUS_VOLTAGE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)INA219_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)INA219_BUS_VOLTAGE_RESOLUTION,
                   INA219_BUS_VOLTAGE_VAR_NAME, INA219_BUS_VOLTAGE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new TIINA219_Voltage object.
     *
     * @note This must be tied with a parent TIINA219 before it can be used.
     */
    TIINA219_Voltage()
        : Variable((const uint8_t)INA219_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)INA219_BUS_VOLTAGE_RESOLUTION,
                   INA219_BUS_VOLTAGE_VAR_NAME, INA219_BUS_VOLTAGE_UNIT_NAME,
                   INA219_BUS_VOLTAGE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TIINA219_Voltage object - no action needed.
     */
    ~TIINA219_Voltage() {}
};

/**
 * @brief typedef for backwards compatibility; use the TIINA219_Voltage class in
 * new code
 *
 * @m_deprecated_since{0,33,0}
 */
typedef TIINA219_Voltage TIINA219_Volt;


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [power use output](@ref sensor_ina219_power) calculated from the voltage
 * and current measured by a [TI INA219 power and current monitor](@ref sensor_ina219).
 *
 * @ingroup sensor_ina219
 */
/* clang-format on */
class TIINA219_Power : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219_Power object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TIINA219Power".
     */
    explicit TIINA219_Power(TIINA219* parentSense, const char* uuid = "",
                            const char* varCode = INA219_POWER_MW_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)INA219_POWER_MW_VAR_NUM,
                   (uint8_t)INA219_POWER_MW_RESOLUTION,
                   INA219_POWER_MW_VAR_NAME, INA219_POWER_MW_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new TIINA219_Power object.
     *
     * @note This must be tied with a parent TIINA219 before it can be used.
     */
    TIINA219_Power()
        : Variable((const uint8_t)INA219_POWER_MW_VAR_NUM,
                   (uint8_t)INA219_POWER_MW_RESOLUTION,
                   INA219_POWER_MW_VAR_NAME, INA219_POWER_MW_UNIT_NAME,
                   INA219_POWER_MW_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TIINA219_Power object - no action needed.
     */
    ~TIINA219_Power() {}
};
/**@}*/
#endif  // SRC_SENSORS_TIINA219_H_
