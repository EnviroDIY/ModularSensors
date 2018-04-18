/*****************************************************************************
logging_to_EnviroDIY.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging data to an SD card and sending the data to
the EnviroDIY data portal.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// Select your modem chip, comment out all of the others
// #define TINY_GSM_MODEM_SIM800  // Select for a SIM800, SIM900, or variant thereof
// #define TINY_GSM_MODEM_A6  // Select for a AI-Thinker A6 or A7 chip
// #define TINY_GSM_MODEM_M590  // Select for a Neoway M590
// #define TINY_GSM_MODEM_U201  // Select for a U-blox U201
// #define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
#define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerEnviroDIY.h>


// ==========================================================================
//    Basic Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "logging_to_EnviroDIY.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
// Your logger's timezone.
const int8_t timeZone = -5;
// Create a new logger instance
LoggerEnviroDIY EnviroDIYLogger;


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorStats.h>

const long serialBaud = 57600;  // Baud rate for the primary serial port for debugging
const int8_t greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int8_t redLED = 9;  // Pin for the red LED (-1 if unconnected)
const int8_t buttonPin = 21;  // Pin for a button to use to enter debugging mode (-1 if unconnected)
const int8_t wakePin = A7;  // Interrupt/Alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int8_t sdCardPin = 12;  // SD Card Chip Select/Slave Select Pin (must be defined!)

// Create the processor "sensor"
const char *MFVersion = "v0.5";
ProcessorStats mayfly(MFVersion) ;


// ==========================================================================
//    Modem/Internet connection options
// ==========================================================================
HardwareSerial &ModemSerial = Serial1; // The serial port for the modem - software serial can also be used.

#if defined(TINY_GSM_MODEM_XBEE)
const int8_t modemSleepRqPin = 23;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
ModemSleepType ModemSleepMode = modem_sleep_reverse;  // How the modem is put to sleep

#elif defined(TINY_GSM_MODEM_ESP8266)
const int8_t modemSleepRqPin = 19;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = -1;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
ModemSleepType ModemSleepMode = modem_always_on;  // How the modem is put to sleep

#else
const int8_t modemSleepRqPin = 23;  // Modem SleepRq Pin (for sleep requests) (-1 if unconnected)
const int8_t modemStatusPin = 19;   // Modem Status Pin (indicates power status) (-1 if unconnected)
const int8_t modemVCCPin = -1;  // Modem power pin, if it can be turned on or off (-1 if unconnected)
ModemSleepType ModemSleepMode = modem_sleep_held;  // How the modem is put to sleep
#endif
// Use "modem_sleep_held" if the DTR pin is held HIGH to keep the modem awake, as with a Sodaq GPRSBee rev6.
// Use "modem_sleep_pulsed" if the DTR pin is pulsed high and then low to wake the modem up, as with an Adafruit Fona or Sodaq GPRSBee rev4.
// Use "modem_sleep_reverse" if the DTR pin is held LOW to keep the modem awake, as with all XBees.
// Use "modem_always_on" if you do not want the library to control the modem power and sleep or if none of the above apply.
#if defined(TINY_GSM_MODEM_ESP8266)
const long ModemBaud = 9600;  // Default for ESP8266 is 115200, but the Mayfly itself stutters above 57600
#elif defined(TINY_GSM_MODEM_SIM800)
const long ModemBaud = 9600;  // SIM800 auto-detects, but I've had trouble making it fast (19200 works)
#elif defined(TINY_GSM_MODEM_XBEE)
const long ModemBaud = 9600;  // Default for XBee is 9600, I've sped mine up to 57600
#else
const long ModemBaud = 9600;  // Modem baud rate
#endif

const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs

// Create the loggerModem instance
// A "loggerModem" is a combination of a TinyGSM Modem, a TinyGSM Client, and an on/off method
loggerModem modem;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
MaximDS3231 ds3231(1);


// ==========================================================================
//    Bosch BME280 Environmental Sensor (Temperature, Humidity, Pressure)
// ==========================================================================
#include <BoschBME280.h>
uint8_t BMEi2c_addr = 0x76;  // The BME280 can be addressed either as 0x76 or 0x77
const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
// Create a sensor object for the BME
BoschBME280 bme280(I2CPower, BMEi2c_addr);
// Create the four variable objects for the BME
Variable *bTemp = new BoschBME280_Temp(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bHumid = new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bPress = new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-efgh-1234567890ab");
Variable *bAlt = new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    MeaSpecMS5803 (Pressure, Temperature)
// ==========================================================================
#include <MeaSpecMS5803.h>
// const int8_t I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 or 0x77
const int MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;
// Create the sensor for the MS5803
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);
// Create the two variables for the MS5803
Variable *msTemp = new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");
Variable *msPress = new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
Variable *variableList[] = {
    bTemp,
    bHumid,
    bPress,
    bAlt,
    msTemp,
    msPress,
    new ProcessorStats_FreeRam(&mayfly, "12345678-abcd-1234-efgh-1234567890ab"),
    new ProcessorStats_Batt(&mayfly, "12345678-abcd-1234-efgh-1234567890ab"),
    new MaximDS3231_Temp(&ds3231, "12345678-abcd-1234-efgh-1234567890ab"),
    new Modem_RSSI(&modem, "12345678-abcd-1234-efgh-1234567890ab"),
    new Modem_SignalPercent(&modem, "12345678-abcd-1234-efgh-1234567890ab"),
    // new YOUR_variableName_HERE(&)
};
int variableCount = sizeof(variableList) / sizeof(variableList[0]);


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

    // Initialize the logger
    EnviroDIYLogger.init(sdCardPin, wakePin, variableCount, variableList,
                loggingInterval, LoggerID);
    EnviroDIYLogger.setAlertPin(greenLED);

    // Setup the logger modem
    #if defined(TINY_GSM_MODEM_ESP8266)
        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, wifiId, wifiPwd);
    #elif defined(TINY_GSM_MODEM_XBEE)
        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, wifiId, wifiPwd);
        // modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, apn);
    #else
        modem.setupModem(&ModemSerial, modemVCCPin, modemStatusPin, modemSleepRqPin, ModemSleepMode, apn);
    #endif

    // Attach the modem to the logger
    EnviroDIYLogger.attachModem(&modem);

    // Set up the connection with EnviroDIY
    EnviroDIYLogger.setToken(registrationToken);
    EnviroDIYLogger.setSamplingFeatureUUID(samplingFeature);

    // Set up the connection with DreamHost
    #ifdef DreamHostPortalRX
        EnviroDIYLogger.setDreamHostPortalRX(DreamHostPortalRX);
    #endif

    // Begin the logger
    EnviroDIYLogger.begin();

    // Check for debugging mode
    EnviroDIYLogger.checkForTestingMode(buttonPin);

    // Blink the LEDs really fast to show start-up is done
    greenredflash(6, 25);
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (EnviroDIYLogger.checkInterval())
    {
        // Print a line to show new reading
        Serial.print(F("------------------------------------------\n"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(greenLED, HIGH);

        // Turn on the modem to let it start searching for the network
        EnviroDIYLogger._logModem->powerUp();
        EnviroDIYLogger._logModem->wake();

        // Send power to all of the sensors
        Serial.print(F("    Powering sensors...\n"));
        EnviroDIYLogger.sensorsPowerUp();
        // Wake up all of the sensors
        Serial.print(F("    Waking sensors...\n"));
        EnviroDIYLogger.sensorsWake();
        // Update the values from all attached sensors
        Serial.print(F("  Updating sensor values...\n"));
        EnviroDIYLogger.updateAllSensors();
        // Put sensors to sleep
        Serial.print(F("  Putting sensors back to sleep...\n"));
        EnviroDIYLogger.sensorsSleep();
        // Cut sensor power
        Serial.print(F("  Cutting sensor power...\n"));
        EnviroDIYLogger.sensorsPowerDown();

        // Generate a csv with the sensor-recorded variables
        String csvToGo = EnviroDIYLogger.generateSensorDataCSV();

        // Generate a json with the sensor-recorded variables
        String jsonToGo = EnviroDIYLogger.generateSensorDataJSON();

        // Calculate the total water pressure
        float waterPressure = msPress->getValue() - bPress->getValue();

        // Add the water pressure to the csv string
        csvToGo += ",";
        csvToGo += String(waterPressure);

        // Add the total water pressure to the raw json
        // TODO!!



        // Connect to the network
        if (EnviroDIYLogger._logModem->connectInternet())
        {
            // Post the data to the WebSDL
            EnviroDIYLogger.postDataEnviroDIY(EnviroDIYLogger.generateEnviroDIYPostRequest(jsonToGo));

            // Disconnect from the network
            EnviroDIYLogger._logModem->disconnectInternet();
        }
        // Turn the modem off
        EnviroDIYLogger._logModem->off();

        // Create a csv data record and save it to the log file
        EnviroDIYLogger.logToSD(csvToGo);

        // Turn off the LED
        digitalWrite(greenLED, LOW);
        // Print a line to show reading ended
        Serial.print(F("------------------------------------------\n\n"));
    }

    // Sleep
    EnviroDIYLogger.systemSleep();
}
