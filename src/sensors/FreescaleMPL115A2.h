/**
 * @file FreescaleMPL115A2.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Bobby Schulz <schu3119@umn.edu>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the FreescaleMPL115A2 sensor subclass and the variable
 * subclasses FreescaleMPL115A2_Temp and FreescaleMPL115A2_Pressure.
 *
 * These are used for the Freescale Semiconductor MPL115A2 Miniature I2C Digital
 * Barometer.
 *
 * This depends on the https://github.com/adafruit/Adafruit_MPL115A2 library
 */
/* clang-format off */
/**
 * @defgroup sensor_mpl115a2 Freescale Semiconductor MPL115A2
 * Classes for the Freescale Semiconductor MPL115A2 digital barometer.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_mpl115a2_intro Introduction
 *
 * The Freescale Semiconductor MPL115A2 is a low-cost, low-power absolute
 * pressure sensor with a digital I2C output.  It is optimized for barometric
 * measurements. Because this sensor can have only one I2C address (0x60), it is
 * only possible to connect one of these sensors to a single I2C bus.  This sensor
 * should be attached to a 2.375-5.5V power source and the power supply to the
 * sensor can be stopped between measurements.  Communication with the MPL115A2
 * is managed by the
 * [Adafruit MPL115A2 library](https://github.com/adafruit/Adafruit_MPL115A2).
 *
 * @note Software I2C is *not* supported for the AM2315.
 * A secondary hardware I2C on a SAMD board is supported.
 *
 * @section sensor_mpl115a2_datasheet Sensor Datasheet
 * Documentation for the sensor can be found at:
 * https://www.adafruit.com/product/992 and
 * https://github.com/adafruit/Adafruit-MPL115A2-Breakout-PCB
 *
 * A copy of the datasheet is available here:
 * https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Freescale-Semiconductor-MPL115A2.pdf)
 *
 * @section sensor_mpl115a2_ctor Sensor Constructors
 * {{ @ref FreescaleMPL115A2::FreescaleMPL115A2(int8_t, uint8_t) }}
 * {{ @ref FreescaleMPL115A2::FreescaleMPL115A2(TwoWire*, int8_t, uint8_t) }}
 *
 * ___
 * @section sensor_mpl115a2_examples Example Code
 * The Freescale Semiconductor MPL115A2 is used in the @menulink{mpl115a2}
 * example.
 *
 * @menusnip{freescale_mpl115a2}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_FREESCALEMPL115A2_H_
#define SRC_SENSORS_FREESCALEMPL115A2_H_

// Debugging Statement
// #define MS_FREESCALEMPL115A2_DEBUG

#ifdef MS_FREESCALEMPL115A2_DEBUG
#define MS_DEBUGGING_STD "FreescaleMPL115A2"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <Adafruit_MPL115A2.h>

/** @ingroup sensor_mpl115a2 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the MPL115A2 can report 2 values.
#define MPL115A2_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define MPL115A2_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_mpl115a2_timing
 * @name Sensor Timing
 * The sensor timing for a Freescale MPL115A2
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the MPL115A2 takes about 6 ms to respond.
#define MPL115A2_WARM_UP_TIME_MS 6
/// @brief Sensor::_stabilizationTime_ms; the MPL115A2 is stable as soon as it
/// warms up (0ms stabilization).
#define MPL115A2_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; the MPL115A2 takes 3ms to complete a
/// measurement.
#define MPL115A2_MEASUREMENT_TIME_MS 3
/**@}*/

/**
 * @anchor sensor_mpl115a2_temp
 * @name Temperature
 * The temperature variable from a Freescale MPL115A2
 * - Range is -20°C to 85°C
 * - Accuracy is not specified on the sensor datasheet
 *
 * {{ @ref FreescaleMPL115A2_Temp::FreescaleMPL115A2_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define MPL115A2_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[0].
#define MPL115A2_TEMP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define MPL115A2_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define MPL115A2_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "FreescaleMPL115A2_Temp"
#define MPL115A2_TEMP_DEFAULT_CODE "FreescaleMPL115A2_Temp"
/**@}*/

/**
 * @anchor sensor_mpl115a2_pressure
 * @name Pressure
 * The pressure variable from a Freescale MPL115A2
 * - Range is 500-1150 hPa
 * - Accuracy ±10 hPa
 *
 * {{ @ref FreescaleMPL115A2_Pressure::FreescaleMPL115A2_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; pressure should have 2 -
/// resolution is 1.5 hPa.
#define MPL115A2_PRESSURE_RESOLUTION 2
/// @brief Sensor variable number; pressure is stored in sensorValues[1].
#define MPL115A2_PRESSURE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "atmosphericPressure"
#define MPL115A2_PRESSURE_VAR_NAME "atmosphericPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "kilopascal" (kPa)
#define MPL115A2_PRESSURE_UNIT_NAME "kilopascal"
/// @brief Default variable short code; "FreescaleMPL115A2_Pressure"
#define MPL115A2_PRESSURE_DEFAULT_CODE "FreescaleMPL115A2_Pressure"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Freescale Semiconductor MPL115A2 sensor](@ref sensor_mpl115a2).
 *
 * @ingroup sensor_mpl115a2
 */
