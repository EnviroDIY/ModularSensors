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
#define TINY_GSM_MODEM_ESP8266  // Select for an ESP8266 using the DEFAULT AT COMMAND FIRMWARE
// #define TINY_GSM_MODEM_XBEE  // Select for Digi brand WiFi or Cellular XBee's

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
const char *sketchName = "baro_rho_correction.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 1;
// Your logger's timezone.
const int8_t timeZone = -5;
// Create a new logger instance
LoggerEnviroDIY EnviroDIYLogger;


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
const long ModemBaud = 57600;  // Default for ESP8266 is 115200, but the Mayfly itself stutters above 57600
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
uint8_t BMEi2c_addr = 0x77;
// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove default)
// Either can be physically mofidied for the other address
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
const uint8_t MS5803i2c_addr = 0x76;  // The MS5803 can be addressed either as 0x76 (default) or 0x77
const int MS5803maxPressure = 14;  // The maximum pressure measurable by the specific MS5803 model
const uint8_t MS5803ReadingsToAvg = 1;
// Create the sensor for the MS5803
MeaSpecMS5803 ms5803(I2CPower, MS5803i2c_addr, MS5803maxPressure, MS5803ReadingsToAvg);
// Create the two variables for the MS5803
Variable *msTemp = new MeaSpecMS5803_Temp(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");
Variable *msPress = new MeaSpecMS5803_Pressure(&ms5803, "12345678-abcd-1234-efgh-1234567890ab");


// ==========================================================================
//    Maxim DS18 One Wire Temperature Sensor
// ==========================================================================
#include <MaximDS18.h>
// OneWire Address [array of 8 hex characters]
// DeviceAddress OneWireAddress1 = {0x28, 0xFF, 0xBD, 0xBA, 0x81, 0x16, 0x03, 0x0C};
// DeviceAddress OneWireAddress2 = {0x28, 0xFF, 0x57, 0x90, 0x82, 0x16, 0x04, 0x67};
// DeviceAddress OneWireAddress3 = {0x28, 0xFF, 0x74, 0x2B, 0x82, 0x16, 0x03, 0x57};
// DeviceAddress OneWireAddress4 = {0x28, 0xFF, 0xB6, 0x6E, 0x84, 0x16, 0x05, 0x9B};
// DeviceAddress OneWireAddress5 = {0x28, 0xFF, 0x3B, 0x07, 0x82, 0x16, 0x03, 0xB3};
const int8_t OneWireBus = 4;  // Pin attached to the OneWire Bus (-1 if unconnected)
const int8_t OneWirePower = 22;  // Pin to switch power on and off (-1 if unconnected)
// MaximDS18 ds18_1(OneWireAddress1, OneWirePower, OneWireBus);
// MaximDS18 ds18_2(OneWireAddress2, OneWirePower, OneWireBus);
// MaximDS18 ds18_3(OneWireAddress3, OneWirePower, OneWireBus);
// MaximDS18 ds18_4(OneWireAddress4, OneWirePower, OneWireBus);
// MaximDS18 ds18_5(OneWireAddress5, OneWirePower, OneWireBus);
MaximDS18 ds18_u(OneWirePower, OneWireBus);


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
    new MaximDS18_Temp(&ds18_u, "12345678-abcd-1234-efgh-1234567890ab"),
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
// Extra information and tokens for calculted variables
// ==========================================================================
const char *waterPressureUUID = "12345678-abcd-1234-efgh-1234567890ab";
const char *waterPresureVarName = "pressureGauge";
const char *waterPresureVarUnit = "Millibar";
const char *waterPresureVarCode = "CorrectedPressure";
const char *waterDepthUUID = "12345678-abcd-1234-efgh-1234567890ab";
const char *waterDepthVarName = "waterDepth";
const char *waterDepthVarUnit = "millimeter";
const char *waterDepthVarCode = "CalcDepth";
const char *rhoDepthUUID = "12345678-abcd-1234-efgh-1234567890ab";
const char *rhoDepthVarName = "rhoDepth";
const char *rhoDepthVarUnit = "millimeter";
const char *rhoDepthVarCode = "DensityDepth";


// ==========================================================================
// Initialize variables for density correction
// ==========================================================================
float waterPressureBar = -9999.0;
float waterPressurePa  = -9999.0;
float waterTempertureC = -9999.0;
float waterDensity = -9999.0;
float rhoDepth = -9999.0;
const float gravitationalConstant = 9.80665; // m/s2, meters per second squared


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

    // Begin the real time clock
    rtc.begin();
    delay(100);

    // Print out the current time
    Serial.print(F("Current RTC time is: "));
    Serial.println(Logger::formatDateTime_ISO8601(Logger::getNowEpoch()));

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

    // Now that the modem is attached, use it to sync the clock with NIST
    Serial.print(F("Attempting to synchronize RTC with NIST\n"));
    // Turn on the modem
    EnviroDIYLogger._logModem->modemPowerUp();
    // Connect to the network
    if (EnviroDIYLogger._logModem->connectInternet())
    {
        EnviroDIYLogger.syncRTClock(EnviroDIYLogger._logModem->getNISTTime());
        // Disconnect from the network
        EnviroDIYLogger._logModem->disconnectInternet();
    }
    // Turn off the modem
    EnviroDIYLogger._logModem->modemPowerDown();

    // Enter the tokens for the connection with EnviroDIY
    EnviroDIYLogger.setToken(registrationToken);
    EnviroDIYLogger.setSamplingFeatureUUID(samplingFeature);

    // Set up the sensors
    EnviroDIYLogger.setupSensors();

    // Generate a logger file name from the LoggerID and the date/time on the RTC
    // This will start a new file every time the logger is reset
    EnviroDIYLogger.setFileName();

    // Set up the log file and add a header to it with all of the information
    // for the sensors and variables in the variable array
    EnviroDIYLogger.setupLogFile();

    // Now we're going to add another set of rows to the file header with
    // just the information for the calculated variable
    // We could make a "prettier" header by adding commas and values onto the
    // end of each line of the multi-line header generated by the
    // generateFileHeader() function, but that would take a bunch of really
    // annoying string subscripting and re-combining (or just rewriting that
    // whole generateFileHeader function, which is already a long annoying
    // string concatenation function) so instead we're just creating another set
    // of header rows for the csv with no values in the first spaces where
    // header values exist and then starting the new header information shifted
    // over by that many columns/commas
    String extraHeaderSpacer = "";
    for (uint8_t i=0; i <= variableCount; i++) extraHeaderSpacer += ',';
    // the first row is the variable UUID's, we have two:
    // for calculated pressure and calculated depth
    EnviroDIYLogger.logToSD(extraHeaderSpacer + String(waterPressureUUID) +
                            F(", ") + String(waterDepthUUID) + F(", ") +
                            String(rhoDepthUUID));
    // the next row is the sensor name, which is blank, so we'll put in a blank row
    EnviroDIYLogger.logToSD(extraHeaderSpacer + F(", "));
    // the third row is the variable name
    EnviroDIYLogger.logToSD(extraHeaderSpacer + String(waterPresureVarName) +
                            F(", ") + String(waterDepthVarName) + F(", ") +
                            String(rhoDepthVarName));
    // the fourth row is the variable unit name
    EnviroDIYLogger.logToSD(extraHeaderSpacer + String(waterPresureVarUnit) +
                            F(", ") + String(waterDepthVarUnit) + F(", ") +
                            String(rhoDepthVarUnit));
    // the last row is the variable code
    EnviroDIYLogger.logToSD(extraHeaderSpacer + String(waterPresureVarCode) +
                            F(", ") + String(waterDepthVarCode) + F(", ") +
                            String(rhoDepthVarCode));

    // Setup the logger sleep mode
    EnviroDIYLogger.setupSleep();

    // Hold up for 10-seconds to allow immediate entry into sensor testing mode
    // EnviroDIYLogger.checkForTestingMode(buttonPin);

    //  Set up an interrupt on a pin to enter sensor testing mode at any time
    pinMode(buttonPin, INPUT_PULLUP);
    enableInterrupt(buttonPin, Logger::testingISR, CHANGE);
    Serial.print(F("Push button on pin "));
    Serial.print(buttonPin);
    Serial.println(F(" at any time to enter sensor testing mode."));

    Serial.print(F("Logger setup finished!\n"));
    Serial.print(F("------------------------------------------\n\n"));

    // Blink the LEDs really fast to show start-up is done
    greenredflash(6, 25);
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
    if (EnviroDIYLogger.checkInterval())
    {
        // Print a line to show new reading
        Serial.print(F("------------------------------------------\n"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(greenLED, HIGH);

        // Turn on the modem to let it start searching for the network
        EnviroDIYLogger._logModem->modemPowerUp();

        // Send power to all of the sensors
        Serial.print(F("Powering sensors...\n"));
        EnviroDIYLogger.sensorsPowerUp();
        // Wake up all of the sensors
        Serial.print(F("Waking sensors...\n"));
        EnviroDIYLogger.sensorsWake();
        // Update the values from all attached sensors
        Serial.print(F("Updating sensor values...\n"));
        EnviroDIYLogger.updateAllSensors();
        // Put sensors to sleep
        Serial.print(F("Putting sensors back to sleep...\n"));
        EnviroDIYLogger.sensorsSleep();
        // Cut sensor power
        Serial.print(F("Cutting sensor power...\n"));
        EnviroDIYLogger.sensorsPowerDown();

        // Generate a csv with the sensor-recorded variables
        String csvRaw = EnviroDIYLogger.generateSensorDataCSV();

        // Generate a json with the sensor-recorded variables
        String jsonRaw = EnviroDIYLogger.generateSensorDataJSON();

        // Calculate the water pressure after barometic correction
        // As long as the seensor updates took less than two minutes, these
        // getValue() functions will return the values from the last update
        // that happened in the updateAllSensors() function
        // The MS5803 reports pressure in millibar, the BME280 in pascal
        // 1 pascal = 0.01 mbar
        // This calculation gives a final result in mbar
        Serial.print(F("Calculating water pressure...\n"));
        float waterPressure = msPress->getValue() - (bPress->getValue())*0.01;
        float waterTempertureC = msTemp->getValue();
        // Now calculate the depth of the water
        // For this, we're using the conversion between mbar and mm pure water at 4°C
        // This calculation gives a final result in mm of water
        float waterDepth = waterPressure*10.1972;
        // Converting water depth for the changes of pressure with depth
        // First get water pressure in Pa for the calculation: 1 mbar = 100 Pa
        waterPressurePa = 100 * waterPressure;
        // Water density (kg/m3) from equation 6 from JonesHarris1992-NIST-DensityWater.pdf
        waterDensity =  + 999.84847
                        + 6.337563e-2 * waterTempertureC
                        - 8.523829e-3 * pow(waterTempertureC,2)
                        + 6.943248e-5 * pow(waterTempertureC,3)
                        - 3.821216e-7 * pow(waterTempertureC,4)
                        ;
        // This calculation gives a final result in mm of water
        rhoDepth = 1000 * waterPressurePa/(waterDensity * gravitationalConstant);  // from P = rho * g * h


        // Add the water pressure and depth to the csv string
        String csvToGo = csvRaw + ",";
        csvToGo += String(waterPressure);
        csvToGo += ",";
        csvToGo += String(waterDepth);
        csvToGo += ",";
        csvToGo += String(rhoDepth);

        // Add the total water pressure to the raw json
        // Figure out how long the original json is
        int jsonLength = jsonRaw.length();
        // Crop off the last ' }' from the json
        String jsonToGo = jsonRaw.substring(0,jsonLength-1);
        // add the UUID for the water pressure
        jsonToGo += F(", \"");
        jsonToGo += String(waterPressureUUID);
        jsonToGo += F("\": ");
        // add the water pressure value
        jsonToGo += String(waterPressure);
        // add the UUID for the water depth
        jsonToGo += F(", \"");
        jsonToGo += String(waterDepthUUID);
        jsonToGo += F("\": ");
        // add the water depth value
        jsonToGo += String(waterDepth);
        // add the UUID for the density corrected water depth
        jsonToGo += F(", \"");
        jsonToGo += String(rhoDepthUUID);
        jsonToGo += F("\": ");
        // add the density corrected water depth value
        jsonToGo += String(rhoDepth);
        // re-add the last '}'
        jsonToGo += F("}");

        // Connect to the network
        Serial.print(F("Connecting to the internet...\n"));
        if (EnviroDIYLogger._logModem->connectInternet())
        {
            // Post the data to the WebSDL
            EnviroDIYLogger.postDataEnviroDIY(jsonToGo);

            // Disconnect from the network
            EnviroDIYLogger._logModem->disconnectInternet();
        }
        // Turn the modem off
        EnviroDIYLogger._logModem->modemPowerDown();

        // Create a csv data record and save it to the log file
        EnviroDIYLogger.logToSD(csvToGo);

        // Turn off the LED
        digitalWrite(greenLED, LOW);
        // Print a line to show reading ended
        Serial.print(F("------------------------------------------\n\n"));
    }

    // Check if it was instead the testing interrupt that woke us up
    // NOTE:  This testing mode will *NOT* show the values from any of the extra
    // calculated variables added in the loop.  It will only show the variables
    // listed in the variableList which is fed to the EnviroDIYLogger.
    if (Logger::startTesting) EnviroDIYLogger.testingMode();

    // Sleep
    EnviroDIYLogger.systemSleep();
}
