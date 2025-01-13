/**
 * @file GeoluxHydroCam.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the GeoluxHydroCam sensor subclass and the
 * GeoluxHydroCam_ImageSize variable subclass.
 *
 * These are for the Geolux HydroCam cameras.
 */
/* clang-format off */
/**
 * @defgroup sensor_hydrocam Geolux HydroCam
 * Classes for the Geolux HydroCam cameras.
 *
 * @ingroup the_sensors
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_hydrocam_intro Introduction
 *
 * > The [Geolux HydroCam](https://www.geolux-radars.com/hydrocam) is a
 * > monitoring camera specifically designed for remote visual observation of
 * > the hydrological monitoring site. Versatile communication interfaces
 * > (RS-232, RS-485, CAN, SDI-12, Ethernet) enable easy integration with Geolux
 * > or third-party dataloggers. Integrated switchable day/night filters and a
 * > high-quality motorized zoom/focus lens enable the Geolux HydroCam to
 * > withstand wide temperature ranges and operation in almost any environmental
 * > monitoring application.
 *
 * Please see the section
 * "[Notes on Arduino Streams and Software Serial](@ref page_arduino_streams)"
 * for more information about what streams can be used along with this library.
 *
 * @section sensor_hydrocam_datasheet Sensor Datasheet
 * - [Datasheet](https://www.geolux-radars.com/_files/ugd/8a15d8_d65c3618247b40ed94886dcb09bb4c33.pdf)
 * - [User Manual v1.2.3](https://www.geolux-radars.com/_files/ugd/e39b2a_35dcbb6cb9974bd59647b20487ca1511.pdf)
 * - [Assembly Guide](https://www.geolux-radars.com/_files/ugd/e39b2a_3ded96e993d249daa4e4000a3e419342.pdf)
 * - [Geolux Instrument Configurator Software 2.3.2](https://www.geolux-radars.com/_files/archives/e39b2a_f3bd3a20c9dd4df69205263102655af7.zip?dn=Setup-GeoluxInstrumentConfigurator-2.3.2.zip)
 *
 * @section sensor_hydrocam_ctor Sensor Constructor
 * {{ @ref GeoluxHydroCam::GeoluxHydroCam }}
 *
 * ___
 * @section sensor_hydrocam_examples Example Code
 * The Geolux HydroCam is used in the @menulink{geolux_hydro_cam} example.
 *
 * @menusnip{geolux_hydro_cam}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_GEOLUXHYDROCAM_H_
#define SRC_SENSORS_GEOLUXHYDROCAM_H_

// Debugging Statement
// #define MS_GEOLUXHYDROCAM_DEBUG

#ifdef MS_GEOLUXHYDROCAM_DEBUG
#define MS_DEBUGGING_STD "GeoluxHydroCam"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "VariableBase.h"
#include "SensorBase.h"

/** @ingroup sensor_hydrocam */
/**@{*/

/**
 * @anchor sensor_hydrocam_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by Maxbotix sonar
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the HRXL can report 1 value.
#define HYDROCAM_NUM_VARIABLES 1
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define HYDROCAM_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_hydrocam_timing
 * @name Sensor Timing
 * The sensor timing for a Geolux HydroCam
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; warm up time to completion of header:  160ms.
#define HYDROCAM_WARM_UP_TIME_MS 250
/// @brief Sensor::_stabilizationTime_ms; the HRXL is stable as soon as it warms
/// up (0ms stabilization).
#define HYDROCAM_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; the HRXL takes 166ms to complete a
/// measurement.
#define HYDROCAM_MEASUREMENT_TIME_MS 250
/**@}*/

/**
 * @anchor sensor_hydrocam_image_size
 * @name Image Size
 * The image size variable from a Geolux HydroCam
 *
 * {{ @ref GeoluxHydroCam_ImageSize::GeoluxHydroCam_ImageSize }}
 */
/**@{*/
/// @brief Decimals places in string representation; image size should have 0 -
/// resolution is 1 byte.
#define HRXL_RESOLUTION 0
/// @brief Sensor variable number; image size is stored in sensorValues[0].
#define HRXL_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "distance"
#define HRXL_VAR_NAME "distance"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "millimeter"
#define HRXL_UNIT_NAME "millimeter"
/// @brief Default variable short code; "HydroCamImageSize"
#define HRXL_DEFAULT_CODE "SonarRange"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [MaxBotix cameras](@ref sensor_hydrocam).
 *
 * @ingroup sensor_hydrocam
 */
