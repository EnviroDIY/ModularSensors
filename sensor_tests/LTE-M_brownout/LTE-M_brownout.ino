// ==========================================================================
//    Modem MCU Type and TinyGSM Client
// ==========================================================================

// Select your modem chip - this determines the exact commands sent to it
#define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

// Create a reference to the serial port for the modem
HardwareSerial &modemSerial = Serial1;  // Use hardware serial if possible

// Create a variable for the modem baud rate - this will be used in the begin function for the port
const long ModemBaud = 9600;  // SARA-U201 default seems to be 9600

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
// TinyGsm *tinyModem = new TinyGsm(modemSerial);

// Use this if you want to spy on modem communication
#include <StreamDebugger.h>
StreamDebugger modemDebugger(modemSerial, Serial);
TinyGsm *tinyModem = new TinyGsm(modemDebugger);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);

// Describe the physical pin connection of your modem to your board
const int8_t modemVccPin = -2;  // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)

// Network connection information
const char *apn = "hologram";  // The APN for the gprs connection, unnecessary for WiFi

void setup()
{
    Serial.begin(115200);
    Serial1.begin(ModemBaud);
    Serial.println("all pins output LOW");

    pinMode(modemVccPin, OUTPUT);
    digitalWrite(19, LOW);
    pinMode(modemSleepRqPin, OUTPUT);
    digitalWrite(20, LOW);
    pinMode(modemStatusPin, INPUT);

    int triesMade = 0;
    bool success = false;
    while (!success and triesMade < 5) {
      // Cannot send anything for 1 "guard time" before entering command mode
      delay(1000);
      tinyModem->streamWrite(GF("+++"));  // enter command mode
      success = (1 == tinyModem->waitResponse(2000, GF("OK\r")));
      triesMade ++;
    }
    if (success)
    {
        tinyModem->sendAT(F("AP"),5);  // Put into bypass mode
        // tinyModem->sendAT(F("AP"),0);  // Put into transparent mode
        tinyModem->waitResponse(GF("OK\r"));
        tinyModem->sendAT(GF("WR"));  // Write changes to flash
        tinyModem->waitResponse(GF("OK\r"));
        tinyModem->sendAT(GF("AC"));  // Apply changes
        tinyModem->waitResponse(GF("OK\r"));
        tinyModem->sendAT(GF("FR"));  // Force a reset to ensure we're in bypass mode
        tinyModem->waitResponse(GF("OK\r"));
        tinyModem->sendAT(GF("CN"));  // Exit command mode
        tinyModem->waitResponse(GF("OK\r"));

        delay(1000);  // wait for the reset
    }
}

void loop()
{
    // delay(1000);
    // tinyModem->streamWrite(GF("+++"));  // enter command mode
    // tinyModem->waitResponse(2000, GF("OK\r"));
    // tinyModem->sendAT(GF("DB"));  // Exit command mode
    // tinyModem->waitResponse(GF("\r"));
    // tinyModem->sendAT(GF("AI"));  // Exit command mode
    // tinyModem->waitResponse(GF("\r"));
    // tinyModem->sendAT(GF("CN"));  // Exit command mode
    // tinyModem->waitResponse(GF("OK\r"));
    // tinyModem->sendAT(GF("+CSQ"));
    // tinyModem->waitResponse(GF("OK\r\n"));
    // tinyModem->sendAT(GF("+CGREG?"));
    // tinyModem->waitResponse(GF("OK\r\n"));
    tinyModem->waitForNetwork(60000L);
}
