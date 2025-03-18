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

#ifdef MS_S3PRESIGNEDPUBLISHER_DEBUG
#define MS_DEBUGGING_STD "S3PresignedPublisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"


// ============================================================================
//  Functions for the S3 by way of a pre-signed URL.
// ============================================================================
/**
 * @brief The S3PresignedPublisher subclass of dataPublisher is for publishing
 * data to S3 over a pre-signed URL. The URL must be refreshed before every send
 * or the same file will be over-written. The absolute maxiumum time a
 * pre-signed URL could be valid for is 7 days, but they're frequently only
 * valid for a few minutes.
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
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    explicit S3PresignedPublisher(Logger& baseLogger, int sendEveryX = 1);
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
                         int sendEveryX = 1);
    /**
     * @brief Construct a new S3 Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param getUrlFxn A function to call to get a new pre-signed URL
     * @param filename The name of the file to be uploaded
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    S3PresignedPublisher(Logger& baseLogger,
                         const char* (*getUrlFxn)(const char*),
                         const char* filename, int sendEveryX = 1);
    /**
     * @brief Construct a new S3 Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param getUrlFxn A function to call to get a new pre-signed URL
     * @param filename The name of the file to be uploaded
     * @param caCertName The name of your certificate authority certificate
     * file
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     *
     * @note The inputs to this is the **NAME** of the certificate **file** as
     * it is stored on you modem module, not the actual certificate content.
     */
    S3PresignedPublisher(Logger& baseLogger,
                         const char* (*getUrlFxn)(const char*),
                         const char* filename, const char* caCertName,
                         int sendEveryX = 1);
    /**
     * @brief Construct a new S3 Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param getUrlFxn A function to call to get a new pre-signed URL
     * @param filename The name of the file to be uploaded
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    S3PresignedPublisher(Logger& baseLogger, Client* inClient,
                         const char* (*getUrlFxn)(const char*),
                         const char* filename, int sendEveryX = 1);
    /**
     * @brief Destroy the S3 Publisher object
     */
    virtual ~S3PresignedPublisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String("S3");
    }

    /**
     * @brief Set funtion to use to get the new URL
     *
     * @param getUrlFxn A function to call to get a new pre-signed URL
     */
    void setURLUpdateFunction(const char* (*getUrlFxn)(const char*));

    /**
     * @brief Set the pre-signed S3 url
     *
     * @param s3Url The pre-signed URL to use to put into an S3 bucket
     */
    void setPreSignedURL(const char* s3Url);

    /**
     * @brief Set the filename to upload
     *
     * @param filename The name of the file to be uploaded
     */
    void setFilename(const char* filename);

    /**
     * @brief Set the name of your certificate authority certificate file.
     *
     * You MUST have already uploaded your certificate to your modem. This will
     * most likely be the Amazon Root CA 1 (RSA 2048 bit key) certificate. You
     * can find Amazon's current CA certificates here:
     * https://docs.aws.amazon.com/iot/latest/developerguide/server-authentication.html
     *
     * @param caCertName The name of your certificate authority certificate
     * file.
     */
    void setCACertName(const char* caCertName);

    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param s3Url The pre-signed URL to use to put into an S3 bucket
     */
    void begin(Logger& baseLogger, Client*                        inClient,
               const char* (*getUrlFxn)(const char*), const char* filename);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param s3Url The pre-signed URL to use to put into an S3 bucket
     */
    void begin(Logger&     baseLogger, const char* (*getUrlFxn)(const char*),
               const char* filename, const char* caCertName = nullptr);

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
    static const char* contentLengthHeader;  ///< The content length header text
    static const char* contentTypeHeader;    ///< The content type header text


 private:
    /**
     * @brief Private reference to function used fetch a new S3 URL.
     */
    const char* (*_getUrlFxn)(const char*) = nullptr;
    /**
     * @brief A pointer to the S3 pre-signed URL
     */
    const char* _PreSignedURL = nullptr;
    /**
     * @brief The name of the file you want to upload to S3
     */
    const char* _filename = nullptr;
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
