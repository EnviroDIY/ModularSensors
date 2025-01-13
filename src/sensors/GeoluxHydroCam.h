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
 * The Geolux HydroCam is supported using **RS232** communication. You **must**
 * have an RS232-to-TTL adapter to be able to communicate with the HydroCam.
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
 * The number of variables that can be returned by Geolux HydroCam
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the HydroCam can report 1 value.
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
/// @brief Sensor::_warmUpTime_ms; warm up time to completion of header: ~340ms.
#define HYDROCAM_WARM_UP_TIME_MS 400
/// @brief Sensor::_stabilizationTime_ms; the HydroCam stabilization time
/// depends on what wake commands we set and whether or not we autofocus. It
/// could be 30+ seconds.
#define HYDROCAM_STABILIZATION_TIME_MS 0
/// @brief Sensor::_measurementTime_ms; the HydroCam takes up to a minute (or
/// maybe even more) to take an image and transfer the data from it.
/// measurement.
#define HYDROCAM_MEASUREMENT_TIME_MS 60000
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
#define HYDROCAM_SIZE_RESOLUTION 0
/// @brief Sensor variable number; image size is stored in sensorValues[0].
#define HYDROCAM_SIZE_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "imageSize"
/// @TODO Request a new variable name for image size.
#define HYDROCAM_SIZE_VAR_NAME "imageSize"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "byte"
#define HYDROCAM_SIZE_UNIT_NAME "byte"
/// @brief Default variable short code; "HydroCamImageSize"
#define HYDROCAM_SIZE_DEFAULT_CODE "HydroCamImageSize"
/**@}*/


/* clang-format off */
/**
 * @brief The Sensor sub-class for the [Geolux HydroCam cameras](@ref sensor_hydrocam).
 *
 * @ingroup sensor_hydrocam
 */
/* clang-format on */
class GeoluxHydroCam : public Sensor {
 public:
    /**
     * @brief Construct a new Geolux HydroCam object
     *
     * @note You **CANNOT** specify a number of measurements to average for the
     * camera. Only one image is taken; averaging does not make sense at all.
     *
     * The RESOLUTION parameter must be one of the following:
     *     - "160x120" (4:3, 0.019 megapixel, Quarter-QVGA, QQVGA)
     *     - "320x240" (4:3, 0.077 megapixel, Quarter VGA , QVGA)
     *     - "640x480" (4:3, 0.307 megapixel, VGA)
     *     - "800x600" (4:3, 0.48 megapixel, Super VGA, SVGA)
     *     - "1024x768" (4:3, 0.79 megapixel, XGA)
     *     - "1280x960" (4:3, 1.23 megapixel, QuadVGA)
     *     - "1600x1200" (4:3, 1.92 megapixel, Ultra-XGA, UXGA)
     *     - "1920x1080" (16:9, 2.07 megapixel, 1080p, Full HD, FHD)
     *     - "2048x1536" (4:3, 3.15 megapixel, Quad-XGA, QXGA)
     *     - "2592x1944" (4:3, 5.04 megapixel, 1944p)
     *
     * @param stream An Arduino data stream for RS232 communication.  See
     * [notes](@ref page_arduino_streams) for more information on what streams
     * can be used.
     * @param powerPin The pin on the mcu controlling power to the Geolux
     * HydroCam. Use -1 if it is continuously powered.
     * - The Geolux HydroCam requires a 9V - 27V DC power supply.
     * @param imageResolution The image resolution to use. Optional with a
     * default value of "1600x1200".
     * @param filePrefix The start of the file name for saved files. The date
     * and time will be appended to the filename. Optional with a default value
     * of "HydroCam_". If you want only the date/time as the file name specify
     * "" as the filename prefix. Otherwise, I strongly recommend ending the
     * prefix with an "_".
     * @param alwaysAutoFocus True to autofocus before every image. This may be
     * necessary if the camera is power cycled between images. If you are not
     * power cycling or moving the camera, I recommend not autofocusing often
     * because the autofocus takes about 30s. Default false.
     */
    GeoluxHydroCam(Stream* stream, int8_t powerPin,
                   const char* imageResolution = "1600x1200",
                   const char* filePrefix      = "HydroCam",
                   bool        alwaysAutoFocus = false);
    /**
     * @copydoc GeoluxHydroCam::GeoluxHydroCam
     */
    GeoluxHydroCam(Stream& stream, int8_t powerPin,
                   const char* imageResolution = "1600x1200",
                   const char* filePrefix      = "HydroCam",
                   bool        alwaysAutoFocus = false);
    /**
     * @brief Destroy the Geolux HydroCam object
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
     * This sets the image resolution and runs an autofocus.
     *
     * @NOTE Autofocus takes approximately 30s to complete.  Unless you plan to
     * move your device betwen readings, I recommend only running the autofocus
     * at setup, not at every power-up.
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
     * For the Geolux HydroCam, this also reads and dumps any returned "header"
     * lines from the sensor.
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

    /**
     * @brief Check whether or not enough time has passed between the camera
     * wake and being ready to image.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that enough time has passed that the camera is
     * ready to take an image.
     *
     * @note We override the default fuction because the amount of time required
     * for imaging depends on the camera's mood and whether or not you autofocus
     * on every reading.
     */
    bool isStable(bool debug = false) override;

    /**
     * @brief Check whether or not the camera has completed imaging.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that the camera is now reporting ready after an
     * image was started.
     *
     * @note We override the default fuction because the amount of time required
     * for imaging depends on the resolution.
     */
    bool isMeasurementComplete(bool debug = false) override;

 private:
    const char* _imageResolution;  ///< The pin to trigger the Geolux HydroCam
    const char* _filePrefix;  ///< The maximum image size of the Geolux HydroCam
    /**
     * @brief True to autofocus before every image. This may be necessary if the
     * camera is power cycled between images. If you are not power cycling or
     * moving the camera, I recommend not autofocusing often because the
     * autofocus takes about 30s.
     */
    bool _alwaysAutoFocus;
    /**
     * @brief Private reference to the stream for communciation with the
     * Geolux HydroCam sensor.
     */
    Stream* _stream;
    /**
     * @brief Private reference to the underlieing GeoluxCamera Instance
     */
    GeoluxCamera* _camera;
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
     * optional with a default value of "HydroCamImageSize".
     */
    explicit GeoluxHydroCam_ImageSize(
        GeoluxHydroCam* parentSense, const char* uuid = "",
        const char* varCode = HYDROCAM_SIZE_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)HYDROCAM_SIZE_VAR_NUM,
                   (uint8_t)HYDROCAM_SIZE_RESOLUTION, HYDROCAM_SIZE_VAR_NAME,
                   HYDROCAM_SIZE_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new GeoluxHydroCam_ImageSize object.
     *
     * @note This must be tied with a parent GeoluxHydroCam before it can be
     * used.
     */
    GeoluxHydroCam_ImageSize()
        : Variable((uint8_t)HYDROCAM_SIZE_VAR_NUM,
                   (uint8_t)HYDROCAM_SIZE_RESOLUTION, HYDROCAM_SIZE_VAR_NAME,
                   HYDROCAM_SIZE_UNIT_NAME, HYDROCAM_SIZE_DEFAULT_CODE) {}
    /**
     * @brief Destroy the GeoluxHydroCam_ImageSize object - no action needed.
     */
    ~GeoluxHydroCam_ImageSize() {}
};
/**@}*/
#endif  // SRC_SENSORS_GEOLUXHYDROCAM_H_
