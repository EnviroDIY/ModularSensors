/**
 * @file InSituRDO.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the InSituRDO sensor subclass and the variable subclasses
 * InSituRDO_Temp, InSituRDO_DOpct, and InSituRDO_DOmgL.
 *
 * These are for the InSitu RDO PRO-X digital dissolved oxygen sensor.
 *
 * This depends on the SDI12Sensors parent class.
 */
/* clang-format off */
/**
 * @defgroup sensor_insitu_rdo InSitu RDO PRO-X
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
 * The RDO PRO-X requires a 9.6 - 16 VDC power supply when operating in SDI-12 mode.
 * The maximum power consumption is 50 mA at 12 VDC.
 * Measurement current is 6 mA typical at 24 VDC and idle current is 160 µA typical at 24 VDC.
 *
 * The [InSitu RDO PRO-X](https://in-situ.com/us/rdo-pro-x-dissolved-oxygen-probe)
 * sensor is implemented as a sub-classes of the SDI12Sensors class.
 * While the sensor can also communicate over Modbus/RS485 or a 4-20 mA loop,
 * in this case I've chosen to use SDI-12 to minimize the number of pins
 * required for communication and because most Arduino style processors have
 * very few possible COM ports available for RS485.
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
 * If you do wish to re-calibrate the probe, that must be done using Win-Situ 5 software,
 * the VuSitu mobile app, or the Modbus/RS485 interface.
 *
 * @section sensor_insitu_rdo_cap Sensor Cap Maintenance
 * The sensor cap has an estimated lifetime of 2 years from the time of first reading.
 * To preserve its lifespan, the sensor cap should not be opened or removed from its
 * packaging until the sensor is ready to be deployed.
 * To clean the sensor cap:
 * > 1. The cap and nose cone must remain on the probe.
 * > 2. Rinse the sensor with clean water from a squirt bottle or spray bottle.
 * > 3. Gently wipe with a soft-bristled brush or soft cloth if biofouling is present.
 *      Use Alconox to remove grease.
 * > 4. If extensive fouling or mineral build-up is present, soak the cap end in vinegar for 15 minutes,
 *      then soak in deionized (DI) water for 15 minutes.
 * The optical window should only be cleaned when the sensor cap is changed.
 *
 * InSitu recommends useing an antifouling guard or airblast adapter to extend deployments and protect your data.
 *
 * @section sensor_insitu_rdo_cap Deployment Notes and Warnings
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
 * The spec sheet is here:  https://in-situ.com/pub/media/support/documents/RDO-PRO-X_SS.pdf
 *
 * @section sensor_insitu_rdo_ctor Sensor Constructor
 * {{ @ref InSituRDO::InSituRDO }}
 *
 * ___
 * @section sensor_insitu_rdo_examples Example Code
 * The InSitu RDO PRO-X is used in the @menulink{insitu_rdo} example.
 *
 * @menusnip{insitu_rdo}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_InSituRDO_H_
#define SRC_SENSORS_InSituRDO_H_

// Included Dependencies
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines
/** @ingroup sensor_insitu_rdo */
/**@{*/

/// @brief Sensor::_numReturnedValues; the RDO PRO-X can report 3 values.
#define INSITU_RDO_NUM_VARIABLES 3

/**
 * @anchor sensor_insitu_rdo_timing
 * @name Sensor Timing
 * The sensor timing for an InSitu RDO PRO-X
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; zzzzzzz
#define INSITU_RDO_WARM_UP_TIME_MS 275
/// @brief Sensor::_stabilizationTime_ms; Per the manual, T90 < 45sec; T95 <
/// 60sec @ 25°C
#define INSITU_RDO_STABILIZATION_TIME_MS 60000
/**
 * @brief Sensor::_measurementTime_ms; zzzz
 */
#define INSITU_RDO_MEASUREMENT_TIME_MS 5335
/**@}*/

/**
 * @anchor sensor_insitu_rdo_temp
 * @name Temperature
 * The temperature variable from an InSitu RDO PRO-X
 * - Range is 0° to 50°C (32° to 122°F)
 * - Accuracy is ± 0.1°C typical
 *
 * {{ @ref InSituRDO_Temp::InSituRDO_Temp }}
 */
/**@{*/
/// @brief Decimals places in string representation; temperature should have 2 -
/// resolution is 0.01°C.
#define INSITU_RDO_TEMP_RESOLUTION 2
/// @brief Variable number; temperature is stored in sensorValues[0].
#define INSITU_RDO_TEMP_VAR_NUM 0
/// @brief Variable name; "temperature"
#define INSITU_RDO_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name; "degreeCelsius" (°C)
#define INSITU_RDO_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code; "DOtempC"
#define INSITU_RDO_TEMP_DEFAULT_CODE "RDOtempC"
/**@}*/

