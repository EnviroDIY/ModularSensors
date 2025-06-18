/**************************************************************
 * @example{lineno} AWS_IoT_SetCertificates.ino
 * @copyright Stroud Water Research Center
 * @license This example is published under the BSD-3 license.
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief An program to load certificates for AWS IoT Core on to your modem to
 * use for later connection.
 *
 * You should run this program once to load your certificates and confirm that
 * you can connect to AWS IoT Core over MQTT.  Once you have confirmed your
 * certificates are loaded and working, there is no reason to rerun this program
 * unless you have a new modem, reset your modem, or your certificates change.
 * Most modules store the certificates in flash, which has a limited number of
 * read/write cycles. To avoid wearing out the flash unnecessarily, you should
 * only run this program when necessarily, don't re-write the certificates every
 * time you want to connect to AWS IoT Core.
 *
 * @note This only works for modules that have support for both using and
 * **loading** certificates in TinyGSM. Modules that support SSL, but not
 *writing certificates, cannot use this example!
 *
 * @m_examplenavigation{extra_aws_certificates,}
 **************************************************************/

// Select your modem:
// #define TINY_GSM_MODEM_SIM7000SSL
#define TINY_GSM_MODEM_SIM7080
// #define TINY_GSM_MODEM_A7672X
// #define TINY_GSM_MODEM_ESP32
// #define TINY_GSM_MODEM_SEQUANS_MONARCH

#define TINY_GSM_KEEPALIVE 180

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include "aws_iot_config.h"

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT SerialBee

// or Software Serial on Uno, Nano
#else
#include <SoftwareSerialMon.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
// #define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 921600

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet.
// This is only needed for this example, not in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]      = "hologram";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourWiFiSSID";
const char wifiPass[] = "YourWiFiPassword";

// MQTT details
// get the broker host/endpoint from AWS IoT Core / Connect / Domain
// Configurations
const char* broker = AWS_IOT_ENDPOINT;
// the secure connection port for MQTT is always 8883
uint16_t port = 8883;
// the client ID should be the name of your "thing" in AWS IoT Core
const char* clientId = THING_NAME;

static const char topicInit[] TINY_GSM_PROGMEM = THING_NAME "/init";

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClientSecure secureClient(modem);
PubSubClient        mqtt(secureClient);

#define LED_PIN 13
int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

boolean mqttConnect() {
    SerialMon.print("Connecting to ");
    SerialMon.print(broker);
    SerialMon.print(" with client ID ");
    SerialMon.println(clientId);

    // Connect to MQTT Broker
    boolean status = mqtt.connect(clientId);

    if (status == false) {
        SerialMon.println(" ...failed to connect to MQTT broker!");
        return false;
    }
    SerialMon.println(" ...success");

    SerialMon.print("Publishing a message to ");
    SerialMon.println(topicInit);
    mqtt.publish(topicInit, "{\"" THING_NAME "\":\"connected\"}");

    return mqtt.connected();
}


