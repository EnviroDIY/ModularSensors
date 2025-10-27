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
 * The only possible speed for communication is 115200. *This is too fast for an
 * 8MHz board like the EnviroDIY Mayfly.*
 *
 * Please see the section
 * "[Notes on Arduino Streams and Software Serial](@ref page_arduino_streams)"
 * for more information about what streams can be used along with this library.
 *
 * This library currently only supports using the same SD card for saving images
 * as is used for writing data files.
 *
 * Keep in mind when using the HydroCam that transferring images from the camera
 * to the SD card is a slow process.  For the smallest images (160x120) the
 * transfer time can be as little as 850ms, but the largest images (2592x1944) can
 * take 70 seconds (over a minute) to transfer.
 *
 * If you choose to autofocus on every image, the autofocus takes 25-30s on firmware
 * prior to version 2, and about 7s on version 2.0.5.
 *
 * Changing settings takes up to 7s.
 *
 * Taking an image (but not transferring the data) takes ~6.2s for the smallest
 * images and over 16s for the largest.
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

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_GEOLUXHYDROCAM_DEBUG
#define MS_DEBUGGING_STD "GeoluxHydroCam"
#endif
#ifdef MS_GEOLUXHYDROCAM_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "GeoluxHydroCam"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
#include "VariableBase.h"
#include "SensorBase.h"
#include "LoggerBase.h"
#include <GeoluxCamera.h>

/** @ingroup sensor_hydrocam */
/**@{*/

/**
 * @brief The file extension for images from the Geolux HydroCam: ".jpg"
 */
#define HYDROCAM_FILE_EXTENSION ".jpg"

/**
 * @brief The minimum spacing between requesting status messages from the
 * HydroCam.
 */
#define HYDROCAM_MINIMUM_STATUS_SPACING 250

/**
 * @anchor sensor_hydrocam_var_counts
 * @name Sensor Variable Counts
 * The number of variables that can be returned by Geolux HydroCam
 */
/**@{*/
/// @brief Sensor::_numReturnedValues; the HydroCam can report 2 values.
#define HYDROCAM_NUM_VARIABLES 2
/// @brief Sensor::_incCalcValues; we don't calculate any additional values.
#define HYDROCAM_INC_CALC_VARIABLES 0
/**@}*/

/**
 * @anchor sensor_hydrocam_timing
 * @name Sensor Timing
 * The sensor timing for a Geolux HydroCam
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; warm up time from power on until boot message
/// finishes is nearly exactly 340ms on firmware 1.3.6 and about 490ms on
/// firmware 2.0.5.
#define HYDROCAM_WARM_UP_TIME_MS 350
/// @brief Sensor::_stabilizationTime_ms; the HydroCam is ready after a minimum
/// of about 11ms after the end of the boot up message. Changing settings takes
/// up to 7s. Running an autofocus takes about 25-30s.
#define HYDROCAM_STABILIZATION_TIME_MS 11
/// @brief The maximum time to wait for boot + settings.
#define HYDROCAM_STABILIZATION_TIME_MAX 16000L
/// @brief Minimum time to wait for an autofocus. Running an autofocus takes
/// about 25-30s on firmwares <2.0.1, but only ~7.5s on firmwares >=2.0.1.
#define HYDROCAM_AUTOFOCUS_TIME_MS 7250L
/// @brief The maximum time to wait for autofocus. This is combined with the
/// stabilization time.
#define HYDROCAM_AUTOFOCUS_TIME_MAX 35000L
/// @brief Sensor::_measurementTime_ms; the HydroCam imaging time is variable
/// depending on the image size, but the typical minimum I've seen for the
/// smallest image (160x120) is ~3.8s on firmware >2.0.1.
/// The largest image takes over 16s on firmwares <2.0.1.
#define HYDROCAM_MEASUREMENT_TIME_MS 3800L
/// @brief The maximum time to wait for an image.
#define HYDROCAM_MEASUREMENT_TIME_MAX 18000L
/**@}*/

