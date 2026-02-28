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

// Include the library config before anything else
#include "ModSensorConfig.h"

// Include the debugging config
#include "ModSensorDebugConfig.h"

// Define the print label[s] for the debugger
#ifdef MS_DATAPUBLISHERBASE_DEBUG
#define MS_DEBUGGING_STD "dataPublisherBase"
#endif
#ifdef MS_DATAPUBLISHERBASE_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "dataPublisherBase"
#endif

// Include the debugger
#include "ModSensorDebugger.h"
// Undefine the debugger label[s]
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP

// Include other in-library and external dependencies
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
     * @brief Construct a new data publisher object untied to any logger or
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
     * @brief Construct a new data publisher object using a single client
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
     * @brief Construct a new data publisher object.
     *
     * @note When creating a data publisher with a pre-existing client object,
     * you must ensure that your client supports SSL/TLS if the publisher
     * requires it.
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
     * @brief Destroy the data publisher object - no action is taken.
     */
    virtual ~dataPublisher();

    /**
     * @brief Set the Client object.
     *
     * Gives the publisher a client instance to use to "print" data.
     *
     * @note This can be used to set a client that is not tied to a logger
     * modem or to TinyGSM!
     *
     * @param inClient A pointer to an Arduino client instance
     */
    void setClient(Client* inClient);

    /**
     * @brief Sets the interval (in units of the logging interval) between
     * attempted data transmissions
     *
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. Not respected by all publishers.
     */
    void setSendInterval(int sendEveryX);

    /**
     * @brief Attach the publisher to a logger.
     *
     * The publisher must be tied to a data logger to provide it with the data
     * to be published.
     *
     * @param baseLogger A reference to the ModularSensors logger instance
     */
    void attachToLogger(Logger& baseLogger);

    /**
     * @brief Set the pointer to a loggerModem instance.
     *
     * @param modemPointer A reference to the ModularSensors loggerModem
     * instance
     */
    void setModemPointer(loggerModem& modemPointer);

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
     * @return The URL or HOST to receive published data
     */
    virtual String getEndpoint(void) = 0;


    /**
     * @brief Checks if the publisher needs an internet connection for the next
     * publishData call (as opposed to just buffering data internally).
     *
     * @return True if an internet connection is needed for the next publish.
     */
    virtual bool connectionNeeded(void);

    /**
     * @brief Opens a socket to the correct receiver and sends out the formatted
     * data.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param forceFlush Ask the publisher to flush buffered data immediately.
     * @return The result of publishing data.  May be an http response code or a
     * result code from PubSubClient.
     */
    virtual int16_t
    publishData(Client* outClient,
                bool    forceFlush = MS_ALWAYS_FLUSH_PUBLISHERS) = 0;
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
     * @return The result of publishing data.  May be an http response code or a
     * result code from PubSubClient. Defaults to -2 if the publisher fails to
     * create a client.
     */
    virtual int16_t publishData(bool forceFlush = MS_ALWAYS_FLUSH_PUBLISHERS);

    /**
     * @copydoc publishMetadata()
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     */
    virtual int16_t publishMetadata(Client* outClient);
    /**
     * @brief Open a socket to the correct receiver and send out *metadata*
     * about the current logger setup.
     *
     * This is to be used only when the logger is starting up!  As the metadata
     * doesn't generally change after start up, there's no reason to call this
     * function any other time.  There is no sensor data included in this
     * metadata!  If there is metadata that has to be included with each data
     * post, that should be included in the publishData function.  Because there
     * is no sensor data included in this metadata and it's only to be called at
     * boot, there's no `forceFlush` option for this function.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @note This does *not* have to be implemented for each publisher! If it is
     * not implemented, it will return 0.
     *
     * @return The result of publishing data.  May be an http response code or a
     * result code from PubSubClient.
     */
    virtual int16_t publishMetadata();

    /**
     * @brief Retained for backwards compatibility; use publishData(Client*
     * outClient) in new code.
     *
     * @m_deprecated_since{0,22,5}
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @return The result of publishing data.  May be an http response code or a
     * result code from PubSubClient.
     */
    virtual int16_t sendData(Client* outClient);
    /**
     * @brief Retained for backwards compatibility; use publishData() in new
     * code.
     *
     * @m_deprecated_since{0,22,5}
     *
     * @return The result of publishing data.  May be an http response code or a
     * result code from PubSubClient.
     */
    virtual int16_t sendData();

    /**
     * @brief Translate a PubSubClient code into a String with the code
     * explanation.
     *
     * @param state A result code returned by a PubSubClient action
     * @return The meaning of the code
     */
    String parseMQTTState(int state);


 protected:
    /**
     * @brief The internal pointer to the logger instance to be used.
     */
    Logger* _baseLogger = nullptr;
    /**
     * @brief The internal pointer to the base logger's modem instance to be
     * used.
     */
    loggerModem* _baseModem = nullptr;
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
     * @param debug_flush If true, flush the TX buffer to the debugging port.
     */
    static void txBufferAppend(const char* data, size_t length,
                               bool debug_flush = true);
    /**
     * @brief Append the given string to the TX buffer, flushing if necessary.
     *
     * @param s The null-terminated string to append.
     * @param debug_flush If true, flush the TX buffer to the debugging port.
     */
    static void txBufferAppend(const char* s, bool debug_flush = true);
    /**
     * @brief Append the given char to the TX buffer, flushing if necessary.
     *
     * @param c The char to append.
     * @param debug_flush If true, flush the TX buffer to the debugging port.
     */
    static void txBufferAppend(char c, bool debug_flush = true);
    /**
     * @brief Write the TX buffer contents to the initialized stream and also to
     * the debugging port.
     * @param debug_flush If true, flush the TX buffer to the debugging port.
     */
    static void txBufferFlush(bool debug_flush = true);

    /**
     * @brief Use the connected base logger's logger modem and underlying
     * TinyGSM instance to create a new client for the publisher.
     *
     * @note This does *NOT* override any client set by the
     * constructor/begin/setClient functions. If you set an external client by
     * any of those methods, this function should not be called. It is protected
     * to prevent being called by external code.
     *
     * @note The default implementation of this function creates an insecure
     * client. Publishers that require SSL must re-implement this function.
     *
     * @return A pointer to an Arduino client instance
     */
    virtual Client* createClient();
    /**
     * @brief Delete a created client. We need to pass this through to avoid a
     * memory leak because we cannot delete from the pointer because the
     * destructor for a client in the Arduino core isn't virtual.
     *
     * @note The client must be deleted by the same type of modem that created
     * it.  This is unlikely to be an issue unless you're trying to use two
     * modems on the same logger.
     *
     * @param client The client to delete
     */
    virtual void deleteClient(Client* client);

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
     * @brief the text "PUT "
     */
    static const char* putHeader;
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