/* clang-format on */
class GeoluxHydroCam : public Sensor {
 public:
    /**
     * @brief Construct a new MaxBotix Sonar object
     *
     * @note You CANNOT specify a number of measurements to average for the
     * camera. Only one image is taken; averaging does not make sense at all.
     *
     * @param stream An Arduino data stream for RS232 communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the MaxSonar.
     * Use -1 if it is continuously powered.
     * - The MaxSonar requires a 9V - 27V DC power supply.
     * @param triggerPin The pin on the mcu controlling the "trigger" for the
     * MaxSonar.  Use -1 or omit for continuous ranging.
     * @param maxRange Maximum valid measurement reported by the specific sensor
     * model (e.g. 5000 or 9999 or 765).
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     * @param convertCm Convert centimeter image size data from certain models
     * to millimeters. Default false.
     */
    GeoluxHydroCam(Stream* stream, int8_t powerPin, int8_t triggerPin = -1,
                   int16_t maxRange = 9999, uint8_t measurementsToAverage = 1,
                   bool convertCm = false);
    /**
     * @copydoc GeoluxHydroCam::GeoluxHydroCam
     */
    GeoluxHydroCam(Stream& stream, int8_t powerPin, int8_t triggerPin = -1,
                   int16_t maxRange = 9999, uint8_t measurementsToAverage = 1,
                   bool convertCm = false);
    /**
     * @brief Destroy the MaxBotix Sonar object
     */
    ~GeoluxHydroCam();

    /**
     * @copydoc Sensor::getSensorLocation()
     */
    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets pin mode on the trigger pin.  It also sets the expected stream
     * timeout for modbus and updates the #_sensorStatus.  No sensor power is
     * required.  This will always return true.
     *
     * @return True if the setup was successful.
     */
    bool setup(void) override;
    /**
     * @brief Wake the sensor up, if necessary.  Do whatever it takes to get a
     * sensor in the proper state to begin a measurement.
     *
     * Verifies that the power is on and updates the #_sensorStatus.  This also
     * sets the #_millisSensorActivated timestamp.
     *
     * For the MaxSonar, this also reads and dumps any returned "header" lines
     * from the sensor.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return True if the wake function completed successfully.
     */
    bool wake(void) override;

    /**
     * @copydoc Sensor::addSingleMeasurementResult()
     */
    bool addSingleMeasurementResult(void) override;

 private:
    int16_t _maxRange;    ///< The maximum image size of the Maxbotix sonar
    int8_t  _triggerPin;  ///< The pin to trigger the Maxbotix sonar
    /**
     * @brief True to convert the output from the Maxbotix from centimeters to
     * millimeters.
     */
    bool _convertCm;
    /**
     * @brief Private reference to the stream for communciation with the
     * Maxbotix sensor.
     */
    Stream* _stream;
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [image size output](@ref sensor_hydrocam_image_size) from a
 * [Geolux HydroCam camera](@ref sensor_hydrocam).
 *
 * @ingroup sensor_hydrocam
 */
/* clang-format on */
class GeoluxHydroCam_ImageSize : public Variable {
 public:
    /**
     * @brief Construct a new GeoluxHydroCam_ImageSize object.
     *
     * @param parentSense The parent GeoluxHydroCam providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "SonarRange".
     */
    explicit GeoluxHydroCam_ImageSize(GeoluxHydroCam* parentSense,
                                      const char*     uuid = "",
                                      const char* varCode  = HRXL_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)HRXL_VAR_NUM, (uint8_t)HRXL_RESOLUTION,
                   HRXL_VAR_NAME, HRXL_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new GeoluxHydroCam_ImageSize object.
     *
     * @note This must be tied with a parent GeoluxHydroCam before it can be
     * used.
     */
    GeoluxHydroCam_ImageSize()
        : Variable((uint8_t)HRXL_VAR_NUM, (uint8_t)HRXL_RESOLUTION,
                   HRXL_VAR_NAME, HRXL_UNIT_NAME, HRXL_DEFAULT_CODE) {}
    /**
     * @brief Destroy the GeoluxHydroCam_ImageSize object - no action needed.
     */
    ~GeoluxHydroCam_ImageSize() {}
};
/**@}*/
#endif  // SRC_SENSORS_GEOLUXHYDROCAM_H_
