/**
 * @file TIINA219M.h
 * @author Written By: Neil Hancock
 *Initial library developement done by Neil hancock
 *
 * @brief Contains the TIINA219M sensor subclass and the variale subclasses
 * TIINA219_Current, TIINA219_Volt, and TIINA219_Power.
 *
 * These are for the Texas Instruments INA219 current/voltage sensor.
 *
 * This depends on the
 * [Adafruit INA219 Library](https://github.com/adafruit/Adafruit_INA219)
 */
/* clang-format off */
/**
 * @defgroup ina219_group TI INA219
 * Classes for the TI INA219 current and voltage sensor.
 *https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout
 *http://www.ti.com/product/INA219
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section ina219_intro Intruduction
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
 * @section ina219_datasheet Sensor Datasheet
 *
 * Documentation for the sensor can be found at:
 * https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout and
 * http://www.ti.com/product/INA219
 *
 * @section ina219_sensor The INA219 Sensor
 * @ctor_doc{TIINA219, int8_t  powerPin, uint8_t i2cAddressHex, uint8_t measurementsToAverage}
 * @subsection ina219_timing Sensor Timing
 * - The sensor warms up in 100ms
 * - A single ADC conversion takes >532 µs (586 µs typical) at 12 bit
 * resolution, but in tests waiting closer to 1.1s gave better data.
 * - Stable numbers can be acheived after 500ms, but waiting up to 4s gave more
 * consistent numbers.
 *
 * @section ina219_current Current Output
 *   - Range is between +/-0.4 Amps and +/-3.2 Amps
 *   - Absolute accuracy is range dependent, and approx 2LSB (R accuracy
 * unknown)
 *   - Result stored in sensorValues[0]
 *   - Resolution is 12-bit
 *     - 0.8mA using +/-3.2 Amp range
 *     - 0.1mA using +/-0.4 Amp range
 *   - Reported as milliamps (mA)
 *   - Default variable code is TIINA219Amp
 * For Current Sensor with 0.1Ohms:
 *  Full scale ranges are
 *   +-3.2Amps resolution is 0.8mA
 *   +-0.4Amps resolution is 0.1mA
 *  Absolute Accuracy is range dependent, and approx 2LSB (R accuracy unknown)
 *
 * For Voltage :
 *  Resolution is +-0.001V
 *  Accuracy is ?
 *  Range is 0 to 26V
 *
 * A single conversion takes >532 µs (586 µs typical) at 12 bit resolution
 * @variabledoc{ina219_current,TIINA219,Current,TIINA219Amp}
 *
 * @section ina219_volt Voltage Output
 *   - Range is 0 to 26V
 *   - Accuracy is ±4mV (1 LSB step size)
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.001V
 *   - Reported as volts (V)
 *   - Default variable code is TIINA219Volt
 * @variabledoc{ina219_volt,TIINA219,Volt,TIINA219Volt}
 *
 * @section ina219_power Power Output
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.01mW
 *   - Reported as milliwatts (mW)
 *   - Default variable code is TIINA219Power
 * @variabledoc{ina219_power,TIINA219,Power,TIINA219Power}
 *
 * ___
 * @section ina219_examples Example Code
 * The TI INA219 is used in the @menulink{ina219} example.
 *
 * @menusnip{ina219}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_TIINA219M_H_
#define SRC_SENSORS_TIINA219M_H_

// Debugging Statement
// #define MS_TIINA219M_DEBUG

#ifdef MS_TIINA219M_DEBUG
#define MS_DEBUGGING_STD "TIINA219M"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_INA219.h>

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the INA219 can report 3 values.
#define INA219_NUM_VARIABLES 3

/// Sensor::_warmUpTime_ms; the INA219 warms up in 100ms.
#define INA219_WARM_UP_TIME_MS 100
/**
 * @brief Sensor::_stabilizationTime_ms; the INA219 is stable after 4000ms.
 *
 * 0.5 s for good numbers, but optimal at 4 s based on tests using
 * INA219timingTest.ino
 */
#define INA219_STABILIZATION_TIME_MS 4000
/**
 * @brief Sensor::_measurementTime_ms; the INA219 takes 1100ms to complete a
 * measurement.
 *
 * 1.0 s according to datasheet, but slightly better stdev when 1.1 s
 */
#define INA219_MEASUREMENT_TIME_MS 1100

/// Decimals places in string representation; current should have 4.
#define INA219_CURRENT_MA_RESOLUTION 4
/// Variable number; current is stored in sensorValues[0].
#define INA219_CURRENT_MA_VAR_NUM 0

/// Decimals places in string representation; bus voltage should have 4.
#define INA219_BUS_VOLTAGE_RESOLUTION 4
/// Variable number; bus voltage is stored in sensorValues[1].
#define INA219_BUS_VOLTAGE_VAR_NUM 1

/// Decimals places in string representation; power draw should have 2.
#define INA219_POWER_MW_RESOLUTION 2
/// Variable number; power draw is stored in sensorValues[2].
#define INA219_POWER_MW_VAR_NUM 2

/// The default address of the INA219
#define INA219_ADDRESS_BASE 0x40

/* Masks to control polling of sensors */