/**
 * @anchor sensor_insitu_rdo_dopercent
 * @name Dissolved Oxygen Percent Saturation
 * The percent saturation variable from an InSitu RDO PRO-X
 * - Range is 0 to 60 mg/L concentration
 * - Accuracy:
 *   - ± 0.1 mg/L from 0 to 20 mg/L
 *   - ± 2% of reading from 20-60 mg/L
 * @note To acheive maximum accuracy, the sensor must be calibrated using either
 * a one or two point calibration.
 *
 * {{ @ref InSituRDO_DOpct::InSituRDO_DOpct }}
 */
/**@{*/
/// @brief Decimals places in string representation; dissolved oxygen percent
/// should have 2 - resolution is 0.01 mg/L.
#define INSITU_RDO_DOPCT_RESOLUTION 2
/// @brief Variable number; dissolved oxygen percent is stored in
/// sensorValues[1]
#define INSITU_RDO_DOPCT_VAR_NUM 1
/// @brief Variable name; "oxygenDissolvedPercentOfSaturation"
#define INSITU_RDO_DOPCT_VAR_NAME "oxygenDissolvedPercentOfSaturation"
/// @brief Variable unit name; "percent" (% saturation)
#define INSITU_RDO_DOPCT_UNIT_NAME "percent"
/// @brief Default variable short code; "DOpercent"
#define INSITU_RDO_DOPCT_DEFAULT_CODE "RDOpercent"
/**@}*/

/**
 * @anchor sensor_insitu_rdo_domgl
 * @name Dissolved Oxygen Concentration
 * The DO concentration variable from an InSitu RDO PRO-X
 * - Range is not specified in sensor datasheet
 * - Accuracy is 1 % of reading or 0.02PPM, whichever is greater
 * @note To acheive maximum accuracy, the sensor must be calibrated using either
 * a one or two point calibration.
 *
 * {{ @ref InSituRDO_DOmgL::InSituRDO_DOmgL }}
 */
/**@{*/
/// @brief Decimals places in string representation; dissolved oxygen
/// concentration should have 2 - resolution is 0.01 mg/L.
#define INSITU_RDO_DOMGL_RESOLUTION 2
/// @brief Variable number; dissolved oxygen concentration is stored in
/// sensorValues[2]
#define INSITU_RDO_DOMGL_VAR_NUM 2
/// @brief Variable name; "oxygenDissolved"
#define INSITU_RDO_DOMGL_VAR_NAME "oxygenDissolved"
/// @brief Variable unit name; "milligramPerLiter" (mg/L)
#define INSITU_RDO_DOMGL_UNIT_NAME "milligramPerLiter"
/// @brief Default variable short code; "DOppm"
#define INSITU_RDO_DOMGL_DEFAULT_CODE "RDOppm"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [InSitu RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
 *
 * @ingroup sensor_insitu_rdo
 */
/* clang-format on */
class InSituRDO : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new InSitu RDO object.
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
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "InSitu RDO PRO-X", INSITU_RDO_NUM_VARIABLES,
                       INSITU_RDO_WARM_UP_TIME_MS,
                       INSITU_RDO_STABILIZATION_TIME_MS,
                       INSITU_RDO_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc InSituRDO::InSituRDO
     */
    InSituRDO(char* SDI12address, int8_t powerPin, int8_t dataPin,
              uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "InSitu RDO PRO-X", INSITU_RDO_NUM_VARIABLES,
                       INSITU_RDO_WARM_UP_TIME_MS,
                       INSITU_RDO_STABILIZATION_TIME_MS,
                       INSITU_RDO_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc InSituRDO::InSituRDO
     */
    InSituRDO(int SDI12address, int8_t powerPin, int8_t dataPin,
              uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "InSitu RDO PRO-X", INSITU_RDO_NUM_VARIABLES,
                       INSITU_RDO_WARM_UP_TIME_MS,
                       INSITU_RDO_STABILIZATION_TIME_MS,
                       INSITU_RDO_MEASUREMENT_TIME_MS) {}
    /**
     * @brief Destroy the InSitu RDO object
     */
    ~InSituRDO() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref sensor_insitu_rdo_temp) from a
 * [InSitu RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
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
 * [dissolved oxygen percent saturation output](@ref sensor_insitu_rdo_dopercent)
 * from a [InSitu RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
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
 * [dissolved oxygen concentration output](@ref sensor_insitu_rdo_domgl) from a
 * [InSitu RDO PRO-X dissolved oxygen sensor](@ref sensor_insitu_rdo).
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
/**@}*/
#endif  // SRC_SENSORS_InSituRDO_H_