/**
 * @anchor sensor_hydrocam_image_size
 * @name Image Size
 * The image size variable from a Geolux HydroCam
 *
 * This variable represents the number of bytes transferred to the
 * SD card, not necessarily (but hopefully) the size of the image
 * as reported by the camera
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
/// @todo Request a new variable name for image size.
#define HYDROCAM_SIZE_VAR_NAME "imageSize"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "byte"
#define HYDROCAM_SIZE_UNIT_NAME "byte"
/// @brief Default variable short code; "HydroCamImageSize"
#define HYDROCAM_SIZE_DEFAULT_CODE "HydroCamImageSize"
/**@}*/

/**
 * @anchor sensor_hydrocam_byte_error
 * @name Byte Error
 * The byte error variable from a Geolux HydroCam
 *
 * {{ @ref GeoluxHydroCam_ByteError::GeoluxHydroCam_ByteError }}
 */
/**@{*/
/// @brief Decimals places in string representation; byte error should have
/// 0 - resolution is 1 byte.
#define HYDROCAM_ERROR_RESOLUTION 0
/// @brief Sensor variable number; byte error is stored in sensorValues[1].
#define HYDROCAM_ERROR_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "flashMemoryErrorCount"
#define HYDROCAM_ERROR_VAR_NAME "flashMemoryErrorCount"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "byte"
#define HYDROCAM_ERROR_UNIT_NAME "byte"
/// @brief Default variable short code; "HydroCamByteError"
#define HYDROCAM_ERROR_DEFAULT_CODE "HydroCamByteError"
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
     * @param baseLogger The logger instance with an attached SD card.
     * @param powerPin2 The pin on the mcu controlling power to the RS232
     * adapter, if it is different from that used to power the sensor.  Use -1
     * or omit if not applicable.
     * @param imageResolution The image resolution to use. Optional with a
     * default value of "1600x1200".
     * @param filePrefix The start of the file name for saved files. **An
     * underscore** and then the date and time will be appended to the prefix to
     * create the filename. The extension will always be
     * `#HYDROCAM_FILE_EXTENSION` (.jpg). Optional with a default value of
     * nullptr. If you want only the date/time as the file name specify
     * "" as the filename prefix. If you do not specify any file prefix, the
     * logger ID will be used
     * @param alwaysAutoFocus True to autofocus before every image. This may be
     * necessary if the camera is power cycled between images. If you are not
     * power cycling or moving the camera, I recommend not autofocusing often
     * because the autofocus takes about 30s. Default false.
     */
    GeoluxHydroCam(Stream* stream, int8_t powerPin, Logger& baseLogger,
                   int8_t powerPin2, const char* imageResolution = "1600x1200",
                   const char* filePrefix      = nullptr,
                   bool        alwaysAutoFocus = false);
    /**
     * @copydoc GeoluxHydroCam::GeoluxHydroCam
     */
    GeoluxHydroCam(Stream& stream, int8_t powerPin, Logger& baseLogger,
                   int8_t powerPin2, const char* imageResolution = "1600x1200",
                   const char* filePrefix      = nullptr,
                   bool        alwaysAutoFocus = false);
    /**
     * @brief Destroy the Geolux HydroCam object
     */
    ~GeoluxHydroCam();

    /**
     * @brief Extra unique function to retrieve the name of the last saved image
     *
     * @return The name of the last saved image
     */
    String getLastSavedImageName();

    String getSensorLocation(void) override;

    /**
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * This sets the image resolution and runs an autofocus.
     *
     * @note Autofocus takes approximately 30s to complete.  Unless you plan to
     * move your device between readings, I recommend only running the autofocus
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
     * For the Geolux HydroCam, this also sets the image resolution.
     *
     * @note This does NOT include any wait for sensor readiness.
     *
     * @return True if the wake function completed successfully.
     */
    bool wake(void) override;
    bool sleep(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;

    /**
     * @copydoc Sensor::isWarmedUp(bool debug)
     *
     * For the Geolux camera, this waits for both the power-on warm up and for
     * an affirmative from the camera that it is ready to accept commands.
     */
    bool isWarmedUp(bool debug = false) override;

    /**
     * @brief Check whether or not enough time has passed between the camera
     * wake and being ready to image.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that enough time has passed that the camera is
     * ready to take an image.
     *
     * @note We override the default function because the amount of time
     * required for imaging depends on the camera's mood and whether or not you
     * autofocus on every reading.
     */
    bool isStable(bool debug = false) override;

    /**
     * @brief Check whether or not the camera has completed imaging.
     *
     * @param debug True to output the result to the debugging Serial
     * @return True indicates that the camera is now reporting ready after an
     * image was started.
     *
     * @note We override the default function because the amount of time
     * required for imaging depends on the resolution.
     */
    bool isMeasurementComplete(bool debug = false) override;

 private:
    const char*
        _imageResolution;  ///< The image resolution from the Geolux HydroCam
    const char*
        _filePrefix;   ///< The prefix to add to files from the Geolux HydroCam
    String _filename;  ///< The filename of the last saved file from the Geolux
                       ///< HydroCam
    /**
     * @brief True to autofocus before every image. This may be necessary if the
     * camera is power cycled between images. If you are not power cycling or
     * moving the camera, I recommend not autofocusing often because the
     * autofocus takes about 30s.
     */
    bool _alwaysAutoFocus;
    /**
     * @brief An internal reference to an SdFat file instance
     */
    File imgFile;
    /**
     * @brief The internal pointer to the logger instance to be used.
     */
    Logger* _baseLogger = nullptr;
    /**
     * @brief Private reference to the stream for communication with the
     * Geolux Camera.
     */
    Stream* _stream;
    /**
     * @brief Private reference to the underlying GeoluxCamera Instance
     */
    GeoluxCamera _camera;
    /**
     * @brief Private reference to the last time the camera status was checked
     */
    uint32_t _last_status_check;
    /**
     * @brief Check whether the camera is ready.
     *
     * The camera is ready if the status is either GeoluxCamera::OK or
     * GeoluxCamera::NONE.
     *
     * Before checking the status, this function will ensure that enough time
     * has passed between messages so we don't hammer the camera too hard with
     * requests. This function will also update the #_last_status_check
     *
     * @param startTime The time to use as the start time to calculate the total
     * elapsed time in waiting; defaults to 0, which uses the last modbus
     * command time.  This is only used for debugging output.
     *
     * @return True if the camera is ready, false otherwise.
     */
    bool isCameraReady(uint32_t startTime);
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [image size output](@ref sensor_hydrocam_image_size) from a
 * [Geolux HydroCam camera](@ref sensor_hydrocam).
 *
 * This variable represents the difference between the number of bytes expected
 * to receive from the camera and the number actually transferred to the SD
 * card.
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


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [byte error output](@ref sensor_hydrocam_byte_error) from a
 * [Geolux HydroCam camera](@ref sensor_hydrocam).
 *
 * @ingroup sensor_hydrocam
 */
/* clang-format on */
class GeoluxHydroCam_ByteError : public Variable {
 public:
    /**
     * @brief Construct a new GeoluxHydroCam_ByteError object.
     *
     * @param parentSense The parent GeoluxHydroCam providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "HydroCamByteError".
     */
    explicit GeoluxHydroCam_ByteError(
        GeoluxHydroCam* parentSense, const char* uuid = "",
        const char* varCode = HYDROCAM_ERROR_DEFAULT_CODE)
        : Variable(parentSense, (uint8_t)HYDROCAM_ERROR_VAR_NUM,
                   (uint8_t)HYDROCAM_ERROR_RESOLUTION, HYDROCAM_ERROR_VAR_NAME,
                   HYDROCAM_ERROR_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new GeoluxHydroCam_ByteError object.
     *
     * @note This must be tied with a parent GeoluxHydroCam before it can be
     * used.
     */
    GeoluxHydroCam_ByteError()
        : Variable((uint8_t)HYDROCAM_ERROR_VAR_NUM,
                   (uint8_t)HYDROCAM_ERROR_RESOLUTION, HYDROCAM_ERROR_VAR_NAME,
                   HYDROCAM_ERROR_UNIT_NAME, HYDROCAM_ERROR_DEFAULT_CODE) {}
    /**
     * @brief Destroy the GeoluxHydroCam_ByteError object - no action
     * needed.
     */
    ~GeoluxHydroCam_ByteError() {}
};
/**@}*/
#endif  // SRC_SENSORS_GEOLUXHYDROCAM_H_

// cSpell:ignore dataloggers QQVGA QVGA QXGA UXGA autofocusing
