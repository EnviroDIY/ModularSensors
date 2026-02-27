/**
 * @file TIADS1x15.h *
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Adapted from CampbellOBS3.h by Sara Geleskie Damiano
 * <sdamiano@stroudcenter.org>
 *
 * @brief This file contains the TIADS1x15 sensor subclass and the
 * TIADS1x15_Voltage variable subclass.
 *
 * These are used for any voltage measurable on a TI ADS1115 or ADS1015.  There
 * is a multiplier allowed for a voltage divider between the raw voltage and the
 * ADS.
 *
 * This depends on the Adafruit ADS1X15 v2.x library
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
 * Modular sensors by way of a Texas Instruments ADS1115 or ADS1015.  The TI ADS1115 ADC
 * is a precision, low-power, 16-bit, I2C-compatible, analog-to-digital-converter (ADC).
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
 * @note ModularSensors supports connecting the ADS1x15 to primary hardware I2C instance
 * or to a secondary hardware I2C instance using the TwoWire parameter in the constructor.
 * Software I2C is *not* supported!
 *
 * Communication with the ADS1x15 depends on the
 * [soligen2010 fork of the Adafruit ADS1015 library](https://github.com/soligen2010/Adafruit_ADS1X15).
 *
 * @note We do *not* use the Adafruit library! The soligen2010 fork corrects some errors
 * in the Adafruit library.  Until those corrections are pulled into the main library, we
 * use the fork instead.
 *
 * @section analog_ads1x15_specs Specifications
 * @note *In all cases, we assume that the ADS1x15 is powered at 3.3V by default with configurable internal gain settings.
 *
 * The default gain setting is 1x (GAIN_ONE) which provides a PGA full-scale range (±4.096V).
 * In single-ended mode the actual ceiling is min(Full-Scale Range (FSR), VDD + 0.3V) — typically 3.6V at 3.3V supply.
 * - Response time: < 1ms
 * - Resample time: 860 samples per second (~1.2ms)
 * - Range:
 *   - Single-ended measurements: Limited by supply voltage (VDD + 0.3V max, absolute max 5.5V)
 *     - 0 - 3.6V [when ADC is powered at 3.3V]
 *   - Differential measurements: Limited by PGA full-scale range (gain-dependent)
 *     - GAIN_TWOTHIRDS = ±6.144V PGA full-scale range
 *     - GAIN_ONE = ±4.096V PGA full-scale range
 *     - GAIN_TWO = ±2.048V PGA full-scale range
 *     - GAIN_FOUR = ±1.024V PGA full-scale range
 *     - GAIN_EIGHT = ±0.512V PGA full-scale range
 *     - GAIN_SIXTEEN = ±0.256V PGA full-scale range
 * - Accuracy:
 *   - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *   - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15% (gain error), <3 LSB (offset error)
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
 * explicitly supported by ModularSensors.  To increase the range available for
 * voltage measurements, this module supports the use of a voltage divider.
 *
 * If you are working with an EnviroDIY Mayfly, the easiest voltage divider to
 * connect is the Grove voltage divider sold by seeed studio.  The grove voltage
 * divider is a simple voltage divider designed to measure high external
 * voltages on a low voltage ADC.  This module employs a variable voltage multiplier
 * via two pairs of voltage dividers, and a unity gain amplification to reduce output
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

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Include known processor settings for default operating voltage
#include "KnownProcessors.h"

// Define the print label[s] for the debugger
#ifdef MS_TIADS1X15_DEBUG
#define MS_DEBUGGING_STD "TIADS1x15"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include "AnalogVoltageBase.h"
#include <Adafruit_ADS1X15.h>

/** @ingroup sensor_ads1x15 */
/**@{*/

