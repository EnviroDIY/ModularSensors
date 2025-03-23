/**
 * @file AWS_IoT_Publisher.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the AWS_IoT_Publisher subclass of dataPublisher for
 * publishing data to AWS IoT Core using the MQTT protocol.
 *
 * This publisher is meant for a generic AWS IoT Core MQTT connection. It is NOT
 * for a specific AWS IoT Core channel or for any specific website or
 * application that uses IoT Core.  See the ReadMe and AWS IoT Core example for
 * more documentation on how to set up an AWS IoT Core instance to receive data
 * from this publisher.
 *
 * Your loggerID will be used as your Thing Name, primary topic, and client name
 * for connection. This publisher expects that you use a connection/publish
 * policy that is tied to your thing name. An example policy is in the
 * extras/AWS_IoT_SetCertificates folder.
 *
 * All messages are published with QoS 0 and no messages are retained.  All
 * connections are made with a clean session.
 *
 * @warning You cannot connect more than one device with the same ClientID to
 * AWS IoT at the same time. This means you should NEVER program two loggers
 * with the same loggerID with the same sketch. They will not both be able to
 * connect.
 *
 * @note At this time, this library has only very rudimentary support for
 * subscriptions and and data reception.  You can subscribe to up to
 * #MS_AWS_IOT_PUBLISHER_SUB_COUNT topics and set a callback function for
 * incoming messages that will be passed to the underlieing PubSubClient
 * instance.  This is **ABSOLUTELY NOT** intended for handling any messages
 * except those received over a very brief connection. The MQTT client only
 * stays open briefly while publishing messages! (Until you call
 * AWS_IoT_Publisher::closeConnection() or until the time has exceeded
 * #MS_AWS_IOT_MAX_CONNECTION_TIME, whichever comes first).  This publisher
 * will completely **BLOCK** all further action while waiting for messages.
 * The connection is closed at all other times and the board is in deep
 * sleep. This will only be good for receiving *retained* messages on topics
 * (that would come through immediately after subscribing) or for receiving
 * messages that are published as an immediate reaction to publishing data.  The
 * only tested use case for this is requesting a new S3 presigned URL for image
 * uploads.
 *
 * @warning AWS IoT Core is only available on the AWS free tier within the first
 * 12 months after initial sign up. After the first 12 months, you will be
 * billed for usage. You are responsible for any and all charges incurred while
 * using AWS IoT Core.
 */

// Header Guards
#ifndef SRC_PUBLISHERS_AWS_IOT_PUBLISHER_H_
#define SRC_PUBLISHERS_AWS_IOT_PUBLISHER_H_

// Include config before anything else
#include "ModSensorConfig.h"

// Debugging Statement
// #define MS_AWS_IOT_PUBLISHER_DEBUG

#ifdef MS_AWS_IOT_PUBLISHER_DEBUG
#define MS_DEBUGGING_STD "AWS_IoT_Publisher"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include "dataPublisherBase.h"
#include "PubSubClient.h"


// ============================================================================
//  Functions for AWS IoT Core over MQTT
// ============================================================================
/**
 * @brief The AWS_IoT_Publisher subclass of dataPublisher for publishing data
 * to AWS IoT Core using the MQTT protocol.
 *
 * @ingroup the_publishers
 */
