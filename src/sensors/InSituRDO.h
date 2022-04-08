/**
 * @file InSituRDO.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the InSituRDO sensor subclass and the variable subclasses
 * InSituRDO_Temp, InSituRDO_DOpct, and InSituRDO_DOmgL.
 *
 * These are for the In-Situ RDO PRO-X digital dissolved oxygen sensor.
 *
 * This depends on the SDI12Sensors parent class.
 */
/* clang-format off */
/**
 * @defgroup sensor_insitu_rdo In-Situ RDO PRO-X
 * Classes for the InSitue Optical RDO PRO-X process optical dissolved oxygen probe
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_insitu_rdo_intro Introduction
 *
 * > The RDO PRO-X Dissolved Oxygen Probe uses optical technology
 * > (EPA-approved RDO technology for Clean Water Act programs) to measure
 * > dissolved oxygen and temperature in demanding process environments.
 *
 * The RDO PRO-X requires a 9.6 - 16 VDC power supply when operating in SDI-12 mode,
 * which is what this library uses.
 * The maximum power consumption is 50 mA at 12 VDC.
 * Measurement current is 6 mA typical at 24 VDC and idle current is 160 µA typical at 24 VDC.
 *
 * The [In-Situ RDO PRO-X](https://in-situ.com/us/rdo-pro-x-dissolved-oxygen-probe)
 * sensor is implemented as a sub-classes of the SDI12Sensors class.
 * While the sensor can also communicate over Modbus/RS485 or a 4-20 mA loop,
 * in this case I've chosen to use SDI-12 to minimize the number of pins
 * required for communication and because most Arduino style processors have
 * very few possible COM ports available for RS485.
 *
 * @note The default SDI-12 address of the RDO PRO-X is "0".
 *
 * @note The default connection settings for ModbusRTU are 19200 baud, 8E1.
 * The default modbus address is 0x01.
 *
 * @warning The RDO PRO-X is not properly compliant with the SDI-12 protocol
 * and will not continue a concurrent measurement if there are any other SDI-12
 * sensors on the same SDI-12 bus.  This means that the RDO PRO-X must either be
 * put on a separate SDI-12 data pin than any other SDI-12 sensors or you must
 * compile with the build flag `-D MS_SDI12_NON_CONCURRENT`.
 *
 * @section sensor_insitu_rdo_Win-Situ Setup with Win-Situ
 *
 * The RDO PRO-X arrives from the factory ready to take measurements.
 * It is *NOT* necessary to connect it to a computer or use the Win-Situ software
 * to do any additional setup.  If you are able to, however, I recommend using
 * Win-Situ to change some of the default settings on the sensor.
 *
 * To connect the sensor to Win-Situ, you can use any form of RS485 to USB interface.
 * In-Situ sells one that specifially works with their cables, but any
 * inexpensive converter adapter with automatic flow control should work.
 * The sensor must be powered at a minimum of **12V** (12-36V) to use the
 * RS485/modbus interface.  This is different than the 9.6V - 16V required for
 * the SDI-12 interface.
 *
 * These are the two settings I recommend changing with Win-Situ:
 *
 * 1.  Disable caching:
 *   - By default, the RDO PRO-X "caches" readings for 5000ms (5s) and will
 * not take a new measurement until the 5s cache expires.  If you want to take
 * measurements at faster than 5s intervals (ie, to average multiple
 * measurements), I strongly recommend setting the cache value to 0ms using the
 * Win-Situ software.  The cache value can be changed in the "Diagnostics" menu
 * found on the "Device Setup" tab of Win-Situ.
 *
 * 2. Enable O2 partial pressure output:
 *   - By default, the partial pressure of oxygen is *not* returned over the
 * SDI-12 interface.  It can be enabled by way of the "SDI-12 Setup..." menu
 * found on the "Device Setup" tab of Win-Situ.
 *
 * @warning The order of variables returned on the SDI-12 bus from the RDO PRO-X
 * can be changed using the Win-Situ software.  This library assumes the default
 * ordering of variables: DO concentration, DO % saturation, Temperature.  If
 * added manually, O2 partial pressure is last.  **If you change reorder the
 * variables with Win-Situ, your results will be labeled incorrectly!**
 *
 * @section sensor_insitu_rdo_calib Calibration
 * This library DOES NOT support calibrating the DO probe.  Per the manufacturer:
 * > This durable probe requires no calibration or conditioning prior to deployment and does not use membranes.
 * > The replaceable RDO Smart Sensor Cap stores calibration coefficients for automatic, error-free setup.
 *
 * > The optical Rugged Dissolved Oxygen sensor is very stable.
 * > The factory calibration should produce readings within 3% accuracy.
 * > If you require readings with greater accuracy we recommend that you perform a 1-point,
 * > 100% water-saturated air calibration.
 *
 * > We recommend that you perform the [two-point 100% and] 0% oxygen calibration
 * > only if you intend to measure dissolved oxygen at a concentration of less than 4mg/L.
 *
 * If you do wish to re-calibrate the probe, that must be done using Win-Situ 5
 * software, the VuSitu mobile app or using the raw Modbus/RS485 commands. Using
 * the Win-Situ software is by far the easiest way.  With the Win-Situ software
 * and the calibration "chamber" shipped with the sensor, performing a 1-point
 * 100% water-saturated air calibration is fairly simple and straight forward.
 *
 * @section sensor_insitu_rdo_cap Sensor Cap Maintenance
 * The sensor cap has an estimated lifetime of 2 years from the time of first reading.
 * To preserve its lifespan, the sensor cap should not be opened or removed from its
 * packaging until the sensor is ready to be deployed.
 *
 * @subsection sensor_insitu_rdo_cap2 To clean the sensor cap:
 * > 1. The cap and nose cone must remain on the probe.
 * > 2. Rinse the sensor with clean water from a squirt bottle or spray bottle.
 * > 3. Gently wipe with a soft-bristled brush or soft cloth if biofouling is present.
 * >    Use Alconox to remove grease.
 * > 4. If extensive fouling or mineral build-up is present, soak the cap end in vinegar for 15 minutes,
 * >    then soak in deionized (DI) water for 15 minutes.
 *
 * The optical window should only be cleaned when the sensor cap is changed.
 *
 * InSitu recommends using an antifouling guard or airblast adapter to extend deployments and protect your data.
 *
 * @section sensor_insitu_rdo_warnings Deployment Notes and Warnings
 * @warning The inside of the controller must be kept free of moisture and humidity.
 * Condensed moisture can migrate through the wiring and cause the probe to fail.
 * Therefore, desiccant should be installed in the controller and be replaced on a regular basis.
 * @warning It is extremely important to use the proper size desiccant for your
 * deployment and to change desiccant often.  Desiccant should be changed
 * before the entire pack has turned pink, and you should use enough to
 * effectively keep cables and probes dry until your next scheduled maintenance.
 * Desiccant lifespan is dependent on site conditions.
 *
 * @section sensor_insitu_rdo_datasheet Sensor Datasheet
 * The manual for this sensor is available at:
 * https://in-situ.com/pub/media/support/documents/RDO_Pro-X_Manual.pdf
 *
 * The spec sheet is here:  https://in-situ.com/pub/media/support/documents/RDO-PRO-X_SS.pdf
 *
 * @section sensor_insitu_rdo_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_insitu_rdo_ctor Sensor Constructor
 * {{ @ref InSituRDO::InSituRDO }}
 *
 * ___
 * @section sensor_insitu_rdo_examples Example Code
 * The In-Situ RDO PRO-X is used in the @menulink{in_situ_rdo} example.
 *
 * @menusnip{in_situ_rdo}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_INSITURDO_H_
#define SRC_SENSORS_INSITURDO_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
/** @ingroup sensor_insitu_rdo */
/**@{*/

