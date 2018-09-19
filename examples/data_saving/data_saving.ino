/*****************************************************************************
data_saving.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data to an SD card and sending only a
portion of that data to the EnviroDIY data portal.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerEnviroDIY.h>


// ==========================================================================
//    Data Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "data_saving.ino";
// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -5;


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorStats.h>

const long serialBaud = 115200;  // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int8_t redLED = 9;  // Pin for the red LED (-1 if unconnected)
const int8_t buttonPin = 21;  // Pin for a button to use to enter debugging mode (-1 if unconnected)
const int8_t wakePin = A7;  // Interrupt/Alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;  // SD Card Chip Select/Slave Select Pin (must be defined!)

// Create and return the processor "sensor"
const char *MFVersion = "v0.5b";
ProcessorStats mayfly(MFVersion);
// Create the battery voltage and free RAM variable objects for the Y504 and return variable-type pointers to them
Variable *mayflyBatt = new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");
Variable *mayflyRAM = new ProcessorStats_FreeRam(&mayfly, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Modem/Internet connection options
// ==========================================================================

// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_A6  // Select for a AI-Thinker A6 or A7 chip
// #define TINY_GSM_MODEM_M590  // Select for a Neoway M590
// #define TINY_GSM_MODEM_UBLOX  // Select for most u-blox cellular modems
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// Include TinyGSM for the modem
// This include must be included below the define of the modem name!
#include <TinyGsmClient.h>

 // Set the serial port for the modem - software serial can also be used.
HardwareSerial &ModemSerial = Serial1;

// Create a variable for the modem baud rate - this will be used in the begin function for the port
const long ModemBaud = 9600;

// Create a new TinyGSM modem to run on that serial port and return a pointer to it
TinyGsm *tinyModem = new TinyGsm(ModemSerial);

// Create a new TCP client on that modem and return a pointer to it
TinyGsmClient *tinyClient = new TinyGsmClient(*tinyModem);

// Describe the physical pin connection of your modem to your board
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
const int8_t modemSleepRqPin = 23;  // Modem Sleep Request Pin (-1 if unconnected)
const int8_t modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)

// And create a pointer to the method to be used to turn the modem on and off
// The on/off method needs the pins and to know whether the sleep request is a HIGH or LOW value
// Use heldOnOff if the sleep request pin is set and kept to keep the modem awake
// Use pulsedOnOff if the sleep request pin is quickly switched to wake the modem up
// The Sodaq GPRSBee rev6 uses heldOnOff with the pin held high
// All Digi XBee's uses heldOnOff with the pin held low
// The Adafruit Fona and Sodaq GPRSBee rev4 use a low pulsedOnOff
ModemOnOff *modemOnOff = new heldOnOff(modemVCCPin, modemSleepRqPin, modemStatusPin, HIGH);
// ModemOnOff *modemOnOff = new heldOnOff(modemVCCPin, modemSleepRqPin, modemStatusPin, LOW);
// ModemOnOff *modemOnOff = new pulsedOnOff(modemVCCPin, modemSleepRqPin, modemStatusPin, HIGH);
// ModemOnOff *modemOnOff = new pulsedOnOff(modemVCCPin, modemSleepRqPin, modemStatusPin, LOW);

// And we still need the connection information for the network
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a Client, and an on/off method
loggerModem modem(tinyModem, tinyClient, modemOnOff, wifiId, wifiPwd);
// loggerModem modem(tinyModem, tinyClient, modemOnOff, apn);

// Create the RSSI and signal strength variable objects for the modem and return
// variable-type pointers to them
Variable *modemRSSI = new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab");
Variable *modemSinalPct = new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
// Create and return the DS3231 sensor object
MaximDS3231 ds3231(1);
// Create the temperature variable object for the DS3231
Variable *ds3231Temp = new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab");


// Set up a serial port for modbus communication - in this case, using AltSoftSerial
#include <AltSoftSerial.h>
AltSoftSerial modbusSerial;


// ==========================================================================
//    Yosemitech Y504 Dissolved Oxygen Sensor
// ==========================================================================
#include <YosemitechY504.h>
byte y504modbusAddress = 0x04;  // The modbus address of the Y504
const int8_t modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y504NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Yosemitech Y504 sensor object
YosemitechY504 y504(y504modbusAddress, modbusSerial, modbusPower, max485EnablePin, y504NumberReadings);
// Create the dissolved oxygen percent, dissolved oxygen concentration, and
// temperature variable objects for the Y504 and return variable-type
// pointers to them
Variable *y504DOpct = new YosemitechY504_DOpct(&y504, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y504DOmgL = new YosemitechY504_DOmgL(&y504, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y504Temp = new YosemitechY504_Temp(&y504, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y511 Turbidity Sensor with Wiper
// ==========================================================================
#include <YosemitechY511.h>
byte y511modbusAddress = 0x1A;  // The modbus address of the Y511
// const int8_t modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y511NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y511-A Turbidity sensor object
YosemitechY511 y511(y511modbusAddress, modbusSerial, modbusPower, max485EnablePin, y511NumberReadings);
// Create the turbidity and temperature variable objects for the Y511 and return variable-type pointers to them
Variable *y511Turb = new YosemitechY511_Turbidity(&y511, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y511Temp = new YosemitechY511_Temp(&y511, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y514 Chlorophyll Sensor
// ==========================================================================
#include <YosemitechY514.h>
byte y514modbusAddress = 0x14;  // The modbus address of the Y514
// const int8_t modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y514NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y514 chlorophyll sensor object
YosemitechY514 y514(y514modbusAddress, modbusSerial, modbusPower, max485EnablePin, y514NumberReadings);
// Create the chlorophyll concentration and temperature variable objects for the Y514 and return variable-type pointers to them
Variable *y514Chloro = new YosemitechY514_Chlorophyll(&y514, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y514Temp = new YosemitechY514_Temp(&y514, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Yosemitech Y520 Conductivity Sensor
// ==========================================================================
#include <YosemitechY520.h>
byte y520modbusAddress = 0x20;  // The modbus address of the Y520
// const int8_t modbusPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// const int8_t max485EnablePin = -1;  // Pin connected to the RE/DE on the 485 chip (-1 if unconnected)
const uint8_t y520NumberReadings = 5;  // The manufacturer recommends averaging 10 readings, but we take 5 to minimize power consumption
// Create and return the Y520 conductivity sensor object
YosemitechY520 y520(y520modbusAddress, modbusSerial, modbusPower, max485EnablePin, y520NumberReadings);
// Create the specific conductance and temperature variable objects for the Y520 and return variable-type pointers to them
Variable *y520Cond = new YosemitechY520_Cond(&y520, "12345678-abcd-1234-efgh-1234567890ab");
Variable *y520Temp = new YosemitechY520_Temp(&y520, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================

// Put all of the variable pointers into an Array
// NOTE:  Since we've created all of the variable pointers above, we can just
// reference them by name here.
Variable *variableList_complete[] = {
    mayflyBatt,
    mayflyRAM,
    ds3231Temp,
    y504DOpct,
    y504DOmgL,
    y504Temp,
    y511Turb,
    y511Temp,
    y514Chloro,
    y514Temp,
    y520Cond,
    y520Temp,
    modemRSSI,
    modemSinalPct
};
// Count up the number of pointers in the array
int variableCount_complete = sizeof(variableList_complete) / sizeof(variableList_complete[0]);
// Create the VariableArray object
VariableArray arrayComplete(variableCount_complete, variableList_complete);
// Create the new logger instance
LoggerEnviroDIY loggerComplete(LoggerID, loggingInterval, sdCardPin, wakePin, &arrayComplete);


// ==========================================================================
//    The array that contains all variables to have their values sent out over the internet
// ==========================================================================

// Put all of the variable pointers into an Array
// NOTE:  Since we've created all of the variable pointers above, we can just
// reference them by name here.
Variable *variableList_toGo[] = {
    y504DOmgL,
    y504Temp,
    y511Turb,
    y514Chloro,
    y520Cond,
    modemRSSI
};
// Count up the number of pointers in the array
int variableCount_toGo = sizeof(variableList_toGo) / sizeof(variableList_toGo[0]);
// Create the VariableArray object
VariableArray arrayToGo(variableCount_toGo, variableList_toGo);
// Create the new logger instance
LoggerEnviroDIY loggerToGo(LoggerID, loggingInterval,sdCardPin, wakePin, &arrayToGo);


// ==========================================================================
// Device registration and sampling feature information
//   This should be obtained after registration at http://data.envirodiy.org
// ==========================================================================
const char *registrationToken = "12345678-abcd-1234-efgh-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-efgh-1234567890ab";     // Sampling feature UUID


// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(int numFlash = 4, int rate = 75)
{
  for (int i = 0; i < numFlash; i++) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    delay(rate);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    delay(rate);
  }
  digitalWrite(redLED, LOW);
}


// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    // Start the primary serial connection
    Serial.begin(serialBaud);

    // Start the serial connection with the modem
    ModemSerial.begin(ModemBaud);

    // Start the stream for the modbus sensors
    modbusSerial.begin(9600);

    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Attach the same modem to both loggers
    // It is only needed for the logger that will be sending out data, but
    // attaching it to both allows either logger to control NIST synchronization
    loggerComplete.attachModem(modem);
    loggerToGo.attachModem(modem);
    loggerComplete.setTestingModePin(buttonPin);
    // There is no reason to call the setAlertPin() function, because we have to
    // write the loop on our own.

    // Set up the connection information with EnviroDIY for both loggers
    // Doing this for both loggers ensures that the header of the csv will have the tokens in it
    loggerComplete.setToken(registrationToken);
    loggerComplete.setSamplingFeatureUUID(samplingFeature);
    loggerToGo.setToken(registrationToken);
    loggerToGo.setSamplingFeatureUUID(samplingFeature);

    // Because we've given it a modem and it knows all of the tokens, we can
    // just "begin" the complete logger to set up the datafile, clock, sleep,
    // and all of the sensors.  We don't need to bother with the "begin" for the
    // other logger because it has the same processor and clock.
    loggerComplete.begin();
}


// ==========================================================================
// Main loop function
// ==========================================================================

// Because of the way the sleep mode is set up, the processor will wake up
// and start the loop every minute exactly on the minute.
void loop()
{
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (loggerComplete.checkInterval())
    {
        // Print a line to show new reading
        Serial.print(F("------------------------------------------\n"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(greenLED, HIGH);

        // Turn on the modem to let it start searching for the network
        modem.modemPowerUp();

        // Start the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.begin(9600);

        // Send power to all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Powering sensors...\n"));
        arrayComplete.sensorsPowerUp();
        // Wake up all of the sensors (do this directly on the VariableArray)
        Serial.print(F("Waking sensors...\n"));
        arrayComplete.sensorsWake();
        // Update the values from all attached sensors (do this directly on the VariableArray)
        Serial.print(F("Updating sensor values...\n"));
        arrayComplete.updateAllSensors();
        // Put sensors to sleep (do this directly on the VariableArray)
        Serial.print(F("Putting sensors back to sleep...\n"));
        arrayComplete.sensorsSleep();
        // Cut sensor power (do this directly on the VariableArray)
        Serial.print(F("Cutting sensor power...\n"));
        arrayComplete.sensorsPowerDown();

        // End the stream for the modbus sensors
        // Because RS485 adapters tend to "steal" current from the data pins
        // we will explicitly start and end the serial connection in the loop.
        modbusSerial.end();

        // Stream the variable results from the complete set of variables to
        // the SD card
        loggerComplete.logToSD();

        // Connect to the network
        Serial.print(F("Connecting to the internet...\n"));
        if (modem.connectInternet())
        {
            // Post the data to the WebSDL
            loggerToGo.postDataEnviroDIY();

            // Disconnect from the network
            modem.disconnectInternet();
        }
        // Turn the modem off
        modem.modemPowerDown();

        // Turn off the LED
        digitalWrite(greenLED, LOW);
        // Print a line to show reading ended
        Serial.print(F("------------------------------------------\n\n"));
    }

    // Check if it was instead the testing interrupt that woke us up
    // Want to enter the testing mode for the "complete" logger so we can see
    // the data from _ALL_ sensors
    // NOTE:  The testingISR attached to the button at the end of the "setup()"
    // function turns on the startTesting flag.  So we know if that flag is set
    // then we want to run the testing mode function.
    if (Logger::startTesting) loggerComplete.testingMode();

    // Once a day, at midnight, sync the clock
    if (Logger::markedEpochTime % 86400 == 0)
    {
        // Turn on the modem
        modem.modemPowerUp();
        // Connect to the network
        if (modem.connectInternet())
        {
            // Synchronize the RTC (the loggers have the same clock, pick one)
            loggerComplete.syncRTClock(modem.getNISTTime());
            // Disconnect from the network
            modem.disconnectInternet();
        }
        // Turn off the modem
        modem.modemPowerDown();
    }

    // Call the processor sleep
    // Only need to do this for one of the loggers
    loggerComplete.systemSleep();
}
