/**
 * @file Decagon5TM.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Decagon5TM subclass of the SDI12Sensors class along with
 * the variable subclasses Decagon5TM_Ea, Decagon5TM_Temp, and Decagon5TM_VWC.
 *
 * These are used for the Meter ECH20 or the discontinued Decagon Devices 5TM Soil
 * Moisture probe.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 *
 * @defgroup fivetm_group Meter ECH2O (5TM)
 * Classes for the @ref fivetm_page
 *
 * @copydoc fivetm_page
 *
 * @ingroup sdi12_group
 */
/* clang-format off */
/**
 * @page fivetm_page Meter ECH2O (5TM)
 *
 * @tableofcontents
 *
 * @section fivetm_intro Introduction
 *
 * Meter Environmental makes two series of soil moisture sensors, the
 * [ECH2O series](https://www.metergroup.com/environment/products/?product_category=9525) and the
 * [Teros series](https://www.metergroup.com/environment/products/teros-12/).
 * __This page is for the ECH2O series.__
 *
 * @note Meter Environmental was formerly known as Decagon Devices and sold the a very similar sensor to the current EC-5 sensor as the 5TM.
 *
 * Both series of sensors communicate with the board using the [SDI-12 protocol](http://www.sdi-12.org/) (and the
 * [Arduino SDI-12 library](https://github.com/EnviroDIY/Arduino-SDI-12)).
 * They require a 3.5-12V power supply, which can be turned off between measurements.
 * While contrary to the manual, they will run with power as low as 3.3V.
 * On the 5TM with a stereo cable, the power is connected to the tip, data to the ring, and ground to the sleeve.
 * On the bare-wire version, the power is connected to the _white_ cable, data to _red_, and ground to the unshielded cable.
 *
 * The SDI-12 protocol specifies that all new devices should come from the manufacturer with a pre-programmed address of "0".
 * For Meter brand sensors, you *must* change the sensor address before you can begin to use it.
 * If you want to use more than one SDI-12 sensor, you must ensure that each sensor has a different address.
 * To find or change the SDI-12 address of your sensor, load and run the
 * [sdi12_address_change](https://github.com/EnviroDIY/ModularSensors/blob/master/tools/sdi12_address_change/sdi12_address_change.ino)
 * program from the [tools](https://github.com/EnviroDIY/ModularSensors/tree/master/tools) directory or the
 * [b_address_change](https://github.com/EnviroDIY/Arduino-SDI-12/tree/master/examples/b_address_change)
 * example within the SDI-12 library.
 *
 * Keep in mind that SDI12 is a slow communication protocol (only 1200 baud) and _ALL interrupts are turned off during communication_.
 * This means that if you have any interrupt driven sensors (like a tipping bucket) attached with an SDI12 sensor,
 * no interrupts (or tips) will be registered during SDI12 communication.
 *
 * @section fivetm_datasheet Sensor Datasheet
 * [Datasheet](http://publications.metergroup.com/Manuals/20431_EC-5_Manual_Web.pdf)
 *
 * @section fivetm_sensor The ECH2O (5TM) Sensor
 * @ctor_doc{Decagon5TM, char SDI12address, int8_t powerPin, int8_t dataPin, uint8_t measurementsToAverage}
 * @subsection fivetm_timing Sensor Timing
 * - Maximum warm-up time in SDI-12 mode: 200ms, assume stability at warm-up
 * - Maximum measurement duration: 200ms
 *
 * @section fivetm_ea Ea Output
 * @variabledoc{Decagon5TM,Ea}
 *   - Range is 0 – 1 m3/m3 (0 – 100% VWC)
 *   - Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 *   - Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.0008 m3/m3 (0.08% VWC) from 0 – 50% VWC
 *   - Reported as farads per meter (F/m)
 *   - Default variable code is SoilEa
 *
 * @section fivetm_temp Temp Output
 * @variabledoc{Decagon5TM,Temp}
 *   - Range is - 40°C to + 50°C
 *   - Accuracy is ± 1°C
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.1°C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is SoilTemp
 *
 * @section fivetm_vwc VWC Output
 * @variabledoc{Decagon5TM,VWC}
 *   - Range is 0 – 1 m3/m3 (0 – 100% VWC)
 *   - Accuracy for Generic calibration equation: ± 0.03 m3/m3 (± 3% VWC) typ
 *   - Accuracy for Medium Specific Calibration: ± 0.02 m3/m3 (± 2% VWC)
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.0008 m3/m3 (0.08% VWC) from 0 – 50% VWC
 *   - Reported as volumetric percent water content (%, m3/100m3)
 *   - Default variable code is SoilVWC
 *
 * ___
 * @section fivetm_examples Example Code
 * The Meter ECH2O (5TM) is used in the @menulink{fivetm} example.
 *
 * @menusnip{fivetm}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_DECAGON5TM_H_
#define SRC_SENSORS_DECAGON5TM_H_

// Debugging Statement
// #define MS_DECAGON5TM_DEBUG

#ifdef MS_DECAGON5TM_DEBUG
#define MS_DEBUGGING_STD "Decagon5TM"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

// Sensor Specific Defines

/// Sensor::_numReturnedValues; the 5TM can report 3 values.
#define TM_NUM_VARIABLES 3
/// Sensor::_warmUpTime_ms; the 5TM warms up in 200ms.
#define TM_WARM_UP_TIME_MS 200
/// Sensor::_stabilizationTime_ms; the 5TM is stable after 0ms.
#define TM_STABILIZATION_TIME_MS 0
/// Sensor::_measurementTime_ms; the 5TM takes 200ms to complete a measurement.
#define TM_MEASUREMENT_TIME_MS 200

/**
 * @brief Decimals places in string representation; EA should have 5.
 *
 * 4 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define TM_EA_RESOLUTION 5
/// Variable number; EA is stored in sensorValues[0].
#define TM_EA_VAR_NUM 0

/**
 * @brief Decimals places in string representation; temperature should have 2.
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define TM_TEMP_RESOLUTION 2
/// Variable number; temperature is stored in sensorValues[1].
#define TM_TEMP_VAR_NUM 1

/**
 * @brief Decimals places in string representation; VWC should have 3.
 *
 * 2 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging
 */
