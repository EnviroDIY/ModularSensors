/**
 * @file RainCounterI2C.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the RainCounterI2C sensor subclass and the variable
 * subclasses RainCounterI2C_Tips and RainCounterI2C_Depth.
 *
 * These are for an external tip counter, used to measure rainfall via a tipping
 * bucket rain gauge
 *
 * The tip counter works on an Adafruit Trinket.  Documentation for it can be
 * found at: https://github.com/EnviroDIY/TippingBucketRainCounter
 *
 * This depends on Testato's
 * [SoftwareWire](https://github.com/Testato/SoftwareWire) library if software
 * I2C is needed.
 */
/* clang-format off */
/**
 * @defgroup sensor_i2c_rain Trinket Tipping Bucket
 * Classes for the Trinket-based I2C tipping bucket rain counter
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_i2c_rain_intro Introduction
 *
 * This module is for use with a simple external I2C tipping bucket counter
 * based on an [Adafriut Trinket](https://www.adafruit.com/product/1501). This
 * is *NOT* for direct counting of tips using an interrupt on the main
 * processor.  The construction and programming of the tipping bucket counter is
 * documented on
 * [GitHub](https://github.com/EnviroDIY/TippingBucketRainCounter).  It is
 * assumed that the processor of the tip counter takes care of its own power
 * management.
 *
 * @section sensor_i2c_rain_datasheet Sensor Datasheet
 * - [Adafriut Trinket](https://www.adafruit.com/product/1501)
 * - [I2C Tipping Bucket Library](https://github.com/EnviroDIY/TippingBucketRainCounter)
 *
 * @section sensor_i2c_rain_flags Build flags
 * - `-D MS_RAIN_SOFTWAREWIRE`
 *      - switches from using hardware I2C to software I2C
 * @warning Either all or none of your attached tipping bucket counters may use
 * software I2C. Using some with software I2C and others with hardware I2C is
 * not supported. Though, honestly, having more than one attached seems pretty
 * unlikely anyway.
 *
 * @section sensor_i2c_rain_ctor Sensor Constructors
 * {{ @ref RainCounterI2C::RainCounterI2C(uint8_t, float) }}
 * {{ @ref RainCounterI2C::RainCounterI2C(TwoWire*, uint8_t, float) }}
 * {{ @ref RainCounterI2C::RainCounterI2C(SoftwareWire*, uint8_t, float) }}
 * {{ @ref RainCounterI2C::RainCounterI2C(int8_t, int8_t, uint8_t, float) }}
 *
 * ___
 * @section sensor_i2c_rain_examples Example Code
 * The Arduino-based I2C tipping bucket rain counter is used in the
 * @menulink{rain_counter_i2c} example.
 *
 * @menusnip{rain_counter_i2c}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_RAINCOUNTERI2C_H_
#define SRC_SENSORS_RAINCOUNTERI2C_H_

// Debugging Statement
// #define MS_RAINCOUNTERI2C_DEBUG

#ifdef MS_RAINCOUNTERI2C_DEBUG
#define MS_DEBUGGING_STD "RainCounterI2C"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

#if defined MS_RAIN_SOFTWAREWIRE
#include <SoftwareWire.h>  // Testato's SoftwareWire
#endif

/** @ingroup sensor_i2c_rain */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the tipping bucket counter can report 2
/// values.
#define BUCKET_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we calculate rain depth from the number of
/// tips, assuming either English or metric calibration.
#define BUCKET_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_i2c_rain_timing
 * @name Sensor Timing
 * The sensor timing for a Trinket-based tipping bucket counter
 * - Readings transferred from the tipping bucket to the logger are from past
 * tips, so there is no need to wait for stability or measuring.
 */
/**@{*/
/** @brief Sensor::_warmUpTime_ms; the tipping bucket counter warms up in 0ms.
 */
#define BUCKET_WARM_UP_TIME_MS 0
/// @brief Sensor::_stabilizationTime_ms; the tipping bucket counter is stable
/// after 0ms.
#define BUCKET_STABILIZATION_TIME_MS 0
/** @brief Sensor::_measurementTime_ms; the tipping bucket counter takes 0ms to
 * complete a measurement.
 */
