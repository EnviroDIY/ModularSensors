/**
 * @file BoschBMP3xx.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the BoschBMP3xx sensor subclass and the variable subclasses
 * BoschBMP3xx_Temp, BoschBMP3xx_Humidity, BoschBMP3xx_Pressure, and
 * BoschBMP3xx_Altitude.
 *
 * These are used for the Bosch BMP3xx digital pressure and humidity sensor.
 *
 * This depends on the [MartinL1's BMP388
 * library](https://github.com/MartinL1/BMP388_DEV).
 *
 * @copydetails BoschBMP3xx
 */
/* clang-format off */
/**
 * @defgroup sensor_bmp3xx Bosch BMP3xx
 * Classes for the Bosch BMP388 and BMP390 pressure sensors.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_bmp3xx_intro Introduction
 * > The BMP390 is a digital sensor with pressure and temperature measurement
 * > based on proven sensing principles. The sensor module is housed in an
 * > extremely compact 10-pin metal-lid LGA package with a footprint of only 2.0
 * > × 2.0 mm² and max 0.8 mm package height. Its small dimensions and its low
 * > power consumption of 3.2 μA @1Hz allow the implementation in battery driven
 * > devices such as mobile phones, GPS modules or watches.
 *
 * The BMP390 replaces the BMP388 and is nearly identical in specs and communication.
 *
 * Although this sensor has the option of either I2C or SPI communication, this
 * library only supports I2C.  The I2C address is either 0x77 or 0x76.  To
 * connect two of these sensors to your system, you must ensure they are
 * soldered so as to have different I2C addresses.  No more than two can be
 * attached.  These sensors should be attached to a 1.7-3.6V power source and
 * the power supply to the sensor can be stopped between measurements.
 *
 * [MartinL1's BMP388 library](https://github.com/MartinL1/BMP388_DEV) is used
 * internally for communication with the BMP3xx.  MartinL1's library was selected
 * over the [Adafruit library](https://github.com/adafruit/Adafruit_BMP3XX)
 * because it allows non-blocking operation in both normal and forced modes.
 * It also provides many enums to help ensure correct value inputs
 *
 * @warning The I2C addresses used by the BMP3xx are the same as those of the BME280
 * and the MS5803!  If you are also using one of those sensors, make sure that the
 * address for that sensor does not conflict with the address of this sensor.
 *
 * @note Neither secondary hardware nor software I2C is supported for the BMP3xx.
 * Only the primary hardware I2C defined in the Arduino core can be used.
 *
 * @section sensor_bmp3xx_filters Bosch Recommended Oversample and Filter Settings
 *
 * @subsection sensor_bmp3xx_pressure_osr Recommended Pressure Oversampling
 *
 * Recommended settings pressure oversampling (adapted from table 6 of the [datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BMP390-Datasheet.pdf))
 *
 * | Oversampling setting  | Pressure oversampling | Typical pressure resolution | Recommended temperature oversampling | Measurement Time (typ., µsec) |
 * | :-------------------: | :-------------------: | :-------------------------: | :----------------------------------: | :---------------------------: |
 * |    Ultra low power    |          ×1           |      16 bit / 2.64 Pa       |                  ×1                  |             6849              |
 * |       Low power       |          ×2           |      17 bit / 1.32 Pa       |                  ×1                  |             8869              |
 * |  Standard resolution  |          ×4           |      18 bit / 0.66 Pa       |                  ×1                  |             12909             |
 * |    High resolution    |          ×8           |      19 bit / 0.33 Pa       |                  ×1                  |             20989             |
 * | Ultra high resolution |          ×16          |      20 bit / 0.17 Pa       |                  ×2                  |             41189             |
 * |  Highest resolution   |          ×32          |      21 bit / 0.085 Pa      |                  ×2                  |             73509             |
 *
 * @subsection sensor_bmp3xx_temp_osr Recommended Temperature Oversampling
 *
 * From section 3.4.2 of the [datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BMP390-Datasheet.pdf):
 *
 * > It is recommended to base the value of *osr_t* [temperature oversampling]
 * > on the selected value of *osrs_p* [pressure oversampling] as per Table 6.
 * > Temperature oversampling above x2 is possible, but will not significantly
 * > improve the accuracy of the pressure output any further.  The reason for
 * > this is that the noise of the compensated pressure value depends more on
 * > the raw pressure than on the raw temperature noise.  Following the
 * > recommended setting will result in an optimal noise to power ratio.
 *
 * @subsection sensor_bmp3xx_filts_uses Settings by Use Case
 *
 * This is a modified version of Bosch's recommendations for pressure and temperature oversampling,
 * IIR filter coeficients, and output data rates for various applications.
 * This appears as table 10 in the
 * [datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BMP390-Datasheet.pdf).
 *
 * Table 10: Recommended filter settings based on use cases
 *
 * |                 Use case                 |  Mode  | Over-sampling setting | Pressure over-sampling | Temperature over-sampling | IIR filter coefficient | Standby Time (ms) | Output Data Rate (ODR) [Hz] | Current Consumption (I<sub>DD</sub>) [μA] | RMS Noise [cm] |
 * | :--------------------------------------: | :----: | :-------------------: | :--------------------: | :-----------------------: | :--------------------: | :---------------: | :-------------------------: | :---------------------------------------: | :------------: |
 * | handheld device low-power (e.g. Android) | Normal |    High resolution    |           x8           |            x1             |           2            |        80         |            12.5             |                    145                    |       11       |
 * |  handheld device dynamic (e.g. Android)  | Normal |  Standard resolution  |           x4           |            x1             |           4            |        20         |             50              |                    310                    |       10       |
 * |    Weather monitoring (lowest power)     | Forced |    Ultra low power    |           x1           |            x1             |          Off           |       N/A¹        |            1/60             |                     4                     |       55       |
 * |              Drop detection              | Normal |       Low power       |           x2           |            x1             |          Off           |        10         |             100             |                    358                    |       36       |
 * |            Indoor navigation             | Normal | Ultra high resolution |          x16           |            x2             |           4            |        40         |             25              |                    560                    |       5        |
 * |                  Drone                   | Normal |  Standard resolution  |           x8           |            x1             |           2            |        20         |             50              |                    570                    |       11       |
 * |           Indoor localization            | Normal |    Ultra low power    |           x1           |            x1             |           4            |        640        |              1              |                     -                     |       -        |
 * ¹ Standby time does not apply in forced mode
 *
 * @section sensor_bmp3xx_datasheet Sensor Datasheet
 * Documentation for the BMP390 sensor can be found at:
 * https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp390/
 *
 * - [BMP390 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BMP390-Datasheet.pdf)
 * - [BMP388 Datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BMP388-Datasheet.pdf)
 *
 * @section sensor_bmp3xx_flags Build flags
 * - ```-D SEALEVELPRESSURE_HPA```
 *      - use to adjust the sea level pressure used to calculate altitude from measured barometric pressure
 *      - if not defined, 1013.25 is used
 *      - The same sea level pressure flag is used for both the BMP3xx and the BME280.
 * Whatever you select will be used for both sensors.
 *
 * @section sensor_bmp3xx_ctor Sensor Constructors
 * {{ @ref BoschBMP3xx::BoschBMP3xx(int8_t, Mode, Oversampling, Oversampling, IIRFilter, TimeStandby, uint8_t) }}
 *
 * ___
 * @section sensor_bmp3xx_examples Example Code
 * The BMP3xx is used in the @menulink{bosch_bmp3xx} example.
 *
 * @menusnip{bosch_bmp3xx}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_BOSCHBMP3XX_H_
#define SRC_SENSORS_BOSCHBMP3XX_H_

// Debugging Statement
// #define MS_BOSCHBMP3XX_DEBUG

#ifdef MS_BOSCHBMP3XX_DEBUG
#define MS_DEBUGGING_STD "BoschBMP3xx"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"
#include <BMP388_DEV.h>

/** @ingroup sensor_bmp3xx */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the BMP3xx can report 3 values.
#define BMP3XX_NUM_VARIABLES 3
/// @brief Sensor::_incCalcValues; altitude is calculted within the Adafruit
/// library.
#define BMP3XX_INC_CALC_VARIABLES 1