#define TM_VWC_RESOLUTION 3
/// Variable number; VWC is stored in sensorValues[2].
#define TM_VWC_VAR_NUM 2

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter ECH2O soil moisture sensors](@ref fivetm_page).
 *
 * These were formerly sold as the Decagon 5TM.
 *
 * @ingroup fivetm_group
 */
/* clang-format on */
class Decagon5TM : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Decagon 5TM object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power on/off, and
     * the Arduino pin sending and receiving data are required for the sensor
     * constructor.  Optionally, you can include a number of distinct readings to
     * average.  The data pin must be a pin that supports pin-change interrupts.
     *
     * @param SDI12address The SDI-12 address of the sensor
     * @param powerPin The pin on the mcu controlling power to the ECH2O.  Use -1 if
     * the sensor is continuously powered.
     * - Requires a 3.5-12V power supply, which can be turned off between measurements
     * @param dataPin The pin on the mcu receiving data from the ECH2O.
     * @param measurementsToAverage The number of measurements to average.
     */
    Decagon5TM(char SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc Decagon5TM::Decagon5TM
     */
    Decagon5TM(char* SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc Decagon5TM::Decagon5TM
     */
    Decagon5TM(int SDI12address, int8_t powerPin, int8_t dataPin,
               uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "Decagon5TM", TM_NUM_VARIABLES, TM_WARM_UP_TIME_MS,
                       TM_STABILIZATION_TIME_MS, TM_MEASUREMENT_TIME_MS) {}
    // Destructor
    /**
     * @brief Destroy the Decagon 5TM object
     */
    ~Decagon5TM() {}

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;
};


// Defines the Ea/Matric Potential Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [apparent dielectric permittivity (εa, matric potential) output](@ref fivetm_ea)
 * from a [Meter ECH20 or Decagon 5TM soil moisture probe](@ref fivetm_page).
 *
 * @ingroup fivetm_group
 */
/* clang-format on */
class Decagon5TM_Ea : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_Ea object.
     *
     * @param parentSense The parent Decagon5TM providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilEa".
     */
    explicit Decagon5TM_Ea(Decagon5TM* parentSense, const char* uuid = "",
                           const char* varCode = "SoilEa")
        : Variable(parentSense, (const uint8_t)TM_EA_VAR_NUM, (uint8_t)TM_EA_RESOLUTION,
                   "permittivity", "faradPerMeter", varCode, uuid) {}
    /**
     * @brief Construct a new Decagon5TM_Ea object.
     *
     * @note This must be tied with a parent Decagon5TM before it can be used.
     */
    Decagon5TM_Ea()
        : Variable((const uint8_t)TM_EA_VAR_NUM, (uint8_t)TM_EA_RESOLUTION,
                   "permittivity", "faradPerMeter", "SoilEa") {}
    /**
     * @brief Destroy the Decagon5TM_Ea object - no action needed.
     */
    ~Decagon5TM_Ea() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref fivetm_temp) output from a
 * [Meter ECH20 or Decagon 5TM soil moisture probe](@ref fivetm_page).
 *
 * @ingroup fivetm_group
 */