class AWS_IoT_Publisher : public dataPublisher {
 public:
    // Constructors
    /**
     * @brief Construct a new AWS IoT Core Publisher object with no members
     * initialized.
     */
    AWS_IoT_Publisher();
    /**
     * @brief Construct a new AWS IoT Core Publisher object
     *
     * @note If a client is never specified, the publisher will attempt to
     * create and use a client on a LoggerModem instance tied to the attached
     * logger.
     *
     * @param baseLogger The logger supplying the data to be published
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    explicit AWS_IoT_Publisher(Logger& baseLogger, int sendEveryX = 1);
    /**
     * @brief Construct a new AWS IoT Core Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    AWS_IoT_Publisher(Logger& baseLogger, Client* inClient, int sendEveryX = 1);
    /**
     * @brief Construct a new AWS IoT Core Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     * @param caCertName The name of your certificate authority certificate
     * file
     * @param clientCertName The name of your client certificate file
     * @param clientKeyName The name of your client private key file
     * @param samplingFeatureUUID The sampling feature UUID for the site on the
     * Monitor My Watershed data portal.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     *
     * @note The inputs to this are the **NAMES** of the certificate **files**
     * as they are stored on you modem module, not the content of the
     * certificates.
     */
    AWS_IoT_Publisher(Logger& baseLogger, const char* awsIoTEndpoint,
                      const char* caCertName, const char* clientCertName,
                      const char* clientKeyName,
                      const char* samplingFeatureUUID, int sendEveryX = 1);
    /**
     * @brief Construct a new AWS IoT Core Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     * @param caCertName The name of your certificate authority certificate
     * file
     * @param clientCertName The name of your client certificate file
     * @param clientKeyName The name of your client private key file
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     *
     * @note The inputs to this are the **NAMES** of the certificate **files**
     * as they are stored on you modem module, not the content of the
     * certificates.
     */
    AWS_IoT_Publisher(Logger& baseLogger, const char* awsIoTEndpoint,
                      const char* caCertName, const char* clientCertName,
                      const char* clientKeyName, int sendEveryX = 1);
    /**
     * @brief Construct a new AWS IoT Core Publisher object
     *
     * @param baseLogger The logger supplying the data to be published
     * @param inClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     * @param samplingFeatureUUID The sampling feature UUID for the site on the
     * Monitor My Watershed data portal.
     * @param sendEveryX Interval (in units of the logging interval) between
     * attempted data transmissions. NOTE: not implemented by this publisher!
     */
    AWS_IoT_Publisher(Logger& baseLogger, Client* inClient,
                      const char* awsIoTEndpoint,
                      const char* samplingFeatureUUID, int sendEveryX = 1);
    /**
     * @brief Destroy the AWS IoT Core Publisher object
     */
    virtual ~AWS_IoT_Publisher();

    // Returns the data destination
    String getEndpoint(void) override {
        return String(_awsIoTEndpoint);
    }

    /**
     * @brief Set the endpoint for your AWS IoT instance.
     *
     * Get the broker host/endpoint from AWS IoT Core > Connect > Domain
     * Configurations.  The endpoint will be something like
     * "xxx-ats.iot.your-region.amazonaws.com".
     *
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance.
     */
    void setEndpoint(const char* awsIoTEndpoint);

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
     * This is exactly the same CA certificate as you would use to upload to S3
     * (ie, the S3 Presigned Publisher). For supported modules you can use the
     * AWS_IOT_SetCertificates sketch in the extras folder to upload your
     * certificate.
     *
     * @param caCertName The name of your certificate authority certificate
     * file.
     */
    void setCACertName(const char* caCertName);

    /**
     * @brief Set the name of your client certificate file.
     *
     * You MUST have already uploaded your certificate to your modem. You must
     * download the certificate when you create it. Your certificate should be
     * tied to a security policy that allows connection, publishing, and
     * subscribing by thing name.
     *
     * For supported modules you can use the AWS_IOT_SetCertificates sketch in
     * the extras folder to upload your certificate.
     *
     * @param clientCertName The name of your client certificate file.
     */
    void setClientCertName(const char* clientCertName);

    /**
     * @brief Set the name of your client private key file
     *
     * You MUST have have already uploaded your certificate to your modem. You
     * must download the certificate when you create it. Your certificate should
     * be tied to a security policy that allows connection, publishing, and
     * subscribing by thing name.
     *
     * For supported modules you can use the AWS_IOT_SetCertificates sketch in
     * the extras folder to upload your certificate.
     *
     * @param clientKeyName The name of your client private key file.
     */
    void setClientKeyName(const char* clientKeyName);