void setup() {
    // Set console baud rate
    SerialMon.begin(921600);
    delay(10);

    pinMode(LED_PIN, OUTPUT);

    // !!!!!!!!!!!
    // Set your reset, enable, power pins here
    // pins
    int8_t _modemPowerPin   = 18;  // Mayfly 1.1
    int8_t _modemSleepRqPin = 23;  // Mayfly 1.1
    int8_t _modemStatusPin  = 19;  // Mayfly 1.1
    // set pin modes
    pinMode(_modemPowerPin, OUTPUT);
    pinMode(_modemSleepRqPin, OUTPUT);
    pinMode(_modemStatusPin, INPUT);
    // wake settings
    uint32_t _wakeDelay_ms = 1000L;  // SIM7080G
    uint32_t _wakePulse_ms = 1100L;  // SIM7080G
    bool     _wakeLevel =
        HIGH;  // SIM7080G is low, but EnviroDIY LTE Bee inverts it

    // start with the modem powered off
    DBG(F("Starting with modem powered down. Wait..."));
    digitalWrite(_modemSleepRqPin, !_wakeLevel);
    digitalWrite(_modemPowerPin, LOW);
    delay(5000L);

    // power the modem
    DBG(F("Powering modem with pin"), _modemPowerPin, F("and waiting"),
        _wakeDelay_ms, F("ms for power up."));
    digitalWrite(_modemPowerPin, HIGH);

#ifndef TINY_GSM_MODEM_ESP32
    delay(_wakeDelay_ms);  // SIM7080G wake delay
    // wake the modem
    DBG(F("Sending a"), _wakePulse_ms, F("ms"),
        _wakeLevel ? F("HIGH") : F("LOW"), F("wake-up pulse on pin"),
        _modemSleepRqPin);
    digitalWrite(_modemSleepRqPin, _wakeLevel);
    delay(_wakePulse_ms);  // >1s
    digitalWrite(_modemSleepRqPin, !_wakeLevel);
    // !!!!!!!!!!!
#endif

    DBG("Wait...");
    delay(500L);

    // Set GSM module baud rate
    TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
    // SerialAT.begin(57600);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.print("Initializing modem...");
    if (!modem.restart()) {  // modem.init();
        SerialMon.println(" ...failed to initialize modem!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");

    // Max out the baud rate, if desired
    // NOTE: Do this **AFTER** the modem has been restarted - many modules
    // revert to default baud rates when reset or powered off. 921600, 460800,
    // 230400, 115200
    modem.setBaud(921600);
    SerialAT.end();
    delay(100);
    SerialAT.begin(921600);
    delay(100);
    modem.init();  // May need to re-init to turn off echo, etc

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);
    String modemManufacturer = modem.getModemManufacturer();
    SerialMon.print("Modem Manufacturer: ");
    SerialMon.println(modemManufacturer);
    String modemModel = modem.getModemModel();
    SerialMon.print("Modem Model: ");
    SerialMon.println(modemModel);
    String modemRevision = modem.getModemRevision();
    SerialMon.print("Modem Revision: ");
    SerialMon.println(modemRevision);

#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }
#endif

#if TINY_GSM_USE_WIFI
    // Wifi connection parameters must be set before waiting for the network
    SerialMon.print(F("Setting SSID/password..."));
    if (!modem.networkConnect(wifiSSID, wifiPass)) {
        SerialMon.println(" ...failed to connect to WiFi!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");
#endif

    // ======================== CERTIFICATE NAMES ========================
    // The certificates are stored in the "certificates.h" file

    const char* root_ca     = AWS_SERVER_CERTIFICATE;
    const char* client_cert = AWS_CLIENT_CERTIFICATE;
    const char* client_key  = AWS_CLIENT_PRIVATE_KEY;

    // The certificates should generally be formatted as ".pem", ".der", or (for
    // some modules) ".p7b" files.

    // For most modules the actual filename doesn't matter much but it CANNOT
    // HAVE SPACES and should be less than 64 characters.
    // NOTE: The certificate names as they are downloaded from AWS IoT Core
    // are often too long for the modem to handle. Pick something shorter, like
    // your thing name.

    // For Espressif modules, only two certificate sets are supported and the
    // certificates must be named "client_ca.{0|1}", "client_cert.{0|1}", or
    // "client_key.{0|1}"
#ifdef TINY_GSM_MODEM_ESP32
    const char* root_ca_name     = "client_ca.0";
    const char* client_cert_name = "client_cert.0";
    const char* client_key_name  = "client_key.0";
#else
    const char* root_ca_name     = "AmazonRootCA1.pem";
    const char* client_cert_name = THING_NAME "-certificate.pem.crt";
    const char* client_key_name  = THING_NAME "-private-key.pem.key";
#endif

    // ======================== CA CERTIFICATE LOADING ========================
    bool ca_cert_success = true;
    // add the server's certificate authority certificate to the modem
    SerialMon.print("Loading Certificate Authority Certificate");
    ca_cert_success &= modem.loadCertificate(root_ca_name, root_ca,
                                             strlen(root_ca));
    if (!ca_cert_success) {  // modem.init();
        SerialMon.println(" ...failed to load CA certificate!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");
    // print out the certificate to make sure it matches
    SerialMon.println(
        "Printing Certificate Authority Certificate to confirm it matches");
    modem.printCertificate(root_ca_name, SerialMon);
    // convert the certificate to the modem's format
    SerialMon.print("Converting Certificate Authority Certificate");
    ca_cert_success &= modem.convertCACertificate(root_ca_name);
    if (!ca_cert_success) {  // modem.init();
        SerialMon.println(" ...failed to convert CA certificate!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");

// NOTE: some modems suggest that you delete the certificate file from the
// file system after converting the certificate.  Do NOT do this with an
// ESP32!  The certificate must be in the file system to be used. On Espressif
// modules, the certificate must be in the file system to be used.
#ifndef TINY_GSM_MODEM_ESP32
    // ca_cert_success &= modem.deleteCertificate(root_ca_name);
#endif

    // ======================= CLIENT CERTIFICATE LOADING
    // ======================= add the client's certificate and private key to
    // the modem
    bool client_cert_success = true;
    SerialMon.print("Loading Client Certificate");
    client_cert_success &= modem.loadCertificate(client_cert_name, client_cert,
                                                 strlen(client_cert));
    ca_cert_success &= modem.loadCertificate(root_ca_name, root_ca,
                                             strlen(root_ca));
    // print out the certificate to make sure it matches
    modem.printCertificate(client_cert_name, SerialMon);
    SerialMon.print(" and Client Private Key ");
    client_cert_success &= modem.loadCertificate(client_key_name, client_key,
                                                 strlen(client_key));
    // print out the certificate to make sure it matches
    modem.printCertificate(client_key_name, SerialMon);
    if (!ca_cert_success) {  // modem.init();
        SerialMon.println(" ...failed to load client certificate or key!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");
    // convert the client certificate pair to the modem's format
    client_cert_success &= modem.convertClientCertificates(client_cert_name,
                                                           client_key_name);
    ca_cert_success &= modem.convertCACertificate(root_ca_name);
    if (!ca_cert_success) {  // modem.init();
        SerialMon.println(" ...failed to convert client certificate and key!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");

    // NOTE: some modems suggest that you delete the certificate file from the
    // file system after converting the certificate.  Do NOT do this with an
    // ESP32!  On Espressif modules, the certificate must be in the file system
    // to be used.
#ifndef TINY_GSM_MODEM_ESP32
    // cert_success &= modem.deleteCertificate(client_cert_name);
    // cert_success &= modem.deleteCertificate(client_key_name);
#endif

    // =================== SET CERTIFICATES FOR THE CONNECTION
    // =================== AWS IoT Core requires mutual authentication
    DBG("Requiring mutual authentication on socket");
    secureClient.setSSLAuthMode(SSLAuthMode::MUTUAL_AUTHENTICATION);
    // attach the uploaded certificates to the secure client
    DBG("Assigning", root_ca_name, "as certificate authority on socket");
    secureClient.setCACertName(root_ca_name);
    DBG("Assigning", client_cert_name, "as client certificate on socket");
    secureClient.setClientCertName(client_cert_name);
    DBG("Assigning", client_key_name, "as client key on socket");
    secureClient.setPrivateKeyName(client_key_name);

    // =================== WAIT FOR NETWORK REGISTRATION ===================
#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
    // The XBee must run the gprsConnect function BEFORE waiting for network!
    // All other modules must wait for network first.
    modem.gprsConnect(apn, gprsUser, gprsPass);
#endif
    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        SerialMon.println(" ...failed to connect to network!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");

    if (modem.isNetworkConnected()) { SerialMon.println("Network connected"); }

    // ====================== MAKE DATA CONNECTION =======================
#if TINY_GSM_USE_GPRS
    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.println(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" ...failed to connect to GPRS!");
        delay(10000);
        return;
    }
    SerialMon.println(" ...success");

    if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }
#endif

    // enable/force time sync with NTP server
    // This is **REQUIRED** for validated SSL connections
    DBG("Enabling time sync with NTP server");
    modem.NTPServerSync("pool.ntp.org", -5);

    // MQTT Broker setup
    mqtt.setServer(broker, port);

    delay(500);
    DBG("Finished setup");
}

void loop() {
    // Make sure we're still registered on the network
    if (!modem.isNetworkConnected()) {
        SerialMon.println("Network disconnected");
        if (!modem.waitForNetwork(180000L, true)) {
            SerialMon.println(" ...failed to reconnect to network!");
            delay(10000);
            return;
        }
        if (modem.isNetworkConnected()) {
            SerialMon.println("Network re-connected");
        }

#if TINY_GSM_USE_GPRS
        // and make sure GPRS/EPS is still connected
        if (!modem.isGprsConnected()) {
            SerialMon.println("GPRS disconnected!");
            SerialMon.print(F("Connecting to "));
            SerialMon.println(apn);
            if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
                SerialMon.println(" ...failed to reconnect to GPRS!");
                delay(10000);
                return;
            }
            if (modem.isGprsConnected()) {
                SerialMon.println("GPRS reconnected");
            }
        }
#endif
    }

    if (!mqtt.connected()) {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 10000L) {
            lastReconnectAttempt = t;
            if (mqttConnect()) { lastReconnectAttempt = 0; }
        }
        delay(100);
        return;
    }

    mqtt.loop();
}