/* clang-format on */
class Decagon5TM_Temp : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_Temp object.
     *
     * @param parentSense The parent Decagon5TM providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilTemp".
     */
    explicit Decagon5TM_Temp(Decagon5TM* parentSense, const char* uuid = "",
                             const char* varCode = "SoilTemp")
        : Variable(parentSense, (const uint8_t)TM_TEMP_VAR_NUM,
                   (uint8_t)TM_TEMP_RESOLUTION, "temperature", "degreeCelsius", varCode,
                   uuid) {}
    /**
     * @brief Construct a new Decagon5TM_Temp object.
     *
     * @note This must be tied with a parent Decagon5TM before it can be used.
     */
    Decagon5TM_Temp()
        : Variable((const uint8_t)TM_TEMP_VAR_NUM, (uint8_t)TM_TEMP_RESOLUTION,
                   "temperature", "degreeCelsius", "SoilTemp") {}
    /**
     * @brief Destroy the Decagon5TM_Temp object - no action needed.
     */
    ~Decagon5TM_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [volumetric water content](@ref fivetm_vwc) output from a
 * [Meter ECH20 or Decagon 5TM soil moisture probe](@ref fivetm_page).
 *
 * @ingroup fivetm_group
 */
/* clang-format on */
class Decagon5TM_VWC : public Variable {
 public:
    /**
     * @brief Construct a new Decagon5TM_VWC object.
     *
     * @param parentSense The parent Decagon5TM providing the result values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SoilVWC".
     */
    explicit Decagon5TM_VWC(Decagon5TM* parentSense, const char* uuid = "",
                            const char* varCode = "SoilVWC")
        : Variable(parentSense, (const uint8_t)TM_VWC_VAR_NUM,
                   (uint8_t)TM_VWC_RESOLUTION, "volumetricWaterContent", "percent",
                   varCode, uuid) {}
    /**
     * @brief Construct a new Decagon5TM_VWC object.
     *
     * @note This must be tied with a parent Decagon5TM before it can be used.
     */
    Decagon5TM_VWC()
        : Variable((const uint8_t)TM_VWC_VAR_NUM, (uint8_t)TM_VWC_RESOLUTION,
                   "volumetricWaterContent", "percent", "SoilVWC") {}
    /**
     * @brief Destroy the Decagon5TM_VWC object - no action needed.
     */
    ~Decagon5TM_VWC() {}
};

#endif  // SRC_SENSORS_DECAGON5TM_H_
