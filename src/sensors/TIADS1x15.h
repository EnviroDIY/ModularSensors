/**
 * @file TIADS1x15.h *
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief This file contains the TIADS1x15 sensor subclass and the
 * TIADS1x15_Voltage variable subclass.
 *
 * These are used for any voltage measureable on a TI ADS1115 or ADS1015.  There
 * is a multiplier allowed for a voltage divider between the raw voltage and the
 * ADS.
 *
 * This depends on the soligen2010 fork of the Adafruit ADS1015 library.
 */
/* clang-format off */
/**
 * @defgroup analog_group Analog Sensors via TI ADS1x15
 * The Sensor and Variable objects for all analog sensors requiring
 * analog-to-digital conversion by TI ADS1x15.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section analog_intro Introduction
 *
 * Many sensors output simple analog voltages and leave the conversion to a digital
 * signal and final result up to the user.  These types of sensors are supported in
 * Modular sensors by way of a Texas Instruments ADS1115 or ADS1015.  The TI ADS1115 ADD
 * is a precision, low-power, 16-bit, I2C-compatible, analog-to-digitalconverter (ADC).
 * It is built into the EnviroDIY Mayfly.  The ADS1015 is a very similar ADC with 12-bit
 * resolution and a slightly lower price point.  Both chips are widely available in
 * Arduino-ready breakouts.
 *
 * ModularSensors always uses the TI ADS1115 (also ADS1113 or ADS1114) by default, but
 * if you wish to use a 12 bit TI ADS1015 (also ADS1013 or ADS1014) you can compile with
 * the build flag ```-DMS_USE_ADS1015```.
 *
 * In the majority of break-out boards, and on the Mayfly, the I2C address of the
 * ADS1x15 is set as 0x48 by tying the address pin to ground.  Up to four of these ADD's
 * be used by changing the address value by changing the connection of the address pin
 * on the ADS1x15.  The ADS1x15 requires an input voltage of 2.0-5.5V, but *this library
 * always assumes the ADS is powered with 3.3V*.
 *
 * @note ModularSensors only supports connecting the ADS1x15 to primary hardware I2C instance.
 * Connecting the ADS to a secondary hardware or software I2C instance is *not* supported!
 *
 * Communication with the ADS1x15 depends on the
 * [soligen2010 fork of the Adafruit ADS1015 library](https://github.com/soligen2010/Adafruit_ADS1X15).
 *
 * @note We do *not* use the Adafruit library! The soligen2010 fork corrects some errors
 * in the Adafruit library.  Until those corrections are pulled into the main library, we
 * use the fork instead.
 *
 * @section analog_ads1x15_specs Specifications
 * @note *In all cases, we assume that the ADS1x15 is powered at 3.3V and set the ADC's internal gain to 1x.
 *
 * This divides the bit resolution over the range of 0-4.096V.
 * - Response time: < 1ms
 * - Resample time: 860 samples per second (~1.2ms)
 * - Range:
 *   - Range is determined by supply voltage - No more than VDD + 0.3 V r 5.5 V
 *     (whichever is smaller) must be applied to this device.
 *   - 0 - 3.6V [when ADC is powered at 3.3V]
 * - Accuracy:
 *   - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset errror)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15% (gain error), <3 LSB (offset errror)
 * - Resolution:
 *   - 16-bit ADC (ADS1115):
 *     - without voltage divider:  0.125 mV
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```):
 *     - without voltage divider:  2 mV
 *
 * @note 1 bit of resolution is lost in single-ended reading.  The maximum possible
 * resolution is over the differential range from negative to positive full scale, a
 * single ended reading is only over the range from 0 to positive full scale).
 *
 * @section analog_ads1x15_datasheet Sensor Datasheet
 * Technical specifications for the TI ADS1115 can be found at: http://www.ti.com/product/ADS1115
 * For the ADS1015, see: https://www.ti.com/product/ADS1015
 * - [ADS1115 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/TI-ADS101x-Analog-to-Digital-Converter.pdf)
 * - [ADS1015 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/TI-ADS111x-Analog-to-Digital-Converter.pdf)
 */
