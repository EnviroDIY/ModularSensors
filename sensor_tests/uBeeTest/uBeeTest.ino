// ==========================================================================
//    Modem MCU Type and TinyGSM Client
// ==========================================================================

// Select your modem chip - this determines the exact commands sent to it
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
#define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
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

bool wakeFxn(void){return true;}  // Turns on when power is applied
bool sleepFxn(void)
{
    // if (modemSleepRqPin < 0) return tinyModem->poweroff();
    // else
    // {
    //     digitalWrite(modemSleepRqPin, LOW);
    //     digitalWrite(redLED, HIGH);
    //     Serial.println("LOW");
    //     delay(1100);  // >1s
    //     digitalWrite(modemSleepRqPin, HIGH);
    //     digitalWrite(redLED, LOW);
    //     Serial.println("HIGH");
    //     return true;
    // }
    Serial.println(F("Powering off via AT command."));
    return tinyModem->poweroff();
}

// Network connection information
const char *apn = "hologram";  // The APN for the gprs connection, unnecessary for WiFi

void setup()
{
    Serial.begin(115200);
    Serial1.begin(ModemBaud);
    Serial.println("all pins output LOW");

    pinMode(19, OUTPUT);
    digitalWrite(19, LOW);
    pinMode(20, OUTPUT);
    digitalWrite(20, LOW);
    pinMode(23, OUTPUT);
    digitalWrite(23, LOW);

    bool success = tinyModem->testAT(7000L);
    Serial.print("All pins low: ");
    Serial.println(success);
    // tinyModem->poweroff();


    // Serial.println("setting 19 high");
    // pinMode(19, OUTPUT);
    // digitalWrite(19, HIGH);
    //
    // success = tinyModem->testAT(7000L);
    // Serial.print("Pin 19 HIGH: ");
    // Serial.println(success);
    // tinyModem->poweroff();

    // Serial.println("setting 20 high");
    // digitalWrite(19, LOW);
    // delay(15000L);
    // digitalWrite(20, HIGH);
    //
    // success = tinyModem->testAT(7000L);
    // Serial.print("Pin 20 HIGH: ");
    // Serial.println(success);
    // tinyModem->poweroff();


    // Serial.println("setting 23 high");
    // digitalWrite(20, LOW);
    // delay(15000L);
    // digitalWrite(23, HIGH);
    //
    // success = tinyModem->testAT(7000L);
    // Serial.print("Pin 23 HIGH: ");
    // Serial.println(success);
    // tinyModem->poweroff();

    // Serial.println("setting 23 low again");
    // digitalWrite(23, LOW);
    // delay(15000L);
    //
    // success = tinyModem->testAT(7000L);
    // Serial.print("Pin 23 LOW: ");
    // Serial.println(success);
    // tinyModem->poweroff();

    Serial.println("setting 20 & 23 high, 19 input");
    digitalWrite(20, HIGH);
    digitalWrite(23, HIGH);
    pinMode(19, INPUT);
    delay(15000L);

    success = tinyModem->testAT(7000L);
    Serial.print("Pin 20 & 23 HIGH: ");
    Serial.println(success);
    Serial.print("Pin 19 state: ");
    Serial.println(digitalRead(19));
    // tinyModem->poweroff();

    Serial.println("outputting a 1 sec low pulse on 20");
    digitalWrite(20, LOW);
    delay(1100);
    digitalWrite(20, HIGH);
    delay(15000L);

    success = tinyModem->testAT(7000L);
    Serial.print("After pin 20 low pulse: ");
    Serial.println(success);
    Serial.print("Pin 19 state: ");
    Serial.println(digitalRead(19));
    // tinyModem->poweroff();

    Serial.println("outputting a 60µs low pulse on 20");
    digitalWrite(20, LOW);
    delayMicroseconds(60);
    digitalWrite(20, HIGH);
    delay(15000L);

    success = tinyModem->testAT(7000L);
    Serial.print("After pin 20 low pulse: ");
    Serial.println(success);
    Serial.print("Pin 19 state: ");
    Serial.println(digitalRead(19));
    // tinyModem->poweroff();

    Serial.println("outputting a 1 sec low pulse on 20");
    digitalWrite(20, LOW);
    delay(1100);
    digitalWrite(20, HIGH);
    Serial.println(millis());
    while (true)
    {
        if (!digitalRead(19))
        {
            Serial.println(millis());
            tinyModem->testAT(1000L);
            break;
        }
    }
    // tinyModem->poweroff();

    Serial.println("setting 20 & 23 high, 19 input pullup");
    digitalWrite(20, HIGH);
    digitalWrite(23, HIGH);
    pinMode(19, INPUT_PULLUP);
    delay(15000L);

    success = tinyModem->testAT(7000L);
    Serial.print("Pin 20 & 23 HIGH: ");
    Serial.println(success);
    Serial.print("Pin 19 state: ");
    Serial.println(digitalRead(19));
    // tinyModem->poweroff();

    Serial.println("outputting a 1 sec low pulse on 20");
    digitalWrite(20, LOW);
    delay(1100);
    digitalWrite(20, HIGH);
    delay(15000L);

    success = tinyModem->testAT(7000L);
    Serial.print("After pin 20 low pulse: ");
    Serial.println(success);
    Serial.print("Pin 19 state: ");
    Serial.println(digitalRead(19));
    // tinyModem->poweroff();

    Serial.println("outputting a 60µs low pulse on 20");
    digitalWrite(20, LOW);
    delayMicroseconds(60);
    digitalWrite(20, HIGH);
    delay(15000L);

    success = tinyModem->testAT(7000L);
    Serial.print("After pin 20 low pulse: ");
    Serial.println(success);
    Serial.print("Pin 19 state: ");
    Serial.println(digitalRead(19));
    // tinyModem->poweroff();

    Serial.println("outputting a 1 sec low pulse on 20");
    digitalWrite(20, LOW);
    delay(1100);
    digitalWrite(20, HIGH);
    Serial.println(millis());
    while (true)
    {
        if (!digitalRead(19))
        {
            Serial.println(millis());
            tinyModem->testAT(1000L);
            break;
        }
    }
    // tinyModem->poweroff();
}

void loop(){}