/**
 * @brief Sensor::_numReturnedValues; the RDO PRO-X can report 4 values.
 *
 * It reports 3 values (DO concentration, DO % saturation, and temperature) by
 * default.  Partial pressure of oxygen can be added to the output using
 * Win-Situ software.
 */
#define INSITU_RDO_NUM_VARIABLES 4
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define INSITU_RDO_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_insitu_rdo_timing
 * @name Sensor Timing
 * The sensor timing for an In-Situ RDO PRO-X.
 *
 * None of these values are specified in the documentation for the sensor; this
 * is all based on SRGD's testing.
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; ~125ms in SRGD tests
#define INSITU_RDO_WARM_UP_TIME_MS 125
/**
 * @brief Sensor::_stabilizationTime_ms; the sensor is stable as soon as it
 * can take a measurement.
 *
 * @note The RDO takes 5-10 minutes to stabilize after being put in a new
 * environment but once the sensor has equilibrated to the environment it
 * does not appear to need any additional stabilization time when it is
 * powered on and off.
 */
#define INSITU_RDO_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; ~775 minimum
#define INSITU_RDO_MEASUREMENT_TIME_MS 850
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  We give the RDO an extra 10ms.
#define INSITU_RDO_EXTRA_WAKE_TIME_MS 0
/**@}*/