#define INA219_POLLMASK_A 0x01
#define INA219_POLLMASK_V 0x02
#define INA219_POLLMASK_W 0x04
//#define INA219_POLLMASK_ALL
//(INA219_POLLMASK_A|INA219_POLLMASK_V|INA219_POLLMASK_W)
#define INA219_POLLMASK_ALL (INA219_POLLMASK_A | INA219_POLLMASK_V)

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [TexasInstruments INA219 sensor](@ref ina219_group).
 *
 * @ingroup ina219_group
 */
/* clang-format on */
class TIINA219M : public Sensor {
 public:
    /**
     * @brief Construct a new TI INA219M object
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
    explicit TIINA219M(int8_t  powerPin,
                       uint8_t i2cAddressHex         = INA219_ADDRESS_BASE,
                       uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the TI INA219M object
     */
    ~TIINA219M();

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

    void    set_active_sensors(uint8_t sensors_mask);
    uint8_t which_sensors_active(void);
    void    setCustomAmpMult(float ampMult);
    float   getCustomAmpMult(void);
    void    setCustomVoltThreshold(float voltLowThreshold_V,
                                   void (*thresholdFxn)(bool  exceed,
                                                     float value_V));
    float   getCustomVoltThreshold(void);

 private:
    Adafruit_INA219 ina219_phy;
    uint8_t         _i2cAddressHex;
    uint8_t         _ina219_pollmask;

 private:
    float _ampMult;
    float _voltLowThreshold_V;  // 0 is off, Drops below activate
                                // _thresholdAlertFxn()
    void (*_thresholdAlertFxn)(
        bool exceed, float value_V);  // Fn to Alert when threshold is exceeded
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [current output](@ref ina219_current) from a [TI INA219 power and current monitor](@ref ina219_group).
 *
 * @ingroup ina219_group
 */
/* clang-format on */
class TIINA219M_Current : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219M_Current object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TIINA219Amp".
     */
    explicit TIINA219M_Current(TIINA219M* parentSense, const char* uuid = "",
                              const char* varCode = "TIINA219Amp")
        : Variable(parentSense, (const uint8_t)INA219_CURRENT_MA_VAR_NUM,
                   (uint8_t)INA219_CURRENT_MA_RESOLUTION, "electricCurrent",
                   "milliamp", varCode, uuid) {}
    /**
     * @brief Construct a new TIINA219M_Current object.
     *
     * @note This must be tied with a parent TIINA219 before it can be used.
     */
    TIINA219M_Current()
        : Variable((const uint8_t)INA219_CURRENT_MA_VAR_NUM,
                   (uint8_t)INA219_CURRENT_MA_RESOLUTION, "electricCurrent",
                   "milliamp", "TIINA219Amp") {}
    /**
     * @brief Destroy the TIINA219M_Current object - no action needed.
     */
    ~TIINA219M_Current(){};
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [bus voltage output](@ref ina219_volt) from a [TI INA219 power and current monitor](@ref ina219_group).
 *
 * @ingroup ina219_group
 */
/* clang-format on */
class TIINA219M_Volt : public Variable {
 public:
    /**
     * @brief Construct a new TIINA219_Volt object.
     *
     * @param parentSense The parent TIINA219 providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "TIINA219Volt".
     */
    explicit TIINA219M_Volt(TIINA219M* parentSense, const char* uuid = "",
                           const char* varCode = "TIINA219Volt")
        : Variable(parentSense, (const uint8_t)INA219_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)INA219_BUS_VOLTAGE_RESOLUTION, "voltage", "volt",
                   varCode, uuid) {}
    /**
     * @brief Construct a new TIINA219_Volt object.
     *
     * @note This must be tied with a parent TIINA219M before it can be used.
     */
    TIINA219M_Volt()
        : Variable((const uint8_t)INA219_BUS_VOLTAGE_VAR_NUM,
                   (uint8_t)INA219_BUS_VOLTAGE_RESOLUTION, "voltage", "volt",
                   "TIINA219Volt") {}
    /**
     * @brief Destroy the TIINA219M_Volt object - no action needed.
     */
    ~TIINA219M_Volt(){};
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [power use output](@ref ina219_power) calculated from the voltage
 * and current measured by a [TI INA219 power and current monitor](@ref ina219_group).
 *
 * @ingroup ina219_group
 */
/* clang-format on */
class TIINA219M_Power : public Variable {
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
    explicit TIINA219M_Power(TIINA219M* parentSense, const char* uuid = "",
                             const char* varCode = "TIINA219Power")
        : Variable(parentSense, (const uint8_t)INA219_POWER_MW_VAR_NUM,
                   (uint8_t)INA219_POWER_MW_RESOLUTION, "electricPower",
                   "milliwatt", varCode, uuid) {}
    /**
     * @brief Construct a new TIINA219M_Power object.
     *
     * @note This must be tied with a parent TIINA219M before it can be used.
     */
    TIINA219M_Power()
        : Variable((const uint8_t)INA219_POWER_MW_VAR_NUM,
                   (uint8_t)INA219_POWER_MW_RESOLUTION, "electricPower",
                   "milliwatt", "TIINA219Power") {}
    /**
     * @brief Destroy the TIINA219_Power object - no action needed.
     */
    ~TIINA219M_Power() {}
};

#endif  // SRC_SENSORS_TIINA219M_H_