/**
 * @anchor sensor_ads1x15_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by ADS1x15
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the ADS1115 can report 1 value.
#define TIADS1X15_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define TIADS1X15_INC_CALC_VARIABLES 0
/**@}*/

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
 *   - Range (with no external voltage divider):
 *     - 0 - min(4.096V, supply voltage + 0.3V)
 *   - Accuracy:
 *     - 16-bit ADC (ADS1115): < 0.25% (gain error), <0.25 LSB (offset error)
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```): < 0.15%
 * (gain error), <3 LSB (offset error)
 *   - Resolution (based on ADC's 4.096V internal reference with 1x gain and no
 * external voltage divider):
 *     - 16-bit ADC (ADS1115):
 *       - @m_span{m-dim}@ref #TIADS1X15_RESOLUTION = 4@m_endspan
 *       - 0.125 mV
 *     - 12-bit ADC (ADS1015, using build flag ```MS_USE_ADS1015```):
 *       - @m_span{m-dim}@ref #TIADS1X15_RESOLUTION = 1@m_endspan
 *       - 2 mV
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

/**
 * @brief TI ADS1x15 base class that inherits from AnalogVoltageBase
 *
 * This class provides ADS1x15-specific analog functionality on top of
 * the generic AnalogVoltageBase class. It handles ADS configuration,
 * I2C communication, and differential/single-ended measurement modes.
 */
class TIADS1x15Base : public AnalogVoltageBase {
 public:

    /**
     * @brief Construct a new TIADS1x15Base object
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * speed and sensitivity requirements of the ADS1x15, only hardware I2C is
     * supported.  For AVR boards, this will be Wire.  For SAMD boards, this
     * can be Wire, Wire1, Wire2, etc..
     * @param voltageMultiplier The voltage multiplier for any voltage dividers
     * @param adsGain The internal gain setting of the ADS1x15
     * @param i2cAddress The I2C address of the ADS 1x15
     * @param adsSupplyVoltage The power supply voltage for the ADS1x15 in volts
     * @param adsDataRate The data rate for the ADS1x15 (samples per second)
     */
    explicit TIADS1x15Base(TwoWire* theI2C,
                           float     voltageMultiplier = 1.0f,
                           adsGain_t adsGain           = GAIN_ONE,
                           uint8_t   i2cAddress = MS_DEFAULT_ADS1X15_ADDRESS,
                           float     adsSupplyVoltage = OPERATING_VOLTAGE,
#ifndef MS_USE_ADS1015
                           uint16_t adsDataRate = RATE_ADS1115_128SPS
#else
                           uint16_t adsDataRate = RATE_ADS1015_1600SPS
#endif
    );

    /**
     * @brief Construct a new TIADS1x15Base object using the default hardware
     * Wire instance.
     *
     * @param voltageMultiplier The voltage multiplier for any voltage dividers
     * @param adsGain The internal gain setting of the ADS1x15
     * @param i2cAddress The I2C address of the ADS 1x15
     * @param adsSupplyVoltage The power supply voltage for the ADS1x15 in volts
     * @param adsDataRate The data rate for the ADS1x15 (samples per second)
     */
    explicit TIADS1x15Base(float     voltageMultiplier = 1.0f,
                           adsGain_t adsGain           = GAIN_ONE,
                           uint8_t   i2cAddress = MS_DEFAULT_ADS1X15_ADDRESS,
                           float     adsSupplyVoltage = OPERATING_VOLTAGE,
#ifndef MS_USE_ADS1015
                           uint16_t adsDataRate = RATE_ADS1115_128SPS
#else
                           uint16_t adsDataRate = RATE_ADS1015_1600SPS
#endif
    );

    /**
     * @brief Destroy the TIADS1x15Base object
     */
    virtual ~TIADS1x15Base() = default;

    /**
     * @brief Initialize the ADS1x15 analog voltage reading system
     *
     * This function performs hardware initialization that cannot be safely
     * done in the constructor, including I2C communication with the ADS1x15
     * device to configure gain and data rate settings.
     *
     * @return True if the initialization was successful, false otherwise
     */
    bool begin(void) override;

    /**
     * @brief Read a single-ended voltage measurement from the ADS1x15
     *
     * @param analogChannel The ADS channel of interest (0-3, physical channel
     * only).  Negative or invalid channel numbers are not clamped and will
     * cause the reading to fail and emit a warning.
     * @param resultValue Reference to store the resulting voltage measurement
     * @return True if the voltage reading was successful
     */
    bool readVoltageSingleEnded(int8_t analogChannel,
                                float& resultValue) override;

    /**
     * @brief Read a differential voltage measurement from the ADS1x15
     *
     * @param analogChannel The primary analog channel for differential
     * measurement. Negative or invalid channel numbers or pairings between the
     * analogChannel and analogReferenceChannel are not clamped and will cause
     * the reading to fail and emit a warning.
     * @param analogReferenceChannel The secondary (reference) analog channel
     * for differential measurement. Negative or invalid channel numbers or
     * pairings between the analogChannel and analogReferenceChannel are not
     * clamped and will cause the reading to fail and emit a warning.
     * @param resultValue Reference to store the resulting voltage measurement
     * @return True if the voltage reading was successful and within valid range
     */
    bool readVoltageDifferential(int8_t analogChannel,
                                 int8_t analogReferenceChannel,
                                 float& resultValue) override;

    String getAnalogLocation(int8_t analogChannel,
                             int8_t analogReferenceChannel) override;

    /**
     * @brief Set the internal gain setting for the ADS1x15
     *
     * @param adsGain The internal gain setting (GAIN_TWOTHIRDS, GAIN_ONE,
     * GAIN_TWO, GAIN_FOUR, GAIN_EIGHT, GAIN_SIXTEEN)
     */
    void setADSGain(adsGain_t adsGain);

    /**
     * @brief Get the internal gain setting for the ADS1x15
     *
     * @return The internal gain setting
     */
    adsGain_t getADSGain(void);

    /**
     * @brief Set the data rate for the ADS1x15
     *
     * @param adsDataRate The data rate setting (samples per second)
     */
    void setADSDataRate(uint16_t adsDataRate);

    /**
     * @brief Get the data rate for the ADS1x15
     *
     * @return The data rate setting (samples per second)
     */
    uint16_t getADSDataRate(void);

    /**
     * @brief Check if the two channels form a valid differential pair
     *
     * @param channel1 First channel (0-3, physical ADS channel indices only)
     * @param channel2 Second channel (0-3, physical ADS channel indices only)
     * @return True if the combination is valid (0-1, 0-3, 1-3, or 2-3)
     *
     * @note Channel parameters use int8_t, consistent with the rest of the
     * ModularSensors channel conventions. Negative values indicate invalid
     * channels.
     */
    static bool isValidDifferentialPair(int8_t channel1, int8_t channel2);

    /**
     * @brief Set the supply voltage for the ADS1x15
     *
     * @param supplyVoltage The supply voltage in volts
     *
     * @note Valid range is 0.0V to 5.5V per ADS1x15 datasheet. Values outside
     * this range will be clamped with debug logging.
     */
    void setSupplyVoltage(float supplyVoltage) override;

    /**
     * @brief Calculate the analog resolution in volts for the ADS1x15
     *
     * For ADS1x15, this calculates the voltage resolution based on the current
     * gain setting, supply voltage, and ADC bit resolution. The resolution
     * depends on:
     * - ADC model: 12-bit (ADS1015) or 16-bit (ADS1115)
     * - Gain setting: determines PGA full-scale range
     * - Supply voltage: limits actual usable range for single-ended
     * measurements
     *
     * The effective full scale range is the minimum of:
     * - PGA full-scale range (gain-dependent)
     * - Supply voltage + 0.3V (for single-ended measurements)
     * - Absolute maximum 5.5V per datasheet
     *
     * @return The analog resolution in volts per LSB
     */
    float calculateAnalogResolutionVolts(void) override;

 protected:
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _wire;
    /**
     * @brief Internal reference to the I2C address of the TI-ADS1x15
     */
    uint8_t _i2cAddress;
    /**
     * @brief The internal gain setting for the ADS1x15
     */
    adsGain_t _adsGain;
    /**
     * @brief The data rate setting for the ADS1x15
     */
    uint16_t _adsDataRate;
    /**
     * @brief Per-instance ADS1x15 driver to maintain separate I2C state
     */
#ifndef MS_USE_ADS1015
    Adafruit_ADS1115 _ads;  // 16-bit version
#else
    Adafruit_ADS1015 _ads;  // 12-bit version
#endif
};

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [external voltage as measured by TI ADS1115 or ADS1015](@ref sensor_ads1x15).
 *
 * @ingroup sensor_ads1x15
 */
/* clang-format on */
class TIADS1x15 : public Sensor {
 public:
    /**
     * @brief Construct a new TIADS1x15 object for single-ended or differential
     * voltage measurements
     *
     * @note ModularSensors only supports connecting the ADS1x15 to the primary
     * hardware I2C instance defined in the Arduino core. Connecting the ADS to
     * a secondary hardware or software I2C instance is *not* supported!
     *
     * @param powerPin The pin on the mcu controlling power to the sensor
     * Use -1 if it is continuously powered.
     * @param adsChannel The ADS channel of interest (0-3, physical channel
     * only). For differential measurements, this is the first (positive)
     * channel.
     * @param analogReferenceChannel The second (reference/negative) ADS channel
     * for differential measurement (0-3, physical channel only). Valid pairs
     * are: 0-1, 0-3, 1-3, or 2-3. Use -1 (default) for single-ended
     * measurements.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param analogVoltageReader Pointer to TIADS1x15Base object for ADS
     * functionality. If nullptr (default), creates a new TIADS1x15Base with
     * default settings.
     */
    TIADS1x15(int8_t powerPin, int8_t adsChannel,
              int8_t         analogReferenceChannel = -1,
              uint8_t        measurementsToAverage  = 1,
              TIADS1x15Base* analogVoltageReader    = nullptr);
    /**
     * @brief Destroy the TIADS1x15 object
     */
    ~TIADS1x15() override;

    // Delete copy constructor and copy assignment operator to prevent shallow
    // copies
    TIADS1x15(const TIADS1x15&)            = delete;
    TIADS1x15& operator=(const TIADS1x15&) = delete;

    // Delete move constructor and move assignment operator
    TIADS1x15(TIADS1x15&&)            = delete;
    TIADS1x15& operator=(TIADS1x15&&) = delete;

    String getSensorLocation(void) override;

    bool setup(void) override;

    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief The second (reference) pin for differential voltage measurements.
     *
     * For single-ended measurements: -1 (not used)
     * For differential measurements: the second ADS channel (0-3)
     *
     * @note The primary pin is stored as Sensor::_dataPin.
     */
    int8_t _analogReferenceChannel = -1;

    /**
     * @brief Pointer to the TIADS1x15Base object providing ADS functionality
     */
    TIADS1x15Base* _analogVoltageReader = nullptr;

    /**
     * @brief Whether this object owns the _analogVoltageReader pointer and
     * should delete it
     */
    bool _ownsAnalogVoltageReader = false;
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
        : Variable(parentSense, (uint8_t)TIADS1X15_VAR_NUM,
                   (uint8_t)TIADS1X15_RESOLUTION, TIADS1X15_VAR_NAME,
                   TIADS1X15_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new TIADS1x15_Voltage object.
     *
     * @note This must be tied with a parent TIADS1x15 before it can be
     * used.
     */
    TIADS1x15_Voltage()
        : Variable((uint8_t)TIADS1X15_VAR_NUM, (uint8_t)TIADS1X15_RESOLUTION,
                   TIADS1X15_VAR_NAME, TIADS1X15_UNIT_NAME,
                   TIADS1X15_DEFAULT_CODE) {}
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

// cspell:words GAIN_TWOTHIRDS

#endif  // SRC_SENSORS_TIADS1X15_H_