    /**
     * @brief Sets all of the AWS IoT Core parameters
     *
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     * @param caCertName The name of your certificate authority certificate
     * file
     * @param clientCertName The name of your client certificate file
     * @param clientKeyName The name of your client private key file
     */
    void setAWSIoTParams(const char* awsIoTEndpoint, const char* caCertName,
                         const char* clientCertName, const char* clientKeyName);
    /**
     * @brief Set the topic to use for publishing data.
     *
     * If not specified, the topic "{LoggerID}/{SamplingFeatureUUID}" will be
     * used.
     *
     * Make sure you have IAM policies set up to allow your device to publish to
     * the specified topic!
     *
     * @param topic The topic
     */
    void setDataPublishTopic(const char* topic);
    /**
     * @brief Set the topic to use for publishing metadata.
     *
     * If not specified, the topic "{LoggerID}/metadata" will be used for the
     * main logger metadata. For each variable, the variable number will be
     * appended to the topic (ie, "{LoggerID}/metadata/variable01").
     *
     * Make sure you have IAM policies set up to allow your device to publish to
     * the specified topics!
     *
     * @param topic The topic
     */
    void setMetadataPublishTopic(const char* topic);

    /**
     * @brief Adds a topic to subscribe to.
     *
     * When publishing data, the publisher will subscribe to the topic.
     *
     * @warning This is **ABSOLUTELY NOT** intended for handling any messages
     * except those received over a very brief connection. The MQTT client only
     * stays open briefly while publishing messages! (Until you call
     * AWS_IoT_Publisher::closeConnection() or until the time has exceeded
     * #MS_AWS_IOT_MAX_CONNECTION_TIME, whichever comes first).  This publisher
     * will completely **BLOCK** all further action while waiting for messages.
     * The connection is closed at all other times and the board is in deep
     * sleep. This will only be good for receiving *retained* messages on topics
     * (that would come through immediately after subscribing) or for receiving
     * messages that are published as an immediate reaction to publishing data.
     *
     * @note The publisher will only subscribe to up to
     * #MS_AWS_IOT_PUBLISHER_SUB_COUNT topics.
     *
     * @note The publisher will not directly unsubscribe from topics, but all
     * subscriptions are cleared at disconnect because we use a clean session.
     *
     * @param topic The topic to subscribe to
     */
    void addSubTopic(const char* topic);
    /**
     * @brief Removes a topic from the subscription list.
     *
     * I don't expect this to be used, but I'm adding it just in case.
     *
     * @note The publisher will not directly unsubscribe from topics, but all
     * subscriptions are cleared at disconnect because we use a clean session.
     *
     * @param topic The topic to remove from the subscription list
     */
    void removeSubTopic(const char* topic);

    /**
     * @brief Adds a topic to subscribe to.
     *
     * When publishing data, the publisher will subscribe to the topic.
     *
     * @warning This is **ABSOLUTELY NOT** intended for handling any messages
     * except those received over a very brief connection. The MQTT client only
     * stays open briefly while publishing messages! (Until you call
     * AWS_IoT_Publisher::closeConnection() or until the time has exceeded
     * #MS_AWS_IOT_MAX_CONNECTION_TIME, whichever comes first).  This publisher
     * will completely **BLOCK** all further action while waiting for messages.
     * The connection is closed at all other times and the board is in deep
     * sleep. This will only be good for receiving *retained* messages on topics
     * (that would come through immediately after subscribing) or for receiving
     * messages that are published as an immediate reaction to publishing data.
     *
     * @note The publisher will only subscribe to up to
     * #MS_AWS_IOT_PUBLISHER_SUB_COUNT topics.
     *
     * @note The publisher will not directly unsubscribe from topics, but all
     * subscriptions are cleared at disconnect because we use a clean session.
     *
     * @param topic The topic to publish to. This should be a static value. If
     * you wish to change the topic, remove the old topic and add a new one.
     * @param contentGetrFxn A function to call to get the content to publish.
     * The function should return a pointer to a char array.
     */
    void addPublishRequest(const char* topic, String (*contentGetrFxn)(void));
    /**
     * @brief Removes a topic from the publish list.
     *
     * @param topic The topic to remove from the publish list
     */
    void removePublishRequest(const char* topic);