/**
 * @anchor sensor_bmp3xx_timing
 * @name Sensor Timing
 * The sensor timing for a Bosch BMP3xx
 */
/**@{*/
/**
 * @brief Sensor::_warmUpTime_ms; BMP3xx should be ready to communicate within
 * 3ms.
 *
 * Time to first communication after both V<sub>DD</sub> > 1.8 V and
 * V<sub>DDIO</sub> > 1.8 V is 2ms (max) for the BMP390.  Power-on time from
 * stand-by mode is 3 ms (max) for the BMP390.  I don't understand why it takes
 * longer to be ready from stand-by than from power off, but we'll use the
 * larger number.
 */
#define BMP3XX_WARM_UP_TIME_MS 3
/**
 * @brief Sensor::_stabilizationTime_ms; BMP3xx is stable after 4000ms.
 *
 * 0.5 s for good numbers, but optimal at 4 s based on tests using
 * bmp3xxtimingTest.ino
 */
#define BMP3XX_STABILIZATION_TIME_MS 4000
/* clang-format off */
/**
 * @brief Sensor::_measurementTime_ms; The number given in this define will be
 * recalculated and over-written in the set-up.
 *
 * The BMP390 takes 78.09ms (max) to complete a measurement at 32x pressure
 * oversampling and 2x temperature oversampling.  A measurement may take up to
 * 138ms at 32x pressure and temperature oversampling, but oversampling rates
 * above 2x for temperature are not recommended.
 *
 * Following 3.9.2 of the datasheet:
 *
 * > In both forced mode and normal mode the pressure and temperature
 * > measurement duration follow the equation:
 * >
 * > \f[T_{conv} = 234 \mu s + pres\_en \times (392 \mu s + 2^{osr\_p} \times 2020 \mu s) + temp\_en \times (163 \mu s + 2^{osr\_t} \times 2020 \mu s)\f]
 * >
 * > With:
 * > - \f$T_{conv}\f$ = total conversion time in μs
 * > - \f$pres\_en\f$ = "0" or "1", depending of the status of the press_en bit
 * > - \f$temp\_en\f$ = "0" or "1", depending of the status of the temp_en bit
 * > - \f$osr\_p\f$ = amount of pressure oversampling repetitions
 * > - \f$osr\_t\f$ = amount of temperature oversampling repetitions
 *
 *
 * Further, based on table 23 in the datasheet, there is up to a 18% difference
 * between the "typical" measurement time (as given by the equation) and the
 * maximum measurement time.
 *
 * ModularSensors will always enable both pressure and temperature measurement
 * and add an extra 18% wait to the calculated measurement time.
 */
