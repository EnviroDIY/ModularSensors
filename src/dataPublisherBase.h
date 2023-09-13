/**
 * @file dataPublisherBase.h
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
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


/**
 * @def MS_SEND_BUFFER_SIZE
 * @brief Send Buffer
 *
 * This determines how many characters to set out at once over the TCP/UDP
 * connection.  Increasing this may decrease data use by a loger, while
 * decreasing it will save memory.  Do not make it smaller than 47 (to keep all
 * variable values with their UUID's) or bigger than 1500 (a typical TCP/UDP
 * Maximum Transmission Unit).
 *
 * This can be changed by setting the build flag MS_SEND_BUFFER_SIZE when
 * compiling.
 *
 * @ingroup the_publishers
 */
#ifndef MS_SEND_BUFFER_SIZE
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
     */
    explicit dataPublisher(Logger& baseLogger);
    /**
     * @brief Construct a new data Publisher object.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     */
    dataPublisher(Logger& baseLogger, Client* inClient);
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
     * @brief Begin the publisher - linking it to the client and logger.
     *
     * This can be used as an alternative to adding the logger and client in the
     * constructor.  This is slightly "safer" because we expect the publishers
     * to be created in the "global scope" and we cannot control the order in
     * which objects in that global scope will be created.  That is, we cannot
     * guarantee that the logger will actually be created before the publisher
     * that wants to attach to it unless we wait to attach the publisher until
     * in the setup or loop function of the main program.  In reality, it is
     * very unlikely that this is necessary.
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
     * constructor.  This is slightly "safer" because we expect the publishers
     * to be created in the "global scope" and we cannot control the order in
     * which objects in that global scope will be created.  That is, we cannot
     * guarantee that the logger will actually be created before the publisher
     * that wants to attach to it unless we wait to attach the publisher until
     * in the setup or loop function of the main program.  In reality, it is
     * very unlikely that this is necessary.
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
     * @brief Open a socket to the correct receiver and sends out the formatted
     * data.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @return **int16_t** The result of publishing data.  May be an http
     * response code or a result code from PubSubClient.
     */
    virtual int16_t publishData(Client* outClient) = 0;
    /**
     * @brief Open a socket to the correct receiver and send out the formatted
     * data.
     *
     * This depends on an internet connection already having been made and
     * either a client having been linked to the publisher or a logger modem
     * having been linked to the logger linked to the publisher.
     *
     * @return **int16_t** The result of publishing data.  May be an http
     * response code or a result code from PubSubClient.
     */
    virtual int16_t publishData();

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
     * @brief A buffer for outgoing data.
     */
    static char txBuffer[MS_SEND_BUFFER_SIZE];
    /**
     * @brief Get the number of empty spots in the buffer.
     *
     * @return **int** The number of available characters in the buffer
     */
    static int bufferFree(void);
    /**
     * @brief Fill the TX buffer with nulls ('\0').
     */
    static void emptyTxBuffer(void);
    /**
     * @brief Write the TX buffer to a stream and also to the debugging
     * port.
     *
     * @param stream A pointer to an Arduino Stream instance to use to print
     * data
     * @param addNewLine True to add a new line character ("\n") at the end of
     * the print
     */
    static void printTxBuffer(Stream* stream, bool addNewLine = false);

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

 public:
    /**
     * @brief Whether the publisher should use data stored in a text file queue
     * on the SD-card or use data values stored in the variable array to create
     * output for the publisher.  This routes subsequent POST construction.
     *
     */
    bool useQueueDataSource = false;
    /**
     * @brief Sets the publisher to use either data stored in a text file queue
     * on the SD-card or use data values stored in the variable array to create
     * output for the publisher.  This routes subsequent POST construction
     *
     * @param state - true for Queued, false for standard
     */
    void setQueuedState(bool state);
    /**
     * @brief Get the current data source for publisher output.
     *
     * @return *true*  The publisher is using data read from a text file on an
     * SD-card.
     * @return *false*  The publisher is using data directly from the variable
     * array.
     */
    bool getQueuedStatus();

    /**
     * @brief The time-out (in milliseconds) to wait for a response from the
     * data publisher
     */
    uint16_t _timerPostTimeout_ms;
    /**
     * @brief Set the time-out (in milliseconds) to wait for a response from the
     * data publisher
     *
     * @param tpt_ms The time-out (in milliseconds) to wait for a response from
     * the data publisher
     */
    void setTimerPostTimeout_mS(uint16_t tpt_ms);
    /**
     * @brief Get the current time-out (in milliseconds) to wait for a response
     * from the data publisher
     * @return *uint16_t* The current time-out (in milliseconds) to wait for a
     * response from the data publisher
     */
    uint16_t getTimerPostTimeout_mS();

    /**
     * @brief The pacing (in milliseconds) between subsequent outputs to the
     * publisher when posting multiple time-points of queued data.
     */
    uint16_t _timerPostPacing_ms;
    /**
     * @brief Set the pacing (in milliseconds) between subsequent outputs to the
     * publisher when posting multiple time-points of queued data.
     *
     * @param tpp_ms The pacing (in milliseconds) between subsequent outputs to
     * the publisher when posting multiple time-points of queued data.
     */
    void setTimerPostPacing_mS(uint16_t tpp_ms);
    /**
     * @brief Get the current pacing (in milliseconds) between subsequent
     * outputs to the publisher when posting multiple time-points of queued
     * data.
     *
     * @return *uint16_t* The current pacing (in milliseconds) between
     * subsequent outputs to the publisher when posting multiple time-points of
     * queued data.
     */
    uint16_t getTimerPostPacing_mS();
};

/* atl_extension */
/*
 * HTTP STATUS Codes that are used by Modular Sensors
 * Placed at the end of the file, to facilitate mergein code
 * https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
 */

#define HTTPSTATUS_CREATED_201 201
// Server Error indicating a Gateway Timeout.
// Server error that doesn't seem to receover
// https://github.com/ODM2/ODM2DataSharingPortal/issues/628
#define HTTPSTATUS_GT_500 500
// Also supplied if the server didn't respond to a POST
#define HTTPSTATUS_GT_504 504
// This is an internaly created error, indicating No Connection with server
#define HTTPSTATUS_NC_901 901
// internal error, not enough power to connect with server
#define HTTPSTATUS_NC_902 902
// internal error, value dumped
#define HTTPSTATUS_NC_903 903
// internal error, value queued
#define HTTPSTATUS_NC_904 904

#endif  // SRC_DATAPUBLISHERBASE_H_