    /**
     * @brief A passthrough to the PubSubClient setCallback function; sets a
     * function to call back when data is received on any of the subscribed
     * topics.
     *
     * This must be a static or main level function! It **cannot** be a member
     * function of a class!  Your callback function should call this publishers
     * AWS_IoT_Publisher::closeConnection() method immedialy after getting your
     * desired response to stop the publisher from waiting for more messages.
     *
     * @warning This is **ABSOLUTELY NOT** intended for handling any messages
     * except those received over a very brief connection. The MQTT client only
     * stays open briefly while publishing messages! (Until you call
     * AWS_IoT_Publisher::closeConnection() or until the time has exceeded
     * #MS_AWS_IOT_MAX_CONNECTION_TIME, whichever comes first).  This publisher
     * will completely **BLOCK** all further action while waiting for messages.
     * The connection is closed at all other times and the board is in deep
     * sleep. This will only be good for receiving *retained* messages on topics
     * (that would come through immediately after subscribing) or for receiving
     * messages that are published as an immediate reaction to publishing data.
     *
     * @param mqtt_callback The function to call when any message is received
     */
    PubSubClient& setCallback(MQTT_CALLBACK_SIGNATURE);

    void closeConnection();

    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     */
    void begin(Logger& baseLogger, Client* inClient, const char* awsIoTEndpoint,
               const char* samplingFeatureUUID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger, Client* inClient)
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     */
    void begin(Logger& baseLogger, Client* inClient,
               const char* awsIoTEndpoint);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     * @param caCertName The name of your certificate authority certificate
     * file
     * @param clientCertName The name of your client certificate file
     * @param clientKeyName The name of your client private key file
     */
    void begin(Logger& baseLogger, const char* awsIoTEndpoint,
               const char* caCertName, const char* clientCertName,
               const char* clientKeyName, const char* samplingFeatureUUID);
    /**
     * @copydoc dataPublisher::begin(Logger& baseLogger)
     * @param awsIoTEndpoint The endpoint for your AWS IoT instance
     * @param caCertName The name of your certificate authority certificate
     * file
     * @param clientCertName The name of your client certificate file
     * @param clientKeyName The name of your client private key file
     */
    void begin(Logger& baseLogger, const char* awsIoTEndpoint,
               const char* caCertName, const char* clientCertName,
               const char* clientKeyName);

    /**
     * @brief Utilize an attached modem to open a SSL connection to AWS IoT Core
     * and publish data over that connection.
     *
     * This depends on an internet connection already having been made and a
     * client being available.
     *
     * @param outClient An Arduino client instance to use to print data to.
     * Allows the use of any type of client and multiple clients tied to a
     * single TinyGSM modem instance
     * @param forceFlush Ask the publisher to flush buffered data immediately.
     * @return The PubSubClient status code of the response.
     */
    int16_t publishData(Client* outClient, bool forceFlush = false) override;
    int16_t publishMetadata(Client* outClient) override;

 protected:
    /**
     * @brief The MQTT port; this is always 8883 for AWS IoT Core MQTT
     * connections
     */
    static const int   mqttPort;
    static const char* samplingFeatureTag;  ///< The JSON feature UUID tag
    static const char* timestampTag;        ///< The JSON feature timestamp tag
    virtual Client*    createClient() override;
    virtual void       deleteClient(Client* _client) override;

 private:
    // Keys for AWS IoT Core
    /**
     * @brief The endpoint for your AWS IoT instance
     */
    const char* _awsIoTEndpoint;
    /**
     * @brief The name of your certificate authority certificate file
     */
    const char* _caCertName;
    /**
     * @brief The name of your client certificate file
     */
    const char* _clientCertName;
    /**
     * @brief The name of your client private key file
     */
    const char* _clientKeyName;
    const char* _dataTopic;      ///< The topic for data
    const char* _metadataTopic;  ///< The topic for metadata
    /**
     * @brief True to continue waiting for subscriptions after publishing data
     */
    bool _waitForSubs;
    /**
     * @brief Internal reference to the PubSubClient instance for MQTT
     * communication.
     */
    PubSubClient _mqttClient;
    /**
     * @brief An array of topics to subscribe to
     */
    const char* sub_topics[MS_AWS_IOT_PUBLISHER_SUB_COUNT];
    /**
     * @brief An array of topics to publish to
     */
    const char* pub_topics[MS_AWS_IOT_PUBLISHER_PUB_COUNT];
    /**
     * @brief An array of functions to call to get publish content
     */
    String (*contentGetrFxns[MS_AWS_IOT_PUBLISHER_PUB_COUNT])(void);
    /// constructor helper
    void init();
};

#endif  // SRC_PUBLISHERS_AWS_IOT_PUBLISHER_H_