#define BMP3XX_MEASUREMENT_TIME_MS 80
/* clang-format on */
/**@}*/

/**
 * @anchor sensor_bmp3xx_temp
 * @name Temperature
 * The temperature variable from a Bosch BMP388 or BMP390
 * - Range is -40°C to +85°C
 *   - Full accuracy between 0°C and +65°C
 * - Absolute accuracy is typ. ± 0.5°C at 25°C
 *   - ± 1.5°C over 0°C to +65°C range
 *
 * {{ @ref BoschBMP3xx_Temp::BoschBMP3xx_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 5 -
/// resolution is 0.0.00015°C at the hightest oversampling.  See table 7 in the
/// [sensor
/// datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BMP390-Datasheet.pdf)
/// for resolution at all bandwidths.
#define BMP3XX_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[0].
#define BMP3XX_TEMP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define BMP3XX_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define BMP3XX_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "BoschBMP3xxTemp"
#define BMP3XX_TEMP_DEFAULT_CODE "BoschBMP3xxTemp"
/**@}*/

/**
 * @anchor sensor_bmp3xx_pressure
 * @name Barometric Pressure
 * The barometric pressure variable from a Bosch BMP388 or BMP390
 * - Range for both the BMP388 and BMP390 is 300‒1250 hPa
 * - Absolute accuracy is typ. ± 50 Pa (±0.50 hPa)
 * - Relative accuracy is typ. ± 3 Pa (±0.03 hPa), equiv. to ± 0.25 m
 *
 * {{ @ref BoschBMP3xx_Pressure::BoschBMP3xx_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; barometric pressure should
/// have 3.  Resolution of output data in highest resolution mode at lowest
/// bandwidth is 0.016 Pa.  See table 6 in the [sensor
/// datasheet](https://github.com/EnviroDIY/ModularSensors/wiki/Sensor-Datasheets/Bosch-BMP390-Datasheet.pdf)
/// for resolution at all bandwidths.
#define BMP3XX_PRESSURE_RESOLUTION 3
/// @brief Sensor variable number; pressure is stored in sensorValues[2].
#define BMP3XX_PRESSURE_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "barometricPressure"
#define BMP3XX_PRESSURE_VAR_NAME "barometricPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "pascal"
/// (Pa)
#define BMP3XX_PRESSURE_UNIT_NAME "pascal"
/// @brief Default variable short code; "BoschBMP3xxPressure"
#define BMP3XX_PRESSURE_DEFAULT_CODE "BoschBMP3xxPressure"
/**@}*/