/* clang-format on */
/* clang-format off */
/**
 * @defgroup sensor_ads1x15 TI ADS1x15 External Voltage Sensor
 * Classes for simple external analog voltage measurements.
 *
 * @ingroup analog_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_ads1x15_intro Introduction
 *
 * Analog data output is supported in ModularSensors by way of the
 * Texas Instruments ADS1115.
 *
 * The External Voltage module is used for any case where the voltage itself is
 * the desired value (as for an external battery).  It can also be used in
 * combination with a calculated variable to support any other analog sensor not
 * explicity supported by ModularSensors.  To increase the range available for
 * voltage measurements, this module supports the use of a voltage divider.
 *
 * If you are working with an EnviroDIY Mayfly, the easiest voltage divider to
 * connect is the Grove voltage divider sold by seeed studio.  The grove voltage
 * divider is a simple voltage divider designed to measure high external
 * voltages on a low voltage ADC.  This module employs a variable gain via two
 * pairs of voltage dividers, and a unity gain amplification to reduce output
 * impedance of the module.
 *
 * @section sensor_ads1x15_datasheet Sensor Datasheet
 * Technical specifications for the Grove Voltage Divider can be found at:
 * http://wiki.seeedstudio.com/Grove-Voltage_Divider
 *
 * @section sensor_ads1x15_flags Build flags
 * - ```-D MS_USE_ADS1015```
 *      - switches from the 16-bit ADS1115 to the 12 bit ADS1015
 *
 * @section sensor_ads1x15_ctor Sensor Constructor
 * {{ @ref TIADS1x15::TIADS1x15 }}
 *
 * ___
 * @section sensor_ads1x15_examples Example Code
 * The TI ADS1X15 external voltage sensor is used in the @menulink{tiads1x15}
 * example.
 *
 * @menusnip{tiads1x15}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_TIADS1X15_H_
#define SRC_SENSORS_TIADS1X15_H_

// Debugging Statement
// #define MS_TIADS1X15_DEBUG

#ifdef MS_TIADS1X15_DEBUG
#define MS_DEBUGGING_STD "TIADS1x15"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_ads1x15 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the ADS1115 can report 1 value.
#define TIADS1X15_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define TIADS1X15_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_ads1x15_timing
 * @name Sensor Timing
 * The sensor timing for a TI ADS1x15 analog-to-digital converter (ADC)
 * The timing used for simple external voltage measurements is that of the
 * ADS1x15.
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the ADS1115 warms up in 2ms.
#define TIADS1X15_WARM_UP_TIME_MS 2
/**
 * @brief Sensor::_stabilizationTime_ms; the ADS1115 is stable 0ms after warm-up
 *
 * We assume a voltage is instantly ready.
 */
#define TIADS1X15_STABILIZATION_TIME_MS 0
/**
 * @brief Sensor::_measurementTime_ms; the ADS1115 completes 860 conversions per
 * second, but the wait for the conversion to complete is built into the
 * underlying library, so we do not need to wait further here.
 */
