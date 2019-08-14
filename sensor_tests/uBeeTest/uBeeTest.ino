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
const long modemBaud = 9600;  // SARA U201 and SARA N211 default is 9600
// const long modemBaud = 115200;  // SARA R410M default is 115200

// Create a TinyGSM modem to run on that serial port
TinyGsm tinyModem(modemSerial);

// // Use this if you want to spy on modem communication
// #include <StreamDebugger.h>
// StreamDebugger modemDebugger(modemSerial, Serial);
// TinyGsm tinyModem(modemDebugger);

// Create a TCP client on that modem
TinyGsmClient tinyClient(tinyModem);

// Describe the physical pin connection of your modem to your board
const int8_t modemVccPin = -2;  // MCU pin controlling modem power (-1 if not applicable)
const int8_t modemSleepRqPin = 23;  // MCU pin used for modem sleep/wake request (-1 if not applicable)
const int8_t modemStatusPin = 19;   // MCU pin used to read modem status (-1 if not applicable)
const bool modemStatusLevel = HIGH;  // The level of the status pin when the module is active (HIGH or LOW)


bool uBeeTestAT(unsigned long timeout = 7000L) {
    for (unsigned long start = millis(); millis() - start < timeout; ) {
        modemSerial.begin(modemBaud);
        if (modemBaud > 57600) {
            tinyModem.setBaud(9600);
            modemSerial.end();
            modemSerial.begin(9600);
        }
        tinyModem.sendAT(GF(""));
        if (tinyModem.waitResponse(200) == 1) return true;
        delay(100);
    }
    return false;
}

bool wakeFxn(void){return true;}  // Turns on when power is applied
bool sleepFxn(void)
{
    // if (modemSleepRqPin < 0) return tinyModem.poweroff();
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
    return tinyModem.poweroff();
}

// Network connection information
const char *apn = "hologram";  // The APN for the gprs connection, unnecessary for WiFi

