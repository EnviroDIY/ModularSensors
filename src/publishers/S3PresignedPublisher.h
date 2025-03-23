/**
 * @file S3PresignedPublisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the S3PresignedPublisher subclass of dataPublisher for
 * publishing data to S3 using a pre-signed URL.
 */

// Header Guards
#ifndef SRC_PUBLISHERS_S3PRESIGNEDPUBLISHER_H_
#define SRC_PUBLISHERS_S3PRESIGNEDPUBLISHER_H_

// Include config before anything else
#include "ModSensorConfig.h"

// Debugging Statement
// #define MS_S3PRESIGNEDPUBLISHER_DEBUG
// #define MS_S3PRESIGNEDPUBLISHER_DEBUG_DEEP

#ifdef MS_S3PRESIGNEDPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "S3PresignedPublisher"
#endif

#ifdef MS_S3PRESIGNEDPUBLISHER_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "dataPublisherBase"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the S3 by way of a pre-signed URL.
// ============================================================================
/**
 * @brief The S3PresignedPublisher subclass of dataPublisher is for publishing
 * data to S3 over a pre-signed URL.
 *
 * Publishing data directly to S3 requires you to use a
 * [SigV4(a)](https://docs.aws.amazon.com/IAM/latest/UserGuide/reference_sigv.html)
 * signature in every request. Generating that URL signature is complex and
 * requires communicating back and forth with temporary connections to get an
 * access key and then using a series of calculations to get a final signing
 * key. AWS support *strongly* recommends you don't try to generate this without
 * one of their SDKs. There is a [embedded C
 * SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/main) for AWS
 * with a [SigV4 component
 * library](https://github.com/aws/SigV4-for-AWS-IoT-embedded-sdk) that can be
 * used in embedded systems or with FreeRTOS, but there is not an Arduino port
 * of it, nor do many Arduino boards meet the memory requirements for it.
 *
 * If you cannot generate your own SigV4 signed request, you can use a
 * _pre-signed URL_ which is valid for a short time to access a single object
 * (file) in a single S3 bucket. The pre-signed URL will give you the same
 * permissions to the S3 object as the generator of the URL.  There are a few
 * caveats to using a pre-signed URL:
 * - The URL is only valid for a single object (file). While you can reuse the
 * URL, repeatedly using the same URL will over-write the same file it will not
 * create a new one.
 *   - To write to a new file, you need a new URL.
 * - The URL is only valid for a short time, usually only a few minutes.
 *   - The absolute maxiumum time a pre-signed URL could be valid for is 7 days,
 * but they're frequently only valid for a few minutes or hours at most.
 *
 * So, to publish to S3, this publisher provides a few options:
 * - You can provide a set URL and set filename to use for the post. The set
 * file will be read from the SD card. The exact URL will be used for the post.
 * If you do not re-call the functions to update the URL and filename
 * externally, the same S3 object will be over-written until the URL expires.
 * - You can provide a set filename to use for the post and a function to call
 * to get a new pre-signed URL. The set file will be read from the SD and
 * the function will be called to generate a new URL based on that provided
 * filename. If you do not call the function to update the filename
 * externally, the same S3 object will be over-written with each post because
 * the filename has not changed. But there should be no URL expiration because a
 * new pre-signed URL will be generated each time.
 * - You can provide a function to call to get a new pre-signed URL and no
 * filename. A filename will be automatically generated based on the logger ID
 * and the last marked date/time, assuming the file is of type
 * `#S3_DEFAULT_FILE_EXTENSION` (".jpg"). The function to get a new pre-signed
 * URL will be called based on the automatic filename. A new object will be
 * created on S3 using the new url and logger/jpg based filename.
 * - You can provide a function to call to get a new pre-signed URL and a file
 * prefix and/or extension to use to generate a filename based on the date/time.
 * The function to get a new pre-signed URL will be called based on the
 * generated filename. A new object will be created on S3 using the new url and
 * prefix/extension based filename.
 * - You can provide a function to call to get a new pre-signed URL and a
 * function to call to get a new filename. Both functions will be called and the
 * upload to S3 will be based on the returned URL and filename.
 *   - NOTE: If your function to call to get the filename returns a nullptr, the
 * default filename based on the logger ID, datetime, and
 * `#S3_DEFAULT_FILE_EXTENSION` will be attempted.
 *
 * @note While there is no physical or AWS-based requirement that the filename
 * on the SD card matches the object name on S3, this library only supports the
 * case where they match.
 *
 * The current tested path is:
 * - The logger creates an image file on the SD card with the GeoLux camera (and
 * also appends numeric data to a csv). The image file is named based on the
 * logger ID and the *marked* date time from the sampling interval.
 * - The logger calls the AWS IoT publisher
 * - The AWS IoT publisher, while sending numeric data to AWS IoT Core,
 * subscribes to its own S3 url topic.
 * - The IoT publisher publishes a filename named based on the
 * logger ID and the *marked* date time from the sampling interval to a
 * basic-ingest topic tied to an IoT Core rule.
 * - The AWS IoT Core rule triggers a lambda function to generate a pre-signed
 * URL for the image file.
 * - The lambda publishes the URL to the logger's pre-signed URL topic.
 * - The subscribed IoT publisher receives the URL and passes it to the S3
 * publisher.
 * - The logger calls the S3 publisher
 * - The S3 publisher uses the logger to connect to the SD card and verify that
 * it can open and read the GeoLux image file.
 * - The S3 publisher uses the pre-signed URL to upload the GeoLux image file to
 * S3.
 *
 * Further documentation on how to set up the AWS IoT rule and lambda function
 * and all of the proper permissions are forthcoming.
 *
 * @todo Generate documentation on the AWS architecture for IoT Core to S3.
 *
 * @ingroup the_publishers
 */