#define BUCKET_MEASUREMENT_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_i2c_rain_depth
 * @name Rain Depth
 * The rain depth variable from a Trinket-based tipping bucket counter
 * - Range and accuracy depend on the tipping bucket used
 *
 * {{ @ref RainCounterI2C_Depth::RainCounterI2C_Depth }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; rain depth should have 2.
 *
 * Resolution is typically either 0.01" or 0.2mm of rainfall, depending on
 * if bucket is calibrated to English or metric units.
 */
#define BUCKET_RAIN_RESOLUTION 2
/// @brief Sensor variable number; rain depth is stored in sensorValues[0].
#define BUCKET_RAIN_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "precipitation"
#define BUCKET_RAIN_VAR_NAME "precipitation"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "millimeter"
#define BUCKET_RAIN_UNIT_NAME "millimeter"
/// @brief Default variable short code; "RainCounterI2CVol"
#define BUCKET_RAIN_DEFAULT_CODE "RainCounterI2CVol"
/**@}*/

/**
 * @anchor sensor_i2c_rain_tips
 * @name Tip Count
 * Defines for tip count variable from a Trinket-based tipping bucket counter
 * - Range and accuracy depend on the tipping bucket used.
 *
 * {{ @ref RainCounterI2C_Tips::RainCounterI2C_Tips }}
 */
/**@{*/
/// @brief Decimals places in string representation; the number of tips should
/// have 0 - resolution is 1 tip.
#define BUCKET_TIPS_RESOLUTION 0
/// @brief Sensor variable number; the number of tips is stored in
/// sensorValues[1].
#define BUCKET_TIPS_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "precipitation"
#define BUCKET_TIPS_VAR_NAME "precipitation"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "event"
#define BUCKET_TIPS_UNIT_NAME "event"
/// @brief Default variable short code; "RainCounterI2CTips"
#define BUCKET_TIPS_DEFAULT_CODE "RainCounterI2CTips"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Arduino-based external tipping bucket counter](@ref sensor_i2c_rain).
 *
 * @ingroup sensor_i2c_rain
 */
/* clang-format on */
class RainCounterI2C : public Sensor {
 public:
#if defined(MS_RAIN_SOFTWAREWIRE) || defined(DOXYGEN)
    /**
     * @brief Construct a new Rain Counter I2C object using a *software* I2C
     * instance.
     *
     * @param theI2C A [SoftwareWire](https://github.com/Testato/SoftwareWire)
     * instance for I2C communication.
     * @param i2cAddressHex The I2C address of the Trinket; can be any number
     * between 0x40 and 0x4F.  The default value is 0x08.
     * @param rainPerTip The depth of rain from a single tip; most likely either
     * 0.01" or 0.2mm, depending on your tipping bucket calibration.  The
     * default value is 0.2.
     */
    RainCounterI2C(SoftwareWire* theI2C, uint8_t i2cAddressHex = 0x08,
                   float rainPerTip = 0.2);
    /**
     * @brief Construct a new Rain Counter I2C object, also creating a
     * [SoftwareWire](https://github.com/Testato/SoftwareWire) I2C instance for
     * communication with that object.
     *
     * @note Unless there are address conflicts between I2C devices, you should
     * not create a new I2C instance.
     *
     * @param dataPin The pin on the mcu that will be used for I2C data (SDA).
     * Must be a valid pin number.
     * @param clockPin The pin on the mcu that will be used for the I2C clock
     * (SCL).  Must be a valid pin number.
     * @param i2cAddressHex The I2C address of the Trinket; can be any number
     * between 0x40 and 0x4F.  The default value is 0x08.
     * @param rainPerTip The depth of rain from a single tip; most likely either
     * 0.01" or 0.2mm, depending on your tipping bucket calibration.  The
     * default value is 0.2.
     */
    RainCounterI2C(int8_t dataPin, int8_t clockPin,
                   uint8_t i2cAddressHex = 0x08, float rainPerTip = 0.2);
#endif
#if !defined(MS_RAIN_SOFTWAREWIRE) | defined DOXYGEN
    /**
     * @brief Construct a new Rain Counter I2C object using a secondary
     * *hardware* I2C instance.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param i2cAddressHex The I2C address of the Trinket; can be any number
     * between 0x40 and 0x4F.  The default value is 0x08.
     * @param rainPerTip The depth of rain from a single tip; most likely either
     * 0.01" or 0.2mm, depending on your tipping bucket calibration.  The
     * default value is 0.2.
     */
    explicit RainCounterI2C(TwoWire* theI2C, uint8_t i2cAddressHex = 0x08,
                            float rainPerTip = 0.2);
    /**
     * @brief Construct a new Rain Counter I2C object using the primary
     * hardware I2C instance.
     *
     * @param i2cAddressHex The I2C address of the Trinket; can be any number
     * between 0x40 and 0x4F.  The default value is 0x08.
     * @param rainPerTip The depth of rain from a single tip; most likely either
     * 0.01" or 0.2mm, depending on your tipping bucket calibration.  The
     * default value is 0.2.
     */
    explicit RainCounterI2C(uint8_t i2cAddressHex = 0x08,
                            float   rainPerTip    = 0.2);
#endif
    /**
     * @brief Destroy the Rain Counter I2C object.  Also destroy the software
     * I2C instance if one was created.
     */
    ~RainCounterI2C();

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
     * @brief The depth of rain per tip.
     */
    float _rainPerTip;
    /**
     * @brief The I2C address of the Trinket counter.
     */
    uint8_t _i2cAddressHex;
#if defined(MS_RAIN_SOFTWAREWIRE) || defined(DOXYGEN)
    /**
     * @brief An internal reference to the SoftwareWire instance.
     */
    SoftwareWire* _i2c;  // Software Wire
    /**
     * @brief A flag denoting whether a new SoftwareWire instance was created.
     * If it was created, it must be destroyed in the destructor to avoid a
     * memory leak.
     */
    bool createdSoftwareWire = false;
#endif
#if !defined(MS_RAIN_SOFTWAREWIRE) || defined(DOXYGEN)
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;  // Hardware Wire
#endif
};


/**
 * @brief The Variable sub-class used for the
 * [total tip count output](@ref sensor_i2c_rain_tips) from an
 * [Adafruit Trinket based I2C tipping bucket counter](@ref sensor_i2c_rain)
 * - gives the number of tips since last reading.
 *
 * @ingroup sensor_i2c_rain
 */
class RainCounterI2C_Tips : public Variable {
 public:
    /**
     * @brief Construct a new RainCounterI2C_Tips object.
     *
     * @param parentSense The parent RainCounterI2C providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainCounterI2CTips".
     */
    explicit RainCounterI2C_Tips(RainCounterI2C* parentSense,
                                 const char*     uuid = "",
                                 const char* varCode = BUCKET_TIPS_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BUCKET_TIPS_VAR_NUM,
                   (uint8_t)BUCKET_TIPS_RESOLUTION, BUCKET_TIPS_VAR_NAME,
                   BUCKET_TIPS_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new RainCounterI2C_Tips object.
     *
     * @note This must be tied with a parent RainCounterI2C before it can be
     * used.
     */
    RainCounterI2C_Tips()
        : Variable((const uint8_t)BUCKET_TIPS_VAR_NUM,
                   (uint8_t)BUCKET_TIPS_RESOLUTION, BUCKET_TIPS_VAR_NAME,
                   BUCKET_TIPS_UNIT_NAME, BUCKET_TIPS_DEFAULT_CODE) {}
    /**
     * @brief Destroy the RainCounterI2C_Tips object - no action needed.
     */
    ~RainCounterI2C_Tips() {}
};

/**
 * @brief The Variable sub-class used for the
 * [depth of rain output](@ref sensor_i2c_rain_depth) from an
 * [Adafruit Trinket based I2C tipping bucket counter](@ref sensor_i2c_rain)
 * - gives the number of mm since the last reading.
 *
 * @ingroup sensor_i2c_rain
 */
class RainCounterI2C_Depth : public Variable {
 public:
    /**
     * @brief Construct a new RainCounterI2C_Depth object.
     *
     * @param parentSense The parent RainCounterI2C providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RainCounterI2CVol".
     */
    explicit RainCounterI2C_Depth(
        RainCounterI2C* parentSense, const char* uuid = "",
        const char* varCode = BUCKET_RAIN_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BUCKET_RAIN_VAR_NUM,
                   (uint8_t)BUCKET_RAIN_RESOLUTION, BUCKET_RAIN_VAR_NAME,
                   BUCKET_RAIN_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new RainCounterI2C_Depth object.
     *
     * @note This must be tied with a parent RainCounterI2C before it can be
     * used.
     */
    RainCounterI2C_Depth()
        : Variable((const uint8_t)BUCKET_RAIN_VAR_NUM,
                   (uint8_t)BUCKET_RAIN_RESOLUTION, BUCKET_RAIN_VAR_NAME,
                   BUCKET_RAIN_UNIT_NAME, BUCKET_RAIN_DEFAULT_CODE) {}
    /**
     * @brief Destroy the RainCounterI2C_Depth object - no action needed.
     */
    ~RainCounterI2C_Depth() {}
};
/**@}*/
#endif  // SRC_SENSORS_RAINCOUNTERI2C_H_
