/**
 * @file dataPublisherBase.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the dataPublisher class - a virtual class used by other
 * publishers to distribute data online.
 *
 * @copydetails dataPublisher
 */
/**
 * @defgroup the_publishers Supported Data Endpoints
 * All implemented data endpoint (publisher) classes
 *
 * @copydetails dataPublisher
 */

// Header Guards
#ifndef SRC_DATAPUBLISHERBASE_H_
#define SRC_DATAPUBLISHERBASE_H_

// Debugging Statement
// #define MS_DATAPUBLISHERBASE_DEBUG

#ifdef MS_DATAPUBLISHERBASE_DEBUG
#define MS_DEBUGGING_STD "dataPublisherBase"
#endif


#ifndef MS_SEND_BUFFER_SIZE
/**
 * @brief Send Buffer
 *
 * This determines how many characters to set out at once over the TCP
 * connection. Increasing this may decrease data use by a logger, while
 * decreasing it will save memory. Do not make it smaller than 32 or bigger
 * than 1500 (a typical TCP Maximum Transmission Unit).
 *
 * This can be changed by setting the build flag MS_SEND_BUFFER_SIZE when
 * compiling.
 */
#define MS_SEND_BUFFER_SIZE 750
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "LoggerBase.h"
#include "Client.h"

/**
 * @brief The dataPublisher class is a virtual class used by other publishers to
 * distribute data online.
 *
 * A dataPublisher is a abstract concept.  It is something that "watches" the
 * logger for new data and correctly formats and sends that data to some online
 * web service.
 *
 * @ingroup base_classes
 */
class dataPublisher {
 public:
    /**
     * @brief Construct a new data Publisher object untied to any logger or
     * client.
     *
     * @note Using the default empty constructor requires you to use a the
     * begin(Logger& baseLogger, Client* inClient) function to initialize
     * the logger and client values.  If a client is never specified, the
     * publisher will attempt to create and use a client on a LoggerModem
     * instance tied to the attached logger.  If a logger is never specified the
     * publisher will not function.
     */
    dataPublisher();
    /**
     * @brief Construct a new data Publisher object using a single client
     * created on the TinyGSM based logger modem.
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. Not respected by all publishers.
     */
    explicit dataPublisher(Logger& baseLogger, int sendEveryX = 1);
    /**
     * @brief Construct a new data Publisher object.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. Not respected by all publishers.
     */
    dataPublisher(Logger& baseLogger, Client* inClient, int sendEveryX = 1);
    /**
     * @brief Destroy the data Publisher object - no action is taken.
     */
    virtual ~dataPublisher();

    /**
     * @brief Set the Client object.
     *
     * Gives the publisher a client instance to use to "print" data.
     *
     * @param inClient A pointer to an Arduino client instance
     */
    void setClient(Client* inClient);

    /**
     * @brief Attach the publisher to a logger.
     *
     * The publisher must be tied to a data loger to provide it with the data to
     * be published.
     *
     * @param baseLogger A reference to the ModularSensors logger instance
     */
    void attachToLogger(Logger& baseLogger);
    /**
     * @brief Sets the interval (in units of the logging interval) between
     * attempted data transmissions
     *
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. Not respected by all publishers.
     */
    void setSendInterval(int sendEveryX);

    /**
     * @brief Begin the publisher - linking it to the client and logger.
     *
     * This can be used as an alternative to adding the logger and client in the
     * constructor.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     */
    void begin(Logger& baseLogger, Client* inClient);

    /**
     * @brief Begin the publisher - linking it to the logger but not
     * attaching a client.
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * This can be used as an alternative to adding the logger and client in the
     * constructor.
     *
     * @param baseLogger The logger supplying the data to be published
     */
    void begin(Logger& baseLogger);


    /**
     * @brief Get the destination for published data - generally the host name
     * of the data receiver.
     *
     * @return **String** The URL or HOST to receive published data
     */
    virtual String getEndpoint(void) = 0;