/**
 * @anchor sensor_insitu_rdo_domgl
 * @name Dissolved Oxygen Concentration
 * The DO concentration variable from an In-Situ RDO PRO-X
 * - Range is 0 to 50 mg/L concentration
 * - Accuracy:
 *   - ± 0.1 mg/L from 0 to 8 mg/L
 *   - ± 0.2 mg/L of reading from 8-20 mg/L
 *   - ± 10% of reading from 20-50 mg/L
 *
 * @note To acheive maximum accuracy, the sensor must be calibrated using either
 * a one or two point calibration.
 *
 * {{ @ref InSituRDO_DOmgL::InSituRDO_DOmgL }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; dissolved oxygen
 * concentration should have 2 - resolution is 0.01 mg/L.
 *
 * Contrary to the spec sheet, the actual resolution returned by the sensor in
 * SDI-12 mode is 0.000001 mg/L.  Since the accuracy is only 0.1 mg/L at best,
 * we will keep only the two digits suggested by the spec sheet.
 */
#define INSITU_RDO_DOMGL_RESOLUTION 2
/// @brief Variable number; dissolved oxygen concentration is stored in
/// sensorValues[0]
#define INSITU_RDO_DOMGL_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "oxygenDissolved"
#define INSITU_RDO_DOMGL_VAR_NAME "oxygenDissolved"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "milligramPerLiter" (mg/L)
#define INSITU_RDO_DOMGL_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "RDOppm"
#define INSITU_RDO_DOMGL_DEFAULT_CODE "RDOppm"
/**@}*/

/**
 * @anchor sensor_insitu_rdo_dopercent
 * @name Dissolved Oxygen Percent Saturation
 * The percent saturation variable from an In-Situ RDO PRO-X
 * - Range is 0 to 50 mg/L concentration
 * - Accuracy:
 *   - ± 0.1 mg/L from 0 to 8 mg/L
 *   - ± 0.2 mg/L of reading from 8-20 mg/L
 *   - ± 10% of reading from 20-50 mg/L
 * @note To acheive maximum accuracy, the sensor must be calibrated using either
 * a one or two point calibration.
 *
 * {{ @ref InSituRDO_DOpct::InSituRDO_DOpct }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; dissolved oxygen percent
 * saturation should have 1.
 *
 * The actual resolution returned by the sensor in SDI-12 mode is 0.00001 %.
 * Since the accuracy is much less than that, we'll ignore the not-significant
 * figures.
 */
#define INSITU_RDO_DOPCT_RESOLUTION 2
/// @brief Variable number; dissolved oxygen percent is stored in
/// sensorValues[1]
#define INSITU_RDO_DOPCT_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "oxygenDissolvedPercentOfSaturation"
#define INSITU_RDO_DOPCT_VAR_NAME "oxygenDissolvedPercentOfSaturation"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "percent"
/// (% saturation)
#define INSITU_RDO_DOPCT_UNIT_NAME "percent"
/// @brief Default variable short code; "RDOpercent"
#define INSITU_RDO_DOPCT_DEFAULT_CODE "RDOpercent"
/**@}*/

/**
 * @anchor sensor_insitu_rdo_temp
 * @name Temperature
 * The temperature variable from an In-Situ RDO PRO-X
 * - Range is 0° to 50°C (32° to 122°F)
 * - Accuracy is ± 0.1°C typical
 *
 * {{ @ref InSituRDO_Temp::InSituRDO_Temp }}
 */
/**@{*/
/// @brief
/**
 * @brief Decimals places in string representation; temperature should have 2 -
 * resolution is 0.01°C.
 *
 * The spec sheet lists 2 decimal resolution, but the returned value has 5.
 * Since the accuracy is less than either, we keep the two mentioned on the spec
 * sheet.
 */
#define INSITU_RDO_TEMP_RESOLUTION 2
/// @brief Variable number; temperature is stored in sensorValues[2].
#define INSITU_RDO_TEMP_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define INSITU_RDO_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define INSITU_RDO_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "RDOtempC"
#define INSITU_RDO_TEMP_DEFAULT_CODE "RDOtempC"
/**@}*/