/**
 * @anchor sensor_bmp3xx_altitude
 * @name Altitude
 * The altitude variable from a Bosch BMP388 or BMP390
 *
 * {{ @ref BoschBMP3xx_Altitude::BoschBMP3xx_Altitude }}
 */
/**@{*/
/// @brief Decimals places in string representation; altitude should have 0 -
/// resolution is 1m.
#define BMP3XX_ALTITUDE_RESOLUTION 0
/// @brief Sensor variable number; altitude is stored in sensorValues[3].
#define BMP3XX_ALTITUDE_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "heightAboveSeaFloor"
#define BMP3XX_ALTITUDE_VAR_NAME "heightAboveSeaFloor"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "meter"
#define BMP3XX_ALTITUDE_UNIT_NAME "meter"
/// @brief Default variable short code; "BoschBMP3xxAltitude"
#define BMP3XX_ALTITUDE_DEFAULT_CODE "BoschBMP3xxAltitude"
/**@}*/

/// The atmospheric pressure at sea level
#ifndef SEALEVELPRESSURE_HPA
#define SEALEVELPRESSURE_HPA (1013.25)
#endif

/* clang-format off */
/**
 * @brief The Sensor sub-class for the [Bosch BMP3xx](@ref sensor_bmp3xx).
 *
 * @ingroup sensor_bmp3xx
 */