    /**
     * @brief Checks if the publisher needs an Internet connection for the next
     * publishData call (as opposed to just buffering data internally).
     *
     * @return True if an internet connection is needed for the next publish.
     */
    virtual bool connectionNeeded(void);

    /**
     * @brief Open a socket to the correct receiver and sends out the formatted
     * data.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param forceFlush Ask the publisher to flush buffered data immediately.
     * @return **int16_t** The result of publishing data.  May be an http
     * response code or a result code from PubSubClient.
     */
    virtual int16_t publishData(Client* outClient, bool forceFlush = false) = 0;
    /**
     * @brief Open a socket to the correct receiver and send out the formatted
     * data.
     *
     * This depends on an internet connection already having been made and
     * either a client having been linked to the publisher or a logger modem
     * having been linked to the logger linked to the publisher.
     *
     * @param forceFlush Ask the publisher to flush buffered data immediately.
     *
     * @return **int16_t** The result of publishing data.  May be an http
     * response code or a result code from PubSubClient.
     */
    virtual int16_t publishData(bool forceFlush = false);

    /**
     * @brief Retained for backwards compatibility; use publishData(Client*
     * outClient) in new code.
     *
     * @m_deprecated_since{0,22,5}
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @return **int16_t** The result of publishing data.  May be an http
     * response code or a result code from PubSubClient.
     */
    virtual int16_t sendData(Client* outClient);
    /**
     * @brief Retained for backwards compatibility; use publishData() in new
     * code.
     *
     * @m_deprecated_since{0,22,5}
     *
     * @return **int16_t** The result of publishing data.  May be an http
     * response code or a result code from PubSubClient.
     */
    virtual int16_t sendData();

    /**
     * @brief Translate a PubSubClient code into a String with the code
     * explanation.
     *
     * @param state A result code returned by a PubSubClient action
     * @return **String** The meaning of the code
     */
    String parseMQTTState(int state);


 protected:
    /**
     * @brief The internal pointer to the logger instance to be used.
     */
    Logger* _baseLogger = nullptr;
    /**
     * @brief The internal pointer to the client instance to be used.
     */
    Client* _inClient = nullptr;

    /**
     * @brief The buffer for outgoing data.
     */
    static char txBuffer[MS_SEND_BUFFER_SIZE];
    /**
     * @brief The pointer to the client instance the TX buffer is writing to.
     */
    static Client* txBufferOutClient;
    /**
     * @brief The number of used characters in the TX buffer.
     */
    static size_t txBufferLen;
    /**
     * @brief Initialize the TX buffer to be empty and start writing to the
     * given client.
     *
     * @param outClient The client to transmit to.
     */
    static void txBufferInit(Client* outClient);
    /**
     * @brief Append the given data to the TX buffer, flushing if necessary.
     *
     * @param data The data start pointer.
     * @param length The number of bytes to add.
     */
    static void txBufferAppend(const char* data, size_t length);
    /**
     * @brief Append the given string to the TX buffer, flushing if necessary.
     *
     * @param s The null-terminated string to append.
     */
    static void txBufferAppend(const char* s);
    /**
     * @brief Append the given char to the TX buffer, flushing if necessary.
     *
     * @param c The char to append.
     */
    static void txBufferAppend(char c);
    /**
     * @brief Write the TX buffer contents to the initialized stream and also to
     * the debugging port.
     */
    static void txBufferFlush();

    /**
     * @brief Interval (in units of the logging interval) between
     * attempted data transmissions. Not respected by all publishers.
     */
    int _sendEveryX = 1;

    // Basic chunks of HTTP
    /**
     * @brief the text "GET "
     */
    static const char* getHeader;
    /**
     * @brief the text "POST "
     */
    static const char* postHeader;
    /**
     * @brief the text " HTTP/1.1"
     */
    static const char* HTTPtag;
    /**
     * @brief the text "\r\nHost: "
     */
    static const char* hostHeader;
};

#endif  // SRC_DATAPUBLISHERBASE_H_
