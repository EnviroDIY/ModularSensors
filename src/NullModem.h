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

class TinyGsmUndefined
{

public:

class GsmClient : public Client
{
  friend class TinyGsmUndefined;

public:
  GsmClient() {}
  GsmClient(TinyGsmUndefined& modem, uint8_t mux = 0) { init(&modem, mux); }
  bool init(TinyGsmUndefined* modem, uint8_t mux = 0) { return false; }

public:
  virtual int connect(const char *host, uint16_t port) { return 0; }
  virtual int connect(IPAddress ip, uint16_t port) { return 0; }
  virtual void stop() {}
  virtual size_t write(const uint8_t *buf, size_t size) { return 0; }
  virtual size_t write(uint8_t c) { return 0; }
  virtual int available() { return 0; }
  virtual int read(uint8_t *buf, size_t size) { return 0; }
  virtual int read() { return 0; }
  virtual int peek() { return 0; }
  virtual void flush() {}
  virtual uint8_t connected() { return 0; }
  virtual operator bool() { return connected(); }
};

public:

  TinyGsmUndefined(Stream& stream)
    : stream(stream)
  {}
  bool begin() { return false;}
  bool init() { return false; }
  bool testAT(uint32_t timeout = 10000L) { return false; }
  int getSignalQuality() { return 0; }
  bool isNetworkConnected() { return false; }
  bool waitForNetwork(uint32_t timeout = 60000L) { return false; }
  bool networkConnect(const char* ssid, const char* pwd) { return false; }
  bool networkDisconnect() { return false; }
  bool gprsConnect(const char* apn, const char* user = "", const char* pw = ""){ return false; }
  bool gprsDisconnect() { return false;}

  private:
    Stream&       stream;
};

typedef TinyGsmUndefined TinyGsm;
typedef TinyGsmUndefined::GsmClient TinyGsmClient;

#endif