/* clang-format on */
class BoschBMP3xx : public Sensor {
 public:
    /**
     * @brief Construct a new Bosch BMP3xx object using the primary hardware I2C
     * instance.
     *
     * @note Neither secondary hardware nor software I2C is supported for the
     * BMP3xx. Only the primary hardware I2C defined in the Arduino core can be
     * used.
     *
     * @param powerPin The pin on the mcu controlling power to the BMP3XX
     * Use -1 if it is continuously powered.
     * - The BMP3xx requires a 1.7 - 3.6V power source
     * @param mode Data sampling mode
     * <br>Possible values are:
     * - `FORCED_MODE` - a single measurement is made upon request and the
     * sensor immediately returns to sleep.  This mode should be used if you are
     * stopping power to the sensor between readings.  You should not use this
     * mode if you wish to use the sensor's on-board IIR filter.
     * - `NORMAL_MODE` - the sensor alteranates between sampling and sleeping at
     * intervals set by the sensor output data rate, results can be read
     * whenever needed.  This mode should *not* be used if you will stop power
     * to the sensor between readings.  If you wish to use the sensor's on-board
     * filtering, you should use normal mode.
     *
     * @param pressureOversample Pressure oversampling setting
     * <br>Possible values are:
     * - `OVERSAMPLING_SKIP`
     * - `OVERSAMPLING_X2`
     * - `OVERSAMPLING_X4`,
     * - `OVERSAMPLING_X8`
     * - `OVERSAMPLING_X16`,
     * - `OVERSAMPLING_X32`
     *
     * @param tempOversample Temperature oversampling setting
     * <br>Possible values are the same as those for pressureOversample.  Using
     * temperature oversampling above X2 is not recommended as it does not
     * further improve pressure data quality.
     *
     * @param filterCoeff Coefficient of the infinite impulse response (IIR)
     * filter (in samples).
     * <br>This is number of past samples considered in calculating the current
     * filtered value.  This setting is **ignored** if the sensor will not be
     * continuously powered.  This only recommended when operating in "normal"
     * sampling mode.
     * <br>Possible values are:
     * - `IIR_FILTER_OFF` (no filtering)
     * - `IIR_FILTER_1`,
     * - `IIR_FILTER_3`
     * - `IIR_FILTER_7`,
     * - `IIR_FILTER_15`
     * - `IIR_FILTER_31`,
     * - `IIR_FILTER_63`
     * - `IIR_FILTER_127`
     *
     * @param timeStandby Standby time between measurements when continuously
     * powered and operating in normal mode.
     * <br>This is the inverse of the output data rate (ODR).
     * <br>This setting is **ignored** when operating in forced mode.
     * <br>Possible values are:
     * - `TIME_STANDBY_5MS` (ODR = 200 Hz)
     * - `TIME_STANDBY_10MS` (ODR = 100 Hz)
     * - `TIME_STANDBY_20MS` (ODR = 50 Hz)
     * - `TIME_STANDBY_40MS` (ODR = 25 Hz)
     * - `TIME_STANDBY_80MS` (ODR = 12.5 Hz)
     * - `TIME_STANDBY_160MS` (ODR = 6.25 Hz)
     * - `TIME_STANDBY_320MS` (ODR = 3.125 Hz)
     * - `TIME_STANDBY_640MS` (ODR = 1.5 Hz)
     * - `TIME_STANDBY_1280MS` (~1.2 seconds, ODR = 0.78 Hz)
     * - `TIME_STANDBY_2560MS` (~2.5 seconds, ODR = 0.39 Hz)
     * - `TIME_STANDBY_5120MS` (~5 seconds, ODR = 0.2 Hz)
     * - `TIME_STANDBY_10240MS` (~10 seconds, ODR = 0.1 Hz)
     * - `TIME_STANDBY_20480MS` (~20 seconds, ODR = 0.05 Hz)
     * - `TIME_STANDBY_40960MS` (~41 seconds, ODR = 0.025 Hz)
     * - `TIME_STANDBY_81920MS` (~82 seconds or 1.4 minutes, ODR = 0.0125 Hz)
     * - `TIME_STANDBY_163840MS` (~164 seconds or 2.7 minutes, ODR = 0.006 Hz)
     * - `TIME_STANDBY_327680MS` (~5.5 minutes, ODR = 0.003 Hz)
     * - `TIME_STANDBY_655360MS` (~11 minutes, ODR = 0.0015 Hz)
     *
     * @note If you are logging data, make sure that your logging interval is
     * **greater than** the length of the IIR filter multiplied by the standby
     * time!
     *
     * @param i2cAddressHex The I2C address of the BMP3xx; must be either 0x76
     * or 0x77.  The default value is 0x76.
     *
     * @note For the BoschBMP3xx we do _**NOT**_ provide a
     * `measurementsToAverage` option.  The sensor already provides on-board
     * averaging by way of oversampling and the IIR filter, so there is no
     * reason to average again on our part.
     *
     * @see @ref sensor_bmp3xx_pressure_osr, @ref sensor_bmp3xx_temp_osr, and
     * @ref sensor_bmp3xx_filts_uses for recommended settings
     */
    explicit BoschBMP3xx(int8_t powerPin, Mode mode = FORCED_MODE,
                         Oversampling pressureOversample = OVERSAMPLING_X16,
                         Oversampling tempOversample     = OVERSAMPLING_X2,
                         IIRFilter    filterCoeff        = IIR_FILTER_OFF,
                         TimeStandby  timeStandby        = TIME_STANDBY_10MS,
                         uint8_t      i2cAddressHex      = 0x76);
    /**
     * @brief Destroy the Bosch BMP3xx object
     */
    ~BoschBMP3xx();

    /**
     * @brief Wake the sensor up, if necessary.  Do whatever it takes to get a
     * sensor in the proper state to begin a measurement.
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
     * This begins the Wire library (sets pin modes for I2C), reads
     * calibration coefficients from the BMP3xx, and updates the #_sensorStatus.
     * The BMP3xx must be powered for setup.
     *
     * @return **bool** True if the setup was successful.
     */
    bool setup(void) override;
    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @copydoc Sensor::startSingleMeasurement()
     */
    bool startSingleMeasurement(void) override;
    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    /**
     * @brief Internal reference the the BMP388_DEV object
     */
    BMP388_DEV bmp_internal;

    /**
     * @brief Data sampling mode
     *
     * Possible values are:
     * - `FORCED_MODE` - a single measurement is made upon request and the
     * sensor immediately returns to sleep.  This mode should be used if you are
     * stopping power to the sensor between readings.  You should not use this
     * mode if you wish to use the sensor's on-board IIR filter.
     * - `NORMAL_MODE` - the sensor alteranates between sampling and sleeping at
     * intervals set by the sensor output data rate, results can be read
     * whenever needed.  This mode should *not* be used if you will stop power
     * to the sensor between readings.  If you wish to use the sensor's on-board
     * filtering, you should use normal mode.
     *
     * @see @ref sensor_bmp3xx_filts_uses
     */
    Mode _mode;