void setup()
{
    Serial.begin(115200);
    Serial1.begin(modemBaud);
    Serial.println("starting with all pins output LOW");
    Serial.println("failure expected");

    pinMode(19, OUTPUT);
    digitalWrite(19, LOW);
    pinMode(20, OUTPUT);
    digitalWrite(20, LOW);
    pinMode(23, OUTPUT);
    digitalWrite(23, LOW);

    bool success = uBeeTestAT();
    Serial.print("  All pins LOW: ");
    Serial.println(success);


    Serial.println("----------------\n");
    Serial.println("Single Pin Tests");
    Serial.println("\nsetting only 19 high");
    Serial.println("expect failure - no power");
    digitalWrite(19, HIGH);
    digitalWrite(20, LOW);
    digitalWrite(23, LOW);

    success = uBeeTestAT();
    Serial.print("  Pin 19 HIGH, 20 & 23 LOW: ");
    Serial.println(success);
    tinyModem.poweroff();

    Serial.println("\nsetting only 20 high");
    Serial.println("expect failure - no power");
    digitalWrite(19, LOW);
    digitalWrite(20, LOW);
    digitalWrite(23, LOW);
    delay(15000L);
    digitalWrite(20, HIGH);

    success = uBeeTestAT();
    Serial.print("  Pin 20 HIGH, 19 & 23 LOW: ");
    Serial.println(success);
    tinyModem.poweroff();


    Serial.println("----------------\n");
    Serial.println("More realistic tests");
    pinMode(19, INPUT);
    digitalWrite(20, LOW);
    digitalWrite(23, LOW);
    delay(15000L);

    Serial.println("\nsetting only 23 high");
    Serial.println("power module, hold PWR_ON (RTS) LOW");
    Serial.println("expect failure from SARA R410M (no pulse), success from U201 (on with power on)");
    digitalWrite(23, HIGH);

    success = uBeeTestAT();
    Serial.print("  Pin 23 HIGH, 19 & 20 LOW: ");
    Serial.println(success);
    Serial.print("  Pin 19 state: ");
    Serial.println(digitalRead(19));
    if (digitalRead(19))
    {
        Serial.println("Shutting back down with AT command");
        tinyModem.poweroff();
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }

    digitalWrite(20, LOW);
    digitalWrite(23, LOW);
    delay(15000L);


    Serial.println("\nsetting 20 & 23 high, 19 input");
    Serial.println("power module, hold PWR_ON (RTS) HIGH");
    Serial.println("expect failure from SARA R410M (no pulse), success from U201 (on with power on)");
    digitalWrite(20, HIGH);
    digitalWrite(23, HIGH);

    success = uBeeTestAT();
    Serial.print("  Pin 20 & 23 HIGH: ");
    Serial.println(success);
    Serial.print("  Pin 19 state: ");
    Serial.println(digitalRead(19));
    if (digitalRead(19))
    {
        Serial.println("Shutting back down with AT command");
        tinyModem.poweroff();
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }


    Serial.println("----------------\n");
    Serial.println("Pulsed tests");
    Serial.println("\noutputting a 60µs low pulse on 20");
    Serial.println("expect failure from SARA R410M (too short), success from U201");
    digitalWrite(20, LOW);
    delayMicroseconds(60);
    digitalWrite(20, HIGH);

    success = uBeeTestAT();
    Serial.print("  After pin 20 low pulse, 23 high: ");
    Serial.println(success);
    Serial.print("  Pin 19 state: ");
    Serial.println(digitalRead(19));
    if (digitalRead(19))
    {
        Serial.println("Shutting back down with AT command");
        tinyModem.poweroff();
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }

    Serial.println("\noutputting another 60µs low pulse again on 20");
    Serial.println("expect failure from SARA R410M (too short), success from U201");
    digitalWrite(20, LOW);
    delayMicroseconds(60);
    digitalWrite(20, HIGH);

    success = uBeeTestAT();
    Serial.print("  After pin 20 60µs low pulse, 23 high: ");
    Serial.println(success);
    Serial.print("  Pin 19 state: ");
    Serial.println(digitalRead(19));
    if (digitalRead(19))
    {
        Serial.println("Attempting to shut down with 1.5 sec low pulse on 20");
        digitalWrite(20, LOW);
        delay(1600);
        digitalWrite(20, HIGH);
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }
    if (digitalRead(19))
    {
        Serial.println("Shutting back down with AT command");
        tinyModem.poweroff();
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }

    Serial.println("\noutputting a 1 sec low pulse on 20");
    Serial.println("expect failure from SARA U201 (too long), success from R410M");
    digitalWrite(20, LOW);
    delay(1100);
    digitalWrite(20, HIGH);

    success = uBeeTestAT();
    Serial.print("  After pin 20 1 sec low pulse, 23 high: ");
    Serial.println(success);
    Serial.print("  Pin 19 state: ");
    Serial.println(digitalRead(19));
    if (digitalRead(19))
    {
        Serial.println("Shutting back down with AT command");
        tinyModem.poweroff();
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }

    Serial.println("\noutputting another 1 sec low pulse on 20");
    Serial.println("expect failure from SARA U201 (too long), success from R410M");
    digitalWrite(20, LOW);
    delay(1100);
    digitalWrite(20, HIGH);

    success = uBeeTestAT();
    Serial.print("  After pin 20 1 sec low pulse, 23 high: ");
    Serial.println(success);
    Serial.print("  Pin 19 state: ");
    Serial.println(digitalRead(19));
    if (digitalRead(19))
    {
        Serial.println("Attempting to shut down with 1.5 sec low pulse on 20");
        digitalWrite(20, LOW);
        delay(1600);
        digitalWrite(20, HIGH);
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }
    if (digitalRead(19))
    {
        Serial.println("Shutting back down with AT command");
        tinyModem.poweroff();
        Serial.print("  Shutdown time (ms): ");
        uint32_t start = millis();
        while ((millis() - start) < 15000L)
        {
            if (!digitalRead(19))
            {
                Serial.println(millis() - start);
                break;
            }
        }
    }


    // Serial.println("\nsetting 20 & 23 high, 19 input pullup");
    // digitalWrite(20, HIGH);
    // digitalWrite(23, HIGH);
    // pinMode(19, INPUT_PULLUP);
    // delay(15000L);
    //
    // success = uBeeTestAT();
    // Serial.print("  Pin 20 & 23 HIGH: ");
    // Serial.println(success);
    // Serial.print("  Pin 19 state: ");
    // Serial.println(digitalRead(19));
    // // tinyModem.poweroff();
    //
    // Serial.println("\noutputting a 1 sec low pulse on 20");
    // digitalWrite(20, LOW);
    // delay(1100);
    // digitalWrite(20, HIGH);
    // delay(15000L);
    //
    // success = uBeeTestAT();
    // Serial.print("  After pin 20 low pulse: ");
    // Serial.println(success);
    // Serial.print("  Pin 19 state: ");
    // Serial.println(digitalRead(19));
    // // tinyModem.poweroff();
    //
    // Serial.println("\noutputting a 60µs low pulse on 20");
    // digitalWrite(20, LOW);
    // delayMicroseconds(60);
    // digitalWrite(20, HIGH);
    // delay(15000L);
    //
    // success = uBeeTestAT();
    // Serial.print("  After pin 20 low pulse: ");
    // Serial.println(success);
    // Serial.print("  Pin 19 state: ");
    // Serial.println(digitalRead(19));
    // // tinyModem.poweroff();
    //
    // Serial.println("\noutputting a 1 sec low pulse on 20");
    // digitalWrite(20, LOW);
    // delay(1100);
    // digitalWrite(20, HIGH);
    // Serial.print("  Shutdown time (ms): ");
    // start = millis();
    // while ((millis() - start) < 15000L)
    // {
    //     if (!digitalRead(19))
    //     {
    //         Serial.println(millis() - start);
    //         break;
    //     }
    // }
    // tinyModem.poweroff();
}

void loop(){}