/**
 * @anchor sensor_insitu_rdo_pressure
 * @name Oxygen Partial Pressure
 * The oxygen partial pressure variable from an In-Situ RDO PRO-X
 *
 * @note The oxygen partial pressure output must be manually enabled in SDI-12
 * mode using the Win-Situ software.
 *
 * {{ @ref InSituRDO_Pressure::InSituRDO_Pressure }}
 */
/**@{*/
/// @brief Decimals places in string representation; pressure should have 3
#define INSITU_RDO_PRESSURE_RESOLUTION 2
/// @brief Variable number; temperature is stored in sensorValues[3].
#define INSITU_RDO_PRESSURE_VAR_NUM 3
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "vaporPressure"
#define INSITU_RDO_PRESSURE_VAR_NAME "vaporPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/); "torr"
#define INSITU_RDO_PRESSURE_UNIT_NAME "torr"
/// @brief Default variable short code; "RDOppO2"
#define INSITU_RDO_PRESSURE_DEFAULT_CODE "RDOppO2"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [In-Situ RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
 *
 * @ingroup sensor_insitu_rdo
 */
/* clang-format on */
class InSituRDO : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new In-Situ RDO object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the RDO; can be a char,
     * char*, or int.
     * @param powerPin The pin on the mcu controlling power to the RDO PRO-X
     * Use -1 if it is continuously powered.
     * - The RDO PRO-X requires a 8-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    InSituRDO(char SDI12address, int8_t powerPin, int8_t dataPin,
              uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "In-Situ RDO PRO-X", INSITU_RDO_NUM_VARIABLES,
              INSITU_RDO_WARM_UP_TIME_MS, INSITU_RDO_STABILIZATION_TIME_MS,
              INSITU_RDO_MEASUREMENT_TIME_MS, INSITU_RDO_EXTRA_WAKE_TIME_MS,
              INSITU_RDO_INC_CALC_VARIABLES) {}
    /**
     * @copydoc InSituRDO::InSituRDO
     */
    InSituRDO(char* SDI12address, int8_t powerPin, int8_t dataPin,
              uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "In-Situ RDO PRO-X", INSITU_RDO_NUM_VARIABLES,
              INSITU_RDO_WARM_UP_TIME_MS, INSITU_RDO_STABILIZATION_TIME_MS,
              INSITU_RDO_MEASUREMENT_TIME_MS, INSITU_RDO_EXTRA_WAKE_TIME_MS,
              INSITU_RDO_INC_CALC_VARIABLES) {}
    /**
     * @copydoc InSituRDO::InSituRDO
     */
    InSituRDO(int SDI12address, int8_t powerPin, int8_t dataPin,
              uint8_t measurementsToAverage = 1)
        : SDI12Sensors(
              SDI12address, powerPin, dataPin, measurementsToAverage,
              "In-Situ RDO PRO-X", INSITU_RDO_NUM_VARIABLES,
              INSITU_RDO_WARM_UP_TIME_MS, INSITU_RDO_STABILIZATION_TIME_MS,
              INSITU_RDO_MEASUREMENT_TIME_MS, INSITU_RDO_EXTRA_WAKE_TIME_MS,
              INSITU_RDO_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the In-Situ RDO object
     */
    ~InSituRDO() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen concentration output](@ref sensor_insitu_rdo_domgl) from a
 * [In-Situ RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
 *
 * @ingroup sensor_insitu_rdo
 */
/* clang-format on */
class InSituRDO_DOmgL : public Variable {
 public:
    /**
     * @brief Construct a new InSituRDO_DOmgL object.
     *
     * @param parentSense The parent InSituRDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RDOppm".
     */
    explicit InSituRDO_DOmgL(
        InSituRDO* parentSense, const char* uuid = "",
        const char* varCode = INSITU_RDO_DOMGL_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)INSITU_RDO_DOMGL_VAR_NUM,
                   (uint8_t)INSITU_RDO_DOMGL_RESOLUTION,
                   INSITU_RDO_DOMGL_VAR_NAME, INSITU_RDO_DOMGL_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new InSituRDO_DOmgL object.
     *
     * @note This must be tied with a parent InSituRDO before it can be
     * used.
     */
    InSituRDO_DOmgL()
        : Variable((const uint8_t)INSITU_RDO_DOMGL_VAR_NUM,
                   (uint8_t)INSITU_RDO_DOMGL_RESOLUTION,
                   INSITU_RDO_DOMGL_VAR_NAME, INSITU_RDO_DOMGL_UNIT_NAME,
                   INSITU_RDO_DOMGL_DEFAULT_CODE) {}
    /**
     * @brief Destroy the InSituRDO_DOmgL object - no action needed.
     */
    ~InSituRDO_DOmgL() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [dissolved oxygen percent saturation output](@ref sensor_insitu_rdo_dopercent)
 * from a [In-Situ RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
 *
 * @ingroup sensor_insitu_rdo
 */
/* clang-format on */
class InSituRDO_DOpct : public Variable {
 public:
    /**
     * @brief Construct a new InSituRDO_DOpct object.
     *
     * @param parentSense The parent InSituRDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RDOpercent".
     */
    explicit InSituRDO_DOpct(
        InSituRDO* parentSense, const char* uuid = "",
        const char* varCode = INSITU_RDO_DOPCT_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)INSITU_RDO_DOPCT_VAR_NUM,
                   (uint8_t)INSITU_RDO_DOPCT_RESOLUTION,
                   INSITU_RDO_DOPCT_VAR_NAME, INSITU_RDO_DOPCT_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new InSituRDO_DOpct object.
     *
     * @note This must be tied with a parent InSituRDO before it can be
     * used.
     */
    InSituRDO_DOpct()
        : Variable((const uint8_t)INSITU_RDO_DOPCT_VAR_NUM,
                   (uint8_t)INSITU_RDO_DOPCT_RESOLUTION,
                   INSITU_RDO_DOPCT_VAR_NAME, INSITU_RDO_DOPCT_UNIT_NAME,
                   INSITU_RDO_DOPCT_DEFAULT_CODE) {}
    /**
     * @brief Destroy the InSituRDO_DOpct object - no action needed.
     */
    ~InSituRDO_DOpct() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_insitu_rdo_temp) from a
 * [In-Situ RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
 *
 * @ingroup sensor_insitu_rdo
 */
/* clang-format on */
class InSituRDO_Temp : public Variable {
 public:
    /**
     * @brief Construct a new InSituRDO_Temp object.
     *
     * @param parentSense The parent InSituRDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RDOtempC".
     */
    explicit InSituRDO_Temp(InSituRDO* parentSense, const char* uuid = "",
                            const char* varCode = INSITU_RDO_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)INSITU_RDO_TEMP_VAR_NUM,
                   (uint8_t)INSITU_RDO_TEMP_RESOLUTION,
                   INSITU_RDO_TEMP_VAR_NAME, INSITU_RDO_TEMP_UNIT_NAME, varCode,
                   uuid) {}
    /**
     * @brief Construct a new InSituRDO_Temp object.
     *
     * @note This must be tied with a parent InSituRDO before it can be
     * used.
     */
    InSituRDO_Temp()
        : Variable((const uint8_t)INSITU_RDO_TEMP_VAR_NUM,
                   (uint8_t)INSITU_RDO_TEMP_RESOLUTION,
                   INSITU_RDO_TEMP_VAR_NAME, INSITU_RDO_TEMP_UNIT_NAME,
                   INSITU_RDO_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the InSituRDO_Temp object - no action needed.
     */
    ~InSituRDO_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [oxygen partial pressure output](@ref sensor_insitu_rdo_pressure) from a
 * [In-Situ RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
 *
 * @ingroup sensor_insitu_rdo
 */
/* clang-format on */
class InSituRDO_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new InSituRDO_Pressure object.
     *
     * @param parentSense The parent InSituRDO providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RDOppO2".
     */
    explicit InSituRDO_Pressure(
        InSituRDO* parentSense, const char* uuid = "",
        const char* varCode = INSITU_RDO_PRESSURE_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)INSITU_RDO_PRESSURE_VAR_NUM,
                   (uint8_t)INSITU_RDO_PRESSURE_RESOLUTION,
                   INSITU_RDO_PRESSURE_VAR_NAME, INSITU_RDO_PRESSURE_UNIT_NAME,
                   varCode, uuid) {}
    /**
     * @brief Construct a new InSituRDO_Pressure object.
     *
     * @note This must be tied with a parent InSituRDO before it can be
     * used.
     */
    InSituRDO_Pressure()
        : Variable((const uint8_t)INSITU_RDO_PRESSURE_VAR_NUM,
                   (uint8_t)INSITU_RDO_PRESSURE_RESOLUTION,
                   INSITU_RDO_PRESSURE_VAR_NAME, INSITU_RDO_PRESSURE_UNIT_NAME,
                   INSITU_RDO_PRESSURE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the InSituRDO_Pressure object - no action needed.
     */
    ~InSituRDO_Pressure() {}
};
/**@}*/
#endif  // SRC_SENSORS_INSITURDO_H_