    /**
     * @brief Pressure oversampling setting
     *
     * Possible values are:
     * - `OVERSAMPLING_SKIP`
     * - `OVERSAMPLING_X2`
     * - `OVERSAMPLING_X4`,
     * - `OVERSAMPLING_X8`
     * - `OVERSAMPLING_X16`,
     * - `OVERSAMPLING_X32`
     *
     * @see @ref sensor_bmp3xx_pressure_osr and @ref sensor_bmp3xx_filts_uses
     * for recommended settings
     */
    Oversampling _pressureOversampleEnum;

    /**
     * @brief Temperature oversampling setting
     *
     * Possible values are the same as those for pressureOversample.
     *
     * @see @ref sensor_bmp3xx_temp_osr and @ref sensor_bmp3xx_filts_uses
     */
    Oversampling _tempOversampleEnum;

    /**
     * @brief Coefficient of the infinite impulse response (IIR)
     * filter (in samples).
     *
     * The number of past samples considered in calculating the current filtered
     * value.  This only should be used when the sensor is in normal sampling
     * mode and continuously powered.
     *
     * Possible values are:
     * - `IIR_FILTER_OFF` (no filtering)
     * - `IIR_FILTER_1`,
     * - `IIR_FILTER_3`
     * - `IIR_FILTER_7`,
     * - `IIR_FILTER_15`
     * - `IIR_FILTER_31`,
     * - `IIR_FILTER_63`
     * - `IIR_FILTER_127`
     *
     * @see @ref sensor_bmp3xx_filts_uses for recommended settings
     */
    IIRFilter _filterCoeffEnum;

    /**
     * @brief Standby time between measurements when continuously
     * powered and operating in normal mode.
     *
     * This is the inverse of the output data rate (ODR).
     *
     * This setting is **ignored** when operating in forced mode.
     *
     * @note If you are logging data, make sure that your logging interval is
     * **greater than** the length of the IIR filter multiplied by the standby
     * time!
     *
     * Possible values are:
     * - `TIME_STANDBY_5MS` (ODR = 200 Hz)
     * - `TIME_STANDBY_10MS` (ODR = 100 Hz)
     * - `TIME_STANDBY_20MS` (ODR = 50 Hz)
     * - `TIME_STANDBY_40MS` (ODR = 25 Hz)
     * - `TIME_STANDBY_80MS` (ODR = 12.5 Hz)
     * - `TIME_STANDBY_160MS` (ODR = 6.25 Hz)
     * - `TIME_STANDBY_320MS` (ODR = 3.125 Hz)
     * - `TIME_STANDBY_640MS` (ODR = 1.5 Hz)
     * - `TIME_STANDBY_1280MS` (~1.2 seconds, ODR = 0.78 Hz)
     * - `TIME_STANDBY_2560MS` (~2.5 seconds, ODR = 0.39 Hz)
     * - `TIME_STANDBY_5120MS` (~5 seconds, ODR = 0.2 Hz)
     * - `TIME_STANDBY_10240MS` (~10 seconds, ODR = 0.1 Hz)
     * - `TIME_STANDBY_20480MS` (~20 seconds, ODR = 0.05 Hz)
     * - `TIME_STANDBY_40960MS` (~41 seconds, ODR = 0.025 Hz)
     * - `TIME_STANDBY_81920MS` (~82 seconds or 1.4 minutes, ODR = 0.0125 Hz)
     * - `TIME_STANDBY_163840MS` (~164 seconds or 2.7 minutes, ODR = 0.006 Hz)
     * - `TIME_STANDBY_327680MS` (~5.5 minutes, ODR = 0.003 Hz)
     * - `TIME_STANDBY_655360MS` (~10 minutes, ODR = 0.0015 Hz)
     *
     * @see @ref sensor_bmp3xx_filts_uses for recommended settings
     */
    TimeStandby _standbyEnum;
    /**
     * @brief The I2C address of the BMP3xx
     */
    uint8_t _i2cAddressHex;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_bmp3xx_temp)
 * from a [Bosch BMP3xx](@ref sensor_bmp3xx).
 *
 * @ingroup sensor_bmp3xx
 */