#define TIADS1X15_MEASUREMENT_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_ads1x15_volt
 * @name Voltage
 * The volt variable from a TI ADS1x15 analog-to-digital converter (ADC)
 *   - Range:
 *     - without voltage divider:  0 - 3.6V [when ADC is powered at 3.3V]
 *     - 1/gain = 3x: 0.3 ~ 12.9V
 *     - 1/gain = 10x: 1 ~ 43V
 *   - Accuracy:
 *     - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset errror)
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset errror)
 *   - Resolution:
 *     - 16-bit ADC (ADS1115):
 *       - @m_span{m-dim}@ref #TIADS1X15_RESOLUTION = 4@m_endspan
 *       - without voltage divider:  0.125 mV
 *       - 1/gain = 3x: 0.375 mV
 *       - 1/gain = 10x: 1.25 mV
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```):
 *       - @m_span{m-dim}@ref #TIADS1X15_RESOLUTION = 1@m_endspan
 *       - without voltage divider:  2 mV
 *       - 1/gain = 3x: 6 mV
 *       - 1/gain = 10x: 20 mV *
 *
 * {{ @ref TIADS1x15_Voltage::TIADS1x15_Voltage }}
 */
/**@{*/
/// Variable number; voltage is stored in sensorValues[0].
#define TIADS1X15_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "voltage"
#define TIADS1X15_VAR_NAME "voltage"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "volt"
#define TIADS1X15_UNIT_NAME "volt"
/// @brief Default variable short code; "extVoltage"
#define TIADS1X15_DEFAULT_CODE "extVoltage"

#ifdef MS_USE_ADS1015
/// @brief Decimals places in string representation; voltage should have 1.
#define TIADS1X15_RESOLUTION 1
#else
/// @brief Decimals places in string representation; voltage should have 4.
#define TIADS1X15_RESOLUTION 4
#endif
/**@}*/

/// @brief The assumed address of the ADS1115, 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS 0x48

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [external votlage as measured by TI ADS1115 or ADS1015](@ref sensor_ads1x15).
 *
 * @ingroup sensor_ads1x15
 */
/* clang-format on */
class TIADS1x15 : public Sensor {
 public:
    /**
     * @brief Construct a new External Voltage object - need the power pin and
     * the data channel on the ADS1x15.
     *
     * The gain value, I2C address, and number of measurements to average are
     * optional.  If nothing is given a 1x gain is used.
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the sensor
     * Use -1 if it is continuously powered.
     * @param adsChannel The ADS channel of interest (0-3).
     * @param gain The gain multiplier, if a voltage divider is used.
     * @param i2cAddress The I2C address of the ADS 1x15, default is 0x48 (ADDR
     * = GND)
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    TIADS1x15(int8_t powerPin, uint8_t adsChannel, float gain = 1,
              uint8_t i2cAddress            = ADS1115_ADDRESS,
              uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the External Voltage object
     */
    ~TIADS1x15();

    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    uint8_t _adsChannel;
    float   _gain;
    uint8_t _i2cAddress;
};

/**
 * @brief typedef for backwards compatibility; use the TIADS1x15 class in new
 * code
 *
 * @m_deprecated_since{0,33,0}
 */
typedef TIADS1x15 ExternalVoltage;


// The single available variable is voltage
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [voltage output](@ref sensor_ads1x15_volt) from a
 * [TI ADS1115 or ADS1015](@ref sensor_ads1x15).
 *
 * @ingroup sensor_ads1x15
 */
/* clang-format on */
class TIADS1x15_Voltage : public Variable {
 public:
    /**
     * @brief Construct a new TIADS1x15_Voltage object.
     *
     * @param parentSense The parent TIADS1x15 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "extVoltage".
     */
    explicit TIADS1x15_Voltage(TIADS1x15* parentSense, const char* uuid = "",
                               const char* varCode = TIADS1X15_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)TIADS1X15_VAR_NUM,
                   (uint8_t)TIADS1X15_RESOLUTION, TIADS1X15_VAR_NAME,
                   TIADS1X15_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new TIADS1x15_Voltage object.
     *
     * @note This must be tied with a parent TIADS1x15 before it can be
     * used.
     */
    TIADS1x15_Voltage()
        : Variable((const uint8_t)TIADS1X15_VAR_NUM,
                   (uint8_t)TIADS1X15_RESOLUTION, TIADS1X15_VAR_NAME,
                   TIADS1X15_UNIT_NAME, TIADS1X15_DEFAULT_CODE) {}
    /**
     * @brief Destroy the TIADS1x15_Voltage object - no action needed.
     */
    ~TIADS1x15_Voltage() {}
};

/**
 * @brief typedef for backwards compatibility; use the TIADS1x15_Voltage class
 * in new code
 *
 * @m_deprecated_since{0,33,0}
 */
typedef TIADS1x15_Voltage ExternalVoltage_Volt;

/**@}*/
#endif  // SRC_SENSORS_TIADS1X15_H_