class S3PresignedPublisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new S3 Publisher object with no members set.
     */
    S3PresignedPublisher();
    /**
     * @brief Construct a new S3 Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param caCertName The name of your certificate authority certificate
     * file - used to validate the server's certificate when connecting to S3
     * with SSL. @see setCACertName()
     * @param getUrlFxn A function to call to get a new pre-signed URL
     * @param getFileNameFxn A function to call to get a new filename
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     *
     * @note The inputs to this is the **NAME** of the certificate **file** as
     * it is stored on you modem module, not the actual certificate content.
     */
    S3PresignedPublisher(Logger& baseLogger, const char* caCertName,
                         String (*getUrlFxn)(String)    = nullptr,
                         String (*getFileNameFxn)(void) = nullptr,
                         int sendEveryX                 = 1);
    /**
     * @brief Construct a new S3 Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    S3PresignedPublisher(Logger& baseLogger, Client* inClient,
                         String (*getUrlFxn)(String)    = nullptr,
                         String (*getFileNameFxn)(void) = nullptr,
                         int sendEveryX                 = 1);
    /**
     * @brief Destroy the S3 Publisher object
     */
    virtual ~S3PresignedPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(s3_parent_host);
    }

    /**
     * @brief Set funtion to use to get the new URL
     *
     * @param getUrlFxn A function to call to get a new pre-signed URL
     */
    void setURLUpdateFunction(String (*getUrlFxn)(String));

    /**
     * @brief Set the pre-signed S3 url
     *
     * @param s3Url The pre-signed URL to use to put into an S3 bucket
     */
    void setPreSignedURL(String s3Url);

    /**
     * @brief Set the filename to upload. If the filename is set, this exact
     * filename will be used for the upload.
     *
     * @param filename The name of the file to be uploaded
     */
    void setFileName(String filename);

    /**
     * @brief Set the filename parameters to use to auto-generate the filename
     * before every post based on the file extension, prefix, and the current
     * date and time.
     *
     * @param extension The file extension to use
     * @param filePrefix The prefix to use for the file name, optional, with an
     * default value of nullptr. If not provided, the logger ID will be used.
     */
    void setFileParams(const char* extension, const char* filePrefix = nullptr);

    /**
     * @brief Set funtion to use to get the new URL
     *
     * @param getFileNameFxn A function to call to get a new filename
     */
    void setFileUpdateFunction(String (*getFileNameFxn)(void));

    /**
     * @brief Set the name of your certificate authority certificate file.
     *
     * You MUST have already uploaded your certificate to your modem. This will
     * most likely be the Amazon Root CA 1 (RSA 2048 bit key) certificate. You
     * can find Amazon's current CA certificates here:
     * https://docs.aws.amazon.com/iot/latest/developerguide/server-authentication.html.
     * Depending on your module, you may instead need a certificate chain file
     * or to use Amazon's older top-chain certificate (Starfield Services Root
     * Certificate Authority - G2).
     *
     * This is exactly the same CA certificate as you would use for an MQTT
     * connection to AWS IoT (ie, the AWS IoT Publisher). For supported modules
     * you can use the AWS_IOT_SetCertificates sketch in the extras folder to
     * upload your certificate.
     *
     * @param caCertName The name of your certificate authority certificate
     * file.
     *
     * @note The inputs to this is the **NAME** of the certificate **file** as
     * it is stored on you modem module, not the actual certificate content.
     */
    void setCACertName(const char* caCertName);

    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param s3Url The pre-signed URL to use to put into an S3 bucket
     */
    void begin(Logger& baseLogger, Client* inClient,
               String (*getUrlFxn)(String));
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param s3Url The pre-signed URL to use to put into an S3 bucket
     */
    void begin(Logger&     baseLogger, String (*getUrlFxn)(String),
               const char* caCertName = nullptr);

    /**
     * @brief Utilizes an attached modem to make a TCP connection to the
     * S3 URL and then stream out a get request over that connection.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param forceFlush Ask the publisher to flush buffered data immediately.
     *
     * @return The http status code of the response.
     */
    int16_t publishData(Client* outClient, bool forceFlush = false) override;

 protected:
    virtual Client*    createClient() override;
    virtual void       deleteClient(Client* _client) override;
    static const char* s3_parent_host;       ///< The host name
    static const int   s3Port;               ///< The host port
    static const char* contentLengthHeader;  ///< The content length header text
    static const char* contentTypeHeader;    ///< The content type header text


 private:
    /**
     * @brief Private reference to function used fetch a new S3 URL.
     */
    String (*_getUrlFxn)(String) = nullptr;
    /**
     * @brief A pointer to the S3 pre-signed URL
     */
    String _PreSignedURL;
    /**
     * @brief The name of the file you want to upload to S3
     */
    String _filename;
    /**
     * @brief  The prefix to add to files, if generating a filename based on the
     * date/time.
     *
     * @note This will be *ignored* if the filename is set. If neither the
     * filename nor the file prefix is set, the logger ID will be used.
     */
    const char* _filePrefix;
    /**
     * @brief  The extension to add to files, if generating a filename based on
     * the date/time
     *
     * @note This will be *ignored* if the filename is set. If neither the
     * filename nor the file extension is set, `#S3_DEFAULT_FILE_EXTENSION`.
     */
    const char* _fileExtension;
    /**
     * @brief Private reference to function used fetch a new file name.
     */
    String (*_getFileNameFxn)(void);
    /**
     * @brief The name of your certificate authority certificate file
     */
    const char* _caCertName = nullptr;
    /**
     * @brief An internal reference to an SdFat file instance
     */
    File putFile;
};

#endif  // SRC_PUBLISHERS_S3PRESIGNEDPUBLISHER_H_