/* clang-format on */
class BoschBMP3xx_Temp : public Variable {
 public:
    /**
     * @brief Construct a new BoschBMP3xx_Temp object.
     *
     * @param parentSense The parent BoschBMP3xx providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoschBMP3xxTemp".
     */
    explicit BoschBMP3xx_Temp(BoschBMP3xx* parentSense, const char* uuid = "",
                              const char* varCode = BMP3XX_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BMP3XX_TEMP_VAR_NUM,
                   (uint8_t)BMP3XX_TEMP_RESOLUTION, BMP3XX_TEMP_VAR_NAME,
                   BMP3XX_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new BoschBMP3xx_Temp object.
     *
     * @note This must be tied with a parent BoschBMP3xx before it can be used.
     */
    BoschBMP3xx_Temp()
        : Variable((const uint8_t)BMP3XX_TEMP_VAR_NUM,
                   (uint8_t)BMP3XX_TEMP_RESOLUTION, BMP3XX_TEMP_VAR_NAME,
                   BMP3XX_TEMP_UNIT_NAME, BMP3XX_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the BoschBMP3xx_Temp object - no action needed.
     */
    ~BoschBMP3xx_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [atmospheric pressure output](@ref sensor_bmp3xx_pressure) from a
 * [Bosch BMP3xx](@ref sensor_bmp3xx).
 *
 * @ingroup sensor_bmp3xx
 */
/* clang-format on */
class BoschBMP3xx_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new BoschBMP3xx_Pressure object.
     *
     * @param parentSense The parent BoschBMP3xx providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoschBMP3xxPressure".
     */
    explicit BoschBMP3xx_Pressure(
        BoschBMP3xx* parentSense, const char* uuid = "",
        const char* varCode = BMP3XX_PRESSURE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BMP3XX_PRESSURE_VAR_NUM,
                   (uint8_t)BMP3XX_PRESSURE_RESOLUTION,
                   BMP3XX_PRESSURE_VAR_NAME, BMP3XX_PRESSURE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new BoschBMP3xx_Pressure object.
     *
     * @note This must be tied with a parent BoschBMP3xx before it can be used.
     */
    BoschBMP3xx_Pressure()
        : Variable((const uint8_t)BMP3XX_PRESSURE_VAR_NUM,
                   (uint8_t)BMP3XX_PRESSURE_RESOLUTION,
                   BMP3XX_PRESSURE_VAR_NAME, BMP3XX_PRESSURE_UNIT_NAME,
                   BMP3XX_PRESSURE_DEFAULT_CODE) {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [altitude](@ref sensor_bmp3xx_altitude) calculated from the measurements
 * made by a [Bosch BMP3xx](@ref sensor_bmp3xx).
 *
 * @ingroup sensor_bmp3xx
 */
/* clang-format on */
class BoschBMP3xx_Altitude : public Variable {
 public:
    /**
     * @brief Construct a new BoschBMP3xx_Altitude object.
     *
     * @param parentSense The parent BoschBMP3xx providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "BoschBMP3xxAltitude".
     */
    explicit BoschBMP3xx_Altitude(
        BoschBMP3xx* parentSense, const char* uuid = "",
        const char* varCode = BMP3XX_ALTITUDE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)BMP3XX_ALTITUDE_VAR_NUM,
                   (uint8_t)BMP3XX_ALTITUDE_RESOLUTION,
                   BMP3XX_ALTITUDE_VAR_NAME, BMP3XX_ALTITUDE_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new BoschBMP3xx_Altitude object.
     *
     * @note This must be tied with a parent BoschBMP3xx before it can be used.
     */
    BoschBMP3xx_Altitude()
        : Variable((const uint8_t)BMP3XX_ALTITUDE_VAR_NUM,
                   (uint8_t)BMP3XX_ALTITUDE_RESOLUTION,
                   BMP3XX_ALTITUDE_VAR_NAME, BMP3XX_ALTITUDE_UNIT_NAME,
                   BMP3XX_ALTITUDE_DEFAULT_CODE) {}
};
/**@}*/
#endif  // SRC_SENSORS_BOSCHBMP3XX_H_
