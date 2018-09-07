/**
 * @file       NullModem.h
 * @author     Volodymyr Shymanskyy
 * @license    LGPL-3.0
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date       Nov 2016
 */

#ifndef NullModem_h
#define NullModem_h

#include <Client.h>
#include <TinyGsmCommon.h>
#define NULL_MODEM_DEBUGGING_SERIAL_OUTPUT Serial

#ifdef NULL_MODEM_DEBUGGING_SERIAL_OUTPUT
    namespace {
        template<typename T>
        static void MS_MODN_DBG(T last) {
            NULL_MODEM_DEBUGGING_SERIAL_OUTPUT.print(last);
        }

        template<typename T, typename... Args>
        static void MS_MODN_DBG(T head, Args... tail) {
            NULL_MODEM_DEBUGGING_SERIAL_OUTPUT.print(head);
            MS_MODN_DBG(tail...);
        }
    }
#else
    #define MS_MODN_DBG(...)
#endif  // NULL_MODEM_DEBUGGING_SERIAL_OUTPUT


class TinyGsmUndefined
{

public:

class GsmClient : public Client
{
  friend class TinyGsmUndefined;

public:
  GsmClient() {}
  GsmClient(TinyGsmUndefined& modem, uint8_t mux = 0) {
    init(&modem, mux);
  }
  bool init(TinyGsmUndefined* modem, uint8_t mux = 0) {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR INIT!\n"));
    return false;
  }

public:
  virtual int connect(const char *host, uint16_t port) {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR CONNECT!\n"));
    return 0;
  }
  virtual int connect(IPAddress ip, uint16_t port) {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR CONNECT!\n"));
    return 0;
  }
  virtual void stop() {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR STOP!\n"));
  }
  virtual size_t write(const uint8_t *buf, size_t size) {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR WRITE!\n"));
    return 0;
  }
  virtual size_t write(uint8_t c) {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR WRITE!\n"));
    return 0;
  }
  virtual size_t write(const char *str) {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR WRITE!\n"));
    return 0;
  }
  virtual int available() {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR AVAILABLE!\n"));
    return 0;
  }
  virtual int read(uint8_t *buf, size_t size) {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR READ!\n"));
    return 0;
  }
  virtual int read() {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR READ!\n"));
    return 0;
  }
  virtual int peek() {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR PEEK!\n"));
    return 0;
  }
  virtual void flush() {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR FLUSH!\n"));
  }
  virtual uint8_t connected() {
    MS_MODN_DBG(F("USING EMPTY CLIENT IMPLEMENTATION FOR CONNECTED!\n"));
    return 0;
  }
  virtual operator bool() { return connected(); }
};
public:

  TinyGsmUndefined(Stream& stream)
    : stream(stream)
  {}
  bool begin() {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR begin!\n"));
    return false;
  }
  bool init() {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR init!\n"));
    return false;
  }
  bool testAT(unsigned long timeout = 10000L) {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR testAT!\n"));
    return false;
  }

  /*
   * Power functions
   */
  /*
   * SIM card functions
   */
  int getSignalQuality() {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR getSignalQuality!\n"));
    return false;
  }
  bool isNetworkConnected() {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR isNetworkConnected!\n"));
    return false;
  }
  bool waitForNetwork(unsigned long timeout = 60000L) {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR waitForNetwork!\n"));
    return false;
  }

  /*
   * WiFi functions
   */
  bool networkConnect(const char* ssid, const char* pwd) {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR networkConnect!\n"));
    return false;
  }
  bool networkDisconnect() {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR networkDisconnect!\n"));
    return false;
  }

  /*
   * GPRS functions
   */
  bool gprsConnect(const char* apn, const char* user = "", const char* pw = "") {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR gprsConnect!\n"));
    return false;
  }
  bool gprsDisconnect() {
    MS_MODN_DBG(F("USING EMPTY MODEM IMPLEMENTATION FOR gprsDisconnect!\n"));
    return false;
  }

  /*
   * Messaging functions
   */
  /*
   * Location functions
   */
  /*
   * Battery functions
   */

  private:
    Stream&       stream;
};

typedef TinyGsmUndefined TinyGsm;
typedef TinyGsmUndefined::GsmClient TinyGsmClient;

#endif