/* clang-format on */
class FreescaleMPL115A2 : public Sensor {
 public:
    /**
     * @brief Construct a new FreescaleMPL115A2 using a secondary *hardware* I2C
     * instance.
     *
     * @note It is only possible to connect *one* MPL115A2 at a time on a single
     * I2C bus.  Software I2C is also not supported.
     *
     * @param theI2C A TwoWire instance for I2C communication.  Due to the
     * limitations of the Arduino core, only a hardware I2C instance can be
     * used.  For an AVR board, there is only one I2C instance possible and this
     * form of the constructor should not be used.  For a SAMD board, this can
     * be used if a secondary I2C port is created on one of the extra SERCOMs.
     * @param powerPin The pin on the mcu controlling power to the MPL115A2
     * Use -1 if it is continuously powered.
     * - The MPL115A2 requires a 2.375 - 5.5V power source
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    FreescaleMPL115A2(TwoWire* theI2C, int8_t powerPin,
                      uint8_t measurementsToAverage = 1);
    /**
     * @brief Construct a new FreescaleMPL115A2 using the primary hardware I2C
     * instance.
     *
     * @note It is only possible to connect *one* MPL115A2 at a time on a single
     * I2C bus.  Software I2C is also not supported.
     *
     * @param powerPin The pin on the mcu controlling power to the MPL115A2
     * Use -1 if it is continuously powered.
     * - The MPL115A2 requires a 2.375 - 5.5V power source
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    explicit FreescaleMPL115A2(int8_t  powerPin,
                               uint8_t measurementsToAverage = 1);
    /**
     * @brief Destroy the FreescaleMPL115A2 object
     */
    ~FreescaleMPL115A2();

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This begins the Wire library (sets pin modes for I2C), reads coefficients
     * from the MPL115A2, and updates the #_sensorStatus.  The sensor must be
     * powered for setup.  This doesn't return anything to indicate failure or
     * success, we just have to hope it worked.
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
     * @brief Private reference to the internal Adafruit_MPL115A2 object.
     */
    Adafruit_MPL115A2 mpl115a2_internal;
    /**
     * @brief An internal reference to the hardware Wire instance.
     */
    TwoWire* _i2c;
};

/**
 * @brief typedef for backwards compatibility; use the FreescaleMPL115A2 class
 * in new code
 *
 * @m_deprecated_since{0,33,0}
 */
typedef FreescaleMPL115A2 MPL115A2;


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_mpl115a2_temp) from a
 * [Freescale Semiconductor MPL115A2](@ref sensor_mpl115a2).
 *
 * @ingroup sensor_mpl115a2
 */
/* clang-format on */
class FreescaleMPL115A2_Temp : public Variable {
 public:
    /**
     * @brief Construct a new FreescaleMPL115A2_Temp object.
     *
     * @param parentSense The parent FreescaleMPL115A2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "FreescaleMPL115A2_Temp".
     */
    explicit FreescaleMPL115A2_Temp(
        FreescaleMPL115A2* parentSense, const char* uuid = "",
        const char* varCode = MPL115A2_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)MPL115A2_TEMP_VAR_NUM,
                   (uint8_t)MPL115A2_TEMP_RESOLUTION, MPL115A2_TEMP_VAR_NAME,
                   MPL115A2_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new FreescaleMPL115A2_Temp object.
     *
     * @note This must be tied with a parent FreescaleMPL115A2 before it can be
     * used.
     */
    FreescaleMPL115A2_Temp()
        : Variable((const uint8_t)MPL115A2_TEMP_VAR_NUM,
                   (uint8_t)MPL115A2_TEMP_RESOLUTION, MPL115A2_TEMP_VAR_NAME,
                   MPL115A2_TEMP_UNIT_NAME, MPL115A2_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the FreescaleMPL115A2_Temp object - no action needed.
     */
    ~FreescaleMPL115A2_Temp() {}
};

/**
 * @brief typedef for backwards compatibility; use the FreescaleMPL115A2_Temp
 * class in new code
 *
 * @m_deprecated_since{0,33,0}
 */
typedef FreescaleMPL115A2_Temp MPL115A2_Temp;


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [pressure output](@ref sensor_mpl115a2_pressure) from a
 * [Freescale Semiconductor MPL115A2](@ref sensor_mpl115a2).
 *
 * @ingroup sensor_mpl115a2
 */
/* clang-format on */
class FreescaleMPL115A2_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new FreescaleMPL115A2_Pressure object.
     *
     * @param parentSense The parent FreescaleMPL115A2 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "FreescaleMPL115A2_Pressure".
     */
    explicit FreescaleMPL115A2_Pressure(
        FreescaleMPL115A2* parentSense, const char* uuid = "",
        const char* varCode = MPL115A2_PRESSURE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)MPL115A2_PRESSURE_VAR_NUM,
                   (uint8_t)MPL115A2_PRESSURE_RESOLUTION,
                   MPL115A2_PRESSURE_VAR_NAME, MPL115A2_PRESSURE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new FreescaleMPL115A2_Pressure object.
     *
     * @note This must be tied with a parent FreescaleMPL115A2 before it can be
     * used.
     */
    FreescaleMPL115A2_Pressure()
        : Variable((const uint8_t)MPL115A2_PRESSURE_VAR_NUM,
                   (uint8_t)MPL115A2_PRESSURE_RESOLUTION,
                   MPL115A2_PRESSURE_VAR_NAME, MPL115A2_PRESSURE_UNIT_NAME,
                   MPL115A2_PRESSURE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the FreescaleMPL115A2_Pressure object - no action needed.
     */
    ~FreescaleMPL115A2_Pressure() {}
};

/**
 * @brief typedef for backwards compatibility; use the
 * FreescaleMPL115A2_Pressure class in new code
 *
 * @m_deprecated_since{0,33,0}
 */
typedef FreescaleMPL115A2_Pressure MPL115A2_Pressure;

/**@}*/
#endif  // SRC_SENSORS_FREESCALEMPL115A2_H_
