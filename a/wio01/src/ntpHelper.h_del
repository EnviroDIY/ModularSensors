
#ifndef SRC_NTP_HELPER_H_
#define SRC_NTP_HELPER_H_
#include "Arduino.h"
#include <AtWiFi.h>
const unsigned int localPort = 2390;      // local port to listen for UDP packets
#ifdef USELOCALNTP
    char timeServer[] = "n.n.n.n"; // local NTP server 
#else
    const char timeServer[] = "time.nist.gov"; // extenral NTP server e.g. time.nist.gov
#endif
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

class ntpHelper {
public:
     bool connectToWiFi(const char* ssid, const char* pwd);
     void printWifiStatus();
     unsigned long getNTPtime();

//#define byte char
uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

private :
unsigned long sendNTPpacket(const char* address) ;

};


#endif // SRC_NTP_HELPER_H_
